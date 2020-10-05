/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCRDOTOCSCPREPDATA_H
#define CSCRDOTOCSCPREPDATA_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelTool.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
/** Algorithm to decode RDO into CscStripPrepData
 - get the RDO container from Storegate
 - loop over the RDO
 - Decode RDO into PrepRawData
 - loop over the PrepRawData and build the PrepRawData container
 - store the PrepRawData container in StoreGate
 @author Ketevi A. Assamagan
 BNL, April 03, 2005
*/
class CscRdoToCscPrepData : public AthAlgorithm {

public:

  CscRdoToCscPrepData(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();

private:
  ToolHandle<Muon::IMuonRdoToPrepDataTool> m_muonRdoToPrepDataTool{this,"CscRdoToCscPrepDataTool","Muon::CscRdoToCscPrepDataTool/CscRdoToPrepDataTool"};
  ToolHandle<IRegSelTool> m_regsel_csc{this,"RegSel_CSC","RegSelTool/RegSelTool_CSC"};

  bool                                    m_print_inputRdo; //!<< If true, will dump information about the input RDOs.  
  bool                                    m_print_prepData; //!<< If true, will dump information about the resulting PRDs.  
  bool m_seededDecoding;
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
  SG::WriteHandleKey<Muon::CscStripPrepDataContainer> m_cscCollection;
};

#endif /// CSCRDOTOCSCPREPDATA_H
