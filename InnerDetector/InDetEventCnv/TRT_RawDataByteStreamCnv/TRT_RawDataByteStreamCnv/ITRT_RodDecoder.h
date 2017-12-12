/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_RAWDATABYTESTREAMCNV_TRT_RODDECODER_H
#define ITRT_RAWDATABYTESTREAMCNV_TRT_RODDECODER_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/IdentifierHash.h"
#include "ByteStreamData/RawEvent.h" 
#include "InDetRawData/TRT_RDO_Container.h"


#include <vector>


// the tool to decode a ROB frament

class ITRT_RodDecoder : virtual public IAlgTool
{

public: 

  //! interfaceID
  static const InterfaceID& interfaceID( ) ;

  //! the method to fill the IDC
  virtual StatusCode fillCollection ( const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* robFrag,
				      TRT_RDO_Container* rdoIdc,
				      std::vector<IdentifierHash>* vecHash = 0 ) = 0;


};



inline const InterfaceID& ITRT_RodDecoder::interfaceID( )
{ 
   static const InterfaceID IID ("ITRT_RodDecoder", 1, 0);
   return IID;
}

#endif
