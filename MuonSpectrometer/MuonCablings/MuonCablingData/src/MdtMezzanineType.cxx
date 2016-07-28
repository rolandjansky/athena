/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingData/MdtMezzanineType.h"

MdtMezzanineType::MdtMezzanineType(uint8_t typeId, uint8_t nOfLayers) :
  m_type(typeId),
  m_nOfLayers(nOfLayers)
{
  // initialize the message service
  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("MessageSvc", msgSvc);
  if (sc.isFailure())   {
    std::cout << "Can't locate the MessageSvc" << std::endl;
  }
  m_log = new MsgStream(msgSvc, "MdtMezzanineType");
  
  m_debug = (m_log->level() <= MSG::VERBOSE);

  // intialize the layer maps
  for (int i=0 ; i<5 ; i++) {
    m_layers[i].clear();
  }
  
}

MdtMezzanineType::MdtMezzanineType(uint8_t typeId) :
  m_type(typeId),
  m_nOfLayers(0)
{
  // initialize the message service
  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("MessageSvc", msgSvc);
  if (sc.isFailure())   {
    std::cout << "Can't locate the MessageSvc" << std::endl;
  }
  m_log = new MsgStream(msgSvc, "MdtMezzanineType");
  
  m_debug = (m_log->level() <= MSG::VERBOSE);

  // intialize the layer maps
  for (int i=0 ; i<5 ; i++) {
    m_layers[i].clear();
  }
  
}

/** Destructor */
MdtMezzanineType::~MdtMezzanineType()
{
  delete m_log;
}


/** add a layer to the mezzanine type */
bool MdtMezzanineType::addLayer(uint8_t layerNumber, MdtLayer layer)
{

  if (layerNumber>4) {
    *m_log << MSG::ERROR << "layer number " << layerNumber << " not accepted!" << endmsg;
    return false;
  }
  if (m_layers[layerNumber].size() != 0) {
    *m_log << MSG::ERROR << "The layer number " << layerNumber 
	   << " is not empty " << endmsg;
    return false;
  }
    
  m_layers[layerNumber]=layer;
  
  // dump the layer content for debugging
  if (m_debug) {
    *m_log << MSG::VERBOSE << "Adding layer " << (int) layerNumber << " ";
    for (unsigned int i=0 ; i<m_layers[layerNumber].size() ; ++i) {
      *m_log << MSG::VERBOSE << (int) m_layers[layerNumber][i];
    }
    *m_log << MSG::VERBOSE << endmsg;
  }

  m_nOfLayers++;

  return true;
}


bool MdtMezzanineType::hasLayer(uint8_t layerNumber) const {

  bool hasL = (m_layers[layerNumber].size()>0);
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
