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

/// client.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __CRYPTO_EXCHANGE_CLIENT_CEX__CLIENT__H
#define __CRYPTO_EXCHANGE_CLIENT_CEX__CLIENT__H


#include "crypto-exchange-client-core/httpClient.hpp"
#include "crypto-exchange-client-core/client.hpp"

#include "crypto-exchange-client-cex/apiMessage.hpp"


namespace as::cryptox::cex {

	class Client : public as::cryptox::Client {
	public:
		static const size_t HttpClientApiIndex = 0;
		static const size_t HttpClientPrivateApiIndex = 1;
		static const size_t WsClientApiIndex = 0;
		static const size_t WsClientPrivateApiIndex = 1;

	protected:
		as::t_string m_apiKey;
		as::t_string m_apiSecret;

	private:
		void addAuthHeaders( HttpHeaderList & headers, as::t_string & body );

	protected:
		void wsErrorHandler(
			as::WsClient &, int, const as::t_string & ) override;

		void wsHandshakeHandler( as::WsClient & ) override;
		bool wsReadHandler( as::WsClient &, const char *, size_t ) override;

		void initCoinMap() override
		{
			cryptox::Client::initCoinMap();
		}

		void initSymbolMap() override;
		void initWsClient( size_t index ) override;

		bool subscribe( size_t wsClientIndex, const as::t_string & data );

	public:
		Client( const as::t_stringview & apiKey = AS_T( "" ),
			const as::t_stringview & apiSecret = AS_T( "" ),
			const as::t_stringview & httpApiUrl = AS_T(
				"https://trade.cex.io/api/spot/rest-public" ),
			const as::t_stringview & httpApiUrlPrivate = AS_T(
				"https://trade.cex.io/api/spot/rest" ),
			const as::t_stringview & wsApiUrl = AS_T(
				"wss://trade.cex.io/api/spot/ws-public" ),
			const as::t_stringview & wsApiUrlPrivate = AS_T(
				"wss://trade.cex.io/api/spot/ws" ) )
			: as::cryptox::Client( { httpApiUrl, httpApiUrlPrivate },
				  { wsApiUrl, wsApiUrlPrivate } )
			, m_apiKey( apiKey )
			, m_apiSecret( apiSecret )
		{
		}

		ApiResponsePairsInfo apiReqPairsInfo();

		void run(
			const t_exchangeClientReadyHandler & handler,
			const std::function<void( size_t )> & beforeRun = []( size_t ) {
			} ) override;

		bool subscribePriceBookTicker( size_t wsClientIndex,
			as::cryptox::Symbol symbol,
			const t_priceBookTickerHandler & handler ) override;

		void subscribeOrderUpdate( size_t wsClientIndex,
			const t_orderUpdateHandler & handler ) override;

		t_order placeOrder( Direction direction,
			as::cryptox::Symbol symbol,
			const FixedNumber & price,
			const FixedNumber & quantity ) override;
	};

} // namespace as::cryptox::cex


#endif
