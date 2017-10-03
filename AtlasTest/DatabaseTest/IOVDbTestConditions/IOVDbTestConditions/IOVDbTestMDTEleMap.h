/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 -----------------------------------------
 ***************************************************************************
 this class reads in the electronic mapping from the TBElectronics.map-file to return the Muon-Offline-Identifier to a given channel.
 ***************************************************************************/
#ifndef IOVDBTEST_MDT_ELE_MAP_H
#define IOVDBTEST_MDT_ELE_MAP_H

//<doc><file>	$Id: IOVDbTestMDTEleMap.h,v 1.5 2006-03-26 17:26:30 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#include "CLIDSvc/CLASS_DEF.h"
#include<string>

#define DIGIT 0
#define CSM_TRIG 1

class IOVDbTestMDTEleMap{
  
 public:
  //Constructor:
  IOVDbTestMDTEleMap();
  
  //Destructor
  ~IOVDbTestMDTEleMap();
  
  //methodes:
  int         runNumber() const;
  int         eventNumber() const;
  int         timeStamp() const;
  std::string name() const;
    
  void set(int runNumber, int eventNumber, const std::string& name); 
  void set(int timeStamp, const std::string& name); 
 
 private:
    int         m_runNumber;
    int         m_eventNumber;
    int         m_timeStamp;
    std::string m_name;    
};  

CLASS_DEF(IOVDbTestMDTEleMap, 130789821, 1)

#endif //IOVDBTEST_MDT_ELE_MAP_H




