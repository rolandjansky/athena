/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetVertexTaggerTool.h

#ifndef JETMOMENTTOOLS_JETVERTEXTAGGERTOOL_H
#define JETMOMENTTOOLS_JETVERTEXTAGGERTOOL_H

/// James Frost \n
/// November 2014
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

#include "JetInterface/IJetTrackSelector.h"
#include "JetRec/JetModifierBase.h"
#include "JetEDM/TrackVertexAssociation.h"
#include "JetInterface/IJetModifier.h"
#include "JetInterface/IJetUpdateJvt.h"

#include <vector>
#include <string>

#include <TFile.h>
#include <TString.h>
#include <TH1D.h>
#include <TH2D.h>

class JetVertexTaggerTool
: public asg::AsgTool,
  virtual public IJetModifier,
  virtual public IJetUpdateJvt {
  ASG_TOOL_CLASS2(JetVertexTaggerTool,IJetModifier,IJetUpdateJvt)

public:
  // Constructor from tool name
  JetVertexTaggerTool(const std::string& name);

  // Initialization.
  StatusCode initialize();

  // Inherited methods to modify a jet
  virtual int modify(xAOD::JetContainer& jetCont) const;

  // Finalization.
  StatusCode finalize();
 
  // Evaluate JVT from Rpt and JVFcorr.
  float evaluateJvt(float rpt, float jvfcorr) const;

  // Update JVT by scaling Rpt byt the ratio of the current and original jet pT values.
  //   jet - jet for which JVT is updated
  //   sjvt - name of the existing JVT moment (and prefix for RpT and JVFcorr).
  //   scale - name of the jet scale holding the original pT
  // The new value for JVT is returned.
  float updateJvt(const xAOD::Jet& jet) const;

  // Local method to return the HS vertex - that of type PriVtx
  const xAOD::Vertex* findHSVertex() const;

private:  // data

  // Configurable parameters
  std::string m_verticesName;
  std::string m_assocTracksName;
  std::string m_tvaName;
  std::string m_tracksName;
  std::string m_jvfCorrName;
  std::string m_sumPtTrkName;
  std::string m_jvtlikelihoodHistName;
  std::string m_jvtfileName;
  std::string m_jvtName;
  ToolHandle<IJetTrackSelector> m_htsel;
  TString m_fn;
  TFile * m_jvtfile;
  TH2F * m_jvthisto;

};


#endif

