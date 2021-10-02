/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingsBuilder.h 713524 2015-12-09 08:56:24Z wsfreund $
#ifndef CALORINGERTOOLS_CALORINGSBUILDER_H
#define CALORINGERTOOLS_CALORINGSBUILDER_H

// Base includes:
#include "CaloRingerTools/ICaloRingsBuilder.h"
#include "AthenaBaseComps/AthAlgTool.h"

// STL includes:
#include <string>
#include <vector>
#include <memory>
#include <limits>

// Wrap-around phi helper include:
#include "CaloGeoHelpers/CaloPhiRange.h"

#include "CaloDetDescr/CaloDetDescrManager.h"

// xAOD Includes:
#include "xAODBase/IParticle.h"
#include "xAODCaloRings/RingSetContainerFwd.h"
#include "xAODCaloRings/CaloRingsContainerFwd.h"
#include "xAODCaloRings/tools/AtlasGeoPoint.h"

// StoreGate includes:
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
// Forward declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
class CaloCellContainer;
class CaloCellList;

namespace Ringer {

class CaloRingsBuilder : public ::AthAlgTool,
                         virtual public ICaloRingsBuilder
{

  public:

    /// @name CaloRingsBuilder ctors and dtors:
    /// @{
    /**
     * @brief Default constructor
     **/
    CaloRingsBuilder(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

    /**
     * @brief Destructor
     **/
    ~CaloRingsBuilder();
    /// @}

    /// Tool main methods:
    /// @{
    /**
     * @brief initialize method
     **/
    virtual StatusCode initialize() override;

    /**
     * @brief method for working on containers
     **/
    virtual StatusCode preExecute( xAOD::CaloRingsContainer* crCont
                                 , xAOD::RingSetContainer* rsCont
                                 , const std::size_t nReserve = 0) override;
    /**
     * @brief build CaloRings for IParticle
     **/
    virtual StatusCode execute(const xAOD::IParticle &particle,
        ElementLink<xAOD::CaloRingsContainer> &clRingsLink) override;
    /**
     * @brief build CaloRings for CaloCluster
     **/
    virtual StatusCode execute(const xAOD::CaloCluster &cluster,
        ElementLink<xAOD::CaloRingsContainer> &clRingsLink) override;
    /**
     * @brief finalize method
     **/
    virtual StatusCode finalize() override;
    /// @}

    /// Extra methods:
    /// @{
    /**
     * @brief Retrieve RawConfiguration Collection
     **/
    const xAOD::RingSetConf::RawConfCollection &rawConf() override {
      return m_rsRawConfCol;
    };
    /**
     * @brief Retrieve whether it was set used shower shape barycenter
     **/
    bool useShowerShapeBarycenter() override {
      return m_useShowShapeBarycenter;
    };
    /// @}

    std::size_t nRingSets() const override { return m_nRingSets; }

    const SG::WriteHandleKey<xAOD::CaloRingsContainer>& crContName() const override { return m_crContName; }
    const SG::WriteHandleKey<xAOD::RingSetContainer>& rsContName() const override { return m_rsContName; }

  protected:

    /// Tool protected methods:
    /// @{
    /**
     * @brief unique execute method for integrating interface code.
     **/
    template<typename T>
    StatusCode executeTemp(
        const T &input,
        ElementLink<xAOD::CaloRingsContainer> &crEL);
    /**
     * @brief main method where the RingSets are built.
     **/
    virtual StatusCode buildRingSet(
        const xAOD::RingSetConf::RawConf &rawConf,
        const AtlasGeoPoint &seed,
        xAOD::RingSet *rs);
    /**
     * @brief Fill RingSet seed for CaloCluster.
     **/
    StatusCode getRingSetSeed(
        const xAOD::RingSetConf::RawConf &conf,
        const xAOD::CaloCluster &cluster,
        AtlasGeoPoint &seed );
    /**
     * @brief Fill RingSet seed for IParticle.
     **/
    StatusCode getRingSetSeed(
        const xAOD::RingSetConf::RawConf &/*rawConf*/,
        const xAOD::IParticle &part,
        AtlasGeoPoint &seed);
    /// @}

    /// Tool props (python configurables):
    /// @{
    /**
     * @brief Name of CaloRingsContainer on Event StoreGate
     **/
    /** @brief electron collection input name*/
    SG::WriteHandleKey<xAOD::CaloRingsContainer> m_crContName {this,
      "CaloRingsContainerName",
      "CaloRings",
      "Name of the CaloRings container"};

    /**
     * @brief Name of RingSetContainer on Event StoreGate
     **/
    SG::WriteHandleKey<xAOD::RingSetContainer> m_rsContName {this,
      "RingSetContainerName",
      "RingSets",
      "Name of the RingSets container"};

    /**
     * @brief Name of CaloCellContainer
     **/
    SG::ReadHandleKey<CaloCellContainer> m_cellsContName {this,
      "CellsContainerName",
      "AllCalo",
      "Key to obtain the cell container"};

    /**
     * @brief Width of the ring in eta
     **/
    Gaudi::Property<std::vector<float>> m_etaWidth {this,
          "EtaWidth", {}, "Each RingSet ring eta width."};
    /**
     * @brief Width of the ring in phi
     **/
    Gaudi::Property<std::vector<float>> m_phiWidth {this,
          "PhiWidth", {}, "Each RingSet ring phi width."};
    /**
     * @brief Maximum cell distance in eta to seed
     **/
    Gaudi::Property<float>  m_cellMaxDEtaDist{this,
          "CellMaxDEtaDist", 0,
          "Maximum cell distance to the seed in eta."};
    /**
     * @brief Maximum cell distance in phi to seed
     **/
    Gaudi::Property<float>  m_cellMaxDPhiDist{this,
          "CellMaxDPhiDist", 0,
          "Maximum cell distance to the seed in phi."};
    /**
     * @brief Number of rings in a ringset
     **/
    Gaudi::Property<std::vector<unsigned int>>  m_nRings{this,
          "NRings", {},
          "Each RingSet number of rings."};
    /**
     *  * @brief Minimum particle energy to build rings (GeV)
     *   **/
    Gaudi::Property<float>  m_minEnergy{this,
          "MinPartEnergy", std::numeric_limits<float>::lowest(),
          "Minimum particle/cluster energy to build rings (GeV)."};
    /**
     *
     * @brief Calorimeter layers in each ringset
     *
     * This property should be read as:
     * std::vector<CaloSampling::CaloSample> m_layersRings;
     *
     **/
    Gaudi::Property<std::vector<int/*=CaloCell_ID::CaloSample*/>>
      m_layers{this,
          "Layers", {},
          "Concatenated list of layers which will be used "
          "to build the RingSets"};
    /**
     * @brief Number of calorimeter layers in each ringset
     **/
    Gaudi::Property<std::vector<unsigned int>> m_nLayers{this,
            "RingSetNLayers", {},
            "Each RingSet number of layers from the Layers "
            "configurable property to use."};
    /**
     * @brief Switch to use shower barycenter seed for each RingSets.
     *
     * This can be set to false if using cluster information to use the cluster
     * center instead of its layer energy baricenter.
     **/
    Gaudi::Property<bool> m_useShowShapeBarycenter{this,
            "useShowerShapeBarycenter", false,
            "Switch to use shower barycenter for each layer, "
            "instead of the cluster center."};


    SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{this,"CaloDetDescrManager", "CaloDetDescrManager"}; 

    /// @}

    /// Tool props (non configurables):
    /// @{
    /**
     * @brief Create and hold RingSetContainer for each event
     **/
    xAOD::RingSetContainer* m_rsCont;
    /**
     * @brief Create and hold CaloRingsContainer for each event
     **/
    xAOD::CaloRingsContainer* m_crCont;
    /**
     * @brief holds each RingSet configuration (filled at initialize)
     **/
    xAOD::RingSetConf::RawConfCollection m_rsRawConfCol;
    /**
     * @brief last valid RingSet seed
     *
     * Propagate other layer centroids in case a layer centroid is not valid.
     **/
    AtlasGeoPoint m_lastValidSeed;
    /**
     * @brief used for phi wrap-around
     **/
    static CaloPhiRange m_phiHelper;
    /**
     * @brief hold the number of RingSets we are building for each CaloRings
     **/
    size_t m_nRingSets;
    /// @}

};

} // namespace Ringer

#endif


