/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_MUONALIGNMENTPAR_H
#define MUONALIGNMENTDATA_MUONALIGNMENTPAR_H

#include "Identifier/Identifier.h"


#include <string>


class MuonAlignmentPar {

 public:

  MuonAlignmentPar();
  virtual ~MuonAlignmentPar()=default;

  Identifier  identify() const {return m_stationId;}
  std::string key()      const {return m_stationKey;}

  void setAmdbId(const std::string& type, int jff, int jzz, int job);
  void getAmdbId(std::string& type, int& jff, int& jzz, int& job) const;

  bool isNew() const        {return m_isNew;}
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
