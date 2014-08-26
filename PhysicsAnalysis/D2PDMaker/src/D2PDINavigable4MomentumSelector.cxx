/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: D2PDINavigable4MomentumSelector.cxx
//
/**
   @class D2PDINavigable4MomentumSelector

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Select INavigable4Momentum and write out good ones as ElementLinks

   Class to select good INavigable4Momentum from an input container (which can either
   be an jet container or a container of ElementLinks poining to an
   INavigable4MomentumCollection) and write out the good ones as a new container of
   ElementLinks pointing to the good INavigable4Momentum in the original input container.
   The INavigable4Momentum is the base class to (as far as I know) every offline object.
   That means that every electron, jet, cluster, track, muon,... inherits from this.
*/
//=============================================================================

// This classes header
#include "D2PDMaker/D2PDINavigable4MomentumSelector.h"

// STL includes
#include <string>
#include <vector>
#include <cfloat>

// Gaudi stuff
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

// INavigable4Momentum includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "EventKernel/INavigable4Momentum.h"




//=============================================================================
// Constructor
//=============================================================================
D2PDINavigable4MomentumSelector::D2PDINavigable4MomentumSelector( const std::string& name,
                                                                  ISvcLocator* pSvcLocator ) :
  D2PDSelectorBase< INavigable4Momentum, INavigable4MomentumCollection >( name, pSvcLocator )
{
}


//=============================================================================
// Destructor
//=============================================================================
D2PDINavigable4MomentumSelector::~D2PDINavigable4MomentumSelector()
{
}


//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode D2PDINavigable4MomentumSelector::initializeMore()
{
  return StatusCode::SUCCESS;
}



//=============================================================================
// Athena execute method
//=============================================================================
StatusCode D2PDINavigable4MomentumSelector::processObject( const INavigable4Momentum* inav4mom,
                                                           bool &isPassed )
{
  // Make sure we have a inav4mom
  if ( inav4mom )
    {
	      
      //--------------------------
      // Do the selections
      //--------------------------

    }
  else
    {
      // Return false 
      isPassed = false;
      ATH_MSG_WARNING ( "Could not dynamic_cast to an inav4mom! Failing this inav4mom!" );
    } // End: if/else have inav4mom


  return StatusCode::SUCCESS;
}





//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode D2PDINavigable4MomentumSelector::finalizeMore()
{
  return StatusCode::SUCCESS;
}




