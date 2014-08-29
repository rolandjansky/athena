/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<iostream>
#include"boost/io/ios_state.hpp"
#include<map>

#include"TRandom3.h"
#include"TF1.h"
#include"TNtupleD.h"
#include"TFile.h"

#include"G4Polycone.hh"

#include"LArWheelSolid.h"
#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
//#define DEBUG 1
#include<stdlib.h>

static double IntPrecision = 0.0001;

EInside LArWheelSolid::Inside_accordion(const G4ThreeVector &p) const
{
  G4ThreeVector p1 = p;
  G4double d = FanHalfThickness - fabs(Calculator->DistanceToTheNearestFan(p1));
  if(d > Tolerance) return kInside;
  if(d > -Tolerance) return kSurface;
  return kOutside;
}

void LArWheelSolid::set_failover_point(G4ThreeVector &p,
#ifdef DEBUG
                                       const char *m) const
#else
  const char *) const
#endif
  {
    p[0] = 0.; p[1] = Rmin; p[2] = Zmin;
    Calculator->DistanceToTheNearestFan(p);

#ifdef DEBUG
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

#if DEBUG > 1
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
  p[2] = rnd->Uniform(Zmin, Zmax);

  G4double rmin, rmax;
  get_r(BoundingPolycone, p[2], rmin, rmax);

  p[1] = rnd->Uniform(rmin, rmax);
  p.setPhi(rnd->Uniform(MinPhi, MaxPhi));
  G4double dphi = p.phi();
  G4double d = Calculator->DistanceToTheNearestFan(p);
  dphi -= p.phi();

  G4int side = 0;
  if(d < 0.) side = -1;
  if(d >= 0.) side = 1;

  G4double a = Calculator->AmplitudeOfSurface(p, side);
  p[0] = a;

  p.rotateZ(dphi);

  if(BoundingPolycone->Inside(p) == kOutside){
    G4ThreeVector D = p; D[2] = 0.;
    G4double d1 = BoundingPolycone->DistanceToIn(p, D);
    if(d1 > 10.*CLHEP::m){
      D *= -1;
      d1 = BoundingPolycone->DistanceToIn(p, D);
    }
    if(d1 > 10.*CLHEP::m){
      set_failover_point(p, "acc fail0");
      return;
    }
    d1 *= 2.;

    G4ThreeVector B = p + D * d1;
    G4double dphi = B.phi();
    Calculator->DistanceToTheNearestFan(B);
    dphi -= B.phi();

    B[0] = Calculator->AmplitudeOfSurface(B, side);
    B.rotateZ(dphi);
    EInside Bi = BoundingPolycone->Inside(B);
    if(Bi == kSurface){
      p = B;
      return;
    }
    if(Bi == kOutside){
      set_failover_point(p, "acc fail1");
      return;
    }
    G4ThreeVector D1 = (p - B).unit();
    G4ThreeVector X = B + D1 * BoundingPolycone->DistanceToOut(B, D1);
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
  const G4double z = rnd->Uniform(Zmin, Zmax);
  G4double rmin, rmax;
  get_r(BoundingPolycone, z, rmin, rmax);
  const bool inner = rnd->Uniform() > 0.5? true: false;

  p[0] = 0.; p[1] = inner? rmin: rmax; p[2] = z;
  p.setPhi(rnd->Uniform(MinPhi, MaxPhi));
  G4double dphi = p.phi();
  Calculator->DistanceToTheNearestFan(p);
  dphi -= p.phi();

  const G4double r = p[1];

  G4ThreeVector A1(0., r, z);
  A1[0] = Calculator->AmplitudeOfSurface(A1, -1);
  A1.rotateZ(dphi);
  EInside A1i = BoundingPolycone->Inside(A1);
  //	EInside A1a = Inside_accordion(A1);
  //std::cout << "A1: " << A1i << " " << A1a << std::endl;
  if(A1i == kSurface){
    //std::cout << "got A1" << std::endl;
    p = A1;
    return;
  }

  G4ThreeVector A2(0., r, z);
  A2[0] = Calculator->AmplitudeOfSurface(A2, 1);
  A2.rotateZ(dphi);
  EInside A2i = BoundingPolycone->Inside(A2);
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
    p = A1 + d * BoundingPolycone->DistanceToIn(A1, d);
    //std::cout << "got A1<->A2" << std::endl;
    return;
  }
  // here A1i == A2i

  G4double step;
  if(A1i == kInside){
    G4double d1 = BoundingPolycone->DistanceToOut(A1);
    G4double d2 = BoundingPolycone->DistanceToOut(A2);
    step = d1 > d2? d1 : d2;
    if(inner) step *= -2;
    else step *= 2;
  } else {
    G4double d1 = BoundingPolycone->DistanceToIn(A1);
    G4double d2 = BoundingPolycone->DistanceToIn(A2);
    step = d1 > d2? d1 : d2;
    if(inner) step *= 2;
    else step *= -2;
  }

  G4ThreeVector B1(0., r + step, z);
  B1[0] = Calculator->AmplitudeOfSurface(B1, -1);
  B1.rotateZ(dphi);
  EInside B1i = BoundingPolycone->Inside(B1);
  //	EInside B1a = Inside_accordion(B1);
  //std::cout << "B1: " << B1i << " " << B1a << std::endl;
  if(B1i == kSurface){
    //std::cout << "got B1" << std::endl;
    p = B1;
    return;
  }
  G4ThreeVector B2(0., r + step, z);
  B2[0] = Calculator->AmplitudeOfSurface(B2, 1);
  B2.rotateZ(dphi);
  EInside B2i = BoundingPolycone->Inside(B2);
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
  G4ThreeVector X1 = B1 + d1 * BoundingPolycone->DistanceToOut(B1, d1);
  G4ThreeVector d2 = (A2 - B2).unit();
  G4ThreeVector X2 = B2 + d2 * BoundingPolycone->DistanceToOut(B2, d2);

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
  p[2] = rnd->Uniform() > 0.5? Zmin: Zmax;

  G4double rmin, rmax;
  get_r(BoundingPolycone, p[2], rmin, rmax);

  p[1] = rnd->Uniform(rmin, rmax);
  p.setPhi(rnd->Uniform(MinPhi, MaxPhi));
  G4double dphi = p.phi();
  Calculator->DistanceToTheNearestFan(p);
  dphi -= p.phi();

  p[0] = rnd->Uniform(
                      Calculator->AmplitudeOfSurface(p, -1),
                      Calculator->AmplitudeOfSurface(p, 1)
                      );

  p.rotateZ(dphi);

  if(BoundingPolycone->Inside(p) != kSurface){
    set_failover_point(p, "flat fail");
  }
}

G4double LArWheelSolid::GetCubicVolume(void)
{
  // sagging ignored, effect should be negligible
  double result =
    f_vol->Integral(Rmin, Rmax, (const Double_t *)0, IntPrecision)
#ifndef DEBUG
    * Calculator->GetNumberOfFans()
#endif
    ;
  return result;
}

G4double LArWheelSolid::get_area_on_polycone(void) const
{
  return f_area_on_pc->Integral(Zmin, Zmax);
}

G4double LArWheelSolid::get_area_on_face(void) const
{
  G4double result = 0.;
  G4double rmin, rmax;
  get_r(BoundingPolycone, Zmin, rmin, rmax);
  result += rmax - rmin;
  get_r(BoundingPolycone, Zmax, rmin, rmax);
  result += rmax - rmin;
  result *= Calculator->GetFanHalfThickness() * 2.;
  return result;
}

G4double LArWheelSolid::get_length_at_r(G4double r) const
{
  f_length->SetParameter(0, r);

  double zmin = BoundingPolycone->DistanceToIn(
                                               G4ThreeVector(0., r, -Zmin), G4ThreeVector(0., 0., 1.)
                                               );
  zmin -= Zmin * 2.;
  double zmax = BoundingPolycone->DistanceToIn(
                                               G4ThreeVector(0., r, Zmax), G4ThreeVector(0., 0., -1.)
                                               );
  zmax = Zmax - zmax;
  double result = f_length->Integral(zmin, zmax);
  return result;
}

G4double LArWheelSolid::get_area_on_side(void) const
{
  return f_side_area->Integral(Rmin, Rmax, (const Double_t *)0, IntPrecision);
}

G4double LArWheelSolid::GetSurfaceArea(void)
{
  double result = 0.;

  double a1 = get_area_on_polycone();
  result += a1;
#ifdef DEBUG
  std::cout << "get_area_on_polycone: " << a1/mm2 << std::endl;
#endif

  double a2 = get_area_on_face();
  result += a2;
#ifdef DEBUG
  std::cout << "get_area_on_face: " << a2/mm2 << std::endl;
#endif

  double a3 = get_area_on_side();
  result += a3;
#ifdef DEBUG
  std::cout << "get_area_on_side: " << a3/mm2 << std::endl;
#endif

  // sagging ignored, effect should be negligible
  return result
#ifndef DEBUG
    * Calculator->GetNumberOfFans()
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
  std::cout << "Solid of type " << LArWheelSolidTypeString(Type)
            << std::endl;
  std::cout.precision(6);
  std::cout << std::fixed;
  const char *prec = getenv("LARWHEELSOLID_TEST_INTPRECISION");
  if(prec) IntPrecision = atof(prec);
  std::cout << "Int. precision " << IntPrecision << std::endl;
  std::cout << "test mode " << test_mode << std::endl;

  std::cout << "Rmin = " << Rmin << " Rmax = " << Rmax << std::endl
            << "Zmin = " << Zmin << " Zmax = " << Zmax << std::endl;

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
  if(NN) N = atoi(NN);
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
#ifdef DEBUG
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

  if(Type == OuterAbsorberWheel) {
    if(test_mode.find("once") != std::string::npos) exit(0); }

  ias.restore();
}

void LArWheelSolid::clean_tests(void)
{
  if(f_area) delete f_area;
  if(f_vol) delete f_vol;
  if(f_area_on_pc) delete f_area_on_pc;
  if(f_length) delete f_length;
  if(f_side_area) delete f_side_area;
}

G4double LArWheelSolid::get_area_at_r(G4double r) const
{
  f_area->SetParameter(0, r);

  double zmin = BoundingPolycone->DistanceToIn(
                                               G4ThreeVector(0., r, Zmin), G4ThreeVector(0., 0., 1.)
                                               );
  double zmax = BoundingPolycone->DistanceToIn(
                                               G4ThreeVector(0., r, Zmax), G4ThreeVector(0., 0., -1.)
                                               );
  zmax = Zmax - zmax;

  double result = f_area->Integral(zmin, zmax);

  return result;
}

static std::map<double, LArWheelSolid *> solid;

double LArWheelSolid_fcn_area(double *x, double *p)
{
  const double &z = x[0];
  const double &r = p[0];
  const double &index = p[1];

  G4ThreeVector a(0., r, z);
  double b = solid[index]->Calculator->AmplitudeOfSurface(a, -1)
    - solid[index]->Calculator->AmplitudeOfSurface(a, 1);
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
  get_r(solid[index]->BoundingPolycone, z, rmin, rmax);

  double result = 0.;
  G4ThreeVector a(0., rmin, z);
  result += solid[index]->Calculator->AmplitudeOfSurface(a, -1)
    - solid[index]->Calculator->AmplitudeOfSurface(a, 1);
  a[1] = rmax;
  result += solid[index]->Calculator->AmplitudeOfSurface(a, -1)
    - solid[index]->Calculator->AmplitudeOfSurface(a, 1);

  return result;
}


double LArWheelSolid_get_dl(double *x, double *par, G4int side)
{
  const double &z = x[0];
  const double &r = par[0];
  const double &index = par[1];

  const double h = 0.001;

  //check what happens if z+h > Zmax etc
  G4ThreeVector p(0., r, z + h);
  G4double D1 = solid[index]->Calculator->AmplitudeOfSurface(p, side);
  p[2] = z - h;
  D1 -= solid[index]->Calculator->AmplitudeOfSurface(p, side);
  D1 /= 2 * h;

  p[2] = z + h / 2;
  G4double D2 = solid[index]->Calculator->AmplitudeOfSurface(p, side);
  p[2] = z - h / 2;
  D2 -= solid[index]->Calculator->AmplitudeOfSurface(p, side);
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
  test_index = double(solid.size());
  solid[test_index] = this;

  std::cout << "LArWheelSolid::init_tests:" << std::endl;
  std::cout << "Put " << this << " with index " << test_index
            << " into list of solids" << std::endl;
  std::cout << "Calculator: "<< Calculator
            << " BP: " << BoundingPolycone << std::endl;

  f_area = new TF1(
                   (GetName() + "_f_area").c_str(), &LArWheelSolid_fcn_area,
                   Zmin, Zmax, 2
                   );
  f_area->FixParameter(1, test_index);

  f_vol = new TF1(
                  (GetName() + "_f_vol").c_str(), &LArWheelSolid_fcn_vol,
                  Rmin, Rmax, 1
                  );
  f_vol->FixParameter(0, test_index);

  f_area_on_pc = new TF1(
                         (GetName() + "_f_area_pc").c_str(), &LArWheelSolid_fcn_area_on_pc,
                         Zmin, Zmax, 1
                         );
  f_area_on_pc->FixParameter(0, test_index);

  f_length = new TF1(
                     (GetName() + "_f_length").c_str(), &fcn_length,
                     Zmin, Zmax, 2
                     );
  f_length->FixParameter(1, test_index);

  f_side_area = new TF1(
                        (GetName() + "_f_side_area").c_str(), &LArWheelSolid_fcn_side_area,
                        Rmin, Rmax, 1
                        );
  f_side_area->FixParameter(0, test_index);
}
