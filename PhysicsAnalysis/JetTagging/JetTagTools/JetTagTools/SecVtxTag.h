/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    @class SecVtxTag
                             -------------------
    Created   : 10.03.04
    author Andreas Wildauer (CERN PH-ATC), andreas.wildauer@cern.ch
    @author2 Fredrik Akesson (CERN PH-ATC), fredrik.akesson@cern.ch
 ***************************************************************************/
#ifndef JETTAGTOOLS_SECVTXTAG_H
#define JETTAGTOOLS_SECVTXTAG_H

#include <vector>
#include <map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTools/ITagTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "xAODTracking/TrackParticle.h"

//namespace xAOD { class TrackParticle; }
namespace Reco { class ITrackToVertex; }

namespace Trk {
  class IVertexFitter;
  class VxCandidate;
  class Track;
}
//class Jet;

namespace Analysis
{
  class SecVtxInfo;
  class LikelihoodTool;
  class HistoHelperRoot;
  class SVForIPTool;

  typedef std::vector<double> FloatVec;
  typedef std::vector<double>::iterator FloatVecIter;
  typedef std::vector<const xAOD::TrackParticle*> TrackVec2;
  typedef std::vector<const Trk::Track*> TrkTrackVec;
  typedef std::vector<const xAOD::TrackParticle*>::const_iterator TrackIter;
  typedef std::multimap<double, const xAOD::TrackParticle*> TrkMM;
  typedef std::multimap<double, const xAOD::TrackParticle*>::iterator TrkMMIter;

  class SecVtxTag : public AthAlgTool , virtual public ITagTool
  {
    public:
      SecVtxTag(const std::string&,const std::string&,const IInterface*);

    /**
    Implementations of the methods defined in the abstract base class
     */
      virtual ~SecVtxTag();
      StatusCode initialize();
      StatusCode finalize();

      /** Set the primary vertex. TODO: This is temporary ! The primary vertex should
      be part of the JetTag IParticle interface implementation. The trouble with
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
      reference histograms from the given MC files (m_runModus="analysis") or to work in analysis mode
      (m_runModus="reference") where already made reference histograms are read.*/
      std::string m_runModus;          //!< 0=Do not read histos, 1=Read referece histos (analysis mode)

      /** Histogram Helper Class */
      HistoHelperRoot* m_histoHelperSig;
      HistoHelperRoot* m_histoHelperBkg;

      /** Storage for the primary vertex. Can be removed when JetTag provides origin(). */
      const Trk::VxCandidate* m_priVtx;

      /** pointer to the fittool interface class */
      ToolHandle< Trk::IVertexFitter > m_fitTool;

      /** track to vertex tool */
      ToolHandle< Reco::ITrackToVertex > m_trackToVertexTool;

      /** likelihood tool */
      ToolHandle< LikelihoodTool > m_likelihoodTool;

      bool m_useVKalTool; //!< flag to be able and use vadims tool
      
      /** List of the variables to be used in the likelihood */
      std::vector<std::string> m_useVariables;

      std::string m_vertexFindingMode;

      /** Track selection cuts for Sec Vtx Tagging */
      double m_minpt;             /// minimum pt[MeV] of the track
      double m_maxd0wrtPrimVtx;   /// maximum d0[mm] with respect to primary vertex
      double m_maxz0wrtPrimVtx;   /// maximum z0[mm] with respect to primary vertex
      double m_min2DSigToPrimVtx; /// minimum a0/siga0 wrt. to primary vertex

      int m_precisionHits; /// # of Pixel + SCT hits
      int m_PixelHits; /// # of Pixel hits
      int m_BLayerHits; /// # of BLayer hits

      /** Selection cuts in the vertex fit */
      double m_buMaxChi2OfTrack; /// maximum chi2 of a track to be added to a sec vtx
      double m_minDistToPrimVtx; /// minimum distance a sec vtx must have from the primary vertex

      /** Preselection routine for the tracks in the jet. */
      bool m_preselect(const xAOD::TrackParticle* track);

      /** Momentum of the jet. TODO: Uses only TrackParticles and ignores clusters. */
      Amg::Vector3D m_jetMomentum(const TrackVec2& tracksInJet);

      const Trk::VxCandidate* m_fitBuildUpVertex(TrackVec2 tracksInJet, TrackVec2& tracksInSV);
      const Trk::VxCandidate* m_fitTearDownVertex(TrackVec2 tracksInJet, TrackVec2& tracksInSV);
      double m_massOfVertex(const TrackVec2& TPInVtx);
      void m_sortTracksInChi2(std::vector<int> &indexOfSortedChi2, const Trk::VxCandidate* myVxCandidate);
      void m_printParameterSettings();
      void m_combinedLikelihoodForThisTag(const FloatVec& sLike, const FloatVec& bLike,
                                          SecVtxInfo* secVtxInfo);


      //name of the VKalVrt secondary vertex in Jet instance
      //(needed to retrieve the ISvxConstituent written 
      //out by the finder and attached to the ParticleJet)
      std::string m_secVxFinderName;


  }; // End class
  inline void SecVtxTag::setOrigin(const Trk::VxCandidate* priVtx) { m_priVtx=priVtx; }
} // End namespace

#endif
