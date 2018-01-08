/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eformat/SourceIdentifier.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "TrigFTK_RawData/FTK_RawPixelCluster.h"
#include "TrigFTK_RawData/FTK_RawSCT_Cluster.h"

#include "TrigFTKByteStream/IFTKByteStreamDecoderEncoderTool.h"

#include "TrigFTKByteStream/TrigFTKByteStreamTool.h"
static const InterfaceID IID_ITrigFTKByteStreamTool("TrigFTKByteStreamTool", 1, 0);



//------------------------------------------------------------------------------
FTK::TrigFTKByteStreamTool::TrigFTKByteStreamTool( const std::string& type, const std::string& name, const IInterface* parent )
  :  AthAlgTool(type,name,parent),
     m_decoder("FTK::FTKByteStreamDecoderEncoderTool"),
     m_isAuxFormat(false)
{
  declareInterface< FTK::TrigFTKByteStreamTool  >( this );
  declareProperty("FTKAuxDataFormat", m_isAuxFormat);
  declareProperty("ROBIDlist",m_robID={0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf,0x10});
}

//------------------------------------------------------------------------------
FTK::TrigFTKByteStreamTool::~TrigFTKByteStreamTool()
{
}

//------------------------------------------------------------------------------
const InterfaceID& FTK::TrigFTKByteStreamTool::interfaceID( )
{
  return IID_ITrigFTKByteStreamTool;
}

//------------------------------------------------------------------------------
StatusCode FTK::TrigFTKByteStreamTool::initialize()
{
  ATH_MSG_DEBUG( "TrigFTKByteStreamTool has been loaded");
  if (m_isAuxFormat) {
    ToolHandle <IFTKByteStreamDecoderEncoderTool> decoder("FTK::FTKByteStreamDecoderEncoderAuxTool");
    m_decoder= decoder;
    ATH_MSG_INFO( "TrigFTKByteStreamTool is in a special configuration for 8 layer Aux format data");
  } else {
    ATH_MSG_INFO( "TrigFTKByteStreamTool is configured for standard 12 layer FLIC format data");
  }
  if (m_decoder.retrieve().isFailure()){
    ATH_MSG_ERROR("could not retrieve " << m_decoder);
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG(m_decoder.name() << " retrieved successfully");
  }
  
  ATH_MSG_DEBUG(" ROBIDlist " << m_robID);
  return AlgTool::initialize();
}

//------------------------------------------------------------------------------
StatusCode FTK::TrigFTKByteStreamTool::finalize()
{
   StatusCode sc = AlgTool::finalize();
   return sc;
}




uint32_t ftkROBID(uint32_t robID) {
  return eformat::helper::SourceIdentifier(eformat::TDAQ_FTK, robID).code();
}


//------------------------------------------------------------------------------
StatusCode FTK::TrigFTKByteStreamTool::convert( const FTK_RawTrackContainer* container, 
						RawEventWrite* re,
						const std::string& objName) {
  ATH_MSG_DEBUG ( "[TrigFTKByteStreamTool::convert from FTK_RawTrackContainer]: " << objName );
  m_fea.clear();
  
  FullEventAssembler<SrcIdMap>::RODDATA* theROD = m_fea.getRodData( ftkROBID(0x1) );
  std::vector<uint32_t>& payload = *theROD ;
  
  //  CHECK(FTKByteStreamDecoderEncoder::encode(container, payload, msg()));
  m_decoder->encode(container, payload);

  m_fea.fill(re, msg());

  return StatusCode::SUCCESS;
}






//------------------------------------------------------------------------------
StatusCode FTK::TrigFTKByteStreamTool::convert(IROBDataProviderSvc& dataProvider,
					       FTK_RawTrackContainer*& result, const std::string& )
{  

  IROBDataProviderSvc::VROBFRAG rob_frags; //typedef std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> VROBFRAG;

  std::vector<uint32_t> vID;

  for(auto irob:m_robID){
    vID.push_back( ftkROBID(irob) );
  }

  dataProvider.addROBData(vID);
  dataProvider.getROBData(vID, rob_frags);

  ATH_MSG_DEBUG ( "[TrigFTKByteStreamTool::convert to FTK_RawTrackContainer] Got ROB fragments: "<< rob_frags.size() );
  result =  new FTK_RawTrackContainer();

  std::vector<uint32_t> ftk_content;

  for (IROBDataProviderSvc::VROBFRAG::const_iterator rob = rob_frags.begin(); rob != rob_frags.end(); ++rob)
  {

    const uint32_t nData  = (*rob)->rod_ndata();
    const size_t sourceID = (*rob)->rod_source_id();
    ATH_MSG_DEBUG ( "[convert to FTK_RawTrackContainer] Reading fragment of size " << nData
		    << " from source " << std::hex << sourceID << std::dec);

    if ( nData == 0 ) {
      ATH_MSG_DEBUG( "[convert to FTK_RawTrackContainer] empty data payload" );      
    }

    if ( nData == 1 ) {
      ATH_MSG_DEBUG( "[convert to FTK_RawTrackContainer] only size in payload, no tracks" );      
      return StatusCode::SUCCESS;
    }


    OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData = 0;
    (*rob)->rod_data(rodData);

    StatusCode scdc = m_decoder->decode(nData, rodData, result);    
    if (scdc.isFailure()){
      ATH_MSG_WARNING("problem in decoding the rob fragment");
    }
  }
  return StatusCode::SUCCESS;
}
