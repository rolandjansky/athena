/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonSegmentPseudoJetGetter.h

#ifndef MuonSegmentPseudoJetGetter_H
#define MuonSegmentPseudoJetGetter_H

// David Adams
// January 2014

/// MuonSegmentPseudoJetGetter is a dual-use tool to retrieve and build the pseudojet
/// inputs from muon segments.
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
/// following: LCTopo, TopoEM, TopoTower, Tower, Truth, Track, PFlow.

#include "AsgTools/AsgTool.h"
#include "JetInterface/IPseudoJetGetter.h"
#include "fastjet/PseudoJet.hh"
#include "xAODMuon/MuonSegmentContainer.h"
#include "JetEDM/PseudoJetVector.h"
#include "JetEDM/LabelIndex.h"

class MuonSegmentPseudoJetGetter
:  public asg::AsgTool,
  virtual public IPseudoJetGetter {
  ASG_TOOL_CLASS(MuonSegmentPseudoJetGetter, IPseudoJetGetter)

public:

  typedef jet::LabelIndex LabelIndex;
  typedef jet::PseudoJetVector PseudoJetVector;

  /// Constructor from tool name.
  MuonSegmentPseudoJetGetter(const std::string& myname);

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

  /// Construct pseudojets from the the inputs using the supplied label index
  /// and append them to the pseudojet vector.
  int append(const xAOD::MuonSegmentContainer& inputs, PseudoJetVector& psjs, const LabelIndex* pli) const;

  /// Return the label for these pseudojets.
  std::string label() const;

  /// Dump to properties to the log.
  void print() const;

protected:  //data

  // Job options.
  std::string m_incoll;             /// Input collection name.
  std::string m_outcoll;            /// Output collection name.
  std::string m_label;              /// Label for the collection.
  double m_pt;                      /// PT assiged to the pseudojets.

};

#endif
