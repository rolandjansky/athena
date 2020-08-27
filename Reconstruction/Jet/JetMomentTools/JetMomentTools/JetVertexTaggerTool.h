/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetVertexTaggerTool.h

#ifndef JETMOMENTTOOLS_JETVERTEXTAGGERTOOL_H
#define JETMOMENTTOOLS_JETVERTEXTAGGERTOOL_H

/// James Frost \n
/// November 2014
///
/// Updated for Run 3 by Bill Balunas
/// March 2020
///
/// Tool to calculate the jet vertex tag (JVT)
/// JVT is a float per jet
///
/// The JVT likelihood is parameterised in terms of:
/// RPT: ratio of the primary vertex track sum to the jet pT
/// JVFCorr: a corrected JVF calculation accounting for the number of PU tracks in the event.
///
/// Calculation requires three main types of information
///     1. Vertex container for the event (from evtStore), with respect to which the JVT track sums 
///     2. Tracks associated to each of the input jet (in the jet aux store)
///     3. Track vertex association object (from evtStore)
///     4. The track container needed for PU track counting
///     5. The ID track selector tool to provide an input track selection.
///     6. An input file with the JVT likelihood stored as a 2D histogram.
///
/// Using this information, the procedure can be broken into three main steps:
///     1. Count the total number of pileup tracks
///     2. Per jet: Get track pT sums needed for JVFCorr and RpT
///     3.          Use JVT likelihood to get JVT value
///
/// Attributes added (each is a float per jet):
///     1. Jvt - the JVTLikelihood value for the jet. This is parameterised in terms of:
///     2. JvtRpt - the ratio of the PV track pT sum to the jet pT
///     3. JvtJvfcorr - a corrected Jvf quantity that corrects for the number of pileup tracks in an event.
///
/// Properties:
///  VertexContainer - name of the vertex container
///  TrackParticleContainer - name of the track container
///  AssociatedTracks - name for attribute holding the list of associated tracks
///  TrackVertexAssociation - name for the container holding the track-vertex associations
///  TrackSelector - tool to select tracks (none ==> no selection)
///  JVTFileName - ROOT Filename containing JVT likelihood histogram	
///  JVTLikelihoodHistName - JVT Likelihood histogram name
///  JVTName - name for the 3 JVT attributes (default is "JVT")
///  K_JVFCorrScale - the scale factor for pileup tracks in the JVFCorr calculation (default is 0.01)
///  Z0Cut - Z0 value (in mm) within which tracks associated to no vertex may be assigned to the primary vertex (default is 3.)
///  PUTrkPtCut - the track pT (in MeV) below which tracks associated to no vertex may be assigned to the primary vertex (default is 30000.)

#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "JetRec/JetModifierBase.h"
#include "JetEDM/TrackVertexAssociation.h"
#include "JetInterface/IJetDecorator.h"
#include "JetInterface/IJetUpdateJvt.h"

#include <vector>
#include <string>

#include <TFile.h>
#include <TString.h>
#include <TH1D.h>
#include <TH2D.h>

class JetVertexTaggerTool
: public asg::AsgTool,
  virtual public IJetDecorator,
  virtual public IJetUpdateJvt {
  ASG_TOOL_CLASS2(JetVertexTaggerTool,IJetDecorator,IJetUpdateJvt)

public:
  // Constructor from tool name
  JetVertexTaggerTool(const std::string& name);

  // Initialization.
  StatusCode initialize() override;

  // Inherited method to decorate a jet container
  virtual StatusCode decorate(const xAOD::JetContainer& jetCont) const override;

  // Evaluate JVT from Rpt and JVFcorr.
  float evaluateJvt(float rpt, float jvfcorr) const;

  // Update JVT by scaling Rpt byt the ratio of the current and original jet pT values.
  //   jet - jet for which JVT is updated
  //   scale - name of the jet scale holding the original pT
  // The new value for JVT is returned.
  float updateJvt(const xAOD::Jet& jet, std::string scale ="JetPileupScaleMomentum") const override;

  // Local method to return the HS vertex - that of type PriVtx
  const xAOD::Vertex* findHSVertex(const xAOD::VertexContainer*&) const;

private:  // data

  // Configurable parameters
  Gaudi::Property<std::string> m_jetContainerName{this,"JetContainer", "", "SG key for the input jet container"};
  Gaudi::Property<std::string> m_jvtlikelihoodHistName{this, "JVTLikelihoodHistName", "JVTRootCore_kNN100trim_pt20to50_Likelihood", "JVT likelihood histogram name"};
  Gaudi::Property<std::string> m_jvtfileName{this, "JVTFileName", "JVTlikelihood_20140805.root", "JVT likelihood file name"};

  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer_key{this, "VertexContainer", "PrimaryVertices", "SG key for input vertex container"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_jvfCorrKey{this, "JVFCorrName", "JVFCorr", "SG key for input JVFCorr decoration"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_sumPtTrkKey{this, "SumPtTrkName", "SumPtTrkPt500", "SG key for input SumPtTrk decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_jvtKey{this, "JVTName", "Jvt", "SG key for output JVT decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_rptKey{this, "RpTName", "JvtRpt", "SG key for output RpT decoration"};

  // Internal objects
  TString m_fn;
  TH2F * m_jvthisto;

};


#endif

