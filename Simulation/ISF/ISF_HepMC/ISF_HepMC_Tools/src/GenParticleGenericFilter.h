/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Elmar Ritsch
 * @date October 2016
 * @brief A generic particle filter tool for HepMC::GenParticle types
 */

#ifndef ISF_HEPMC_GENPARTICLEGENERICFILTER_H
#define ISF_HEPMC_GENPARTICLEGENERICFILTER_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_HepMC_Interfaces/IGenParticleFilter.h"

namespace ISF {

// ISF forward declarations
class ISFParticle;

/// used to store a list of PDG particle codes
typedef std::vector<int>      PDGCodes;


/**
 * @class GenParticleGenericFilter
 * @headerfile GenParticleGenericFilter.h
 *
 * @brief Core Athena algorithm for the Integrated Simulation Framework
 *
 * This GenParticle filter provides a general way of selecting/filtering out particles
 * during GenEvent read-in.
 */
  class GenParticleGenericFilter : public extends<AthAlgTool, IGenParticleFilter> {

  public:
    /// Constructor with framework parameters
    GenParticleGenericFilter( const std::string& t, const std::string& n, const IInterface* p );

    /// Empty Destructor
    ~GenParticleGenericFilter(){}

    /// Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();

    /// Interface method that returns whether the given particle passes all cuts or not
    bool pass(const HepMC::GenParticle& particle) const;

  private:
    /// Check whether the given particle passes all configure cuts or not
    bool check_cuts_passed(const HepMC::GenParticle& particle) const;

    /// the cuts defined by the use
    double        m_minEta;     //!< min pseudorapidity cut
    double        m_maxEta;     //!< max pseudorapidity cut
    double        m_minPhi;     //!< min phi cut
    double        m_maxPhi;     //!< max phi cut
    double        m_minMom;     //!< min momentum cut
    double        m_maxMom;     //!< max momentum cut
    PDGCodes      m_pdgs;       //!< list of accepted particle PDG IDs (any accepted if empty)

    /// Geometrical region (=cylindrical volume around z-axis) within which this filter is applicable
    double        m_maxApplicableRadius;
};

} // ISF namespace


#endif //> !ISF_HEPMC_GENPARTICLEGENERICFILTER_H
