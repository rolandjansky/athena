/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDERTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "ByteStreamData/RawEvent.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "GaudiKernel/IIncidentListener.h"
#include "SCT_RawDataByteStreamCnv/ISCTRawDataProviderTool.h"

#include <set>
#include <string>

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

class ISCT_ByteStreamErrorsSvc;
class ISCT_RodDecoder;

class SCTRawDataProviderTool : virtual public ISCTRawDataProviderTool, 
                               virtual public AthAlgTool, 
                               virtual public IIncidentListener
{

 public:
   
  //! AlgTool InterfaceID
  //  static const InterfaceID& interfaceID( ) ;
  
  //! constructor
  SCTRawDataProviderTool( const std::string& type, const std::string& name,
			    const IInterface* parent ) ;

  //! destructor 
  virtual ~SCTRawDataProviderTool() ;

  //! initialize
  virtual StatusCode initialize();

  //! finalize
  virtual StatusCode finalize();
  
  //! this is the main decoding method
  virtual StatusCode convert( std::vector<const ROBFragment*>& vecRobs,
			      SCT_RDO_Container*               rdoIdc );

  /** function to be executed at BeginEvent incident */
  virtual void handle(const Incident& inc);

private: 
  
  ToolHandle<ISCT_RodDecoder>  m_decoder; 
  ServiceHandle<ISCT_ByteStreamErrorsSvc> m_bsErrSvc;
  
  // bookkeeping if we have decoded a ROB already
  std::set<uint32_t> m_robIdSet;

};

#endif



