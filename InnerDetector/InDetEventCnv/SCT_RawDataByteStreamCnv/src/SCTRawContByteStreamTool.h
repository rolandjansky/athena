/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTRAWEVENTTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTRAWEVENTTOOL_H
///STL
#include <stdint.h>
///Base classes 
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_RawDataByteStreamCnv/ISCTRawContByteStreamTool.h"
///Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
///other athena
#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 

class ISCT_RodEncoder;
class ISCT_CablingSvc;
class SrcIdMap;
class SCT_ID;
namespace InDetDD {
  class SCT_DetectorManager;
}

#include <string>


/** An AthAlgTool class to provide conversion from SCT RDO container
 *  to ByteStream, and fill it in RawEvent
 *  created:  Oct 25, 2002, by Hong Ma 
 *  requirements:   typedef for CONTAINER class method 
 *   StatusCode convert(CONTAINER* cont, RawEvent* re, MsgStream& log ); 
  */

class SCTRawContByteStreamTool: virtual public ISCTRawContByteStreamTool, virtual public AthAlgTool {
public:

  // RawData type
  typedef SCT_RDORawData          RDO ;
  // Collection type 
  typedef InDetRawDataCollection< SCT_RDORawData > SCTRawCollection; 
  // Container type
  typedef SCT_RDO_Container       SCTRawContainer; 


  //! constructor
  SCTRawContByteStreamTool( const std::string& type, const std::string& name,
			    const IInterface* parent ) ;
  
  //! destructor 
  virtual ~SCTRawContByteStreamTool() ;

  //  static const InterfaceID& interfaceID( ) ;

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  //! New convert method which makes use of the encoder class (as done for other detectors)
  StatusCode convert(SCT_RDO_Container* cont, RawEventWrite* re, MsgStream& log); 
  
private: 
  
  ToolHandle<ISCT_RodEncoder> m_encoder;
  ServiceHandle<ISCT_CablingSvc> m_cabling;
  const InDetDD::SCT_DetectorManager* m_sct_mgr;
  const SCT_ID*                m_sct_idHelper;
  unsigned short               m_RodBlockVersion;
  FullEventAssembler<SrcIdMap> m_fea; 

};
#endif



