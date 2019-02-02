/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOLS_CSCRDOTOCSCPREPDATATOOL_H
#define MUONCSC_CNVTOOLS_CSCRDOTOCSCPREPDATATOOL_H 

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "CSCcabling/CSCcablingSvc.h"

#include <string>

namespace MuonGM {
  class MuonDetectorManager;
}

class CscIdHelper;
class ICscCalibTool;
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
  class IMuonRawDataProviderTool;
  class ICSC_RDO_Decoder;
  class CscRdoToCscPrepDataTool : public AthAlgTool, virtual public IMuonRdoToPrepDataTool {

  public:
    
    CscRdoToCscPrepDataTool(const std::string& type, const std::string& name,
                            const IInterface* parent);
    
    
    /** destructor 
     */ 
    virtual ~CscRdoToCscPrepDataTool();
    
    /** AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID( ) ;
    
    
    StatusCode initialize();
    StatusCode finalize();
    StatusCode decode(std::vector<IdentifierHash>& givenIdhs, std::vector<IdentifierHash>& decodedIdhs);
    //debugging
    void printPrepData();
    void printInputRdo();      
    
  private:
    
    /// private method for the decoding RDO --> PrepRawData
    StatusCode decode(const CscRawDataContainer* rdo, IdentifierHash givenIdh, 
			std::vector<IdentifierHash>& decodedIdhs);
    StatusCode decode(const CscRawDataContainer* rdo, 
		      std::vector<IdentifierHash>& decodedIdhs);
    virtual StatusCode decode( const std::vector<uint32_t>& ) {return StatusCode::FAILURE;}
    
  private:
    
    /// Muon Detector Descriptor
    const MuonGM::MuonDetectorManager * m_muonMgr;
    
    /// CSC identifier helper
    const CscIdHelper * m_cscHelper;
    
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


