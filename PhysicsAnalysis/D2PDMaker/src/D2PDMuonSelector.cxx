/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: D2PDMuonSelector.cxx
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

// This classes header
#include "D2PDMaker/D2PDMuonSelector.h"

// STL includes
#include <string>
#include <vector>
#include <cfloat>

// Gaudi stuff
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

// muon includes
#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"




//=============================================================================
// Constructor
//=============================================================================
D2PDMuonSelector::D2PDMuonSelector( const std::string& name,
                                    ISvcLocator* pSvcLocator ) :
  D2PDSelectorBase< Analysis::Muon, Analysis::MuonContainer >( name, pSvcLocator ),
  m_doAcceptAuthor(false),
  m_idxAcceptAuthor(0),
  m_idxRequireIsCombined(0),
  m_idxVetoIsCombined(0),
  m_idxRequireIsStandAlone(0),
  m_idxVetoIsStandAlone(0),
  m_idxRequireIsCaloMuonId(0),
  m_idxVetoIsCaloMuonId(0),
  m_idxRequireIsSegmentTagged(0),
  m_idxVetoIsSegmentTagged(0),
  m_idxRequireIsLoose(0),
  m_idxRequireIsMedium(0),
  m_idxRequireIsTight(0),
  m_doAcceptAuthorList(false),
  m_idxAcceptAuthorList(0),
  m_doRequireAuthorList(false),
  m_idxRequireAuthorList(0),
  m_doVetoAuthorList(false),
  m_idxVetoAuthorList(0),
  m_doMuonParameterList(false),
  m_idxMuonParameterList(0),
  m_doChi2(false),
  m_idxChi2(0)
{
  declareProperty("muonRequireIsCombined",             m_requireIsCombined=false,           "Require an isCombined muon" );
  declareProperty("muonAcceptIsCombined",              m_acceptIsCombined=false,            "Accept an isCombined muon" );
  declareProperty("muonVetoIsCombined",                m_vetoIsCombined=false,              "Veto an isCombined muon" );

  declareProperty("muonRequireIsStandAlone",           m_requireIsStandAlone=false,         "Require an isStandAlone muon" );
  declareProperty("muonAcceptIsStandAlone",            m_acceptIsStandAlone=false,          "Accept an isStandAlone muon" );
  declareProperty("muonVetoIsStandAlone",              m_vetoIsStandAlone=false,            "Veto an isStandAlone muon" );

  declareProperty("muonRequireIsCaloMuonId",           m_requireIsCaloMuonId=false,         "Require an isCaloMuonId muon" );
  declareProperty("muonAcceptIsCaloMuonId",            m_acceptIsCaloMuonId=false,          "Accept an isCaloMuonId muon" );
  declareProperty("muonVetoIsCaloMuonId",              m_vetoIsCaloMuonId=false,            "Veto an isCaloMuonId muon" );

  declareProperty("muonRequireIsSegmentTagged",        m_requireIsSegmentTagged=false,      "Require a segment tagged muon" );
  declareProperty("muonAcceptIsSegmentTagged",         m_acceptIsSegmentTagged=false,       "Accept a segment tagged muon" );
  declareProperty("muonVetoIsSegmentTagged",           m_vetoIsSegmentTagged=false,         "Veto a segment tagged muon" );

  declareProperty("muonRequireIsLoose",                m_requireIsLoose=false,              "Require an isLoose muon" );
  declareProperty("muonRequireIsMedium",               m_requireIsMedium=false,             "Require an isMedium muon" );
  declareProperty("muonRequireIsTight",                m_requireIsTight=false,              "Require an isTight muon" );

  declareProperty("muonAcceptAuthorList",              m_acceptAuthorList,                  "Muon author list for accept (logical OR combination)" );
  declareProperty("muonRequireAuthorList",             m_requireAuthorList,                 "Muon author list for require (logical AND combination)" );
  declareProperty("muonVetoAuthorList",                m_vetoAuthorList,                    "Muon author list for veto (logical NOT combination)" );

  declareProperty("muonParametersList",                m_muonParameterList,                 "List of MuonParameters::ParamDef to use for selection" );
  declareProperty("muonParametersMinList",             m_muonParameterMinList,              "List of MuonParameters::ParamDef lower cut values" );
  declareProperty("muonParametersMaxList",             m_muonParameterMaxList,              "List of MuonParameters::ParamDef upper cut values" );
  declareProperty("muonParametersUseRelativeCutsList", m_muonParameterUseRelCutsList,
                  "List of bools to decide if one should use relative (devided by muon.pt) cuts for the MuonParameters::ParamDef list" );

  declareProperty("muonMatchChi2OverDoFMin",           m_matchChi2OverDoFMin=-DBL_MAX,      "Minumum required Chi2/nDoF from track match" );
  m_matchChi2OverDoFMin.declareUpdateHandler( &D2PDMuonSelector::setupChi2, this );
  declareProperty("muonMatchChi2OverDoFMax",           m_matchChi2OverDoFMax=DBL_MAX,       "Maxumum required Chi2/nDoF from track match" );
  m_matchChi2OverDoFMax.declareUpdateHandler( &D2PDMuonSelector::setupChi2, this );
}

//=============================================================================
// Destructor
//=============================================================================
D2PDMuonSelector::~D2PDMuonSelector()
{
}


//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode D2PDMuonSelector::initializeMore()
{
  // Print the used configuration
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "==> initializeMore " << name() << "..." );
      
      // Print out the used configuration
      ATH_MSG_INFO ( " using muonRequireIsCombined             = " << m_requireIsCombined );
      ATH_MSG_INFO ( " using muonAcceptIsCombined              = " << m_acceptIsCombined  );
      ATH_MSG_INFO ( " using muonVetoIsCombined                = " << m_vetoIsCombined    );

      ATH_MSG_INFO ( " using muonRequireIsStandAlone           = " << m_requireIsStandAlone );
      ATH_MSG_INFO ( " using muonAcceptIsStandAlone            = " << m_acceptIsStandAlone  );
      ATH_MSG_INFO ( " using muonVetoIsStandAlone              = " << m_vetoIsStandAlone    );

      ATH_MSG_INFO ( " using muonRequireIsCaloMuonId           = " << m_requireIsCaloMuonId );
      ATH_MSG_INFO ( " using muonAcceptIsCaloMuonId            = " << m_acceptIsCaloMuonId  );
      ATH_MSG_INFO ( " using muonVetoIsCaloMuonId              = " << m_vetoIsCaloMuonId    );

      ATH_MSG_INFO ( " using muonRequireIsSegmentTagged        = " << m_requireIsSegmentTagged );
      ATH_MSG_INFO ( " using muonAcceptIsSegmentTagged         = " << m_acceptIsSegmentTagged  );
      ATH_MSG_INFO ( " using muonVetoIsSegmentTagged           = " << m_vetoIsSegmentTagged    );

      ATH_MSG_INFO ( " using muonRequireIsLoose                = " << m_requireIsLoose );
      ATH_MSG_INFO ( " using muonRequireIsMedium               = " << m_requireIsMedium );
      ATH_MSG_INFO ( " using muonRequireIsTight                = " << m_requireIsTight );

      ATH_MSG_INFO ( " using muonAcceptAuthorList              = " << m_acceptAuthorList  );
      ATH_MSG_INFO ( " using muonRequireAuthorList             = " << m_requireAuthorList );
      ATH_MSG_INFO ( " using muonVetoAuthorList                = " << m_vetoAuthorList    );
      
      ATH_MSG_INFO ( " using muonParametersList                = " << m_muonParameterList    );
      ATH_MSG_INFO ( " using muonParametersMinList             = " << m_muonParameterMinList );
      ATH_MSG_INFO ( " using muonParametersMaxList             = " << m_muonParameterMaxList );
      ATH_MSG_INFO ( " using muonParametersUseRelativeCutsList = " << m_muonParameterUseRelCutsList );

      ATH_MSG_INFO ( " using muonMatchChi2OverDoFMin           = " << m_matchChi2OverDoFMin );
      ATH_MSG_INFO ( " using muonMatchChi2OverDoFMax           = " << m_matchChi2OverDoFMax );
    }


  // Do some consistency checks
  if ( m_muonParameterList.size() != m_muonParameterMinList.size()
       || m_muonParameterList.size() != m_muonParameterMaxList.size()
       || m_muonParameterMinList.size() != m_muonParameterMaxList.size()
       ||  m_muonParameterList.size() != m_muonParameterUseRelCutsList.size()
       )
    {
      ATH_MSG_ERROR ( "The length of the MuonParameterList do not agree"
                      << "! Length of muonParametersList=" << m_muonParameterList.size()
                      << ". Length of muonParametersMinList=" << m_muonParameterMinList.size()
                      << ". Length of muonParametersMaxList=" << m_muonParameterMaxList.size()
                      << ". Length of muonParametersUseRelativeCutsList=" << m_muonParameterUseRelCutsList.size() );
      return StatusCode::FAILURE;
    }


  if ( m_acceptIsCombined 
       || m_acceptIsStandAlone
       || m_acceptIsCaloMuonId
       || m_acceptIsSegmentTagged )
    {
      m_doAcceptAuthor = true;
    }
  if ( m_acceptAuthorList.size() > 0 ) { m_doAcceptAuthorList = true; }
  if ( m_requireAuthorList.size() > 0 ) { m_doRequireAuthorList = true; }
  if ( m_vetoAuthorList.size() > 0 ) { m_doVetoAuthorList = true; }
  if ( m_muonParameterList.size() > 0 ) { m_doMuonParameterList = true; }




  // Setup the internal cutflow stuff
  if ( m_printInternalCutflow )
    {
      if ( m_doAcceptAuthor )
        {
          m_idxAcceptAuthor = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "acceptIsCombined=%d, acceptIsStandAlone=%d, acceptIsCaloMuonId=%d, acceptIsSegmentTagged=%d",
                                                          m_acceptIsCombined, m_acceptIsStandAlone, m_acceptIsCaloMuonId, m_acceptIsSegmentTagged ),
                                                    std::make_pair(0,0) ) );
        }
      if ( m_requireIsCombined )
        {
          m_idxRequireIsCombined = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "requireIsCombined=%d", m_requireIsCombined ), std::make_pair(0,0) ) );
        }
      if ( m_vetoIsCombined)
        {
          m_idxVetoIsCombined = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "vetoIsCombined=%d", m_vetoIsCombined ), std::make_pair(0,0) ) );
        }
      if ( m_requireIsStandAlone )
        {
          m_idxRequireIsStandAlone = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "requireIsStandAlone=%d", m_requireIsStandAlone ), std::make_pair(0,0) ) );
        }
      if ( m_vetoIsStandAlone )
        {
          m_idxVetoIsStandAlone = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "vetoIsStandAlone=%d", m_vetoIsStandAlone ), std::make_pair(0,0) ) );
        }
      if ( m_requireIsCaloMuonId )
        {
          m_idxRequireIsCaloMuonId = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "requireIsCaloMuonId=%d", m_requireIsCaloMuonId ), std::make_pair(0,0) ) );
        }
      if ( m_vetoIsCaloMuonId )
        {
          m_idxVetoIsCaloMuonId = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "vetoIsCaloMuonId=%d", m_vetoIsCaloMuonId ), std::make_pair(0,0) ) );
        }
      if ( m_requireIsSegmentTagged )
        {
          m_idxRequireIsSegmentTagged = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "requireIsSegmentTagged=%d", m_requireIsSegmentTagged ), std::make_pair(0,0) ) );
        }
      if ( m_vetoIsSegmentTagged )
        {
          m_idxVetoIsSegmentTagged = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "vetoIsSegmentTagged=%d", m_vetoIsSegmentTagged ), std::make_pair(0,0) ) );
        }
      if ( m_requireIsLoose )
        {
          m_idxRequireIsLoose = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "requireIsLoose=%d", m_requireIsLoose ), std::make_pair(0,0) ) );
        }
      if ( m_requireIsMedium )
        {
          m_idxRequireIsMedium = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "requireIsMedium=%d", m_requireIsMedium ), std::make_pair(0,0) ) );
        }
      if ( m_requireIsTight )
        {
          m_idxRequireIsTight = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "requireIsTight=%d", m_requireIsTight ), std::make_pair(0,0) ) );
        }
      if ( m_doAcceptAuthorList )
        {
          m_idxAcceptAuthorList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "doAcceptAuthorList=%d", m_doAcceptAuthorList ), std::make_pair(0,0) ) );
        }
      if ( m_doRequireAuthorList )
        {
          m_idxRequireAuthorList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "doRequireAuthorList=%d", m_doRequireAuthorList ), std::make_pair(0,0) ) );
        }
      if ( m_doVetoAuthorList )
        {
          m_idxVetoAuthorList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "doVetoAuthorList=%d", m_doVetoAuthorList ), std::make_pair(0,0) ) );
        }
      if ( m_doMuonParameterList )
        {
          m_idxMuonParameterList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "doMuonParameterList=%d", m_doMuonParameterList ), std::make_pair(0,0) ) );
        }
      if ( m_doChi2 )
        {
          m_idxChi2 = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < matchChi2OverDoF < %g",
                                                          m_matchChi2OverDoFMin.value(), m_matchChi2OverDoFMax.value()), std::make_pair(0,0) ) );
        }
    }


  return StatusCode::SUCCESS;
}



//=============================================================================
// Athena execute method
//=============================================================================
StatusCode D2PDMuonSelector::processObject( const Analysis::Muon* muon,
                                            bool &isPassed )
{
  //-----------------------------------------
  // Do the selection
  //-----------------------------------------

  // Make sure we have an muon returned by the dynamic_cast
  if ( muon )
    {
      // Calculte some standard variables
      double muonPt = muon->pt();

      //--------------------------
      // Do the actual selections
      //--------------------------
      // do the muon author accept selection
      if ( isPassed && m_doAcceptAuthor )
        {
          bool acceptAuthor(false);
          if ( !acceptAuthor && m_acceptIsCombined )
            {
              acceptAuthor = muon->isCombinedMuon() ;
            }
          if ( !acceptAuthor && m_acceptIsStandAlone )
            {
              acceptAuthor = muon->isStandAloneMuon() ;
            }
          if ( !acceptAuthor && m_acceptIsCaloMuonId )
            {
              acceptAuthor = muon->isCaloMuonId() ;
            }
          if ( !acceptAuthor && m_acceptIsSegmentTagged)
            {
              acceptAuthor = muon->isSegmentTaggedMuon() ;
            }  

          if ( !acceptAuthor
               && !m_acceptIsCombined
               && !m_acceptIsStandAlone
               && !m_acceptIsCaloMuonId
               && !m_acceptIsSegmentTagged
               ) // Need to pass this muon in case no accept method is requested
            {
              acceptAuthor = true;
            }
          isPassed = acceptAuthor;
          ATH_MSG_VERBOSE ( "Muon passes the accept author cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxAcceptAuthor] += 1; }
        }



      // isCombined selection (accept selection is above)
      if ( isPassed && m_requireIsCombined )
        {
          isPassed = muon->isCombinedMuon() ;
          ATH_MSG_VERBOSE ( "Muon passes the require isCombined cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRequireIsCombined] += 1; }
        }
      if ( isPassed && m_vetoIsCombined )
        {
          isPassed = !( muon->isCombinedMuon() ) ;
          ATH_MSG_VERBOSE ( "Muon passes the veto isCombined cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxVetoIsCombined] += 1; }
        }

      // isStandAlone selection (accept selection is above)
      if ( isPassed && m_requireIsStandAlone )
        {
          isPassed = muon->isStandAloneMuon() ;
          ATH_MSG_VERBOSE ( "Muon passes the require IsStandAlone cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRequireIsStandAlone] += 1; }
        }
      if ( isPassed && m_vetoIsStandAlone )
        {
          isPassed = !( muon->isStandAloneMuon() ) ;
          ATH_MSG_VERBOSE ( "Muon passes the veto IsStandAlone cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxVetoIsStandAlone] += 1; }
        }

      // isCaloMuonId selection (accept selection is above)
      if ( isPassed && m_requireIsCaloMuonId )
        {
          isPassed = muon->isCaloMuonId() ;
          ATH_MSG_VERBOSE ( "Muon passes the require IsCaloMuonId cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRequireIsCaloMuonId] += 1; }
        }
      if ( isPassed && m_vetoIsCaloMuonId )
        {
          isPassed = !( muon->isCaloMuonId() ) ;
          ATH_MSG_VERBOSE ( "Muon passes the veto IsCaloMuonId cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxVetoIsCaloMuonId] += 1; }
        }

      // isSegmentTagged selection (accept selection is above)
      if ( isPassed && m_requireIsSegmentTagged )
        {
          isPassed = muon->isSegmentTaggedMuon() ;
          ATH_MSG_VERBOSE ( "Muon passes the require isSegmentTaggedMuon cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRequireIsSegmentTagged] += 1; }
        }
      if ( isPassed && m_vetoIsSegmentTagged )
        {
          isPassed = !( muon->isSegmentTaggedMuon() ) ;
          ATH_MSG_VERBOSE ( "Muon passes the veto isSegmentTaggedMuon cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxVetoIsSegmentTagged] += 1; }
        }

      // isLoose selection
      if ( isPassed && m_requireIsLoose )
        {
          isPassed = muon->isLoose() ;
          ATH_MSG_VERBOSE ( "Muon passes the requireIsLoose cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRequireIsLoose] += 1; }
        }

      // isMedium selection
      if ( isPassed && m_requireIsMedium )
        {
          isPassed = muon->isMedium() ;
          ATH_MSG_VERBOSE ( "Muon passes the requireIsMedium cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRequireIsMedium] += 1; }
        }

      // isTight selection
      if ( isPassed && m_requireIsTight )
        {
          isPassed = muon->isTight() ;
          ATH_MSG_VERBOSE ( "Muon passes the requireIsTight cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRequireIsTight] += 1; }
        }


      // Muon author accept list selection
      if ( isPassed && m_doAcceptAuthorList )
        {
          // Deal with the logical OR combination from the m_acceptAuthorList
          bool passAccept(false);
          if ( m_acceptAuthorList.size() == 0 )
            {
              passAccept = true;
            }
          std::vector<unsigned int>::iterator acceptItr    = m_acceptAuthorList.begin();
          std::vector<unsigned int>::iterator acceptItrEnd = m_acceptAuthorList.end();
          for ( ; acceptItr != acceptItrEnd; ++acceptItr )
            {
              passAccept = passAccept || ( muon->isAuthor( (MuonParameters::Author)(*acceptItr) ) == true );
            } // End: loop over acceptAuthorList
          isPassed = passAccept;
          ATH_MSG_VERBOSE ( "Muon passes the accept author list cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxAcceptAuthorList] += 1; }
        }

      // Muon author require list selection
      if ( isPassed && m_doRequireAuthorList )
        {
          // Deal with the logical AND combination from the m_requireAuthorList
          std::vector<unsigned int>::iterator requireItr    = m_requireAuthorList.begin();
          std::vector<unsigned int>::iterator requireItrEnd = m_requireAuthorList.end();
          for ( ; requireItr != requireItrEnd; ++requireItr )
            {
              isPassed = isPassed && ( muon->isAuthor( (MuonParameters::Author)(*requireItr) ) == true );
            } // End: loop over requireAuthorList
          ATH_MSG_VERBOSE ( "Muon passes the require author list cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRequireAuthorList] += 1; }
        }

      // Muon author veto list selection
      if ( isPassed && m_doVetoAuthorList )
        {
          // Deal with the logical NOT combination from the m_vetoAuthorList
          bool passVeto(false);
          std::vector<unsigned int>::iterator vetoItr    = m_vetoAuthorList.begin();
          std::vector<unsigned int>::iterator vetoItrEnd = m_vetoAuthorList.end();
          for ( ; vetoItr != vetoItrEnd; ++vetoItr )
            {
              passVeto = passVeto || ( muon->isAuthor( (MuonParameters::Author)(*vetoItr) ) == true );
            } // End: loop over vetoAuthorList
          isPassed = !passVeto;
          ATH_MSG_VERBOSE ( "Muon passes the veto author list cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxVetoAuthorList] += 1; }
        }


      // Get all requested MuonParameters and perform the selection
      if ( isPassed && m_doMuonParameterList )
        {
          const double inv_muonPt = (muonPt != 0 ? 1. / muonPt : 1);
          for ( unsigned int i=0; i < m_muonParameterList.size(); ++i )
            {
              MuonParameters::ParamDef currentPar = (MuonParameters::ParamDef)m_muonParameterList[i];
              double     lowerCut = m_muonParameterMinList[i];
              double     upperCut = m_muonParameterMaxList[i];
              bool useRelativeCut = m_muonParameterUseRelCutsList[i];
              if ( !useRelativeCut )
                {
                  isPassed = isPassed && ( muon->parameter(currentPar) >= lowerCut ) && ( muon->parameter(currentPar) <= upperCut );
                }
              else
                {
                  if ( muonPt != 0.0 )
                    {
                      isPassed = isPassed && ( (muon->parameter(currentPar))*inv_muonPt >= lowerCut ) && ( (muon->parameter(currentPar))*inv_muonPt <= upperCut );
                    }
                  else
                    {
                      ATH_MSG_WARNING ( "Have a muon with zero pt! Ignoring this cut..." );
                    }
                }
            } // End: loop over MuonParametersList
          ATH_MSG_VERBOSE ( "Muon passes the MuonParameters list cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxMuonParameterList] += 1; }
        }


      // matchChi2OverDoF
      if ( isPassed && m_doChi2 )
        {
          if ( muon->matchChi2OverDoF() >= m_matchChi2OverDoFMin
               && muon->matchChi2OverDoF() <= m_matchChi2OverDoFMax )
            {
              isPassed = true;
            }
          else
            {
              isPassed = false;
            }
          ATH_MSG_VERBOSE ( "Muon passes the chi2 cut " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxChi2] += 1; }
        }
    }
  else
    {
      // Return false 
      isPassed = false;
      ATH_MSG_WARNING ( "Could not dynamic_cast to a muon! Failing this muon!" );
    } // End: if/else have muon
  
  return StatusCode::SUCCESS;
}





//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode D2PDMuonSelector::finalizeMore()
{
  return StatusCode::SUCCESS;
}


