/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/I4MomentumContainer.h"

#include "EventShapeTools/YFlipValues.h"

#include "fastjet/JetDefinition.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"

#include <cmath>

YFlipValues::YFlipValues(const std::string& type,
			 const std::string& name,
			 const IInterface* pParent)
  : EventShapeToolBase(type, name, pParent)
  , m_algorithm("anti-kt")
  , m_radius(0.7)
  , m_recombscheme("E")
  , m_strategy("Best")
  , m_no_flipValues(6)
  // added 2D option
  , m_calc3D(true)
{
  declareInterface<IEventShapeToolBase>( this );
  declareProperty("Algorithm",    m_algorithm);
  declareProperty("Radius",       m_radius);
  declareProperty("RecombScheme", m_recombscheme);
  declareProperty("Strategy",     m_strategy);
  // adding 2D option
  declareProperty("CalculateIn3D",     m_calc3D);
  
  // selection of the jet collection
  // declareProperty("Inclusive",      m_inclusive);
  declareProperty("CalculateNFlipValues", m_no_flipValues);
}

StatusCode YFlipValues::initialize()
{
  // set up the clustering algorithm (default is cambridge)
  fastjet::JetFinder algo = fastjet::cambridge_algorithm;
  if (m_algorithm == "cambridge") {
    // already set ...
  } else if (m_algorithm == "kt") {
    algo = fastjet::kt_algorithm;
  } else if (m_algorithm == "anti-kt") {
    algo = fastjet::antikt_algorithm;
  } else if (m_algorithm == "genkt") {
    algo = fastjet::genkt_algorithm;
  } else if (m_algorithm == "passive cambridge") {
    algo = fastjet::cambridge_for_passive_algorithm;
  } else if (m_algorithm == "passive genkt") {
    algo = fastjet::genkt_for_passive_algorithm;
  } else {
    msg( MSG::FATAL) << "jobOption Problem in " << this->name()
		     << " ! Property \042Algorithm\42 mis-configured! ("
		     << m_algorithm << ") Giving up!" << endreq;
    return StatusCode::FAILURE;
  }
  
  // recombination scheme:
  fastjet::RecombinationScheme recomb_scheme = fastjet::E_scheme;
  if (m_recombscheme == "E") {
    // already set ...
  } else if (m_recombscheme == "pt") {
    recomb_scheme = fastjet::pt_scheme;
  } else if (m_recombscheme == "pt2") {
    recomb_scheme = fastjet::pt2_scheme;
  } else if (m_recombscheme == "Et") {
    recomb_scheme = fastjet::Et_scheme;
  } else if (m_recombscheme == "Et2") {
    recomb_scheme = fastjet::Et2_scheme;
  } else if (m_recombscheme == "BIpt") {
    recomb_scheme = fastjet::BIpt_scheme;
  } else if (m_recombscheme == "BIpt2") {
    recomb_scheme = fastjet::BIpt2_scheme;
  } else {
    msg( MSG::FATAL) << "jobOption Problem " << this->name()
		     << " ! Property \042RecombinationScheme\42 mis-configured! ("
		     << m_recombscheme << ") Giving up!" << endreq;
    return StatusCode::FAILURE;
  }
  
  // strategy
  fastjet::Strategy strategy = fastjet::Best;
  if (m_strategy == "N2MinHeapTiled") {
    strategy = fastjet::N2MinHeapTiled;
  } else if (m_strategy == "N2Tiled") {
    strategy = fastjet::N2Tiled;
  } else if (m_strategy == "N2PoorTiled") {
    strategy = fastjet::N2PoorTiled;
  } else if (m_strategy == "N2Plain") {
    strategy = fastjet::N2Plain;
  } else if (m_strategy == "N3Dumb") {
    strategy = fastjet::N3Dumb;
  } else if (m_strategy == "Best") {
    // this is the default
  } else if (m_strategy == "NlnN") {
    strategy = fastjet::NlnN;
  } else if (m_strategy == "NlnN3pi") {
    strategy = fastjet::NlnN3pi;
  } else if (m_strategy == "NlnN4pi") {
    strategy = fastjet::NlnN4pi;
  } else if (m_strategy == "NlnNCam4pi") {
    strategy = fastjet::NlnNCam4pi;
  } else if (m_strategy == "NlnNCam2pi2R") {
    strategy = fastjet::NlnNCam2pi2R;
  } else if (m_strategy == "NlnNCam") {
    strategy = fastjet::NlnNCam;
  } else {
    msg( MSG::FATAL) << "jobOption Problem " << this->name()
		     << " ! Property \042Strategy\42 mis-configured! ("
		     << m_strategy << ") Giving up!" << endreq;
    return StatusCode::FAILURE;
  }
  
  if ( m_radius < 0 )
    {
      msg( MSG::FATAL) << "jobOption Problem " << this->name()
		       << " ! Property \042Radius\42 mis-configured! ("
		       << m_radius << ") Giving up!" << endreq;
    return StatusCode::FAILURE;
  }
  
  // ok, create jet definitions (delete default first)
  m_jetdefinition = new fastjet::JetDefinition(algo, m_radius, recomb_scheme, strategy);
  
  msg( MSG::INFO ) << "Will use FastJet with these parameters" << endreq;
  
  switch(algo)
    {
    case fastjet::cambridge_algorithm:             m_algorithm = "Cambridge"; break;
    case fastjet::kt_algorithm:                    m_algorithm = "Kt"; break;
    case fastjet::antikt_algorithm:                m_algorithm = "Anti-Kt"; break;
    case fastjet::genkt_algorithm:                 m_algorithm = "GenKt"; break;
    case fastjet::cambridge_for_passive_algorithm: m_algorithm = "Passive Cambridge"; break;
    case fastjet::genkt_for_passive_algorithm:     m_algorithm = "Passive GenKt"; break;
    case fastjet::plugin_algorithm:                m_algorithm = "Plugin"; break;
    default:                                       m_algorithm = "unknown"; break;
    }
  
  // strategy
  switch(strategy)
    {
    case fastjet::Best:            m_strategy = "Best"; break;
    case fastjet::N2MinHeapTiled:  m_strategy = "N2MinHeapTiled"; break;
    case fastjet::N2Tiled:         m_strategy = "N2Tiled"; break;
    case fastjet::N2PoorTiled:     m_strategy = "N2PoorTiled"; break;
    case fastjet::N2Plain:         m_strategy = "N2Plain"; break;
    case fastjet::N3Dumb:          m_strategy = "N3Dumb"; break;
    case fastjet::NlnN:            m_strategy = "NlnN"; break;
    case fastjet::NlnN3pi:         m_strategy = "NlnN3pi"; break;
    case fastjet::NlnN4pi:         m_strategy = "NlnN4pi"; break;
    case fastjet::NlnNCam4pi:      m_strategy = "NlnNCam4pi"; break;
    case fastjet::NlnNCam2pi2R:    m_strategy = "NlnNCam2pi2R"; break;
    case fastjet::NlnNCam:         m_strategy = "NlnNCam";; break;
    case fastjet::plugin_strategy: m_strategy = "Plugin"; break;
    default:                       m_strategy = "unknown"; break;
    }
  
  msg( MSG::INFO ) << " Algorithm............... " << m_algorithm << endreq;
  msg( MSG::INFO ) << " Radius.................. " << m_radius << endreq; 
  msg( MSG::INFO ) << " Recombination Scheme.... " << m_jetdefinition->recombiner()->description() << endreq; 
  msg( MSG::INFO ) << " Strategy................ " << m_strategy << endreq;
  msg( MSG::INFO ) << "Will calculate " << m_no_flipValues << " FlipValues" << endreq;

  // adding 2D option
  msg( MSG::INFO ) << "Calculating Y Flip Values in "
                   << (m_calc3D ? "three" : "two")
                   << " dimensions. Default: Three-D"
                   << endreq;
  
  return StatusCode::SUCCESS;
}

StatusCode
YFlipValues::calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
		       EventShapeStore* eventShapeStore )
{
  if (msgLvl(MSG::VERBOSE))
    msg(MSG::VERBOSE) << "Initial collection has size " << theParticles->size() << endreq;
  
  std::vector<fastjet::PseudoJet> input_particles(theParticles->size());
  
  for ( unsigned int index(0); index < theParticles->size(); ++index )
    if (m_calc3D)
      input_particles[index] = fastjet::PseudoJet( (*theParticles)[index]->px(),
						   (*theParticles)[index]->py(),
						   (*theParticles)[index]->pz(),
						   (*theParticles)[index]->e() );
    else
      input_particles[index] = fastjet::PseudoJet( (*theParticles)[index]->px(),
						   (*theParticles)[index]->py(),
						   0.0,					// pz = 0
						   (*theParticles)[index]->pt() );	// m = 0
  
  m_jetdefinition = new fastjet::JetDefinition (fastjet::kt_algorithm,
						m_radius,
						fastjet::E_scheme,
						fastjet::Best);
  m_clusterSeq = new fastjet::ClusterSequence(input_particles, *m_jetdefinition);
  
  std::vector<double> yflip(m_no_flipValues);
  if (msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "Calculation of YFlipValues gave: " << endreq;
  for ( unsigned int i(0); i < m_no_flipValues; i++ )
    {
      yflip[i] = m_clusterSeq->exclusive_dmerge(i+1);
      if (msgLvl(MSG::DEBUG))
	msg(MSG::DEBUG) << "  Y" << i+1 << " = " << yflip[i] << endreq;
    }
  eventShapeStore->insert(EventShapes::FLIPVALUES, yflip, true);
  
  return StatusCode::SUCCESS;
}

