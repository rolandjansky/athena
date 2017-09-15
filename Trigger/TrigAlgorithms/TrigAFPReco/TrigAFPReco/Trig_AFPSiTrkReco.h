/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIG_AFPSITRKRECO_H
#define TRIG_AFPSITRKRECO_H 1

#include <string>

#include "TrigInterfaces/FexAlgo.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AFP_ByteStream2RawCnv/AFP_RawDataProviderTool.h"
#include "AFP_LocRecoInterfaces/IAFPSiDLocRecoTool.h"
#include "AFP_Raw2Digi/IAFP_Raw2DigiTool.h"

class ISvcLocator;

/**
 * @brief Class for the AFP FEX Algorithm which reconstructs tracks
 *
 * This class does the reconstruction of AFP tracks, 
 * starting with the raw data reconstruction,
 * then reconstructing Si hits and, with that, AFP tracks
 */

class Trig_AFPSiTrkReco: public HLT::FexAlgo {

public:
   /// Sets the tools and services needed to their default values, as well as the containers names that can be defined in the Python class 
  Trig_AFPSiTrkReco(const std::string& name, ISvcLocator* pSvcLocator);
   /// Class destructor. Not doing anything at the moment
  virtual ~Trig_AFPSiTrkReco();

   /// Initialize method, where AFP_RawDataProviderTool, AFP_Raw2DigiTool, AFPSiDLocRecoTool and ROBDataProviderSvc are retrieved
  HLT::ErrorCode hltInitialize();
  /** 
   * @brief Execute method where the tools are called and the reconstruction is made
   *
   * In this method the following steps are done:
   * - Record AFP_RawDataContainer that will be later filled
   * - Retrieve ROB data through the ROBDataProviderSvc;
   * - Reconstruct raw data from ROB data using AFP_RawDataProviderTool
   * - Reconstruct Silicon hits from raw data using AFP_Raw2DigiTool
   * These last steps are only taken if the evtStore does not find a SiHitContainer. Afterwards, if no AFPTrackContainer is found:
   * - Reconstruct AFP tracks from Silicon hits using AFPSiDLocRecoTool
   */

  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
   /// Finalize method. Nothing is done, except for a print statement in Debug stream
  HLT::ErrorCode hltFinalize();
private:
  
   /// Service used by AFP_RawDataProviderTool to retrieve ROB data. Inside package ByteStreamCnvSvcBase
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
   /// Tool to reconstruct raw data from ROB. Inside package AFP_ByteStream2RawCnv 
  ToolHandle<AFP_RawDataProviderTool> m_rawDataTool;
   /// Tool to reconstruct Si hist from raw data. Inside package AFP_Raw2Digi 
  ToolHandle<IAFP_Raw2DigiTool> m_digiTool;
   /// Tool to reconstruct AFP tracks from Si hits. Inside package AFP_LocRecoInterfaces 
  ToolHandle<IAFPSiDLocRecoTool> m_trackRecoTool;
   /// Name of the raw data container used to save to storegate, which is then used by AFP_RawDataProviderTool. Default: "AFP_RawData"
  std::string m_rawDataCollectionKey;
  /// Name of the container with silicon detector hits container in storegate. Used both to save reconstructed hits and to them in input to track reconstruction. Default: "AFPSiHitContainer"
  std::string m_siHitContainerName;
   /// Name of the AFP tracks container that will be stored in storegate to be used by the hypothesis algorithm. Default: "AFPTrackContainer"
  std::string m_trackContainerName;

  static const std::vector<unsigned int> s_robIDs;
};

#endif
