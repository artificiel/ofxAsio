#pragma once

#include "Socket.h"

namespace ofxAsio {
	namespace TCP {
		class Client : public Socket {
		public:
			void init(std::string address, int port);
			void update() override;
		protected:
			std::string address;
			int port;
		};
	}
}
