/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CNVTOOLS_MDTCSMCONTRAWEVENTTOOL_H
#define MUONMDT_CNVTOOLS_MDTCSMCONTRAWEVENTTOOL_H

#include <stdint.h>
#include <map>
#include "AthenaBaseComps/AthAlgTool.h"
#include "MDT_Hid2RESrcID.h"

#include "ByteStreamData/RawEvent.h" 

#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 

#include "MuonMDT_CnvTools/IMDT_RDOtoByteStreamTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"


class MdtCsmContainer; 
class MsgStream;

#include <string>

namespace Muon {
/** An AlgTool class to provide conversion from LArRawChannelContainer
  *  to ByteStream, and fill it in RawEvent
  *  created:  Sept 25, 2002, by Hong Ma 
  *  requirements:   typedef for CONTAINER class method 
  *  statusCode convert(CONTAINER* cont, RawEvent* re, MsgStream& log ); 
  *
  * Adapted for Muons by Ketevi A. Assamagan
  * Jan-14-2003, BNL
  * Conversion of Rpc Pad Container to byte stream
  * 
  * Adapted for MDTs ny Michela Biglietti
  * March 2003, Naples
  * Conversion of a Mdt Csm to ByteStream
  */

class MdtCsmContByteStreamTool: public AthAlgTool, virtual public IMDT_RDOtoByteStreamTool
  {

public:

  typedef MdtCsmContainer CONTAINER ; 

  /** constructor
  */
   MdtCsmContByteStreamTool( const std::string& type, const std::string& name,
        const IInterface* parent ) ;

  /** destructor 
  */ 
  //virtual ~MdtCsmContByteStreamTool() ;

  /** AlgTool InterfaceID
  */
  //  static const InterfaceID& interfaceID( ) ;

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  StatusCode convert(CONTAINER* cont, RawEventWrite* re, MsgStream& log ); 

private: 

  MDT_Hid2RESrcID* m_hid2re; 

  ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
    "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
  
  FullEventAssembler<MDT_Hid2RESrcID> m_fea ;    

};
}
#endif



