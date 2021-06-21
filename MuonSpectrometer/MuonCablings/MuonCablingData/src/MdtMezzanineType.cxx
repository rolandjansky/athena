/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingData/MdtMezzanineType.h"

MdtMezzanineType::MdtMezzanineType(uint8_t typeId, uint8_t nOfLayers, MsgStream &log) :
  m_type(typeId),
  m_nOfLayers(nOfLayers)
{
  m_debug = (log.level() <= MSG::VERBOSE);

}

MdtMezzanineType::MdtMezzanineType(uint8_t typeId, MsgStream &log) :
  m_type(typeId),
  m_nOfLayers(0)
{
  m_debug = (log.level() <= MSG::VERBOSE);
  
}

/** Destructor */
MdtMezzanineType::~MdtMezzanineType()
{}


/** add a layer to the mezzanine type */
bool MdtMezzanineType::addLayer(uint8_t layerNumber, MdtLayer layer, MsgStream &log)
{

  if (layerNumber>4) {
    log << MSG::ERROR << "layer number " << layerNumber << " not accepted!" << endmsg;
    return false;
  }
  if (!m_layers[layerNumber].empty()) {
    log << MSG::ERROR << "The layer number " << layerNumber 
	   << " is not empty " << endmsg;
    return false;
  }
    
  m_layers[layerNumber]=std::move(layer);
  
  // dump the layer content for debugging
  if (m_debug) {
    log << MSG::VERBOSE << "Adding layer " << (int) layerNumber << " ";
    for (unsigned int i=0 ; i<m_layers[layerNumber].size() ; ++i) {
      log << MSG::VERBOSE << (int) m_layers[layerNumber][i];
    }
    log << MSG::VERBOSE << endmsg;
  }

  m_nOfLayers++;

  return true;
}


bool MdtMezzanineType::hasLayer(uint8_t layerNumber) const {

  bool hasL = !m_layers[layerNumber].empty();
  return hasL;
}


/** print out the mezzanine content to the MSG stream */
void MdtMezzanineType::print() {
  
  std::cout << "==============================" << std::endl;
  std::cout << "Dumping mezzanine type: " << m_type << std::endl;

  for (int i=0 ; i<m_nOfLayers ; ++i) {
    
    std::cout << "Number of channels in layer " << i << " is: " << m_layers[i].size();
    
    for (std::vector<uint8_t>::const_iterator ic=m_layers[i].begin() ; 
	 ic != m_layers[i].end() ; ++ic) {

      std::cout << "Channel: " << i << "  tube number: " << (*ic) << std::endl;

    }
    
  }

}
