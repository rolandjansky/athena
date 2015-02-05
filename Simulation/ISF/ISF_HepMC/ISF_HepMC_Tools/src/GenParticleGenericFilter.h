/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticleGenericFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

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

    class ISFParticle;

    /** used to store a list of PDG particle codes */
    typedef std::vector<int>      PDGCodes;

  /** @class GenParticleGenericFilter
   
      This GenParticle filter provides a general way of selecting/filtering out particles during GenEvent read-in.
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class GenParticleGenericFilter : public AthAlgTool,
                                   virtual public IGenParticleFilter { 
      
    public: 
      /** Constructor with parameters */
      GenParticleGenericFilter( const std::string& t, const std::string& n, const IInterface* p );
      
      /** Destructor */
      ~GenParticleGenericFilter(){}

      /** Athena algtool's Hooks */
      StatusCode  initialize();
      StatusCode  finalize();

      /** Returns the Particle Stack, should register truth */
      bool pass(const HepMC::GenParticle& particle) const;
	  
	private:
      /** the cuts defined by the use */
      double        m_minEta;     //!< min pseudorapidity cut
      double        m_maxEta;     //!< max pseudorapidity cut
      double        m_minPhi;     //!< min phi cut
      double        m_maxPhi;     //!< max phi cut
      double        m_minMom;     //!< min momentum cut
      double        m_maxMom;     //!< max momentum cut
      PDGCodes      m_pdg;        //!< list of PDG particle codes
  }; 
  
}


#endif //> !ISF_HEPMC_GENPARTICLEGENERICFILTER_H
