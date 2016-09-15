#ifndef MULTI_CONNECTION_TCP_SERVER_SOCKET_TLS
#define MULTI_CONNECTION_TCP_SERVER_SOCKET_TLS

#include "mcts/socket.h"
#include <streams_boost/asio.hpp>
#include <streams_boost/asio/ssl.hpp>
#include <streams_boost/scoped_ptr.hpp>

namespace mcts
{
  class TLSSocket : public Socket
  {
  public:
    TLSSocket(streams_boost::asio::io_service & ioService);

    virtual void connect();
    virtual void accept();
    virtual void shutdown(streams_boost::asio::ip::tcp::socket::shutdown_type what, streams_boost::system::error_code & ec);

    virtual void async_write(streams_boost::asio::const_buffers_1 buffer, async_complete_func handler);
    virtual void async_write(streams_boost::array<streams_boost::asio::const_buffer, 2> buffers, async_complete_func handler);
    virtual void async_read(streams_boost::asio::mutable_buffers_1 buffer, async_complete_func handler);
    virtual void async_read_some(streams_boost::asio::mutable_buffers_1 buffer, async_complete_func handler);

  private:

    /// Boost SSL Wrapper Stream
    streams_boost::scoped_ptr<streams_boost::asio::ssl::stream<streams_boost::asio::ip::tcp::socket&> > ssl_;

  };
}

#endif