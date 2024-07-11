#include "Message.h"
//#include "ofxFmtLib.hpp"
//----------
Message::Message() {
	
}

//----------
Message::Message(const string & data) {
	this->setData(data);
}

//----------
Message::Message(const void * data, size_t size) {
	this->setData(data, size);
}

//----------
Message::Message(const ofPixels & data) {
	this->setData(data);
}

//----------
Message::Message(const ofShortPixels & data) {
	this->setData(data);
}

//----------
Message::Message(const ofFloatPixels & data) {
	this->setData(data);
}

//----------
Message::Message(const ofMesh & data) {
	this->setData(data);
}

//----------
void Message::setData(const string & data) {
	this->setData(data.data(), data.size());
}

//----------
void Message::setData(const void * data, size_t size) {
	this->headerAndBody.resize(size + sizeof(Header::String));
	
	auto & header = this->getHeader<Header::String>(true);
	auto body = this->getBodyData();
	memcpy(body, data, size);
}

//----------
void Message::setData(const ofPixels & data) {
	const auto headerSize = sizeof(Header::Pixels);
	const auto bodySize = data.size(); // inner payload
	
	this->headerAndBody.resize(headerSize + bodySize);
	
	auto & header = this->getHeader<Header::Pixels>(true);
	header.width = data.getWidth();
	header.height = data.getHeight();
	header.pixelFormat = data.getPixelFormat();
	
	auto body = this->getBodyData();
	memcpy(body, data.getData(), bodySize);
}

//----------
void Message::setData(const ofShortPixels & data) {
	const auto headerSize = sizeof(Header::ShortPixels);
	const auto bodySize = data.size() * sizeof(uint16_t); // inner payload
	
	this->headerAndBody.resize(headerSize + bodySize);
	
	auto & header = this->getHeader<Header::ShortPixels>(true);
	header.width = data.getWidth();
	header.height = data.getHeight();
	header.pixelFormat = data.getPixelFormat();
	
	auto body = this->getBodyData();
	memcpy(body, data.getData(), bodySize);
}

//----------
void Message::setData(const ofFloatPixels & data) {
	const auto headerSize = sizeof(Header::FloatPixels);
	const auto bodySize = data.size() * sizeof(float); // inner payload
	
	this->headerAndBody.resize(headerSize + bodySize);
	
	auto & header = this->getHeader<Header::FloatPixels>(true);
	header.width = data.getWidth();
	header.height = data.getHeight();
	header.pixelFormat = data.getPixelFormat();
	
	auto body = this->getBodyData();
	memcpy(body, data.getData(), bodySize);
}

//----------


void Message::setData(const ofMesh & data) {
	const auto headerSize = sizeof(Header::Mesh);
	
	const auto verticesDataSize = data.getNumVertices() * sizeof(ofVec3f);
	const auto colorsDataSize = data.getNumColors() * sizeof(ofFloatColor);
	const auto normalsDataSize = data.getNumNormals() * sizeof(ofVec3f);
	const auto texCoordsDataSize = data.getNumTexCoords() * sizeof(ofVec2f);
	const auto indicesDataSize = data.getNumIndices() * sizeof(ofIndexType);
	
	const size_t bodySize = verticesDataSize + colorsDataSize + normalsDataSize + texCoordsDataSize + indicesDataSize;
	
	this->headerAndBody.resize(headerSize + bodySize);
	
	// header
	{
		auto & header = this->getHeader<Header::Mesh>(true);
		header.verticesSize = (uint32_t) data.getNumVertices();
		header.colorsSize = (uint32_t) data.getNumColors();
		header.normalsSize = (uint32_t) data.getNumNormals();
		header.texCoordsSize = (uint32_t) data.getNumTexCoords();
		header.indicesSize = (uint32_t) data.getNumIndices();
		
		header.primitiveMode = data.getMode();
		
		header.useColors = data.usingColors();
		header.useNormals = data.usingNormals();
		header.useTextures = data.usingTextures();
		header.useIndices = data.usingIndices();
	}
	
	// body
	{
		auto body = (uint8_t *) this->getBodyData();
		
		memcpy(body, data.getVerticesPointer(), verticesDataSize);
		body += verticesDataSize;
		
		memcpy(body, data.getColorsPointer(), colorsDataSize);
		body += colorsDataSize;
		
		memcpy(body, data.getNormalsPointer(), normalsDataSize);
		body += normalsDataSize;
		
		memcpy(body, data.getTexCoordsPointer(), texCoordsDataSize);
		body += texCoordsDataSize;
		
		memcpy(body, data.getIndexPointer(), indicesDataSize);
	}
}

void Message::setData(const std::vector<std::unique_ptr<ofMesh>> & data) {
	const auto headerSize = sizeof(Header::MeshVector);
	const auto meshHeaderSize = sizeof(Header::Mesh);

	size_t bodySize { 0 };
	for (const auto & mesh: data) {
		
		const auto verticesDataSize = mesh->getNumVertices() * sizeof(ofVec3f);
		const auto colorsDataSize = mesh->getNumColors() * sizeof(ofFloatColor);
		const auto normalsDataSize = mesh->getNumNormals() * sizeof(ofVec3f);
		const auto texCoordsDataSize = mesh->getNumTexCoords() * sizeof(ofVec2f);
		const auto indicesDataSize = mesh->getNumIndices() * sizeof(ofIndexType);
		bodySize += (meshHeaderSize + verticesDataSize + colorsDataSize + normalsDataSize + texCoordsDataSize + indicesDataSize);
	}
//	ofLogNotice("message of size") << bodySize << " for #mesh " << data.size();
	headerAndBody.resize(headerSize + bodySize);
	auto & header = this->getHeader<Header::MeshVector>(true);
	header.meshesSize = (uint16_t) data.size();

	auto body = (uint8_t *) this->getBodyData();
	for (const auto & mesh: data) {
		
		auto header = Header::Mesh();
		header.verticesSize = (uint32_t) mesh->getNumVertices();
		header.colorsSize = (uint32_t) mesh->getNumColors();
		header.normalsSize = (uint32_t) mesh->getNumNormals();
		header.texCoordsSize = (uint32_t) mesh->getNumTexCoords();
		header.indicesSize = (uint32_t) mesh->getNumIndices();
		
		header.primitiveMode = mesh->getMode();
		
		header.useColors = mesh->usingColors();
		header.useNormals = mesh->usingNormals();
		header.useTextures = mesh->usingTextures();
		header.useIndices = mesh->usingIndices();

		memcpy(body, &header, meshHeaderSize);
		body += meshHeaderSize;

		const auto verticesDataSize = mesh->getNumVertices() * sizeof(ofVec3f);
		const auto colorsDataSize = mesh->getNumColors() * sizeof(ofFloatColor);
		const auto normalsDataSize = mesh->getNumNormals() * sizeof(ofVec3f);
		const auto texCoordsDataSize = mesh->getNumTexCoords() * sizeof(ofVec2f);
		const auto indicesDataSize = mesh->getNumIndices() * sizeof(ofIndexType);

		memcpy(body, mesh->getVerticesPointer(), verticesDataSize);
		body += verticesDataSize;
		
		memcpy(body, mesh->getColorsPointer(), colorsDataSize);
		body += colorsDataSize;
		
		memcpy(body, mesh->getNormalsPointer(), normalsDataSize);
		body += normalsDataSize;
		
		memcpy(body, mesh->getTexCoordsPointer(), texCoordsDataSize);
		body += texCoordsDataSize;
		
		memcpy(body, mesh->getIndexPointer(), indicesDataSize);
		body += indicesDataSize;
	}
}

//----------
void Message::clear() {
	this->headerAndBody.clear();
}

//----------
bool Message::getData(string & data) const {
	if (this->hasHeader<Header::String>()) {
		auto header = this->getHeader<Header::String>();
		data.assign((char *) this->getBodyData(), this->getBodySize());
		return true;
	}
	else {
		ofLogError("Message") << "Message Header doesn't match String/Basic type";
		return false;
	}
}

//----------
bool Message::getData(void * data, size_t & size) const {
	if (this->hasHeader<Header::String>()) {
		auto header = this->getHeader<Header::String>();
		auto bodySize = this->getBodySize();
		if (bodySize > size) {
			ofLogError("Message") << "Insufficient size in your buffer. Cannot get data";
			return false;
		}
		else {
			memcpy(data, this->getBodyData(), bodySize);
			size = bodySize;
			return true;
		}
	}
	else {
		ofLogWarning("Message") << "Message Header doesn't match String/Basic type";
		return false;
	}
}

//----------
bool Message::getData(ofPixels & data) const {
	if (this->hasHeader<Header::Pixels>()) {
		const auto & header = this->getHeader<Header::Pixels>();
		auto bodySize = this->getBodySize();
		ofPixelFormat pixelFormat = (ofPixelFormat)header.pixelFormat;
		
		//reallocate if we need to
		if (data.getWidth() != header.width || data.getHeight() != header.height || data.getPixelFormat() != pixelFormat) {
			data.allocate(header.width, header.height, pixelFormat);
		}
//		ofLogNotice("gg") << fmt::format("data {}x{}/{}@{}, header {}x{}/{}@{}",data.getWidth(), data.getHeight(), data.getPixelFormat(), data.size(), header.width, header.height,pixelFormat, bodySize);
		if (data.size() != bodySize) {
			ofLogError("Message") << "Message body is of wrong size to fill pixels. Maybe a bug in sender?";
			return false;
		}
		else {
			memcpy(data.getData(), this->getBodyData(), bodySize);
			return true;
		}
	}
	else {
		ofLogWarning("Message") << "Message Header doesn't match Pixels type";
		return false;
	}
}

//----------
bool Message::getData(ofShortPixels & data) {
	if (this->hasHeader<Header::ShortPixels>()) {
		const auto & header = this->getHeader<Header::ShortPixels>();
		auto bodySize = this->getBodySize();
		ofPixelFormat pixelFormat = (ofPixelFormat)header.pixelFormat;
		
		//reallocate if we need to
		data.allocate(header.width, header.height, pixelFormat);
		if (data.size() * sizeof(uint16_t) != bodySize) {
			ofLogError("Message") << "Message body is of wrong size to fill pixels. Maybe a bug in sender?";
			return false;
		}
		else {
			memcpy(data.getData(), this->getBodyData(), bodySize);
			return true;
		}
	}
	else {
		ofLogWarning("Message") << "Message Header doesn't match ShortPixels type";
		return false;
	}
}

//----------
bool Message::getData(ofFloatPixels & data) {
	if (this->hasHeader<Header::FloatPixels>()) {
		const auto & header = this->getHeader<Header::FloatPixels>();
		auto bodySize = this->getBodySize();
		ofPixelFormat pixelFormat = (ofPixelFormat)header.pixelFormat;
		
		//reallocate if we need to
		data.allocate(header.width, header.height, pixelFormat);
		if (data.size() * sizeof(float) != bodySize) {
			ofLogError("Message") << "Message body is of wrong size to fill pixels. Maybe a bug in sender?";
			return false;
		}
		else {
			memcpy(data.getData(), this->getBodyData(), bodySize);
			return true;
		}
	}
	else {
		ofLogWarning("Message") << "Message Header doesn't match FloatPixels type";
		return false;
	}
}

//----------
bool Message::getData(ofMesh & data) const {
	if (this->hasHeader<Header::Mesh>()) {
		const auto & header = this->getHeader<Header::Mesh>();
		auto bodySize = this->getBodySize();
		
		auto & vertices = data.getVertices();
		auto & colors = data.getColors();
		auto & normals = data.getNormals();
		auto & texCoords = data.getTexCoords();
		auto & indices = data.getIndices();
		
		//resize as needed
		vertices.resize(header.verticesSize);
		colors.resize(header.colorsSize);
		normals.resize(header.normalsSize);
		texCoords.resize(header.texCoordsSize);
		indices.resize(header.indicesSize);
		
		//data sizes
		const auto verticesDataSize = header.verticesSize * sizeof(ofVec3f);
		const auto colorsDataSize = header.colorsSize * sizeof(ofFloatColor);
		const auto normalsDataSize = header.normalsSize * sizeof(ofVec3f);
		const auto texCoordsDataSize = header.texCoordsSize * sizeof(ofVec2f);
		const auto indicesDataSize = header.indicesSize * sizeof(ofIndexType);
		
		//copy in data
		{
			auto body = (uint8_t *) this->getBodyData();
			
			memcpy(data.getVerticesPointer(), body, verticesDataSize);
			body += verticesDataSize;
			
			memcpy(data.getColorsPointer(), body, colorsDataSize);
			body += colorsDataSize;
			
			memcpy(data.getNormalsPointer(), body, normalsDataSize);
			body += normalsDataSize;
			
			memcpy(data.getTexCoordsPointer(), body, texCoordsDataSize);
			body += texCoordsDataSize;
			
			memcpy(data.getIndexPointer(), body, indicesDataSize);
			body += indicesDataSize;
		}
		
		//apply header
		{
			data.setMode((ofPrimitiveMode) header.primitiveMode);
			header.useColors ? data.enableColors() : data.disableColors();
			header.useNormals ? data.enableNormals() : data.disableNormals();
			header.useTextures ? data.enableTextures() : data.disableTextures();
		}
		
		return true;
	}
	else {
		ofLogWarning("Message") << "Message Header doesn't match Mesh type";
		return false;
	}
	
}

bool Message::getData(std::vector<std::unique_ptr<ofMesh>> & data) const {
	if (this->hasHeader<Header::MeshVector>()) {
		const auto & header = this->getHeader<Header::MeshVector>();
		auto bodySize = this->getBodySize();
//		ofLogNotice("extracting") << header.meshesSize;
		
		auto body = (uint8_t *) this->getBodyData();

		for (size_t i = 0; i<header.meshesSize; i++) {

			
			data.emplace_back(std::make_unique<ofMesh>());

			Header::Mesh mesh_header;
			memcpy(&mesh_header, body, sizeof(Header::Mesh));
			body +=  sizeof(Header::Mesh);
		
			auto & vertices = data.back()->getVertices();
			auto & colors = data.back()->getColors();
			auto & normals = data.back()->getNormals();
			auto & texCoords = data.back()->getTexCoords();
			auto & indices = data.back()->getIndices();
			
			//resize as needed
			vertices.resize(mesh_header.verticesSize);
			colors.resize(mesh_header.colorsSize);
			normals.resize(mesh_header.normalsSize);
			texCoords.resize(mesh_header.texCoordsSize);
			indices.resize(mesh_header.indicesSize);
			
			//data sizes
			const auto verticesDataSize = mesh_header.verticesSize * sizeof(ofVec3f);
			const auto colorsDataSize = mesh_header.colorsSize * sizeof(ofFloatColor);
			const auto normalsDataSize = mesh_header.normalsSize * sizeof(ofVec3f);
			const auto texCoordsDataSize = mesh_header.texCoordsSize * sizeof(ofVec2f);
			const auto indicesDataSize = mesh_header.indicesSize * sizeof(ofIndexType);
			
			//copy in data
			{
//				auto body = (uint8_t *) this->getBodyData();
				
				memcpy(data.back()->getVerticesPointer(), body, verticesDataSize);
				body += verticesDataSize;
				
				memcpy(data.back()->getColorsPointer(), body, colorsDataSize);
				body += colorsDataSize;
				
				memcpy(data.back()->getNormalsPointer(), body, normalsDataSize);
				body += normalsDataSize;
				
				memcpy(data.back()->getTexCoordsPointer(), body, texCoordsDataSize);
				body += texCoordsDataSize;
				
				memcpy(data.back()->getIndexPointer(), body, indicesDataSize);
				body += indicesDataSize;
			}
			
			//apply header
			{
				data.back()->setMode((ofPrimitiveMode) mesh_header.primitiveMode);
				mesh_header.useColors ? data.back()->enableColors() : data.back()->disableColors();
				mesh_header.useNormals ? data.back()->enableNormals() : data.back()->disableNormals();
				mesh_header.useTextures ? data.back()->enableTextures() : data.back()->disableTextures();
			}
			 
		}
		return true;
	}
	else {
		ofLogWarning("Message") << "Message Header doesn't match Mesh type";
		return false;
	}
	
}

//----------
void Message::pushData(const void * data, size_t dataSize) {
	this->headerAndBody.append((const char *) data, dataSize);
}

//----------
void * Message::getHeaderData() {
	return (void *) this->headerAndBody.data();
}

//----------
const void * Message::getHeaderData() const {
	return (void *) this->headerAndBody.data();
}

//----------
size_t Message::getHeaderSize() const {
	if (this->headerAndBody.size() > sizeof(Header::Basic)) {
		const auto & header = this->getHeader<Header::Basic>();
		return header.headerSize;
	}
	else {
		return 0;
	}
}

//----------
void * Message::getBodyData() {
	return &this->headerAndBody[0] + this->getHeaderSize();
}

//----------
const void * Message::getBodyData() const {
	return &this->headerAndBody[0] + this->getHeaderSize();
}

//----------
size_t Message::getBodySize() const {
	return this->headerAndBody.size() - this->getHeaderSize();
}

//----------
bool Message::empty() const {
	if (this->headerAndBody.empty()) {
		return true;
	}
	else if(this->headerAndBody.size() < this->getHeaderSize()) {
		return true;
	}
	else {
		return false;
	}
}

//----------
void Message::resizeHeaderAndBody(size_t size) {
	this->headerAndBody.resize(size);
}

//----------
const string & Message::getMessageString() const {
	return this->headerAndBody;
}

//----------
size_t Header::MultiTrack_2_3_Frame::FrameSettings::size() const {
	return this->width * this->height * getBytesPerPixel(this->pixelFormat);
}

