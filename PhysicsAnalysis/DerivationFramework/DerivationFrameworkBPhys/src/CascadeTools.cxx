/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
/////////////////////////////////////////////////////////////////
// CascadeTools.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
#include "DerivationFrameworkBPhys/CascadeTools.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"


namespace DerivationFramework {


CascadeTools::CascadeTools(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p)
{
  declareInterface<CascadeTools>(this);
}

CascadeTools::~CascadeTools() {}

//Light speed constant for various calculations
constexpr double s_CONST = 1000./299.792;

double CascadeTools::invariantMass(const std::vector<TLorentzVector> &particleMom) const
{
  if(particleMom.size() == 0) return -999999.;
  TLorentzVector totalMom;
  unsigned int NTrk = particleMom.size();
  for( unsigned int it=0; it<NTrk; it++) totalMom += particleMom[it];
  return totalMom.M();
}

double CascadeTools::invariantMass(const std::vector<TLorentzVector> &particleMom2, const std::vector<double> &masses) const
{
  if(particleMom2.size() == 0) return -999999.;
  TLorentzVector totalMom;
  unsigned int NTrk = particleMom2.size();
  if (masses.size() != NTrk) {
    ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
    return -999999.;
  }
  TLorentzVector temp;
  for( unsigned int it=0; it<NTrk; it++) {
    double esq = particleMom2[it].Px()*particleMom2[it].Px() + particleMom2[it].Py()*particleMom2[it].Py() +
                 particleMom2[it].Pz()*particleMom2[it].Pz() + masses[it]*masses[it];
    double e = (esq>0.) ? sqrt(esq) : 0.;

    temp.SetPxPyPzE(particleMom2[it].Px(),particleMom2[it].Py(),particleMom2[it].Pz(),e);
    totalMom += temp;
  }
  return totalMom.M();
}

double CascadeTools::invariantMassError(const std::vector<TLorentzVector> &particleMom, const Amg::MatrixX& cov) const
{
  if(particleMom.size() == 0) return -999999.;
  unsigned int NTrk = particleMom.size();
  TLorentzVector totalMom;
  for( unsigned int it=0; it<NTrk; it++) totalMom += particleMom[it];

  Amg::MatrixX D_vec(3*NTrk+3,1); D_vec.setZero();
  for( unsigned int it=0; it<NTrk; it++) {
    D_vec(3*it+3) = 2.*(totalMom.E()*particleMom[it].Px()/particleMom[it].E()-totalMom.Px());
    D_vec(3*it+4) = 2.*(totalMom.E()*particleMom[it].Py()/particleMom[it].E()-totalMom.Py());
    D_vec(3*it+5) = 2.*(totalMom.E()*particleMom[it].Pz()/particleMom[it].E()-totalMom.Pz());
  }
  Amg::MatrixX merr = D_vec.transpose() * cov * D_vec;
  double massVarsq = merr(0,0);
  if (massVarsq <= 0.) ATH_MSG_DEBUG("massError: negative sqrt massVarsq " << massVarsq);
  double massVar = (massVarsq>0.) ? sqrt(massVarsq) : 0.;
  double massErr = massVar/(2.*totalMom.M());
  return massErr;
}

double CascadeTools::invariantMassError(const std::vector<TLorentzVector> &particleMom2, const Amg::MatrixX& cov, const std::vector<double> &masses) const
{
  if(particleMom2.size() == 0) return -999999.;
  unsigned int NTrk = particleMom2.size();
  if (masses.size() != NTrk) {
    ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
    return -999999.;
  }
  std::vector<TLorentzVector> particleMom(NTrk); particleMom.clear();
  for( unsigned int it=0; it<NTrk; it++) {
    double esq = particleMom2[it].Px()*particleMom2[it].Px() + particleMom2[it].Py()*particleMom2[it].Py() +
                 particleMom2[it].Pz()*particleMom2[it].Pz() + masses[it]*masses[it];
    double e = (esq>0.) ? sqrt(esq) : 0.;
    particleMom[it].SetPxPyPzE(particleMom2[it].Px(),particleMom2[it].Py(),particleMom2[it].Pz(),e);
  }
  TLorentzVector totalMom;
  for( unsigned int it=0; it<NTrk; it++) totalMom += particleMom[it];

  std::vector<double>dm2dpx(NTrk), dm2dpy(NTrk), dm2dpz(NTrk);
  for( unsigned int it=0; it<NTrk; it++) {
    dm2dpx[it] = 2.*(totalMom.E()*particleMom[it].Px()/particleMom[it].E()-totalMom.Px());
    dm2dpy[it] = 2.*(totalMom.E()*particleMom[it].Py()/particleMom[it].E()-totalMom.Py());
    dm2dpz[it] = 2.*(totalMom.E()*particleMom[it].Pz()/particleMom[it].E()-totalMom.Pz());
  }
  Amg::MatrixX D_vec(3*NTrk+3,1); D_vec.setZero();
  for( unsigned int it=0; it<NTrk; it++) {
    D_vec(3*it+3) = dm2dpx[it];
    D_vec(3*it+4) = dm2dpy[it];
    D_vec(3*it+5) = dm2dpz[it];
  }
  Amg::MatrixX merr = D_vec.transpose() * cov * D_vec;
  double massVarsq = merr(0,0);
  if (massVarsq <= 0.) ATH_MSG_DEBUG("massError: negative sqrt massVarsq " << massVarsq);
  double massVar = (massVarsq>0.) ? sqrt(massVarsq) : 0.;
  double massErr = massVar/(2.*totalMom.M());
  return massErr;
}

double CascadeTools::pT(const std::vector<TLorentzVector> &particleMom) const
{
  if(particleMom.size() == 0) return -999999.;
  Amg::Vector3D P = momentum(particleMom);;
  return P.perp();
}

double CascadeTools::pTError(const std::vector<TLorentzVector> &particleMom, const Amg::MatrixX& cov) const
{
  if(particleMom.size() == 0) return -999999.;
  Amg::Vector3D P = momentum(particleMom);;
  double Px = P.x();
  double Py = P.y();
  double PT = P.perp();

  unsigned int NTrk = particleMom.size();
  Amg::MatrixX D_vec(3*NTrk+3,1); D_vec.setZero();
  for( unsigned int it=0; it<NTrk; it++) {
    D_vec(3*it+3) = Px/PT;
    D_vec(3*it+4) = Py/PT;
    D_vec(3*it+5) = 0.;
  }
  Amg::MatrixX PtErrSq = D_vec.transpose() * cov * D_vec;
  double PtErrsq = PtErrSq(0,0);
  if (PtErrsq <= 0.) ATH_MSG_DEBUG("ptError: negative sqrt PtErrsq " << PtErrsq);
  return (PtErrsq>0.) ? sqrt(PtErrsq) : 0.;
}

double CascadeTools::lxy(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const
{
  if(particleMom.size() == 0) return -999999.;
  auto vert = SV->position() - PV->position();
  double dx = vert.x();
  double dy = vert.y();
  Amg::Vector3D P = momentum(particleMom);;
  double dxy = (P.x()*dx + P.y()*dy)/P.perp();
  return dxy;
}

double CascadeTools::lxyError(const std::vector<TLorentzVector> &particleMom, const Amg::MatrixX& cov, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const
{
  if(particleMom.size() == 0) return -999999.;
  auto vert = SV->position() - PV->position();
  double dx = vert.x();
  double dy = vert.y();
  Amg::Vector3D P = momentum(particleMom);;
  double Px = P.x();
  double Py = P.y();
  double PT = P.perp();
  double LXYoverPT = (Px*dx+Py*dy)/(PT*PT);

  unsigned int NTrk = particleMom.size();

  double dLxydx = Px/PT;
  double dLxydy = Py/PT;
  double dLxydx0 = -dLxydx;
  double dLxydy0 = -dLxydy;

  Amg::MatrixX D_vec(3*NTrk+6,1); D_vec.setZero();
  D_vec(0) = dLxydx;
  D_vec(1) = dLxydy;
  D_vec(2) = 0.;
  for( unsigned int it=0; it<NTrk; it++) {
    D_vec(3*it+3) = (dx - LXYoverPT*Px)/PT;
    D_vec(3*it+4) = (dy - LXYoverPT*Py)/PT;
    D_vec(3*it+5) = 0.;
  }
  D_vec(3*NTrk+3) = dLxydx0;
  D_vec(3*NTrk+4) = dLxydy0;
  D_vec(3*NTrk+5) = 0.;

  unsigned int ndim = 3*NTrk+3;
  Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
  W_mat.block(0,0,ndim,ndim) = cov;
  W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = PV->covariancePosition();
  Amg::MatrixX V_err = D_vec.transpose() * W_mat * D_vec;

  double LxyErrsq = V_err(0,0);
  if (LxyErrsq <= 0.) ATH_MSG_DEBUG("lxyError: negative sqrt LxyErrsq " << LxyErrsq);
  return (LxyErrsq>0.) ? sqrt(LxyErrsq) : 0.;
}

double CascadeTools::tau(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const
{
  if(particleMom.size() == 0) return -999999.;
  double M = invariantMass(particleMom);
  double LXY = lxy(particleMom,SV,PV);
  double PT = pT(particleMom);
  return s_CONST*M*LXY/PT;
}

double CascadeTools::tauError(const std::vector<TLorentzVector> &particleMom, const Amg::MatrixX& cov, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const
{
  if(particleMom.size() == 0) return -999999.;
  double M = invariantMass(particleMom);
  auto vert = SV->position() - PV->position();
  double dx = vert.x();
  double dy = vert.y();
  Amg::Vector3D P = momentum(particleMom);;
  double Px = P.x();
  double Py = P.y();
  double PT = P.perp();
  double LXY = Px*dx+Py*dy;

  unsigned int NTrk = particleMom.size();
  TLorentzVector totalMom;
  for( unsigned int it=0; it<NTrk; it++) totalMom += particleMom[it];

  double dTaudx = (M*Px)/(PT*PT);
  double dTaudy = (M*Py)/(PT*PT);
  double dTaudx0 = -dTaudx;
  double dTaudy0 = -dTaudy;

  Amg::MatrixX D_vec(3*NTrk+6,1); D_vec.setZero();
  D_vec(0) = dTaudx;
  D_vec(1) = dTaudy;
  D_vec(2) = 0.;
  for( unsigned int it=0; it<NTrk; it++) {
    D_vec(3*it+3) = (((totalMom.E()*particleMom[it].Px()*LXY)/(M*particleMom[it].E()))-Px*LXY/M+M*dx)/(PT*PT) - 2.*M*LXY*Px/(PT*PT*PT*PT);
    D_vec(3*it+4) = (((totalMom.E()*particleMom[it].Py()*LXY)/(M*particleMom[it].E()))-Py*LXY/M+M*dy)/(PT*PT) - 2.*M*LXY*Py/(PT*PT*PT*PT);
    D_vec(3*it+5) = 0.;
  }
  D_vec(3*NTrk+3) = dTaudx0;
  D_vec(3*NTrk+4) = dTaudy0;
  D_vec(3*NTrk+5) = 0.;

  unsigned int ndim = 3*NTrk+3;
  Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
  W_mat.block(0,0,ndim,ndim) = cov;
  W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = PV->covariancePosition();
  Amg::MatrixX V_err = D_vec.transpose() * W_mat * D_vec;

  double tauErrsq = V_err(0,0);
  if (tauErrsq <= 0.) ATH_MSG_DEBUG("tauError: negative sqrt tauErrsq " << tauErrsq);
  double tauErr = (tauErrsq>0.) ? sqrt(tauErrsq) : 0.;
  return s_CONST*tauErr;
}

double CascadeTools::tau(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV, double M) const
{
  if(particleMom.size() == 0) return -999999.;
  double LXY = lxy(particleMom,SV,PV);
  double PT = pT(particleMom);
  return s_CONST*M*LXY/PT;
}

double CascadeTools::tauError(const std::vector<TLorentzVector> &particleMom, const Amg::MatrixX& cov, const xAOD::Vertex* SV, const xAOD::Vertex* PV, double M) const
{
  if(particleMom.size() == 0) return -999999.;
  auto vert = SV->position() - PV->position();
  double dx = vert.x();
  double dy = vert.y();
  Amg::Vector3D P = momentum(particleMom);;
  double Px = P.x();
  double Py = P.y();
  double PT = P.perp();
  double LXY = Px*dx+Py*dy;

  unsigned int NTrk = particleMom.size();
  TLorentzVector totalMom;
  for( unsigned int it=0; it<NTrk; it++) totalMom += particleMom[it];

  double dTaudx = (M*Px)/(PT*PT);
  double dTaudy = (M*Py)/(PT*PT);
  double dTaudx0 = -dTaudx;
  double dTaudy0 = -dTaudy;

  Amg::MatrixX D_vec(3*NTrk+6,1); D_vec.setZero();
  D_vec(0) = dTaudx;
  D_vec(1) = dTaudy;
  D_vec(2) = 0.;
  for( unsigned int it=0; it<NTrk; it++) {
    D_vec(3*it+3) = (M*dx)/(PT*PT) - 2.*M*LXY*Px/(PT*PT*PT*PT);
    D_vec(3*it+4) = (M*dy)/(PT*PT) - 2.*M*LXY*Py/(PT*PT*PT*PT);
    D_vec(3*it+5) = 0.;
  }
  D_vec(3*NTrk+3) = dTaudx0;
  D_vec(3*NTrk+4) = dTaudy0;
  D_vec(3*NTrk+5) = 0.;

  unsigned int ndim = 3*NTrk+3;
  Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
  W_mat.block(0,0,ndim,ndim) = cov;
  W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = PV->covariancePosition();
  Amg::MatrixX V_err = D_vec.transpose() * W_mat * D_vec;

  double tauErrsq = V_err(0,0);
  if (tauErrsq <= 0.) ATH_MSG_DEBUG("tauError: negative sqrt tauErrsq " << tauErrsq);
  double tauErr = (tauErrsq>0.) ? sqrt(tauErrsq) : 0.;
  return s_CONST*tauErr;
}

Amg::Vector3D CascadeTools::pca(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const
{
  if(particleMom.size() == 0) {
    Amg::Vector3D p; p.setZero();
    return p;
  }
  Amg::Vector3D pv = PV->position();
  Amg::Vector3D sv = SV->position();
  Amg::Vector3D P = momentum(particleMom);;
  double p2 = P.mag2();
  double pdr = P.dot((sv - pv));
  return sv - P*pdr/p2;
}

double CascadeTools::cosTheta(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const
{
  if(particleMom.size() == 0) return -999999.;
  Amg::Vector3D P = momentum(particleMom);;
  Amg::Vector3D vtx = SV->position();
  vtx -= PV->position();
  return (P.dot(vtx))/(P.mag()*vtx.mag());
}

double CascadeTools::cosTheta_xy(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const
{
  if(particleMom.size() == 0) return -999999.;
  Amg::Vector3D P = momentum(particleMom);;
  Amg::Vector3D vtx = SV->position();
  vtx -= PV->position();
  double pT = P.perp();
  return (P.x()*vtx.x()+P.y()*vtx.y())/(pT*vtx.perp());
}

double CascadeTools::a0z(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const
{
  if(particleMom.size() == 0) return -999999.;
  Amg::Vector3D pv = PV->position();
  Amg::Vector3D ca_point = pca(particleMom,SV,PV);
  Amg::Vector3D a0_vec = pv - ca_point;
  return a0_vec.z();
}

double CascadeTools::a0zError(const std::vector<TLorentzVector> &particleMom, const Amg::MatrixX& cov, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const
{
  if(particleMom.size() == 0) return -999999.;
  auto vert = SV->position() - PV->position();
  double dx = vert.x();
  double dy = vert.y();
  double dz = vert.z();
  Amg::Vector3D P = momentum(particleMom);;
  double Px = P.x();
  double Py = P.y();
  double Pz = P.z();
  double P2 = P.mag2();
  double L = Px*dx+Py*dy+Pz*dz;

  unsigned int NTrk = particleMom.size();


  double da0zdx = (Px*Pz)/P2;
  double da0zdy = (Py*Pz)/P2;
  double da0zdz = (Pz*Pz)/P2 - 1.;
  double da0zdx0 = -da0zdx;
  double da0zdy0 = -da0zdy;
  double da0zdz0 = -da0zdz;

  Amg::MatrixX D_vec(3*NTrk+6,1); D_vec.setZero();
  D_vec(0) = da0zdx;
  D_vec(1) = da0zdy;
  D_vec(2) = da0zdz;
  for( unsigned int it=0; it<NTrk; it++) {
    D_vec(3*it+3) = (Pz*(P2*dx-2.*L*Px))/(P2*P2);
    D_vec(3*it+4) = (Pz*(P2*dy-2.*L*Py))/(P2*P2);
    D_vec(3*it+5) = (Pz*(P2*dz-2.*L*Pz))/(P2*P2)+L/P2;
  }
  D_vec(3*NTrk+3) = da0zdx0;
  D_vec(3*NTrk+4) = da0zdy0;
  D_vec(3*NTrk+5) = da0zdz0;

  unsigned int ndim = 3*NTrk+3;
  Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
  W_mat.block(0,0,ndim,ndim) = cov;
  W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = PV->covariancePosition();
  Amg::MatrixX V_err = D_vec.transpose() * W_mat * D_vec;

  double a0zErrsq = V_err(0,0);
  if (a0zErrsq <= 0.) ATH_MSG_DEBUG("a0zError: negative sqrt a0zErrsq " << a0zErrsq);
  return (a0zErrsq>0.) ? sqrt(a0zErrsq) : 0.;
}

double CascadeTools::a0xy(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const
{
  if(particleMom.size() == 0) return -999999.;
  double cosineTheta_xy = cosTheta_xy(particleMom,SV,PV);
  double sinTheta_xy = ((1.-cosineTheta_xy*cosineTheta_xy)>0.) ? sqrt((1.-cosineTheta_xy*cosineTheta_xy)) : 0.;
  return (SV->position()-PV->position()).perp() * sinTheta_xy;
}

double CascadeTools::a0xyError(const std::vector<TLorentzVector> &particleMom, const Amg::MatrixX& cov, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const
{
  if(particleMom.size() == 0) return -999999.;
  auto vert = SV->position() - PV->position();
  double dx = vert.x();
  double dy = vert.y();
  Amg::Vector3D P = momentum(particleMom);;
  double Px = P.x();
  double Py = P.y();
  double P2 = P.perp()*P.perp();
  double L = Px*dx+Py*dy;
  double dR2 = vert.perp()*vert.perp();
  double d = sqrt((P2*dR2-L*L)/P2);

  unsigned int NTrk = particleMom.size();

  double da0dx = (P2*dx-L*Px)/(P2*d);
  double da0dy = (P2*dy-L*Py)/(P2*d);
  double da0dx0 = -da0dx;
  double da0dy0 = -da0dy;

  Amg::MatrixX D_vec(3*NTrk+6,1); D_vec.setZero();
  D_vec(0) = da0dx;
  D_vec(1) = da0dy;
  D_vec(2) = 0.;
  for( unsigned int it=0; it<NTrk; it++) {
    D_vec(3*it+3) = (L*(L*Px-P2*dx))/(P2*P2*d);
    D_vec(3*it+4) = (L*(L*Py-P2*dy))/(P2*P2*d);
    D_vec(3*it+5) = 0.;
  }
  D_vec(3*NTrk+3) = da0dx0;
  D_vec(3*NTrk+4) = da0dy0;
  D_vec(3*NTrk+5) = 0.;

  unsigned int ndim = 3*NTrk+3;
  Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
  W_mat.block(0,0,ndim,ndim) = cov;
  W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = PV->covariancePosition();
  Amg::MatrixX V_err = D_vec.transpose() * W_mat * D_vec;

  double a0xyErrsq = V_err(0,0);
  if (a0xyErrsq <= 0.) ATH_MSG_DEBUG("a0xyError: negative sqrt a0xyErrsq " << a0xyErrsq);
  return (a0xyErrsq>0.) ? sqrt(a0xyErrsq) : 0.;
}

double CascadeTools::a0(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const
{
  if(particleMom.size() == 0) return -999999.;
  double cosineTheta = cosTheta(particleMom,SV,PV);
  double sinTheta = ((1.-cosineTheta*cosineTheta)>0.) ? sqrt((1.-cosineTheta*cosineTheta)) : 0.;
  return (SV->position()-PV->position()).mag() * sinTheta;
}

double CascadeTools::a0Error(const std::vector<TLorentzVector> &particleMom, const Amg::MatrixX& cov, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const
{
  if(particleMom.size() == 0) return -999999.;
  auto vert = SV->position() - PV->position();
  double dx = vert.x();
  double dy = vert.y();
  double dz = vert.z();
  Amg::Vector3D P = momentum(particleMom);;
  double Px = P.x();
  double Py = P.y();
  double Pz = P.z();
  double P2 = P.mag2();
  double L = Px*dx+Py*dy+Pz*dz;
  double dR2 = vert.mag2();
  double d = sqrt((P2*dR2-L*L)/P2);

  unsigned int NTrk = particleMom.size();

  double da0dx = (P2*dx-L*Px)/(P2*d);
  double da0dy = (P2*dy-L*Py)/(P2*d);
  double da0dz = (P2*dz-L*Pz)/(P2*d);
  double da0dx0 = -da0dx;
  double da0dy0 = -da0dy;
  double da0dz0 = -da0dz;

  Amg::MatrixX D_vec(3*NTrk+6,1); D_vec.setZero();
  D_vec(0) = da0dx;
  D_vec(1) = da0dy;
  D_vec(2) = da0dz;
  for( unsigned int it=0; it<NTrk; it++) {
    D_vec(3*it+3) = (L*(L*Px-P2*dx))/(P2*P2*d);
    D_vec(3*it+4) = (L*(L*Py-P2*dy))/(P2*P2*d);
    D_vec(3*it+5) = (L*(L*Pz-P2*dz))/(P2*P2*d);
  }
  D_vec(3*NTrk+3) = da0dx0;
  D_vec(3*NTrk+4) = da0dy0;
  D_vec(3*NTrk+5) = da0dz0;

  unsigned int ndim = 3*NTrk+3;
  Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
  W_mat.block(0,0,ndim,ndim) = cov;
  W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = PV->covariancePosition();
  Amg::MatrixX V_err = D_vec.transpose() * W_mat * D_vec;

  double a0Errsq = V_err(0,0);
  if (a0Errsq <= 0.) ATH_MSG_DEBUG("a0Error: negative sqrt a0Errsq " << a0Errsq);
  return (a0Errsq>0.) ? sqrt(a0Errsq) : 0.;
}

Amg::Vector3D CascadeTools::momentum(const std::vector<TLorentzVector> &particleMom) const
{
  if(particleMom.size() == 0) {
    Amg::Vector3D p; p.setZero();
    return p;
  }
  TLorentzVector totalMom;
  unsigned int NTrk = particleMom.size();
  for( unsigned int it=0; it<NTrk; it++) totalMom += particleMom[it];
  TVector3 P3 = totalMom.Vect();
  Amg::Vector3D mom(P3.Px(),P3.Py(),P3.Pz());
  return mom;
}

double CascadeTools::massProbability(double V0Mass, double mass, double massErr) const
{
  if(massErr > 0.) {
    double chi2 = (V0Mass - mass)*(V0Mass - mass)/(massErr*massErr);
    int ndf = 1;
    Genfun::CumulativeChiSquare myCumulativeChiSquare(ndf);
    if (chi2 > 0.) {
      double achi2prob = 1.-myCumulativeChiSquare(chi2);
      return achi2prob;
    } else {
      ATH_MSG_DEBUG("chi2 <= 0");
      return -1.;
    }
  } else {
    return -1.;
  }
}

double CascadeTools::vertexProbability(int ndf, double chi2) const
{
  if (ndf > 0.) {
    Genfun::CumulativeChiSquare myCumulativeChiSquare(ndf);
    if (chi2 > 0.) {
      double chi2prob = 1.-myCumulativeChiSquare(chi2);
      return chi2prob;
    } else {
      ATH_MSG_DEBUG("chi2 <= 0");
      return -1.;
    }
  } else {
    ATH_MSG_DEBUG("ndf <= 0");
    return -1.;
  }
}


Amg::MatrixX * CascadeTools::convertCovMatrix(const xAOD::Vertex * vxCandidate) const
{
  unsigned int NTrk = vxCandidate->nTrackParticles();
  std::vector<float> matrix = vxCandidate->covariance();

  int ndim = 0;

  if ( matrix.size() == (3*NTrk+3)*(3*NTrk+3+1)/2) {
    ndim = 3*NTrk+3;
  } else if (matrix.size() == (5*NTrk+3)*(5*NTrk+3+1)/2) {
    ndim = 5*NTrk+3;
  } else {
    return nullptr;
  }

  Amg::MatrixX* mtx = new Amg::MatrixX(ndim,ndim);

  long int ij=0;
  for (int i=1; i<= ndim; i++) {
    for (int j=1; j<=i; j++){
      if (i==j) {
        (*mtx)(i-1,j-1)=matrix[ij];
      } else {
        (*mtx).fillSymmetric(i-1,j-1,matrix[ij]);
      }
      ij++;
     }
  }
 // NOTE: mtx is a pointer! Take care of deleting it after you do not need it anymore!!!

  return mtx;
} 
  
Amg::MatrixX CascadeTools::SetFullMatrix(int NTrk, const std::vector<float> & Matrix) const
{

  Amg::MatrixX mtx(3+3*NTrk,3+3*NTrk);   // Create identity matrix of needed size

  int ij=0;

  for(int i=0; i<(3+3*NTrk); i++){
    for(int j=0; j<=i; j++){
                mtx(i,j)=Matrix[ij];
       if(i!=j) mtx(j,i)=Matrix[ij];
       ij++;
    }
  }

  return mtx;
}

} //end of namespace definitions

