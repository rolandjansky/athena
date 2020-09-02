/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETCONVERSIONFINDERTOOLS_CONVERSIONPOSTSELECTOR_H
#define INDETCONVERSIONFINDERTOOLS_CONVERSIONPOSTSELECTOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h" //ParticleMasses struct
#include "TrkParameters/TrackParameters.h" //typedef
#include "xAODTracking/VertexFwd.h"
#include "GeoPrimitives/GeoPrimitives.h" //Amg::Vector3D typedef
#include <vector>



namespace CLHEP{
  class HepLorentzVector;
}

namespace InDet {

  /**
     @class ConversionPostSelector
     This class selects tracks for conversion finder
     @author Tatjana Lenz , Thomas Koffas
  */

  class ConversionPostSelector : public AthAlgTool {

  public:
    ConversionPostSelector (const std::string& type,const std::string& name, const IInterface* parent);
    virtual ~ConversionPostSelector();

    static const InterfaceID& interfaceID();
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    /** Conversion candidate post-fit selectors. Return true if the argument
     * track fulfills the selection*/
    bool selectConversionCandidate(xAOD::Vertex* myCandidate,
                                   int flag,
                                   std::vector<Amg::Vector3D>& trkL) const;
    bool selectSecVtxCandidate(xAOD::Vertex* myCandidate,
                               int flag,
                               std::vector<Amg::Vector3D>& trkL,
                               int&) const;

    /** Decorate vertices with values used in post selector **/
    void decorateVertex(xAOD::Vertex& vertex,
                        float inv_mass,
                        float pt1,
                        float pt2,
                        float fR,
                        float deltaPhiVtxTrk) const;

  private:
    /** Properties for track selection:
	all cuts are ANDed */
    std::vector<double> m_maxChi2;     /** Chi2 cut.  */
    std::vector<double> m_invMassCut;  /** Invariant mass cut.  */
    std::vector<double> m_fitMomentum; /** Converted photon reconstructed momentum at vertex cut.  */
    std::vector<double> m_minRadius;   /** Converted photon reconstructed vertex radial position cut.  */
    double              m_minPt;       /** Pt of the two participating tracks at the vertex. */
    double              m_maxdR;       /** Distance of first track hit- reconstructed vertex radial position.  */
    double              m_maxPhiVtxTrk;/** Maximum difference in phi between reconstructed vertex and track at vertex. */
    bool                m_decorateVertices;/** Decorate vertices with values used for vertex selection */


    /** Masses and mass ranges for different V0 hypotheses */
    double m_massK0      ;
    double m_sigmaK0     ;
    double m_massLambda  ;
    double m_sigmaLambda ;
    int    m_nsig        ;

    /** struct of Particle Masses */
    static const Trk::ParticleMasses s_particleMasses;

    /** Compute the four-momentum of a particle according to a mass hypothesis.  */
    CLHEP::HepLorentzVector fourP(const Trk::TrackParameters&, const Trk::TrackParameters&, double, bool) const;
  };

}
#endif // INDETCONVERSIONFINDERTOOLS_CONVERSIONPOSTSELECTOR_H

