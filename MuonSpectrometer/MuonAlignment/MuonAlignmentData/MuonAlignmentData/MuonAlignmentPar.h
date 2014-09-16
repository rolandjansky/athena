/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_MUONALIGNMENTPAR_H
#define MUONALIGNMENTDATA_MUONALIGNMENTPAR_H

#include "Identifier/Identifier.h"


#include <string>


class MuonAlignmentPar {

 public:

  MuonAlignmentPar();
  ~MuonAlignmentPar();
  
  // Full constructor
  //  MuonAlignmentPar(Identifier stationId, SimpleTime sinceTime, SimpleTime tillTime);
  
  Identifier  identify()      {return m_stationId;}
  std::string key()           {return m_stationKey;}

  void setAmdbId(std::string type, int jff, int jzz, int job);
  void getAmdbId(std::string& type, int& jff, int& jzz, int& job);

  bool isNew()              {return m_isNew;}
  void isNew(bool newPar)   {m_isNew = newPar;}
  

  
 protected:
  
  // Identifier
  Identifier m_stationId;
  
  // geomodel key
  std::string m_stationKey;

  // Amdb identifier 
  std::string m_Type;
  int m_Jff;
  int m_Jzz;
  int m_Job;
  


  bool m_isNew;

};



#endif  // MUONALIGNMENTDATA_MUONALIGNMENTPAR_H
