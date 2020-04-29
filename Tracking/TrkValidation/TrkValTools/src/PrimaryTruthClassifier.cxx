/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// PrimaryTruthClassifier.cxx
//   Source file for class PrimaryTruthClassifier
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkValTools/PrimaryTruthClassifier.h"
#include "TrkValEvent/TruthClassificationDefs.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AtlasHepMC/GenVertex.h"

Trk::PrimaryTruthClassifier::PrimaryTruthClassifier(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool                    (type,name,parent),
    m_maxRStartPrimary         (  25.0*CLHEP::mm),
    m_maxZStartPrimary         ( 200.0*CLHEP::mm),
    m_minREndPrimary           ( 400.0*CLHEP::mm),
    m_minZEndPrimary           (2300.0*CLHEP::mm),
    m_maxRStartSecondary       ( 360.0*CLHEP::mm),
    m_maxZStartSecondary       (2000.0*CLHEP::mm),
    m_minREndSecondary         (1000.0*CLHEP::mm),
    m_minZEndSecondary         (3200.0*CLHEP::mm)
{
  declareInterface<ITrackTruthClassifier>(this);

  declareProperty("maxRStartPrimary",   m_maxRStartPrimary);
  declareProperty("maxZStartPrimary",   m_maxZStartPrimary);
  declareProperty("minREndPrimary",	    m_minREndPrimary);
  declareProperty("minZEndPrimary",	    m_minZEndPrimary);
  declareProperty("maxRStartSecondary", m_maxRStartSecondary);
  declareProperty("maxZStartSecondary", m_maxZStartSecondary);
  declareProperty("minREndSecondary",	m_minREndSecondary);
  declareProperty("minZEndSecondary",	m_minZEndSecondary);

}

///////////////////////////////
/// initialize
///////////////////////////////
StatusCode Trk::PrimaryTruthClassifier::initialize() {
    ATH_MSG_INFO ("initialise successful in "<<name() );
    return StatusCode::SUCCESS;
}

StatusCode Trk::PrimaryTruthClassifier::finalize() {
  ATH_MSG_INFO ("starting finalize() in " << name() );
  return StatusCode::SUCCESS;
}

void Trk::PrimaryTruthClassifier::initClassification
(const McEventCollection& /*SimTracks*/,
 const std::vector<const HepMC::GenParticle *>* /*genSignal*/) const {

  // nothing to prepare as local data at start of collection analysis
  return;
}

//////////////////////////////////////////
// classification from InDetRecStatistics
//////////////////////////////////////////
unsigned int Trk::PrimaryTruthClassifier::classify(const HepMC::GenParticle& genParticle) const {

 
  /* note on using HepMC::FourVector/3Vector against HepGeom::Point3D<double>: The versions from HepMC2 do not know
     operator+, operator- etc. */

    
  //classify as primary, secondary or truncated
  bool  primary=false;
  bool  secondary=false;
  bool  truncated=false;

  if (genParticle.production_vertex()) {
    HepMC::FourVector      startVertex = genParticle.production_vertex()->position();

    // primary vertex inside innermost layer?
    if ( fabs(startVertex.perp()) < m_maxRStartPrimary 
         && fabs(startVertex.z()) < m_maxZStartPrimary)
      {
        if (genParticle.end_vertex() == 0) {  
          primary=true;
        } else {
          HepMC::FourVector endVertex = genParticle.end_vertex()->position();
          if (  endVertex.perp()         > m_minREndPrimary 
                || fabs(startVertex.z()) > m_minZEndPrimary)
            primary=true; else truncated = true;
        }
      }
    else if ( startVertex.perp()    <  m_maxRStartSecondary && 
              fabs(startVertex.z()) <  m_maxZStartSecondary)
      {
        if (genParticle.end_vertex() == 0) {  
          secondary=true;
        } else {
          HepMC::FourVector endVertex = genParticle.end_vertex()->position();
          if (endVertex.perp()            > m_minREndSecondary
              || fabs(endVertex.z())      > m_minZEndSecondary) {
            secondary=true;
          }
        }
      }
  } //end classification

  if (truncated) return Trk::TruthClassification::Truncated;
  if (secondary) return Trk::TruthClassification::Secondary;
  if (primary) return Trk::TruthClassification::Primary;
  ATH_MSG_DEBUG ( "Could not classify this particle: " 
        << genParticle );
  return Trk::TruthClassification::OutsideClassification;

}
