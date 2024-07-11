#pragma once

#include "../Includes.h"
#include "../DataGram.h"

namespace ofxAsio {
	namespace TCP {
		class Socket {
		public:
			Socket();

			virtual void update() = 0;
			bool isActive() const;
			bool isConnected() const;

			std::shared_ptr<DataGram> receive(size_t bufferSize = 9216);
			bool send(std::shared_ptr<DataGram>);
		protected:
			bool active = false;
			asio::io_service ioService;
			asio::ip::tcp::socket socket;
		};
	}
}
