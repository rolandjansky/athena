/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef V0TOOLS_H
#define V0TOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "EventPrimitives/EventPrimitives.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

/**
 *  @class V0Tools
 *  A simple tool to calculate a number of properties of a given V0 candidate.
 * 
 *  begin: 20-03-2007
 *  @authors Evelina Bouhova-Thacker (Lancaster University)
 *  e.bouhova@cern.ch
 */

namespace CLHEP{
    class HepLorentzVector;
}
 
namespace Trk
{
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
 
/**   
 * AlgTool interface methods 
 */
  static const InterfaceID& interfaceID() 
  {
   return IID_V0Tools;
  }

 
/**
 *  Methods, returning the invariant mass, error on the invariant mass and 
 *  Chi2 probability of the invariant mass of an xAOD::Vertex
 *  for a given hypothesis for the masses of the input tracks and the V0 mass.
 *
 *  if a negative value for a track is provided, the invariantMass and invariantMassError
 *  are returned excluding that track
 */
  double invariantMass(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMass(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const;
  double invariantMassError(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMassError(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const;
  double invariantMassProbability(const xAOD::Vertex * vxCandidate, double V0Mass, double posTrackMass, double negTrackMass) const;   
  double invariantMassProbability(const xAOD::Vertex * vxCandidate, double V0Mass, const std::vector<double> &masses) const;   
  double massProbability(double V0Mass, double mass, double massErr) const;

/**
 *  Methods, returning the refitted 3-momenta of the positive and negative tracks
 *  and the V0 (for more than one track with the same charge, the first one is returned)
 */ 
  static Amg::Vector3D trackMomentum(const xAOD::Vertex * vxCandidate, unsigned int trkIndex) ;
  static Amg::Vector3D positiveTrackMomentum(const xAOD::Vertex * vxCandidate) ;
  static Amg::Vector3D negativeTrackMomentum(const xAOD::Vertex * vxCandidate) ;
  static Amg::Vector3D V0Momentum(const xAOD::Vertex * vxCandidate) ;

/**
 *  Methods, returning the refitted 4-momenta of the positive and negative tracks
 *  and the V0 for a given hypothesis for the masses of the input tracks and the V0 mass.
 */
  static xAOD::TrackParticle::FourMom_t track4Momentum(const xAOD::Vertex * vxCandidate, unsigned int trkIndex, double trackMass) ;
  xAOD::TrackParticle::FourMom_t positiveTrack4Momentum(const xAOD::Vertex * vxCandidate, double posTrackMass) const;
  xAOD::TrackParticle::FourMom_t negativeTrack4Momentum(const xAOD::Vertex * vxCandidate, double negTrackMass) const;
  xAOD::TrackParticle::FourMom_t V04Momentum(const xAOD::Vertex * vxCandidate, double V0Mass) const;

/**
 *  NDoF of the vertex fit
 */
  static float ndof(const xAOD::Vertex * vxCandidate) ;

/**
 *  Chisq of the vertex fit
 */
  static float chisq(const xAOD::Vertex * vxCandidate) ;

/**
 *  Probability of the vertex fit
 */
  double vertexProbability(const xAOD::Vertex * vxCandidate) const;

/**
 *  vertex position
 */
  static Amg::Vector3D vtx(const xAOD::Vertex * vxCandidate) ;

/**
 *  Rxy of the vertex
 */
  static double rxy(const xAOD::Vertex * vxCandidate) ;

/**
 * wrt an xAOD::Vertex vertex
 */
  static double rxy(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) ;

/**
 * wrt an Amg::Vector3D vertex
 */
  static double rxy(const xAOD::Vertex * vxCandidate, const Amg::Vector3D& vertex) ;

/**
 * rxy_var
 */
  static double rxy_var(double dx, double dy, const Amg::MatrixX& cov) ;

/**
 * error on Rxy
 */
  double rxyError(const xAOD::Vertex * vxCandidate) const;

/**
 * wrt an xAOD::Vertex vertex
 */
  double rxyError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const;

/**
 * wrt an Amg::Vector3D vertex
 */
  double rxyError(const xAOD::Vertex * vxCandidate, const Amg::Vector3D& vertex) const;

/**
 * transverse momentum of the V0
 */
  double pT(const xAOD::Vertex * vxCandidate) const;

/**
 * error on the transverse momentum of the V0
 */
  double pTError(const xAOD::Vertex * vxCandidate) const;

/**
  * statistical separation (Mahalanobis distance)
  */
  double separation(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const;
  double separation(const xAOD::Vertex * vxCandidate, const Amg::Vector3D& vertex) const;

/**
  * closest distance in Z and the transverse plane of the momentum vector to an xAOD::Vertex
  * a0xy using transverse measurements only
  * a0z using the point of closest approach (3D)
  */
  double a0xy(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const;
  double a0z(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const;

/**
  * closest distance of the momentum vector to an xAOD::Vertex
  */
  double a0(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const;

/**
  * point of closest approach of the momentum vector to an xAOD::Vertex
  */
  Amg::Vector3D pca(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const;

/**
  * errors on a0xy and a0z
  */
  double a0xyError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const;
  double a0zError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const;

/**
  * error on a0
  */
  double a0Error(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, bool in3D=true) const;

/**
 * projection of distance in xy of the vertex wrt an xAOD::Vertex vertex along the momentum direction
 * (Px*dx+Py*dy)/pT
 */
  double lxy(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const;

/**
 * error on lxy
 */
  double lxyError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const;

/**
 * projection of distance in 3D of the vertex wrt an xAOD::Vertex vertex along the momentum direction
 * (Px*dx+Py*dy+Pz*dz)/p
 */
  double lxyz(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const;

/**
 * error on lxyz
 */
  double lxyzError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const;

/**
 * proper time wrt an xAOD::Vertex vertex assuming posTrackMass and negTrackMass
 * tau = CONST*M*lxy/pT
 */
  double tau(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double posTrackMass, double negTrackMass) const;

/**
 * proper time wrt an xAOD::Vertex vertex assuming track masses
 */
  double tau(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses) const;

/**
 * proper time wrt an xAOD::Vertex vertex assuming posTrackMass and negTrackMass (imposing massV0)
 * making a correction to the proper time consistent with the imposed V0 mass
 */
  double tau(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double posTrackMass, double negTrackMass, double massV0) const;

/**
 * proper time wrt an xAOD::Vertex vertex assuming track masses (imposing massV0)
 * making a correction to the proper time consistent with the imposed V0 mass
 */
  double tau(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses, double massV0) const;

/**
 * proper time wrt an xAOD::Vertex vertex assuming massV0
 * imposing a V0 mass without making an adjustment 
 */
  double tau(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double massV0) const;

/**
 * proper time error wrt an xAOD::Vertex vertex assuming posTrackMass and negTrackMass
 * tau = CONST*M*lxy/pT
 */
  double tauError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double posTrackMass, double negTrackMass) const;

/**
 * proper time error wrt an xAOD::Vertex vertex assuming track masses
 */
  double tauError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses) const;

/**
 * proper time error wrt an xAOD::Vertex vertex assuming posTrackMass and negTrackMass (imposing massV0) 
 * independent of massV0, variable included to match the corresponding proper time method
 */
  double tauError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double posTrackMass, double negTrackMass, double massV0) const;

/**
 * proper time error wrt an xAOD::Vertex vertex assuming track masses (imposing massV0)
 * independent of massV0, variable included to match the corresponding proper time method
 */
  double tauError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses, double massV0) const;

/**
 * proper time error wrt an xAOD::Vertex vertex assuming massV0
 * imposing a V0 mass without making an adjustment 
 */
  double tauError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double massV0) const;

/**
 * mass-proper time covariance
 */
  Amg::MatrixX tauMassCovariance(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses) const;
  double massTauCov(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double posTrackMass, double negTrackMass) const;
  double massTauCov(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses) const;

/**
 * proper time in 3D wrt an xAOD::Vertex vertex assuming track masses
 */
  double tau3D(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses) const;

/**
 * proper time in 3D wrt an xAOD::Vertex vertex assuming massV0
 * imposing a V0 mass without making an adjustment 
 */
  double tau3D(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double massV0) const;

/**
 * proper time error in 3D wrt an xAOD::Vertex vertex assuming track masses
 */
  double tau3DError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses) const;

/**
 * proper time error in 3D wrt an xAOD::Vertex vertex assuming massV0
 * imposing a V0 mass without making an adjustment 
 */
  double tau3DError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double massV0) const;

/**
 * Polarization angles in helicity frame (using positive track):
 *
 * thetaStar (in rad)
 */
  double thetaStar(const xAOD::Vertex * vxCandidate, double mass1, double mass2) const;

/**
 * cosThetaStar
 */
  double cosThetaStar(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  static double cosThetaStar(const CLHEP::HepLorentzVector & posTrack, const CLHEP::HepLorentzVector & negTrack) ;

/**
 * phiStar
 */
  //double phiStar(xAOD::Vertex * vxCandidate) const;
  double phiStar(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  static double phiStar(const CLHEP::HepLorentzVector & v0, const CLHEP::HepLorentzVector & track) ;

/**
 * cosTheta (pointing angle to an Amg::Vector3D or an xAOD::Vertex)
 */
  double cosTheta(const xAOD::Vertex * vxCandidate, const Amg::Vector3D& vertex) const;
  double cosTheta(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const;

/**
 * cosTheta (pointing angle to an Amg::Vector3D or an xAOD::Vertex in transverse plane)
 */
  double cosTheta_xy(const xAOD::Vertex * vxCandidate, const Amg::Vector3D& vertex) const;
  double cosTheta_xy(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const;

/**
 * sum of the charges of the tracks in the vertex
 */
  static float charge(const xAOD::Vertex * vxCandidate) ;

/**
 * pointers to original tracks
 */
  static const xAOD::TrackParticle* origTrack(const xAOD::Vertex * vxCandidate, int trkIndex) ;
  static const xAOD::TrackParticle* positiveOrigTrack(const xAOD::Vertex * vxCandidate) ;
  static const xAOD::TrackParticle* negativeOrigTrack(const xAOD::Vertex * vxCandidate) ;

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
 *
 *  if a negative value for a track is provided, the invariantMass and invariantMassError
 *  are returned excluding that track
 */
  double invariantMassBeforeFitIP(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMassBeforeFitIP(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const;
  double invariantMassBeforeFit(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMassBeforeFit(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const;
  double invariantMassBeforeFit(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass, const Amg::Vector3D& vertex) const;
  double invariantMassBeforeFit(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses, const Amg::Vector3D& vertex) const;
  double invariantMassErrorBeforeFitIP(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMassErrorBeforeFitIP(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const;
  double invariantMassErrorBeforeFit(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double invariantMassErrorBeforeFit(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const;
  double invariantMassErrorBeforeFit(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass, const Amg::Vector3D& vertex) const;
  double invariantMassErrorBeforeFit(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses, const Amg::Vector3D& vertex) const;

  static Amg::MatrixX convertCovMatrix(const xAOD::Vertex * vxCandidate) ;

/**
 * pointer from a mass constrained (Kshort, Lambda or Lambdabar) V0 to the unconstrained one
 */
  static const xAOD::Vertex* v0Link(const xAOD::Vertex * vxCandidate) ;

/**
 * pointers to Kshort, Lambda or Lambdabar mass constrained V0s, if they exist, from the unconstrained one
 */
  static const xAOD::Vertex* kshortLink(const xAOD::Vertex * vxCandidate) ;
  static const xAOD::Vertex* lambdaLink(const xAOD::Vertex * vxCandidate) ;
  static const xAOD::Vertex* lambdabarLink(const xAOD::Vertex * vxCandidate) ;
 
  static Amg::MatrixX makeV0Cov(const xAOD::Vertex * vxCandidate) ;
 
  private:

  double massErrorV0Fitter(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double massErrorV0Fitter(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const;
  double massErrorVKalVrt(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double massErrorVKalVrt(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const;
  double massErrorVxCandidate(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const;
  double massErrorVxCandidate(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const;

  ToolHandle < Trk::IExtrapolator > m_extrapolator;
 
 };//end of class definitions


}//end of namespace definitions

#endif
