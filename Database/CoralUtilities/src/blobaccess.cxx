/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CoralUtilities/blobaccess.h"

#include <iostream>
#include "zlib.h"
#include "TMemFile.h"
#include "TTree.h"
#include "CoralBase/Blob.h"
#include "CxxUtils/base64.h"

namespace CoralUtilities {


// Compression Functions
// -------------------------------------------------

bool compressBlob(const char* in, coral::Blob& out) {
	uLongf comprLen = compressBound(strlen(in));
	coral::Blob blob;
	blob.resize(comprLen);
	unsigned char* ptr = static_cast<unsigned char*>(blob.startingAddress());
	compress(ptr, &comprLen, reinterpret_cast<const unsigned char*>(in), strlen(in));
	blob.resize(comprLen);
	out = blob;
	return true;
}

bool writeBlobFromString(const std::string_view in, coral::Blob& out) {
	return compressBlob(in.data(), out);
}

bool writeBlobFromJson(const nlohmann::json& in, coral::Blob& out) {
	return writeBlobFromString(in.dump(), out);
}

bool writeBlobFromTTree(TTree* tree, coral::Blob& out) {
	TMemFile fout("buffer","recreate");
	fout.cd();
	tree->CloneTree(-1, "fast")->Write();
	fout.Write();
	uLongf comprLen = fout.GetSize();
	unsigned char *buffer = new unsigned char[comprLen];
	fout.CopyTo(buffer, comprLen);
	coral::Blob blob; 
	blob.resize(comprLen);
	char* ptr = reinterpret_cast<char*>(blob.startingAddress()); 
	*ptr = *const_cast<char*>(CxxUtils::base64_encode(buffer, comprLen).c_str());
	out = blob;
	return true;
}




// Reading and Uncompressing Functions
// -------------------------------------------------

bool uncompressBlob(const coral::Blob &blob, unsigned char*& out, unsigned long& len){
    uLongf uncompLen = 50000;
	std::unique_ptr<unsigned char[]> uncompBuf(new unsigned char[uncompLen+1]);
	uLongf actualLen;
	while(true) {
		actualLen = uncompLen;
		int res(uncompress(uncompBuf.get(), &actualLen, reinterpret_cast<const unsigned char*>(blob.startingAddress()), static_cast<uLongf>(blob.size())));
		if(res == Z_OK       ) break;
		if(res == Z_BUF_ERROR) { // double buffer if it was not big enough
			uncompLen *= 2;
			std::cout << "ATTENTION: Increasing buffer to " << uncompLen << std::endl;
			uncompBuf.reset(new unsigned char[uncompLen+1]);
			continue;
		}
		// something else is wrong
		uncompBuf.reset();
		return false;
	}
	uncompBuf.get()[actualLen]=0; // append 0 to terminate string
	out = uncompBuf.release();
	len = actualLen;
	return true;
}
	 
bool readBlobAsString(const coral::Blob &blob, std::string& out){
	unsigned char* bf = nullptr;
	uLongf len = 0;
	if(!uncompressBlob(blob, bf, len)) return false;
	const char* cstring = reinterpret_cast<const char*>(bf); // need to cast to char*
	out.assign(cstring); // copy over to C++ string
	delete[] bf; // also deletes cstring since it points to the same memory as bf
	return true;
}
	 
bool readBlobAsJson(const coral::Blob &blob, nlohmann::json& out){
	std::string str = "";
	if(!readBlobAsString(blob, str)) return false;
	try {
		out = nlohmann::json::parse(str);
	}
	catch(const std::exception& e) {
		std::cout << "ATTENTION: Cannot unpack Blob object as JSON!" << std::endl;
		return false;
	}
	return true;
}

bool readBlobAsTTree(const coral::Blob &blob, TTree*& out){
	std::string sb = reinterpret_cast<const char*>(blob.startingAddress());
	std::vector<unsigned char> bdata = CxxUtils::base64_decode(sb);
	TMemFile f("buffer", reinterpret_cast<char*>(bdata.data()), static_cast<uLongf>(bdata.size())); 
	TTree* t = (TTree*) f.Get("tree");
	t->LoadBaskets();
	t->SetDirectory(0);
	f.Close();
	out = t;
	return true;
}
	 
/*
// keep for now; msgpack maybe used later at some point
bool readBlobAsMsgpack(const coral::Blob &blob, std::string& out){
	unsigned char* bf; uLongf len;
	if(!uncompressBlob(blob, &bf, len)) return false;
	const char* temp = reinterpret_cast<char*>(bf);
	try {
		auto upd = msgpack::unpack(temp, len);
		msgpack::object obj = upd.get();
		std::stringstream ss;
		ss << obj;
		out = ss.str();
	}
	catch(const std::exception& e) {
		std::cout << "ATTENTION: Cannot unpack Blob object as Msgpack!" << std::endl;
		return false;
	}
	return true;
}
*/



}
