/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKROADFILEINPUT_H
#define FTKROADFILEINPUT_H

#include <map>

#include "TrigFTKSim/FTKRoadStream.h"
#include "TrigFTKSim/FTKRoad.h"
#include "TrigFTKSim/FTKSS.h"
#include "TrigFTKSim/FTKObjectInput.h"
#include "TrigFTKSim/FTKRoadInput.h"

class FTKRoadFileInput : public FTKRoadInput, public FTKObjectInput<FTKRoadStream,FTKRoad>  {
 public:
  virtual ~FTKRoadFileInput() {;}

  int addFile(const char *fname) { return FTKObjectInput<FTKRoadStream,FTKRoad>::addFile(fname); } // add a single file
  
  int addFileList(const char *lname) { return FTKObjectInput<FTKRoadStream,FTKRoad>::addFileList(lname); } // add a file with a list of files

  const char* getCurrentFileName() { return FTKObjectInput<FTKRoadStream,FTKRoad>::getCurrentFileName(); }

  void setNBanks(int v) { FTKObjectInput<FTKRoadStream,FTKRoad>::setNBanks(v); }
  int getNBanks() const { return FTKObjectInput<FTKRoadStream,FTKRoad>::getNBanks(); }

  FTKRoadStream* getStream(int id) { return FTKObjectInput<FTKRoadStream,FTKRoad>::getStream(id); }

  void init(bool *goodRegions=0x0) { FTKObjectInput<FTKRoadStream,FTKRoad>::init(goodRegions); } // initialize the structur

  unsigned long runNumber(const unsigned int& ibank) const { return( (m_data && (m_data[ibank])) ? (m_data[ibank])->runNumber() : 0 ); }
  unsigned long eventNumber(const unsigned int& ibank) const { return( (m_data && (m_data[ibank])) ? (m_data[ibank])->eventNumber() : 0 ); }


  int nextEvent(); // read the next event
  const FTKRoad* nextRoad(int); // move to the next road and return the pointer
  const FTKRoad* firstRoad(int); // move to the first road and return the pointer
  const FTKSS& getSS(const unsigned int&,const unsigned int&,const unsigned int&) const; // retrieve superstrip by (bank,plane,ssid)
  const FTKSS& getUnusedSS(const unsigned int&,const unsigned int&,const unsigned int&) const; // retrieve superstrip by (bank,plane,ssid)
  FTKRoad* getRoad(int region,int roadid,int bankid); // retrieve road by its indices
  const std::map<int,FTKSS>& getSSPlane(int bank,int pl) const { return m_data[bank]->getSSPlane(pl); }
};
#endif // FTKROADINPUT_H
