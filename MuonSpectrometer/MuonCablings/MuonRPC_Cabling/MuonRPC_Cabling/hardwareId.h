/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  MUONRPC_CABLING_HARDWAREID_H
#define  MUONRPC_CABLING_HARDWAREID_H

//==== UTILITY CLASS TO CONVERT hardwareId used e.g. by DCS into offline Id and the other way round  ====//
//==== Author: M. Corradi (2016)

class hardwareId {

 public:
  hardwareId();
  bool SetHwId(std::string);
  bool setOfflineID(int stName, int stEta, int stPhi, int dbR, int dbPhi, int dbZ, int gasGap);
  void dump();
  
 private:
  bool convert_offline2HW();
  bool convert_HW2offline();    


 public: // would be nicer to make private and use access methods
  int m_stName, m_stEta, m_stPhi, m_dbR, m_dbPhi, m_dbZ, m_gasGap;
  std::string m_HwId;
  std::string m_HwStationName;
  int    m_HwStationNumber = 0;
  std::string m_HwSide;
  std::string m_HwPICO;
  int    m_HwSector = 0,m_HwDP = 0,m_HwLy = 0,m_HwDZ = 0;
  
};
#endif //  MUONRPC_CABLING_

