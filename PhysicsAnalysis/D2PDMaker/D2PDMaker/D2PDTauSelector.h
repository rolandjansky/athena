/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_D2PDTAUSELECTOR_H
#define D2PDMAKER_D2PDTAUSELECTOR_H

//============================================================================
// Name: D2PDTauSelector.h
//
/**
   @class D2PDTauSelector

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Select taus and write out good ones as ElementLinks

   Class to select good taus from an input container (which can either
   be an tau container or a container of ElementLinks poining to an
   tau container) and write out the good ones as a new container of
   ElementLinks pointing to the good taus in the original input container.
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

// tau includes
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"

// Include the filter tool
#include "D2PDMaker/FilterTool.h"


class D2PDTauSelector : public D2PDSelectorBase< Analysis::TauJet, Analysis::TauJetContainer >
{
public:

  /** Default constructor */
  D2PDTauSelector(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default destructor */
  virtual ~D2PDTauSelector() ;
  

private:
  /** This method is executed once at the end of the 
      standard athena initialize() method. */
  StatusCode initializeMore();

  /** This method is executed ONCE PER EVENT, right before
      the loop over the electrons begins. */
  //StatusCode executeMore();

  /** This method is executed ONCE FOR EVERY PROCESSED TAU. */
  StatusCode processObject( const Analysis::TauJet* tau,
                            bool &isPassed );

  /** This method is executed once at the end of the 
      standard athena finalize() method. */
  StatusCode finalizeMore();


private:

  /** Get a handle on the filter tool */
  FilterTool* m_filterTool;


private:

  //------------------------------------------------------
  // Call-back methods
  //------------------------------------------------------

  /// Call-back method to configure the cut properties (min/max): IsTau
  void setupIsTau( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): Author
  void setupAuthor( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): Discriminant
  void setupDiscriminant( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): Charge
  void setupCharge( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): 
  void setupNTracks( Gaudi::Details::PropertyBase& prop );



  //------------------------------------------------------
  // Selections
  //------------------------------------------------------

  /// do author cut
  bool m_doAuthor;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxAuthor;

  /** author selection */
  LongProperty m_author;


  /// do  cut
  bool m_doAcceptAuthorList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxAcceptAuthorList;

  /** author selection list; logical OR */
  std::vector<long> m_acceptAuthorList;


  /// do  cut
  bool m_doRequireAuthorList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRequireAuthorList;

  /** author selection list; logical AND */
  std::vector<long> m_requireAuthorList;


  /// do  cut
  bool m_doVetoAuthorList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxVetoAuthorList;

  /** author selection list; logical NOT */
  std::vector<long> m_vetoAuthorList;



  /// do  cut
  bool m_doIsTau;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxIsTau;

  /** tauID isTau selection */
  LongProperty m_isTau;


  /// do  cut
  bool m_doAcceptIsTauList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxAcceptIsTauList;

  /** tauID isTau selection list; logical OR */
  std::vector<long> m_acceptIsTauList;


  /// do  cut
  bool m_doRequireIsTauList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRequireIsTauList;

  /** tauID isTau selection list; logical AND */
  std::vector<long> m_requireIsTauList;


  /// do  cut
  bool m_doVetoIsTauList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxVetoIsTauList;

  /**tauID isTau selection list; logical NOT */
  std::vector<long> m_vetoIsTauList;


  /// do  cut
  bool m_doTauDiscriminant;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxTauDiscriminant;

  /** tauID discriminant selection */
  LongProperty m_tauDiscriminant;

  /** tauID discriminant selection, minimum allowed value */
  DoubleProperty m_tauDiscriminantMin;

  /** tauID discriminant selection, maximum allowed value */
  DoubleProperty m_tauDiscriminantMax;


  /// do  cut
  bool m_doCharge;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxCharge;

  /** minimum charge of tau */
  IntegerProperty m_chargeMin;

  /** maximim charge of tau */
  IntegerProperty m_chargeMax;


  /// do  cut
  bool m_doAcceptChargeList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxAcceptChargeList;

  /** charge list of tau for accept (logical OR combination)*/
  std::vector<int> m_acceptChargeList;


  /// do  cut
  bool m_doVetoChargeList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxVetoChargeList;

  /** charge list of tau for veto (logical NOT combination)*/
  std::vector<int> m_vetoChargeList;


  /** allow charge conjugate */
  bool m_allowChargeConj;


  /// do  cut
  bool m_doNTracks;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxNTracks;

  /** minimum number of tracks of tau */
  UnsignedIntegerProperty m_nTracksMin;

  /** maximim number of tracks of tau */
  UnsignedIntegerProperty m_nTracksMax;


  /// do  cut
  bool m_doAcceptNTracksList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxAcceptNTracksList;

  /** nTracks list of tau for accept (logical OR combination)*/
  std::vector<unsigned int> m_acceptNTracksList;


  /// do  cut
  bool m_doVetoNTracksList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxVetoNTracksList;

  /** nTracks list of tau for veto (logical NOT combination)*/
  std::vector<unsigned int> m_vetoNTracksList;


  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRequireTauSafeLooseID;

  /** tau save loose ID */
  bool m_requireTauSafeLooseID;


}; // End class D2PDTauSelector



///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline void D2PDTauSelector::setupIsTau( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doIsTau = true;
  return;
}

inline void D2PDTauSelector::setupAuthor( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doAuthor = true;
  return;
}

inline void D2PDTauSelector::setupDiscriminant( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doTauDiscriminant = true;
  return;
}

inline void D2PDTauSelector::setupCharge( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doCharge = true;
  return;
}

inline void D2PDTauSelector::setupNTracks( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doNTracks = true;
  return;
}


#endif
