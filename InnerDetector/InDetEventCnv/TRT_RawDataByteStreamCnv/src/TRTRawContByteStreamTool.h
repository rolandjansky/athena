/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_RAWDATABYTESTREAMCNV_TRTRAWCONTRAWEVENTTOOL_H
#define TRT_RAWDATABYTESTREAMCNV_TRTRAWCONTRAWEVENTTOOL_H

#include "TRT_RawDataByteStreamCnv/ITRTRawContByteStreamTool.h"

#include "GaudiKernel/ToolHandle.h"


#include "TRT_Cabling/ITRT_CablingSvc.h"


#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 
#include "ByteStreamCnvSvcBase/SrcIdMap.h" 
#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"


class TRT_ID;



/** An AlgTool class to provide conversion from TRT RDO container
  *  to ByteStream, and fill it in RawEvent
  *  created:  Oct 25, 2002, by Hong Ma 
  *  requirements:   typedef for CONTAINER class 
		     method 
               StatusCode convert(CONTAINER* cont, RawEvent* re, MsgStream& log ); 
  */

class TRTRawContByteStreamTool: public extends<AthAlgTool, ITRTRawContByteStreamTool>
{
 public:

  // RawData type
  typedef TRT_RDORawData          RDO ;

  // Collection type 
  typedef InDetRawDataCollection< RDO > TRTRawCollection; 


  //! constructor
  TRTRawContByteStreamTool( const std::string& type, const std::string& name,
			    const IInterface* parent ) ;
  
  //! destructor 
  virtual ~TRTRawContByteStreamTool() ;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  //! New convert method which makes use of the encoder class (as done for other detectors)
  virtual StatusCode convert(TRT_RDO_Container* cont) const override; 
  
private: 
   ServiceHandle<ByteStreamCnvSvc> m_byteStreamCnvSvc
   { this, "ByteStreamCnvSvc", "ByteStreamCnvSvc" };

  ServiceHandle<ITRT_CablingSvc>  m_trt_CablingSvc;
  
   const TRT_ID*                   m_trt_idHelper;
   unsigned short                  m_RodBlockVersion;

};
#endif



