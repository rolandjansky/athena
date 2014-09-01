///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SkimDecisionRunPeriodAwareFilterAlg.cxx 
// Implementation file for class SkimDecisionRunPeriodAwareFilterAlg
// Author: Karsten Koeneke karsten.koeneke@cern.ch
/////////////////////////////////////////////////////////////////// 

// SelectionUtils includes
#include "SkimDecisionRunPeriodAwareFilterAlg.h"

// STL includes
#include <string>
#include <vector>

// boost includes
#include <boost/algorithm/string.hpp>

// FrameWork includes
#include "GaudiKernel/Property.h"

// EDM includes
#include "EventBookkeeperMetaData/SkimDecisionCollection.h"
#include "EventBookkeeperMetaData/SkimDecision.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"

// ROOT includes
#include "TRandom3.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
SkimDecisionRunPeriodAwareFilterAlg::SkimDecisionRunPeriodAwareFilterAlg( const std::string& name, 
                                                                          ISvcLocator* pSvcLocator ) : 
  ::AthFilterAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  declareProperty( "SkimDecisionCollection", m_skimDecContName,     "The name (key) of the SkimDecisionCollection to use" );
  declareProperty( "SkimDecisionNameList",   m_skimDecNameList,
                   "The list of SkimDecision names to accept; within one run number block, use ',' as seperator" );
  declareProperty( "DataBeginRunNumberList", m_dataBeginRunNumList, "The list of run numbers where a block begins (for data)" );
  declareProperty( "DataEndRunNumberList",   m_dataEndRunNumList,   "The list of run numbers where a block ends (for data)" );
  declareProperty( "MCBeginRunNumberList",   m_mcBeginRunNumList,   "The list of run numbers where a block begins (for MC)" );
  declareProperty( "MCEndRunNumberList",     m_mcEndRunNumList,     "The list of run numbers where a block ends (for MC)" );
  declareProperty( "MCRelativeLumiList",     m_mcRelLumiList,
                   "The list of relate integrated luminosities (fractional) for each MC run-number block (set to something larger than 1.0 if valid for a whole range)" );
}



// Destructor
///////////////
SkimDecisionRunPeriodAwareFilterAlg::~SkimDecisionRunPeriodAwareFilterAlg()
{}



// Athena Algorithm's Hooks
////////////////////////////

// Called once before the event loop starts by the Athena framework
StatusCode SkimDecisionRunPeriodAwareFilterAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");
  ATH_MSG_DEBUG (" using SkimDecisionCollection = " << m_skimDecContName );
  ATH_MSG_DEBUG (" using SkimDecisionNameList   = " << m_skimDecNameList );
  ATH_MSG_DEBUG (" using DataBeginRunNumberList = " << m_dataBeginRunNumList );
  ATH_MSG_DEBUG (" using DataEndRunNumberList   = " << m_dataEndRunNumList );
  ATH_MSG_DEBUG (" using MCBeginRunNumberList   = " << m_mcBeginRunNumList );
  ATH_MSG_DEBUG (" using MCEndRunNumberList     = " << m_mcEndRunNumList );
  ATH_MSG_DEBUG (" using MCRelativeLumiList     = " << m_mcRelLumiList );

  // Try to find the commas in the SkimDecisionNameList and make a list of lists
  // and initialize the index vectors to -1
  for ( unsigned int i=0; i<m_skimDecNameList.size(); ++i )
    {
      std::vector<std::string> tokens;
      boost::split(tokens, m_skimDecNameList[i], boost::is_any_of(","));
      m_skimDecNameListList.push_back(tokens);
      std::vector<int> indices;
      for ( unsigned int j=0; j<tokens.size(); ++j )
        {
          indices.push_back(-1);
        }
      m_skimDecIdxListList.push_back(indices);
    }

  // Make some sanity checks
  bool allOK(true);
  if ( m_skimDecNameListList.size() != m_dataBeginRunNumList.size()
       || m_skimDecNameListList.size() != m_dataEndRunNumList.size() )
    {
      ATH_MSG_FATAL ("SkimDecisionNameList has different size from DataBeginRunNumberList or DataEndRunNumberList" );
      allOK = false;
    }
  if ( m_mcBeginRunNumList.size() != 0 || m_mcEndRunNumList.size() != 0 )
    {
      if ( m_skimDecNameListList.size() != m_mcBeginRunNumList.size()
           || m_skimDecNameListList.size() != m_mcEndRunNumList.size()
           || m_skimDecNameListList.size() != m_mcRelLumiList.size() )
        {
          ATH_MSG_FATAL ("SkimDecisionNameList has different size from MCBeginRunNumberList or MCEndRunNumberList or MCRelativeLumiList" );
          allOK = false;
        }
    }

  if ( !allOK )
    {
      return StatusCode::FAILURE; 
    }

  return StatusCode::SUCCESS;
}



// Called once after the event loop is finished by the Athena framework
StatusCode SkimDecisionRunPeriodAwareFilterAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}



// Called once per event by the Athena framework
StatusCode SkimDecisionRunPeriodAwareFilterAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // Get the EventInfo object
  const EventInfo* eventInfo(NULL);
  ATH_CHECK( evtStore()->retrieve(eventInfo) );

  // Get the SkimDecisionCollection
  const SkimDecisionCollection* skimDecCont(NULL);
  ATH_CHECK( evtStore()->retrieve( skimDecCont, m_skimDecContName ) );


  // See if we have MC or data and get the run number
  bool isSim = eventInfo->event_type()->test(EventType::IS_SIMULATION);
  unsigned int runnumber = eventInfo->event_ID()->run_number();
  unsigned int eventnumber = eventInfo->event_ID()->event_number();


  // Event pass decision
  bool passAccept(false);

  // For data ===================================================
  if ( !isSim )
    {
      ATH_MSG_VERBOSE( "Detected that I am running on data. Current run number=" << runnumber );
      unsigned int outerSize( m_dataBeginRunNumList.size() );
      if ( outerSize == 0 ) passAccept = true;
      for ( unsigned int i=0; i<outerSize; ++i )
        {
          // Get the right run number range
          if ( runnumber >= m_dataBeginRunNumList[i] && runnumber <= m_dataEndRunNumList[i] )
            {
              ATH_MSG_VERBOSE( "Found right run number range with index i=" << i );
              // Now, loop over the inner list for the current run-number range
              unsigned int innerSize( m_skimDecNameListList[i].size() );
              for ( unsigned int j=0; j<innerSize; ++j )
                {
                  if ( this->skimDecisionNameIsAccepted( skimDecCont, m_skimDecNameListList[i][j], m_skimDecIdxListList, i, j ) )
                    {
                      ATH_MSG_DEBUG( "Found SkimDecision in data with name " << m_skimDecNameListList[i][j]
                                     << " from SkimDecisionNameList in SkimDecisionCollection with key " << m_skimDecContName );
                      setFilterPassed( true );
                      return StatusCode::SUCCESS;
                    }
                } // End: inner loop over all accept requests
            } // End: check that we are in the right run number range
        } // End: loop over run number ranges
    }
  // For MC   ===================================================
  else
    {
      ATH_MSG_VERBOSE( "Detected that I am running on simulation. Current run number=" << runnumber );
      // First, get the MC channel number and calculate the seed for the random number generator
      unsigned int mcchannelnumber = eventInfo->event_type()->mc_channel_number();
      unsigned int seed = mcchannelnumber * eventnumber;

      // Get the random number generator
      TRandom3 random3;

      unsigned int outerSize( m_mcBeginRunNumList.size() );
      if ( outerSize == 0 ) passAccept = true;
      for ( unsigned int i=0; i<outerSize; ++i )
        {
          // Get the right run number range
          if ( runnumber >= m_mcBeginRunNumList[i] && runnumber <= m_mcEndRunNumList[i] )
            {
              ATH_MSG_VERBOSE( "Found right run number range with index i=" << i );
              // Get the current fraction of lumi for this range
              double lumiFrac = m_mcRelLumiList[i];
              bool needIncrement(false);
              if ( lumiFrac < 1.0 )
                {
                  random3.SetSeed(seed);
                  double rdmNum = random3.Uniform();
                  if ( rdmNum >= lumiFrac ) // go to the next outer index
                    {
                      i += 1;
                    }
                  else
                    {
                      needIncrement = true;
                    }
                }

              // Check that we didn't leave the boundary of the vector
              if ( i >= outerSize )
                {
                  ATH_MSG_FATAL ("We ran over the lenghts of the vector of the SkimDecisionNameList!" );
                  setFilterPassed( false );
                  return StatusCode::FAILURE;
                }

              // Now, loop over the inner list for the current run-number range
              unsigned int innerSize( m_skimDecNameListList[i].size() );
              for ( unsigned int j=0; j<innerSize; ++j )
                {
                  if ( this->skimDecisionNameIsAccepted( skimDecCont, m_skimDecNameListList[i][j], m_skimDecIdxListList, i, j ) )
                    {
                      ATH_MSG_DEBUG( "Found SkimDecision in MC with name " << m_skimDecNameListList[i][j]
                                     << " from SkimDecisionNameList in SkimDecisionCollection with key " << m_skimDecContName );
                      setFilterPassed( true );
                      return StatusCode::SUCCESS;
                    }
                } // End: inner loop over all accept requests
              
              // Increment to jump over the next entry in this run period
              if ( needIncrement )
                {
                  i += 1;
                }

            } // End: check that we are in the right run number range
        } // End: loop over run number ranges

    } // End: if/else !isMC


  // Determine the global event passing decision
  setFilterPassed( passAccept );

  return StatusCode::SUCCESS;
}









// Private helper method to find the SkimDecision isAccepted() answer
bool SkimDecisionRunPeriodAwareFilterAlg::skimDecisionNameIsAccepted( const SkimDecisionCollection* skimDecCont,
                                                                      std::string& skimDecName,
                                                                      std::vector< std::vector<int> >& skimDecIdxList,
                                                                      unsigned int idxA,
                                                                      unsigned int idxB )
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
  if ( skimDecIdxList[idxA][idxB] >= 0 && skimDecIdxList[idxA][idxB] < (int)skimCollSize )
    {
      const SkimDecision* skimDec = skimDecCont->at(skimDecIdxList[idxA][idxB]);
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
                  skimDecIdxList[idxA][idxB] = (int)i; // Set the index to the found one
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
              skimDecIdxList[idxA][idxB] = (int)i; // Set the index to the found one
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
