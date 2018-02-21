/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <stdexcept>
#include "boost/io/ios_state.hpp"
#include <map>

#include "TRandom3.h"
#include "TF1.h"
#include "TNtupleD.h"
#include "TFile.h"
// For root version ifdef
#include "TROOT.h"

#include "G4Polycone.hh"

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "LArWheelSolid.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
//#define LOCAL_DEBUG 1
#include<stdlib.h>

static double IntPrecision = 0.0001;

EInside LArWheelSolid::Inside_accordion(const G4ThreeVector &p) const
{
  G4ThreeVector p1 = p;
  int p1_fan = 0;
  G4double d = m_FanHalfThickness - fabs(GetCalculator()->DistanceToTheNearestFan(p1, p1_fan));
  if(d > s_Tolerance) return kInside;
  if(d > -s_Tolerance) return kSurface;
  return kOutside;
}

void LArWheelSolid::set_failover_point(G4ThreeVector &p,
#ifdef LOCAL_DEBUG
                                       const char *m) const
#else
  const char *) const
#endif
  {
    p[0] = 0.; p[1] = m_Rmin; p[2] = m_Zmin;
    int p_fan = 0;
    GetCalculator()->DistanceToTheNearestFan(p, p_fan);

#ifdef LOCAL_DEBUG
    if(m) std::cout << m << std::endl;
#endif
  }

static void get_r(const G4Polycone *p, G4double z,
                  G4double &rmin, G4double &rmax
                  )
{
  G4ThreeVector from(10.*CLHEP::m, 0., z);
  rmax = from[0] - p->DistanceToIn(from, G4ThreeVector(-1., 0., 0.));
  from[0] = 0.;
  rmin = p->DistanceToIn(from, G4ThreeVector(1., 0., 0.));
}

static TRandom *rnd = 0;
G4ThreeVector LArWheelSolid::GetPointOnSurface(void) const
{
  if(rnd == 0){
    rnd = new TRandom3(0);
  }

  G4double r = rnd->Uniform();

  G4ThreeVector p(0., 0., 0.);

  G4double level1 = .980;
  G4double level2 = .993;
  const char *v = getenv("LARWHEELSOLID_TEST_MODE_LEVEL1");
  if(v) level1 = atof(v);
  const char *v1 = getenv("LARWHEELSOLID_TEST_MODE_LEVEL2");
  if(v1) level2 = atof(v1);

#if LOCAL_DEBUG > 1
  std::cout << "LWS::GPOS " << r << std::endl;
#endif

  if(r <= level1){
    get_point_on_accordion_surface(p);
  } else if(r <= level2){
    get_point_on_polycone_surface(p);
  } else if(r <= 1.){
    get_point_on_flat_surface(p);
  } else {
    G4Exception("LArWheelSolid", "Rnd generator error", FatalException, "GetPointOnSurface: Wrong data from rnd generator");
  }
  return p;
}

void LArWheelSolid::get_point_on_accordion_surface(G4ThreeVector &p) const
{
  p[0] = 0.;
  p[1] = 0.;
  p[2] = rnd->Uniform(m_Zmin, m_Zmax);

  G4double rmin, rmax;
  get_r(m_BoundingPolycone, p[2], rmin, rmax);

  p[1] = rnd->Uniform(rmin, rmax);
  p.setPhi(rnd->Uniform(m_MinPhi, m_MaxPhi));
  G4double dphi = p.phi();
  int p_fan = 0;
  G4double d = GetCalculator()->DistanceToTheNearestFan(p, p_fan);
  dphi -= p.phi();

  G4int side = 0;
  if(d < 0.) side = -1;
  if(d >= 0.) side = 1;

  G4double a = GetCalculator()->AmplitudeOfSurface(p, side, p_fan);
  p[0] = a;

  p.rotateZ(dphi);

  if(m_BoundingPolycone->Inside(p) == kOutside){
    G4ThreeVector D = p; D[2] = 0.;
    G4double d1 = m_BoundingPolycone->DistanceToIn(p, D);
    if(d1 > 10.*CLHEP::m){
      D *= -1;
      d1 = m_BoundingPolycone->DistanceToIn(p, D);
    }
    if(d1 > 10.*CLHEP::m){
      set_failover_point(p, "acc fail0");
      return;
    }
    d1 *= 2.;

    G4ThreeVector B = p + D * d1;
    G4double dphi = B.phi();
    int B_fan = 0;
    GetCalculator()->DistanceToTheNearestFan(B, B_fan);
    dphi -= B.phi();

    B[0] = GetCalculator()->AmplitudeOfSurface(B, side, B_fan);
    B.rotateZ(dphi);
    EInside Bi = m_BoundingPolycone->Inside(B);
    if(Bi == kSurface){
      p = B;
      return;
    }
    if(Bi == kOutside){
      set_failover_point(p, "acc fail1");
      return;
    }
    G4ThreeVector D1 = (p - B).unit();
    G4ThreeVector X = B + D1 * m_BoundingPolycone->DistanceToOut(B, D1);
    if(Inside(X) == kSurface){
      p = X;
    } else { // failed
      set_failover_point(p, "acc fail2");
      return;
    }
  }
}

void LArWheelSolid::get_point_on_polycone_surface(G4ThreeVector &p) const
{
  const G4double z = rnd->Uniform(m_Zmin, m_Zmax);
  G4double rmin, rmax;
  get_r(m_BoundingPolycone, z, rmin, rmax);
  const bool inner = rnd->Uniform() > 0.5? true: false;

  p[0] = 0.; p[1] = inner? rmin: rmax; p[2] = z;
  p.setPhi(rnd->Uniform(m_MinPhi, m_MaxPhi));
  G4double dphi = p.phi();
  int p_fan = 0;
  GetCalculator()->DistanceToTheNearestFan(p, p_fan);
  dphi -= p.phi();

  const G4double r = p[1];

  G4ThreeVector A1(0., r, z);
  A1[0] = GetCalculator()->AmplitudeOfSurface(A1, -1, p_fan);
  A1.rotateZ(dphi);
  EInside A1i = m_BoundingPolycone->Inside(A1);
  //	EInside A1a = Inside_accordion(A1);
  //std::cout << "A1: " << A1i << " " << A1a << std::endl;
  if(A1i == kSurface){
    //std::cout << "got A1" << std::endl;
    p = A1;
    return;
  }

  G4ThreeVector A2(0., r, z);
  A2[0] = GetCalculator()->AmplitudeOfSurface(A2, 1, p_fan);
  A2.rotateZ(dphi);
  EInside A2i = m_BoundingPolycone->Inside(A2);
  //	EInside A2a = Inside_accordion(A2);
  //std::cout << "A2: " << A2i << " " << A2a << std::endl;
  if(A2i == kSurface){
    //std::cout << "got A2" << std::endl;
    p = A2;
    return;
  }

  if(A1i != A2i){
    if(A2i == kOutside){
      std::swap(A1, A2);
      std::swap(A1i, A2i);
    }
    // here A1 is outside BP, A2 is inside BP
    G4ThreeVector d = (A2 - A1).unit();
    p = A1 + d * m_BoundingPolycone->DistanceToIn(A1, d);
    //std::cout << "got A1<->A2" << std::endl;
    return;
  }
  // here A1i == A2i

  G4double step;
  if(A1i == kInside){
    G4double d1 = m_BoundingPolycone->DistanceToOut(A1);
    G4double d2 = m_BoundingPolycone->DistanceToOut(A2);
    step = d1 > d2? d1 : d2;
    if(inner) step *= -2;
    else step *= 2;
  } else {
    G4double d1 = m_BoundingPolycone->DistanceToIn(A1);
    G4double d2 = m_BoundingPolycone->DistanceToIn(A2);
    step = d1 > d2? d1 : d2;
    if(inner) step *= 2;
    else step *= -2;
  }

  G4ThreeVector B1(0., r + step, z);
  B1[0] = GetCalculator()->AmplitudeOfSurface(B1, -1, p_fan);
  B1.rotateZ(dphi);
  EInside B1i = m_BoundingPolycone->Inside(B1);
  //	EInside B1a = Inside_accordion(B1);
  //std::cout << "B1: " << B1i << " " << B1a << std::endl;
  if(B1i == kSurface){
    //std::cout << "got B1" << std::endl;
    p = B1;
    return;
  }
  G4ThreeVector B2(0., r + step, z);
  B2[0] = GetCalculator()->AmplitudeOfSurface(B2, 1, p_fan);
  B2.rotateZ(dphi);
  EInside B2i = m_BoundingPolycone->Inside(B2);
  //	EInside B2a = Inside_accordion(B2);
  //std::cout << "B2: " << B2i << " " << B2a << std::endl;
  if(B2i == kSurface){
    //std::cout << "got B2" << std::endl;
    p = B2;
    return;
  }

  if(B1i == A1i || B2i == A1i){ // failed
    set_failover_point(p, "pol fail1");
    return;
  }
  if(A1i == kInside){
    std::swap(A1, B1);
    std::swap(A2, B2);
    std::swap(A1i, B1i);
    std::swap(A2i, B2i);
  }
  // here A* outside, B* inside, all on accordion surface

  G4ThreeVector d1 = (A1 - B1).unit();
  G4ThreeVector X1 = B1 + d1 * m_BoundingPolycone->DistanceToOut(B1, d1);
  G4ThreeVector d2 = (A2 - B2).unit();
  G4ThreeVector X2 = B2 + d2 * m_BoundingPolycone->DistanceToOut(B2, d2);

  G4ThreeVector X = X1;
  G4double phi1 = X1.phi(), phi2 = X2.phi();
  // X1 corresponds to side = -, X2 to side = +
  if(phi1 > 0. && phi2 < 0.) phi2 += CLHEP::twopi;
  G4double phiX = rnd->Uniform(phi1, phi2);
  if(phiX > CLHEP::pi) phiX -= CLHEP::twopi;
  X.setPhi(phiX);

  if(Inside(X) == kSurface){
    p = X;
  } else { // failed
    set_failover_point(p, "pol fail2");
  }
}

void LArWheelSolid::get_point_on_flat_surface(G4ThreeVector &p) const
{
  p[0] = 0.;
  p[1] = 0.;
  p[2] = rnd->Uniform() > 0.5? m_Zmin: m_Zmax;

  G4double rmin, rmax;
  get_r(m_BoundingPolycone, p[2], rmin, rmax);

  p[1] = rnd->Uniform(rmin, rmax);
  p.setPhi(rnd->Uniform(m_MinPhi, m_MaxPhi));
  G4double dphi = p.phi();
  int p_fan = 0;
  GetCalculator()->DistanceToTheNearestFan(p, p_fan);
  dphi -= p.phi();

  p[0] = rnd->Uniform(
                      GetCalculator()->AmplitudeOfSurface(p, -1, p_fan),
                      GetCalculator()->AmplitudeOfSurface(p, 1, p_fan)
                      );

  p.rotateZ(dphi);

  if(m_BoundingPolycone->Inside(p) != kSurface){
    set_failover_point(p, "flat fail");
  }
}

G4double LArWheelSolid::GetCubicVolume(void)
{
  // sagging ignored, effect should be negligible
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
  double result =
    f_vol->Integral(m_Rmin, Rmax, (const Double_t *)0, IntPrecision)
#else
    double result =
    m_f_vol->Integral(m_Rmin, m_Rmax, IntPrecision)
#endif

#ifndef LOCAL_DEBUG
    * GetCalculator()->GetNumberOfFans()
#endif
    ;
  return result;
}

G4double LArWheelSolid::get_area_on_polycone(void) const
{
  return m_f_area_on_pc->Integral(m_Zmin, m_Zmax);
}

G4double LArWheelSolid::get_area_on_face(void) const
{
  G4double result = 0.;
  G4double rmin, rmax;
  get_r(m_BoundingPolycone, m_Zmin, rmin, rmax);
  result += rmax - rmin;
  get_r(m_BoundingPolycone, m_Zmax, rmin, rmax);
  result += rmax - rmin;
  result *= GetCalculator()->GetFanHalfThickness() * 2.;
  return result;
}

G4double LArWheelSolid::get_length_at_r(G4double r) const
{
  m_f_length->SetParameter(0, r);

  double zmin = m_BoundingPolycone->DistanceToIn(
                                               G4ThreeVector(0., r, -m_Zmin), G4ThreeVector(0., 0., 1.)
                                               );
  zmin -= m_Zmin * 2.;
  double zmax = m_BoundingPolycone->DistanceToIn(
                                               G4ThreeVector(0., r, m_Zmax), G4ThreeVector(0., 0., -1.)
                                               );
  zmax = m_Zmax - zmax;
  double result = m_f_length->Integral(zmin, zmax);
  return result;
}

G4double LArWheelSolid::get_area_on_side(void) const
{
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
  return f_side_area->Integral(m_Rmin, m_Rmax, (const Double_t *)0, IntPrecision);
#else
  return m_f_side_area->Integral(m_Rmin, m_Rmax, IntPrecision);
#endif
}

G4double LArWheelSolid::GetSurfaceArea(void)
{
  double result = 0.;

  double a1 = get_area_on_polycone();
  result += a1;
#ifdef LOCAL_DEBUG
  std::cout << "get_area_on_polycone: " << a1/mm2 << std::endl;
#endif

  double a2 = get_area_on_face();
  result += a2;
#ifdef LOCAL_DEBUG
  std::cout << "get_area_on_face: " << a2/mm2 << std::endl;
#endif

  double a3 = get_area_on_side();
  result += a3;
#ifdef LOCAL_DEBUG
  std::cout << "get_area_on_side: " << a3/mm2 << std::endl;
#endif

  // sagging ignored, effect should be negligible
  return result
#ifndef LOCAL_DEBUG
    * GetCalculator()->GetNumberOfFans()
#endif
    ;
}

void LArWheelSolid::test(void)
{
  boost::io::ios_all_saver ias(std::cout);
  const char *on = getenv("LARWHEELSOLID_TEST");
  if(on == 0) return;
  std::string test_mode = on;

  std::cout << "============| LArWheelSolid test() routine |=============="
            << std::endl;
  std::cout << "Solid of type " << LArWheelSolidTypeString(m_Type)
            << std::endl;
  std::cout.precision(6);
  std::cout << std::fixed;
  const char *prec = getenv("LARWHEELSOLID_TEST_INTPRECISION");
  if(prec) IntPrecision = atof(prec);
  std::cout << "Int. precision " << IntPrecision << std::endl;
  std::cout << "test mode " << test_mode << std::endl;

  std::cout << "m_Rmin = " << m_Rmin << " m_Rmax = " << m_Rmax << std::endl
            << "m_Zmin = " << m_Zmin << " m_Zmax = " << m_Zmax << std::endl;

  //	TFile *F = new TFile("LArWheelSolid_test.root", "RECREATE");
  TFile *F = 0;
  TNtupleD *T = 0;
  if(test_mode.find("root") != std::string::npos){
    F = new TFile("LArWheelSolid_test.root", "UPDATE");
    T = new TNtupleD(GetName(), GetName(), "x:y:z");
  }
  int N = 1000000;
  const int Nmax(1000000000);
  char *NN = getenv("LARWHEELSOLID_TEST_NPOINTS");

  if(NN) {
    char *endptr;
    N = strtol(NN, &endptr, 0);
    if (endptr[0] != '\0') {
      throw std::invalid_argument("Could not convert string to int: " + std::string(NN));
    }
  }
  if (Nmax<N) {
    std::cout << "Number of points from LARWHEELSOLID_TEST_NPOINTS environment variable ("<<N<<") is too large. Using " << Nmax << " instead." << std::endl;
    N=Nmax;
  }
  if (N<0) {
    std::cout << "Number of points from LARWHEELSOLID_TEST_NPOINTS environment variable ("<<N<<") is negative!!. Using 0 instead." << std::endl;
    N=0;
  }
  if(test_mode.find("points") == std::string::npos){
    N = 0;
  } else {
    std::cout << N << " points" << std::endl;
  }
  for(int i = 0; i < N; ++ i){
    G4ThreeVector p = GetPointOnSurface();
#ifdef LOCAL_DEBUG
    EInside ii = Inside(p);
    if(ii != kSurface){
      std::cout << i << " "
                << (ii == kInside? "inside": "outside")
                << std::endl;
    }
#endif
    if(T) T->Fill(p[0], p[1], p[2]);
  }
  if(F){
    T->Write();
    F->Write();
    F->Close();
    delete F;
  }

  if(test_mode.find("volume") != std::string::npos){
    double cv = GetCubicVolume();
    std::cout << "GetCubicVolume: " << cv/CLHEP::mm3 << " mm^3" << std::endl;
  }

  if(test_mode.find("area") != std::string::npos){
    double sa = GetSurfaceArea();
    std::cout << "GetSurfaceArea: " << sa/CLHEP::mm2 << " mm^2" << std::endl;
  }

  std::cout << "======= end of ArWheelSolid test() routine ============="
            << std::endl;

  if(m_Type == OuterAbsorberWheel) {
    if(test_mode.find("once") != std::string::npos) exit(0); }

  ias.restore();
}

void LArWheelSolid::clean_tests(void) {
	if(m_f_area) {
		delete m_f_area;
	  	m_f_area = 0;
	}
	if(m_f_vol) {
		delete m_f_vol;
		m_f_vol = 0;
	}

	if(m_f_area_on_pc) {
		delete m_f_area_on_pc;
		m_f_area_on_pc = 0;
	}

	if(m_f_length) {
		delete m_f_length;
		m_f_length = 0;
	}
	if(m_f_side_area) {
		delete m_f_side_area;
		m_f_side_area = 0;
	}
}

G4double LArWheelSolid::get_area_at_r(G4double r) const
{
  m_f_area->SetParameter(0, r);

  double zmin = m_BoundingPolycone->DistanceToIn(
                                               G4ThreeVector(0., r, m_Zmin), G4ThreeVector(0., 0., 1.)
                                               );
  double zmax = m_BoundingPolycone->DistanceToIn(
                                               G4ThreeVector(0., r, m_Zmax), G4ThreeVector(0., 0., -1.)
                                               );
  zmax = m_Zmax - zmax;

  double result = m_f_area->Integral(zmin, zmax);

  return result;
}

static std::map<double, LArWheelSolid *> solid;

double LArWheelSolid_fcn_area(double *x, double *p)
{
  const double &z = x[0];
  const double &r = p[0];
  const double &index = p[1];

  G4ThreeVector a(0., r, z);
  double b = solid[index]->GetCalculator()->AmplitudeOfSurface(a, -1, 121) // sagging ignored, effect should be negligible, use arbitrary fan number
    - solid[index]->GetCalculator()->AmplitudeOfSurface(a, 1, 121);
  return b;
}

double LArWheelSolid_fcn_vol(double *x, double *p)
{
  const double &r = x[0];
  const double &index = p[0];

  return solid[index]->get_area_at_r(r);
}

double LArWheelSolid_fcn_area_on_pc(double *x, double *p)
{
  const double &z = x[0];
  const double &index = p[0];

  G4double rmin, rmax;
  get_r(solid[index]->m_BoundingPolycone, z, rmin, rmax);

  double result = 0.;
  G4ThreeVector a(0., rmin, z);
  result += solid[index]->GetCalculator()->AmplitudeOfSurface(a, -1, 232) // sagging ignored, effect should be negligible, use arbitrary fan number
    - solid[index]->GetCalculator()->AmplitudeOfSurface(a, 1, 232);
  a[1] = rmax;
  result += solid[index]->GetCalculator()->AmplitudeOfSurface(a, -1, 343)
    - solid[index]->GetCalculator()->AmplitudeOfSurface(a, 1, 343);

  return result;
}


double LArWheelSolid_get_dl(double *x, double *par, G4int side)
{
  const double &z = x[0];
  const double &r = par[0];
  const double &index = par[1];

  const double h = 0.001;

  //check what happens if z+h > m_Zmax etc
  G4ThreeVector p(0., r, z + h);
  G4double D1 = solid[index]->GetCalculator()->AmplitudeOfSurface(p, side, 5665);  // sagging ignored, effect should be negligible, use arbitrary fan number
  p[2] = z - h;
  D1 -= solid[index]->GetCalculator()->AmplitudeOfSurface(p, side, 5665);
  D1 /= 2 * h;

  p[2] = z + h / 2;
  G4double D2 = solid[index]->GetCalculator()->AmplitudeOfSurface(p, side, 5665);
  p[2] = z - h / 2;
  D2 -= solid[index]->GetCalculator()->AmplitudeOfSurface(p, side, 5665);
  D2 /= h;

  G4double D = (D2 * 4 - D1) / 3.;
  G4double dl = sqrt(1 + D * D);

  return dl;
}

static double fcn_length(double *x, double *p)
{
  return LArWheelSolid_get_dl(x, p, 1) + LArWheelSolid_get_dl(x, p, -1);
}

double LArWheelSolid_fcn_side_area(double *x, double *p)
{
  const double &r = x[0];
  const double &index = p[0];

  return solid[index]->get_length_at_r(r);
}

void LArWheelSolid::init_tests(void)
{
  m_test_index = double(solid.size());
  solid[m_test_index] = this;

#ifdef DEBUG_LARWHEELSOLID
	std::cout << "LArWheelSolid::init_tests: put " << this
	          << " with index " << m_test_index << std::endl;
#endif

  m_f_area = new TF1(
                   (GetName() + "_f_area").c_str(), &LArWheelSolid_fcn_area,
                   m_Zmin, m_Zmax, 2
                   );
  m_f_area->FixParameter(1, m_test_index);

  m_f_vol = new TF1(
                  (GetName() + "_f_vol").c_str(), &LArWheelSolid_fcn_vol,
                  m_Rmin, m_Rmax, 1
                  );
  m_f_vol->FixParameter(0, m_test_index);

  m_f_area_on_pc = new TF1(
                         (GetName() + "_f_area_pc").c_str(), &LArWheelSolid_fcn_area_on_pc,
                         m_Zmin, m_Zmax, 1
                         );
  m_f_area_on_pc->FixParameter(0, m_test_index);

  m_f_length = new TF1(
                     (GetName() + "_f_length").c_str(), &fcn_length,
                     m_Zmin, m_Zmax, 2
                     );
  m_f_length->FixParameter(1, m_test_index);

  m_f_side_area = new TF1(
                        (GetName() + "_f_side_area").c_str(), &LArWheelSolid_fcn_side_area,
                        m_Rmin, m_Rmax, 1
                        );
  m_f_side_area->FixParameter(0, m_test_index);
}

#ifdef DEBUG_LARWHEELSOLID
G4bool LArWheelSolid::test_dti(
	const G4ThreeVector &inputP, const G4ThreeVector &inputV,
	const G4double distance
) const
{
	if(distance > 10.*CLHEP::m){
		LWSDBG(1, std::cout << "DTI test skipped, distance > 10 m"
	                        << std::endl);
		return false;
	}
	unsigned long counter = 0;
	counter ++;
	G4ThreeVector p;
	if(m_BoundingShape->Inside(inputP) == kOutside){
		p = inputP + inputV * m_BoundingShape->DistanceToIn(inputP, inputV);
	} else p = inputP;
	const G4double phi0 = p.phi();
	int p_fan = 0;
	const G4double d = GetCalculator()->DistanceToTheNearestFan(p, p_fan);
	if(fabs(d) < m_FHTplusT){
		std::cout << "DTI test already inside" << MSG_VECTOR(p) << std::endl;
		return false;
	}
	G4ThreeVector v( inputV );
	v.rotateZ(p.phi() - phi0);
	const G4double dd = s_IterationPrecision;
	LWSDBG(1, std::cout << "Start DTI test, expect "
	                    << long(distance / dd) << " iterations"
	                    << std::endl);

	G4int V = Verbose;
	Verbose = 0;

	const G4double d1 = distance - s_IterationPrecision;
	bool first = true;
	for(G4double t = s_IterationPrecision; t < d1; t += dd){
		G4ThreeVector p1 = p + v * t;
		if(fabs(GetCalculator()->DistanceToTheNeutralFibre(p1, p_fan)) < m_FHTplusT){
			std::cout << "DTI test at " << MSG_VECTOR(inputP) << " -> "
			          << MSG_VECTOR(inputV) << ", translated to "
			          << MSG_VECTOR(p) << " - > " << MSG_VECTOR(v)
			          << " in range of "
			          << distance << ": found nearer intersection at local point"
			          << MSG_VECTOR(p1) << ", distance " << t
			          << ", call " << counter
			          << std::endl;
			Verbose = V;

			if(first){
				first = false;
				FILE *F = fopen("dti_error.dat", "w");
				if(F){
					fprintf(F, "%10e %10e %10e\n", p.x(), p.y(), p.z());
					fprintf(F, "%10e %10e %10e\n", v.x(), v.y(), v.z());
					for(G4double e = s_IterationPrecision; e < d1; e += dd){
						p1 = p + v * e;
						G4double f = fabs(GetCalculator()->DistanceToTheNeutralFibre(p1, p_fan)) - m_FanHalfThickness;
						fprintf(F, "%10e %10e\n", e, f);
					}
					fclose(F);
				}
			}

			return true;
		}
	}
	Verbose = V;
	LWSDBG(1, std::cout << "DTI test at " << MSG_VECTOR(p) << " -> "
			            << MSG_VECTOR(v) << " in range of "
			            << distance << ": allright" << std::endl);
	return false;
}

G4bool LArWheelSolid::test_dto(
	const G4ThreeVector &inputP, const G4ThreeVector &inputV,
	const G4double distance
) const
{
	if(distance > 10.*CLHEP::m){
		LWSDBG(1, std::cout << "DTO test skipped, distance > 10 m"
	                        << std::endl);
		return false;
	}
	unsigned long counter = 0;
	counter ++;
	G4ThreeVector p( inputP );
	const G4double phi0 = p.phi();
	int p_fan = 0;
	const G4double d = GetCalculator()->DistanceToTheNearestFan(p, p_fan);
	if(fabs(d) > m_FHTplusT){
		std::cout << "DTO test already outside" << MSG_VECTOR(p) << std::endl;
		return false;
	}
	G4ThreeVector v( inputV );
	v.rotateZ(p.phi() - phi0);
	const G4double dd = s_IterationPrecision;
	LWSDBG(1, std::cout << "Start DTO test, expect "
	                    << long(distance / dd) << " iterations"
	                    << std::endl);

	G4int V = Verbose;
	Verbose = 0;

	const G4double d1 = distance - s_IterationPrecision;
	static bool first = true;
	for(G4double t = s_IterationPrecision; t < d1; t += dd){
		G4ThreeVector p1 = p + v * t;
		if(fabs(GetCalculator()->DistanceToTheNeutralFibre(p1, p_fan)) > m_FHTplusT){
			std::cout << "DTO test at " << MSG_VECTOR(inputP) << " -> "
			          << MSG_VECTOR(inputV) << ", translated to "
			          << MSG_VECTOR(p) << " - > " << MSG_VECTOR(v)
			          << " in range of "
			          << distance << ": found nearer intersection at local point"
			          << MSG_VECTOR(p1) << ", distance " << t
			          << ", call " << counter
			          << std::endl;
			Verbose = V;

			if(first){
				first = false;
				FILE *F = fopen("dto_error.dat", "w");
				if(F){
					fprintf(F, "%10e %10e %10e\n", p.x(), p.y(), p.z());
					fprintf(F, "%10e %10e %10e\n", v.x(), v.y(), v.z());
					for(G4double e = s_IterationPrecision; e < d1; e += dd){
						p1 = p + v * e;
						G4double f = fabs(GetCalculator()->DistanceToTheNeutralFibre(p1, p_fan)) - m_FanHalfThickness;
						fprintf(F, "%10e %10e\n", e, f);
					}
					fclose(F);
				}
			}

			return true;
		}
	}
	Verbose = V;
	LWSDBG(1, std::cout << "DTO test at " << MSG_VECTOR(p) << " -> "
			            << MSG_VECTOR(v) << " in range of "
			            << distance << ": allright" << std::endl);
	return false;
}
#endif
