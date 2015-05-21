/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonDetectorFactory001_H
#define MuonDetectorFactory001_H

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "GeoModelKernel/GeoVDetectorFactory.h"

#include <string>

class GeoPhysVol;
class StoreGateSvc;
class IRDBAccessSvc;

namespace MuonGM {

class MuonSystemDescription;


class MuonDetectorFactory001: public GeoVDetectorFactory {

typedef std::map<std::string,std::string> AltAsciiDBMap;

public:

  // Constructor:
  MuonDetectorFactory001( StoreGateSvc *pDetStore );

  // Destructor:
  ~MuonDetectorFactory001();

  // Creates the raw geometry tree: required,
  virtual void create (GeoPhysVol* world);

  // Get the manager.
  virtual MuonDetectorManager* getDetectorManager() const;

  inline void setDBAtlasVersion(std::string v);
  inline void setDBMuonVersion(std::string v);
  inline void setDBkey(std::string v);
  inline void setDBnode(std::string v);
    
  inline void setLayout(std::string );
  inline void setCutoutsFlag(int );
  inline void setCutoutsBogFlag(int );
  inline void setCtbBisFlag(int );
  inline void setRDBAccess(IRDBAccessSvc* access);    
  inline void setAltAsciiDBMap(const AltAsciiDBMap asciidbmap);
  inline void setUseRDB(int rdb);    
  inline void setControlAlines(int cA);
  inline void setMinimalGeoFlag(int minimalGeo);
  inline void setControlCscIntAlines(int cA);
  inline void setDumpAlines(bool cA);
  inline void setDumpCscIntAlines(bool cA);
  inline void setUseCscIntAlinesFromGM(bool cA);
  inline void setSelection(std::vector<std::string>, std::vector<int>, std::vector<int>);
  inline void setCachingFlag(int value);
  inline void setDumpMemoryBreakDown(bool value);  
  inline void setCacheFillingFlag(int value);
  inline void setMdtDeformationFlag(int mdtDeformationFlag);
  inline void setMdtAsBuiltParaFlag(int mdtAsBuiltParaFlag);
  inline void setFineClashFixingFlag(int value);
    
    
private:

  std::string m_DBAtlasVersion;
  std::string m_DBMuonVersion;
  std::string m_DBkey;
  std::string m_DBnode;

  std::string m_layout;
  int m_includeCutouts;
  int m_includeCutoutsBog;
  int m_includeCtbBis;
  int m_rdb;
  int m_controlAlines;
  int m_minimalGeoFlag;
  int m_controlCscIntAlines;
  bool m_dumpAlines;
  bool m_dumpCscIntAlines;
  bool m_useCscIntAlinesFromGM;
    
  int m_caching;
  int m_cacheFillingFlag;
  int m_mdtDeformationFlag;
  int m_mdtAsBuiltParaFlag;
  bool m_dumpMemoryBreakDown;  
  int m_enableFineClashFixing;
    
  std::vector<std::string> m_selectedStations;
  std::vector<int> m_selectedStEta;
  std::vector<int> m_selectedStPhi;

  MuonSystemDescription* m_muon;
  MuonDetectorManager*   m_manager;
  StoreGateSvc*          m_pDetStore;
  IRDBAccessSvc*         m_pRDBAccess;
  AltAsciiDBMap          m_altAsciiDBMap;

};

void MuonDetectorFactory001::setDBAtlasVersion(std::string v) {m_DBAtlasVersion = v;}
void MuonDetectorFactory001::setDBMuonVersion(std::string v) {m_DBMuonVersion = v;}    
void MuonDetectorFactory001::setDBkey(std::string v) {m_DBkey = v;}
void MuonDetectorFactory001::setDBnode(std::string v) {m_DBnode = v;}

void MuonDetectorFactory001::setLayout(std::string str) {m_layout = str;}
void MuonDetectorFactory001::setCutoutsFlag(int flag) {m_includeCutouts = flag;}
void MuonDetectorFactory001::setCutoutsBogFlag(int flag) {m_includeCutoutsBog = flag;}
void MuonDetectorFactory001::setCtbBisFlag(int flag) {m_includeCtbBis = flag;}
void MuonDetectorFactory001::setUseRDB(int rdb) {m_rdb = rdb;}
void MuonDetectorFactory001::setRDBAccess(IRDBAccessSvc* access) {m_pRDBAccess = access;}
void MuonDetectorFactory001::setAltAsciiDBMap(AltAsciiDBMap asciidbmap) { std::swap(m_altAsciiDBMap,asciidbmap); }
void MuonDetectorFactory001::setControlAlines(int cA) {m_controlAlines = cA;}
void MuonDetectorFactory001::setMinimalGeoFlag(int minimalGeo) {m_minimalGeoFlag = minimalGeo;}
void MuonDetectorFactory001::setControlCscIntAlines(int cA) {m_controlCscIntAlines = cA;}
void MuonDetectorFactory001::setDumpAlines(bool dumpAlines) {m_dumpAlines = dumpAlines;}    
void MuonDetectorFactory001::setDumpCscIntAlines(bool dumpAlines) {m_dumpCscIntAlines = dumpAlines;}    
void MuonDetectorFactory001::setUseCscIntAlinesFromGM(bool useIlinesFromGM) {m_useCscIntAlinesFromGM = useIlinesFromGM;}   

void MuonDetectorFactory001::setSelection(std::vector<std::string> vst, 
                                     std::vector<int> veta, 
                                     std::vector<int> vphi)
{
  m_selectedStations = vst; m_selectedStEta = veta; m_selectedStPhi = vphi;
}

void MuonDetectorFactory001::setCachingFlag(int value){m_caching = value;}
void MuonDetectorFactory001::setCacheFillingFlag(int value){m_cacheFillingFlag = value;}
void MuonDetectorFactory001::setMdtDeformationFlag(int value){m_mdtDeformationFlag = value;}
void MuonDetectorFactory001::setMdtAsBuiltParaFlag(int value){m_mdtAsBuiltParaFlag = value;}
void MuonDetectorFactory001::setDumpMemoryBreakDown(bool value){m_dumpMemoryBreakDown = value;}
void MuonDetectorFactory001::setFineClashFixingFlag(int value){m_enableFineClashFixing = value;}
    
    
} // namespace MuonGM

#endif
