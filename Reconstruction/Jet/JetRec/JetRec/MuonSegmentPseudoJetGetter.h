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
///  - Pt : The pT set on the MS ghosts

#include "AsgTools/AsgTool.h"
#include "JetInterface/IPseudoJetGetter.h"
#include "fastjet/PseudoJet.hh"
#include "xAODMuon/MuonSegmentContainer.h"
#include "JetEDM/PseudoJetVector.h"
#include "JetRec/PseudoJetContainer.h"


class MuonSegmentPseudoJetGetter
:  public asg::AsgTool,
  virtual public IPseudoJetGetter {
  ASG_TOOL_CLASS(MuonSegmentPseudoJetGetter, IPseudoJetGetter)

public:

  typedef jet::PseudoJetVector PseudoJetVector;

  /// Constructor from tool name.
  MuonSegmentPseudoJetGetter(const std::string& myname);

  /// Initialization.
  /// Can be skipped.
  virtual StatusCode initialize();

  /// Method to construct the PseudoJetVector and record in StoreGate
  StatusCode createAndRecord() const;

  /// Returns the pseudojet collection.
  /// If already existing, the collection in the event store is returned.
  /// If not, an new collection is created and filled by calling @c appendTo.
  /// Returns null if the collection cannot be created.
  const PseudoJetContainer* getC() const;

  /// Return the label for these pseudojets.
  std::string label() const;

  /// Dump to properties to the log.
  void print() const;

protected:  //data

  // Job options.
  SG::ReadHandleKey<xAOD::MuonSegmentContainer> m_incoll;        /// Input collection name.
  SG::WriteHandleKey<PseudoJetContainer> m_outcoll;      /// Output collection name.

  std::string m_label;              /// Label for the collection.
  double m_pt;                      /// PT assiged to the pseudojets.


 private:
  std::vector<fastjet::PseudoJet> 
    createPseudoJets(const xAOD::MuonSegmentContainer*) const; 

};

#endif
