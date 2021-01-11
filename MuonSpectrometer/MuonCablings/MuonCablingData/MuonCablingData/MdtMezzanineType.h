/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CABLING_MDTMEZZANINETYPE_H
#define MUONMDT_CABLING_MDTMEZZANINETYPE_H

/**
 *
 *  @brief Atlas MDT Mezzanine Type
 *  
 *  This class describes the internal mapping of a 
 *  generic MDT mezzanine type.
 *
 *  @author Stefano.Rosati@roma1.infn.it
 *
 **/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/Bootstrap.h"

#include <vector>

typedef std::vector<uint8_t> MdtLayer;

class MdtMezzanineType {

 public:

  /** constructors with arguments */
  MdtMezzanineType(uint8_t typeId, uint8_t nOfLayers);

  /** constructor only with typeId */
  MdtMezzanineType(uint8_t typeId);
  
  ~MdtMezzanineType();

  /** add a layer type */
  bool addLayer(uint8_t layerNumber, MdtLayer layer);

  /** return the mezzanine type */
  uint8_t type() const {return m_type;}

  /** return the number of layers */
  uint8_t nOfLayers() const { return m_nOfLayers;}

  /** return the number of tubes in each layer */
  uint8_t nOfTubesInLayer() const { return 24/m_nOfLayers; }

  /** check if a given layer has been initialized */
  bool hasLayer(uint8_t layerNumber) const;

  /** retrieve a given layer */
  MdtLayer getLayer(int layer) const {return m_layers[layer];}

  /** dump the full layers mapping */
  void print();

 private:

  /** this is the generic Id of this type (as in COOL) */
  uint8_t m_type;

  /** number of layers */
  uint8_t m_nOfLayers;

  /** Array with the layers (maximum 4) */
  MdtLayer m_layers[5];

  /** Output level and message service */
  bool m_debug;
  MsgStream* m_log;
  
};

#endif // MUONMDT_CABLING_MDTMEZZANINETYPE_H

