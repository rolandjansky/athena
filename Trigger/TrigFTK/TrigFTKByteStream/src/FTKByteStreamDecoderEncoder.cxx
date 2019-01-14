/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * FTKByteStreamDecoderEncoder bytestream encoding and decoding for the FTK
 *
 *              original version by B. Mindur and T. Bold
 *  2017/03/14  trailer [en,de]coding  / J. Masik
 */

#include "GaudiKernel/MsgStream.h"
#include "TrigFTKByteStream/IFTKByteStreamDecoderEncoderTool.h"
#include "FTKByteStreamDecoderEncoder.h"
#include "AthenaBaseComps/AthMessaging.h"

using namespace FTKByteStreamDecoderEncoder;


namespace FTK {


  /*
    size_t decodeNumberOfTracks(OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData) {
    uint16_t low, high;
    uint32_t all;
    unpackNumberOfTracks(rodData, low, high, all);
    return all;
    }
  */


  FTKByteStreamDecoderEncoderTool::FTKByteStreamDecoderEncoderTool(const std::string& toolname, 
								   const std::string& type, 
								   const IInterface* parent) 
    : AthAlgTool(toolname, type, parent),
      m_doHeader(false),
      m_doTrailer(false)
  {
    declareInterface< IFTKByteStreamDecoderEncoderTool  >( this );
    declareProperty("doHeader", m_doHeader);
    declareProperty("doTrailer", m_doTrailer);
  }

  FTKByteStreamDecoderEncoderTool::~FTKByteStreamDecoderEncoderTool(){;}

  StatusCode   FTKByteStreamDecoderEncoderTool::initialize(){
    if (m_doHeader) ATH_MSG_INFO("Unpacking Header of size " << FTKByteStreamDecoderEncoder::headerSize);
    if (m_doTrailer) ATH_MSG_INFO("Unpacking Trailer ");
    return StatusCode::SUCCESS;
  }

  StatusCode   FTKByteStreamDecoderEncoderTool::finalize(){
    return StatusCode::SUCCESS;
  }

  //not used
  void FTKByteStreamDecoderEncoderTool::packNumberOfTracks(uint16_t nTracksLowPt, uint16_t nTracksHighPt, 
							   std::vector<uint32_t>& rod ){
    uint32_t size_data = nTracksHighPt;
    size_data <<= 16;
    size_data |= nTracksLowPt;
    rod.push_back( size_data );
  }

  //not used
  void FTKByteStreamDecoderEncoderTool::unpackNumberOfTracks(OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, 
							     uint16_t& nTracksLowPt, uint16_t& nTracksHighPt, uint32_t& nTracks){
    nTracksLowPt  =  rodData[0] & 0xffff; 
    nTracksHighPt =  (rodData[0]>>16) & 0xffff; 
  
    nTracks = nTracksLowPt; // conversion to 32 + assignement
    nTracksLowPt += nTracksHighPt; // adding to more capable type
  }

  
  void FTKByteStreamDecoderEncoderTool::packPixelCluster(const FTK_RawPixelCluster& cluster, 
							 std::vector<uint32_t>& payload){
    payload.push_back( cluster.getWordA() );
    payload.push_back( cluster.getWordB() );
  }


  void FTKByteStreamDecoderEncoderTool::unpackPixCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, 
							 FTK_RawPixelCluster& cluster){
    cluster.setWordA(*data);
    cluster.setWordB(*(data+1));
  }

  void FTKByteStreamDecoderEncoderTool::packSCTCluster(const FTK_RawSCT_Cluster& cluster, 
						       std::vector<uint32_t>& payload){
    payload.push_back( cluster.getWord() );
  }

  void FTKByteStreamDecoderEncoderTool::unpackSCTCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, 
							 FTK_RawSCT_Cluster& cluster){
    cluster.setWord(*data);
  }

  void FTKByteStreamDecoderEncoderTool::packTrack(const FTK_RawTrack* track, 
						  std::vector<uint32_t>& payload){
    payload.push_back( track->getTH1() );
    payload.push_back( track->getTH2() );
    payload.push_back( track->getTH3() );
    payload.push_back( track->getTH4() );
    payload.push_back( track->getTH5() );
    payload.push_back( track->getTH6() );
  
    for ( std::vector<FTK_RawPixelCluster>::const_iterator it = track->getPixelClusters().begin();
	  it != track->getPixelClusters().end(); ++it ) {
      packPixelCluster(*it, payload);
    }
  
    for ( std::vector<FTK_RawSCT_Cluster>::const_iterator it = track->getSCTClusters().begin();
	  it != track->getSCTClusters().end(); ++it ) {
      packSCTCluster(*it, payload);
    }
  }

  FTK_RawTrack* FTKByteStreamDecoderEncoderTool::unpackFTTrack( OFFLINE_FRAGMENTS_NAMESPACE::PointerType data){
    uint32_t data2 = (data[2] & 0xffffff) | ((FTK_RAWTRACK_VERSION)<<24); // force to latest version
    FTK_RawTrack* track = new FTK_RawTrack(data[0], data[1], data2, data[3], data[4], data[5]); // first six words are track params
    ATH_MSG_DEBUG("[Track: " << track->getInvPt() << " " << track->getPhi() << " " 
		  << track->getCotTh() << " " << track->getD0() << "]");
    track->setIsAuxFormat(false);

    data += TrackParamsBlobSize;
  
    // get pixel hits  
    track->getPixelClusters().resize(NPixLayers);
    for ( size_t i = 0; i < size_t(NPixLayers); ++i) {
      size_t offset = PixHitParamsBlobSize*i;
      unpackPixCluster(data+offset, track->getPixelCluster(i) );    
    }
    data += PixHitParamsBlobSize*NPixLayers;
  
    // gets SCT hits
    track->getSCTClusters().resize(NSCTLayers);
    for ( size_t i = 0; i < size_t(NSCTLayers); ++i) {
      size_t offset = SCTHitParamsBlobSize*i;
      unpackSCTCluster(data+offset, track->getSCTCluster(i) );    
    }
    // data pointer  shifted in the unpackFTTrack loop
    return track;
  }

  
  void FTKByteStreamDecoderEncoderTool::packHeader(std::vector<uint32_t> &payload){
    payload.push_back(FTKByteStreamDecoderEncoder::headerMarker);
    payload.push_back(FTKByteStreamDecoderEncoder::headerSize);
    payload.push_back(1);     //major and minor number
    payload.push_back(11);    //source identifier
    payload.push_back(2);     //run number
    payload.push_back(3);     //extended lvl1 ID
    payload.push_back(3);     //bunch crossing ID
    payload.push_back(3);     //lvl1 trigger type 
    payload.push_back(3);     //detector event type
  
    if (payload.size() != FTKByteStreamDecoderEncoder::headerSize){
      ATH_MSG_ERROR("Inconsistent header size");
    }
  }

  void FTKByteStreamDecoderEncoderTool::unpackHeader(OFFLINE_FRAGMENTS_NAMESPACE::PointerType &rodData){
    //marker
    uint32_t marker = rodData[0];
    if (marker!=FTKByteStreamDecoderEncoder::headerMarker){
      ATH_MSG_DEBUG("Not dealing with an FTK fragment " << std::hex << marker << " vs the marker " << FTKByteStreamDecoderEncoder::headerMarker << std::dec );
      //rodData += FTKByteStreamDecoderEncoder::headerSize -1;
      //return;
    }
    //skip to the end
    rodData += FTKByteStreamDecoderEncoder::headerSize;
  }

  void FTKByteStreamDecoderEncoderTool::packTrailer(std::vector<uint32_t> &payload){

    //    packMonitoring()
    payload.push_back(1);   //extL1id
    payload.push_back(0);   //error_flag
    payload.push_back(10);   //reserved word #1
    payload.push_back(20);   //reserved word #2
    payload.push_back(0);    //FLIC status
    payload.push_back(0);    //numelements
    payload.push_back(0);    //numdataelements
    payload.push_back(0x1);  //statusBlock
    
  }

  void FTKByteStreamDecoderEncoderTool::unpackMonitoring(OFFLINE_FRAGMENTS_NAMESPACE::PointerType &rodData){
    if ( (rodData[0] & 0xFFFF0000) == 0xE0DA0000) {
      ATH_MSG_DEBUG("marker 0xE0DA0000 found");
      //while (
    }
    return;
  }


  void FTKByteStreamDecoderEncoderTool::unpackTrailer(OFFLINE_FRAGMENTS_NAMESPACE::PointerType &rodData){
    auto beforeMonitoring = rodData;
    unpackMonitoring(rodData);
    if (rodData == beforeMonitoring){
      ATH_MSG_DEBUG("Extra monitoring records not found");
    } else {
      ATH_MSG_DEBUG("Monitoring records of size " << (rodData-beforeMonitoring)/sizeof(OFFLINE_FRAGMENTS_NAMESPACE::PointerType));
    }

    uint32_t extL1id = rodData[0];     rodData++;
    ATH_MSG_DEBUG("extL1id " << extL1id);

    uint32_t error_flag = rodData[0];  rodData++;
    ATH_MSG_DEBUG("error_flag " << error_flag);
    //2 reserved words
    ATH_MSG_DEBUG("reserved " << rodData[0]);   rodData++;
    ATH_MSG_DEBUG("reserved " << rodData[0]);   rodData++;

    //FLIC status  (TBD)
    ATH_MSG_DEBUG("FLIC " << rodData[0]);   rodData++;
    rodData++;

    //
    uint32_t  numStatusElems = rodData[0]; rodData++;
    ATH_MSG_DEBUG("Number of Status Elements " << numStatusElems);

    uint32_t  numDataElems  = rodData[0]; rodData++;
    ATH_MSG_DEBUG("Number of Data Elements " << numDataElems);
    
    //status block position
    uint32_t  statusBlock = rodData[0]; rodData++;
    if (statusBlock != 0x1){
      ATH_MSG_ERROR("Status block position should read 0x1");
    }
  }

  StatusCode FTKByteStreamDecoderEncoderTool::encode(const FTK_RawTrackContainer* container, std::vector<uint32_t>& payload) {
  
    // We used to have a word defining the number of low/high pt tracks, that is no longer necessary
    // // do not know yet what tracks are high pT, so all will be low pT
    // payload.reserve( 1 + TrackParamsBlobSize * container->size() );
    // packNumberOfTracks( container->size(), 0, payload );  
  
    payload.reserve(TrackParamsBlobSize * container->size() );
  
    if (m_doHeader){
      packHeader(payload);
    }
    
    for ( FTK_RawTrackContainer::const_iterator track = container->begin(); 
	  track != container->end(); ++track ) {
      packTrack(*track, payload);
    }

    if (m_doTrailer){
      packTrailer(payload);
    }
    
    return StatusCode::SUCCESS;
  
  }  


  StatusCode FTKByteStreamDecoderEncoderTool::decode(const uint32_t nDataWords, OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, FTK_RawTrackContainer* result) {
    
    uint32_t tracktotalsize=nDataWords;
    if (m_doHeader){
      if (tracktotalsize > FTKByteStreamDecoderEncoder::headerSize) {
	tracktotalsize -= FTKByteStreamDecoderEncoder::headerSize;
      } else {
	ATH_MSG_VERBOSE("FTKByteStreamDecoderEncoderAuxTool::decode for nDataWords= "  << nDataWords << " Ntracks= 0");
	return StatusCode::SUCCESS;
      }
    }
    if (m_doTrailer){
      if (tracktotalsize >FTKByteStreamDecoderEncoder:: trailerSize) {
	tracktotalsize -= FTKByteStreamDecoderEncoder::trailerSize;
      } else {
	ATH_MSG_VERBOSE("FTKByteStreamDecoderEncoderAuxTool::decode for nDataWords= "  << nDataWords << " Ntracks= 0");
	return StatusCode::SUCCESS;
      }
    }
    uint32_t nTracks = tracktotalsize / TrackBlobSize;
    ATH_MSG_VERBOSE("FTKByteStreamDecoderEncoderAuxTool::decode for nDataWords= " << nDataWords<< " Ntracks= " << nTracks);
    
    if (m_doHeader){
      unpackHeader(rodData);
    }
    ATH_MSG_DEBUG("rodData: " << rodData);
    result->reserve(result->size() + nTracks);
    for ( size_t i = 0; i < nTracks; ++i ) {
      FTK_RawTrack* track = unpackFTTrack( rodData );
      rodData += TrackBlobSize;
      result->push_back(track);
    }

    if (m_doTrailer){
      unpackTrailer(rodData);
    }
    
    return StatusCode::SUCCESS;
  }


}
