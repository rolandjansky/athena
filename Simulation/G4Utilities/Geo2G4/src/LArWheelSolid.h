/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArWheelSolid
// Authors: A. M. Soukharev, with revisions by William Seligman

// Revised 11-May-2001 WGS to integrate it with LArEMECConstruction
// November 2001 updates and bugfixes by A. Soukharev
// August 2002 - A. Soukharev - speed improvements
// 15 Apr 2003 AMS move geometry calculations to Calculator

#ifndef __LArWheelSolid_HH__
#define __LArWheelSolid_HH__

#include "AthenaKernel/MsgStreamMember.h"
#include "G4VSolid.hh"

// set this to allow debug output in LArWheelSolid methods
// disabled by default to avoid any performance degradation
//#define DEBUG_LARWHEELSOLID

// set this to use speed-improved version of DistanceToOut
#define LArWheelSolidDTO_NEW

// set this to use speed-improved version of DistanceToIn
#define LArWheelSolidDTI_NEW

#ifdef DEBUG_LARWHEELSOLID
extern G4bool Verbose;
#define MSG_VECTOR(v) "(" << v.x() << ", " << v.y() << ", " << v.z() << ")"
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

typedef enum {
  InnerAbsorberWheel,
  OuterAbsorberWheel,
  InnerElectrodWheel,
  OuterElectrodWheel,
  InnerAbsorberModule,
  OuterAbsorberModule,
  InnerElectrodModule,
  OuterElectrodModule,
  InnerGlueWheel,
  OuterGlueWheel,
  InnerLeadWheel,
  OuterLeadWheel
} LArWheelSolid_t;

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

  const G4Polycone *GetBoundingPolycone(void) const { return BoundingPolycone; }
  const LArWheelCalculator *GetCalculator(void) const { return Calculator; }
  LArWheelSolid_t GetType(void) const { return Type; }

  G4ThreeVector GetPointOnSurface(void) const;
  G4double GetCubicVolume(void);
  G4double GetSurfaceArea(void);

private:
  static const G4double Tolerance;
  static const G4double IterationPrecision;
  static const G4double IterationPrecision2;
  static const unsigned int IterationsLimit;

  LArWheelSolid_t Type;
  LArWheelCalculator *Calculator;
  G4double FanHalfThickness;
  G4double FanPhiAmplitude;
  G4double MinPhi;
  G4double MaxPhi;
  G4double PhiPosition;
  G4Polycone* BoundingPolycone;
  G4Polycone** FanSection;
  G4int MaxFanSection;
  G4double *FanSectionLimits;
  G4int MaxFanSectionLimits;

  LArFanSections *m_fs;

  // limits for use in service functions
  G4double Zmin, Zmax, Rmin, Rmax;

  void inner_solid_init(const G4String &);
  void outer_solid_init(const G4String &);
  void set_phi_size(void);

  virtual G4double distance_to_in(const G4ThreeVector &,
                                  const G4double,
                                  const G4ThreeVector &) const;
  virtual G4double distance_to_in(G4ThreeVector &, const G4ThreeVector &) const;
  virtual G4double distance_to_in_ref(G4ThreeVector &, const G4ThreeVector &) const;
  G4double in_iteration_process(const G4ThreeVector &,
                                G4double,
                                const G4ThreeVector &) const;
  G4double search_for_nearest_point(const G4ThreeVector &,
                                    const G4ThreeVector &) const;
  G4bool search_for_most_remoted_point(const G4ThreeVector &,
                                       const G4ThreeVector &,
                                       G4ThreeVector &) const;
  G4double distance_to_out(const G4ThreeVector &,
                           const G4ThreeVector &) const;
  G4double distance_to_out_ref(const G4ThreeVector &, const G4ThreeVector &) const;
  G4double out_iteration_process(const G4ThreeVector &,
                                 const G4ThreeVector &) const;
  G4int select_fan_section(G4double) const;

  bool fs_inner_escape(G4double &b,
                       const G4ThreeVector &p, const G4ThreeVector &v) const;
  void fs_outer_escape(const G4int &fan_section, G4double &b,
                       const G4ThreeVector &p, const G4ThreeVector &v) const;

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

  TF1 *f_area, *f_vol, *f_area_on_pc, *f_length, *f_side_area;

  double test_index;
  friend double LArWheelSolid_fcn_area(double *, double *);
  friend double LArWheelSolid_fcn_vol(double *, double *);
  friend double LArWheelSolid_fcn_area_on_pc(double *, double *);
  friend double LArWheelSolid_get_dl(double *, double *, G4int);
  friend double LArWheelSolid_fcn_side_area(double *, double *);

#ifdef DEBUG_LARWHEELSOLID
  G4double in_chord_method(
                           const G4ThreeVector &p0, const G4ThreeVector &p1,
                           const G4ThreeVector &v) const;

  static const char* inside(EInside i)
  {
    switch(i){
    case kInside: return "inside"; break;
    case kSurface: return "surface"; break;
    case kOutside: return "outside"; break;
    }
    return "unknown";
  }
#endif
};

#endif // __LArWheelSolid_HH__
