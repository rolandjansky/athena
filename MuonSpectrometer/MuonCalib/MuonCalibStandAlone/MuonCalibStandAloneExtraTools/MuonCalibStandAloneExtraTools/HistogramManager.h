/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 13.08.2008, AUTHOR: MAURO IODICE
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
#ifndef SRC_HISTOGRAMMANAGER_H
#define SRC_HISTOGRAMMANAGER_H

#include "TMath.h"
#include <string.h>
#include <stdio.h>

#include <sstream>
#include <iostream>
#include <fstream> 

#include <TFile.h>
#include <TString.h>
#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TObjArray.h"

#include "MuonCalibStandAloneExtraTools/StringUtil.h"
#include "MuonCalibStandAloneExtraTools/MDTName.h"

#define UPDATETIME 2000

//using namespace std;
//using namespace MuonCalib;

class MdtIdHelper;

class MdtChamber {
public:
  std::string m_region;
  std::string m_side;
  int m_sector;
  int m_absEta;
  int m_eta_id;
  int m_phi_id;
  std::string m_chamberType;
  std::string m_chamberName;
  std::string m_athenaChamberName;

  void Fill(std::string region, std::string side, int sector, std::string chamberType, int absEta_id) {
    ToString ts;
    std::string sectorString=ts(sector);
    if(sector<10) sectorString="0"+sectorString;
    m_region = region;
    m_side = side;
    m_sector = sector;
    m_chamberType = chamberType;
    m_absEta = absEta_id;
    int eta_id = absEta_id;
    if (side=="C") eta_id = -absEta_id;
    m_eta_id = eta_id;
    int phi_id = (int) (sector+1)/2;
    m_phi_id = phi_id;
    std::string chName = chamberType+ts(absEta_id)+side+sectorString;
    m_chamberName = chName;
    std::string chAthenaName = chamberType+"_"+ts(phi_id)+"_"+ts(eta_id);
    m_athenaChamberName = chAthenaName;
    return;
  }
};

struct sortMdtChambersByName {
  bool operator()(MuonCalib::MDTName a ,MuonCalib::MDTName b) {
    std::string t1=a.getOnlineName();
    std::string t2=b.getOnlineName();
    return (t1 < t2);
  }
};

class HistogramManager {
public:
  HistogramManager();
  HistogramManager(const MdtIdHelper *mdtIdHelper);
  ~HistogramManager();
  void buildGlobalHistos();
  void buildTrackHistos(); 
  void buildDebugHistos();
  void buildTopLevel(std::string region, std::string side, int sectorMin, int sectorMax);
  void buildSector(std::string region,std::string side, int sector);
  void buildChamberHistos(MuonCalib::MDTName);

  void setChamberCutOut(std::string chamber, TH1F *href );
  void setChamberDisconnectedTubes(std::string chamber, TH1F *href );
  int  GetTubeOffsetML1(std::string chamberName);
  int  GetTubeOffsetAtEndML1(std::string chamberName);

  std::vector<MuonCalib::MDTName> GetChamberList(std::string region, std::string side, int sector);

  bool openOutputFile(std::string filename);
  bool openReadOnlyFile(std::string filename);
  bool openUpdateFile(std::string filename);
  void WriteAndCloseFile();  

  void SetDoTracks(bool);

  TObject * GetMdtHisto(std::string histo_name);
  TObject * GetMdtHisto(std::string histo_name, std::string region, std::string side);
  TObject * GetMdtHisto(std::string histo_name, std::string region, std::string side, int sector);
  TObject * GetMdtHisto(std::string histo_name, MuonCalib::MDTName);

  std::string GetMdtDirectoryName(); 
  std::string GetMdtDirectoryName(std::string region, std::string side);
  std::string GetMdtDirectoryName(std::string region, std::string side, int sector);
  std::string GetMdtDirectoryName(std::string region, std::string side, int sector, std::string chamberType, int eta);
  std::string GetMdtDirectoryName(MuonCalib::MDTName); 

  TObject * GetTDaqHisto(std::string histo_name, std::string region);
  TObject * GetTDaqHisto(std::string histo_name, std::string region, std::string side);
  TObject * GetTDaqHisto(std::string histo_name, std::string region, std::string side, int sector);
  TObject * GetTDaqHisto(std::string histo_name, std::string region, std::string side, int sector, std::string chamberType, int eta);

  std::string GetTDaqDirectoryName(std::string region); 
  std::string GetTDaqDirectoryName(std::string region, std::string side);
  std::string GetTDaqDirectoryName(std::string region, std::string side, int sector);
  std::string GetTDaqDirectoryName(std::string region, std::string side, int sector, std::string chamberType, int eta);

  TObject* GetHisto(std::string main_dir, std::string histo_name);

  void ReadChamberMapFile (std::string chamberName, int *chamberGeoParams, int numParams);
  int GetChamberNumOfML (std::string chamberName);
  int GetChamberNumOfMezzPerML (std::string chamberName);
  int GetChamberTubesPerMezz (std::string chamberName);

  // std::vector<std::string> GetChamberTypeList(std::string region, std::string side, int sector);
  int GetEtaMax(std::string region, std::string side, int sector, std::string chamberType);

  // void buildAll(int sectorMin, int sectorMax);
  // void test();
 
  TFile* rootFile(){return m_rootfile;};
  TFile *m_rootfile;

 private:
  TObjArray m_hList;
  const MdtIdHelper * m_MdtIdHelper;

  bool m_doTracks;

  // ToString ts;
  // ToChar _tc;
  
};

////////////////////////////////////////////////////////////////////
//
// UTILITIES of General usage
//

/*********************
//
class ToString{
  public:
  template< class T >
    std::string operator()( const T& i )
  {
    std::ostringstream os;
    os << i;
    return os.str();
  }
};
*********************************/


/*********************************
class ToChar {
 public:
    char* operator()( const std::string i )
  {
    return i.c_str();
  }
    char* operator()( const int i )
  {
    ToString ts;
    return ts(i).c_str();
  }
    char* operator()( const float i )
  {
    ToString ts;
    return ts(i).c_str();
  }
    char* operator()( const char * i )
  {
    return i;
  }

};


*********************************/

#endif //SRC_HISTOMANAGER_H

