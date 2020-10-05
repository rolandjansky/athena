/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

  // this function is inherited from GeoVDetectorFactory where it is declared const.
  // However, the MuonDetectorManager cannot be const since it holds the pointers to the readout elements,
  // and those can change with alignment. Thus, this const method will create a thread-safety warning since 
  // the returned object is *not* const
  virtual MuonDetectorManager* getDetectorManager() const;

  inline void setDBAtlasVersion(std::string v);
  inline void setDBMuonVersion(std::string v);
  inline void setDBkey(std::string v);
  inline void setDBnode(std::string v);

  inline void setAmdcDb(bool value);
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
  inline void hasCSC(bool value);
  inline void hasSTgc(bool value);
  inline void hasMM(bool value);
  inline void setABLinesAsciiSideA(const std::string& );
  inline void setABLinesAsciiSideC(const std::string& );

private:

  std::string m_DBAtlasVersion;
  std::string m_DBMuonVersion;
  std::string m_DBkey;
  std::string m_DBnode;

  bool m_isAmdcDb;
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
  bool m_hasCSC;
  bool m_hasSTgc;
  bool m_hasMM;

  std::string m_NSWABLinesSideA;
  std::string m_NSWABLinesSideC;

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

void MuonDetectorFactory001::setAmdcDb(bool value) {m_isAmdcDb = value;}
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
void MuonDetectorFactory001::hasCSC(bool value){m_hasCSC = value;}
void MuonDetectorFactory001::hasSTgc(bool value){m_hasSTgc = value;}
void MuonDetectorFactory001::hasMM(bool value){m_hasMM = value;}
void MuonDetectorFactory001::setABLinesAsciiSideA(const std::string& str) {m_NSWABLinesSideA = str; }
void MuonDetectorFactory001::setABLinesAsciiSideC(const std::string& str) {m_NSWABLinesSideC = str; }


} // namespace MuonGM

#endif
