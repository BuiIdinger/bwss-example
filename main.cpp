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

    std::string message = "hello";
    connection->send(message, bwss::OpCodes::TEXT_FRAME);
    connection->send(message, bwss::OpCodes::BINARY_FRAME);
  };
  
  // Invoked when a message is received
  bwss::onMessage = [](bwss::Connection* connection, std::string message) {
    std::unique_lock<std::mutex> connectionMutex(connection->mutex);
    std::cout << "Got message from " << connection->socket << ", message is " << message << "\n";
    connectionMutex.unlock();

    // Echo message back
    connection->send(message, bwss::OpCodes::TEXT_FRAME);
  };
  
  // Called when a connection was closed, connections allocated memory will be deleted
  // after the this lambda has finished, so you can still send messages, access user data here
  bwss::onClose = [](bwss::Connection* connection) {
    std::unique_lock<std::mutex> connectionGuard(connection->mutex);
    std::cout << "Connection " << connection->socket << " was closed" << "\n";
    connectionGuard.unlock();
  };

  // Start server, this will block the main thread
  bwss::run();

  return 0;
}
