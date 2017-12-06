/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPIXELRAWDATABYTESTREAMCNV_PIXELRAWDATAPROVIDERTOOL_H
#define IPIXELRAWDATABYTESTREAMCNV_PIXELRAWDATAPROVIDERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamData/RawEvent.h" 
#include "InDetRawData/PixelRDO_Container.h"

// the tool to decode a ROB frament
class IPixelRawDataProviderTool : virtual public IAlgTool
{

 public:
   
  //! AlgTool InterfaceID
  static const InterfaceID& interfaceID( ) ;
  
  //! destructor 
  virtual ~IPixelRawDataProviderTool(){};

  //! this is the main decoding method
  virtual StatusCode convert( std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
		      IPixelRDO_Container*               rdoIdc ) = 0;

};

inline const InterfaceID& IPixelRawDataProviderTool::interfaceID(){
static const InterfaceID IID_IPixelRawDataProviderTool("IPixelRawDataProviderTool", 1, 0);
return IID_IPixelRawDataProviderTool;
}

#endif
