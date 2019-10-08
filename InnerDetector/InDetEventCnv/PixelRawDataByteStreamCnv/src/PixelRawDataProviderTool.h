/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELRAWDATABYTESTREAMCNV_PIXELRAWDATAPROVIDERTOOL_H
#define PIXELRAWDATABYTESTREAMCNV_PIXELRAWDATAPROVIDERTOOL_H

#include "PixelRawDataByteStreamCnv/IPixelRawDataProviderTool.h"
#include "PixelRawDataByteStreamCnv/IPixelRodDecoder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteHandle.h"

#include "InDetRawData/InDetTimeCollection.h"

#include <set>
#include <string>

class IPixelRodDecoder;

// the tool to decode a ROB frament
class PixelRawDataProviderTool : virtual public IPixelRawDataProviderTool, public AthAlgTool
{

 public:
   
  //! constructor
  PixelRawDataProviderTool( const std::string& type, const std::string& name,
			    const IInterface* parent ) ;

  //! destructor 
  ~PixelRawDataProviderTool() ;

  //! initialize
  StatusCode initialize() override;

  //! finalize
  StatusCode finalize() override;
  
  //! this is the main decoding method
  StatusCode convert( std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
		      IPixelRDO_Container*               rdoIdc ) final;


private: 
  
  ToolHandle<IPixelRodDecoder>  m_decoder
  {this, "Decoder", "PixelRodDecoder", "Tool for PixelRodDecoder"};


  SG::WriteHandleKey<InDetTimeCollection> m_LVL1CollectionKey;
  SG::WriteHandle<InDetTimeCollection>    m_LVL1Collection;
  SG::WriteHandleKey<InDetTimeCollection> m_BCIDCollectionKey;
  SG::WriteHandle<InDetTimeCollection>    m_BCIDCollection;

  int m_DecodeErrCount;
  uint32_t m_LastLvl1ID;
};

#endif
