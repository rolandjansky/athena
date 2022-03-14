/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITK_PIXEL_RDO_ANALYSIS_H
#define ITK_PIXEL_RDO_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/LockedHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimData/PixelSimHelper.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

class TTree;
class PixelID;
class PixelRDORawData;

namespace InDetDD {
  class PixelDetectorManager;
}

namespace ITk
{

class PixelRDOAnalysis : public AthAlgorithm
{

public:
  PixelRDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

private:
  SG::ReadHandleKey<PixelRDO_Container> m_inputKey {this, "CollectionName", "ITkPixelRDOs", "Input ITk Pixel RDO collection name"};
  SG::ReadHandleKey<InDetSimDataCollection> m_inputTruthKey {this, "SDOCollectionName", "ITkPixelSDO_Map", "Input ITk Pixel SDO collection name"};
  const PixelID *m_pixelID{};
  const InDetDD::PixelDetectorManager *m_pixelManager{};

  Gaudi::Property<std::string> m_histPath {this, "HistPath", "/RDOAnalysis/ITkPixel/", ""};
  Gaudi::Property<std::string> m_sharedHistPath {this, "SharedHistPath", "/RDOAnalysis/histos/", ""};
  Gaudi::Property<std::string> m_ntuplePath {this, "NtuplePath", "/RDOAnalysis/ntuples/", ""};
  Gaudi::Property<std::string> m_ntupleName {this, "NtupleName", "ITkPixel", ""};
  Gaudi::Property<bool> m_doPosition {this, "DoPosition", true, ""};

  ServiceHandle<ITHistSvc> m_thistSvc {this, "HistSvc", "THistSvc", ""};

  // RDO
  std::vector<unsigned long long>* m_rdoID{};
  std::vector<unsigned int>* m_rdoWord{};
  // PixelID
  std::vector<int>* m_barrelEndcap{};
  std::vector<int>* m_layerDisk{};
  std::vector<int>* m_phiModule{};
  std::vector<int>* m_etaModule{};
  std::vector<int>* m_phiIndex{};
  std::vector<int>* m_etaIndex{};
  std::vector<int>* m_isInnermost{};
  std::vector<int>* m_isNextToInnermost{};
  // PixelRDORawData
  std::vector<int>* m_ToT{};
  std::vector<int>* m_BCID{};
  std::vector<int>* m_LVL1A{};
  std::vector<int>* m_LVL1ID{};
  // Global and Local positions
  std::vector<double>* m_globalX{};
  std::vector<double>* m_globalY{};
  std::vector<double>* m_globalZ{};
  std::vector<double>* m_localX{};
  std::vector<double>* m_localY{};
  std::vector<double>* m_localZ{};

  // SDO
  std::vector<unsigned long long>* m_sdoID{};
  std::vector<int>* m_sdoWord{};
  // PixelID
  std::vector<int>* m_barrelEndcap_sdo{};
  std::vector<int>* m_layerDisk_sdo{};
  std::vector<int>* m_phiModule_sdo{};
  std::vector<int>* m_etaModule_sdo{};
  std::vector<int>* m_phiIndex_sdo{};
  std::vector<int>* m_etaIndex_sdo{};
  // PixelSimHelper
  std::vector<bool>* m_noise{};
  std::vector<bool>* m_belowThresh{};
  std::vector<bool>* m_disabled{};
  std::vector<bool>* m_badTOT{};
  // Deposit
  std::vector<int>* m_barcode{};
  std::vector<int>* m_eventIndex{};
  std::vector<int>* m_charge{};
  std::vector< std::vector<int> >* m_barcode_vec{};
  std::vector< std::vector<int> >* m_eventIndex_vec{};
  std::vector< std::vector<float> >* m_charge_vec{};

  // HISTOGRAMS
  TH1* m_h_rdoID{};
  TH1* m_h_rdoWord{};
  TH1* m_h_barrelEndcap{};
  TH1* m_h_layerDisk{};
  TH1* m_h_phiModule{};
  TH1* m_h_etaModule{};
  TH1* m_h_phiIndex{};
  TH1* m_h_etaIndex{};
  TH1* m_h_ToT{};
  TH1* m_h_BCID{};
  TH1* m_h_LVL1A{};
  TH1* m_h_LVL1ID{};
  // barrel pixel
  TH1* m_h_brlLayer{};
  TH1* m_h_brlPhiMod{};
  TH1* m_h_brlEtaMod{};
  TH1* m_h_brlPhiIndex{};
  TH1* m_h_brlEtaIndex{};
  TH1* m_h_brlToT{};
  TH1* m_h_brlBCID{};
  TH1* m_h_brlLVL1A{};
  TH1* m_h_brlLVL1ID{};
  // endcap pixel
  TH1* m_h_ecDisk{};
  TH1* m_h_ecPhiMod{};
  TH1* m_h_ecEtaMod{};
  TH1* m_h_ecPhiIndex{};
  TH1* m_h_ecEtaIndex{};
  TH1* m_h_ecToT{};
  TH1* m_h_ecBCID{};
  TH1* m_h_ecLVL1A{};
  TH1* m_h_ecLVL1ID{};

  TH1* m_h_sdoID{};
  TH1* m_h_sdoWord{};
  TH1* m_h_barrelEndcap_sdo{};
  TH1* m_h_layerDisk_sdo{};
  TH1* m_h_phiModule_sdo{};
  TH1* m_h_etaModule_sdo{};
  TH1* m_h_phiIndex_sdo{};
  TH1* m_h_etaIndex_sdo{};
  TH1* m_h_barcode{};
  TH1* m_h_eventIndex{};
  TH1* m_h_charge{};

  TH1* m_h_belowThresh_brl{};
  TH1* m_h_belowThresh_ec{};

  TH1* m_h_disabled_brl{};
  TH1* m_h_disabled_ec{};

  std::vector<TH1*> m_h_brlinclPhiIndex_perLayer;
  std::vector<TH1*> m_h_brlinclEtaIndex_perLayer;

  std::vector<TH1*> m_h_brlflatPhiIndex_perLayer;
  std::vector<TH1*> m_h_brlflatEtaIndex_perLayer;

  std::vector<TH1*> m_h_ecPhiIndex_perLayer;
  std::vector<TH1*> m_h_ecEtaIndex_perLayer;

  TH1* m_h_phiIndexInnermost{};
  TH1* m_h_etaIndexInnermost{};

  TH1* m_h_phiIndexNextToInnermost{};
  TH1* m_h_etaIndexNextToInnermost{};

  TH2* m_h_globalXY{};
  TH2* m_h_globalZR{};
  LockedHandle<TH2> m_h_globalXY_shared{};
  LockedHandle<TH2> m_h_globalZR_shared{};
  TH1* m_h_globalX{};
  TH1* m_h_globalY{};
  TH1* m_h_globalZ{};

  TH1* m_h_truthMatchedRDOs{};

  TTree* m_tree{};
};

} // namespace ITk

#endif // ITK_PIXEL_RDO_ANALYSIS_H
