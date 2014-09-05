/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_RAWDATABYTESTREAMCNV_TRTRAWDATAPROVIDERTOOL_H
#define TRT_RAWDATABYTESTREAMCNV_TRTRAWDATAPROVIDERTOOL_H

#include "TRT_RawDataByteStreamCnv/ITRTRawDataProviderTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "ByteStreamData/RawEvent.h" 

#include "InDetRawData/InDetTimeCollection.h"
#include "TRT_RawDataByteStreamCnv/ITRT_RodDecoder.h"
#include "TRT_ConditionsServices/ITRT_ByteStream_ConditionsSvc.h"


#include <set>
#include <string>

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

// the tool to decode a ROB frament

class TRTRawDataProviderTool : virtual public ITRTRawDataProviderTool, 
                               public AthAlgTool, virtual IIncidentListener
{

 public:
   
  //! AlgTool InterfaceID
  static const InterfaceID& interfaceID( ) ;
  
  //! constructor
  TRTRawDataProviderTool( const std::string& type, const std::string& name,
			    const IInterface* parent ) ;

  //! destructor 
  virtual ~TRTRawDataProviderTool() ;

  //! initialize
  virtual StatusCode initialize();

  //! finalize
  virtual StatusCode finalize();
  
  //! this is the main decoding method
  StatusCode convert( std::vector<const ROBFragment*>& vecRobs,
		      TRT_RDO_Container*               rdoIdc );

  //! Incident listener
  virtual void handle( const Incident &incident );

private: 
  TRTRawDataProviderTool( ); //Not implemented
  
  ToolHandle<ITRT_RodDecoder>  m_decoder;   

  ServiceHandle<ITRT_ByteStream_ConditionsSvc>   m_bsErrSvc;

  // bookkeeping if we have decoded a ROB already
  std::set<uint32_t> m_robIdSet;
  uint32_t      m_LastLvl1ID;
  InDetTimeCollection* m_LVL1Collection;
  InDetTimeCollection* m_BCCollection;
  bool  m_storeInDetTimeColls;
};

#endif



