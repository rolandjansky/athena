/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTRAWEVENTTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTRAWEVENTTOOL_H

///Base classes 
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_RawDataByteStreamCnv/ISCTRawContByteStreamTool.h"

///other athena
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

///Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

///STL
#include <cstdint>
#include <string>

class SrcIdMap;
class ISCT_RodEncoder;
class ISCT_CablingSvc;
class SCT_ID;

/** An AthAlgTool class to provide conversion from SCT RDO container
 *  to ByteStream, and fill it in RawEvent
 *  created:  Oct 25, 2002, by Hong Ma 
 *  requirements:   typedef for CONTAINER class method 
 *   StatusCode convert(CONTAINER* cont, RawEvent* re, MsgStream& log ); 
 */

class SCTRawContByteStreamTool: public extends<AthAlgTool, ISCTRawContByteStreamTool> {
 public:

  // RawData type
  typedef SCT_RDORawData RDO;
  // Collection type 
  typedef InDetRawDataCollection<SCT_RDORawData> SCTRawCollection; 
  // Container type
  typedef SCT_RDO_Container SCTRawContainer; 

  //! constructor
  SCTRawContByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent);
  
  //! destructor
  virtual ~SCTRawContByteStreamTool() = default;

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  //! New convert method which makes use of the encoder class (as done for other detectors)
  StatusCode convert(SCT_RDO_Container* cont, RawEventWrite* re, MsgStream& log); 
  
 private: 
  
  ToolHandle<ISCT_RodEncoder> m_encoder{this, "Encoder", "SCT_RodEncoder", "SCT ROD Encoder for RDO to BS conversion"};
  ServiceHandle<ISCT_CablingSvc> m_cabling;
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  const SCT_ID* m_sct_idHelper;
  unsigned short m_RodBlockVersion;
  FullEventAssembler<SrcIdMap> m_fea; 

};

#endif // SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTRAWEVENTTOOL_H
