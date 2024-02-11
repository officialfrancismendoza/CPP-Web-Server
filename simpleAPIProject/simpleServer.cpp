#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

// Custom Request Handler 
class Handler : public oatpp::web::server::HttpRequestHandler {
public: 
    /*
        Shared Ptr
            - Type of smart pointer
            - Automatically deallocates when it needs to (once there is no more copies of the pointer)
            - Can make as many copies of it as you want; will only delete once last copy has been destroyed
    */
    // Just made a general handler, for now only having one purpose
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
       return ResponseFactory::createResponse(Status::CODE_200, "Successful Response. Hello World!"); 
    }
};

void run() {
    // Make router for incoming HTTP requests
    auto router = oatpp::web::server::HttpRouter::createShared();

    // Create a route for GET "/hello"
    /* 
        NOTE: Here, we are making a shared smart ptr that references a dynamic instance of Handler
              This is functionally the same as allocating new memory for it if we called "new Handler"
    */
    router->route("GET", "/hello", std::make_shared<Handler>());

    // Create HTTP connection handler with router
    auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

    // Create TCP connection provider
    auto connectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared({"localhost", 8000, oatpp::network::Address::IP_4});

    // Create server which takes provided TCP connections and passes them to HTTP connection handler
    oatpp::network::Server server(connectionProvider, connectionHandler);

    // Print info about server port
    OATPP_LOGI("MyApp", "Server running on port %s", connectionProvider->getProperty("port").getData());

    // Run Server
    server.run();
}

int main() {
    // Initialize the Oatpp environment
    oatpp::base::Environment::init();

    // Run Application
    run();

    // Destroy Oatpp environment
    oatpp::base::Environment::destroy();

    return 0;
}