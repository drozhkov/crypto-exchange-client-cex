#include <iostream>
#include <exception>

#include "crypto-exchange-client-cex/client.hpp"

#include "api-secret.hpp"


int main()
{
	try {
		as::cryptox::cex::Client client(
			as::cryptox::cex::ApiKey(), as::cryptox::cex::ApiSecret() );

		client
			.ErrorHandler( []( as::cryptox::Client &, size_t wsClientIndex ) {
				std::cerr << "ErrorHandler" << std::endl;
			} )
			.run( []( as::cryptox::Client & c, size_t wsClientIndex ) {
				std::cout << "ready: " << wsClientIndex << std::endl;

				// c.subscribeOrderUpdate( wsClientIndex,
				//	[]( as::cryptox::Client & c,
				//		size_t,
				//		as::cryptox::t_order_update & u ) {
				//		std::cout << "order update: " << u.orderId << std::endl;
				//	} );


				if ( as::cryptox::cex::Client::WsClientApiIndex ==
					wsClientIndex ) {

					auto symbol = c.toSymbol(
						as::cryptox::Coin::BTC, as::cryptox::Coin::USDT );

					c.subscribePriceBookTicker( wsClientIndex,
						symbol,
						[]( as::cryptox::Client & c,
							size_t,
							as::cryptox::t_price_book_ticker & t ) {
							const auto & pair = c.toPair( t.symbol );

							std::cout << "t_price_book_ticker: " << pair.Name()
									  << ", " << t.askPrice.toString() << " / "
									  << t.askQuantity.toString() << " - "
									  << t.bidPrice.toString() << " / "
									  << t.bidQuantity.toString() << std::endl;
						} );
				}
			} );
	}
	catch ( const std::exception & x ) {
		std::cerr << x.what() << std::endl;
	}
	catch ( ... ) {
		std::cerr << "error" << std::endl;
	}

	return 0;
}
