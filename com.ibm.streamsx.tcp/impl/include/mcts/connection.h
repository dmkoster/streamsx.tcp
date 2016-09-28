/*
#######################################################################
# Copyright (C)2012, International Business Machines Corporation and
# others. All Rights Reserved.
#######################################################################
*/

#ifndef MULTI_CONNECTION_TCP_SERVER_CONNECTION
#define MULTI_CONNECTION_TCP_SERVER_CONNECTION

#include "mcts/data_item.h"
#include "mcts/data_handler.h"
#include "mcts/info_handler.h"
#include "mcts/socket.h"
#include "mcts/tls_socket.h"

#include <streams_boost/asio.hpp>
#include <streams_boost/array.hpp>
#include <streams_boost/shared_ptr.hpp>
#include <streams_boost/weak_ptr.hpp>
#include <streams_boost/enable_shared_from_this.hpp>
#include <streams_boost/function.hpp>
#include <streams_boost/thread/mutex.hpp>

//#include <SPL/Runtime/Type/Blob.h>

namespace mcts 
{

    /// Set the profile of the connect
    /// CLIENT: Connections to specified address and port
    /// SERVER: Listens on specified address and port
    enum Role { CLIENT, SERVER };

    /// Sets the security tyoe
    /// NONE: Plain TCP Connection
    /// TLS: TLSv1.2 TCP Connection
    enum ConnectionSecurity { NONE, TLS };

    /// Represents a single connection from a client.
    class TCPConnection
        : public streams_boost::enable_shared_from_this<TCPConnection>
    {
    public:

    	/// Construct a connection with the given io_service.
        /// @param ioService the IO service
        /// @param handler the handler
        TCPConnection(ConnectionSecurity sec, Role role, streams_boost::asio::io_service& ioService, uint32_t blockSize, outFormat_t outFormat,
                      DataHandler & dHandler,  InfoHandler & iHandler,
                      const std::string & certificate,
                      const std::string & key);

        /// Destructor
        ~TCPConnection();
        
        static uint32_t getNumberOfConnections() 
        {
            return numConnections_;
        }

        /// Get the number of pending outstanding writes.
        uint32_t * getNumOutstandingWritesPtr();

        /// Get the ip associated with the connection.
        std::string & remoteIp();

        /// Get the ip associated with the connection.
        uint32_t remotePort();

        /// Get the socket
        SocketPtr socket() { return socket_; }

        /// Start the first asynchronous operation for the connection.
        void start();

        /// Shutdown sending operation for the connection.
        void shutdown_conn(bool makeConnReadOnly);

    private:
        /// Handle completion of a read operation.
        void handleRead(const streams_boost::system::error_code& e,
                        std::size_t bytesTransferred);
        
        // The socket that handles network operations 
        SocketPtr socket_;

        // The role of the socket
        Role role_;

        /// The handler used to process the incoming request.
        DataHandler & dataHandler_;

        /// The handler used to process the status messages (connect, disconnect, reject).
        InfoHandler & infoHandler_;

        /// Buffer for incoming data.
        streams_boost::array<char, 8192> buffer_;

        /// The incoming data item
        DataItem dataItem_;

        /// IP address of the remote host
        std::string remoteIp_;

        /// Port of the remote host
        uint32_t remotePort_;

        // Blocksize
        uint32_t blockSize_;

        // output format
        outFormat_t outFormat_;

        static uint32_t numConnections_;

        uint32_t numOutstandingWrites_;

        bool isShutdown_;

        static SocketPtr createConnection(ConnectionSecurity sec, mcts::Role role, streams_boost::asio::io_service& ioService, const std::string & certificate, const std::string & key)
        {
            // Non-TLS Client & Server
            if(sec != TLS) 
                return SocketPtr(new Socket(ioService));
            // TLS & Client
            else if(role == CLIENT) 
                return SocketPtr(new TLSSocket(ioService, certificate));
            // TLS & Server
            else 
                return SocketPtr(new TLSSocket(ioService, certificate, key));
        }
    };
    
    typedef streams_boost::shared_ptr<TCPConnection> TCPConnectionPtr;
    typedef streams_boost::weak_ptr<TCPConnection> TCPConnectionWeakPtr;
} 

#endif /* MULTI_CONNECTION_TCP_SERVER_CONNECTION */
