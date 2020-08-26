/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_D2PDMUONSELECTOR_H
#define D2PDMAKER_D2PDMUONSELECTOR_H

//============================================================================
// Name: D2PDMuonSelector.h
//
/**
   @class D2PDMuonSelector

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Select muons and write out good ones as ElementLinks

   Class to select good muons from an input container (which can either
   be an muon container or a container of ElementLinks poining to an
   muon container) and write out the good ones as a new container of
   ElementLinks pointing to the good muons in the original input container.
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

// muon includes
#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"

// Include the filter tool
#include "D2PDMaker/FilterTool.h"
#include "AnalysisUtils/KinematicSelector.h"
#include "AnalysisUtils/KinematicSelectorCore.h"



class D2PDMuonSelector : public D2PDSelectorBase< Analysis::Muon, Analysis::MuonContainer >
{

public:

  /** Default constructor */
  D2PDMuonSelector(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default destructor */
  virtual ~D2PDMuonSelector() ;


private:
  /** This method is executed once at the end of the 
      standard athena initialize() method. */
  StatusCode initializeMore();

  /** This method is executed ONCE PER EVENT, right before
      the loop over the muons begins. */
  //StatusCode executeMore();

  /** This method is executed ONCE FOR EVERY PROCESSED MUON. */
  StatusCode processObject( const Analysis::Muon* muon,
                            bool &isPassed );

  /** This method is executed once at the end of the 
      standard athena finalize() method. */
  StatusCode finalizeMore();


private:

  /// Call-back method to configure the cut properties (min/max): mass
  void setupChi2( Gaudi::Details::PropertyBase& prop );


  //------------------------------------------------------
  // Selections
  //------------------------------------------------------

  /// do accept author cut
  bool m_doAcceptAuthor;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxAcceptAuthor;


  /** require isCombined selection */
  bool m_requireIsCombined;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRequireIsCombined;


  /** accept isCombined selection */
  bool m_acceptIsCombined;


  /** veto isCombined selection */
  bool m_vetoIsCombined;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxVetoIsCombined;


  /** require isStandAlone selection */
  bool m_requireIsStandAlone;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRequireIsStandAlone;


  /** accept isStandAlone selection */
  bool m_acceptIsStandAlone;

  /** veto isStandAlone selection */
  bool m_vetoIsStandAlone;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxVetoIsStandAlone;


  /** require isCaloMuonId selection */
  bool m_requireIsCaloMuonId;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRequireIsCaloMuonId;


  /** accept isCaloMuonId selection */
  bool m_acceptIsCaloMuonId;

  /** veto isCaloMuonId selection */
  bool m_vetoIsCaloMuonId;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxVetoIsCaloMuonId;


  /** require isSegmentTagged selection */
  bool m_requireIsSegmentTagged;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRequireIsSegmentTagged;


  /** accept isSegmentTagged selection */
  bool m_acceptIsSegmentTagged;

  /** veto isSegmentTagged selection */
  bool m_vetoIsSegmentTagged;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxVetoIsSegmentTagged;


  /** require isLoose selection */
  bool m_requireIsLoose;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRequireIsLoose;


  /** require isMedium selection */
  bool m_requireIsMedium;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRequireIsMedium;


  /** require isTight selection */
  bool m_requireIsTight;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRequireIsTight;




  /// do accept author list cut
  bool m_doAcceptAuthorList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxAcceptAuthorList;

  /** author selection list; logical OR */
  std::vector<unsigned int> m_acceptAuthorList;


  /// do require author list cut
  bool m_doRequireAuthorList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRequireAuthorList;

  /** author selection list; logical AND */
  std::vector<unsigned int> m_requireAuthorList;


  /// do veto author list cut
  bool m_doVetoAuthorList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxVetoAuthorList;

  /** author selection list; logical NOT */
  std::vector<unsigned int> m_vetoAuthorList;



  /// do muon parameter list cut
  bool m_doMuonParameterList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxMuonParameterList;

  /** list of MuonParameters::ParamDef (e.g., isolation quantities) */
  std::vector<unsigned long> m_muonParameterList;

  /** list of minimum cut values for the MuonParameter list */
  std::vector<double> m_muonParameterMinList;

  /** list of maximum cut values for the MuonParameter list */
  std::vector<double> m_muonParameterMaxList;

  /** list of bools to decide if one should use relative 
      (devided by muon.pt) cuts for the MuonParameter list */
  std::vector<bool> m_muonParameterUseRelCutsList;



  /// do muon chi2 cut
  bool m_doChi2;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxChi2;

  /** minumum matchChi2OverDoF */
  DoubleProperty m_matchChi2OverDoFMin;

  /** maxumum matchChi2OverDoF */
  DoubleProperty m_matchChi2OverDoFMax;




}; // End class D2PDMuonSelector



///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline void D2PDMuonSelector::setupChi2( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doChi2 = true;
  return;
}



#endif
