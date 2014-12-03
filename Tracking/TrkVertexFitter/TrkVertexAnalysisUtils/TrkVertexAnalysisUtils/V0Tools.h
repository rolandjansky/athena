/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef V0TOOLS_H
#define V0TOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "EventKernel/PdtPdg.h"
#include "GaudiKernel/ToolHandle.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticleContainer.h"

/**
 *  @class V0Tools
 *  A simple tool to calculate a number of properties of a given V0 candidate.
 * 
 *  begin: 20-03-2007
 *  @authors Evelina Bouhova-Thacker (Lancaster University), Rob Henderson (Lancater University)
 *  e.bouhova@cern.ch, r.henderson@lancaster.ac.uk  
 *
 */
namespace Rec
{
 class TrackParticle;
}

namespace Trk
{
 class RecVertex;
 class Vertex;
 class VxCandidate;
 class ExtendedVxCandidate;
 class V0Candidate;
 class V0Hypothesis;
 class TrackParticleBase;
 class CovarianceMatrix;
 class IExtrapolator;

 static const InterfaceID IID_V0Tools("V0Tools", 1, 1);
 
 class V0Tools : public AthAlgTool
 {
  public:
 
/**
 * Default constructor due to Athena interface
 */
   V0Tools(const std::string& t, const std::string& n, const IInterface*  p);
 
/**
 * Virtual destructor
 */ 
   ~V0Tools();
 
/**
 * Standard AlgTool methods
 */ 
   StatusCode initialize();
   StatusCode finalize();
 
/**   
 * AlgTool interface methods 
 */
  static const InterfaceID& interfaceID() 
  {
   return IID_V0Tools;
  }

/**
 *  Methods, returning pointers to the different V0 Hypotheses
 */
  const Trk::V0Hypothesis* findHypothesis(const V0Candidate * v0Candidate, const int pdgID) const;
  
  const Trk::V0Hypothesis* v0Unconstr(const V0Candidate * v0Candidate) const;
  const Trk::V0Hypothesis* v0Kshort(const V0Candidate * v0Candidate) const;
  const Trk::V0Hypothesis* v0Lambda(const V0Candidate * v0Candidate) const;
  const Trk::V0Hypothesis* v0LambdaBar(const V0Candidate * v0Candidate) const;
  const Trk::V0Hypothesis* v0Gamma(const V0Candidate * v0Candidate) const;
 
 
/**
 *  Methods, returning the invariant mass, error on the invariant mass and 
 *  Chi2 probability of the invariant mass of an xAOD::Vertex
 *  for a given hypothesis for the masses of the input tracks and the V0 mass.
 */
  double invariantMass(xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMass(xAOD::Vertex * vxCandidate, std::vector<double> masses) const;
  double invariantMassError(xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMassError(xAOD::Vertex * vxCandidate, std::vector<double> masses) const;
  double invariantMassProbability(xAOD::Vertex * vxCandidate, double V0Mass, double posTrackMass, double negTrackMass) const;   
  double invariantMassProbability(xAOD::Vertex * vxCandidate, double V0Mass, std::vector<double> masses) const;   
  double massProbability(double V0Mass, double mass, double massErr) const;

/**
 *  Methods, returning the refitted 3-momenta of the positive and negative tracks
 *  and the V0 (for more than one track with the same charge, the first one is returned)
 */ 
  Amg::Vector3D trackMomentum(xAOD::Vertex * vxCandidate, unsigned int trkIndex) const;
  Amg::Vector3D positiveTrackMomentum(xAOD::Vertex * vxCandidate) const;
  Amg::Vector3D negativeTrackMomentum(xAOD::Vertex * vxCandidate) const;
  Amg::Vector3D V0Momentum(xAOD::Vertex * vxCandidate) const;

/**
 *  Methods, returning the refitted 4-momenta of the positive and negative tracks
 *  and the V0 for a given hypothesis for the masses of the input tracks and the V0 mass.
 */
  xAOD::TrackParticle::FourMom_t track4Momentum(xAOD::Vertex * vxCandidate, unsigned int trkIndex, double trackMass) const;
  xAOD::TrackParticle::FourMom_t positiveTrack4Momentum(xAOD::Vertex * vxCandidate, double posTrackMass) const;
  xAOD::TrackParticle::FourMom_t negativeTrack4Momentum(xAOD::Vertex * vxCandidate, double negTrackMass) const;
  xAOD::TrackParticle::FourMom_t V04Momentum(xAOD::Vertex * vxCandidate, double V0Mass) const;

/**
 *  NDoF of the vertex fit
 */
  float ndof(xAOD::Vertex * vxCandidate) const;

/**
 *  Chisq of the vertex fit
 */
  float chisq(xAOD::Vertex * vxCandidate) const;

/**
 *  Probability of the vertex fit
 */
  double vertexProbability(xAOD::Vertex * vxCandidate) const;

/**
 *  vertex position
 */
  Amg::Vector3D vtx(xAOD::Vertex * vxCandidate) const;

/**
 *  Rxy of the vertex
 */
  double rxy(xAOD::Vertex * vxCandidate) const;

/**
 * wrt an xAOD::Vertex vertex
 */
  double rxy(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex) const;

/**
 * wrt an Amg::Vector3D vertex
 */
  double rxy(xAOD::Vertex * vxCandidate, Amg::Vector3D& vertex) const;

/**
 * rxy_var
 */
  double rxy_var(double dx, double dy, const Amg::MatrixX& cov) const;

/**
 * error on Rxy
 */
  double rxyError(xAOD::Vertex * vxCandidate) const;

/**
 * wrt an xAOD::Vertex vertex
 */
  double rxyError(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex) const;

/**
 * wrt an Amg::Vector3D vertex
 */
  double rxyError(xAOD::Vertex * vxCandidate, Amg::Vector3D& vertex) const;

/**
 * transverse momentum of the V0
 */
  double pT(xAOD::Vertex * vxCandidate) const;

/**
 * error on the transverse momentum of the V0
 */
  double pTError(xAOD::Vertex * vxCandidate) const;

/**
  * statistical separation (Mahalanobis distance)
  */
  double separation(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex) const;
  double separation(xAOD::Vertex * vxCandidate, Amg::Vector3D& vertex) const;

/**
  * closest distance in Z and the transverse plane of the momentum vector to an xAOD::Vertex
  * a0xy using transverse measurements only
  * a0z using the point of closest approach (3D)
  */
  double a0xy(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex) const;
  double a0z(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex) const;

/**
  * closest distance of the momentum vector to an xAOD::Vertex
  */
  double a0(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex) const;

/**
  * point of closest approach of the momentum vector to an xAOD::Vertex
  */
  Amg::Vector3D pca(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex) const;

/**
  * errors on a0xy and a0z
  */
  double a0xyError(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex) const;
  double a0zError(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex) const;

/**
  * error on a0
  */
  double a0Error(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex, bool in3D=true) const;

/**
 * projection of distance in xy of the vertex wrt an xAOD::Vertex vertex along the momentum direction
 * (Px*dx+Py*dy)/pT
 */
  double lxy(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex) const;

/**
 * error on lxy
 */
  double lxyError(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex) const;

/**
 * proper time wrt an xAOD::Vertex vertex assuming posTrackMass and negTrackMass
 * tau = CONST*M*lxy/pT
 */
  double tau(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex, double posTrackMass, double negTrackMass) const;

/**
 * proper time wrt an xAOD::Vertex vertex assuming track masses
 */
  double tau(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex, std::vector<double> masses) const;

/**
 * proper time wrt an xAOD::Vertex vertex assuming posTrackMass and negTrackMass (imposing massV0)
 * making a correction to the proper time consistent with the imposed V0 mass
 */
  double tau(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex, double posTrackMass, double negTrackMass, double massV0) const;

/**
 * proper time wrt an xAOD::Vertex vertex assuming track masses (imposing massV0)
 * making a correction to the proper time consistent with the imposed V0 mass
 */
  double tau(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex, std::vector<double> masses, double massV0) const;

/**
 * proper time wrt an xAOD::Vertex vertex assuming massV0
 * imposing a V0 mass without making an adjustment 
 */
  double tau(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex, double massV0) const;

/**
 * proper time error wrt an xAOD::Vertex vertex assuming posTrackMass and negTrackMass
 * tau = CONST*M*lxy/pT
 */
  double tauError(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex, double posTrackMass, double negTrackMass) const;

/**
 * proper time error wrt an xAOD::Vertex vertex assuming track masses
 */
  double tauError(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex, std::vector<double> masses) const;

/**
 * proper time error wrt an xAOD::Vertex vertex assuming posTrackMass and negTrackMass (imposing massV0) 
 * independent of massV0, variable included to match the corresponding proper time method
 */
  double tauError(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex, double posTrackMass, double negTrackMass, double massV0) const;

/**
 * proper time error wrt an xAOD::Vertex vertex assuming track masses (imposing massV0)
 * independent of massV0, variable included to match the corresponding proper time method
 */
  double tauError(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex, std::vector<double> masses, double massV0) const;

/**
 * proper time error wrt an xAOD::Vertex vertex assuming massV0
 * imposing a V0 mass without making an adjustment 
 */
  double tauError(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex, double massV0) const;

/**
 * mass-proper time covariance
 */
  Amg::MatrixX tauMassCovariance(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex, std::vector<double> masses) const;
  double massTauCov(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex, double posTrackMass, double negTrackMass) const;
  double massTauCov(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex, std::vector<double> masses) const;

/**
 * Polarization angles in helicity frame (using positive track):
 *
 * thetaStar (in rad)
 */
  double thetaStar(xAOD::Vertex * vxCandidate, double mass1, double mass2) const;

/**
 * cosThetaStar
 */
  double cosThetaStar(xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double cosThetaStar(const CLHEP::HepLorentzVector & posTrack, const CLHEP::HepLorentzVector & negTrack) const;

/**
 * phiStar
 */
  //double phiStar(xAOD::Vertex * vxCandidate) const;
  double phiStar(xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double phiStar(const CLHEP::HepLorentzVector & v0, const CLHEP::HepLorentzVector & track) const;

/**
 * cosTheta (pointing angle to an Amg::Vector3D or an xAOD::Vertex)
 */
  double cosTheta(xAOD::Vertex * vxCandidate, Amg::Vector3D& vertex) const;
  double cosTheta(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex) const;

/**
 * cosTheta (pointing angle to an Amg::Vector3D or an xAOD::Vertex in transverse plane)
 */
  double cosTheta_xy(xAOD::Vertex * vxCandidate, Amg::Vector3D& vertex) const;
  double cosTheta_xy(xAOD::Vertex * vxCandidate, xAOD::Vertex* vertex) const;

/**
 * sum of the charges of the tracks in the vertex
 */
  float charge(xAOD::Vertex * vxCandidate) const;

/**
 * pointers to original tracks
 */
  const xAOD::TrackParticle* origTrack(xAOD::Vertex * vxCandidate, int trkIndex) const;
  const xAOD::TrackParticle* positiveOrigTrack(xAOD::Vertex * vxCandidate) const;
  const xAOD::TrackParticle* negativeOrigTrack(xAOD::Vertex * vxCandidate) const;

/**
 * create neutral TrackParticle from vxCandidate
 */
  // JRC: not possible with new EDM
  //Rec::TrackParticle* createParticle(const ExtendedVxCandidate * vxCandidate) const;
  //Rec::TrackParticle* createParticle(const Trk::V0Hypothesis * v0Hypothesis) const;

/**
 *  Methods, returning the invariant mass and the error on the invariant mass
 *  calculated from the original track parameters (at the perigee (IP) or at a given vertex position
 *  if no vertex is specified, the reconstructed vertex position is used
 */
  double invariantMassBeforeFitIP(xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMassBeforeFitIP(xAOD::Vertex * vxCandidate, std::vector<double> masses) const;
  double invariantMassBeforeFit(xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMassBeforeFit(xAOD::Vertex * vxCandidate, std::vector<double> masses) const;
  double invariantMassBeforeFit(xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass, Amg::Vector3D& vertex) const;
  double invariantMassBeforeFit(xAOD::Vertex * vxCandidate, std::vector<double> masses, Amg::Vector3D& vertex) const;
  double invariantMassErrorBeforeFitIP(xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMassErrorBeforeFitIP(xAOD::Vertex * vxCandidate, std::vector<double> masses) const;
  double invariantMassErrorBeforeFit(xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMassErrorBeforeFit(xAOD::Vertex * vxCandidate, std::vector<double> masses) const;
  double invariantMassErrorBeforeFit(xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass, Amg::Vector3D& vertex) const;
  double invariantMassErrorBeforeFit(xAOD::Vertex * vxCandidate, std::vector<double> masses, Amg::Vector3D& vertex) const;

  Amg::MatrixX * convertCovMatrix(xAOD::Vertex * vxCandidate) const;

/**
 * pointer from a mass constrained (Kshort, Lambda or Lambdabar) V0 to the unconstrained one
 */
  xAOD::Vertex* v0Link(xAOD::Vertex * vxCandidate) const;

/**
 * pointers to Kshort, Lambda or Lambdabar mass constrained V0s, if they exist, from the unconstrained one
 */
  xAOD::Vertex* kshortLink(xAOD::Vertex * vxCandidate) const;
  xAOD::Vertex* lambdaLink(xAOD::Vertex * vxCandidate) const;
  xAOD::Vertex* lambdabarLink(xAOD::Vertex * vxCandidate) const;
 
  private:

  double massErrorV0Fitter(xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double massErrorV0Fitter(xAOD::Vertex * vxCandidate, std::vector<double> masses) const;
  double massErrorVKalVrt(xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double massErrorVKalVrt(xAOD::Vertex * vxCandidate, std::vector<double> masses) const;
  double massErrorVxCandidate(xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double massErrorVxCandidate(xAOD::Vertex * vxCandidate, std::vector<double> masses) const;

  ToolHandle < Trk::IExtrapolator > m_extrapolator;
 
 };//end of class definitions

 inline const Trk::V0Hypothesis* V0Tools::v0Unconstr(const V0Candidate * v0Candidate) const
 {return findHypothesis(v0Candidate,PDG::null);}
 
 inline const Trk::V0Hypothesis* V0Tools::v0Kshort(const V0Candidate * v0Candidate) const
 {return findHypothesis(v0Candidate,PDG::K_S0);}
 
 inline const Trk::V0Hypothesis* V0Tools::v0Lambda(const V0Candidate * v0Candidate) const
 {return findHypothesis(v0Candidate,PDG::Lambda0);}
 
 inline const Trk::V0Hypothesis* V0Tools::v0LambdaBar(const V0Candidate * v0Candidate) const
 {return findHypothesis(v0Candidate, PDG::anti_Lambda0);}
 
 inline const Trk::V0Hypothesis* V0Tools::v0Gamma(const V0Candidate * v0Candidate) const
 {return findHypothesis(v0Candidate,PDG::gamma);}

}//end of namespace definitions

#endif
