/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_UTILCORE_H
#define ANP_UTILCORE_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Author : Rustem Ospanov
 *
 * @Brief  : Global utility functions for: strings, ROOT, etc
 * 
 **********************************************************************************/

// C/C++
#include <iostream>
#include <string>
#include <vector>

#include "TrigCostBase/Registry.h"

class TDirectory;
class TH1;

namespace Anp
{
  void StringTok(std::vector<std::string>& ls,
		 const std::string& str,
		 const std::string& tok);
  
  // Recursively make ROOT directories
  TDirectory* GetDir(std::string path, TDirectory *dir);
  TDirectory* GetDir(TDirectory *dir, std::string path);

  // Set histogram directory and name (if not empty)
  TH1* SetDir(TH1 *h, TDirectory *dir, const std::string &name = "");

  // Round variable using error as guidance
  std::pair<std::string, std::string> Round2Pair(double value, double error, int nprec=5);

  // Pad string with blanks
  std::string PadStrFront(std::string str, int width);
  std::string PadStrBack (std::string str, int width);

  // Get file size in MB
  double file_size(const std::string &path);

  // Make HTML link
  std::string MakeLink(const std::string &link, const std::string &text);

  std::vector<int>         GetIntVec   (const std::string &list);
  std::vector<std::string> GetStringVec(const std::string &list);

  // XML parsing using lbxml2
  Registry ParseXml(const std::string &fname, bool debug = false);
}

//
// Templates
//
namespace Anp
{
  template <class T> void Print(const std::vector<T> &tvec, std::ostream &os) {
    if(tvec.empty()) return;
    for(unsigned int i = 0; i < tvec.size(); ++i) os << tvec[i] << " ";
    os << std::endl;
  }
}

#endif
