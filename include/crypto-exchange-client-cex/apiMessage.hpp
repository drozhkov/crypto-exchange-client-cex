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

/// apiMessage.hpp
///
/// 0.0 - created (Denis Rozhkov <denis@rozhkoff.com>)
///

#ifndef __CRYPTO_EXCHANGE_CLIENT_CEX__API_MESSAGE__H
#define __CRYPTO_EXCHANGE_CLIENT_CEX__API_MESSAGE__H


#include <iostream>
#include <vector>

#include "boost/json.hpp"

#include "crypto-exchange-client-core/core.hpp"
#include "crypto-exchange-client-core/exception.hpp"
#include "crypto-exchange-client-core/apiMessage.hpp"


namespace as::cryptox::cex {

	class ApiMessage : public ::as::cryptox::ApiMessage<ApiMessage> {};

	class ApiRequest : public ApiMessage {
	public:
		constexpr static as::t_stringview PairsInfo()
		{
			return AS_T( "/get_pairs_info" );
		}
	};

	class ApiResponsePairsInfo : public ApiMessage {
	public:
		struct Pair {
			as::t_string name;
			as::t_string baseName;
			as::t_string quoteName;
		};

	protected:
		std::vector<Pair> m_pairs;

	public:
		static ApiResponsePairsInfo deserialize( const ::as::t_stringview & s )
		{
			auto v = boost::json::parse( s );
			auto & o = v.get_object();

			ApiResponsePairsInfo result;

			if ( !o.contains( "ok" ) ) {
				return result;
			}

			for ( const auto & e : o["data"].get_array() ) {
				auto & s = e.get_object();

				Pair pair;
				pair.baseName.assign( s.at( "base" ).get_string() );
				pair.quoteName.assign( s.at( "quote" ).get_string() );
				pair.name.assign( pair.baseName + "-" + pair.quoteName );

				result.m_pairs.push_back( std::move( pair ) );
			}

			return result;
		}

		const std::vector<Pair> & Pairs() const
		{
			return m_pairs;
		}
	};

} // namespace as::cryptox::cex


#endif
