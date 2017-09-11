
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////// -*- C++ -*- /////////////////////////////
// ParticleSortingTool.cxx
// Implementation file for class ParticleSortingTool
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////


// EventUtils includes
#include "ParticleSortingTool.h"

// STL includes
#include <vector>
#include <string>

// FrameWork includes

// EDM includes
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODTracking/NeutralParticleContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODParticleEvent/ParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "AthContainers/ConstDataVector.h"



// Constructors
////////////////
ParticleSortingTool::ParticleSortingTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent ) :
  ::AthAlgTool  ( type, name, parent ),
  m_inCollKey(""),
  m_outCollKey(""),
  m_sortVar("pt"),
  m_sortDescending(true),
  m_contID(0),
  m_sortID(0),
  m_nEventsProcessed(0)
{
  declareInterface< DerivationFramework::IAugmentationTool >(this);

  declareProperty("InputContainer",  m_inCollKey="",   "Input container name" );

  declareProperty("OutputContainer", m_outCollKey="",
                  "The name of the output container (with SG::VIEW_ELEMENTS) with the sorted copy of input objects" );

  declareProperty("SortVariable",    m_sortVar="pt",
                  "Define by what parameter to sort (default: 'pt'; allowed: 'pt', 'eta', 'phi', 'm', 'e', 'rapidity')" );

  declareProperty("SortDescending",   m_sortDescending=true,
                  "Define if the container should be sorted in a descending order (default=true)" );
}


// Destructor
///////////////
ParticleSortingTool::~ParticleSortingTool()
{}



// Athena algtool's Hooks
////////////////////////////
StatusCode ParticleSortingTool::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using = " << m_inCollKey );
  ATH_MSG_DEBUG ( " using = " << m_outCollKey );

  // initialize the counters
  m_contID           = 0;
  m_sortID           = 0;
  m_nEventsProcessed = 0;

  // Figure out how to sort
  if ( m_sortVar.value() == "pt" )            { m_sortID = 1; }
  else if ( m_sortVar.value() == "eta" )      { m_sortID = 2; }
  else if ( m_sortVar.value() == "phi" )      { m_sortID = 3; }
  else if ( m_sortVar.value() == "m" )        { m_sortID = 4; }
  else if ( m_sortVar.value() == "e" )        { m_sortID = 5; }
  else if ( m_sortVar.value() == "rapidity" ) { m_sortID = 6; }
  else {
    ATH_MSG_INFO("Didn't find a valid value for SortVariable=" << m_sortVar.value() << "."
                 << " Assuming it's an auxdata member");
    m_sortID = 7;
  }
  if ( m_sortDescending.value() ) { m_sortID *= -1; }

  return StatusCode::SUCCESS;
}




StatusCode ParticleSortingTool::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}



// Declare a short pre-processor macro to deal with the different container types
#define COPY_AND_SORT_CONTAINER( CONTAINERTYPE )                                       \
else if ( evtStore()->contains<CONTAINERTYPE>( m_inCollKey.value() ) ) {               \
  ATH_MSG_DEBUG("Trying to copy, sort, and record container of type "#CONTAINERTYPE ); \
  const CONTAINERTYPE* inCont;                                                         \
  ATH_CHECK( evtStore()->retrieve( inCont, m_inCollKey.value() ) );                    \
  CONTAINERTYPE* outCont = new CONTAINERTYPE( SG::VIEW_ELEMENTS );                     \
  *outCont = *inCont;                                                                  \
  ATH_CHECK( evtStore()->record ( outCont, m_outCollKey.value() ) );                   \
  ATH_CHECK( this->doSort(outCont) );                                                  \
}


// Declare a short pre-processor macro to deal with the different container types
#define OVERWRITE_AND_SORT_CONTAINER( CONTAINERTYPE )                                                \
else if ( evtStore()->contains<CONTAINERTYPE>( m_inCollKey.value() ) ) {                             \
  ATH_MSG_DEBUG("Trying to copy, sort, and overwrite container of type "#CONTAINERTYPE );            \
  const CONTAINERTYPE* inCont;                                                                       \
  ATH_CHECK( evtStore()->retrieve( inCont, m_inCollKey.value() ) );                                  \
  ConstDataVector<CONTAINERTYPE>* outCont = new ConstDataVector<CONTAINERTYPE>( SG::VIEW_ELEMENTS ); \
  for ( const CONTAINERTYPE::base_value_type* inPart : *inCont ){                                    \
    outCont->push_back(inPart);                                                                      \
  }                                                                                                  \
  ATH_CHECK( evtStore()->overwrite( outCont, m_inCollKey.value() ) );                                \
  ATH_CHECK( this->doSortConst<CONTAINERTYPE>(outCont) );                                            \
}



StatusCode ParticleSortingTool::addBranches() const
{
  // Increase the event counter
  ++m_nEventsProcessed;

  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> addBranches " << name() << " on " << m_nEventsProcessed << ". event..." );

  if ( m_outCollKey.value().empty() ) {
    // Try to get the input container as non-const
    ATH_MSG_DEBUG("Got an empty 'OutputCollection' property. "
                  << "Trying to retrieve a non-const version of the 'InputContainer'...");
    xAOD::IParticleContainer* inCont = evtStore()->tryRetrieve<xAOD::IParticleContainer>( m_inCollKey.value() );
    if (inCont){ ATH_CHECK( this->doSort(inCont) ); }
    else {
      ATH_MSG_DEBUG("We couldn't retrieve a non-const version of the input container... try const.");
      const xAOD::IParticleContainer* inCont2 = nullptr;
      ATH_CHECK( evtStore()->retrieve( inCont2, m_inCollKey.value()) );
      // Now, do the copy and sorting and overwriting of all known container types
      if (false) {
      }
      OVERWRITE_AND_SORT_CONTAINER(xAOD::MuonContainer)
      OVERWRITE_AND_SORT_CONTAINER(xAOD::ElectronContainer)
      OVERWRITE_AND_SORT_CONTAINER(xAOD::PhotonContainer)
      OVERWRITE_AND_SORT_CONTAINER(xAOD::TauJetContainer)
      OVERWRITE_AND_SORT_CONTAINER(xAOD::JetContainer)
      OVERWRITE_AND_SORT_CONTAINER(xAOD::PFOContainer)
      OVERWRITE_AND_SORT_CONTAINER(xAOD::NeutralParticleContainer)
      OVERWRITE_AND_SORT_CONTAINER(xAOD::TrackParticleContainer)
      OVERWRITE_AND_SORT_CONTAINER(xAOD::TruthParticleContainer)
      OVERWRITE_AND_SORT_CONTAINER(xAOD::CompositeParticleContainer)
      OVERWRITE_AND_SORT_CONTAINER(xAOD::ParticleContainer)
      OVERWRITE_AND_SORT_CONTAINER(xAOD::CaloClusterContainer)
      else {
        ATH_MSG_ERROR("Couln't find the provided intput container in store gate for later overwriting");
        return StatusCode::FAILURE;
      }
    }
  }
  else {
    ATH_MSG_DEBUG("Got a non-empty 'OutputCollection' property. "
                  << "Trying to retrieve a const version of the 'InputContainer'...");

    // Now, do the copy and sorting of all known container types
    if (false) {
    }
    COPY_AND_SORT_CONTAINER(xAOD::MuonContainer)
    COPY_AND_SORT_CONTAINER(xAOD::ElectronContainer)
    COPY_AND_SORT_CONTAINER(xAOD::PhotonContainer)
    COPY_AND_SORT_CONTAINER(xAOD::TauJetContainer)
    COPY_AND_SORT_CONTAINER(xAOD::JetContainer)
    COPY_AND_SORT_CONTAINER(xAOD::PFOContainer)
    COPY_AND_SORT_CONTAINER(xAOD::NeutralParticleContainer)
    COPY_AND_SORT_CONTAINER(xAOD::TrackParticleContainer)
    COPY_AND_SORT_CONTAINER(xAOD::TruthParticleContainer)
    COPY_AND_SORT_CONTAINER(xAOD::CompositeParticleContainer)
    COPY_AND_SORT_CONTAINER(xAOD::ParticleContainer)
    COPY_AND_SORT_CONTAINER(xAOD::CaloClusterContainer)
    else {
      ATH_MSG_ERROR("Couln't find the provided intput container in store gate");
      return StatusCode::FAILURE;
    }

  }

  return StatusCode::SUCCESS;
}



StatusCode ParticleSortingTool::doSort( xAOD::IParticleContainer* cont ) const
{
  if ( !cont ) {
    ATH_MSG_ERROR("No container to be sorted");
    return StatusCode::FAILURE;
  }
  // Actually do the sorting, using a C++11 lambda function construct
  // to be able to use the member function here
  if ( abs(m_sortID) == 1 ) {
    cont->sort( [this](const xAOD::IParticle* a, const xAOD::IParticle* b) {
                  return this->comparePt(a,b);
                } );
  }
  else if ( abs(m_sortID) == 2 ) {
    cont->sort( [this](const xAOD::IParticle* a, const xAOD::IParticle* b) {
                  return this->compareEta(a,b);
                } );
  }
  else if ( abs(m_sortID) == 3 ) {
    cont->sort( [this](const xAOD::IParticle* a, const xAOD::IParticle* b) {
                  return this->comparePhi(a,b);
                } );
  }
  else if ( abs(m_sortID) == 4 ) {
    cont->sort( [this](const xAOD::IParticle* a, const xAOD::IParticle* b) {
                  return this->compareMass(a,b);
                } );
  }
  else if ( abs(m_sortID) == 5 ) {
    cont->sort( [this](const xAOD::IParticle* a, const xAOD::IParticle* b) {
                  return this->compareEnergy(a,b);
                } );
  }
  else if ( abs(m_sortID) == 6 ) {
    cont->sort( [this](const xAOD::IParticle* a, const xAOD::IParticle* b) {
                  return this->compareRapidity(a,b);
                } );
  }
  else if ( abs(m_sortID) == 7 ) {
    cont->sort( [this](const xAOD::IParticle* a, const xAOD::IParticle* b) {
                  return this->compareAuxData(a,b);
                } );
  }

  return StatusCode::SUCCESS;
}


bool ParticleSortingTool::comparePt( const xAOD::IParticle* partA,
                                     const xAOD::IParticle* partB ) const
{
  const double a = partA->pt();
  const double b = partB->pt();
  return this->compareDouble(a,b);
}


bool ParticleSortingTool::compareEta( const xAOD::IParticle* partA,
                                      const xAOD::IParticle* partB ) const
{
  const double a = partA->eta();
  const double b = partB->eta();
  return this->compareDouble(a,b);
}


bool ParticleSortingTool::comparePhi( const xAOD::IParticle* partA,
                                      const xAOD::IParticle* partB ) const
{
  const double a = partA->phi();
  const double b = partB->phi();
  return this->compareDouble(a,b);
}


bool ParticleSortingTool::compareMass( const xAOD::IParticle* partA,
                                       const xAOD::IParticle* partB ) const
{
  const double a = partA->m();
  const double b = partB->m();
  return this->compareDouble(a,b);
}


bool ParticleSortingTool::compareEnergy( const xAOD::IParticle* partA,
                                         const xAOD::IParticle* partB ) const
{
  const double a = partA->e();
  const double b = partB->e();
  return this->compareDouble(a,b);
}


bool ParticleSortingTool::compareRapidity( const xAOD::IParticle* partA,
                                           const xAOD::IParticle* partB ) const
{
  const double a = partA->rapidity();
  const double b = partB->rapidity();
  return this->compareDouble(a,b);
}

bool ParticleSortingTool::compareAuxData( const xAOD::IParticle* partA,
                                       const xAOD::IParticle* partB ) const
{
  const double a = partA->auxdata<float>( this->m_sortVar.value() );
  const double b = partB->auxdata<float>( this->m_sortVar.value() );
  return this->compareDouble(a,b);
}
