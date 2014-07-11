/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef ASSOCIATIONCOMPS_ATHENABARCODEASSOCIATIONTOOL_H
#define ASSOCIATIONCOMPS_ATHENABARCODEASSOCIATIONTOOL_H

//============================================================================
// Name: AthenaBarCodeAssociationTool.h
//
/**
   @class AthenaBarCodeAssociationTool

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date January 2010

   @brief Tool to match an object to every object from an input container
   based on the AthenaBarCodes of both objects.

*/
//=============================================================================

// Include the base class and interface
#include "AssociationKernel/AssociationBaseTool.h"
#include "AssociationKernel/IAssociationTool.h"

// INav4Mom includes
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INav4MomLinkContainer.h"
#include "NavFourMom/INav4MomAssocs.h"




class AthenaBarCodeAssociationTool : public AssociationBaseTool
{

public:

  /** Default constructor */
  AthenaBarCodeAssociationTool( const std::string& type, 
                                const std::string& name,
                                const IInterface* parent );

  /** Default destructor */
  virtual ~AthenaBarCodeAssociationTool() ;
  

public:
  /** Gaudi Service Interface method implementations */
  StatusCode initialize() ;

  /** Gaudi Service Interface method implementations */
  StatusCode finalize() ;


public:
    
  /** Calculate the Associations and store the results */
  virtual StatusCode calculateAssociations( const INavigable4MomentumCollection* objectContainer,
                                            const size_t objectIndex,
                                            INav4MomAssocs& inav4MomAssocs );

  /** Calculate the Associations and store the results */
  virtual inline StatusCode calculateAssociations( const INav4MomLink& objectLink,
                                            INav4MomAssocs& inav4MomAssocs )
  {
    return AssociationBaseTool::calculateAssociations(objectLink, inav4MomAssocs);
  }


private:
  //------------------------------------------------------
  // Options
  //------------------------------------------------------

  /** Require that both objects have an identical AthenaBarCode, 
      including having the same version (default=false) */
  bool m_requireSameAthenaBarCode;

  /// Counter of how often the method was called
  unsigned long m_nCalls;


}; // End class AthenaBarCodeAssociationTool


#endif
