/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTRAWEVENTSERVICE_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTRAWEVENTSERVICE_H
///STL
#include <stdint.h>
///Base classes 
#include "AthenaBaseComps/AthService.h"
#include "SCT_RawDataByteStreamCnv/ISCTRawContByteStreamService.h"
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
class StoreGateSvc;

#include <string>


/** An AthService class to provide conversion from SCT RDO container
 *  to ByteStream, and fill it in RawEvent
 *  created:  Oct 25, 2002, by Hong Ma 
 *  requirements:   typedef for CONTAINER class method 
 *   StatusCode convert(CONTAINER* cont, RawEvent* re, MsgStream& log ); 
 */

class SCTRawContByteStreamService: virtual public ISCTRawContByteStreamService, virtual public AthService {
 public:

  // RawData type
  typedef SCT_RDORawData          RDO ;
  // Collection type 
  typedef InDetRawDataCollection< SCT_RDORawData > SCTRawCollection; 
  // Container type
  typedef SCT_RDO_Container       SCTRawContainer; 


  //! constructor
  SCTRawContByteStreamService(const std::string& name, ISvcLocator* svcloc) ;
  
  //! destructor 
  virtual ~SCTRawContByteStreamService() ;

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  //! New convert method which makes use of the encoder class (as done for other detectors)
  StatusCode convert(SCT_RDO_Container* cont, RawEventWrite* re, MsgStream& log); 
  
 private: 
  
  ToolHandle<ISCT_RodEncoder> m_encoder{this, "Encoder", "SCT_RodEncoder", "SCT ROD Encoder for RDO to BS conversion"};
  ServiceHandle<ISCT_CablingSvc> m_cabling;
  ServiceHandle<StoreGateSvc> m_detStore;
  const InDetDD::SCT_DetectorManager* m_sct_mgr;
  const SCT_ID* m_sct_idHelper;
  unsigned short m_RodBlockVersion;
  FullEventAssembler<SrcIdMap> m_fea; 

};

#endif
