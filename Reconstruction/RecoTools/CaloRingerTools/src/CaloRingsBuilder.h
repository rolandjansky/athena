/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingsBuilder.h 713524 2015-12-09 08:56:24Z wsfreund $
#ifndef CALORINGERTOOLS_CALORINGSBUILDER_H
#define CALORINGERTOOLS_CALORINGSBUILDER_H

// Base includes:
#include "CaloRingerTools/ICaloRingsBuilder.h"
#include "AthenaBaseComps/AthAlgTool.h"

//CxxUtils for override final
#include "CxxUtils/final.h"
#include "CxxUtils/override.h"

// STL includes:
#include <string>
#include <vector>
#include <memory>

// Wrap-around phi helper include:
#include "CaloGeoHelpers/CaloPhiRange.h"

// xAOD Includes:
#include "xAODBase/IParticle.h"
#include "xAODCaloRings/RingSetContainerFwd.h"
#include "xAODCaloRings/CaloRingsContainerFwd.h"
#include "xAODCaloRings/tools/AtlasGeoPoint.h"

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
    virtual StatusCode initialize() ATH_OVERRIDE;
    /**
     * @brief method for working on containers
     **/
    virtual StatusCode contExecute(const size_t nReserve = 0) ATH_OVERRIDE;
    /**
     * @brief build CaloRings for IParticle
     **/
    virtual StatusCode execute(const xAOD::IParticle &particle,
        ElementLink<xAOD::CaloRingsContainer> &clRingsLink) ATH_OVERRIDE;
    /**
     * @brief build CaloRings for CaloCluster
     **/
    virtual StatusCode execute(const xAOD::CaloCluster &cluster,
        ElementLink<xAOD::CaloRingsContainer> &clRingsLink) ATH_OVERRIDE;
    /**
     * @brief finalize method
     **/
    virtual StatusCode finalize() ATH_OVERRIDE;
    /// @}

    /// Extra methods:
    /// @{
    /**
     * @brief Retrieve RawConfiguration Collection
     **/
    const xAOD::RingSetConf::RawConfCollection &rawConf() ATH_OVERRIDE {
      return m_rsRawConfCol;
    };
    /**
     * @brief Retrieve whether it was set used shower shape barycenter
     **/
    bool useShowerShapeBarycenter() ATH_OVERRIDE {
      return m_useShowShapeBarycenter;
    };
    /// @}


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
    std::string m_crContName;
    /**
     * @brief Name of RingSetContainer on Event StoreGate
     **/
    std::string m_rsContName;
    /**
     * @brief Name of CaloCellContainer
     **/
    std::string m_cellsContName;
    /**
     * @brief Width of the ring in eta
     **/
    std::vector<float> m_etaWidth;
    /**
     * @brief Width of the ring in phi
     **/
    std::vector<float> m_phiWidth;
    /**
     * @brief Maximum cell distance in eta to seed
     **/
    float m_cellMaxDEtaDist;
    /**
     * @brief Maximum cell distance in phi to seed
     **/
    float m_cellMaxDPhiDist;
    /**
     * @brief Number of rings in a ringset
     **/
    std::vector<unsigned int> m_nRings;
    /**
     *  * @brief Minimum particle energy to build rings (GeV)
     *   **/
    float m_minEnergy;
    /**
     *
     * @brief Calorimeter layers in each ringset
     *
     * This property should be read as:
     * std::vector<CaloSampling::CaloSample> m_layersRings;
     *
     **/
    std::vector<int/*=CaloCell_ID::CaloSample*/> m_layers;
    /**
     * @brief Number of calorimeter layers in each ringset
     **/
    std::vector<unsigned int> m_nLayers;
    /**
     * @brief Switch to use shower barycenter seed for each RingSets.
     *
     * This can be set to false if using cluster information to use the cluster
     * center instead of its layer energy baricenter.
     **/
    bool m_useShowShapeBarycenter;
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
     * @brief Hold the retrieved cells container.
     **/
    const CaloCellContainer* m_cellCont;
    /**
     * @brief holds each RingSet configuration (filled at initialize)
     **/
    xAOD::RingSetConf::RawConfCollection m_rsRawConfCol;
    /**
     * @brief last valid RingSet seed
     *
     * Propagate third EM layer position to hadronic layers.
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


