/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RandomSimSelector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_RANDOMSIMFILTER_H
#define ISF_TOOLS_RANDOMSIMFILTER_H 1

// ISF includes
#include "BaseSimulationSelector.h"

class TRandom;

namespace ISF
{

  /** @class RandomSimSelector

      This SimlationSelector implementation will select all particles that are handed to it.
      ( passFilter() always returns true )

      @author Elmar.Ritsch -at- cern.ch
  */
  class RandomSimSelector : public BaseSimulationSelector
  {

  public:
    /** Constructor with parameters */
    RandomSimSelector( const std::string& t, const std::string& n, const IInterface* p );

    /** Destructor */
    ~RandomSimSelector();

    // Athena algtool's Hooks
    StatusCode  initialize() override final;
    StatusCode  finalize() override final;

    /** check whether given particle passes all cuts -> will be used for routing decision*/
    inline virtual bool passSelectorCuts(const ISFParticle& particle) const override final;

  private:

    mutable TRandom* m_random;
  };

}


#endif //> !ISF_TOOLS_RANDOMSIMFILTER_H
