/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_RAWDATABYTESTREAMCNV_TRTRAWDATAPROVIDERTOOL_H
#define ITRT_RAWDATABYTESTREAMCNV_TRTRAWDATAPROVIDERTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetRawData/TRT_RDO_Container.h"
#include "ByteStreamData/RawEvent.h" 

#include <vector>

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

// the tool to decode a ROB frament

class ITRTRawDataProviderTool : virtual public IAlgTool
{

 public:
   
  //! AlgTool InterfaceID
  static const InterfaceID& interfaceID( ) ;
  
  
  //! this is the main decoding method
  virtual StatusCode convert( std::vector<const ROBFragment*>& vecRobs,
		      TRT_RDO_Container*               rdoIdc ) = 0;

};

inline const InterfaceID& ITRTRawDataProviderTool::interfaceID( )
{ 
   static const InterfaceID IID ("ITRTRawDataProviderTool", 1, 0);
   return IID;
}

#endif



