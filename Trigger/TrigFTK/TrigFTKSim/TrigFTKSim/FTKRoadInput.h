/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKROADINPUT_H
#define FTKROADINPUT_H

#include <map>

#include "TrigFTKSim/FTKRoad.h"
#include "TrigFTKSim/FTKRoadStream.h"
#include "TrigFTKSim/FTKSS.h"

class FTKRoadInput {
 public:
  virtual ~FTKRoadInput() {;}

  virtual int addFile(const char*) = 0; // add a single file
  
  virtual int addFileList(const char *) = 0; // add a file with a list of files

  virtual const char* getCurrentFileName() = 0;

  virtual void setNBanks(int v) = 0;
  virtual int getNBanks() const = 0;

  virtual FTKRoadStream* getStream(int id) = 0;

  virtual void init(bool *goodRegions=0x0) = 0; // initialize the structur

  virtual int nextEvent() = 0; // read the next event
  virtual unsigned long runNumber(const unsigned int& ibank) const = 0;
  virtual unsigned long eventNumber(const unsigned int& ibank) const  = 0;

  virtual const FTKRoad* nextRoad(int) = 0; // move to the next road and return the pointer
  virtual const FTKRoad* firstRoad(int) = 0; // move to the first road and return the pointer
  virtual const FTKSS& getSS(const unsigned int&,const unsigned int&,const unsigned int&) const = 0; // retrieve superstrip by (bank,plane,ssid)
  virtual const FTKSS& getUnusedSS(const unsigned int&,const unsigned int&,const unsigned int&) const = 0; // retrieve superstrip by (bank,plane,ssid)
  virtual FTKRoad* getRoad(int region,int roadid,int bankid) = 0; // retrieve road by its indices
  virtual const std::map<int,FTKSS>& getSSPlane(int bank,int pl) const = 0;
};
#endif // FTKROADINPUT_H
