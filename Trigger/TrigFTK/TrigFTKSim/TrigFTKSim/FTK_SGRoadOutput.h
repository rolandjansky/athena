/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_SGRoadOutput_H
#define FTK_SGRoadOutput_H

#include "TrigFTKSim/FTKRoadOutput.h"
#include "TrigFTKSim/FTKRoadStream.h"
#include "TrigFTKSim/FTK_SGRoadOutputI.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

class FTK_SGRoadOutput : virtual public FTK_SGRoadOutputI, public AthAlgTool,
			 public FTKRoadOutput {
private:
  int m_nbanks;

  FTKRoadStream **m_data;

  StoreGateSvc*  m_storeGate;

  mutable MsgStream m_log;

  void notValid(const char *);
public:
  FTK_SGRoadOutput(const std::string&, const std::string&, const IInterface*);
  virtual ~FTK_SGRoadOutput();

  StatusCode initialize();
  StatusCode finalize();

  virtual FTKRoadOutput* reference() { return dynamic_cast<FTKRoadOutput*>(this);}

  virtual void setNBanks(int nbanks) { m_nbanks = nbanks;}
  virtual int getNBanks() const { return m_nbanks;}

  virtual void setFileName(const char *) { notValid("setFileName"); }
  virtual const char* getFileName() const { return "NoFile";}
  virtual void setOutDir(const char *) { notValid("setOutDir");}
  virtual const char* getOutDir() const { return "StoreGate";}
  virtual void setMultiOut(bool) { notValid("setMultiOut");}
  virtual bool getMultiOut() const { return false;}
  virtual void init(int bufsize = 0, bool *goodresion=0x0);
  virtual FTKRoadStream* getStream(int id) { return m_data[id];}

  virtual void beginFile(const char *fname=0);
  virtual void eventBegin();
  virtual void eventEnd();
  virtual void endFile();

  virtual unsigned long runNumber(const unsigned int & /*ibank*/) const 
  { return 0;}
  virtual unsigned long eventNumber(const unsigned int & /*ibank*/) const
  { return 0;}
  virtual void setRunNumber(const unsigned int & /*ibank*/,
			    const unsigned long & /*runnum*/) {;}
  virtual void setEventNumber(const unsigned int & /*ibank*/,
			      const unsigned long & /*evtnum*/) {;}

  virtual void addRoad(int, const FTKRoad&);
  virtual void inc4LRoad(int, const int& /*val=1*/) {;}
  virtual void addSS(int,int, int,const FTKSS&);
  virtual void addSSPlane(int, int,const std::map<int,FTKSS>&);
  virtual void addUnusedSSMap(int, const std::map<int,std::map<int,FTKSS> >&);

  // cluster/ss information for Naoki's timing simulation
  virtual void naoAddNclus_road(int, const std::vector<int>&) {;}
  virtual void naoSetNhitsTot(int, int) {;}
  virtual void naoSetNclusTot(int, int) {;}
  virtual void naoSetNclus(int, const std::vector<int>&) {;}
  virtual void naoSetNclus_road(int, const std::vector<int>&) {;}
  virtual void naoSetNss(int, const std::vector<int>&) {;}
  virtual void naoSetNroadsAM(int, int) {;}
  virtual void naoSetNroadsAMComplete(int, int) {;}
  virtual void naoSetNroadsAMMissPix(int, int) {;}
  virtual void naoSetNroadsAMMissSCT(int, int) {;}
  virtual void naoSetNroadsMOD(int, int) {;}
  virtual void naoSetNroadsRW(int, int) {;}
};
#endif // FTK_SGRoadOutput_H
