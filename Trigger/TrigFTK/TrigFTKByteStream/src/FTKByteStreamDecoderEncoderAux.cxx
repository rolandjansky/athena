/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * FTKByteStreamDecoderEncoderAux bytestream encoding and decoding for the FTK
 *
 *              original version by B. Mindur and T. Bold
 *  2017/03/14  trailer [en,de]coding  / J. Masik
 */

#include "GaudiKernel/MsgStream.h"
#include "FTKByteStreamDecoderEncoderAux.h"
#include "AthenaBaseComps/AthMessaging.h"


using namespace FTKByteStreamDecoderEncoderAux;


namespace FTK {


  /*
    size_t decodeNumberOfTracks(OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData) {
    uint16_t low, high;
    uint32_t all;
    unpackNumberOfTracks(rodData, low, high, all);
    return all;
    }
  */


  FTKByteStreamDecoderEncoderAuxTool::FTKByteStreamDecoderEncoderAuxTool(const std::string& toolname, 
								   const std::string& type, 
								   const IInterface* parent) 
    : AthAlgTool(toolname, type, parent),
      m_doHeader(true),
      m_doTrailer(false)
  {
    declareInterface< IFTKByteStreamDecoderEncoderTool  >( this );
    declareProperty("doHeader", m_doHeader);
    declareProperty("doTrailer", m_doTrailer);
    declareProperty("towerID", m_towerID);
  }


  FTKByteStreamDecoderEncoderAuxTool::~FTKByteStreamDecoderEncoderAuxTool(){;}

  StatusCode   FTKByteStreamDecoderEncoderAuxTool::initialize(){
    return StatusCode::SUCCESS;
  }

  StatusCode   FTKByteStreamDecoderEncoderAuxTool::finalize(){
    return StatusCode::SUCCESS;
  }

  //not used
  void FTKByteStreamDecoderEncoderAuxTool::packNumberOfTracks(uint16_t nTracksLowPt, uint16_t nTracksHighPt, 
							   std::vector<uint32_t>& rod ){
    uint32_t size_data = nTracksHighPt;
    size_data <<= 16;
    size_data |= nTracksLowPt;
    rod.push_back( size_data );
  }

  //not used
  void FTKByteStreamDecoderEncoderAuxTool::unpackNumberOfTracks(OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, 
							     uint16_t& nTracksLowPt, uint16_t& nTracksHighPt, uint32_t& nTracks){
    nTracksLowPt  =  rodData[0] & 0xffff; 
    nTracksHighPt =  (rodData[0]>>16) & 0xffff; 
  
    nTracks = nTracksLowPt; // conversion to 32 + assignement
    nTracksLowPt += nTracksHighPt; // adding to more capable type
  }

  
  void FTKByteStreamDecoderEncoderAuxTool::packPixelCluster(const FTK_RawPixelCluster& cluster, 
							 std::vector<uint32_t>& payload){
    payload.push_back( cluster.getWordA() );
    payload.push_back( cluster.getWordB() );
  }


  void FTKByteStreamDecoderEncoderAuxTool::unpackPixCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, 
							 FTK_RawPixelCluster& cluster){
    cluster.setWordA(0); // no moduleID
    cluster.setWordB(*data);
  }

  void FTKByteStreamDecoderEncoderAuxTool::packSCTCluster(const FTK_RawSCT_Cluster& cluster, 
						       std::vector<uint32_t>& payload){

    payload.push_back(0);
    unsigned int word = (unsigned int) cluster.getHitCoord();
    unsigned int width = cluster.getHitWidth();
    if (width>0) width-=1; // store packed with 0-7 representing cluster widths 1-8
    if (width>7) width=7;
    word += width<<12;
    payload.push_back(word);

  }

  void FTKByteStreamDecoderEncoderAuxTool::unpackSCTCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, 
							 FTK_RawSCT_Cluster& cluster){

    uint32_t coord = (*data & 0x7ff);
    uint32_t width = ( ((*data)>>12) & 0x7);
    uint32_t word = (width<<28) + (coord<<16); 
    cluster.setWord(word);
  }

  void FTKByteStreamDecoderEncoderAuxTool::packTrack(const FTK_RawTrack* track, 
						  std::vector<uint32_t>& payload){
    unsigned int word = 0x80000000 | track->getSectorID() | (track->getTower()<<24);
    payload.push_back( word );
    payload.push_back( track->getRoadID() );
    
    for ( std::vector<FTK_RawPixelCluster>::const_iterator it = track->getPixelClusters().begin();
	  it != track->getPixelClusters().end(); ++it ) {
      packPixelCluster(*it, payload);
    }
    
    for ( std::vector<FTK_RawSCT_Cluster>::const_iterator it = track->getSCTClusters().begin();
	  it != track->getSCTClusters().end(); ++it ) {
      packSCTCluster(*it, payload);
    }
  }

  FTK_RawTrack* FTKByteStreamDecoderEncoderAuxTool::unpackFTTrack( OFFLINE_FRAGMENTS_NAMESPACE::PointerType data){

    FTK_RawTrack* track = new FTK_RawTrack();
    track->setSectorID(data[0] & 0xffff);
    //    track->setTower(m_towerID);  // Bugfix for towerID set to 0 in data
    track->setTower((data[0]>>24) & 0x3f);  // Bugfix for towerID set to 0 in data
    track->setRoadID(data[1] & 0xffffff);
    uint32_t lay1 = (data[0]>>16)&0x1;
    uint32_t lay2 = (data[0]>>17)&0x1;
    uint32_t lay3 = (data[0]>>18)&0x1;
    uint32_t lay4 = (data[0]>>19)&0x1;
    uint32_t lay6 = (data[0]>>20)&0x1;
    uint32_t lay8 = (data[0]>>21)&0x1;
    uint32_t lay9 = (data[0]>>22)&0x1;
    uint32_t lay10 = (data[0]>>23)&0x1;
    uint32_t layermap = (lay1<<1 | lay2<<2 | lay3<<3 | lay4<<4 | lay6<<6 | lay8 <<8 | lay9 << 9 | lay10 << 10);
    track->setLayerMap(layermap); 
    track->setIsAuxFormat(true);

    data += TrackParamsBlobSize;
    


    // get pixel hits  
    track->getPixelClusters().resize(NPixLayers);
    for ( size_t i = 1; i < NPixLayers; ++i) {
      unpackPixCluster(data, track->getPixelCluster(i) );    
      data += PixHitParamsBlobSize;

    }
    
    // gets SCT hits


    std::vector<size_t> sctplane={0,2,4,5,6};

    track->getSCTClusters().resize(NSCTLayers);
    for ( size_t i = 0; i < sctplane.size(); ++i) {

      unpackSCTCluster(data, track->getSCTCluster(sctplane[i]) );    

      data+= SCTHitParamsBlobSize;
    }
    // no more shifts needed
    return track;
  }  


  
  void FTKByteStreamDecoderEncoderAuxTool::packHeader(std::vector<uint32_t> &payload){
    payload.push_back(0xb0f00000);
    payload.push_back(0xff1234ff);
    payload.push_back(2);     //run number
    payload.push_back(3);     //extended lvl1 ID
    payload.push_back(3);     //TTCrcBCID
    payload.push_back(3);     //lvl1 trigger type 
    payload.push_back(3);     //detector event type
  
    if (payload.size() != FTKByteStreamDecoderEncoderAux::headerSize){
      ATH_MSG_ERROR("Inconsistent header size");
    }
  }

  void FTKByteStreamDecoderEncoderAuxTool::unpackHeader(OFFLINE_FRAGMENTS_NAMESPACE::PointerType &rodData){

    //skip to the end
    rodData += headerSize;
  }

  void FTKByteStreamDecoderEncoderAuxTool::packTrailer(std::vector<uint32_t> &payload){

    //    packMonitoring()
    payload.push_back(0xe0da0000);   
    payload.push_back(3); //extL1id
    payload.push_back(0);   //error_flag
    payload.push_back(10);   //reserved word #1
    payload.push_back(0);  //module data word count
    payload.push_back(0xe0f0000); 
    
  }

  void FTKByteStreamDecoderEncoderAuxTool::unpackMonitoring(OFFLINE_FRAGMENTS_NAMESPACE::PointerType &rodData){
    if ( (rodData[0] & 0xFFFF0000) == 0xE0DA0000) {
      ATH_MSG_VERBOSE("marker 0xE0DA0000 found");
    }
    return;
  }


  void FTKByteStreamDecoderEncoderAuxTool::unpackTrailer(OFFLINE_FRAGMENTS_NAMESPACE::PointerType &rodData){
    auto beforeMonitoring = rodData;
    unpackMonitoring(rodData);
    if (rodData == beforeMonitoring){
      ATH_MSG_VERBOSE("Extra monitoring records not found");
    } else {
      ATH_MSG_VERBOSE("Monitoring records of size " << (rodData-beforeMonitoring)/sizeof(OFFLINE_FRAGMENTS_NAMESPACE::PointerType));
    }

    //first word
    uint32_t  firstword = rodData[0]; 
    if (firstword != 0xe0da0000){
      ATH_MSG_ERROR("Trailer first word should read 0xe0da0000 but got 0x"<<std::hex<<firstword<<std::dec);
    }
    uint32_t extL1id = rodData[1];     
    ATH_MSG_VERBOSE("extL1id " << extL1id);

    uint32_t error_flag = rodData[2];  
    ATH_MSG_VERBOSE("error_flag " << error_flag);
    //reserved word
    ATH_MSG_VERBOSE("reserved " << rodData[3]);   rodData++;

    uint32_t  numDataElems  = rodData[4]; 
    ATH_MSG_VERBOSE("Number of Data Elements " << numDataElems);
    
    //last word
    uint32_t  lastword = rodData[5]; 
    if (lastword != 0xe0f00000){
      ATH_MSG_ERROR("trailer lastword should read 0xe0f00000");
    }
  }

  StatusCode FTKByteStreamDecoderEncoderAuxTool::encode(const FTK_RawTrackContainer* container, std::vector<uint32_t>& payload) {
  
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


  StatusCode FTKByteStreamDecoderEncoderAuxTool::decode(uint32_t nDataWords, OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, FTK_RawTrackContainer* result) {


    if (nDataWords <= (headerSize + trailerSize)) {
      ATH_MSG_VERBOSE("FTKByteStreamDecoderEncoderAuxTool::decode for nDataWords= "  << nDataWords << " Ntracks= 0");
      return StatusCode::SUCCESS;
    }
    
    uint32_t nTracks = (nDataWords - headerSize - trailerSize) / TrackBlobSize;
    uint32_t nOver = nDataWords - nTracks*TrackBlobSize - headerSize - trailerSize;
    
    ATH_MSG_VERBOSE("FTKByteStreamDecoderEncoderAuxTool::decode for nDataWords= " << nDataWords<< " Ntracks= " << nTracks << " remainder (should be zero) " << nOver);

    if (m_doHeader){
      unpackHeader(rodData);
    }

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
