#include <bwss/bwss.h>
#include <iostream>
#include <memory>

// Socket data, pretty self-explanatory
struct SocketData {
  std::string name;
  int32_t age;
  bool open = true;
};

int main() {
  // Open lambda, is called when a connection has had its tcp 
  // handshake completed succsssfully and the websocket upgrade
  // has been completed
  bwss::onOpen = [](bwss::Connection* connection) {
    std::unique_lock<std::mutex> connectionGuard(connection->mutex, std::defer_lock);
    std::unique_lock<std::mutex> userDataGuard(connection->userDataMutex, std::defer_lock);

    connectionGuard.lock();
    std::cout << "New connection from " << connection->socket << "." << "\n";
    connectionGuard.unlock();

    //
    // USER DATA
    //

    // I suggest you load needed user data, and your user data class here
    // dont do it in your message lambda or you'll just overide the socket data

    // When accessing user data use the internal mutex to protect userdata

    SocketData data;
 
    userDataGuard.lock();
    connection->userData = std::make_shared<SocketData>(data);
    userDataGuard.unlock();

    // Every library function is thread-safe, just make sure to not hold onto the
    // mutex when calling any functions from this library, otherwise a deadlock will
    // occur
    std::shared_ptr<SocketData> userData = connection->getUserData<SocketData>();

    // Setting user data, use the internal mutex to protect the user data
    userDataGuard.lock();
    userData->age = 21;
    userData->name = "buildinger";
    userDataGuard.unlock();

    //
    // SENDING MESSAGES
    //
    std::string message = "hello";

    // Send a message like so, pass in either text frame or binary code op-code
    // dont use the other opcodes, if passing in binary op code, the library will
    // automically convert the string into binary, so don't convert the message into
    // binary before passing it into the send function
    connection->send(message, bwss::OpCodes::TEXT_FRAME);
    
    // Send a binary message
    connection->send(message, bwss::OpCodes::BINARY_FRAME);

    //
    // TOPICS
    //

    // Topics are a good way to publish messages to many connections, for example, you could
    // subscribe a connection to a topic called 'game-15956', you can publish a message (so
    // sending a message) to all of those connections that are subscribed to that topic, any
    // connections that arn't subscribed to that 'game-15956' will not receive the publish message

    // Subscribe this connection to a topic
    connection->subscribe("game-15956");

    // We can check if an connection subscribed to a topic like so
    if (connection->isSubscribed("game-15956")) {
      std::cout << "Connection is subscribed to game-15956 topic" << "\n";
    } else {
      std::cout << "Connection isnt subscribed to game-15956 topic" << "\n";
    }

    //
    // CLOSING
    //

    // Close a connection like this, you should probably give a close code, and a reason why
    // you dont have, its recommended
    // connection->close(bwss::CloseCodes::NORMAL_CLOSURE, "Random close reason");

    //
    // PING AND PONG
    //

    // I highly recommended you dont control ping and pongs by your self as the library does
    // its self automatically

    // Last time since a keep-alive request succeededs
    connectionGuard.lock();
    std::cout << connection->lastKeepAliveRequest << "\n";
    connectionGuard.unlock();

    // Triggers a ping frame, will send to the connection, this will automatically update the
    // last keep alive reqeust
    connection->ping();

    // Triggers a pong frame
    connection->pong();

    // Publish to a topic, this will send a message to every connection that is subscribed to the requested topic
    bwss::publish(message, "test");
  };
  
  // Invoked when a message is received
  bwss::onMessage = [](bwss::Connection* connection, std::string message) {
    // std::unique_lock<std::mutex> connectionMutex(connection->mutex);
    std::cout << "Got message from " << connection->socket << ", message is " << message << "\n";
   // connectionMutex.unlock();

    // Echo message back
    connection->send(message, bwss::OpCodes::TEXT_FRAME);

    if (message.find("ping") != std::string::npos) {
      connection->ping();
    }

    if (message.find("close") != std::string::npos) {
      connection->close(bwss::CloseCodes::VIOLATED_POLICY, "Yuh, I just can do this.");
      delete connection; // Make sure to delete connection
      return;
    }

    // Acessing user data
    std::shared_ptr<SocketData> userData = connection->getUserData<SocketData>();
    std::unique_lock<std::mutex> userDataGuard(connection->userDataMutex);
    userDataGuard.unlock();
  };
  
  // Called when a connection was closed, connections allocated memory will be deleted
  // after the this lambda has finished, so you can still send messages, access user data here
  bwss::onClose = [](bwss::Connection* connection) {
    std::unique_lock<std::mutex> connectionGuard(connection->mutex);
    std::cout << "Connection " << connection->socket << " was closed" << "\n";
    connectionGuard.unlock();

    // Get ptr to user data
    std::shared_ptr<SocketData> userData = connection->getUserData<SocketData>();
    std::unique_lock<std::mutex> userDataGuard(connection->userDataMutex);
    userData->open = false;
    userDataGuard.unlock();
  };

  // Start server, this will block the main thread
  bwss::run();

  return 0;
}
