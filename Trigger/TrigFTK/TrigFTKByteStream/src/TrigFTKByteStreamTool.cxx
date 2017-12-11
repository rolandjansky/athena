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

#include "FTKByteStreamDecoderEncoder.h"

#include "TrigFTKByteStream/TrigFTKByteStreamTool.h"
static const InterfaceID IID_ITrigFTKByteStreamTool("TrigFTKByteStreamTool", 1, 0);


//------------------------------------------------------------------------------
FTK::TrigFTKByteStreamTool::TrigFTKByteStreamTool( const std::string& type, const std::string& name, const IInterface* parent )
  :  AthAlgTool(type,name,parent),
     m_decoder("FTK::FTKByteStreamDecoderEncoderTool")
{
  declareInterface< FTK::TrigFTKByteStreamTool  >( this );
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
   if (m_decoder.retrieve().isFailure()){
     ATH_MSG_ERROR("could not retrieve " << m_decoder);
     return StatusCode::FAILURE;
   }
   else {
     ATH_MSG_DEBUG(m_decoder.name() << " retrieved successfully");
   }

   return AlgTool::initialize();
}

//------------------------------------------------------------------------------
StatusCode FTK::TrigFTKByteStreamTool::finalize()
{
   StatusCode sc = AlgTool::finalize();
   return sc;
}




uint32_t ftkROBID() {
  return eformat::helper::SourceIdentifier(eformat::TDAQ_FTK, 0x1).code();
}


//------------------------------------------------------------------------------
StatusCode FTK::TrigFTKByteStreamTool::convert( const FTK_RawTrackContainer* container, 
						RawEventWrite* re,
						const std::string& objName) {
  ATH_MSG_DEBUG ( "[TrigFTKByteStreamTool::convert from FTK_RawTrackContainer]: " << objName );
  m_fea.clear();
  
  FullEventAssembler<SrcIdMap>::RODDATA* theROD = m_fea.getRodData( ftkROBID() );
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
  std::vector<uint32_t> vID;
  vID.push_back( ftkROBID() );


  IROBDataProviderSvc::VROBFRAG rob_frags; //typedef std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> VROBFRAG;
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

    uint32_t nTracks = nData / FTKByteStreamDecoderEncoder::TrackBlobSize;
    
    // sanity check about the sizes
    // if ( nData == nTracks * FTKByteStreamDecoderEncoder::TrackBlobSize + 1) {
    //   ATH_MSG_DEBUG( "[convert to FTK_RawTrackContainer] Payload sizes look right" );      
    // } else{
    //   ATH_MSG_ERROR ( "[convert to FTK_RawTrackContainer] Payload sizes look wrong " );      
    //   return StatusCode::FAILURE;
    // }


    StatusCode scdc = m_decoder->decode(nTracks, rodData, result);    
    if (scdc.isFailure()){
      ATH_MSG_WARNING("problem in decoding the rob fragment");
    }
  }
  return StatusCode::SUCCESS;
}
