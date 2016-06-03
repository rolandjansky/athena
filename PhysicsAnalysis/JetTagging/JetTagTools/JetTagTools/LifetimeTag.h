/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    @class LifetimeTag.h  
    This algtool calculates variables needed for lifetime tagging (i.e. btagging
    in the Rphi plane).
    It calculates the signed impact parameter d0 and its significance Sd0.

    In reference mode the significance is filled into a histogram.
    In analysis mode a ref histo is read in and a likelihood according to
    the significance of each track calculated.
    Created 10.03.04


    @author Andreas Wildauer (CERN PH-ATC), andreas.wildauer@cern.ch
    @author2 Fredrik Akesson (CERN PH-ATC), fredrik.akesson@cern.ch
 ***************************************************************************/

#ifndef JETTAGTOOLS_LIFETIMETAG_H
#define JETTAGTOOLS_LIFETIMETAG_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include <map>
#include "GeoPrimitives/GeoPrimitives.h"
#include "JetTagTools/ITagTool.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/TrackParticle.h"

//class Jet;

//namespace xAOD  { class TrackParticle; }
namespace Reco { class ITrackToVertex; }
namespace Trk
{
  class VxCandidate;
}

namespace Analysis
{

  class LifetimeInfo;
  class LikelihoodTool;
  class HistoHelperRoot;

  class LifetimeTag : public AthAlgTool , virtual public ITagTool
  {
    public:
      LifetimeTag(const std::string&,const std::string&,const IInterface*);

    /**
    Implementations of the methods defined in the abstract base class
     */
      virtual ~LifetimeTag();
      StatusCode initialize();
      StatusCode finalize();

      /** Set the primary vertex. TODO: This is temporary ! The primary vertex should
      be part of the Jet IParticle interface implementation. The trouble with
      ElementLink and persistency has to be solved for that. Revisit ... */
      void setOrigin(const Trk::VxCandidate* priVtx);

      /** The tagging method. TODO: The first argument will be removed as soon as we
       have the JetTag containing the necessary pointers to the constituent
       TrackParticles via navigation. But maybe this is not a very good idea since then
       a JetTag allways have to be created even if the tagging does not work. Think ...
       */
      void tagJet(xAOD::Jet& jetToTag);

      /** finalize the histos: Normalize other histos and fill the integratedNegativeIP histos.
       This cannot be done in the finalize() of this algtool because the histo service
       writes the histos BEFORE AlgTool::finalize() methods are called.
       However, it is closed AFTER Algorithm::finalize() methods are called ...
       In the calling Algorithm::finalize() method the following method will be called.
       */
      void finalizeHistos();

    private:
      /** This switch is needed to indicate what to do. The algorithm can be run to produce
      reference histograms from the given MC files (m_runModus=0) or to work in analysis mode
      (m_runModus=1) where already made reference histograms are read.*/
      std::string    m_runModus;          //!< 0=Do not read histos, 1=Read referece histos (analysis mode)

      /** Histogram Helper Class */
      HistoHelperRoot* m_histoHelperSig;
      HistoHelperRoot* m_histoHelperBkg;

      /** track to vertex tool */
      ToolHandle< Reco::ITrackToVertex > m_trackToVertexTool;

      /** Storage for the primary vertex. Can be removed when JetTag provides origin(). */
      // this pointer does not need to be deleted in the destructor (because it
      // points to something in storegate)
      const Trk::VxCandidate* m_priVtx;

      /** Likelihood Tool */
      ToolHandle< LikelihoodTool > m_likelihoodTool;                  //!< pointer to likelihood tool

      /** List of the variables to be used in the likelihood */
      std::vector<std::string> m_useVariables;

      /** specify the tag type (1D or 2D) */
      std::string m_lifetimeMode;

      /** Track selection cuts for LifetimeTagging */
      double m_minpt;           /// minimum pt[MeV] of the track
      double m_maxd0wrtPrimVtx; /// maximum d0[mm] with respect to primary vertex
      double m_maxz0wrtPrimVtx; /// maximum z0[mm] with respect to primary vertex

      int m_precisionHits; /// # of Pixel + SCT hits
      int m_PixelHits; /// # of Pixel hits
      int m_BLayerHits; /// # of BLayer hits
      
      /** booleans to know which variable was taken */
      std::map<std::string, bool> m_doVariable;

      /** Preselection routine for the tracks in the jet. */
      bool m_preselect(const xAOD::TrackParticle* track);

      /** Momentum of the jet. TODO: Uses only TrackParticles and ignores clusters. */
      Amg::Vector3D m_jetMomentum(const std::vector<const xAOD::TrackParticle*>& tracksInJet);

      /** just print some info at the beginning */
      void m_printParameterSettings();

      /** calculate the sign of the impact parameter */
      double m_calculateSignOfIP(const Trk::Perigee* perigee, Amg::Vector3D& unit);

  }; // End class
  inline void LifetimeTag::setOrigin(const Trk::VxCandidate* priVtx) { m_priVtx=priVtx; }
} // End namespace

#endif
