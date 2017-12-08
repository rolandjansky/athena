/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthAssocSimSelector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_TRUTHASSOCSIMSELECTOR_H
#define ISF_TOOLS_TRUTHASSOCSIMSELECTOR_H 1

// STL includes
#include <vector>
#include <set>

// ISF includes
#include "ISF_Interfaces/ISimulationSelector.h"

// HepMC includes
#include "HepMC/IteratorRange.h"

namespace ISF
{

  /** @class TruthAssocSimSelector

      SimulationSelector using the TruthAssociation of ISFParticles, eg:
      * check on parent particle
      * check on production process

      @author Elmar.Ritsch -at- cern.ch
  */
  class TruthAssocSimSelector : public ISimulationSelector
  {

  public:
    /** Constructor with parameters */
    TruthAssocSimSelector( const std::string& t, const std::string& n, const IInterface* p );

    /** Destructor */
    ~TruthAssocSimSelector();

    // Athena algtool's Hooks
    virtual StatusCode  initialize() override final;
    virtual StatusCode  finalize() override final;

    /** check whether given particle passes all cuts -> will be used for routing decision*/
    virtual bool passSelectorCuts(const ISFParticle& particle) const override final;

  private:
    /** ISFParticle has to have a relative which is in this list */
    std::vector<int>                      m_relativesVec; //!< Python property
    std::set<int>                         m_relatives;    //!< used during runtime (faster)


    /** ISFParticle relation to the given pdg codes */
    int                                   m_relationProp; //!< Python property
    HepMC::IteratorRange                  m_relation;     //!< HepMC

  };

}

#endif //> !ISF_TOOLS_TRUTHASSOCSIMSELECTOR_H
