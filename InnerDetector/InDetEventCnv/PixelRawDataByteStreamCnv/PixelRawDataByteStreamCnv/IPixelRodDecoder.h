/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPIXELRAWDATABYTESTREAM_PIXEL_RODDECODER_H
#define IPIXELRAWDATABYTESTREAM_PIXEL_RODDECODER_H

#include "GaudiKernel/IAlgTool.h"

#include "InDetRawData/PixelRDO_Container.h"
#include "ByteStreamData/RawEvent.h" 

class IPixelRodDecoder : virtual public IAlgTool{

public: 

  static const InterfaceID& interfaceID( ) ;
  // destructor 
  virtual ~IPixelRodDecoder(){}; 

  virtual StatusCode fillCollection  (const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *robFrag, IPixelRDO_Container* rdoIdc, 
				      std::vector<IdentifierHash>* vecHash = NULL) = 0;
};

inline const InterfaceID& IPixelRodDecoder::interfaceID(){
static const InterfaceID IID_IPixelRodDecoder ("IPixelRodDecoder", 1, 0);
return IID_IPixelRodDecoder;
}


#endif
