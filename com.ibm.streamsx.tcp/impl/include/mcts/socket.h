/*
#######################################################################
# Copyright (C)2016, International Business Machines Corporation and
# others. All Rights Reserved.
#######################################################################
*/

#ifndef MULTI_CONNECTION_TCP_SERVER_SOCKET
#define MULTI_CONNECTION_TCP_SERVER_SOCKET

#include <streams_boost/asio.hpp>
#include <streams_boost/function.hpp>

namespace mcts 
{
  class Socket
  {
    public:
      typedef streams_boost::function<void(const streams_boost::system::error_code&, std::size_t)> async_complete_func;
      typedef streams_boost::function<void(const streams_boost::system::error_code)> accept_complete_func;
      typedef streams_boost::function<void(const streams_boost::system::error_code)> connect_complete_func;

      Socket(streams_boost::asio::io_service & ioService);

      /// The endpoing of this socket connection. Is always the opposite side regardless of send/receiver nature
      streams_boost::asio::ip::tcp::endpoint getEndpoint() const { return socket_.remote_endpoint(); } 
      
      /// Get the native socket handle. This can be used to set socket options in a 
      /// more conventional style. Settings may be erased if they conflict with boost
      /// regular operations
      int32_t getNativeSocketHandle() { return socket_.native_handle(); } 

      /// Return the underlying socket
      streams_boost::asio::ip::tcp::socket& getUnderlyingSocket() { return socket_; }
      
      /// Status of socket
      bool isOpen() const { return socket_.is_open(); }

      virtual void handleConnect(connect_complete_func handler, const streams_boost::system::error_code & ec);
      virtual void handleAccept(accept_complete_func handler, const streams_boost::system::error_code & ec);

      /// Shuts down the socket, either send, receive, or both
      virtual void shutdown(streams_boost::asio::ip::tcp::socket::shutdown_type what, streams_boost::system::error_code & ec);
      
      /// Cancels all pending aysnchronous operations on the socket
      virtual void cancel(streams_boost::system::error_code & ec);

      /// Dispatches a write operation and immediately returns without blocking for completion
      virtual void async_write(streams_boost::asio::const_buffers_1 buffer, async_complete_func handler);
      virtual void async_write(streams_boost::array<streams_boost::asio::const_buffer, 2> buffers, async_complete_func handler);
      
      /// Dispatches a read operations and immediately returns without blocking for completion
      virtual void async_read(streams_boost::asio::mutable_buffers_1 buffer, async_complete_func handler);
      virtual void async_read_some(streams_boost::asio::mutable_buffers_1 buffer, async_complete_func handler);

    protected:
      /// The strand is used to ensure all async operations execute in sequence against
      /// the socket to avoid simultaneous reads/writes
      streams_boost::asio::io_service::strand strand_;

      /// The Boost TCP/IP Socket
      streams_boost::asio::ip::tcp::socket socket_;
      
  };

  typedef streams_boost::shared_ptr<Socket> SocketPtr;
  typedef streams_boost::weak_ptr<Socket> SocketWeakPtr;
}

#endif