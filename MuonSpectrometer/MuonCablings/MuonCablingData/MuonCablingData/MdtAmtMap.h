/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CABLING_MDTAMTMAP_H
#define MUONMDT_CABLING_MDTAMTMAP_H

/**
 *
 *  @brief Atlas MDT Tdc Mapping class
 *  
 *  This class implements the channel mapping of a single MDT TDC.
 *  The mapping is between the TDC channel and the tube multilayer, layer, number in 
 *  the offline convention
 *
 *  @author Stefano.Rosati@roma1.infn.it
 *
 **/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/Bootstrap.h"

#include <stdint.h>

class MdtMezzanineType;
class MdtIdHelper;

#define CHANMAX 24
#define NOTSET 99

class MdtAmtMap {

 public:

  /** empty constructor */
  MdtAmtMap(uint8_t tdcId);

  /** constructor */
  /** arguments are the mezzanine type, one channel (usually chan 0) and the */
  /** corresponding offline informations */
  MdtAmtMap(const MdtMezzanineType* mezType, uint8_t tdcId, uint8_t channel, 
	    int station, int eta, int phi, 
	    int multiLayer, int layerZero, int tubeZero, 
	    const MdtIdHelper* helper, std::shared_ptr<MsgStream> ext_log);

  /** destructor */
  ~MdtAmtMap()=default;

  /** set multilayer */
  //bool setMultiLayer(uint8_t multiLayer) {m_multiLayer = multiLayer;}

  /** add a channel */
  bool setChannel(uint8_t channel, uint8_t layer, uint8_t tube);

  /** retrieve the full information */
  bool offlineId(uint8_t channel, int& station, int& eta, int& phi, 
		 int& multilayer, int& layer, int& tube);

  /** get the mezzanine type */
  uint8_t mezzanineType() {return m_mezType;}
 
  /** return the tdc id */
  uint8_t moduleId() {return m_moduleId;}

  /** get the multilayer (independent of the channel) */
  int multiLayer() {return m_multiLayer;} 

  /** get the layer number */
  int layer(uint8_t channel) {return m_layer[channel];}

  /** get the tube number */
  int tube(uint8_t channel) {return m_tube[channel];}

 private:
  
  /** Private functions */

  /** initialize the channel-to-tube map */
  bool initMap(const MdtMezzanineType* mezType, uint8_t channel, int layerZero, int tubeZero);


  uint8_t m_moduleId;

  /** tube corresponding to each tdc channel */
  int m_station;
  int m_eta;
  int m_phi;
  int m_multiLayer;
  int m_layer[CHANMAX];
  int m_tube[CHANMAX];

  /** mezzanine type */
  uint8_t m_mezType;

  /** Output level and message service */
  bool m_debug;
  std::shared_ptr<MsgStream> m_log;

  const MdtIdHelper* m_mdtIdHelper;

};

#endif   // MUONMDT_CABLING_MDTAMTMAP_H
