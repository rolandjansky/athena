/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELRAWDATABYTESTREAMCNV_PIXELRAWDATAPROVIDERTOOL_H
#define PIXELRAWDATABYTESTREAMCNV_PIXELRAWDATAPROVIDERTOOL_H

#include "PixelRawDataByteStreamCnv/IPixelRawDataProviderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "InDetRawData/InDetTimeCollection.h"

#include <set>
#include <string>

class IPixelRodDecoder;
class IPixelByteStreamErrorsSvc;

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
  
  ToolHandle<IPixelRodDecoder>  m_decoder;
  ServiceHandle<IPixelByteStreamErrorsSvc> m_bsErrSvc;
  
  // bookkeeping if we have decoded a ROB already
  std::set<uint32_t> m_robIdSet;

  InDetTimeCollection* m_LVL1Collection;
  InDetTimeCollection* m_BCIDCollection;
  int m_DecodeErrCount;
  uint32_t m_LastLvl1ID;
};

#endif
