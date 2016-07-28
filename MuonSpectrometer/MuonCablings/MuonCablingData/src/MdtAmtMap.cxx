/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingData/MdtAmtMap.h"
#include "MuonCablingData/MdtMezzanineType.h"

#include "MuonIdHelpers/MdtIdHelper.h"

MdtAmtMap::MdtAmtMap(uint8_t tdcId) :
  m_moduleId(tdcId),
  m_station(0),
  m_eta(0),
  m_phi(0),
  m_multiLayer(0),
  m_mezType(0),
  m_debug(false),
  m_log(0),
  m_mdtIdHelper(0)
{ }


// constructor
// constructor
MdtAmtMap::MdtAmtMap(const MdtMezzanineType* mezType, uint8_t tdcId, uint8_t channel, 
		     int station, 
		     int eta, int phi, int multiLayer, int layer, int tube, 
		     const MdtIdHelper* helper, MsgStream* ext_log) :
  m_moduleId(tdcId),
  m_station(station),
  m_eta(eta),
  m_phi(phi),
  m_multiLayer(multiLayer),
  m_mezType(mezType->type()),
  m_log(ext_log),
  m_mdtIdHelper(helper)
{


  //std::cout<<" Here we are "<<std::endl;
  m_debug = (m_log->level() <= MSG::VERBOSE);

  if (m_debug) { 
    *m_log << MSG::VERBOSE << "Adding Tdc number: " << (int) tdcId << endmsg;
  }


  // Initialize the values to un-filled
  for (int i = 0 ; i<CHANMAX ; ++i) {
    m_layer[i] = NOTSET;
    m_tube[i] = NOTSET;
  }
  
  // call the function that initializes the actual map
  if ( initMap(mezType, channel, layer, tube) ) {

    if (m_debug) {
      *m_log << MSG::VERBOSE << "Map initialized successfully for tdc number: " << MSG::hex 
	     << (int) tdcId << MSG::dec << endmsg;
    }

  }
  else {
    if (m_debug) {
      *m_log << MSG::VERBOSE << "Could not initialize completely the TDC map" << endmsg;
    }
  }

}


// destructor
MdtAmtMap::~MdtAmtMap() 
{ 
}


/** initialize the channel-to-tube map */
/** channel, layer, tube, contain the channel and position of the first tube */
bool MdtAmtMap::initMap(const MdtMezzanineType* mezType, uint8_t chanZero, int layerZero, int tubeZero)
{

  bool successful = true;

  if (m_debug) {
    *m_log << MSG::VERBOSE << "Initializing the map, the mezzanine type is " 
	   << (int) m_mezType << " with " << (int) mezType->nOfTubesInLayer() << " tubes per layer and " << (int) mezType->nOfLayers() << " layers"  
	   << endmsg;
  }

  for (uint8_t chan=0 ; chan<24 ; ++chan) {

    int layer=0;
    int tube=0;

    // special case of the BME of end 2013
    if ( mezType->type()==50 ) {

      layer = 4-chan%4 ;
      tube = tubeZero - int ( chan/4 );

    }
    else if ( mezType->type()==60 ) {
      layer = chan%4+1 ;
      tube = tubeZero + int ( chan/4 );
    }
    else {
            
      // calculate the layer
      if (layerZero==1) {
	layer=chan/(mezType->nOfTubesInLayer())+1;
      }
      else {
	layer=layerZero-chan/(mezType->nOfTubesInLayer());
      }
      
      std::vector<uint8_t> chanOrder = mezType->getLayer(layer);
      
      //    if (m_debug) {
      //      *m_log << MSG::DEBUG << "Channel order : ";
      //      for (int i=0 ; i< chanOrder.size() ; ++i) {
      //	*m_log << MSG::DEBUG << (int) chanOrder[i];
      //      }
      //      *m_log << MSG::DEBUG << endmsg;
      //    }
      
      // calculate the tube
      uint8_t localchan = chan%(mezType->nOfTubesInLayer());
      
      // special case of the BIR with 3 tubes overlapping to another mezzanine
      if (chanZero==5 && m_station==7 && abs(m_eta)==3 && ((mezType->type()>40
							    && localchan%2==0) || (mezType->type()<40 && localchan<3))) continue;
      
      //    *m_log << "the zero channel is: " << (int) chanZero 
      //	   << " and the local channel is: " << (int) localchan << endmsg;
      tube=chanOrder[localchan]-chanOrder[chanZero]+tubeZero;

    }


    bool setChan = setChannel(chan,layer,tube);

    if (!setChan) {
      *m_log << MSG::ERROR << "Could not set the channel " << (int) chan << endmsg;
      successful = false;
    }

    //    m_layer[chan]=layer;
    //m_tube[chan]=tube;

  }

  return successful;
}

/** setting the single channel in the map */
bool MdtAmtMap::setChannel(uint8_t channel, uint8_t layer, uint8_t tube)
{

  bool channelSet = false;

  /** set the channel if not already set */
  if (m_layer[channel]==NOTSET && m_tube[channel]==NOTSET) {

    /** Comment momentarily the output channel by channel (to be uncommented for debugging...) */
    //
    //if (m_debug) {
    //
    //*m_log << MSG::VERBOSE << "Adding channel: " << (int) channel << " connected to tube: " 
    //     << (int) tube  << " layer: " << (int) layer << endmsg;
    //
    //}    

    // do the validity check only for the BIR (some mezzanines are exceeding the tube range)
    //
    if (m_station==7) {

      Identifier id = m_mdtIdHelper->channelID(m_station,m_eta,m_phi,
					       m_multiLayer,layer,tube);

      if ( tube > m_mdtIdHelper->tubeMax(id) || tube < m_mdtIdHelper->tubeMin(id) ) {
	
	if (m_debug) {
	  *m_log << MSG::VERBOSE << "The tube is invalid (as expected) ! not adding it..." 
		 << endmsg;
	}

	return true;

      }

    }
      

    m_layer[channel] = layer;
    m_tube[channel] = tube;
    channelSet = true;
  }
  /** otherwise return an error */
  else {

    *m_log << MSG::ERROR << "Error in TDC number " << m_moduleId << endmsg;
    *m_log << MSG::ERROR << "The channel: " << channel << " has been already assigned to tube: "
	   << m_tube[channel] << " layer: " << m_layer[channel] << endmsg;

    channelSet = false;
  }

  return channelSet;
}

// get the offlineId
bool MdtAmtMap::offlineId(uint8_t channel, int& station, int& eta, int& phi, 
			  int& multilayer, int& layer, int& tube) 
{

  if ( channel >= CHANMAX ) {
    *m_log << MSG::WARNING << "Error in tdc " << MSG::hex << (int) m_moduleId << MSG::dec    
	   << " channel: " << MSG::hex << (int) channel << MSG::dec << " not defined!!" << endmsg;  
    return false;
  }

  if ( m_layer[channel]==NOTSET || m_tube[channel]==NOTSET ) {
    //    *m_log << MSG::ERROR << "Error in tdc " << MSG::hex << m_moduleId << MSG::dec
    //   << ". channel: " << MSG::hex << channel << MSG::dec << " not found" << endmsg;
    return false;
  }

  station = m_station;
  eta = m_eta;
  phi = m_phi;
  multilayer = m_multiLayer;
  layer = m_layer[channel];
  tube = m_tube[channel];

  return true;
}
