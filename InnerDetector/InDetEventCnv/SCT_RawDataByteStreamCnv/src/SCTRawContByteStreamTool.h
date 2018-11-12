/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTBYTESTREAMTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTBYTESTREAMTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_RawDataByteStreamCnv/ISCTRawContByteStreamTool.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

#include "GaudiKernel/ToolHandle.h"

#include <mutex>
#include <string>

class ISCT_RodEncoder;
class ISCT_CablingTool;
class SCT_ID;
class SrcIdMap;

/** An AthAlgTool class to provide conversion from SCT RDO container
 *  to ByteStream, and fill it in RawEvent
 *  created:  Oct 25, 2002, by Hong Ma 
 *  requirements:   typedef for CONTAINER class method 
 *   StatusCode convert(CONTAINER* cont, RawEvent* re, MsgStream& log ); 
 */

class SCTRawContByteStreamTool: public extends<AthAlgTool, ISCTRawContByteStreamTool> {
 public:

  //! constructor
  SCTRawContByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent);
  
  //! destructor
  virtual ~SCTRawContByteStreamTool() = default;

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  //! New convert method which makes use of the encoder class (as done for other detectors)
  StatusCode convert(const SCT_RDO_Container* sctRDOCont, 
                     RawEventWrite* rawEvtWrite, MsgStream& log) const;
  
 private: 
  
  ToolHandle<ISCT_RodEncoder> m_encoder{this, "Encoder", "SCT_RodEncoder", "SCT ROD Encoder for RDO to BS conversion"};
  ToolHandle<ISCT_CablingTool> m_cabling{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};
  const SCT_ID* m_sctIDHelper;
  unsigned short m_rodBlockVersion;
  mutable FullEventAssembler<SrcIdMap> m_fullEventAssembler; // This has to be data member.
  mutable std::mutex m_mutex;
};

#endif // SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTBYTESTREAMTOOL_H
