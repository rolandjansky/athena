/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_SCT_RDOSTESTTOOL
#define G4AT_SCT_RDOSTESTTOOL

#include "DigiTestToolBase.h"

class SCT_ID;
//class ISiliconConditionsSvc;
//class ISiPropertiesSvc;

class SCT_RDOsTestTool : public DigiTestToolBase {

public:

  SCT_RDOsTestTool(const std::string& name,
         const std::string& type,
         const IInterface* parent);

  StatusCode initialize();

  StatusCode processEvent();

  StatusCode finalize();

 private:
  StatusCode CheckSDOs();
  double GetBarrelOccupancy(const TH2* hist, const int layer, const double basescale, double& error);
  double GetEndcapOccupancy(const TH2* hist, const int disk, const double basescale, double& error);
  //ServiceHandle<ISiliconConditionsSvc> m_siConditionsSvc;
  //ServiceHandle<ISiPropertiesSvc> m_siPropertiesSvc;
  //const InDetDD::SCT_DetectorManager *m_sctmgr; //!< Pointer to the SCT detector manager
  const SCT_ID *m_sctID;        //!<  Pointer to the SCT ID helper
  unsigned int m_numberOfEventsSelected;

  std::string m_collection;
  // SCT histograms
  TH1 *m_nRDO_Colls;
  TH1 *m_nEmptyRDO_Colls;
  TH1 *m_BarrelEndcap;
  TH1 *m_BarrelLayer;
  TH1 *m_EndcapLayer;
  TH1 *m_SCT_Side;
  TH1 *m_BarrelEtaModule;
  TH1 *m_EndcapEtaModule;
  TH1 *m_BarrelPhiModule;
  TH1 *m_EndcapPhiModule;
  TH1 *m_BarrelStripNumber;
  TH1 *m_EndcapStripNumber;
  TH1 *m_BarrelRDOClusterSize;
  TH1 *m_EndcapRDOClusterSize;
  TH1 *m_BarrelOccupancyByLayer;
  TH2 *m_h_sct_barrel_occ_layer[4];
  TH1 *m_EndCapA_OccupancyByDisk;
  TH2 *m_h_sct_endcapA_occ_layer[9];
  TH1 *m_EndCapC_OccupancyByDisk;
  TH2 *m_h_sct_endcapC_occ_layer[9];
  TH1 *m_SCT_OccupancyByModuleType;
  std::map<int,int> m_NoBarrelModules;
  std::map<int,int> m_NoInnerModules;
  std::map<int,int> m_NoMiddleModules;
  std::map<int,int> m_NoShortMiddleModules;
  std::map<int,int> m_NoOuterModules;
  double m_NumberModulesVetoed[5];

};

#endif
