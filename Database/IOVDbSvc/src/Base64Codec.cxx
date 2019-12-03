/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// @file Base64Codec.cxx
// Header for base64 encoding/decoding functions
// @author Shaun Roe
// @date November 2019

#include "CoralBase/Blob.h"
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <sstream>
#include <algorithm>

using namespace boost::archive::iterators;


namespace IOVDbNamespace{
std::string
  base64Encode(const coral::Blob & blob){
    typedef base64_from_binary<transform_width<const char *,6,8>> base64Text;
    const unsigned int nBytes = blob.size();
    //Blob::startingAddress returns a const void *, so cast to byte size
    const auto address = static_cast<const char *>(blob.startingAddress());
    unsigned int writePaddChars = (3-nBytes%3)%3;
    std::string result(base64Text(address), base64Text(address + nBytes));
    result.append(writePaddChars,'=');
    return result;
  }

  coral::Blob
  base64Decode(const std::string & /*base64String*/){
    coral::Blob blob;
    return blob;
  }
}
