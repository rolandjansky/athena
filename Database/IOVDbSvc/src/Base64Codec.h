/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// @file Base64Codec.h
// Header for base64 encoding/decoding functions
// @author Shaun Roe
// @date November 2019
#ifndef IOVDbSvc_Base64Codec_h
#define IOVDbSvc_Base64Codec_h
#include <string>

namespace coral{
  class Blob;
}
namespace IOVDbNamespace{
  std::string
  base64Encode(const coral::Blob & blob);

  coral::Blob
  base64Decode(const std::string & /*base64String*/);
}
#endif
