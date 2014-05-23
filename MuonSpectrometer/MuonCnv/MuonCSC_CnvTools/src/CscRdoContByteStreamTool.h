/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOLS_CSCRDOCONTRAWEVENTTOOL_H
#define MUONCSC_CNVTOOLS_CSCRDOCONTRAWEVENTTOOL_H

#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 

#include "MuonCSC_CnvTools/ICSC_RDOtoByteStreamTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "CSC_Hid2RESrcID.h"

#include "ByteStreamData/RawEvent.h" 
#include "MuonIdHelpers/CscIdHelper.h"
#include <string>

class Identifier;
class CscIdHelper;
class CSCcablingSvc;
class CscRawDataCollection;


namespace Muon {
  /*
    An AlgTool to provide conversion between CSC RDO <---> ByteStream,
    and fill it in RawEvent.
    
    @author Ketevi A. Assamagan BNL December 27 2003
  */
  class CscRdoContByteStreamTool : public AthAlgTool, virtual public ICSC_RDOtoByteStreamTool {

  public:
    
    typedef CscRawDataContainer CONTAINER;
    
    /** constructor
     */
    CscRdoContByteStreamTool (const std::string& type, const std::string& name,
                              const IInterface* parent);
    
    /** destructor 
     */ 
    virtual ~CscRdoContByteStreamTool();
    
    /** AlgTool InterfaceID
     */
    //    static const InterfaceID& interfaceID( ) ;
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    /** to read the cosmic data */ 
    bool isCosmic () const { return m_isCosmic; }
    
    /** for the old cosmic data before the ROB id=ROD id fix */
    bool isOldCosmic () const { return m_isOldCosmic; }
    
    StatusCode convert(const CONTAINER* cont, RawEventWrite* re, MsgStream& log);
    //StatusCode convert(const RawEvent* re, CONTAINER* cont, MsgStream& log);
    
  private: 
    
    CSC_Hid2RESrcID m_hid2re;
    const CscIdHelper * m_cscHelper; 
    
    // event assembler
    FullEventAssembler<CSC_Hid2RESrcID> m_fea;    
    
    uint32_t m_eventType;
    uint16_t m_rodVersion;
    
    CSCcablingSvc * m_cabling;
    
    bool m_isCosmic;
    bool m_isOldCosmic;
    
  };
}

#endif



