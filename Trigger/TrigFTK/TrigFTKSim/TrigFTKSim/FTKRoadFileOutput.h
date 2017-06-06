/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKROADFILEOUTPUT_H
#define FTKROADFILEOUTPUT_H

#include "FTKRoadOutput.h"
#include "FTKObjectOutput.h"
#include "FTKRoad.h"
#include "FTKRoadStream.h"

class FTKRoadFileOutput : public FTKObjectOutput<FTKRoadStream>, 
  public FTKRoadOutput {
  
 public:
  virtual ~FTKRoadFileOutput();
  virtual void setNBanks(int val) 
  { FTKObjectOutput<FTKRoadStream>::setNBanks(val); }
  virtual int getNBanks() const
  { return FTKObjectOutput<FTKRoadStream>::getNBanks(); }

  virtual void setFileName(const char *fname)
  { FTKObjectOutput<FTKRoadStream>::setFileName(fname);}
  virtual const char* getFileName() const 
  { return FTKObjectOutput<FTKRoadStream>::getFileName(); }
  virtual void setOutDir(const char *dname)
  { FTKObjectOutput<FTKRoadStream>::setOutDir(dname); }
  virtual const char* getOutDir() const
  { return FTKObjectOutput<FTKRoadStream>::getOutDir(); }
  virtual void setMultiOut(bool v)
  { FTKObjectOutput<FTKRoadStream>::setMultiOut(v); }
  virtual bool getMultiOut() const
  { return FTKObjectOutput<FTKRoadStream>::getMultiOut(); }
  virtual void init(int bufsize = 0, bool *goodregions=0x0)
  { FTKObjectOutput<FTKRoadStream>::init(bufsize,goodregions); }
  virtual FTKRoadStream* getStream(int id) 
  { return FTKObjectOutput<FTKRoadStream>::getStream(id); }

  virtual void beginFile(const char *fname=0)
  { FTKObjectOutput<FTKRoadStream>::beginFile(fname); }
  virtual void eventBegin() { FTKObjectOutput<FTKRoadStream>::eventBegin(); }
  virtual void eventEnd() { FTKObjectOutput<FTKRoadStream>::eventEnd(); }
  virtual void endFile() { FTKObjectOutput<FTKRoadStream>::endFile(); }

  virtual unsigned long runNumber(const unsigned int& ival) const
  { return FTKObjectOutput<FTKRoadStream>::runNumber(ival); }
  virtual unsigned long eventNumber(const unsigned int &ival) const
  { return FTKObjectOutput<FTKRoadStream>::eventNumber(ival); }
  virtual void setRunNumber(const unsigned int &v1,const unsigned long &v2)
  { FTKObjectOutput<FTKRoadStream>::setRunNumber(v1,v2); }
  virtual void setEventNumber(const unsigned int &v1,const unsigned long &v2)
  { FTKObjectOutput<FTKRoadStream>::setEventNumber(v1,v2); }
   
  void addRoad(int,const FTKRoad&);
  void inc4LRoad(int,const int& val=1);
  void addSS(int,int,int,const FTKSS&);
  void addSSPlane(int bank,int pl,const std::map<int,FTKSS>& res) {
    m_data[bank]->addSSPlane(pl,res);
  }
  void addUnusedSSMap(int, const std::map<int,std::map<int,FTKSS> >&);

  // cluster/ss information for Naoki's timing simulation
  void naoSetNhitsTot(int bank, int val) { m_data[bank]->naoSetNhitsTot(val); }
  void naoSetNclusTot(int bank, int val) { m_data[bank]->naoSetNclusTot(val); }
  void naoSetNclus(int bank, const std::vector<int>& val) { m_data[bank]->naoSetNclus(val); }
  void naoAddNclus_road(int bank, const std::vector<int>& val) {m_data[bank]->naoAddNclus_road(val); }
  void naoSetNclus_road(int bank, const std::vector<int>& val) { m_data[bank]->naoSetNclus_road(val); }
  void naoSetNss(int bank, const std::vector<int>& val) { m_data[bank]->naoSetNss(val); }
  void naoSetNroadsAM(int bank, int val) { m_data[bank]->naoSetNroadsAM(val); }
  void naoSetNroadsAMComplete(int bank, int val) { m_data[bank]->naoSetNroadsAMComplete(val); }
  void naoSetNroadsAMMissPix(int bank, int val) { m_data[bank]->naoSetNroadsAMMissPix(val); }
  void naoSetNroadsAMMissSCT(int bank, int val) { m_data[bank]->naoSetNroadsAMMissSCT(val); }
  void naoSetNroadsMOD(int bank, int val) { m_data[bank]->naoSetNroadsMOD(val); }
  void naoSetNroadsRW(int bank, int val) { m_data[bank]->naoSetNroadsRW(val); }

};

#endif // FTKROADFILEOUTPUT_H
