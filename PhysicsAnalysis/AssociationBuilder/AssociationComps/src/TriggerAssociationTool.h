/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef ASSOCIATIONCOMPS_TRIGGERASSOCIATIONTOOL_H
#define ASSOCIATIONCOMPS_TRIGGERASSOCIATIONTOOL_H

//============================================================================
// Name: TriggerAssociationTool.h
//
/**
   @class TriggerAssociationTool

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date January 2010

   @brief Tool to match an object to every object from an input container
   within a given DeltaR range.
   WARNING: Currently, the link to the matched trigger object is NOT saved, 
   but rather a link to the object itself. This way, you only know THAT a 
   match was found, but not which trigger object matched!

*/
//=============================================================================


// STL includes
#include <string>
#include <vector>
#include <utility>

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

//Include the base class
#include "AssociationKernel/AssociationBaseTool.h"
#include "AssociationKernel/IAssociationTool.h"
#include "NavFourMom/INav4MomAssocs.h"

// Forward declaration(s):
class INavigable4Momentum;
class INav4MomLinkContainer;
class TrigMatchTool;

class ElectronContainer;
class PhotonContainer;
class JetCollection;

namespace Analysis
{
  class MuonContainer;
  class TauJetContainer;
}



class TriggerAssociationTool : public AssociationBaseTool
{

public:

  /** Default constructor */
  TriggerAssociationTool( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent );

  /** Default destructor */
  virtual ~TriggerAssociationTool() ;
  

public:
  /** Gaudi Service Interface method implementations */
  StatusCode initialize() ;

  /** Gaudi Service Interface method implementations */
  StatusCode finalize() ;


public:
    
  /** Calculate the Associations and store the results; for general purpose */
  virtual StatusCode calculateAssociations( const INavigable4MomentumCollection* objectContainer,
                                            const size_t objectIndex,
                                            INav4MomAssocs& inav4MomAssocs );

  /** Calculate the Associations and store the results */
  virtual inline StatusCode calculateAssociations( const INav4MomLink& objectLink,
                                            INav4MomAssocs& inav4MomAssocs )
  {
    return AssociationBaseTool::calculateAssociations(objectLink, inav4MomAssocs);
  }


  /** Calculate the Associations and store the results; for electrons */
  virtual StatusCode calculateAssociations( const ElectronContainer* electronContainer,
                                            const size_t electronIndex,
                                            INav4MomAssocs& inav4MomAssocs );

  /** Calculate the Associations and store the results; for photons */
  virtual StatusCode calculateAssociations( const PhotonContainer* photonContainer,
                                            const size_t photonIndex,
                                            INav4MomAssocs& inav4MomAssocs );

  /** Calculate the Associations and store the results; for muons */
  virtual StatusCode calculateAssociations( const Analysis::MuonContainer* muonContainer,
                                            const size_t muonIndex,
                                            INav4MomAssocs& inav4MomAssocs );

  /** Calculate the Associations and store the results; for taus */
  virtual StatusCode calculateAssociations( const Analysis::TauJetContainer* tauContainer,
                                            const size_t tauIndex,
                                            INav4MomAssocs& inav4MomAssocs );

  /** Calculate the Associations and store the results; for jets */
  virtual StatusCode calculateAssociations( const JetCollection* jetContainer,
                                            const size_t jetIndex,
                                            INav4MomAssocs& inav4MomAssocs );



private:
  //------------------------------------------------------
  // Options
  //------------------------------------------------------

  /** Handle for the trigger matching tool */
  ToolHandle< TrigMatchTool > m_matchTool;

  /** The trigger name to which to match to */
  std::string m_triggerName;

  /** Size of the DeltaR cone used for matching */
  double m_deltaR;

  /** Match only to passed triggers */
  bool m_matchToPassedTriggersOnly;

  /** Which trigger level to match to */
  int m_triggerLevel;

  /// Counter of how often the method was called
  unsigned long m_nCalls;

}; // End class TriggerAssociationTool


#endif
