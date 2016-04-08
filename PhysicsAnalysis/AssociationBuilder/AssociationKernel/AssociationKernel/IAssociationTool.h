/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef ASSOCIATIONKERNEL_IASSOCIATIONTOOL_H
#define ASSOCIATIONKERNEL_IASSOCIATIONTOOL_H

//============================================================================
// Name: IAssociationTool.h
//
/**
   @class IAssociationTool

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date March 2010

   @brief This is the interface class for all tools that will be used to calculate 
          associations between objects and store the association result into
          StoreGate using the INav4MomAssocs class.
*/
//=============================================================================

// Athena Tool Interface
#include "GaudiKernel/IAlgTool.h"

// particles includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomLink.h"

// forward declarations
class INav4MomAssocs;


static const InterfaceID IID_IAssociationTool("IAssociationTool", 1, 0);


class IAssociationTool
  : virtual public ::IAlgTool
{
public:

  /** AlgTool interface methods */
  static const InterfaceID& interfaceID();


public:  
  /** Calculate the Associations and store the results */
  virtual StatusCode calculateAssociations( const INavigable4MomentumCollection* objectContainer,
                                            const size_t objectIndex,
                                            INav4MomAssocs& inav4MomAssocs ) = 0;


  /** Calculate the Associations and store the results */
  virtual StatusCode calculateAssociations( const INav4MomLink& objectLink,
                                            INav4MomAssocs& inav4MomAssocs ) = 0;

}; // End class IAssociationTool


///////////////////////////////////////////////////////////////////
/// Inline methods:
///////////////////////////////////////////////////////////////////
inline const InterfaceID& IAssociationTool::interfaceID() 
{
  return IID_IAssociationTool; 
}

#endif
