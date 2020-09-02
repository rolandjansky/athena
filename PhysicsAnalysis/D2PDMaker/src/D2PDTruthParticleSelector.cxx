/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: D2PDTruthParticleSelector.cxx
//
/**
   @class D2PDTruthParticleSelector

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Select TruthParticles and write out good ones as ElementLinks

   Class to select good TruthParticles from an input container (which should be
   a TruthParticleContainer) and write out the good ones as a new container of
   ElementLinks pointing to the good TruthParticles in the original input container.
*/
//=============================================================================

// Preprocessor magic for debugging
//#define XXX std::cout << "I am here: " << __FILE__ << ":" << __LINE__ << std::endl;

// This classes header
#include "D2PDMaker/D2PDTruthParticleSelector.h"

// MC Truth includes
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/IsGenerator.h"
#include "TruthHelper/IsStatus.h"


//=============================================================================
// Constructor
//=============================================================================
D2PDTruthParticleSelector::D2PDTruthParticleSelector( const std::string& name,
                                                      ISvcLocator* pSvcLocator ) :
  D2PDSelectorBase< TruthParticle, TruthParticleContainer >( name, pSvcLocator ),
  m_doPDGIDList(false),
  m_idxPDGIDList(0),
  m_doOriginPDGIDList(false),
  m_idxOriginPDGIDList(0),
  m_doDaughterPDGIDList(false),
  m_idxDaughterPDGIDList(0),
  m_idxRemoveDocLine(0),
  m_idxKeepOnlyGenerator(0),
  m_idxKeepOnlyStable(0),
  m_idxRemoveSelfDecay(0),
  m_doAcceptStatusCodeList(false),
  m_idxAcceptStatusCodeList(0),
  m_doRejectStatusCodeList(false),
  m_idxRejectStatusCodeList(0)
{
  declareProperty("pdgIDList",                 m_pdgIDList, "The list of PDG_IDs that this algorithm should select" );
  declareProperty("allowAntiParticle",         m_allowAntiPart=true, "Allow to select the anti particle as well" );

  declareProperty("removeDocumentationLine",   m_removeDocLine=false, "Remove status code == 3 objects, i.e., documentation lines" );
  declareProperty("keepOnlyGenerator",         m_keepOnlyGenerator=false, "Keep only generator particles" );
  declareProperty("keepOnlyStable",            m_keepOnlyStable=false, "Keep only generator stable particles" );
  declareProperty("removeSelfDecay",           m_removeSelfDecay=false, "Remove unphysical decays of a paricle into itself" );

  declareProperty("acceptStatusCodeList",      m_acceptStatusCodeList, "Accept only particles with these status codes" );
  declareProperty("rejectStatusCodeList",      m_rejectStatusCodeList, "Reject particles with these status codes" );

  declareProperty("pdgIDListDaughter",         m_daughterPdgIDList, "The list of PDG_IDs of the daughters" );
  declareProperty("allowDaughterAntiParticle", m_allowDaughterAntiPart=true, "Allow anti particle for the daughters" );

  declareProperty("pdgIDListOrigin",           m_originPdgIDList, "The list of PDG_IDs of the (grand-(grand)) mothers" );
  declareProperty("allowOriginAntiParticle",   m_allowOriginAntiPart=true, "Allow anti particle for the (grand-(grand)) mothers" );
  declareProperty("maxNumberOfGenerations",    m_maxNumGenerations=INT_MAX, "The maximum number of generations of the origin" );
}


//=============================================================================
// Destructor
//=============================================================================
D2PDTruthParticleSelector::~D2PDTruthParticleSelector()
{
}


//=============================================================================
// Called during the Athena initialize method
//=============================================================================
StatusCode D2PDTruthParticleSelector::initializeMore()
{
  // Print the used configuration
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "==> initializeMore " << name() << "..." );
      
      // Print out the used configuration
      ATH_MSG_INFO ( " using pdgIDList                 = " << m_pdgIDList );
      ATH_MSG_INFO ( " using allowAntiParticle         = " << m_allowAntiPart );

      ATH_MSG_INFO ( " using removeDocumentationLine   = " << m_removeDocLine );
      ATH_MSG_INFO ( " using keepOnlyGenerator         = " << m_keepOnlyGenerator );
      ATH_MSG_INFO ( " using keepOnlyStable            = " << m_keepOnlyStable );
      ATH_MSG_INFO ( " using removeSelfDecay           = " << m_removeSelfDecay );

      ATH_MSG_INFO ( " using acceptStatusCodeList      = " << m_acceptStatusCodeList );
      ATH_MSG_INFO ( " using rejectStatusCodeList      = " << m_rejectStatusCodeList );

      ATH_MSG_INFO ( " using pdgIDListDaughter         = " << m_daughterPdgIDList );
      ATH_MSG_INFO ( " using allowDaughterAntiParticle = " << m_allowDaughterAntiPart );

      ATH_MSG_INFO ( " using pdgIDListOrigin           = " << m_originPdgIDList );
      ATH_MSG_INFO ( " using allowOriginAntiParticle   = " << m_allowOriginAntiPart );
      ATH_MSG_INFO ( " using maxNumberOfGenerations    = " << m_maxNumGenerations );
    }


  // Determine which cuts to use
  if ( m_pdgIDList.size() > 0 ) { m_doPDGIDList = true; }
  if ( m_daughterPdgIDList.size() > 0 ) { m_doDaughterPDGIDList = true; }
  if ( m_originPdgIDList.size() > 0 ) { m_doOriginPDGIDList = true; }
  if ( m_acceptStatusCodeList.size() > 0 ) { m_doAcceptStatusCodeList = true; }
  if ( m_rejectStatusCodeList.size() > 0 ) { m_doRejectStatusCodeList = true; }


  // Setup the internal cutflow stuff
  if ( m_printInternalCutflow )
    {
      if ( m_doPDGIDList )
        {
          m_idxPDGIDList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doPDGIDList=1", std::make_pair(0,0) ) );
        }
      if ( m_removeDocLine )
        {
          m_idxRemoveDocLine = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "removeDocLine=1", std::make_pair(0,0) ) );
        }
      if ( m_keepOnlyGenerator )
        {
          m_idxKeepOnlyGenerator = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "keepOnlyGenerator=1", std::make_pair(0,0) ) );
        }
      if ( m_keepOnlyStable )
        {
          m_idxKeepOnlyStable = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "keepOnlyStable=1", std::make_pair(0,0) ) );
        }
      if ( m_removeSelfDecay )
        {
          m_idxRemoveSelfDecay = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "removeSelfDecay=1", std::make_pair(0,0) ) );
        }
      if ( m_doAcceptStatusCodeList )
        {
          m_idxAcceptStatusCodeList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doAcceptStatusCodeList=1", std::make_pair(0,0) ) );
        }
      if ( m_doRejectStatusCodeList )
        {
          m_idxRejectStatusCodeList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doRejectStatusCodeList=1", std::make_pair(0,0) ) );
        }
      if ( m_doDaughterPDGIDList )
        {
          m_idxDaughterPDGIDList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doDaughterPDGIDList=1", std::make_pair(0,0) ) );
        }
      if ( m_doOriginPDGIDList )
        {
          m_idxOriginPDGIDList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doOriginPDGIDList=1", std::make_pair(0,0) ) );
        }
    } // End: if ( m_printInternalCutflow )


  return StatusCode::SUCCESS;
}



//=============================================================================
// Called during the Athena execute method, once for every TruthParticle
//=============================================================================
StatusCode D2PDTruthParticleSelector::processObject( const TruthParticle* part,
                                                     bool &isPassed )
{
  ATH_MSG_VERBOSE ( "Starting processObject" );
  // Make sure we have a part
  if ( part )
    {
      ATH_MSG_VERBOSE ( "We got a valid pointer" );
      // Disclaimer: Some of the code below is inspired by the EventView code
      //             from the EVTruthParticleInserter.

      // Get the GenParticle from the TruthParticle
      // This inelegant piece is needed to avoid const correctness problems
      HepMC::ConstGenParticlePtr genPart = (HepMC::ConstGenParticlePtr)(part->genParticle());
      const int pdgID  = genPart->pdg_id();
      const int status = genPart->status();

      // Now, determine if the particle at hand has the user required PDG_ID
      if ( isPassed && m_doPDGIDList )
        {
          ATH_MSG_VERBOSE ( "Trying to select TruthParticle PDG_ID" );
          bool passPDGID = false;
          std::vector<int>::const_iterator pdgItr    = m_pdgIDList.begin();
          std::vector<int>::const_iterator pdgItrEnd = m_pdgIDList.end();
          if ( m_pdgIDList.size() == 0 )
            {
              passPDGID = true;
            }
          else
            {
              if ( m_allowAntiPart )
                {
                  for ( ; pdgItr != pdgItrEnd; ++pdgItr )
                    {
                      passPDGID = ( abs(pdgID) == abs(*pdgItr) );
                      if ( passPDGID )
                        {
                          break;
                        }
                    }
                }
              else
                {
                  for ( ; pdgItr != pdgItrEnd; ++pdgItr )
                    {
                      passPDGID = ( pdgID == *pdgItr );
                      if ( passPDGID )
                        {
                          break;
                        }
                    }
                } // End: if/else ( m_allowAntiPart )
            } // End: if ( m_pdgIDList.size() == 0 )
          // Translate the result to the global is passed boolean
          isPassed = isPassed && passPDGID;
          ATH_MSG_VERBOSE ( "TruthParticle PDG_ID is: " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxPDGIDList] += 1; }
        }


      // Check that we don't have status code == 3 which is just a documentation 
      // particle and NOT physical
      if ( isPassed && m_removeDocLine )
        {
          ATH_MSG_VERBOSE ( "Trying to select TruthParticle removing DocLine" );
          TruthHelper::IsStatus isStatus(3);
          isPassed = isPassed && !(isStatus(genPart));
          ATH_MSG_VERBOSE ( "TruthParticle removed DocLine is: " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRemoveDocLine] += 1; }
        }


      // Now, use some HepMC helpers to figure out if the particle was 
      // created by the generator (and not by the simulation)
      if ( isPassed && m_keepOnlyGenerator )
        {
          ATH_MSG_VERBOSE ( "Trying to select TruthParticle from generator" );
          TruthHelper::IsGenerator isGen;
          isPassed = isPassed && ( status == 10902 || isGen(genPart) );
          // The status 10902 was introduced at the AOD level here:
          // http://alxr.usatlas.bnl.gov/lxr/source/atlas/PhysicsAnalysis/TruthParticleID/McParticleTools/src/EtaPtFilterTool.cxx#353
          ATH_MSG_VERBOSE ( "TruthParticle from generator is: " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxKeepOnlyGenerator] += 1; }
        }


      // Now, use some HepMC helpers to figure out if the particle was 
      // stable at generator level
      if ( isPassed && m_keepOnlyStable )
        {
          ATH_MSG_VERBOSE ( "Trying to select TruthParticle generator stable" );
          TruthHelper::IsGenStable isGenStable;
          isPassed = isPassed && ( !m_keepOnlyStable || isGenStable(genPart) );
          ATH_MSG_VERBOSE ( "TruthParticle is generator stable is: " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxKeepOnlyStable] += 1; }
        }


      // Remove unphysical particles that decay into itself. This can happen 
      // because the generator may adjust the 4-momenta of the generated particles
      // at the parton shower stage
      if ( isPassed && m_removeSelfDecay )
        {
          ATH_MSG_VERBOSE ( "Trying to select TruthParticle removing SelfDecay" );
          if( genPart->end_vertex() )
            {
              int barcode = HepMC::barcode(genPart);
              auto childItrBegin =    genPart->end_vertex()->particles_begin(HepMC::children);
              auto childItrEnd = genPart->end_vertex()->particles_end(HepMC::children);
              for (auto childItr=childItrBegin; childItr!=childItrEnd; ++childItr )
                {
                  auto child = (*childItr);
                  if( child->pdg_id() == pdgID
                      && child->barcode() != barcode
                      && child->barcode() < 200000 )
                    {
                      isPassed = false;
                    }
                } // End: loop over child particles
            } // End: if the end_vertex exists
          ATH_MSG_VERBOSE ( "TruthParticle removed SelfDecay is: " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRemoveSelfDecay] += 1; }
        } // End: if m_removeSelfDecay


      // Now, determine if the particle at hand has the user required accept status code
      if ( isPassed && m_doAcceptStatusCodeList )
        {
          ATH_MSG_VERBOSE ( "Trying to select TruthParticle accepting StatusCode" );
          bool passAcceptStatusCode = false;
          std::vector<int>::const_iterator acceptStatusCodeItr    = m_acceptStatusCodeList.begin();
          std::vector<int>::const_iterator acceptStatusCodeItrEnd = m_acceptStatusCodeList.end();
          if ( m_acceptStatusCodeList.size() == 0 )
            {
              passAcceptStatusCode = true;
            }
          else
            {
              for ( ; acceptStatusCodeItr != acceptStatusCodeItrEnd; ++acceptStatusCodeItr )
                {
                  if ( status == *acceptStatusCodeItr )
                    {
                      passAcceptStatusCode = true;
                      break;
                    }
                }
            } // End: if ( m_acceptStatusCodeList.size() == 0 )
          // Translate the result to the global is passed boolean
          isPassed = isPassed && passAcceptStatusCode;
          ATH_MSG_VERBOSE ( "TruthParticle accept StatusCode is: " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxAcceptStatusCodeList] += 1; }
        }


      // Now, determine if the particle at hand has the user required reject status code
      if ( isPassed && m_doRejectStatusCodeList )
        {
          ATH_MSG_VERBOSE ( "Trying to select TruthParticle rejecting StatusCode" );
          bool rejectPartRejectStatusCode = false;
          std::vector<int>::const_iterator rejectStatusCodeItr    = m_rejectStatusCodeList.begin();
          std::vector<int>::const_iterator rejectStatusCodeItrEnd = m_rejectStatusCodeList.end();
          for ( ; rejectStatusCodeItr != rejectStatusCodeItrEnd; ++rejectStatusCodeItr )
            {
              if ( status == *rejectStatusCodeItr )
                {
                  rejectPartRejectStatusCode = true;
                  break;
                }
            }
          // Translate the result to the global is passed boolean
          isPassed = isPassed && !rejectPartRejectStatusCode;
          ATH_MSG_VERBOSE ( "TruthParticle reject StatusCode is: " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRejectStatusCodeList] += 1; }
        }


      // Now, check that all the daughters of this particle are allowed in the daughter PDG_ID list
      if ( isPassed && m_doDaughterPDGIDList )
        {
          ATH_MSG_VERBOSE ( "Trying to select TruthParticle daughter PDG_ID" );
          bool allDaughtersMatch = true;
          std::vector<HepMC::ConstGenParticlePtr > daughters;
          bool foundDaughters = getDaughters( genPart, daughters );

          if ( foundDaughters )
            {
              // Iterate over the list of found daughters
              for (auto daughter: daughters)
                {
                  bool foundThisID = false;
                  // Iterate over the list of allowed PDG_IDs
                  std::vector<int>::const_iterator daughterPDGItr    = m_daughterPdgIDList.begin();
                  std::vector<int>::const_iterator daughterPDGItrEnd = m_daughterPdgIDList.end();
                  for ( ; daughterPDGItr != daughterPDGItrEnd; ++daughterPDGItr )
                    {
                      if ( m_allowDaughterAntiPart )
                        {
                          if ( std::abs( daughter->pdg_id() ) == abs(*daughterPDGItr) )
                            {
                              foundThisID = true;
                            }
                        }
                      else
                        {
                          if ( daughter->pdg_id() == (*daughterPDGItr) )
                            {
                              foundThisID = true;
                            }
                        }
                      // Terminate this loop
                      if ( foundThisID )
                        {
                          break;
                        }
                    } // End: loop over the list of allowed daughter PDG_IDs

                  allDaughtersMatch = allDaughtersMatch && foundThisID;

                } // End: loop over found daughters
            }
          else
            {
              allDaughtersMatch = false;
            } // End: if/else ( foundDaughters )
          isPassed = isPassed && allDaughtersMatch;
          ATH_MSG_VERBOSE ( "TruthParticle daughter match PDG_ID is: " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxDaughterPDGIDList] += 1; }
        } // End: if ( m_daughterPdgIDList.size() != 0 )


      // Now, look at the origin of the particle and if it fulfills the selection
      if ( isPassed && m_doOriginPDGIDList )
        {
          ATH_MSG_VERBOSE ( "Trying to select TruthParticle origin PDG_ID" );
          bool originFound = false;
          //const TruthParticle* iPart = const_cast<TruthParticle*>(part);
          const TruthParticle* iPart = part;

          // Go up the decay tree for the allowed number of generations
          for ( int iGen=0; iGen < m_maxNumGenerations; ++iGen )
            {
              ATH_MSG_VERBOSE ( "Now in mother generation: " << iGen );
              const TruthParticle* moth;
              int motherPDGID(0);
              motherPDGID = getMotherPDGID( iPart, moth );
          
              if ( motherPDGID != 0 )
                {
                  ATH_MSG_VERBOSE ( "Found a mother with PDG_ID=" << motherPDGID
                                    << ", pointer=" << moth
                                    << " and barcode=" << moth->barcode()
                                    << " for particle with PDG_ID=" << pdgID
                                    << ", pointer=" << iPart
                                    << " and barcode=" << iPart->barcode() );

                  // Check that a generator particle doesn't have a simulation mother
                  if ( iPart->barcode() < 200000 && moth->barcode() >= 200000 )
                    {
                      ATH_MSG_DEBUG ( "Found a mother with PDG_ID=" << motherPDGID
                                      << ", pointer=" << moth
                                      << " and barcode=" << moth->barcode()
                                      << " for particle with PDG_ID=" << pdgID
                                      << ", pointer=" << iPart
                                      << " and barcode=" << iPart->barcode() 
                                      << ". This should not happen, i.e., the particle seems to come from the generator"
                                      << " while the mother comes from simulation??? Failing this particle..." );
                      break;
                    }

                  //int motherPDGID = moth->pdgId();
                  iPart = moth;

                  // Iterate over the list of allowed PDG_IDs
                  std::vector<int>::const_iterator originPDGItr    = m_originPdgIDList.begin();
                  std::vector<int>::const_iterator originPDGItrEnd = m_originPdgIDList.end();
                  for ( ; originPDGItr != originPDGItrEnd; ++originPDGItr )
                    {
                      if ( m_allowOriginAntiPart )
                        {
                          if ( abs(motherPDGID) == abs(*originPDGItr) )
                            {
                              originFound = true;
                              break;
                            }
                        }
                      else
                        {
                          if ( motherPDGID == (*originPDGItr) )
                            {
                              originFound = true;
                              break;
                            }
                        } // End: if/else ( m_allowOriginAntiPart )
                    } // End: loop over the PDG_ID list
                  // Terminate the loop over generations
                  if ( originFound )
                    {
                      break;
                    }
                } 
              else
                {
                  // no more mother is found
                  ATH_MSG_VERBOSE ( "Found no more mother for particle with PDG_ID=" << pdgID );
                  break;
                }
            } // End: loop over the generations of the decay tree
          isPassed = isPassed && originFound;
          ATH_MSG_VERBOSE ( "TruthParticle origin match PDG_ID is: " << isPassed );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxOriginPDGIDList] += 1; }
        } // End: if m_originPdgIDList.size() != 0

    }
  else
    {
      // Return false 
      isPassed = false;
      ATH_MSG_WARNING ( "Could not dynamic_cast to an part! Failing this part!" );
    } // End: if/else have part

  ATH_MSG_VERBOSE ( "Done processObject" );

  return StatusCode::SUCCESS;
}





//=============================================================================
// Called during the Athena finalize method
//=============================================================================
StatusCode D2PDTruthParticleSelector::finalizeMore()
{
  ATH_MSG_VERBOSE ( "In finalizeMore" );
  return StatusCode::SUCCESS;
}




//=============================================================================
// Search for the first mother with a different PDG_ID
//=============================================================================
int D2PDTruthParticleSelector::getMotherPDGID( const TruthParticle* part,
                                               const TruthParticle* &moth,
                                               int counter,
                                               const TruthParticle* previousPart )
{
  ATH_MSG_VERBOSE ( "In getMotherPDGID" );
  int pdgID = part->pdgId();
  if( part->nParents()>0 )
    {
      const TruthParticle* tmpMoth = part->mother(0);
      if ( !tmpMoth ) 
        {
          ATH_MSG_DEBUG ( "Did NOT get a valid mother, even though we have nParents>0." );
          return 0;
        }
      if ( part == tmpMoth ) 
        {
          ATH_MSG_DEBUG ( "Mother is the same pointer as self... something is fishy." );
          return 0;
        }
      if ( previousPart )
        {
          if ( part == previousPart || tmpMoth == previousPart ) 
            {
              ATH_MSG_DEBUG ( "The previous particle has the same pointer (" << previousPart
                              << ") as the particle (" << part
                              << ") or the mother (" << tmpMoth
                              << ")... something is fishy." );
              return 0;
            }
        }
      int tmpMothPDGID = tmpMoth->pdgId();
      if ( pdgID != tmpMothPDGID )
        { 
          moth = tmpMoth;
          ATH_MSG_VERBOSE ( "Given particle pointer value is: " << part
                            << " and has PDG_ID=" << pdgID
                            << " and barcode=" << part->barcode()
                            << " and mother pointer is: " << tmpMoth
                            << " with PDG_ID=" << tmpMothPDGID
                            << " and barcode=" << tmpMoth->barcode()
                            << ". Done!");
          return tmpMothPDGID;
        }
      else
        { 
          ATH_MSG_VERBOSE ( "Given particle pointer value is: " << part
                            << " and has PDG_ID=" << pdgID
                            << " and barcode=" << part->barcode()
                            << " and mother pointer is: " << tmpMoth
                            << " with PDG_ID=" << tmpMothPDGID
                            << " and barcode=" << tmpMoth->barcode()
                            << ". Now at counter=" << counter );
          counter += 1;
          return this->getMotherPDGID( tmpMoth, moth, counter, part ); 
        }    
    }  
  return 0;
}


//=============================================================================
// Search for the daughters with different PDG_IDs
//=============================================================================
bool D2PDTruthParticleSelector::getDaughters( HepMC::ConstGenParticlePtr genPart, 
                                              std::vector<HepMC::ConstGenParticlePtr > &daughters )
{
  ATH_MSG_VERBOSE ( "In getDaughters" );
  if( genPart->end_vertex() )
    {
      int pdgID   = genPart->pdg_id();
      int barcode = genPart->barcode();

      // Loop over all children
      auto childItrBegin = genPart->end_vertex()->particles_begin(HepMC::children);
      auto  childItrEnd = genPart->end_vertex()->particles_end(HepMC::children);
      for (auto childItr=childItrBegin; childItr != childItrEnd; ++childItr )
        {
          auto child = (*childItr);
          if( child->pdg_id() == pdgID
              && child->barcode() != barcode
              && child->barcode() < 200000 )
            {
              return getDaughters( child, daughters );
            }
          else
            {
              daughters.push_back( child );
            }
        } // End: loop over child particles
    } // End: if the end_vertex exists

  return true;
}
