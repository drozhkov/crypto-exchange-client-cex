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

/// wsMessage.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __CRYPTO_EXCHANGE_CLIENT_CEX__WS_MESSAGE__H
#define __CRYPTO_EXCHANGE_CLIENT_CEX__WS_MESSAGE__H


#include <sstream>
#include <iomanip>

// not ctime as we need gmtime_s
#include <time.h>

#include "boost/json.hpp"

#include "crypto-exchange-client-core/core.hpp"
#include "crypto-exchange-client-core/client.hpp"
#include "crypto-exchange-client-core/wsMessage.hpp"

#include "crypto-exchange-client-cex/apiMessage.hpp"


namespace as::cryptox::cex {

	class WsMessage : public ::as::cryptox::WsMessage {
	public:
		static const ::as::cryptox::t_api_message_type_id TypeIdPong = 100;

		static const ::as::cryptox::t_api_message_type_id
			TypeIdPriceBookTicker = 101;

		static const ::as::cryptox::t_api_message_type_id TypeIdConnected = 102;
		static const ::as::cryptox::t_api_message_type_id TypeIdOrderBook = 103;

	protected:
		virtual void deserialize( boost::json::value & o ) = 0;

	public:
		WsMessage( t_api_message_type_id typeId )
			: ::as::cryptox::WsMessage( typeId )
		{
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="data"></param>
		/// <param name="size"></param>
		/// <returns></returns>
		static std::shared_ptr<::as::cryptox::ApiMessageBase> deserialize(
			const char * data, size_t size );

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static std::string Ping()
		{
			boost::json::object o;

			o["e"] = "ping";

			return boost::json::serialize( o );
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="symbolName"></param>
		/// <returns></returns>
		static as::t_string GetTicker( const t_stringview & symbolName )
		{
			boost::json::object o;

			o["e"] = "get_ticker";
			o["oid"] = uuidString();
			o["data"] = { { "pairs", { symbolName } } };

			return boost::json::serialize( o );
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="symbolName"></param>
		/// <returns></returns>
		static as::t_string SubscribeOrderBook(
			const t_stringview & symbolName )
		{

			boost::json::object o;

			o["e"] = "order_book_subscribe";
			o["oid"] = uuidString();
			o["data"] = { { "pair", symbolName } };

			return boost::json::serialize( o );
		}
	};

	class WsMessageConnected : public WsMessage {
	protected:
		void deserialize( boost::json::value & o ) override;

	public:
		WsMessageConnected()
			: WsMessage( TypeIdConnected )
		{
		}
	};

	class WsMessagePong : public WsMessage {
	protected:
		void deserialize( boost::json::value & o ) override;

	public:
		WsMessagePong()
			: WsMessage( TypeIdPong )
		{
		}
	};

	class WsMessageOrderBook : public WsMessage {
		using t_order_book_entry = std::vector<std::pair<t_number, t_number>>;

		t_string m_symbolName;
		t_order_book_entry m_bids;
		t_order_book_entry m_asks;

	protected:
		void deserialize( boost::json::value & o ) override;

	public:
		WsMessageOrderBook()
			: WsMessage( TypeIdOrderBook )
		{
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		const t_string & SymbolName() const
		{
			return m_symbolName;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		const t_order_book_entry & Bids() const
		{
			return m_bids;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		const t_order_book_entry & Asks() const
		{
			return m_asks;
		}
	};

} // namespace as::cryptox::cex


#endif
