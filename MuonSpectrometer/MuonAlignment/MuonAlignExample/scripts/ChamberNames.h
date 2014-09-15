/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CHAMBERNAMES_H
#define CHAMBERNAMES_H

#include <map>
#include <iostream>
#include <string>

class ChamberNames {

 public:

  ChamberNames(std::string filename);
  virtual ~ChamberNames();
  
  std::string chamberName(int identifier) const;
  int chamberIdFromString(const std::string& chname) const;  

  int stEta(int identifier) const;
  int stEta(const std::string& chname) const;

  int stPhi(int identifier) const;
  int stPhi(const std::string& chname) const;

 private:
  ChamberNames();
  std::map<int,std::string> m_chamberIdMap;
  
};

#endif // CHAMBERNAMES_H
