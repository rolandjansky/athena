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

// Interface includes:
#include "CaloRingerTools/ICaloRingsBuilder.h"
#include "CaloRingerTools/ICaloRingerInputReader.h"

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
          m_part_size(0)
      {;}

  
    /// Properties 
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
     * @brief Prepare to loop for nParticles
     **/
    void prepareToLoopFor(const size_t nParticles) { 
      // Initiate container which will hold the CaloRings and reserve one slot per
      // particle as initial guess:
      if( m_builder->contExecute( nParticles ).isFailure() ){
        throw std::runtime_error("Couldn't instantiate CaloRings builder container.");
      }
      // Reset counters, set number of particles to nParticles:
      m_part_size = nParticles;
      m_part_counter = 0;
    }
    /// @}
    virtual ~BuildCaloRingsFctorBase(){;}

};

/**
 * @class BuildCaloRingsFctorWithCluster<particle_t>
 * @brief Template for CaloRings builder functor.
 **/
template< typename particle_t >
class BuildCaloRingsFctor : public BuildCaloRingsFctorBase
{

  private:
    typedef BuildCaloRingsFctorBase base_t;
    // Import base template properties:
    using base_t::m_builder;
    using base_t::m_msg;
    using base_t::m_part_counter;
    using base_t::m_part_size;

    /// @brief Holds particle name:
    static constexpr const char *m_particle_name = 
      Ringer::GetParticleProp<particle_t>::name;
    /// @brief Holds if particle has cluster access:
    static constexpr bool m_particle_has_cluster = 
      Ringer::GetParticleProp<particle_t>::hasCluster;

  public:
    /**
     * @brief Main ctor, repass information for interface 
     **/
    BuildCaloRingsFctor(
        ToolHandle<ICaloRingsBuilder> &builder,
        MsgStream &msg)
    : BuildCaloRingsFctorBase(
        builder,
        msg){;}

    /// Methods 
    /// @{
    /**
     * @brief Looping functor method when it has access to cluster. 
     **/
    void operator() (particle_t *part) const;

    /**
     * @brief Shows current looping status.
     **/
    void displayLoopingMessage() const;

    /**
     * @brief Prepare to loop for nParticles. 
     **/
    using base_t::prepareToLoopFor;
    /// @}
};

} // namespace Ringer

#endif // CALORINGERTOOLS_CALORINGERREADERUTILS_H

#ifndef INCLUDE_HEADER_ONLY // Use to avoid circular includes:
#include "CaloRingerReaderUtils.icc"
#endif // INCLUDE_HEADER_ONLY

