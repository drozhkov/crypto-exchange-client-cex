/*
 *	Copyright (c) 2024 Denis Rozhkov <denis@rozhkoff.com>
 *	This file is part of crypto-exchange-client-cex.
 *
 *	crypto-exchange-client-cex is free software: you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or (at your
 *	option) any later version.
 *
 *	crypto-exchange-client-cex is distributed in the hope that it will be
 *	useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 *	Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License along with
 *	crypto-exchange-client-cex. If not, see <https://www.gnu.org/licenses/>.
 */

/// client.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include <sstream>

#include "boost/json.hpp"
#include "boost/iostreams/device/array.hpp"
#include "boost/iostreams/filtering_stream.hpp"
#include "boost/iostreams/filter/gzip.hpp"
#include "boost/iostreams/copy.hpp"

#include "crypto-exchange-client-core/logger.hpp"
#include "crypto-exchange-client-core/exception.hpp"

#include "crypto-exchange-client-cex/client.hpp"
#include "crypto-exchange-client-cex/wsMessage.hpp"


namespace as::cryptox::cex {

	void Client::addAuthHeaders(
		HttpHeaderList & headers, ::as::t_string & body )
	{
	}

	/// <summary>
	///
	/// </summary>
	/// <param name="client"></param>
	/// <param name="code"></param>
	/// <param name="message"></param>
	void Client::wsErrorHandler(
		WsClient & client, int code, const as::t_string & message )
	{

		AS_LOG_ERROR_LINE(
			client.Index() << AS_T( ":" ) << code << AS_T( ":" ) << message );
	}

	/// <summary>
	///
	/// </summary>
	/// <param name="client"></param>
	void Client::wsHandshakeHandler( WsClient & client )
	{
		client.readAsync();
	}

	/// <summary>
	///
	/// </summary>
	/// <param name="client"></param>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <returns></returns>
	bool Client::wsReadHandler(
		WsClient & client, const char * data, size_t size )
	{

		try {
			AS_LOG_TRACE_LINE(
				client.Index() << ": " << std::string( data, size ) );

			auto message = WsMessage::deserialize( data, size );

			switch ( message->TypeId() ) {
				case WsMessage::TypeIdConnected: {
					client.startPing(
						[]( WsClient & client ) {
							auto m = WsMessage::Ping();
							client.write( m.c_str(), m.length() );
						},
						std::chrono::seconds( 9 ) );

					AS_CALL( m_clientReadyHandler, *this, client.Index() );
				}

				break;

				case WsMessage::TypeIdOrderBook: {
					auto m = static_cast<WsMessageOrderBook *>( message.get() );

					as::cryptox::t_price_book_ticker t;
					t.symbol = toSymbol( m->SymbolName().c_str() );

					auto & ob = m_orderBookMap[t.symbol];


					{
						auto & entries = m->Bids();

						for ( size_t i = 0; i < entries.size(); ++i ) {
							ob.addBid( entries[i].first, entries[i].second );
						}
					}

					{
						auto & entries = m->Asks();

						for ( size_t i = 0; i < entries.size(); ++i ) {
							ob.addAsk( entries[i].first, entries[i].second );
						}
					}

					const auto & bestAsk = ob.BestAsk();
					t.askPrice = bestAsk.first;
					t.askQuantity = bestAsk.second;

					const auto & bestBid = ob.BestBid();
					t.bidPrice = bestBid.first;
					t.bidQuantity = bestBid.second;

					callSymbolHandler( t.symbol,
						m_priceBookTickerHandlerMap,
						client.Index(),
						t );
				}

				break;
			}
		}
		catch ( const std::exception & x ) {
			AS_LOG_ERROR_LINE( x.what() );
		}
		catch ( ... ) {
		}

		return true;
	}

	/// <summary>
	///
	/// </summary>
	void Client::initSymbolMap()
	{
		AS_LOG_INFO_LINE( "initializing..." );

		as::cryptox::Client::initSymbolMap();

		auto apiRes = apiReqPairsInfo();
		m_pairList.resize( apiRes.Pairs().size() + 2 );
		m_pairList[0] = as::cryptox::Pair( as::cryptox::Coin::_undef,
			as::cryptox::Coin::_undef,
			AS_T( "undefined" ) );

		size_t index = 1;

		for ( const auto & p : apiRes.Pairs() ) {
			AS_LOG_TRACE_LINE( p.name );

			as::cryptox::Coin quote = toCoin( p.quoteName.c_str() );
			as::cryptox::Coin base = toCoin( p.baseName.c_str() );

			as::cryptox::Pair pair( base, quote, p.name );

			m_pairList[index] = pair;

			addSymbolMapEntry(
				p.name, static_cast<as::cryptox::Symbol>( index ) );

			index++;
		}

		AS_LOG_INFO_LINE( "done" );
	}

	/// <summary>
	///
	/// </summary>
	/// <param name="index"></param>
	void Client::initWsClient( size_t index )
	{
		as::cryptox::Client::initWsClient( index );
	}

	/// <summary>
	///
	/// </summary>
	/// <returns></returns>
	ApiResponsePairsInfo Client::apiReqPairsInfo()
	{
		auto url =
			m_httpApiUrls[HttpClientApiIndex].add( ApiRequest::PairsInfo() );

		auto res = m_httpClient.get( url, HttpHeaderList() );

		return ApiResponsePairsInfo::deserialize( res );
	}

	/// <summary>
	///
	/// </summary>
	/// <param name="index"></param>
	/// <param name="data"></param>
	/// <returns></returns>
	bool Client::subscribe( size_t index, const as::t_string & data )
	{
		auto r = callWsClient<bool>( index, [&data]( WsClient * client ) {
			client->write( data.c_str(), data.length() );
			return true;
		} );

		return ( r.first && r.second );
	}

	/// <summary>
	///
	/// </summary>
	/// <param name="handler"></param>
	/// <param name="beforeRun"></param>
	void Client::run( const t_exchangeClientReadyHandler & handler,
		const std::function<void( size_t )> & beforeRun )
	{

		as::cryptox::Client::run( handler );
	}

	/// <summary>
	///
	/// </summary>
	/// <param name="wsClientIndex"></param>
	/// <param name="symbol"></param>
	/// <param name="handler"></param>
	/// <returns></returns>
	bool Client::subscribePriceBookTicker( size_t wsClientIndex,
		as::cryptox::Symbol symbol,
		const t_priceBookTickerHandler & handler )
	{

		as::cryptox::Client::subscribePriceBookTicker(
			wsClientIndex, symbol, handler );

		auto message = WsMessage::SubscribeOrderBook( toName( symbol ) );
		AS_LOG_TRACE_LINE( message );

		return subscribe( wsClientIndex, message );
	}

	/// <summary>
	///
	/// </summary>
	/// <param name="wsClientIndex"></param>
	/// <param name="handler"></param>
	void Client::subscribeOrderUpdate(
		size_t wsClientIndex, const t_orderUpdateHandler & handler )
	{

		as::cryptox::Client::subscribeOrderUpdate( wsClientIndex, handler );
	}

	/// <summary>
	///
	/// </summary>
	/// <param name="direction"></param>
	/// <param name="symbol"></param>
	/// <param name="price"></param>
	/// <param name="quantity"></param>
	/// <returns></returns>
	t_order Client::placeOrder( Direction direction,
		as::cryptox::Symbol symbol,
		const FixedNumber & price,
		const FixedNumber & quantity )
	{

		return ( t_order() );
	}

} // namespace as::cryptox::cex
