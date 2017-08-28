/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_RoadMarketIO_h
#define FTK_RoadMarketIO_h

#include "TrigFTKSim/FTKRoadInput.h"
#include "TrigFTKSim/FTKRoadOutput.h"

class FTK_RoadMarketTool;

class FTK_RoadMarketInput : public FTKRoadInput {
private:
  FTK_RoadMarketTool *m_market; // real object if the stream

  FTKRoad **m_cur_road;
  int *m_cur_iroad;

  void notValid(const char *);
public:
  FTK_RoadMarketInput(FTK_RoadMarketTool*);

  virtual int addFile(const char*) { return 0;}
  
  virtual int addFileList(const char *) { return 0;} // add a file with a list of files

  virtual const char* getCurrentFileName() { return "StoreGate"; }

  virtual void setNBanks(int v);
  virtual int getNBanks() const;

  virtual FTKRoadStream* getStream(int id);

  virtual void init(bool */*goodRegions=0x0*/) {;} // nothing special to do

  virtual int nextEvent(); // read the next event
  virtual unsigned long runNumber(const unsigned int& ibank) const;
  virtual unsigned long eventNumber(const unsigned int& ibank) const;

  virtual const FTKRoad* nextRoad(int); // move to the next road and return the pointer
  virtual const FTKRoad* firstRoad(int); // move to the first road and return the pointer
  virtual const FTKSS& getSS(const unsigned int&,const unsigned int&,const unsigned int&) const; // retrieve superstrip by (bank,plane,ssid)
  virtual const FTKSS& getUnusedSS(const unsigned int&,const unsigned int&,const unsigned int&) const; // retrieve superstrip by (bank,plane,ssid)
  virtual FTKRoad* getRoad(int region,int roadid,int bankid); // retrieve road by its indices
  virtual const std::map<int,FTKSS>& getSSPlane(int bank,int pl) const;
};

class FTK_RoadMarketOutput : public FTKRoadOutput {
private:
  FTK_RoadMarketTool *m_market;

  void notValid(const char *);
public:
  FTK_RoadMarketOutput(FTK_RoadMarketTool*);

  virtual void setNBanks(int nbanks);
  virtual int getNBanks() const;

  virtual void setFileName(const char *) { notValid("setFileName"); }
  virtual const char* getFileName() const { return "NoFile";}
  virtual void setOutDir(const char *) { notValid("setOutDir");}
  virtual const char* getOutDir() const { return "InMemory";}
  virtual void setMultiOut(bool) { notValid("setMultiOut");}
  virtual bool getMultiOut() const { return false;}
  virtual void init(int, bool*);
  virtual FTKRoadStream* getStream(int id);

  virtual void beginFile(const char *) {;}
  virtual void eventBegin();
  virtual void eventEnd();
  virtual void endFile() {;}

  virtual unsigned long runNumber(const unsigned int &ibank) const;
  virtual unsigned long eventNumber(const unsigned int &ibank) const;
  virtual void setRunNumber(const unsigned int &ibank,
			    const unsigned long &runnum);
  virtual void setEventNumber(const unsigned int &ibank,
			      const unsigned long &evtnum);

  virtual void addRoad(int, const FTKRoad&);
  virtual void inc4LRoad(int, const int& val=1);
  virtual void addSS(int,int, int,const FTKSS&);
  virtual void addSSPlane(int, int,const std::map<int,FTKSS>&);
  virtual void addUnusedSSMap(int, const std::map<int,std::map<int,FTKSS> >&);

  // cluster/ss information for Naoki's timing simulation
  virtual void naoSetNhitsTot(int, int);
  virtual void naoSetNclusTot(int, int);
  virtual void naoSetNclus(int, const std::vector<int>&);
  virtual void naoSetNclus_road(int, const std::vector<int>&);
  virtual void naoAddNclus_road(int, const std::vector<int>&);
  virtual void naoSetNss(int, const std::vector<int>&);
  virtual void naoSetNroadsAM(int, int);
  virtual void naoSetNroadsAMComplete(int, int);
  virtual void naoSetNroadsAMMissPix(int, int);
  virtual void naoSetNroadsAMMissSCT(int, int);
  virtual void naoSetNroadsMOD(int, int);
  virtual void naoSetNroadsRW(int, int);
};

#endif // FTK_RoadMarketIO_h
