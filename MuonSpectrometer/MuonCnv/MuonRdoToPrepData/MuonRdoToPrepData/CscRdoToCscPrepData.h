/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCRDOTOCSCPREPDATA_H
#define CSCRDOTOCSCPREPDATA_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"
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
  StatusCode finalize();

private:

  ToolHandle< Muon::IMuonRdoToPrepDataTool >    m_muonRdoToPrepDataTool;
  bool                                    m_print_inputRdo; //!<< If true, will dump information about the input RDOs.  
  bool                                    m_print_prepData; //!<< If true, will dump information about the resulting PRDs.  
  bool m_seededDecoding;
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
  ServiceHandle<IRegSelSvc> m_regionSelector; //<! pointer to RegionSelectionSvc
  SG::WriteHandleKey<Muon::CscStripPrepDataContainer> m_cscCollection;
};

#endif /// CSCRDOTOCSCPREPDATA_H

/*
#ifndef CSCRDOTOCSCPREPDATA_H
#define CSCRDOTOCSCPREPDATA_H 

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
//#include "CscCalibTools/ICscCalibTool.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"

#include <string>

class MsgStream;
*/
/** Algorithm to decode RDO into CscStripPrepData
 - get the RDO container from Storegate
 - loop over the RDO
 - Decode RDO into PrepRawData
 - loop over the PrepRawData and build the PrepRawData container
 - store the PrepRawData container in StoreGate
 @author Ketevi A. Assamagan
 BNL, April 03, 2005
*/
/*
class CscRdoToCscPrepData : public Algorithm {

 public:

  CscRdoToCscPrepData(const std::string& name, ISvcLocator* pSvcLocator);
    
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

    ToolHandle< Muon::IMuonRdoToPrepDataTool >    m_tool; //!< Tool used to do actual decoding.

    std::string                             m_outputCollectionLocation;

    bool                                    m_print_prepData; //!<< If true, will dump information about the resulting PRDs.

    MsgStream                               m_log;

    bool                                    m_debug; //!< Set in initialize(). If true, output DEBUG info
    bool                                    m_verbose; //!< Set in initialize(). If true, output VERBOSE info
};

#endif /// CSCRDOTOCSCPREPDATA_H
*/

