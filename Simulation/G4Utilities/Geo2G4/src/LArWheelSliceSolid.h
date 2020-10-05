/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_LARWHEELSLICESOLID_H
#define GEO2G4_LARWHEELSLICESOLID_H

#include "AthenaKernel/MsgStreamMember.h"
#include "G4VSolid.hh"

// set this to allow debug output in LArWheelSliceSolid methods
// disabled by default to avoid any performance degradation
//#define DEBUG_LARWHEELSLICESOLID

// set this to check in dti and dto functions if particle direction
// pointing inside or outside of volume to return zero fast when it is required by spec.
// currently at development stage, requires accurate surface normal calculations
//#define CHECK_DIRTONORM_ANGLE_ON_SURFACE

#ifdef DEBUG_LARWHEELSLICESOLID
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
class LArWheelCalculator;
class TF1;
class G4Polyhedra;

class LArWheelSliceSolid : public G4VSolid
{
  public:
    typedef enum { Inner, Outer } pos_t;
    typedef enum { Absorber, Electrode, Glue, Lead } type_t;

    LArWheelSliceSolid(
        const G4String& name,
        pos_t pos, type_t type, size_t slice,
        G4int zside = 1,
        const LArWheelCalculator *calc = 0
    );
    LArWheelSliceSolid(const G4String& name);
    virtual ~LArWheelSliceSolid(){}

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
    G4GeometryType GetEntityType() const { return TypeStr(); }
    void DescribeYourselfTo(G4VGraphicsScene&) const;
    G4VisExtent GetExtent() const;
    G4Polyhedron* CreatePolyhedron() const;
    virtual std::ostream& StreamInfo(std::ostream& os) const { return os; }

    G4ThreeVector GetPointOnSurface(void) const;
    G4double GetCubicVolume(void);
    G4double GetSurfaceArea(void);
  //

    const G4VSolid *GetBoundingShape(void) const { return m_BoundingShape; }
    const LArWheelCalculator *GetCalculator(void) const { return m_Calculator; }

  private:
    static const G4double s_Tolerance;
    static const G4double s_AngularTolerance;
    static const G4double s_IterationPrecision;
    static const G4double s_IterationPrecision2;
    static const unsigned int s_IterationsLimit;

    pos_t m_Pos;
    type_t m_Type;
    const LArWheelCalculator *m_Calculator;
    G4VSolid* m_BoundingShape;

    G4double m_FanHalfThickness, m_FHTplusT, m_FHTminusT;

  // limits used in DTI
    G4double m_Xmin, m_Xmax;

  // Special limit, used in dto
    G4double m_Ymin;

  // limits for use in service functions
    G4double m_Zmin, m_Zmax, m_Rmin, m_Rmax;

    void inner_solid_init(const G4String &, size_t slice);
    void outer_solid_init(const G4String &, size_t slice);
    void fill_zsect(std::vector<G4double> &, G4double zMid = 0.) const;

    virtual G4double distance_to_in(G4ThreeVector &, const G4ThreeVector &, int) const;
    G4double in_iteration_process(const G4ThreeVector &,
                                  G4double, G4ThreeVector &, int) const;
    G4double search_for_nearest_point(
        const G4ThreeVector &, const G4double,
        const G4ThreeVector &, int
    ) const;
    G4bool search_for_most_remoted_point(
        const G4ThreeVector &, const G4ThreeVector &,
        G4ThreeVector &, const int
    ) const;
    G4double out_iteration_process(
        const G4ThreeVector &, G4ThreeVector &, const int
    ) const;

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
    friend double LArWheelSliceSolid_fcn_area(double *, double *);
    friend double LArWheelSliceSolid_fcn_vol(double *, double *);
    friend double LArWheelSliceSolid_fcn_area_on_pc(double *, double *);
    friend double LArWheelSliceSolid_get_dl(double *, double *, G4int);
    friend double LArWheelSliceSolid_fcn_side_area(double *, double *);

#ifdef DEBUG_LARWHEELSLICESOLID
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
#endif
  private:
    G4String TypeStr(void) const;

    void createSolid(const G4String& name, G4int zside, size_t slice);
    inline void check_slice(size_t size, size_t slice) const;
};

#endif // GEO2G4_LARWHEELSLICESOLID_H
