// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PseudoJetGetter.h

#ifndef PseudoJetGetter_H
#define PseudoJetGetter_H

// David Adams
// January 2014

/// PseudoJetGetter is a dual-use tool to retrieve and build the pseudojet
/// inputs used in jet finding.
///
/// Tool Properties:
///  - InputCollection: Name of the input collection.
///  - OutputCollection: Name of the output collection of pseudojets.
///  - Label: Label for the constituents. See note below.
///  - SkipNegativeEnergy: Flag indicating that inputs with negative energy
/// should be ignored.
///  - GhostScale : If nonzero, the pseudojets are labeled as ghosts and
/// their four-momenta are scaled by this factor.
///
/// Note: The label is attached to the CUI (constituent user info) associated with
/// created pseudojet and should be unique for each set of pseuodjets used as input
/// for finding a particular collection of jets.
///
/// The label for a primary pseudojet getter (the first in the vector presented
/// to JetRecTool) is used to deduce the input type and must be one of the
/// following: LCTopo, EMTopo, TopoTower, Tower, Truth, Track, PFlow.
/// Names beginning with "EM" are assumed to correspond to uncalibrated clusters.

#include "AsgTools/AsgTool.h"
#include "JetInterface/IPseudoJetGetter.h"
#include "fastjet/PseudoJet.hh"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODJet/JetContainer.h"
#include "JetEDM/PseudoJetVector.h"
#include "JetEDM/IndexedConstituentUserInfo.h"
#include "JetEDM/LabelIndex.h"

class PseudoJetGetter
:  public asg::AsgTool,
  virtual public IPseudoJetGetter {
  ASG_TOOL_CLASS(PseudoJetGetter, IPseudoJetGetter)

public:

  typedef jet::LabelIndex LabelIndex;
  typedef jet::PseudoJetVector PseudoJetVector;

  /// Constructor from tool name.
  PseudoJetGetter(const std::string& myname);

  /// Initialization.
  /// Can be skipped.
  virtual StatusCode initialize();

  /// Returns the pseudojet collection.
  /// If already existing, the collection in the event store is returned.
  /// If not, an new collection is created and filled by calling @c appendTo.
  /// Returns null if the collection cannot be created.
  const PseudoJetVector* get() const;

  /// Find the input collection in the event store and call @c append
  /// to fill the psedojet vector.
  virtual int appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const;

  /// Construct pseudojets from the the inputs using the suppled label index
  /// and append them to the pseudojet vector.
  template<typename TList>
  int append(const TList& inputs, PseudoJetVector& psjs, const LabelIndex* pli) const;

  /// Return the label for these pseudojets.
  std::string label() const;

  /// Dump to properties to the log.
  void print() const;

  /// Method to return the list of input containers.
  /// The names of required input containers are appended to connames.
  /// Returns nonzero for error.
  /// Default returns 0 and adds no names.
  int inputContainerNames(std::vector<std::string>& connames);

  /// Method to return the list of output containers.
  /// The names of produced output containers are appended to connames.
  /// Returns nonzero for error.
  /// Default returns 0 and adds no names.
  int outputContainerNames(std::vector<std::string>& connames);

protected:  // member functions

  virtual jet::IConstituentUserInfo*
  buildCUI(const xAOD::IParticle* ppar, jet::IConstituentUserInfo::Index idx,
           const LabelIndex* pli) const;

protected:  // data

  // Job options.
  std::string m_incoll;             /// Input collection name.
  std::string m_outcoll;            /// Output collection name.
  std::string m_label;              /// Label for the collection.
  bool m_skipNegativeEnergy;        /// Flag indicating to skip objects with E<0.
  double m_ghostscale;              /// Ghost scale factor.
  bool m_negEnergyAsGhosts;         /// Flag indicating to treat objects with E<0 as ghosts  (usefull hor HI)


  // Cached flags.
  bool m_emtopo;                    /// True if inputs are EM-scale topo clusters.

};


#ifdef USE_BOOST_AUTO
#include <boost/typeof/typeof.hpp>
#endif

template<typename TList>
int PseudoJetGetter::
append(const TList& inputs, PseudoJetVector& psjs, const LabelIndex* pli) const {
  int nskip = 0;
  jet::IConstituentUserInfo::Index labidx = 0;
  if ( pli != 0 ) labidx = pli->index(m_label);
  else ATH_MSG_WARNING("Index-to-label map is not supplied.");
  //  if ( m_ghostscale ) labidx = -labidx;
  if(!m_negEnergyAsGhosts && m_ghostscale) labidx = -labidx;
  ATH_MSG_DEBUG( "Ghost scale =  " << m_ghostscale << "; idx = " << labidx );
  
  /// Loop over input, buid CUI and PseudoJets
#ifdef USE_BOOST_AUTO
  for ( BOOST_AUTO(iinp, inputs.begin()); iinp!=inputs.end(); ++iinp ) {
    BOOST_AUTO(ppar, *iinp); // IParticle pointer
#else
  for ( auto iinp=inputs.begin(); iinp!=inputs.end(); ++iinp ) {
    auto ppar = *iinp; // IParticle pointer
#endif
    if ( ppar == nullptr || (m_skipNegativeEnergy && ppar->e()<FLT_MIN) ) {
      if ( ppar == nullptr ) ATH_MSG_DEBUG("Null object!");
      else ATH_MSG_VERBOSE("Skipping constituent with E = " << ppar->e());
      ++nskip;
      continue;
    }
    // Take momentum from IParticle.
    fastjet::PseudoJet psj(ppar->p4());
    // Form EM scale, use uncalibrated four-vector.1
    if ( m_emtopo ) {
      const xAOD::CaloCluster* pclu = dynamic_cast<const xAOD::CaloCluster*>(ppar);
      if ( pclu == 0 ) {
        ATH_MSG_WARNING("EM particle is not type CaloCluster");
        continue;
      }
      psj.reset(pclu->p4(xAOD::CaloCluster::UNCALIBRATED));
    }

    if( m_negEnergyAsGhosts) {
      if (ppar->e() <= 0.0)  { // Important use the IParticle::e(), because IParticle::p4().e() might always be >0.
        psj.reset_momentum(psj.px(), psj.py(), psj.pz(), std::abs( psj.e() )); // make sure e>0 in fastjet
        psj *= m_ghostscale;
      }
    } else if ( m_ghostscale ) psj *= m_ghostscale;

    bool show = psjs.size() < 20;
    if ( show ) ATH_MSG_VERBOSE("index/label: " << labidx << "/" << m_label);
    if ( show ) ATH_MSG_VERBOSE("old/p4/new"
                  << " pt: "   << ppar->pt()  << "/" << ppar->p4().Pt()  << "/" << psj.pt()
                  << ", pz: /"                       << ppar->p4().Pz()  << "/" << psj.pz()
                  << ", p: /"                        << ppar->p4().P()   << "/"
                  << ", E: "   << ppar->e()   << "/" << ppar->p4().E()   << "/" << psj.e()
                  << ", m: "   << ppar->m()   << "/" << ppar->p4().M()   << "/" << psj.m()
                  << ", eta: " << ppar->eta() << "/" << ppar->p4().Eta() << "/" << psj.eta()
                );
    if ( pli != 0 ) {
      jet::IConstituentUserInfo* pcui = this->buildCUI(ppar, labidx, pli);
      psj.set_user_info(pcui);
      if ( show ) ATH_MSG_VERBOSE("User info particle: " << pcui->particle());
    }
    psjs.push_back(psj);
  } // end loop over input 

  if ( nskip ) ATH_MSG_DEBUG("Skipped constituent count: " << nskip);
  ATH_MSG_DEBUG("After append, PseudoJet count is " << psjs.size());
  return 0;
}

#endif
