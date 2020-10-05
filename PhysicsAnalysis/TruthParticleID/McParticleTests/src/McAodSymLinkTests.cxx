///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// McAodSymLinkTests.cxx 
// Implementation file for class McAodSymLinkTests
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <limits>

// FrameWork includes
#include "Gaudi/Property.h"

// NavFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/IParticleContainer.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticleContainer.h"

// McParticleTests includes
#include "McAodSymLinkTests.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
McAodSymLinkTests::McAodSymLinkTests( const std::string& name, 
				      ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  std::string descr;
  descr = "StoreGate location of the TruthParticleContainer we want to test";
  declareProperty( "TruthParticles",
		   m_truthParticlesName = "SpclMC",
		   descr );
}

// Destructor
///////////////
McAodSymLinkTests::~McAodSymLinkTests()
{ 
  ATH_MSG_DEBUG( "Calling destructor"  );
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode McAodSymLinkTests::initialize()
{
  ATH_MSG_INFO( "Initializing " << name() << "..." );
  return StatusCode::SUCCESS;
}

StatusCode McAodSymLinkTests::finalize()
{
  ATH_MSG_INFO( "Finalizing " << name() << "..."  );
  return StatusCode::SUCCESS;
}

StatusCode McAodSymLinkTests::execute()
{  
  ATH_MSG_DEBUG( "Executing " << name() << "..." );

  const TruthParticleContainer * mcParts = 0;
  ATH_CHECK( evtStore()->retrieve( mcParts, 
                                   m_truthParticlesName.value()) );

  const IParticleContainer* iparts = 0;
  ATH_CHECK( evtStore()->retrieve( iparts, 
                                   m_truthParticlesName.value()) );
  
  const INavigable4MomentumCollection* inav = 0;
  ATH_CHECK( evtStore()->retrieve( inav, 
                                   m_truthParticlesName.value()) );
  
  const std::size_t mcPartsSize = mcParts->size();
  const std::size_t ipartsSize  = iparts->size();
  const std::size_t inavSize    = inav->size();

  if ( !( mcPartsSize == ipartsSize  &&
	  mcPartsSize == inavSize    &&
	  ipartsSize  == inavSize    ) ) {
    ATH_MSG_ERROR ("Symlinked containers do not have the same size !!");
    ATH_MSG_ERROR (" TruthParticleContainer :        " << mcPartsSize);
    ATH_MSG_ERROR (" IParticleContainer :            " << ipartsSize);
    ATH_MSG_ERROR (" INavigable4MomentumCollection : " << inavSize);
    return StatusCode::FAILURE;
  }

  bool allGood = true;
  static const double eps = std::numeric_limits<double>::epsilon();
  for ( std::size_t i = 0; i != mcPartsSize; ++i ) {
    const double tp_ene = (*mcParts)[i]->e();
    const double ip_ene = (*iparts)[i]->e();
    const double in_ene = (*inav)[i]->e();
    if ( ! ( (tp_ene - ip_ene) < eps &&
	     (tp_ene - in_ene) < eps &&
	     (ip_ene - in_ene) < eps ) )
    {
      ATH_MSG_ERROR ("symlink FAILS at index [" << i << "]: ");
      ATH_MSG_ERROR (" TruthParticle::e(): " << tp_ene);
      ATH_MSG_ERROR (" IParticle::e():     " << ip_ene);
      ATH_MSG_ERROR (" INav4Mom::e():      " << in_ene);
      ATH_MSG_ERROR (" epsilon<double>:    " << eps);
      allGood = false;
    }
  }
  if (!allGood) {
    return StatusCode::FAILURE;
  }

  // this string is needed for the unit-test
  ATH_MSG_INFO( "McAodSymLink tests OK"  );

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

