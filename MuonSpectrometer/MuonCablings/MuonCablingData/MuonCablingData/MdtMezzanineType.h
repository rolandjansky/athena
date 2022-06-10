/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
 *
 **/

#include <array>
#include <vector>

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"

using MdtLayer = std::array<uint8_t, 8>;

class MdtMezzanineType {
public:
    /** constructors with arguments */
    MdtMezzanineType(uint8_t typeId, uint8_t nOfLayers);

    /** constructor only with typeId */
    MdtMezzanineType(uint8_t typeId);

    ~MdtMezzanineType() = default;

    /** add a layer type */
    bool addLayer(uint8_t layerNumber, MdtLayer layer, MsgStream& log);

    /** return the mezzanine type */
    uint8_t type() const { return m_type; }

    /** return the number of layers */
    uint8_t nOfLayers() const { return m_nOfLayers; }

    /** return the number of tubes in each layer */
    uint8_t nOfTubesInLayer() const { return 24 / m_nOfLayers; }

    /** check if a given layer has been initialized */
    bool hasLayer(uint8_t layerNumber) const;

    /** retrieve a given layer */
    const MdtLayer& getLayer(int layer) const { return m_layers.at(layer); }

    /** dump the full layers mapping */
    void print();

private:
    /** this is the generic Id of this type (as in COOL) */
    uint8_t m_type{0};

    /** number of layers */
    uint8_t m_nOfLayers{0};

    /** Array with the layers (maximum 4) */
    std::array<MdtLayer, 5> m_layers;
    std::array<bool, 5> m_layerInit{};
};

#endif  // MUONMDT_CABLING_MDTMEZZANINETYPE_H
