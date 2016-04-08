/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef ASSOCIATIONKERNEL_ASSOCIATIONBASETOOL_H
#define ASSOCIATIONKERNEL_ASSOCIATIONBASETOOL_H

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


// STL includes
#include <string>
#include <vector>
#include <cfloat>

// Athena Tool
#include "AssociationKernel/IAssociationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

// Needed includes for objects
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomLink.h"


// Forward declarations
class INav4MomAssocs;


class AssociationBaseTool
  : virtual public IAssociationTool,
            public AthAlgTool
{
public:
  /** Default constructor */
  AssociationBaseTool( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent );

  /** Default destructor */
  virtual ~AssociationBaseTool() ;

  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();


public:
  /** Calculate the Associations and store the results */
  virtual StatusCode calculateAssociations( const INavigable4MomentumCollection* objectContainer,
                                            const size_t objectIndex,
                                            INav4MomAssocs& inav4MomAssocs );

  /// Calculate the Associations and store the results
  StatusCode calculateAssociations( const INav4MomLink& objectLink,
                                    INav4MomAssocs& inav4MomAssocs );


protected:

  /** Input collection name, used to match/associate to these objects */
  std::string  m_matchToCollKey;

  /** Gets set to true if the input collection is a vector of ElementLinks */
  bool m_inputIsLinkColl;

  /** The prefix for the UserData variables */
  std::string m_userDataPrefix;

  /** Decide if the additional UserData should also be stored into the output file (default=false) */
  bool m_writeUserData;

}; // End class AssociationBaseTool


#endif
