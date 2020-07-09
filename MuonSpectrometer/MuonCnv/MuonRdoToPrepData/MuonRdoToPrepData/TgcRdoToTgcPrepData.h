/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRDOTOTGCPREPDATA_H
#define TGCRDOTOTGCPREPDATA_H 

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"


#include <string>
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelTool.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

/** Algorithm to decode RDO into TgcPrepData, using the Muon::TgcRdoToTgcPrepDataTool 
 @author Takashi Kubota (Takashi.Kubota@cern.ch)
*/

class TgcRdoToTgcPrepData : public AthAlgorithm {

 public:

  TgcRdoToTgcPrepData(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  void printTgcPrepRawData(); //!< Prints information about the resultant PRDs.

  ToolHandle< Muon::IMuonRdoToPrepDataTool >    m_tool; //!< Tool used to do actual decoding.

  bool                                    m_print_inputRdo; //!<< If true, will dump information about the input RDOs.
  bool                                    m_print_prepData; //!<< If true, will dump information about the resulting PRDs.

  int                                     m_setting; //!< Setting for debugging
  std::vector<int>                        m_settingVector; //!< Setting for debugging

  bool m_seededDecoding;
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
  ToolHandle<IRegSelTool> m_regsel_tgc; //<! pointer to RegionSelectionTool
  SG::WriteHandleKey<Muon::TgcPrepDataContainer> m_tgcCollection;
};

#endif /// TGCRDOTOTGCPREPDATA_H
