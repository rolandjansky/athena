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
 *  Chi2 probability of the invariant mass of a V0Hypothesis
 *  for a given hypothesis for the masses of the input tracks and the V0 mass.
 */
  double invariantMass(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMass(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const;
  double invariantMassError(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMassError(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const;
  double invariantMassProbability(const ExtendedVxCandidate * vxCandidate, double V0Mass, double posTrackMass, double negTrackMass) const;   
  double invariantMassProbability(const ExtendedVxCandidate * vxCandidate, double V0Mass, std::vector<double> masses) const;   
  double massProbability(double V0Mass, double mass, double massErr) const;

/**
 *  Methods, returning the refitted 3-momenta of the positive and negative tracks
 *  and the V0
 */ 
  Amg::Vector3D trackMomentum(const ExtendedVxCandidate * vxCandidate, int trkIndex) const;
  Amg::Vector3D positiveTrackMomentum(const ExtendedVxCandidate * vxCandidate) const;
  Amg::Vector3D negativeTrackMomentum(const ExtendedVxCandidate * vxCandidate) const;
  Amg::Vector3D V0Momentum(const ExtendedVxCandidate * vxCandidate) const;

/**
 *  Methods, returning the refitted 4-momenta of the positive and negative tracks
 *  and the V0 for a given hypothesis for the masses of the input tracks and the V0 mass.
 */
  CLHEP::HepLorentzVector track4Momentum(const ExtendedVxCandidate * vxCandidate, int trkIndex, double trackMass) const;
  CLHEP::HepLorentzVector positiveTrack4Momentum(const ExtendedVxCandidate * vxCandidate, double posTrackMass) const;
  CLHEP::HepLorentzVector negativeTrack4Momentum(const ExtendedVxCandidate * vxCandidate, double negTrackMass) const;
  CLHEP::HepLorentzVector V04Momentum(const ExtendedVxCandidate * vxCandidate, double V0Mass) const;

/**
 *  NDoF of the vertex fit
 */
  int ndof(const ExtendedVxCandidate * vxCandidate) const;

/**
 *  Chisq of the vertex fit
 */
  double chisq(const ExtendedVxCandidate * vxCandidate) const;

/**
 *  Probability of the vertex fit
 */
  double vertexProbability(const ExtendedVxCandidate * vxCandidate) const;

/**
 *  vertex position
 */
  Amg::Vector3D vtx(const ExtendedVxCandidate * vxCandidate) const;

/**
 *  Rxy of the vertex
 */
  double rxy(const ExtendedVxCandidate * vxCandidate) const;

/**
 * wrt a Trk::RecVertex vertex
 */
  double rxy(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const;

/**
 * wrt a Trk::Vertex vertex
 */
  double rxy(const ExtendedVxCandidate * vxCandidate, Trk::Vertex& vertex) const;

/**
 * wrt a Hep3Vector vertex
 */
  double rxy(const ExtendedVxCandidate * vxCandidate, Amg::Vector3D& vertex) const;

/**
 * rxy_var
 */
  double rxy_var(double dx, double dy, const Amg::MatrixX& cov) const;

/**
 * error on Rxy
 */
  double rxyError(const ExtendedVxCandidate * vxCandidate) const;

/**
 * wrt a Trk::RecVertex vertex
 */
  double rxyError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const;

/**
 * (wrt a Trk::Vertex vertex
 */
  double rxyError(const ExtendedVxCandidate * vxCandidate, Trk::Vertex& vertex) const;

/**
 * wrt a Hep3Vector vertex
 */
  double rxyError(const ExtendedVxCandidate * vxCandidate, Amg::Vector3D& vertex) const;

/**
 * transverse momentum of the V0
 */
  double pT(const ExtendedVxCandidate * vxCandidate) const;

/**
 * error on the transverse momentum of the V0
 */
  double pTError(const ExtendedVxCandidate * vxCandidate) const;

/**
  * statistical separation (Mahalanobis distance)
  */
  double separation(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const;
  double separation(const ExtendedVxCandidate * vxCandidate, Amg::Vector3D& vertex) const;

/**
  * closest distance in Z and the transverse plane of the momentum vector to a Trk::RecVertex
  * a0xy using transverse measurements only
  * a0z using the point of closest approach (3D)
  */
  double a0xy(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const;
  double a0z(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const;

/**
  * closest distance of the momentum vector to a Trk::RecVertex
  */
  double a0(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const;

/**
  * point of closest approach of the momentum vector to a Trk::RecVertex
  */
  Amg::Vector3D pca(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const;

/**
  * errors on a0xy and a0z
  */
  double a0xyError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const;
  double a0zError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const;

/**
  * error on a0
  */
  double a0Error(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, bool in3D=true) const;

/**
 * projection of distance in xy of the vertex wrt a Trk::RecVertex vertex along the momentum direction
 * (Px*dx+Py*dy)/pT
 */
  double lxy(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const;

/**
 * error on lxy
 */
  double lxyError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const;

/**
 * proper time wrt a Trk::RecVertex vertex assuming posTrackMass and negTrackMass
 * tau = CONST*M*lxy/pT
 */
  double tau(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, double posTrackMass, double negTrackMass) const;

/**
 * proper time wrt a Trk::RecVertex vertex assuming track masses
 */
  double tau(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, std::vector<double> masses) const;

/**
 * proper time wrt a Trk::RecVertex vertex assuming posTrackMass and negTrackMass (imposing massV0)
 * making a correction to the proper time consistent with the imposed V0 mass
 */
  double tau(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, double posTrackMass, double negTrackMass, double massV0) const;

/**
 * proper time wrt a Trk::RecVertex vertex assuming track masses (imposing massV0)
 * making a correction to the proper time consistent with the imposed V0 mass
 */
  double tau(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, std::vector<double> masses, double massV0) const;

/**
 * proper time wrt a Trk::RecVertex vertex assuming massV0
 * imposing a V0 mass without making an adjustment 
 */
  double tau(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, double massV0) const;

/**
 * proper time error wrt a Trk::RecVertex vertex assuming posTrackMass and negTrackMass
 * tau = CONST*M*lxy/pT
 */
  double tauError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, double posTrackMass, double negTrackMass) const;

/**
 * proper time error wrt a Trk::RecVertex vertex assuming track masses
 */
  double tauError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, std::vector<double> masses) const;

/**
 * proper time error wrt a Trk::RecVertex vertex assuming posTrackMass and negTrackMass (imposing massV0) 
 * independent of massV0, variable included to match the corresponding proper time method
 */
  double tauError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, double posTrackMass, double negTrackMass, double massV0) const;

/**
 * proper time error wrt a Trk::RecVertex vertex assuming track masses (imposing massV0)
 * independent of massV0, variable included to match the corresponding proper time method
 */
  double tauError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, std::vector<double> masses, double massV0) const;

/**
 * proper time error wrt a Trk::RecVertex vertex assuming massV0
 * imposing a V0 mass without making an adjustment 
 */
  double tauError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, double massV0) const;

/**
 * mass-proper time covariance
 */
  Amg::MatrixX tauMassCovariance(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, std::vector<double> masses) const;
  double massTauCov(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, double posTrackMass, double negTrackMass) const;
  double massTauCov(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, std::vector<double> masses) const;

/**
 * Polarization angles in helicity frame (assume positive track to be the first one in vxCandidate):
 *
 * thetaStar (in rad)
 */
  double thetaStar(const ExtendedVxCandidate * vxCandidate, double mass1, double mass2) const;

/**
 * cosThetaStar
 */
  double cosThetaStar(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const;
  double cosThetaStar(const CLHEP::HepLorentzVector & posTrack, const CLHEP::HepLorentzVector & negTrack) const;

/**
 * phiStar
 */
  double phiStar(const ExtendedVxCandidate * vxCandidate) const;
  double phiStar(const CLHEP::HepLorentzVector & v0, const CLHEP::HepLorentzVector & track) const;
  //double phiStar(const CLHEP::HepLorentzVector & posTrack, const CLHEP::HepLorentzVector & negTrack) const;

/**
 * cosTheta (pointing angle to a Hep3Vector or a RecVertex)
 */
  double cosTheta(const ExtendedVxCandidate * vxCandidate, Amg::Vector3D& vertex) const;
  double cosTheta(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const;

/**
 * cosTheta (pointing angle to a RecVertex in transverse plane)
 */
  double cosTheta_xy(const ExtendedVxCandidate * vxCandidate, Amg::Vector3D& vertex) const;
  double cosTheta_xy(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const;

/**
 * sum of the charges of the tracks in the vertex
 */
  int charge(const ExtendedVxCandidate * vxCandidate) const;

/**
 * pointers to original tracks
 */
  const Trk::TrackParticleBase* origTrackPB(const VxCandidate * vxCandidate, int trkIndex) const;
  const Trk::TrackParticleBase* origTrackPB(const ExtendedVxCandidate * vxCandidate, int trkIndex) const;
  const Trk::TrackParticleBase* positiveOrigTrackPB(const ExtendedVxCandidate * vxCandidate) const;
  const Trk::TrackParticleBase* negativeOrigTrackPB(const ExtendedVxCandidate * vxCandidate) const;

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
  double invariantMassBeforeFitIP(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMassBeforeFitIP(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const;
  //double invariantMassErrorBeforeFitIP(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const;
  //double invariantMassErrorBeforeFitIP(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const;
  double invariantMassBeforeFit(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMassBeforeFit(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const;
  double invariantMassBeforeFit(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass, Amg::Vector3D& vertex) const;
  double invariantMassBeforeFit(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses, Amg::Vector3D& vertex) const;
  double invariantMassErrorBeforeFit(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMassErrorBeforeFit(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const;
  double invariantMassErrorBeforeFit(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass, Amg::Vector3D& vertex) const;
  double invariantMassErrorBeforeFit(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses, Amg::Vector3D& vertex) const;

 
  private:

  double massErrorV0Fitter(const Trk::ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const;
  double massErrorV0Fitter(const Trk::ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const;
  double massErrorVKalVrt(const Trk::ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const;
  double massErrorVKalVrt(const Trk::ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const;
  double massErrorVxCandidate(const Trk::ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const;
  double massErrorVxCandidate(const Trk::ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const;

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
