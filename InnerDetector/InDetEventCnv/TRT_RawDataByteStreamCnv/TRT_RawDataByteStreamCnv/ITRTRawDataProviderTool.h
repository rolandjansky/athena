/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_RAWDATABYTESTREAMCNV_TRTRAWDATAPROVIDERTOOL_H
#define ITRT_RAWDATABYTESTREAMCNV_TRTRAWDATAPROVIDERTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetRawData/TRT_RDO_Container.h"
#include "ByteStreamData/RawEvent.h" 

#include <vector>

class TRT_BSErrContainer;

// the tool to decode a ROB frament

class ITRTRawDataProviderTool : virtual public IAlgTool
{

 public:
  DeclareInterfaceID (ITRTRawDataProviderTool, 1, 0);
  
  //! this is the main decoding method
  virtual StatusCode convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
			     TRT_RDO_Container* rdoIdc,
			     TRT_BSErrContainer* bserr) const = 0;

};

#endif



