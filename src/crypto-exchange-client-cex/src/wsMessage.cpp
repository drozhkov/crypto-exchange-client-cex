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

/// wsMessage.cpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#include "crypto-exchange-client-cex/wsMessage.hpp"


namespace as::cryptox::cex {

	std::shared_ptr<::as::cryptox::ApiMessageBase> WsMessage::deserialize(
		const char * data, size_t size )
	{

		auto v = boost::json::parse( { data, size } );
		auto & v1 = v;

		WsMessage * r = nullptr;

		auto & o = v.get_object();

		if ( o.contains( "e" ) && !o.contains( "error" ) ) {
			if ( o["e"].get_string() == "connected" ) {
				r = new WsMessageConnected;
			}
			else if ( o["e"].get_string() == "pong" ) {
				r = new WsMessagePong;
			}
			else if ( o["e"].get_string() == "order_book_subscribe" ||
				o["e"].get_string() == "order_book_increment" ) {

				r = new WsMessageOrderBook;
			}
		}

		if ( nullptr == r ) {
			return s_unknown;
		}

		r->deserialize( v1 );

		return std::shared_ptr<::as::cryptox::WsMessage>( r );
	}

	/// <summary>
	///
	/// </summary>
	/// <param name="o"></param>
	void WsMessageConnected::deserialize( boost::json::value & o )
	{
	}

	/// <summary>
	///
	/// </summary>
	/// <param name="o"></param>
	void WsMessagePong::deserialize( boost::json::value & o )
	{
	}

	/// <summary>
	///
	/// </summary>
	/// <param name="o"></param>
	void WsMessageOrderBook::deserialize( boost::json::value & o )
	{
		auto & data = o.at( "data" ).get_object();
		m_symbolName.assign( data["pair"].get_string() );

		for ( const auto & entry : data["bids"].get_array() ) {
			t_number price( entry.get_array()[0].get_string() );
			t_number quantity( entry.get_array()[1].get_string() );

			m_bids.emplace_back( price, quantity );
		}

		for ( const auto & entry : data["asks"].get_array() ) {
			t_number price( entry.get_array()[0].get_string() );
			t_number quantity( entry.get_array()[1].get_string() );

			m_asks.emplace_back( price, quantity );
		}
	}

} // namespace as::cryptox::cex
