// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackExample.h 626373 2014-11-05 17:13:41Z neldik $
#ifndef ANALYSISEXAMPLES_TRACKEXAMPLE_H
#define ANALYSISEXAMPLES_TRACKEXAMPLE_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Tool include(s):
#include "AnalysisTools/AnalysisTools.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"

// Forward declaration(s):
class TH1;

/**
 *  @short xAOD::TrackParticle handling example
 *
 *         This class contains a series of examples to show people how to:
 *            - access a TrackParticle container from the ESD or the AOD 
 *            - access track parameters from the TrackParticle
 *            - use the track isolation tools
 *
 * @author Ketevi A. Assamagan: February, 2005
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 626373 $
 * $Date: 2014-11-05 18:13:41 +0100 (Wed, 05 Nov 2014) $
 */
class TrackExample : public AthHistogramAlgorithm {

public:
   /// Regular Algorithm constructor
   TrackExample( const std::string& name, ISvcLocator* pSvcLocator );

   /// Function initialising the algorithm
   virtual StatusCode initialize();
   /// Function executing the algorithm
   virtual StatusCode execute();

private:
   /// get a handle to the tool helper
   ToolHandle< AnalysisTools > m_analysisTools;

   /// the TrackParticle container to retrieve
   std::string m_trackParticleContainerName;

   /// Event weight
   double m_eventWeight;

   /// TrackIsolationTool
   ToolHandle< xAOD::ITrackIsolationTool > m_isolationTool;

   /// @name Histograms of reconstructed quantities
   /// @{
   TH1* m_histNumberOfTrackParticles;
   TH1* m_histQoverP;
   TH1* m_histP;
   TH1* m_histQ;
   TH1* m_histEta;
   TH1* m_histPhi;
   TH1* m_histIsolationP;
   TH1* m_histOriginType;

   TH1* m_histVertexX;
   TH1* m_histVertexY;
   TH1* m_histVertexZ;
   TH1* m_histd0wrtPrimVtx;
   TH1* m_histz0wrtPrimVtx;
   /// @}

   /// the cuts - to adjusted in the job options

   /// cut Pt cut
   double m_trackPCut;

   /// track isolation cone
   int m_isolationCone;

}; // class TrackExample

#endif // ANALYSISEXAMPLES_TRACKEXAMPLE_H
