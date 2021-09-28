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
  static void tokenize(const std::string& str,
           std::vector<std::string>& tokens,
                 char delimiters = ' ');
  static std::vector<std::string_view> tokenize(std::string_view str,
                       char delimiters = ' ');
  static int atoi(std::string_view);
  static float stof(std::string_view);
  static int atoi(char a) noexcept { return ((int)a) - ((int)'0'); }
}; 
}

#endif
