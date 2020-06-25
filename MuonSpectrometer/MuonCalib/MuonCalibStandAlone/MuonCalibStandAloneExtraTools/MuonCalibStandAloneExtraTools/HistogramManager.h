/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SRC_HISTOGRAMMANAGER_H
#define SRC_HISTOGRAMMANAGER_H

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
#include "MuonIdHelpers/MdtIdHelper.h"

#define UPDATETIME 2000

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
  HistogramManager(const Muon::IMuonIdHelperSvc *idHelper);
  ~HistogramManager()=default;
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

  int GetEtaMax(std::string region, std::string side, int sector, std::string chamberType);
 
  TFile* rootFile(){return m_rootfile;};
  TFile *m_rootfile;

 private:
  TObjArray m_hList;
  const MdtIdHelper* m_mdtIdHelper;

  bool m_doTracks;
  
};

#endif //SRC_HISTOMANAGER_H

