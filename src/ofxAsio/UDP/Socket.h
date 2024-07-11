#pragma once

#include "../Includes.h"
#include "DataGram.h"

namespace ofxAsio {
	namespace UDP {
		class Socket {
		public:
			struct AsyncArguments {
				bool success;
				std::string error;
				std::shared_ptr<DataGram> dataGram;
			};

			Socket();
			Socket(int port);
			~Socket();

			std::shared_ptr<DataGram> receive(size_t bufferSize = 9216);

			void asyncReceiveOnce(const std::function<void(AsyncArguments)> &, size_t bufferSize = 9216);
			void asyncReceiveAll(const std::function<void(AsyncArguments)> &, size_t bufferSize = 9216);
			
			bool send(std::shared_ptr<DataGram>);

			void close();
		protected:
			asio::io_service ioService;
			asio::io_service::work work;
			asio::ip::udp::socket socket;

			std::thread asyncThread;

			struct AsyncIncoming {
				std::string buffer;
				asio::ip::udp::endpoint endpoint;
			} asyncIncoming;
		};
	}
}
