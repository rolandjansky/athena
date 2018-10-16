#ifndef HEPVis_SbPolyhedron_h
#define HEPVis_SbPolyhedron_h
//--------------------------------------------------------------------//
// JFB:                                                               //
// SbPolyhedron was HepPolyhedron, retrofitted to Open Inventor       //
// infrastructure:                                                    //
//--------------------------------------------------------------------//


// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: SbPolyhedron.h,v 1.1.1.1 2007-05-29 16:08:16 tkittel Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// Class Description:
// SbPolyhedron is an intermediate class between description of a shape
// and visualization systems. It is intended to provide some service like:
//   - polygonization of shapes with triangulization (quadrilaterization)
//     of complex polygons;
//   - calculation of normals for faces and vertices;
//   - finding result of boolean operation on polyhedra;
//
// Public constructors:
//
//   SbPolyhedronBox (dx,dy,dz)
//                                        - create polyhedron for Box;
//   SbPolyhedronTrd1 (dx1,dx2,dy,dz)
//                                        - create polyhedron for G3 Trd1;
//   SbPolyhedronTrd2 (dx1,dx2,dy1,dy2,dz)
//                                        - create polyhedron for G3 Trd2;
//   SbPolyhedronTrap (dz,theta,phi, h1,bl1,tl1,alp1, h2,bl2,tl2,alp2)
//                                        - create polyhedron for G3 Trap;
//   SbPolyhedronPara (dx,dy,dz,alpha,theta,phi)
//                                        - create polyhedron for G3 Para;
//   SbPolyhedronTube (rmin,rmax,dz)
//                                        - create polyhedron for G3 Tube;
//   SbPolyhedronTubs (rmin,rmax,dz,phi1,dphi)
//                                        - create polyhedron for G3 Tubs;
//   SbPolyhedronCone (rmin1,rmax1,rmin2,rmax2,dz)
//                                        - create polyhedron for G3 Cone;
//   SbPolyhedronCons (rmin1,rmax1,rmin2,rmax2,dz,phi1,dphi)
//                                        - create polyhedron for G3 Cons;
//   SbPolyhedronPgon (phi,dphi,npdv,nz, z(*),rmin(*),rmax(*))
//                                        - create polyhedron for G3 Pgon;
//   SbPolyhedronPcon (phi,dphi,nz, z(*),rmin(*),rmax(*))
//                                        - create polyhedron for G3 Pcon;
//   SbPolyhedronSphere (rmin,rmax,phi,dphi,the,dthe)
//                                        - create polyhedron for Sphere;
//   SbPolyhedronTorus (rmin,rmax,rtor,phi,dphi)
//                                        - create polyhedron for Torus;
// Public functions:
//
//   GetNoVertices ()       - returns number of vertices;
//   GetNoFacets ()         - returns number of faces;
//   GetNextVertexIndex (index,edgeFlag) - get vertex indeces of the
//                            quadrilaterals in order;
//                            returns false when finished each face;
//   GetVertex (index)      - returns vertex by index;
//   GetNextVertex (vertex,edgeFlag) - get vertices with edge visibility
//                            of the quadrilaterals in order;
//                            returns false when finished each face;
//   GetNextVertex (vertex,edgeFlag,normal) - get vertices with edge
//                            visibility and normal of the quadrilaterals
//                            in order; returns false when finished each face;
//   GetNextEdgeIndeces (i1,i2,edgeFlag) - get indeces of the next edge;
//                            returns false for the last edge;
//   GetNextEdgeIndeces (i1,i2,edgeFlag,iface1,iface2) - get indeces of
//                            the next edge with indeces of the faces
//                            to which the edge belongs;
//                            returns false for the last edge;
//   GetNextEdge (p1,p2,edgeFlag) - get next edge;
//                            returns false for the last edge;
//   GetNextEdge (p1,p2,edgeFlag,iface1,iface2) - get next edge with indeces
//                            of the faces to which the edge belongs;
//                            returns false for the last edge;
//   GetFacet (index,n,nodes,edgeFlags=0,normals=0) - get face by index;
//   GetNextFacet (n,nodes,edgeFlags=0,normals=0) - get next face with normals
//                            at the nodes; returns false for the last face;
//   GetNormal (index)      - get normal of face given by index;
//   GetUnitNormal (index)  - get unit normal of face given by index;
//   GetNextNormal (normal) - get normals of each face in order;
//                            returns false when finished all faces;
//   GetNextUnitNormal (normal) - get normals of unit length of each face
//                            in order; returns false when finished all faces;
//   GetSurfaceArea()       - get surface area of the polyhedron;
//   GetVolume()            - get volume of the polyhedron;
//   GetNumberOfRotationSteps()   - get number of steps for whole circle;
//   SetNumberOfRotationSteps (n) - set number of steps for whole circle;
//   ResetNumberOfRotationSteps() - reset number of steps for whole circle
//                            to default value;
// History:
//
// 20.06.96 Evgeni Chernyaev <Evgueni.Tcherniaev@cern.ch> - initial version
//
// 23.07.96 John Allison
// - added GetNoVertices, GetNoFacets, GetNextVertex, GetNextNormal
//
// 30.09.96 E.Chernyaev
// - added GetNextVertexIndex, GetVertex by Yasuhide Sawada
// - added GetNextUnitNormal, GetNextEdgeIndeces, GetNextEdge
// - improvements: angles now expected in radians
//                 int -> G4int, double -> G4double
// - G4ThreeVector replaced by either G4Point3D or G4Normal3D
//
// 15.12.96 E.Chernyaev
// - private functions G4PolyhedronAlloc, G4PolyhedronPrism renamed
//   to AllocateMemory and CreatePrism
// - added private functions GetNumberOfRotationSteps, RotateEdge,
//   RotateAroundZ, SetReferences
// - rewritten G4PolyhedronCons;
// - added G4PolyhedronPara, ...Trap, ...Pgon, ...Pcon, ...Sphere, ...Torus,
//   so full List of implemented shapes now looks like:
//   BOX, TRD1, TRD2, TRAP, TUBE, TUBS, CONE, CONS, PARA, PGON, PCON,
//   SPHERE, TORUS
//
// 01.06.97 E.Chernyaev
// - RotateAroundZ modified and SetSideFacets added to allow Rmin=Rmax
//   in bodies of revolution
//
// 24.06.97 J.Allison
// - added static private member s_numberOfRotationSteps and static public
//   functions void SetNumberOfRotationSteps (G4int n) and
//   void ResetNumberOfRotationSteps ().  Modified
//   GetNumberOfRotationSteps() appropriately.  Made all three functions
//   inline (at end of this .hh file).
//   Usage:
//    G4Polyhedron::SetNumberOfRotationSteps
//     (fpView -> GetViewParameters ().GetNoOfSides ());
//    pPolyhedron = solid.CreatePolyhedron ();
//    G4Polyhedron::ResetNumberOfRotationSteps ();
//
// 19.03.00 E.Chernyaev
// - added boolean operations (add, subtract, intersect) on polyhedra;
//
// 25.05.01 E.Chernyaev
// - added GetSurfaceArea() and GetVolume();
//
// 16.03.07 T. Kittelmann
// - added SbPolyhedronPolygonXSect (to HEPVis, not GEANT4).
//
// 12.12.2012 R.M. Bianchi <riccardo.maria.bianchi@cern.ch>
// - apply some changes (from the newest OpenScientist/HEPVis from Guy BARRAND)
//   needed for the new BooleanProcessor.h from Geant4 4.9.6
// - added a new GetVertexFast(), from Guy Barrand.
//

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/SbLinear.h>


// VP1 change
#include <VP1HEPVis/SbRotation.h> //using doubles instead of floats.
//---


// rbianchi change

//- original
//typedef SbVec3f HVPoint3D;
//typedef SbVec3f HVNormal3D;
//typedef SbVec3f HVVector3D;

//- new
//NOTE : SbVec3d() does not initialize its internal fields.
//       To mimick the CLHEP/Point3D that initializes its fields,
//       we introduce the below HVPoint3D.
class HVPoint3D : public SbVec3d {
public:
  HVPoint3D();
  HVPoint3D(double x,double y,double z);
  HVPoint3D(const HVPoint3D& v);
  HVPoint3D(const SbVec3d& v);
  HVPoint3D& operator=(const HVPoint3D& v);
  HVPoint3D& operator=(const SbVec3d& v);
  friend HVPoint3D operator +(const HVPoint3D& v1, const HVPoint3D& v2);
};
HVPoint3D operator +(const HVPoint3D& v1,const HVPoint3D& v2);
typedef HVPoint3D HVNormal3D;
typedef HVPoint3D HVVector3D;
//---

// rbianchi change
//typedef SbPlane HVPlane3D;
//typedef SbPlane HVPlane3D;
#include <VP1HEPVis/SbPlane.h>
typedef HEPVis::SbPlane HVPlane3D;
//---
typedef SbRotation HVRotation;

#include <iostream>

#ifndef DEFAULT_NUMBER_OF_STEPS
#define DEFAULT_NUMBER_OF_STEPS 24
#endif

class SbFacet {
  friend class SbPolyhedron;
  friend std::ostream& operator<<(std::ostream&, const SbFacet &facet);

 private:
  struct { int v,f; } m_edge[4];

 public:
  SbFacet(int v1=0, int f1=0, int v2=0, int f2=0,
          int v3=0, int f3=0, int v4=0, int f4=0)
  { m_edge[0].v=v1; m_edge[0].f=f1; m_edge[1].v=v2; m_edge[1].f=f2;
    m_edge[2].v=v3; m_edge[2].f=f3; m_edge[3].v=v4; m_edge[3].f=f4; }
};

class SbPolyhedron {
  friend std::ostream& operator<<(std::ostream&, const SbPolyhedron &ph);

 private:
  static int s_numberOfRotationSteps;

 protected:
  int m_nvert, m_nface;
  HVPoint3D *m_pV;
  SbFacet    *m_pF;

  // Allocate memory for SbPolyhedron
  void AllocateMemory(int Nvert, int Nface);

  // Find neighbouring facet
  int FindNeighbour(int iFace, int iNode, int iOrder) const;

  // Find normal at node
  HVNormal3D FindNodeNormal(int iFace, int iNode) const;

  // Create SbPolyhedron for prism with quadrilateral base
  void CreatePrism();

  // Generate facets by revolving an edge around Z-axis
  void RotateEdge(int k1, int k2, double r1, double r2,
                  int v1, int v2, int vEdge,
                  bool ifWholeCircle, int ns, int &kface);

  // Set side facets for the case of incomplete rotation
  void SetSideFacets(int ii[4], int vv[4],
                     int *kk, double *r,
                     double dphi, int ns, int &kface);

  // Create SbPolyhedron for body of revolution around Z-axis
  void RotateAroundZ(int nstep, double phi, double dphi,
                     int np1, int np2,
                     const double *z, double *r,
                     int nodeVis, int edgeVis);

  // For each edge set reference to neighbouring facet
  void SetReferences();

  // Invert the order on nodes in facets
  void InvertFacets();

 public:
  // Constructor
  SbPolyhedron(int Nvert=0, int Nface=0)
    : m_nvert(Nvert), m_nface(Nface),
      m_pV(Nvert ? new HVPoint3D[Nvert+1] : 0),
      m_pF(Nface ? new SbFacet[Nface+1] : 0) {}

  // Copy constructor
  SbPolyhedron(const SbPolyhedron & from);

  // Destructor
  virtual ~SbPolyhedron() { delete [] m_pV; delete [] m_pF; }

  // Assignment
  virtual SbPolyhedron & operator=(const SbPolyhedron & from);

  // Get number of vertices
  int GetNoVertices() const { return m_nvert; }

  // Get number of facets
  int GetNoFacets() const { return m_nface; }

  // rbianchi change
  //- original version
  // Transform the polyhedron
  //SbPolyhedron & Transform(const HVRotation &rot, const HVVector3D & trans);
  //- new version from newest OpenScientist/HEPVis
  //G.Barrand : the same than upper but by using doubles.
  SbPolyhedron& Transform(const HEPVis::SbRotation& rot,
                            const SbVec3d& trans);
  //---


  // Get next vertex index of the quadrilateral
  bool GetNextVertexIndex(int & index, int & edgeFlag) const;

  // Get vertex by index
  HVPoint3D GetVertex(int index) const;
  // rbianchi - 14.12.2012
  const HVPoint3D& GetVertexFast(int index) const; //G.Barrand
  //---

  // Get next vertex + edge visibility of the quadrilateral
  bool GetNextVertex(HVPoint3D & vertex, int & edgeFlag) const;

  // Get next vertex + edge visibility + normal of the quadrilateral
  bool GetNextVertex(HVPoint3D & vertex, int & edgeFlag,
                           HVNormal3D & normal) const;

  // Get indeces of the next edge with indeces of the faces
  bool GetNextEdgeIndeces(int & i1, int & i2, int & edgeFlag,
                                int & iface1, int & iface2) const;

  // Get indeces of the next edge
  bool GetNextEdgeIndeces(int & i1, int & i2, int & edgeFlag) const;

  // Get next edge
  bool GetNextEdge(HVPoint3D &p1, HVPoint3D &p2, int &edgeFlag) const;

  // Get next edge
  bool GetNextEdge(HVPoint3D &p1, HVPoint3D &p2, int &edgeFlag,
                         int &iface1, int &iface2) const;

  // Get face by index
  void GetFacet(int iFace, int &n, int *iNodes,
                int *edgeFlags = 0, int *iFaces = 0) const;

  // Get face by index
  void GetFacet(int iFace, int &n, HVPoint3D *nodes,
                int *edgeFlags = 0, HVNormal3D *normals = 0) const;

  // Get next face with normals at the nodes
  bool GetNextFacet(int &n, HVPoint3D *nodes,
                          int *edgeFlags=0, HVNormal3D *normals=0) const;

  // Get normal of the face given by index
  HVNormal3D GetNormal(int iFace) const;

  // Get unit normal of the face given by index
  HVNormal3D GetUnitNormal(int iFace) const;

  // Get normal of the next face
  bool GetNextNormal(HVNormal3D &normal) const;

  // Get normal of unit length of the next face
  bool GetNextUnitNormal(HVNormal3D &normal) const;

  // Boolean operations
  SbPolyhedron add(const SbPolyhedron &p) const;
  SbPolyhedron subtract(const SbPolyhedron &p) const;
  SbPolyhedron intersect(const SbPolyhedron &p) const;

  // Get area of the surface of the polyhedron
  double GetSurfaceArea() const;

  // Get volume of the polyhedron
  double GetVolume() const;

  // Get number of steps for whole circle
  static int GetNumberOfRotationSteps() { return s_numberOfRotationSteps; }

  // Set number of steps for whole circle
  static void SetNumberOfRotationSteps(int n);

  // Reset number of steps for whole circle to default value
  static void ResetNumberOfRotationSteps() {
    s_numberOfRotationSteps = DEFAULT_NUMBER_OF_STEPS;
  }
};

//TK-deleteclass SbPolyhedronTest : public SbPolyhedron {
//TK-delete public:
//TK-delete  SbPolyhedronTest();
//TK-delete  virtual ~SbPolyhedronTest();
//TK-delete  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
//TK-delete    return SbPolyhedron::operator = (from);
//TK-delete  }
//TK-delete};
//TK-delete

class SbPolyhedronTrd2 : public SbPolyhedron {
 public:
  SbPolyhedronTrd2(double Dx1, double Dx2,
                    double Dy1, double Dy2, double Dz);
  virtual ~SbPolyhedronTrd2();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronTrd1 : public SbPolyhedronTrd2 {
 public:
  SbPolyhedronTrd1(double Dx1, double Dx2,
                    double Dy, double Dz);
  virtual ~SbPolyhedronTrd1();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronBox : public SbPolyhedronTrd2 {
 public:
  SbPolyhedronBox(double Dx, double Dy, double Dz);
  virtual ~SbPolyhedronBox();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronTrap : public SbPolyhedron {
public:
  SbPolyhedronTrap(double Dz, double Theta, double Phi,
                    double Dy1,
                    double Dx1, double Dx2, double Alp1,
                    double Dy2,
                    double Dx3, double Dx4, double Alp2);
  virtual ~SbPolyhedronTrap();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronPara : public SbPolyhedronTrap {
public:
  SbPolyhedronPara(double Dx, double Dy, double Dz,
                    double Alpha, double Theta, double Phi);
  virtual ~SbPolyhedronPara();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronCons : public SbPolyhedron {
public:
  SbPolyhedronCons(double Rmn1, double Rmx1,
                    double Rmn2, double Rmx2, double Dz,
                    double Phi1, double Dphi);
  virtual ~SbPolyhedronCons();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronCone : public SbPolyhedronCons {
public:
  SbPolyhedronCone(double Rmn1, double Rmx1,
                    double Rmn2, double Rmx2, double Dz);
  virtual ~SbPolyhedronCone();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronTubs : public SbPolyhedronCons {
public:
  SbPolyhedronTubs(double Rmin, double Rmax, double Dz,
                    double Phi1, double Dphi);
  virtual ~SbPolyhedronTubs();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronTube : public SbPolyhedronCons {
public:
  SbPolyhedronTube (double Rmin, double Rmax, double Dz);
  virtual ~SbPolyhedronTube();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronPgon : public SbPolyhedron {
public:
  SbPolyhedronPgon(double phi, double dphi, int npdv, int nz,
                    const double *z,
                    const double *rmin,
                    const double *rmax);
  virtual ~SbPolyhedronPgon();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronPcon : public SbPolyhedronPgon {
public:
  SbPolyhedronPcon(double phi, double dphi, int nz,
                    const double *z,
                    const double *rmin,
                    const double *rmax);
  virtual ~SbPolyhedronPcon();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronSphere : public SbPolyhedron {
public:
  SbPolyhedronSphere(double rmin, double rmax,
                      double phi, double dphi,
                      double the, double dthe);
  virtual ~SbPolyhedronSphere();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronTorus : public SbPolyhedron {
public:
  SbPolyhedronTorus(double rmin, double rmax, double rtor,
                    double phi, double dphi);
  virtual ~SbPolyhedronTorus();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

#include<vector>
class SbPolyhedronPolygonXSect : public SbPolyhedron {

  // Added March 2007 by T. Kittelmann (Thomas.Kittelmann@cern.ch).

public:
  SbPolyhedronPolygonXSect(const std::vector<double>& x,const std::vector<double>& y, const double& dz);
  //fixme: add interface with plain c arrays instead of vectors?
virtual ~SbPolyhedronPolygonXSect();

  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
protected:
  class Internals;
  friend class Internals;
};

// Class SbPolyhedronArbitrary for tessellated solids inside boolean operations
class SbPolyhedronArbitrary : public SbPolyhedron {

 public:
  SbPolyhedronArbitrary(const int nVertices, const int nFacets);
  virtual ~SbPolyhedronArbitrary();

  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }

  void AddVertex(const double v1, const double v2, const double v3);
  void AddFacet(const int iv1, const int iv2, const int iv3, const int iv4=0);

  void Finalize();

 protected:
  int m_nVertexCount;
  int m_nFacetCount;
};

class SbPolyhedronGenericTrap : public SbPolyhedron {
public:
  SbPolyhedronGenericTrap(double Dz, const std::vector<std::pair<double,double> > Vertices);
  virtual ~SbPolyhedronGenericTrap();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

#endif
