/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingerReaderUtils.h 688728 2015-08-11 16:01:24Z ssnyder $
#ifndef CALORINGERTOOLS_CALORINGERREADERUTILS_H
#define CALORINGERTOOLS_CALORINGERREADERUTILS_H

// STL includes:
#include <string>
#include <memory>
#include <vector>

// Athena framework include:
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthContainers/DataVector.h"

// xAOD include:
#include "xAODCaloRings/tools/getCaloRingsDecorator.h"

// Interface includes:
#include "CaloRingerTools/ICaloRingsBuilder.h"

// Local includes:
#include "RingerKnownParticles.h"

// Forward declarations:
class CaloCellContainer;
class MsgStream;

namespace Ringer {

/**
 * @class BuildCaloRingsFctorBase
 * @brief Interface for CaloRings builder functor.
 **/
class BuildCaloRingsFctorBase {

  protected:
    /**
     * @brief Main ctor
     * Initialize values.
     **/
    BuildCaloRingsFctorBase(
        ToolHandle<ICaloRingsBuilder> &builder,
        MsgStream &msg):
          m_builder(builder),
          m_msg(msg),
          m_part_counter(0),
          m_part_size(0),
          m_crContH(nullptr),
          m_rsContH(nullptr)
      {;}


    /// Protected Properties
    /// @{
    /// @brief CaloRings which will be used
    ToolHandle<ICaloRingsBuilder> &m_builder;
    /// @brief Message Stream which will be used:
    MsgStream &m_msg;
    /// @brief Hold number of particles already procesed for this event:
    mutable size_t m_part_counter;
    /// @brief Hold number of particles to be processed:
    size_t m_part_size;
    /// @}

    /// Methods
    /// @{
    /**
     * @brief Set containers and prepare to loop for nParticles
     **/
    StatusCode prepareToLoopFor( std::size_t nParticles );

    /**
     * @brief Increment particle looping counter
     **/
    void incrementCounter() const { ++m_part_counter; }

    /**
     * @brief Release the handles when finished looping
     **/
    void checkRelease() const;

    /// @}
    virtual ~BuildCaloRingsFctorBase(){;}

  private:
    /// Private Properties
    /// @{
    /// @brief Keep CaloRingsContainer handle in scope until finished looping
    mutable SG::WriteHandle<xAOD::CaloRingsContainer>* m_crContH;
    /// @brief Keep RingSetContainer handle in scope until finished looping
    mutable SG::WriteHandle<xAOD::RingSetContainer>* m_rsContH;
    /// @}

};

/**
 * @class BuildCaloRingsFctorWithCluster<container_t>
 * @brief Template for CaloRings builder functor.
 **/
template< typename container_t >
class BuildCaloRingsFctor : public BuildCaloRingsFctorBase
{
  public:
    /// @brief Particle type that the functor will decorate the CaloRingsLinks
    typedef typename container_t::base_value_type particle_t;
    /// @brief CaloRings links decorator handle type
    typedef typename ::xAOD::caloRingsDecoH_t< container_t > decor_t;

  private:
    typedef BuildCaloRingsFctorBase base_t;
    // Import base template properties:
    using base_t::m_builder;
    using base_t::m_msg;
    using base_t::m_part_counter;
    using base_t::m_part_size;

    using base_t::checkRelease;

    /// @brief Holds particle name:
    static constexpr const char *m_particle_name = Ringer::GetParticleProp<particle_t>::name;
    /// @brief Holds if particle has cluster access:
    static constexpr bool m_particle_has_cluster = Ringer::GetParticleProp<particle_t>::hasCluster;

    /**
     * @brief Shows current looping status.
     **/
    void displayLoopingMessage() const;

    /**
     * @brief Decorator key
     **/
    SG::WriteDecorHandleKey< container_t > m_decorKey;

    /**
     * @brief Write decorator handle
     **/
    mutable decor_t* m_decor;

  public:
    /**
     * @brief Main ctor, repass information for interface
     **/
    BuildCaloRingsFctor(
        const std::string &decoContName,
        ToolHandle<ICaloRingsBuilder> &builder,
        MsgStream &msg)
    : BuildCaloRingsFctorBase(
        builder,
        msg),
      m_decorKey( decoContName + "." + xAOD::caloRingsLinksDecorKey() ){;}

    /// Methods
    /// @{
    /**
     * @brief Looping functor method when it has access to cluster.
     **/
    void operator() (const particle_t *part) const;

    /**
     * @brief Initialize the decorator keys
     **/
    StatusCode initialize();

    /**
     * @brief Prepare to loop for nParticles.
     **/
    StatusCode prepareToLoopFor( std::size_t nParticles );

    /**
     * @brief Release the handles when finished looping
     **/
    void checkRelease() const;
    /// @}
};

} // namespace Ringer

#endif // CALORINGERTOOLS_CALORINGERREADERUTILS_H

#ifndef INCLUDE_HEADER_ONLY // Use to avoid circular includes:
#include "CaloRingerReaderUtils.icc"
#endif // INCLUDE_HEADER_ONLY

