/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_LARWHEELSOLID_H
#define GEO2G4_LARWHEELSOLID_H

#include "AthenaKernel/MsgStreamMember.h"
#include "G4VSolid.hh"

// set this to allow debug output in LArWheelSolid methods
// disabled by default to avoid any performance degradation
//#define DEBUG_LARWHEELSOLID

// set this to use native G4 FanBound's methods for DisToIn
// instead of local calculations
//#define LARWHEELSOLID_USE_FANBOUND

// set this to use BoundingShape's methods for DisToOut
// instead of local calculations
#define LARWHEELSOLID_USE_BS_DTO

// change this to have more z sections
#define LARWHEELSOLID_ZSECT_MULT 1


// set this to check in dti and dto functions if particle direction
// pointing inside or outside of volume to return zero fast when it is required by spec.
// currently at development stage, requires accurate surface normal calculations
//#define CHECK_DIRTONORM_ANGLE_ON_SURFACE

#ifdef DEBUG_LARWHEELSOLID
#define LWSDBG(a, b) if(Verbose >= a) b
#define MSG_VECTOR(v) "(" << v.x() << ", " << v.y() << ", " << v.z() << ")"
//#define LWS_HARD_TEST_DTI
//#define LWS_HARD_TEST_DTO
#else
#define LWSDBG(a, b)
#endif

// Forward declarations.
class G4VGraphicsScene;
class G4VisExtent;
class G4Polyhedron;
class G4NURBS;
class G4VoxelLimits;
class G4AffineTransform;
class G4Polycone;
class LArWheelCalculator;
class TF1;
class LArFanSections;
class G4Polyhedra;

#include "LArWheelSolid_type.h"

inline const char *LArWheelSolidTypeString(LArWheelSolid_t type)
{
  switch(type){
  case InnerAbsorberWheel: return("InnerAbsorberWheel");
  case OuterAbsorberWheel: return("OuterAbsorberWheel");
  case InnerElectrodWheel: return("InnerElectrodWheel");
  case OuterElectrodWheel: return("OuterElectrodWheel");
  case InnerAbsorberModule: return("InnerAbsorberModule");
  case OuterAbsorberModule: return("OuterAbsorberModule");
  case InnerElectrodModule: return("InnerElectrodModule");
  case OuterElectrodModule: return("OuterElectrodModule");
  case InnerLeadWheel: return("InnerLeadWheel");
  case OuterLeadWheel: return("OuterLeadWheel");
  case InnerGlueWheel: return("InnerGlueWheel");
  case OuterGlueWheel: return("OuterGlueWheel");
  }
  return("unknown");
}

class LArWheelSolid : public G4VSolid
{
public:

  LArWheelSolid(const G4String& name, LArWheelSolid_t type, G4int zside = 1);
  virtual ~LArWheelSolid();

  // Mandatory for custom solid Geant4 functions
  EInside Inside(const G4ThreeVector&) const;
  G4double DistanceToIn(const G4ThreeVector&,
                        const G4ThreeVector&) const;
  G4double DistanceToIn(const G4ThreeVector&) const;
  G4double DistanceToOut(const G4ThreeVector&,
                         const G4ThreeVector&,
                         const G4bool calcNorm = false,
                         G4bool* validNorm = 0,
                         G4ThreeVector* n = 0) const;
  G4double DistanceToOut(const G4ThreeVector&) const;
  G4ThreeVector SurfaceNormal (const G4ThreeVector&) const;
  G4bool CalculateExtent(const EAxis,
                         const G4VoxelLimits&,
                         const G4AffineTransform&,
                         G4double&,
                         G4double&) const;
  G4GeometryType GetEntityType() const;
  void DescribeYourselfTo(G4VGraphicsScene&) const;
  G4VisExtent GetExtent() const;
  G4Polyhedron* CreatePolyhedron() const;

  // 07-Feb-2003 WGS: For compatibility with Geant 4.5.0
  virtual std::ostream& StreamInfo(std::ostream& os) const { return os; }

  const G4Polycone *GetBoundingPolycone(void) const { return m_BoundingPolycone; }
  const LArWheelCalculator *GetCalculator(void) const { return m_Calculator; }
  LArWheelSolid_t GetType(void) const { return m_Type; }

  G4ThreeVector GetPointOnSurface(void) const;
  G4double GetCubicVolume(void);
  G4double GetSurfaceArea(void);

private:
  static const G4double s_Tolerance;
  static const G4double s_AngularTolerance;
  static const G4double s_IterationPrecision;
  static const G4double s_IterationPrecision2;
  static const unsigned int s_IterationsLimit;

  G4bool m_IsOuter;
  const LArWheelSolid_t m_Type;
  LArWheelCalculator *m_Calculator;
  G4double m_FanHalfThickness, m_FHTplusT, m_FHTminusT;
  G4double m_FanPhiAmplitude;
  G4double m_MinPhi;
  G4double m_MaxPhi;
  const G4double m_PhiPosition;
  G4Polycone* m_BoundingPolycone;
  G4VSolid* m_BoundingShape;
#ifdef LARWHEELSOLID_USE_FANBOUND
  G4VSolid* m_FanBound;
#endif

  std::vector<G4double> m_Zsect;
  G4int m_Zsect_start_search;

  LArFanSections *m_fs;

  // z at outer wheel "bend"
  G4double m_Zmid;
  // Special limit, used in dto
  G4double m_Ymin;
  // limits for use in service functions
  G4double m_Zmin, m_Zmax, m_Rmin, m_Rmax;
  //artificial level to distinguish between inner and outer cones
  G4double m_Ymid;

  void inner_solid_init(const G4String &);
  void outer_solid_init(const G4String &);
  void set_phi_size(void);

  virtual G4double distance_to_in(G4ThreeVector &, const G4ThreeVector &, int) const;
  G4double in_iteration_process(const G4ThreeVector &,
                                G4double, G4ThreeVector &, int) const;
  G4double search_for_nearest_point(
	const G4ThreeVector &, const G4double,
	const G4ThreeVector &, int
  ) const;
  G4bool search_for_most_remoted_point(const G4ThreeVector &,
                                       const G4ThreeVector &,
                                       G4ThreeVector &, const int) const;
  G4double out_iteration_process(const G4ThreeVector &,
                                 G4ThreeVector &, const int) const;

  typedef enum {
	  NoCross, ExitAtInner, ExitAtOuter,
	  ExitAtFront, ExitAtBack, ExitAtSide
  } FanBoundExit_t;

  FanBoundExit_t find_exit_point(const G4ThreeVector &p,
                                 const G4ThreeVector &v,
                                 G4ThreeVector &q) const;
  G4bool fs_cross_lower(const G4ThreeVector &p, const G4ThreeVector &v,
                        G4ThreeVector &q) const;
  G4bool fs_cross_upper(const G4ThreeVector &p, const G4ThreeVector &v,
                        G4ThreeVector &q) const;
  G4bool check_D(G4double &b,
                 G4double A, G4double B, G4double C, G4bool) const;

/*
  FanBoundExit_t find_exit_point(const G4ThreeVector &p,
                                 const G4ThreeVector &v,
                                 EInside inside_bs,
                                 G4ThreeVector &q) const;
  G4bool fs_check_inner(const G4ThreeVector &p, const G4ThreeVector &v,
                        G4bool surface, G4ThreeVector &q) const;
*/
  G4int select_section(const G4double &Z) const;

  EInside Inside_accordion(const G4ThreeVector&) const;
  void get_point_on_accordion_surface(G4ThreeVector &) const;
  void get_point_on_polycone_surface(G4ThreeVector &) const;
  void get_point_on_flat_surface(G4ThreeVector &) const;
  void set_failover_point(G4ThreeVector &p, const char *m = 0) const;

  G4double get_area_on_polycone(void) const;
  G4double get_area_on_face(void) const;
  G4double get_area_on_side(void) const;

  G4double get_area_at_r(G4double r) const;
  G4double get_length_at_r(G4double r) const;

  void test(void);
  void clean_tests(void);
  void init_tests(void);
  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

protected:
  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;

  TF1 *m_f_area, *m_f_vol, *m_f_area_on_pc, *m_f_length, *m_f_side_area;

  double m_test_index;
  friend double LArWheelSolid_fcn_area(double *, double *);
  friend double LArWheelSolid_fcn_vol(double *, double *);
  friend double LArWheelSolid_fcn_area_on_pc(double *, double *);
  friend double LArWheelSolid_get_dl(double *, double *, G4int);
  friend double LArWheelSolid_fcn_side_area(double *, double *);

#ifdef DEBUG_LARWHEELSOLID
  static const char* inside(EInside i)
  {
    switch(i){
    case kInside: return "inside"; break;
    case kSurface: return "surface"; break;
    case kOutside: return "outside"; break;
    }
    return "unknown";
  }

  public:
	static G4int Verbose;
	void SetVerbose(G4int v){ Verbose = v; }
	G4bool test_dti(const G4ThreeVector &p,
	                const G4ThreeVector &v, const G4double distance) const;
	G4bool test_dto(const G4ThreeVector &p,
	                const G4ThreeVector &v, const G4double distance) const;
  private:
	const char *TypeStr(void) const { return LArWheelSolidTypeString(m_Type); }
#endif
};

#endif // GEO2G4_LARWHEELSOLID_H
