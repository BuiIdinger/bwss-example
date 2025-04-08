// 
//
//
// ©

#include <bwss/bwss.h>
#include <iostream>
#include <string>

int main() {
  bwss::onOpen = [](bwss::Connection* connection) {
    // Topics are a good way to publish messages to many connections, for example, you could
    // subscribe a connection to a topic called 'game-15956', you can publish a message (so
    // sending a message) to all of those connections that are subscribed to that topic, any
    // connections that arn't subscribed to that 'game-15956' will not receive the publish message

    // You can use topics as a way to sort out connections for a game, lets say you have lobbys
    // and each lobby has a id, you could subscribe connections that want to join that lobby by
    // by using the ->subscribe("lobby"). Now you can allow a global chat in that lobby by using
    // bwss::publish("Hello", "lobby");, and every connection that is subscribed to that lobby topic
    // will get that connection

    // Subscribe this connection to a topic
    connection->subscribe("game-15956");

    // We can check if an connection subscribed to a topic like so
    if (connection->isSubscribed("game-15956")) {
      std::cout << "Connection is subscribed to game-15956 topic" << "\n";
    } else {
      std::cout << "Connection isnt subscribed to game-15956 topic" << "\n";
    }

    // In the future, this will be added
    // const std::vector<std::string> subscribedTopics = connection->getSubscribedTopics();

    // And possibly see a list of topics that exist
    // const std::vector<std::string> allTopics = bwss::getTopics();

    // Publish to a topic, this will send a message to every connection that is subscribed to the requested topic
    std::string message = "hello";
    bwss::publish(message, "test");
  };
  
  // Invoked when a message is received
  bwss::onMessage = [](bwss::Connection* connection, std::string message) {

  };

  bwss::onClose = [](bwss::Connection* connection) {

  };

  bwss::run();

  return BWSS_SUCCESS;
}
