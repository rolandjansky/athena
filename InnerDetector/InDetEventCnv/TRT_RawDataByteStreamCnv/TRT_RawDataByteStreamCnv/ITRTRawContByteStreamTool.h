/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_RAWDATABYTESTREAMCNV_TRTRAWCONTRAWEVENTTOOL_H
#define ITRT_RAWDATABYTESTREAMCNV_TRTRAWCONTRAWEVENTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "ByteStreamData/RawEvent.h" 
#include "InDetRawData/TRT_RDO_Container.h"



class ITRTRawContByteStreamTool: virtual public IAlgTool 
{
 public:

   static const InterfaceID& interfaceID( ) ;


   //! New convert method which makes use of the encoder class (as done for other detectors)
   virtual StatusCode convert(TRT_RDO_Container* cont, RawEventWrite* re ) = 0; 

};

inline const InterfaceID& ITRTRawContByteStreamTool::interfaceID( )
{ 
   static const InterfaceID IID ("ITRTRawContByteStreamTool", 1, 0);
   return IID;
}


#endif



