/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKROADOUTPUT_H
#define FTKROADOUTPUT_H

class FTKRoad;
class FTKRoadStream;
class FTKSS;

#include <map>
#include <vector>

class FTKRoadOutput {
 public:
  virtual ~FTKRoadOutput() {;}

  virtual void setNBanks(int) = 0;
  virtual int getNBanks() const = 0;

  virtual void setFileName(const char *fname) = 0;
  virtual const char* getFileName() const = 0;
  virtual void setOutDir(const char *dname) = 0;
  virtual const char* getOutDir() const = 0;
  virtual void setMultiOut(bool v) = 0;
  virtual bool getMultiOut() const = 0;
  virtual void init(int bufsize = 0, bool *gooregions=0x0) = 0;
  virtual FTKRoadStream* getStream(int id) = 0;

  virtual void beginFile(const char *fname=0) = 0;
  virtual void eventBegin() = 0;
  virtual void eventEnd() = 0;
  virtual void endFile() = 0;

  virtual unsigned long runNumber(const unsigned int&) const = 0;
  virtual unsigned long eventNumber(const unsigned int&) const = 0;
  virtual void setRunNumber(const unsigned int&,const unsigned long&) = 0;
  virtual void setEventNumber(const unsigned int&,const unsigned long&) = 0;

  virtual void addRoad(int, const FTKRoad&) = 0;
  virtual void inc4LRoad(int, const int& val=1) = 0;
  virtual void addSS(int,int, int,const FTKSS&) = 0;
  virtual void addSSPlane(int, int,const std::map<int,FTKSS>&) = 0;
  virtual void addUnusedSSMap(int, const std::map<int,std::map<int,FTKSS> >&) = 0;

  // cluster/ss information for Naoki's timing simulation
  virtual void naoSetNhitsTot(int, int) = 0;
  virtual void naoSetNclusTot(int, int) = 0;
  virtual void naoSetNclus(int, const std::vector<int>&) = 0;
  virtual void naoSetNclus_road(int, const std::vector<int>&) = 0;
  virtual void naoAddNclus_road(int, const std::vector<int>&) = 0;
  virtual void naoSetNss(int, const std::vector<int>&) = 0;
  virtual void naoSetNroadsAM(int, int) = 0;
  virtual void naoSetNroadsAMComplete(int, int) = 0;
  virtual void naoSetNroadsAMMissPix(int, int) = 0;
  virtual void naoSetNroadsAMMissSCT(int, int) = 0;
  virtual void naoSetNroadsMOD(int, int) = 0;
  virtual void naoSetNroadsRW(int, int) = 0;
};

#endif // FTKROADOUTPUT_H
