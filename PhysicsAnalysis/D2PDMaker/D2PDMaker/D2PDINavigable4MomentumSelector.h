/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_D2PDINAVIGABLE4MOMENTUMSELECTOR_H
#define D2PDMAKER_D2PDINAVIGABLE4MOMENTUMSELECTOR_H

//============================================================================
// Name: D2PDINavigable4MomentumSelector.h
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

// Include the header of the base class
#include "D2PDMaker/D2PDSelectorBase.h"

// STL includes
#include <string>
#include <vector>

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// INavigable4Momentum includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "EventKernel/INavigable4Momentum.h"



class D2PDINavigable4MomentumSelector : public D2PDSelectorBase< INavigable4Momentum, INavigable4MomentumCollection >
{
public:

  /** Default constructor */
  D2PDINavigable4MomentumSelector(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default destructor */
  virtual ~D2PDINavigable4MomentumSelector() ;
  

private:
  /** This method is executed once at the end of the 
      standard athena initialize() method. */
  StatusCode initializeMore();

  /** This method is executed ONCE PER EVENT, right before
      the loop over the electrons begins. */
  //StatusCode executeMore();

  /** This method is executed ONCE FOR EVERY PROCESSED INavigable4Momentum. */
  StatusCode processObject( const INavigable4Momentum* inav4mom,
                            bool &isPassed );

  /** This method is executed once at the end of the 
      standard athena finalize() method. */
  StatusCode finalizeMore();


private:


  //------------------------------------------------------
  // Selections
  //------------------------------------------------------

}; // End class D2PDINavigable4MomentumSelector


#endif
