/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_TRT_RDOSTESTTOOL
#define G4AT_TRT_RDOSTESTTOOL

#include "DigiTestToolBase.h"

class TRT_ID;
class TRT_RDORawData;

class TRT_RDOsTestTool : public DigiTestToolBase {

public:

  TRT_RDOsTestTool(const std::string& name,
                   const std::string& type,
                   const IInterface* parent);

  StatusCode initialize();

  StatusCode processEvent();

  StatusCode finalize();

 private:
  int trailingEdge(const TRT_RDORawData* r) const;
  bool firstBinHigh(const TRT_RDORawData* r) const;
  bool lastBinHigh(const TRT_RDORawData* r) const;
  StatusCode CheckSDOs();
  double GetBarrelOccupancy(const TH2* hist, const int layer, const double basescale, double& error);
  double GetEndcapOccupancy(const TH2* hist, const int disk, const double basescale, double& error);
  const TRT_ID *m_trtID;        //!<  Pointer to the TRT ID helper
  unsigned int m_numberOfEventsSelected;

  std::string m_collection;
  // TRT histograms
  TH1 *m_nRDO_Colls;
  TH1 *m_nEmptyRDO_Colls;
  TH1 *m_BarrelEndcap;
  TH1 *m_BarrelLayer;
  TH1 *m_EndcapLayer;
  TH1 *m_BarrelStrawLayer;
  TH1 *m_EndcapStrawLayer;
  TH1 *m_BarrelPhiModule;
  TH1 *m_EndcapPhiModule;
  TH1 *m_BarrelStraw;
  TH1 *m_EndcapStraw;
  TH1 *m_BarrelTimeOverThreshold;
  TH1 *m_EndcapTimeOverThreshold;
  TH1 *m_BarrelDriftTimeBin;
  TH1 *m_EndcapDriftTimeBin;
  TH1 *m_BarrelTrailingEdge;
  TH1 *m_EndcapTrailingEdge;
//   TH1 *m_BarrelOccupancyByLayer;
//   TH2 *m_h_trt_barrel_occ_layer[4];
//   TH1 *m_EndCapA_OccupancyByDisk;
//   TH2 *m_h_trt_endcapA_occ_layer[9];
//   TH1 *m_EndCapC_OccupancyByDisk;
//   TH2 *m_h_trt_endcapC_occ_layer[9];
//   TH1 *m_TRT_OccupancyByModuleType;
//   std::map<int,int> NoBarrelModules;
//   std::map<int,int> NoInnerModules;
//   std::map<int,int> NoMiddleModules;
//   std::map<int,int> NoShortMiddleModules;
//   std::map<int,int> NoOuterModules;
//   double m_NumberModulesVetoed[5];

};

#endif
