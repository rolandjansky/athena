/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCRDOTORPCPREPDATA_H
#define RPCRDOTORPCPREPDATA_H 

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "IRegionSelector/IRegSelSvc.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
/** Algorithm to decode RDO into RpcPrepData, using the Muon::RpcRdoToRpcPrepDataTool 
 @author Ketevi A. Assamagan
 @author Stefania Spagnolo
*/

class RpcRdoToRpcPrepData : public AthAlgorithm {

public:

    RpcRdoToRpcPrepData(const std::string& name, ISvcLocator* pSvcLocator);

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private:

    // EJWM - where is this implemented? Removing to avoid missing symbol
    //void printRpcPrepRawData(); //!< Prints information about the resultant PRDs.

    ToolHandle< Muon::IMuonRdoToPrepDataTool >    m_tool; //!< Tool used to do actual decoding.

    bool                                    m_print_inputRdo; //!<< If true, will dump information about the inputRDOs.
    bool                                    m_print_prepData; //!<< If true, will dump information about the resulting PRDs.
    bool m_seededDecoding;
    SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
    ServiceHandle<IRegSelSvc> m_regionSelector; //<! pointer to RegionSelectionSvc
    SG::WriteHandleKey<Muon::RpcPrepDataContainer> m_rpcCollection;

};

#endif /// RPCRDOTORPCPREPDATA_H

