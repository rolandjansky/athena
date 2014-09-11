/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_PIXELRDOSTESTTOOL
#define G4AT_PIXELRDOSTESTTOOL

#include "DigiTestToolBase.h"

class PixelID;

class PixelRDOsTestTool : public DigiTestToolBase {

public:

  PixelRDOsTestTool(const std::string& name,
                    const std::string& type,
                    const IInterface* parent);

  StatusCode initialize();

  StatusCode processEvent();

  StatusCode finalize();

 private:
  StatusCode CheckSDOs();
  double GetBarrelOccupancy(const TH2* hist, const int layer, const double basescale, double& error);
  double GetEndcapOccupancy(const TH2* hist, const int disk, const double basescale, double& error);
  const PixelID *m_pixelID;        //!<  Pointer to the Pixel ID helper
  unsigned int m_numberOfEventsSelected;
  std::string m_collection;
  // Pixel histograms
  TH1 *m_nRDO_Colls;
  TH1 *m_nEmptyRDO_Colls;
  TH1 *m_BarrelEndcap;
  TH1 *m_BarrelLayer;
  TH1 *m_EndcapLayer;
//   TH1 *m_PixelSide;
  TH1 *m_BarrelEtaModule;
  TH1 *m_EndcapEtaModule;
  TH1 *m_BarrelPhiModule;
  TH1 *m_EndcapPhiModule;
//   TH1 *m_BarrelRDOClusterSize;
//   TH1 *m_EndcapRDOClusterSize;
  TH1 *m_BarrelToT;
  TH1 *m_EndcapToT;
  TH1 *m_BarrelBCID;
  TH1 *m_EndcapBCID;
  TH1 *m_BarrelLVL1A;
  TH1 *m_EndcapLVL1A;
  TH1 *m_BarrelLVL1D;
  TH1 *m_EndcapLVL1D;

  TH1 *m_BarrelOccupancyByLayer;
  TH2 *m_h_pixel_barrel_occ_layer[3];
  TH1 *m_EndCapA_OccupancyByDisk;
  TH2 *m_h_pixel_endcapA_occ_layer[5];
  TH1 *m_EndCapC_OccupancyByDisk;
  TH2 *m_h_pixel_endcapC_occ_layer[5];
//   TH1 *m_PixelOccupancyByModuleType;
//   std::map<int,int> NoBarrelModules;
//   std::map<int,int> NoInnerModules;
//   std::map<int,int> NoMiddleModules;
//   std::map<int,int> NoShortMiddleModules;
//   std::map<int,int> NoOuterModules;
//   double m_NumberModulesVetoed[5];

};

#endif
