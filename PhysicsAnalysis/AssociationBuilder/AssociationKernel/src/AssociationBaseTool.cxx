/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: AssociationBaseTool.h
//
/**
   @class AssociationBaseTool

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date March 2010

   @brief This is the base class for all tools that will be used to calculate 
          associations between objects and store the association result into
          StoreGate using the INav4MomAssocs class. Additional information can
          be also written to the output file using the UserDataSvc. This class
          is meant to simplyfy life.
*/
//=============================================================================

// This classes header
#include "AssociationKernel/AssociationBaseTool.h"

// STL includes
#include <string>
#include <vector>
#include <iostream>

// particles includes
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomAssocs.h"
#include "NavFourMom/INav4MomLink.h"



//=============================================================================
// Constructor
//=============================================================================
AssociationBaseTool::AssociationBaseTool( const std::string& type, 
                                          const std::string& name,
                                          const IInterface* parent ) :
  AthAlgTool( type, name, parent )
{
  // declare interface
  declareInterface<IAssociationTool>( this );

  // Properties go here
  declareProperty("inputAssociateToCollection", m_matchToCollKey = "",    "Input (link) collections to match to" );
  declareProperty("userDataPrefix",             m_userDataPrefix = "",    "Prefix for the name of the UserData variables" );
  declareProperty("writeUserData",              m_writeUserData  = false, "Flag to write out the UserData" );
}



//=============================================================================
// Destructor
//=============================================================================
AssociationBaseTool::~AssociationBaseTool()
{
}




//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode AssociationBaseTool::initialize()
{
  // Print the used configuration
  ATH_MSG_DEBUG ( "==> initialize " << name() << "..." );
  ATH_MSG_DEBUG ( " using inputAssociateToCollection = " << m_matchToCollKey );
  ATH_MSG_DEBUG ( " using userDataPrefix             = " << m_userDataPrefix );
  ATH_MSG_DEBUG ( " using writeUserData              = " << m_writeUserData );

  return StatusCode::SUCCESS;
}




// =============================================================================
// Athena finalize method
// =============================================================================
StatusCode AssociationBaseTool::finalize()
{
  return StatusCode::SUCCESS;
}





// =============================================================================
// Calculate the Associations and store the results
// =============================================================================
StatusCode AssociationBaseTool::calculateAssociations( const INav4MomLink& objectLink,
                                                       INav4MomAssocs& inav4MomAssocs )
{
  // Make sure we have a valid link
  if ( objectLink.isValid() )
    {
      // Get the pointer to the container that containst the positive matches
      const INavigable4MomentumCollection* objectsContainer = objectLink.getStorableObjectPointer();
      
      // Get the index of the matched object in the container that containst the positive matches
      unsigned int objectIndex = objectLink.index();
      
      return calculateAssociations( objectsContainer, objectIndex, inav4MomAssocs );
    }
  else
    {
      ATH_MSG_ERROR ( "Found a non-valid ElementLink in tool " << name() );
      return StatusCode::FAILURE;
    }
}


//=============================================================================
// Calculate the Associations for a simple element (INavigable4Momentum)
//=============================================================================
StatusCode AssociationBaseTool::calculateAssociations( const INavigable4MomentumCollection* ,
                                                       const size_t ,
                                                       INav4MomAssocs& )
{
  return StatusCode::SUCCESS;
}
 

