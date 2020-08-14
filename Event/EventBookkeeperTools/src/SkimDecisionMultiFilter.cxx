///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SkimDecisionMultiFilter.cxx 
// Implementation file for class SkimDecisionMultiFilter
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
// Description: This filter allows to make an event decision based
//              on a list of SkimDecisions to ask.
/////////////////////////////////////////////////////////////////// 

// EventBookkeeperTools includes
#include "SkimDecisionMultiFilter.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"

// Handy definitions for error checking
#include "AthenaKernel/errorcheck.h"

// Event includes
#include "EventBookkeeperMetaData/SkimDecisionCollection.h"
#include "EventBookkeeperMetaData/SkimDecision.h"



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
SkimDecisionMultiFilter::SkimDecisionMultiFilter( const std::string& name, 
                                                  ISvcLocator* pSvcLocator ) : 
  ::AthFilterAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  declareProperty( "SkimDecisionCollection",      m_skimDecContName, "The name (key) of the SkimDecisionCollection to use" );
  declareProperty( "AcceptSkimDecisionNameList",  m_accSkimDecNameList, "The list of SkimDecision names to accept (logical OR)" );
  declareProperty( "RequireSkimDecisionNameList", m_reqSkimDecNameList, "The list of SkimDecision names to require (logical AND)" );
  declareProperty( "VetoSkimDecisionNameList",    m_vetoSkimDecNameList, "The list of SkimDecision names to veto (logical NOT)" );

}

// Destructor
///////////////
SkimDecisionMultiFilter::~SkimDecisionMultiFilter()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode SkimDecisionMultiFilter::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_MSG_INFO (" using SkimDecisionCollection      = " << m_skimDecContName );
  ATH_MSG_INFO (" using AcceptSkimDecisionNameList  = " << m_accSkimDecNameList );
  ATH_MSG_INFO (" using RequireSkimDecisionNameList = " << m_reqSkimDecNameList );
  ATH_MSG_INFO (" using VetoSkimDecisionNameList    = " << m_vetoSkimDecNameList );

  // Initialize the index vectors to -1
  for ( unsigned int i=0; i<m_accSkimDecNameList.size(); ++i )  m_accIdxList.push_back(-1);
  for ( unsigned int i=0; i<m_reqSkimDecNameList.size(); ++i )  m_reqIdxList.push_back(-1);
  for ( unsigned int i=0; i<m_vetoSkimDecNameList.size(); ++i ) m_vetoIdxList.push_back(-1);

  return StatusCode::SUCCESS;
}

StatusCode SkimDecisionMultiFilter::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode SkimDecisionMultiFilter::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // Get the SkimDecisionCollection
  const SkimDecisionCollection* skimDecCont(NULL);
  CHECK( evtStore()->retrieve( skimDecCont, m_skimDecContName ) );


  // Run the accept determination
  bool passAccept(false);
  if ( m_accSkimDecNameList.size() == 0 ) passAccept = true;
  for ( unsigned int i=0; i<m_accSkimDecNameList.size(); ++i )
    {
      if ( skimDecisionNameIsAccepted( skimDecCont, m_accSkimDecNameList[i], m_accIdxList, i ) )
        {
          ATH_MSG_DEBUG( "Found SkimDecision with name " << m_accSkimDecNameList[i]
                         << " from AcceptSkimDecisionNameList in SkimDecisionCollection with key " << m_skimDecContName );
          passAccept = true;
          break;
        }
    } // End: loop over all accept requests


  // Run the require determination
  bool passRequire(true);
  for ( unsigned int i=0; i<m_reqSkimDecNameList.size(); ++i )
    {
      passRequire = passRequire && skimDecisionNameIsAccepted( skimDecCont, m_reqSkimDecNameList[i], m_reqIdxList, i );
      ATH_MSG_DEBUG( "Found SkimDecision with name " << m_reqSkimDecNameList[i]
                     << " from RequireSkimDecisionNameList in SkimDecisionCollection with key " << m_skimDecContName );
    } // End: loop over all require requests


  // Run the veto determination
  bool veto(false);
  for ( unsigned int i=0; i<m_vetoSkimDecNameList.size(); ++i )
    {
      if ( skimDecisionNameIsAccepted( skimDecCont, m_vetoSkimDecNameList[i], m_vetoIdxList, i ) )
        {
          ATH_MSG_DEBUG( "Found SkimDecision with name " << m_vetoSkimDecNameList[i]
                         << " from VetoSkimDecisionNameList in SkimDecisionCollection with key " << m_skimDecContName );
          veto = true;
          break;
        }
    } // End: loop over all veto requests
  bool passVeto = !veto;


  // Determine the global event passing decision
  setFilterPassed( passAccept && passRequire && passVeto );


  return StatusCode::SUCCESS;
}




// Private helper method to find the SkimDecision isAccepted() answer
bool SkimDecisionMultiFilter::skimDecisionNameIsAccepted( const SkimDecisionCollection* skimDecCont,
                                                          std::string& skimDecName,
                                                          std::vector<int>& skimDecIdxList,
                                                          unsigned int idx )
{
  // Pointer check
  if ( !skimDecCont )
    {
      ATH_MSG_WARNING( "Got a zero pointer of type SkimDecisionCollection with key " << m_skimDecContName );
      return false;
    }

  // Get the size of the SkimDecisionCollection
  const unsigned int skimCollSize = skimDecCont->size();

  // If we already have an index, choose the fast path 
  if ( skimDecIdxList[idx] >= 0 && skimDecIdxList[idx] < (int)skimCollSize )
    {
      const SkimDecision* skimDec = skimDecCont->at(skimDecIdxList[idx]);
      if ( !skimDec )
        {
          ATH_MSG_WARNING( "Couldn't find SkimDecision with name " << skimDecName
                           << " in SkimDecisionCollection with key " << m_skimDecContName );
          return false;
        }

      // If the name is correct for the given index
      if ( skimDec->getName() == skimDecName )
        {
          return skimDec->isAccepted();
        }
      else // The index was wrong and we need to search for the name 
        {
          // Loop over all SkimDecisions in the SkimDecisionCollection
          bool foundSkimDec(false);
          for ( unsigned int i=0; i<skimCollSize; ++i )
            {
              // Get the current SkimDecision
              const SkimDecision* skimDec = skimDecCont->at(i);
              if ( !skimDec ) continue;

              // Found the right skim decision
              if ( skimDecName == skimDec->getName() )
                {
                  skimDecIdxList[idx] = (int)i; // Set the index to the found one
                  foundSkimDec = true;
                  return skimDec->isAccepted();
                }
            }
          // If none of the SkimDecision names matched
          if ( !foundSkimDec )
            {
              ATH_MSG_WARNING( "Couldn't find SkimDecision with name " << skimDecName
                               << " in SkimDecisionCollection with key " << m_skimDecContName );
              return false;
            }
        }
    } // End: if we have a valid index
  else // The index was wrong and we need to search for the name 
    {
      // Loop over all SkimDecisions in the SkimDecisionCollection
      bool foundSkimDec(false);
      for ( unsigned int i=0; i<skimCollSize; ++i )
        {
          // Get the current SkimDecision
          const SkimDecision* skimDec = skimDecCont->at(i);
          if ( !skimDec ) continue;
          
          // Found the right skim decision
          if ( skimDecName == skimDec->getName() )
            {
              skimDecIdxList[idx] = (int)i; // Set the index to the found one
              foundSkimDec = true;
              return skimDec->isAccepted();
            }
        }
      // If none of the SkimDecision names matched
      if ( !foundSkimDec )
        {
          ATH_MSG_WARNING( "Couldn't find SkimDecision with name " << skimDecName
                           << " in SkimDecisionCollection with key " << m_skimDecContName );
          return false;
        }
    }
  // If we got to here, something went wrong
  ATH_MSG_WARNING( "We should have never reached this part of the code! Tried to search for SkimDecision with name " << skimDecName
                   << " in SkimDecisionCollection with key " << m_skimDecContName );
  return false;
}
