/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: D2PDTauSelector.cxx
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

// This classes header
#include "D2PDMaker/D2PDTauSelector.h"

// STL includes
#include <string>
#include <vector>
#include <cfloat>

// Gaudi stuff
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

// The AthenaBarCode, used to check if two particles are identical
#include "Navigation/IAthenaBarCode.h"

// The needed ElementLink stuff
#include "DataModel/DataVector.h"
#include "DataModel/ElementLink.h" 

// tau includes
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJetParameters.h"
#include "Particle/TrackParticle.h"





//=============================================================================
// Constructor
//=============================================================================
D2PDTauSelector::D2PDTauSelector( const std::string& name,
                                  ISvcLocator* pSvcLocator ) :
  D2PDSelectorBase< Analysis::TauJet, Analysis::TauJetContainer >( name, pSvcLocator ),
  m_filterTool(0),
  m_doAuthor(false),
  m_idxAuthor(0),
  m_doAcceptAuthorList(false),
  m_idxAcceptAuthorList(0),
  m_doRequireAuthorList(false),
  m_idxRequireAuthorList(0),
  m_doVetoAuthorList(false),
  m_idxVetoAuthorList(0),
  m_doIsTau(false),
  m_idxIsTau(0),
  m_doAcceptIsTauList(false),
  m_idxAcceptIsTauList(0),
  m_doRequireIsTauList(false),
  m_idxRequireIsTauList(0),
  m_doVetoIsTauList(false),
  m_idxVetoIsTauList(0),
  m_doTauDiscriminant(false),
  m_idxTauDiscriminant(0),
  m_doCharge(false),
  m_idxCharge(0),
  m_doAcceptChargeList(false),
  m_idxAcceptChargeList(0),
  m_doVetoChargeList(false),
  m_idxVetoChargeList(0),
  m_doNTracks(false),
  m_idxNTracks(0),
  m_doAcceptNTracksList(false),
  m_idxAcceptNTracksList(0),
  m_doVetoNTracksList(false),
  m_idxVetoNTracksList(0),
  m_idxRequireTauSafeLooseID(0)
{
  declareProperty("tauIsTau",                 m_isTau=-1,                "Tau ID IsTau Bitmask" );
  m_isTau.declareUpdateHandler( &D2PDTauSelector::setupIsTau, this );

  declareProperty("tauAcceptIsTauList",       m_acceptIsTauList,         "Tau ID IsTau list for accept (logical OR combination)" );
  declareProperty("tauRequireIsTauList",      m_requireIsTauList,        "Tau ID IsTau list for require (logical AND combination)" );
  declareProperty("tauVetoIsTauList",         m_vetoIsTauList,           "Tau ID IsTau list for veto (logical NOT combination)" );

  declareProperty("tauAuthor",                m_author=-1,               "Tau author" );
  m_author.declareUpdateHandler( &D2PDTauSelector::setupAuthor, this );

  declareProperty("tauAcceptAuthorList",      m_acceptAuthorList,        "Tau author list for accept (logical OR combination)" );
  declareProperty("tauRequireAuthorList",     m_requireAuthorList,       "Tau author list for require (logical AND combination)" );
  declareProperty("tauVetoAuthorList",        m_vetoAuthorList,          "Tau author list for veto (logical NOT combination)" );

  declareProperty("tauDiscriminant",          m_tauDiscriminant=-1,          "TauID discriminant" );
  m_tauDiscriminant.declareUpdateHandler( &D2PDTauSelector::setupDiscriminant, this );
  declareProperty("tauDiscriminantMin",       m_tauDiscriminantMin=-DBL_MAX, "TauID discriminant minumum cut value" );
  m_tauDiscriminantMin.declareUpdateHandler( &D2PDTauSelector::setupDiscriminant, this );
  declareProperty("tauDiscriminantMax",       m_tauDiscriminantMax=DBL_MAX , "TauID discriminant maxumum cut value" );
  m_tauDiscriminantMax.declareUpdateHandler( &D2PDTauSelector::setupDiscriminant, this );

  declareProperty("chargeMin",                m_chargeMin=-INT_MAX,      "Min charge" );
  m_chargeMin.declareUpdateHandler( &D2PDTauSelector::setupCharge, this );
  declareProperty("chargeMax",                m_chargeMax=INT_MAX,       "Max charge" );
  m_chargeMax.declareUpdateHandler( &D2PDTauSelector::setupCharge, this );
  declareProperty("acceptChargeList",         m_acceptChargeList,        "Charge list for accept (logical OR combination)" );
  declareProperty("vetoChargeList",           m_vetoChargeList,          "Charge list for veto (logical NOT combination)" );
  declareProperty("allowChargeConjugate",     m_allowChargeConj=true,    "Allow charge conjugate" );

  declareProperty("minNumberTracks",          m_nTracksMin=0,            "Min number of tracks" );
  m_nTracksMin.declareUpdateHandler( &D2PDTauSelector::setupNTracks, this );
  declareProperty("maxNumberTracks",          m_nTracksMax=INT_MAX,      "Max number of tracks" );
  m_nTracksMax.declareUpdateHandler( &D2PDTauSelector::setupNTracks, this );
  declareProperty("acceptNumberOfTracksList", m_acceptNTracksList,       "Number of tracks list for accept (logical OR combination)" );
  declareProperty("vetoNumberOfTracksList",   m_vetoNTracksList,         "Number of tracks list for veto (logical NOT combination)" );

  declareProperty("requireTauSafeLooseID",    m_requireTauSafeLooseID=false, "Require tau safe loose identification" );
}


//=============================================================================
// Destructor
//=============================================================================
D2PDTauSelector::~D2PDTauSelector()
{
}


//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode D2PDTauSelector::initializeMore()
{
  // Print the used configuration
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "==> initializeMore " << name() << "..." );
      
      // Print out the used configuration
      ATH_MSG_INFO ( " using tauID                    = " << m_isTau );
      ATH_MSG_INFO ( " using tauAcceptIsTauList       = " << m_acceptIsTauList  );
      ATH_MSG_INFO ( " using tauRequireIsTauList      = " << m_requireIsTauList );
      ATH_MSG_INFO ( " using tauVetoIsTauList         = " << m_vetoIsTauList    );

      ATH_MSG_INFO ( " using tauAuthor                = " << m_author );
      ATH_MSG_INFO ( " using tauAcceptAuthorList      = " << m_acceptAuthorList  );
      ATH_MSG_INFO ( " using tauRequireAuthorList     = " << m_requireAuthorList );
      ATH_MSG_INFO ( " using tauVetoAuthorList        = " << m_vetoAuthorList    );
      
      ATH_MSG_INFO ( " using tauDiscriminant          = " << m_tauDiscriminant    );
      ATH_MSG_INFO ( " using tauDiscriminantMin       = " << m_tauDiscriminantMin );
      ATH_MSG_INFO ( " using tauDiscriminantMax       = " << m_tauDiscriminantMax );

      ATH_MSG_INFO ( " using chargeMin                = " << m_chargeMin        );
      ATH_MSG_INFO ( " using chargeMax                = " << m_chargeMax        );
      ATH_MSG_INFO ( " using acceptChargeList         = " << m_acceptChargeList );
      ATH_MSG_INFO ( " using vetoChargeList           = " << m_vetoChargeList   );
      ATH_MSG_INFO ( " using allowChargeConjugate     = " << m_allowChargeConj  );

      ATH_MSG_INFO ( " using minNumberOfTracks        = " << m_nTracksMin        );
      ATH_MSG_INFO ( " using maxNumberOfTracks        = " << m_nTracksMax        );
      ATH_MSG_INFO ( " using acceptNumberOfTracksList = " << m_acceptNTracksList );
      ATH_MSG_INFO ( " using vetoNumberOfTracksList   = " << m_vetoNTracksList   );

      ATH_MSG_INFO ( " using requireTauSafeLooseID    = " << m_requireTauSafeLooseID );
    }

  // get the FilterTool 
  ATH_CHECK( toolSvc()->retrieveTool("FilterTool", m_filterTool) );


  if ( m_acceptAuthorList.size() > 0 ) { m_doAcceptAuthorList = true; }
  if ( m_requireAuthorList.size() > 0 ) { m_doRequireAuthorList = true; }
  if ( m_vetoAuthorList.size() > 0 ) { m_doVetoAuthorList = true; }
  if ( m_acceptIsTauList.size() > 0 ) { m_doAcceptIsTauList = true; }
  if ( m_requireIsTauList.size() > 0 ) { m_doRequireIsTauList = true; }
  if ( m_vetoIsTauList.size() > 0 ) { m_doVetoIsTauList = true; }
  if ( m_acceptChargeList.size() > 0 ) { m_doAcceptChargeList = true; }
  if ( m_vetoChargeList.size() > 0 ) { m_doVetoChargeList = true; }
  if ( m_acceptNTracksList.size() > 0 ) { m_doAcceptNTracksList = true; }
  if ( m_vetoNTracksList.size() > 0 ) { m_doVetoNTracksList = true; }


  // Setup the internal cutflow stuff
  if ( m_printInternalCutflow )
    {
      if ( m_doIsTau )
        {
          m_idxIsTau = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "IsTau=%ld", m_isTau.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doAuthor )
        {
          m_idxAuthor = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "author=%ld", m_author.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doAcceptAuthorList )
        {
          m_idxAcceptAuthorList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doAcceptAuthorList=1", std::make_pair(0,0) ) );
        }
      if ( m_doRequireAuthorList )
        {
          m_idxRequireAuthorList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doRequireAuthorList=1", std::make_pair(0,0) ) );
        }
      if ( m_doVetoAuthorList )
        {
          m_idxVetoAuthorList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doVetoAuthorList=1", std::make_pair(0,0) ) );
        }
      if ( m_doAcceptIsTauList )
        {
          m_idxAcceptIsTauList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doAcceptIsTauList=1", std::make_pair(0,0) ) );
        }
      if ( m_doRequireIsTauList )
        {
          m_idxRequireIsTauList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doRequireIsTauList=1", std::make_pair(0,0) ) );
        }
      if ( m_doVetoIsTauList )
        {
          m_idxVetoIsTauList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doVetoIsTauList=1", std::make_pair(0,0) ) );
        }
      if ( m_doTauDiscriminant )
        {
          m_idxTauDiscriminant = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < tau discriminant < %g",
                                                          m_tauDiscriminantMin.value(), m_tauDiscriminantMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doCharge )
        {
          m_idxCharge = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%d < tau charge < %d",
                                                          m_chargeMin.value(), m_chargeMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doAcceptChargeList )
        {
          m_idxAcceptChargeList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doAcceptChargeList=1", std::make_pair(0,0) ) );
        }
      if ( m_doVetoChargeList )
        {
          m_idxVetoChargeList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doVetoChargeList=1", std::make_pair(0,0) ) );
        }
      if ( m_doNTracks )
        {
          m_idxNTracks = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%d < n tracks < %d",
                                                          m_nTracksMin.value(), m_nTracksMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doAcceptNTracksList )
        {
          m_idxAcceptNTracksList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doAcceptNTracksList=1", std::make_pair(0,0) ) );
        }
      if ( m_doVetoNTracksList )
        {
          m_idxVetoNTracksList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doVetoNTracksList=1", std::make_pair(0,0) ) );
        }
      if ( m_requireTauSafeLooseID )
        {
          m_idxRequireTauSafeLooseID = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "requireTauSafeLooseID=1", std::make_pair(0,0) ) );
        }
    }

  return StatusCode::SUCCESS;
}



//=============================================================================
// Athena execute method
//=============================================================================
StatusCode D2PDTauSelector::processObject( const Analysis::TauJet* tau,
                                           bool &isPassed )
{
  // Print a verbose message
  ATH_MSG_VERBOSE ( "Starting tau processObject." );


  // Make sure we have a tau
  if ( tau )
    {
      ATH_MSG_VERBOSE ( "Having a tau." );

      //--------------------------
      // Do the selections
      //--------------------------
      // Tau author selection
      if ( isPassed && m_doAuthor )
        {
          isPassed = ( tau->hasAuthor( (TauJetParameters::Author)m_author.value() ) == true );
          ATH_MSG_VERBOSE ( "Tau author cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxAuthor] += 1; }
        }

      // Tau author accept list selection
      if ( isPassed && m_doAcceptAuthorList )
        {
          // Deal with the logical OR combination from the m_acceptAuthorList
          bool passAccept(false);
          if ( m_acceptAuthorList.size() == 0 )
            {
              passAccept = true;
            }
          std::vector<long>::iterator acceptItr    = m_acceptAuthorList.begin();
          std::vector<long>::iterator acceptItrEnd = m_acceptAuthorList.end();
          for ( ; acceptItr != acceptItrEnd; ++acceptItr )
            {
              passAccept = passAccept || ( tau->hasAuthor( (TauJetParameters::Author)(*acceptItr) ) == true );
            } // End: loop over acceptAuthorList
          isPassed = passAccept;
          ATH_MSG_VERBOSE ( "Tau author accept list cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxAcceptAuthorList] += 1; }
        }

      // Tau author require list selection
      if ( isPassed && m_doRequireAuthorList )
        {
          // Deal with the logical AND combination from the m_requireAuthorList
          std::vector<long>::iterator requireItr    = m_requireAuthorList.begin();
          std::vector<long>::iterator requireItrEnd = m_requireAuthorList.end();
          for ( ; requireItr != requireItrEnd; ++requireItr )
            {
              isPassed = isPassed && ( tau->hasAuthor( (TauJetParameters::Author)(*requireItr) ) == true );
            } // End: loop over requireAuthorList
          ATH_MSG_VERBOSE ( "Tau author require list cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRequireAuthorList] += 1; }
        }

      // Tau author veto list selection
      if ( isPassed && m_doVetoAuthorList )
        {
          // Deal with the logical NOT combination from the m_vetoAuthorList
          bool passVeto(false);
          std::vector<long>::iterator vetoItr    = m_vetoAuthorList.begin();
          std::vector<long>::iterator vetoItrEnd = m_vetoAuthorList.end();
          for ( ; vetoItr != vetoItrEnd; ++vetoItr )
            {
              passVeto = passVeto || ( tau->hasAuthor( (TauJetParameters::Author)(*vetoItr) ) == true );
            } // End: loop over vetoAuthorList
          isPassed = !passVeto;
          ATH_MSG_VERBOSE ( "Tau author veto list cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxVetoAuthorList] += 1; }
        }


      // TauID selection
      if ( isPassed && m_doIsTau )
        {
          isPassed = ( tau->tauID()->isTau( (TauJetParameters::IsTauFlag)m_isTau.value() ) == 0 );
          ATH_MSG_VERBOSE ( "Tau ID cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxIsTau] += 1; }
        }


      // Tau ID accept list selection
      if ( isPassed && m_doAcceptIsTauList )
        {
          // Deal with the logical OR combination from the m_acceptIsTauList
          bool passAccept(false);
          if ( m_acceptIsTauList.size() == 0 )
            {
              passAccept = true;
            }
          std::vector<long>::iterator acceptItr    = m_acceptIsTauList.begin();
          std::vector<long>::iterator acceptItrEnd = m_acceptIsTauList.end();
          for ( ; acceptItr != acceptItrEnd; ++acceptItr )
            {
              passAccept = passAccept || ( tau->tauID()->isTau( (TauJetParameters::IsTauFlag)(*acceptItr) ) == true );
            } // End: loop over acceptIsTauList
          isPassed = passAccept;
          ATH_MSG_VERBOSE ( "Tau ID accept list cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxAcceptIsTauList] += 1; }
        }

      // Tau ID require list selection
      if ( isPassed && m_doRequireIsTauList )
        {
          // Deal with the logical AND combination from the m_requireIsTauList
          std::vector<long>::iterator requireItr    = m_requireIsTauList.begin();
          std::vector<long>::iterator requireItrEnd = m_requireIsTauList.end();
          for ( ; requireItr != requireItrEnd; ++requireItr )
            {
              isPassed = isPassed && ( tau->tauID()->isTau( (TauJetParameters::IsTauFlag)(*requireItr) ) == true );
            } // End: loop over requireIsTauList
          ATH_MSG_VERBOSE ( "Tau ID require list cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRequireIsTauList] += 1; }
        }

      // Tau ID veto list selection
      if ( isPassed && m_doVetoIsTauList )
        {
          // Deal with the logical NOT combination from the m_vetoIsTauList
          bool passVeto(false);
          std::vector<long>::iterator vetoItr    = m_vetoIsTauList.begin();
          std::vector<long>::iterator vetoItrEnd = m_vetoIsTauList.end();
          for ( ; vetoItr != vetoItrEnd; ++vetoItr )
            {
              passVeto = passVeto || ( tau->tauID()->isTau( (TauJetParameters::IsTauFlag)(*vetoItr) ) == true );
            } // End: loop over vetoIsTauList
          isPassed = !passVeto;
          ATH_MSG_VERBOSE ( "Tau ID veto list cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxVetoIsTauList] += 1; }
        }


      // TauID discriminant selection
      if ( isPassed && m_doTauDiscriminant )
        {
          isPassed = ( tau->tauID()->discriminant( (TauJetParameters::TauID)m_tauDiscriminant.value() ) >= m_tauDiscriminantMin.value()
                       && tau->tauID()->discriminant( (TauJetParameters::TauID)m_tauDiscriminant.value() ) <= m_tauDiscriminantMax.value() );
          ATH_MSG_VERBOSE ( "Tau ID discriminant cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxTauDiscriminant] += 1; }
        }


      // Tau charge selection
      if ( isPassed && m_doCharge )
        {
          isPassed = m_filterTool->chargeFilter( tau,
                                                 m_chargeMin,
                                                 m_chargeMax,
                                                 m_allowChargeConj );
          ATH_MSG_VERBOSE ( "Tau charge cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxCharge] += 1; }
        }

      // Tau charge accept list selection
      if ( isPassed && m_doAcceptChargeList )
        {
          // Deal with the logical OR combination from the m_acceptChargeList
          int tauCharge = (int)tau->charge();
          bool passAccept(false);
          if ( m_acceptChargeList.size() == 0 )
            {
              passAccept = true;
            }
          std::vector<int>::iterator acceptItr    = m_acceptChargeList.begin();
          std::vector<int>::iterator acceptItrEnd = m_acceptChargeList.end();
          for ( ; acceptItr != acceptItrEnd; ++acceptItr )
            {
              if ( m_allowChargeConj )
                {
                  if ( std::abs(tauCharge) == (*acceptItr) )
                    {
                      passAccept = true;
                      break;
                    } 
                }
              else
                {
                  if ( tauCharge == (*acceptItr) )
                    {
                      passAccept = true;
                      break;
                    } 
                }
            } // End: loop over acceptChargeList
          isPassed = passAccept;
          ATH_MSG_VERBOSE ( "Tau charge accept list cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxAcceptChargeList] += 1; }
        }

      // Tau charge veto list selection
      if ( isPassed && m_doVetoChargeList )
        {
          // Deal with the logical NOT combination from the m_vetoChargeList
          int tauCharge = (int)tau->charge();
          bool vetoCharge(false);
          std::vector<int>::iterator vetoItr    = m_vetoChargeList.begin();
          std::vector<int>::iterator vetoItrEnd = m_vetoChargeList.end();
          for ( ; vetoItr != vetoItrEnd; ++vetoItr )
            {
              if ( m_allowChargeConj )
                {
                  if ( std::abs(tauCharge) == (*vetoItr) )
                    {
                      vetoCharge = true;
                      break;
                    } 
                }
              else
                {
                  if ( tauCharge == (*vetoItr) )
                    {
                      vetoCharge = true;
                      break;
                    } 
                }
            } // End: loop over vetoChargeList
          isPassed = !vetoCharge;
          ATH_MSG_VERBOSE ( "Tau charge veto list cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxVetoChargeList] += 1; }
        }


      // Tau number of tracks selection
      if ( isPassed && m_doNTracks )
        {
          isPassed = ( tau->numTrack() >= m_nTracksMin
                       && tau->numTrack() <= m_nTracksMax );
          ATH_MSG_VERBOSE ( "Tau number of tracks cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxNTracks] += 1; }
        }

      // Tau number of track accept list selection
      if ( isPassed && m_doAcceptNTracksList )
        {
          // Deal with the logical OR combination from the m_acceptNTrackList
          unsigned int tauNTrack = (unsigned int)tau->numTrack();
          bool passAccept(false);
          if ( m_acceptNTracksList.size() == 0 )
            {
              passAccept = true;
            }
          std::vector<unsigned int>::iterator acceptItr    = m_acceptNTracksList.begin();
          std::vector<unsigned int>::iterator acceptItrEnd = m_acceptNTracksList.end();
          for ( ; acceptItr != acceptItrEnd; ++acceptItr )
            {
              if ( tauNTrack == (*acceptItr) )
                {
                  passAccept = true;
                  break;
                }
            } // End: loop over acceptNTracksList
          isPassed = passAccept;
          ATH_MSG_VERBOSE ( "Tau number of tracks accept list cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxAcceptNTracksList] += 1; }
        }

      // Tau number of track veto list selection
      if ( isPassed && m_doVetoNTracksList )
        {
          // Deal with the logical NOT combination from the m_vetoNTrackList
          unsigned int tauNTrack = (unsigned int)tau->numTrack();
          bool vetoNTrack(false);
          std::vector<unsigned int>::iterator vetoItr    = m_vetoNTracksList.begin();
          std::vector<unsigned int>::iterator vetoItrEnd = m_vetoNTracksList.end();
          for ( ; vetoItr != vetoItrEnd; ++vetoItr )
            {
              if ( tauNTrack == (*vetoItr) )
                {
                  vetoNTrack = true;
                  break;
                }
            } // End: loop over vetoNTracksList
          isPassed = !vetoNTrack;
          ATH_MSG_VERBOSE ( "Tau number of tracks veto list cut is " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxVetoNTracksList] += 1; }
        }


      // Use the tau safe loose identification
      if ( isPassed && m_requireTauSafeLooseID )
        {
          bool isTauSafeLooseID(false);
          // Get the needed variables
          const Analysis::TauCommonDetails* details = tau->details<const Analysis::TauCommonDetails>();
          if ( !details )
            {
              ATH_MSG_WARNING ( "Couldn't get TauCommonDetails... skipping this cut!" );
            }
          else
            {
              ATH_MSG_VERBOSE ( "Got the TauCommonDetails, will perform selection." );
              unsigned int tauNTrack    = (unsigned int)tau->numTrack();
              double tauEMRadius        = details->seedCalo_EMRadius();
              double tauTrkAvgDist      = details->seedCalo_trkAvgDist();
              double tauEtOverPtLeadTrk = details->etOverPtLeadTrk();
              double tauPtOverEtLeadTrk(0.0);
              if ( tauEtOverPtLeadTrk!= 0.0 )
                {
                  tauPtOverEtLeadTrk = 1/tauEtOverPtLeadTrk;
                }
              // Calculate the tau safe loose ID decision
              if ( tauNTrack <= 1
                   && tauEMRadius < 0.15
                   && tauTrkAvgDist < 0.09
                   && tauPtOverEtLeadTrk > 0.06 )
                {
                  isTauSafeLooseID = true;
                }
              else if ( tauNTrack >= 2
                        && tauEMRadius < 0.15
                        && tauTrkAvgDist < 0.12
                        && tauPtOverEtLeadTrk > 0.12 )
                {
                  isTauSafeLooseID = true;
                }
              isPassed = isTauSafeLooseID;
            }
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRequireTauSafeLooseID] += 1; }
        }
    }
  else
    {
      // Return false 
      isPassed = false;
      ATH_MSG_WARNING ( "Could not dynamic_cast to an tau! Failing this tau!" );
    } // End: if/else have tau

  
  return StatusCode::SUCCESS;
}





//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode D2PDTauSelector::finalizeMore()
{
  return StatusCode::SUCCESS;
}



