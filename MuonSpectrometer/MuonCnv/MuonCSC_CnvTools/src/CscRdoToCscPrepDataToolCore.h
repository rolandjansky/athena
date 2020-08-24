/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOLS_CSCRDOTOCSCPREPDATATOOLCORE_H
#define MUONCSC_CNVTOOLS_CSCRDOTOCSCPREPDATATOOLCORE_H 

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "CSCcabling/CSCcablingSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "CscCalibTools/ICscCalibTool.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include <string>
#include <vector>

class CscRawDataContainer;

////////////////////////////////////////////////////////////////////////////////////////
/// Author: Ketevi A. Assamagan
/// BNL, April 03, 2005
///
/// algorithm to decode RDO into CscStripPrepData
/// get the RDO container from Storegate
/// loop over the RDO
/// Decode RDO into PrepRawData
/// loop over the PrepRawData and build the PrepRawData container
/// store the PrepRawData container in StoreGate
////////////////////////////////////////////////////////////////////////////////////////

namespace Muon {

  /// This class is only used in a single-thread mode as CscRdoToCscPrepDataToolMT has the 
  /// equivalent functions defined for a thread-safe setup
  class CscRdoToCscPrepDataToolCore : public AthAlgTool, virtual public IMuonRdoToPrepDataTool {

  public:
    
    CscRdoToCscPrepDataToolCore(const std::string& type, const std::string& name, const IInterface* parent);

    virtual ~CscRdoToCscPrepDataToolCore()=default;
    
    /** AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID( ) ;
    
    
    StatusCode initialize();
    virtual StatusCode decode(std::vector<IdentifierHash>& givenIdhs, std::vector<IdentifierHash>& decodedIdhs);
    //debugging
    void printPrepData();
    void printInputRdo();      
    
  protected:
    
    /// private method for the decoding RDO --> PrepRawData
    virtual StatusCode decode(const CscRawDataContainer* rdo, IdentifierHash givenIdh, 
			std::vector<IdentifierHash>& decodedIdhs);
    virtual StatusCode decode(const CscRawDataContainer* rdo, 
		      std::vector<IdentifierHash>& decodedIdhs);
    virtual StatusCode decode( const std::vector<uint32_t>& ) {return StatusCode::FAILURE;}

    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muDetMgrKey {this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"}; 
    
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    
    /// CscStripPrepRawData containers
    Muon::CscStripPrepDataContainer* m_outputCollection;
    SG::WriteHandleKey<Muon::CscStripPrepDataContainer> m_outputCollectionKey;
 
    SG::ReadHandleKey<CscRawDataContainer> m_rdoContainerKey;
    
    /// CSC Calibration tools
    ToolHandle<ICscCalibTool> m_cscCalibTool;
    ToolHandle<ICSC_RDO_Decoder> m_cscRdoDecoderTool;

    ServiceHandle<CSCcablingSvc> m_cabling;
    /// Identifier hash offset
    int m_cscOffset;
    
    bool m_decodeData; //!< toggle on/off the decoding of CSC RDO into CscStripPrepData
    bool m_fullEventDone;
  };
}
#endif /// MUONCSC_CNVTOOL_CSCRDOTOCSCPREPDATA_H


