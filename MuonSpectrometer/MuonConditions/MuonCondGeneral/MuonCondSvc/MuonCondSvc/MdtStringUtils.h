/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_MDTSTRINGUTILS_H
#define MUONCONDSVC_MDTSTRINGUTILS_H

namespace MuonCalib {
class MdtStringUtils {
 public:
  static void tokenize(const std::string& str,
		       std::vector<std::string>& tokens,
        	       const std::string& delimiters = " ");
}; 
}

#endif
