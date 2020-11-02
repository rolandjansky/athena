/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_RAWDATABYTESTREAMCNV_TRTRAWCONTRAWEVENTTOOL_H
#define ITRT_RAWDATABYTESTREAMCNV_TRTRAWCONTRAWEVENTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetRawData/TRT_RDO_Container.h"



class ITRTRawContByteStreamTool: virtual public IAlgTool 
{
 public:
   DeclareInterfaceID (ITRTRawContByteStreamTool, 1, 0);

   //! New convert method which makes use of the encoder class (as done for other detectors)
   virtual StatusCode convert(TRT_RDO_Container* cont) const = 0; 

};


#endif



