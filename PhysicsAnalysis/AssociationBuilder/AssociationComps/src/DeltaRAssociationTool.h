/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef ASSOCIATIONCOMPS_DELTARASSOCIATIONTOOL_H
#define ASSOCIATIONCOMPS_DELTARASSOCIATIONTOOL_H

//============================================================================
// Name: DeltaRAssociationTool.h
//
/**
   @class DeltaRAssociationTool

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date January 2010

   @brief Tool to match an object to every object from an input container
   within a given DeltaR range.

*/
//=============================================================================


// STL includes
#include <string>
#include <vector>
#include <utility>

//Include the base class
#include "AssociationKernel/AssociationBaseTool.h"
#include "AssociationKernel/IAssociationTool.h"

// INav4Mom includes
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INav4MomLinkContainer.h"
#include "NavFourMom/INav4MomLink.h"
#include "NavFourMom/INav4MomAssocs.h"


// Forward declarations
class egamma;
namespace Analysis
{
  class Muon;
}




class DeltaRAssociationTool : public AssociationBaseTool
{

public:

  /** Default constructor */
  DeltaRAssociationTool( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent );

  /** Default destructor */
  virtual ~DeltaRAssociationTool() ;
  

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
  // Methods
  //------------------------------------------------------

  /** method to find the track matching between electron and muon */
  bool findElectronMuonMatch(const Analysis::Muon* objectFrom, const egamma* eg);


private:
  //------------------------------------------------------
  // Options
  //------------------------------------------------------

  /** The name for the UserData variable DeltaR to the macthed object */
  std::string m_userDataMatchDeltaRName;

  /** store only the best match instead of all matches fullfilling the given parameters */
  bool m_storeOnlyBestMatch;


  /** use the calorimeter cluster position of electrons and photons of the object for
      which a match is searched (default=false) */
  bool m_useMatchFromCaloClusterPosition;

  /** use the calorimeter cluster position of electrons and photons of the objects 
      to be matched to (default=false) */
  bool m_useMatchToCaloClusterPosition;

  /** minimum deltaR */
  double m_deltaRMin;

  /** maximum deltaR */
  double m_deltaRMax;

  /** Limit for the (e,mu) track matching - only for the FSR matching case*/
  double m_trackMatchCut;

  /** Get the FSR candidates - off by default */
  bool m_doFsr;



private:
  //------------------------------------------------------
  // Internally used variables
  //------------------------------------------------------
  /** Create a vector that will hold the results of matching as a pair of DeltaR, ElementLink */
  std::vector< std::pair< double, INav4MomLink > >* m_resultVecPair;

  /** Create a vector that will hold the DeltaR results of the matching */
  std::vector< double >* m_resultVecDeltaR;

  /// Counter of how often the method was called
  unsigned long m_nCalls;

}; // End class DeltaRAssociationTool


#endif
