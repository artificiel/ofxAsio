#pragma once

#include "../Includes.h"

namespace ofxAsio {
	namespace UDP {
		class EndPoint {
		public:
			EndPoint();
			EndPoint(asio::ip::udp::endpoint &);
			EndPoint(std::string ipAddress, int port);

			asio::ip::udp::endpoint & getEndPoint();
			const asio::ip::udp::endpoint & getEndPoint() const;
		protected:
			asio::ip::udp::endpoint endPoint;
		};

		std::ostream & operator<<(std::ostream &, const EndPoint &);
	}
}
