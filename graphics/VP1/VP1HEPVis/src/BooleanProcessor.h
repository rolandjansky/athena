/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PLEASE NOTE: This file has been copied and pasted from GEANT4, rel. 4.9.6.cand00
 *
 * original source file:
 * /afs/cern.ch/sw/geant4/releases/geant4.9.6.cand00/share/source/graphics_reps/src/BooleanProcessor.src
 *
 *
 * Update: Riccardo-Maria BIANCHI <rbianchi@cern.ch> - 2012-11-16
 *
 *
 * NOTES:
 *
 * ===================
 * VP1 Customization:
 *
 *  - look into the code for comments "// VP1 change"
 *
 *
*/


/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor                            Date:    10.12.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Internal class for executing boolean operations           *
 *           on Polyhedra                                              *
 *                                                                     *
 ***********************************************************************/




/* // VP1 change
//G.Barrand : begin
#define BP_GEANT4
*/

#ifdef BP_GEANT4 //G.Barrand
#include "G4Plane3D.hh"
#include "G4Point3D.hh"
#include "G4Normal3D.hh"
typedef G4Plane3D HVPlane3D;
typedef G4Point3D HVPoint3D;
typedef G4Normal3D HVNormal3D;

#else //BP_HEPVIS

#define ExtNode          HEPVis_ExtNode
#define ExtEdge          HEPVis_ExtEdge
#define ExtFace          HEPVis_ExtFace
#define FaceList         HEPVis_FaceList
#define ExtPolyhedron    HEPVis_ExtPolyhedron
#define BooleanProcessor HEPVis_BooleanProcessor

#define HepPolyhedron SbPolyhedron
#define G4Facet SbFacet

// VP1 change
//#include <HEPVis/SbPlane.h>
#include <VP1HEPVis/SbPlane.h>
typedef HEPVis::SbPlane HVPlane3D;
//---


#endif




//using namespace HepGeom;

//#define BP_DEBUG

//G.Barrand : end

#define INITIAL_SIZE 200
#define CRAZY_POINT  HVPoint3D(-10.e+6, -10.e+6, -10.e+6)
//#define GRANULARITY  10.e+5;
#define GRANULARITY  10.e+5  //G.Barrand : rm the trailing ;

#define SWAP(A,B) w = A; A = B; B = w

#define OP_UNION         0    // Operations
#define OP_INTERSECTION  1
#define OP_SUBTRACTION   2

#define OUT_OF_PLANE     0    // Face vs face statuses
#define ON_PLANE         1
#define INTERSECTION     2
#define EDGE             3
#define NON_PLANAR_FACE  4

#define UNKNOWN_FACE     0    // Face statuses
#define ORIGINAL_FACE   -1
#define NEW_FACE        -2
#define UNSUITABLE_FACE -3
#define DEFECTIVE_FACE  -4



// -------------------------------------------- Simplified STL vector ---
//G.Barrand : begin
#include <vector>
using namespace std;
/*
template<class T>
class vector {
 private:
  int cur_size, max_size;
  T * v;

 public:
  vector(): cur_size(0), max_size(INITIAL_SIZE), v(new T[INITIAL_SIZE]) {}
  ~vector() { delete [] v; }

  void      clear()                 { cur_size = 0; }
  int       size()            const { return cur_size; }
  T &       operator[](int i)       { return v[i]; }
  const T & operator[](int i) const { return v[i]; }
  T &       front()                 { return v[0]; }
  const T & front()           const { return v[0]; }
  T &       back()                  { return v[cur_size-1]; }
  const T & back()            const { return v[cur_size-1]; }
  void      pop_back()              { cur_size--; }
  void      push_back(const T & a) {
    if (cur_size == max_size) {
      T * w     = v;
      max_size *= 2;
      v         = new T[max_size];
      for (int i=0; i<cur_size; i++) v[i] = w[i];
      v[cur_size++] = a;
      delete [] w;
    }else{
      v[cur_size++] = a;
    }
  }
};
*/
//G.Barrand : end

// ---------------------------------------------------- Extended node ---
class ExtNode {
 public:
  HVPoint3D v;
  int        s;

 public:
  ExtNode(HVPoint3D vertex=HVPoint3D(), int status=0)
    : v(vertex), s(status) {}
  ~ExtNode() {}

  ExtNode(const ExtNode & node) : v(node.v), s(node.s) {}

  ExtNode & operator=(const ExtNode & node) {
    if (&node == this) return *this;
    v = node.v;
    s = node.s;
    return *this;
  }
};

// ---------------------------------------------------- Extended edge ---
class ExtEdge {
 public:
  int       i1, i2;           // end points
  int       iface1;           // native face
  int       iface2;           // neighbouring face
  int       ivis;             // visibility: +1 (visible), -1 (invisible)
  int       inext;            // index of next edge

 public:
  ExtEdge(int k1=0, int k2=0, int kface1=0, int kface2=0, int kvis=0) :
    i1(k1), i2(k2), iface1(kface1), iface2(kface2), ivis(kvis), inext(0) {}

  ~ExtEdge() {};

  ExtEdge(const ExtEdge & edge) :
    i1(edge.i1), i2(edge.i2), iface1(edge.iface1), iface2(edge.iface2),
    ivis(edge.ivis), inext(edge.inext) {}

  ExtEdge & operator=(const ExtEdge & edge) {
    if (&edge == this) return *this;
    i1     = edge.i1;
    i2     = edge.i2;
    iface1 = edge.iface1;
    iface2 = edge.iface2;
    ivis   = edge.ivis;
    inext  = edge.inext;
    return *this;
  }

  void invert() {
    int w;
    SWAP(i1, i2);
  }
};

// ---------------------------------------------------- Extended face ---
class ExtFace {
 private:
  std::vector<ExtEdge>& m_edges; //G.Barrand  // VP1 change
 public:
  int        iedges[4];        // indices of original edges
  HVPlane3D plane;            // face plane
  double     rmin[3], rmax[3]; // bounding box
  int        iold;             // head of the list of the original edges
  int        inew;             // head of the list of the new edges
  int        iprev;            // index of previous face
  int        inext;            // index of next face

 public:
  //G.Barrand : ExtFace(int iedge=0) : iold(iedge), inew(0), iprev(iprev), inext(0) {}
  ExtFace(std::vector<ExtEdge>& a_edges,int iedge)
  : m_edges(a_edges), iold(iedge), inew(0), iprev(0), inext(0) {
    //G.Barrand : initialize arrays to quiet valgrind.
   {for (int i=0; i<4; i++) { iedges[i] = 0; }}
   {for (int i=0; i<3; i++) { rmin[i] = 0; rmax[i] = 0; }}
  }
  ~ExtFace() {}

  ExtFace(const ExtFace & face) :
    m_edges(face.m_edges), //G.Barrand
    plane(face.plane), iold(face.iold), inew(face.inew),
    iprev(face.iprev), inext(face.inext)
  {
    int i;
    for (i=0; i<4; i++) { iedges[i] = face.iedges[i]; }
    for (i=0; i<3; i++) { rmin[i] = face.rmin[i]; rmax[i] = face.rmax[i]; }
  }

  ExtFace & operator=(const ExtFace & face) {
    if (&face == this) return *this;
    //FIXME : edges(face.edges) ???? //G.Barrand
    int i;
    for (i=0; i<4; i++) { iedges[i] = face.iedges[i]; }
    plane  = face.plane;
    for (i=0; i<3; i++) { rmin[i] = face.rmin[i]; rmax[i] = face.rmax[i]; }
    iold   = face.iold;
    inew   = face.inew;
    iprev  = face.iprev;
    inext  = face.inext;
    return *this;
  }

  void invert();
};

// ---------------------------------------------------- Global arrays ---
//G.Barrand : MacIntel : crash with g++-4.0.1 with -O on some subtract.
//            Anyway static of objects is proved to be not safe.
//            We put the below vector as members of BooleanProcessor.
//GB static std::vector<ExtNode> nodes;        // vector of nodes
//GB static std::vector<ExtEdge> edges;        // vector of edges
//GB static std::vector<ExtFace> faces;        // vector of faces

// ---------------------------------------------------- List of faces ---
class FaceList {
 private:
  std::vector<ExtFace>& m_faces; //G.Barrad : end  // VP1 change
 private:
  int m_ihead;
  int m_ilast;

 public:
  //G.Barrand : FaceList() : ihead(0), ilast(0) {}
  FaceList(std::vector<ExtFace>& a_faces) : m_faces(a_faces),m_ihead(0),m_ilast(0) {}
  ~FaceList() {}

  void clean() { m_ihead = 0; m_ilast = 0; }
  int front()  { return m_ihead; }

  void push_back(int i) {
    if (m_ilast == 0) { m_ihead = i; } else { m_faces[m_ilast].inext = i; }
    ExtFace& face = m_faces[i]; //G.Barrand : optimize.
    face.iprev = m_ilast;
    face.inext = 0;
    m_ilast = i;
  }

  void remove(int i) {
    ExtFace& face = m_faces[i]; //G.Barrand : optimize.
    if (m_ihead == i) {
      m_ihead = face.inext;
    }else{
      m_faces[face.iprev].inext = face.inext;
    }
    if (m_ilast == i) {
      m_ilast = face.iprev;
    }else{
      m_faces[face.inext].iprev = face.iprev;
    }
    face.iprev = 0;
    face.inext = 0;
  }
};

// --------------------- Polyhedron with extended access to
//                       its members from the BooleanProcessor class ---
class ExtPolyhedron : public HepPolyhedron {
  friend class BooleanProcessor;
  virtual HepPolyhedron& operator = (const HepPolyhedron& from) {
    return HepPolyhedron::operator = (from);
  }
};

// ----------------------------------------- Boolean processor class ---
class BooleanProcessor {
 private:
  static int s_ishift; //G.Barrand // VP1 change
  std::vector<ExtNode> m_nodes;        // vector of nodes //G.Barrand // VP1 change
  std::vector<ExtEdge> m_edges;        // vector of edges //G.Barrand // VP1 change
  std::vector<ExtFace> m_faces;        // vector of faces //G.Barrand // VP1 change
 private:
  int             m_processor_error;   // is set in case of error // VP1 change
  int             m_operation;  // 0 (union), 1 (intersection), 2 (subtraction) // VP1 change
  int             m_ifaces1, m_ifaces2;  // lists of faces // VP1 change
  int             m_iout1,   m_iout2;    // lists of faces with status "out" // VP1 change
  int             m_iunk1,   m_iunk2;    // lists of faces with status "unknown" // VP1 change
  double          m_rmin[3], m_rmax[3];  // intersection of bounding boxes // VP1 change
  double          m_del;               // precision (tolerance) // VP1 change

  FaceList        m_result_faces;      // list of accepted faces // VP1 change
  FaceList        m_suitable_faces;    // list of suitable faces // VP1 change
  FaceList        m_unsuitable_faces;  // list of unsuitable faces // VP1 change
  FaceList        m_unknown_faces;     // list of unknown faces // VP1 change

  vector<int>     m_external_contours; // heads of external contours // VP1 change
  vector<int>     m_internal_contours; // heads of internal contours // VP1 change

 private:
  void   takePolyhedron(const HepPolyhedron & p, double, double, double);
  double findMinMax();
  void   selectOutsideFaces(int & ifaces, int & iout);
  int    testFaceVsPlane(ExtEdge & edge);
  void   renumberNodes(int & i1, int & i2, int & i3, int & i4);
  int    testEdgeVsEdge(ExtEdge & edge1, ExtEdge & edge2);
  void   removeJunkNodes() { while(m_nodes.back().s != 0) m_nodes.pop_back(); }
  void   divideEdge(int & i1, int & i2);
  void   insertEdge(const ExtEdge & edge);
  void   caseII(ExtEdge & edge1, ExtEdge & edge2);
  void   caseIE(ExtEdge & edge1, ExtEdge & edge2);
  void   caseEE(ExtEdge & edge1, ExtEdge & edge2);
  void   testFaceVsFace(int iface1, int iface2);
  void   invertNewEdges(int iface);
  void   checkDoubleEdges(int iface);
  void   assembleFace(int what, int iface);
  void   assembleNewFaces(int what, int ihead);
  void   initiateLists();
  void   assemblePolyhedra();
  void   findABC(double x1, double y1, double x2, double y2,
                 double &a, double &b, double &c) const;
  int    checkDirection(double *x, double *y) const;
  int    checkIntersection(int ix, int iy, int i1, int i2) const;
  void   mergeContours(int ix, int iy, int kext, int kint);
  int    checkTriangle(int iedge1, int iedge2, int ix, int iy) const;
  void   triangulateContour(int ix, int iy, int ihead);
  void   modifyReference(int iface, int i1, int i2, int iref);
  void   triangulateFace(int iface);
  HepPolyhedron createPolyhedron();

 public:
  //G.Barrand : BooleanProcessor() {}
  BooleanProcessor() //G.Barrand
  :m_processor_error(0)  // The next few fields are work space, initialised
  ,m_operation(0)        // here to prevent Coverity warnings.
  ,m_ifaces1(0)          // "
  ,m_ifaces2(0)          // "
  ,m_iout1(0)            // "
  ,m_iout2(0)            // "
  ,m_iunk1(0)            // "
  ,m_iunk2(0)            // "
  ,m_del(0.)             // "
  ,m_result_faces(m_faces)
  ,m_suitable_faces(m_faces)
  ,m_unsuitable_faces(m_faces)
  ,m_unknown_faces(m_faces)
  {  // rmin, rmax also initialised here to prevent Coverity warnings.
    for (int i = 0; i < 3; i++) {
      m_rmin[i] = 0.;
      m_rmax[i] = 0.;
    }
  }

  ~BooleanProcessor() {}

  HepPolyhedron execute(int op,
                        const HepPolyhedron &a,
                        const HepPolyhedron &b,
                        int& err);

  void draw();
  void draw_edge(int, int);
  void draw_contour(int, int, int);
  void draw_faces(int, int, int);
  void print_face(int);
  void print_edge(int);
  int get_processor_error() const {return m_processor_error;}

  void dump(); //G.Barrand
  static int get_shift(); //G.Barrand
  static void set_shift(int); //G.Barrand
  static int get_num_shift(); //G.Barrand
};

inline void ExtFace::invert()
/***********************************************************************
 *                                                                     *
 * Name: ExtFace::invert()                           Date:    28.02.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Invert face                                               *
 *                                                                     *
 ***********************************************************************/
{
  int iEprev, iEcur, iEnext;

  iEprev = 0; iEcur = iold;
  while (iEcur > 0) {
    ExtEdge& edge = m_edges[iEcur]; //G.Barrand : optimize.
    edge.invert();
    iEnext = edge.inext;
    edge.inext = iEprev;
    iEprev = iEcur;
    iEcur  = iEnext;
  }
  if (iold > 0) iold = iEprev;

  iEprev = 0; iEcur = inew;
  while (iEcur > 0) {
    ExtEdge& edge = m_edges[iEcur]; //G.Barrand : optimize.
    edge.invert();
    iEnext = edge.inext;
    edge.inext = iEprev;
    iEprev = iEcur;
    iEcur  = iEnext;
  }
  if (inew > 0) inew = iEprev;

#ifdef BP_GEANT4 //G.Barrand
  plane = HVPlane3D(-plane.a(), -plane.b(), -plane.c(), -plane.d());
#else
  plane = HVPlane3D(-plane.getNormal(), -plane.getDistanceFromOrigin());
#endif
}

void BooleanProcessor::takePolyhedron(const HepPolyhedron & p,
                                      double dx, double dy, double dz)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::takePolyhedron            Date:    16.12.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Transfer Polyhedron to internal representation            *
 *                                                                     *
 ***********************************************************************/
{
  int i, k, nnode, iNodes[5], iVis[4], iFaces[4];
  int dnode = m_nodes.size() - 1;
  int dface = m_faces.size() - 1;

  //   S E T   N O D E S

  //  for (i=1; i <= p.GetNoVertices(); i++) {
  //    nodes.push_back(ExtNode(p.GetVertex(i)));
  //  }

  HVPoint3D ppp;
  for (i=1; i <= p.GetNoVertices(); i++) {
#ifdef BP_GEANT4 //G.Barrand
    ppp = p.GetVertex(i);
    ppp.setX(ppp.x()+dx);
    ppp.setY(ppp.y()+dy);
    ppp.setZ(ppp.z()+dz);
#else
    ppp = p.GetVertexFast(i);
    ppp += HVPoint3D(dx,dy,dz);
#endif
    m_nodes.push_back(ExtNode(ppp));
  }

  //   S E T   F A C E S

  for (int iface=1; iface <= p.GetNoFacets(); iface++) {
    m_faces.push_back(ExtFace(m_edges,m_edges.size()));

    //   S E T   F A C E   N O D E S

    p.GetFacet(iface, nnode, iNodes, iVis, iFaces);
    for (i=0; i<nnode; i++) {
      //if (iNodes[i] < 1 || iNodes[i] > p.GetNoVertices()) m_processor_error = 1;
      //if (iFaces[i] < 1 || iFaces[i] > p.GetNoFacets())   m_processor_error = 1;

      if (iNodes[i] < 1 || iNodes[i] > p.GetNoVertices()) { //G.Barrand
        m_processor_error = 1;
#ifdef BP_DEBUG
        G4cerr
          << "BooleanProcessor::takePolyhedron : problem 1."
          << G4endl;
#endif
      }
      if (iFaces[i] < 1 || iFaces[i] > p.GetNoFacets()) { //G.Barrand
        m_processor_error = 1;
#ifdef BP_DEBUG
        G4cerr
          << "BooleanProcessor::takePolyhedron : problem 2."
          << G4endl;
#endif
      }
      iNodes[i] += dnode;
      iFaces[i] += dface;
    }

    //   S E T   E D G E S

    iNodes[nnode] = iNodes[0];
    m_faces.back().iedges[3] = 0;
    for (i=0; i<nnode; i++) {
      m_faces.back().iedges[i] = m_edges.size();
      m_edges.push_back(ExtEdge(iNodes[i], iNodes[i+1],
                                iface+dface, iFaces[i], iVis[i]));
      m_edges.back().inext     = m_edges.size();
    }
    m_edges.back().inext = 0;

    //   S E T   F A C E   M I N - M A X

    ExtFace& face = m_faces.back();     //G.Barrand : optimize.
    for (i=0; i<3; i++) {
      face.rmin[i] = m_nodes[iNodes[0]].v[i];
      face.rmax[i] = m_nodes[iNodes[0]].v[i];
    }
    for (i=1; i<nnode; i++) {
      ExtNode& node = m_nodes[iNodes[i]]; //G.Barrand : optimize.
      for (k=0; k<3; k++) {
        if (face.rmin[k] > node.v[k])
            face.rmin[k] = node.v[k];
        if (face.rmax[k] < node.v[k])
            face.rmax[k] = node.v[k];
      }
    }

    //   S E T   F A C E   P L A N E

    HVNormal3D n = (m_nodes[iNodes[2]].v-m_nodes[iNodes[0]].v).cross
                        (m_nodes[iNodes[3]].v-m_nodes[iNodes[1]].v);
    HVPoint3D  point(0,0,0);

    for (i=0; i<nnode; i++) { point += m_nodes[iNodes[i]].v; }
    if (nnode > 1) point *= (1./nnode);


    //G.Barrand : faces.back().plane = HVPlane3D(n.unit(), point);
    m_faces.back().plane = HVPlane3D(n, point); //G.Barrand

    //   S E T   R E F E R E N C E   T O   T H E   N E X T   F A C E

    m_faces.back().inext = m_faces.size();
  }
  m_faces.back().inext = 0;
}

double BooleanProcessor::findMinMax()
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::findMinMax                Date:    16.12.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Find min-max (bounding) boxes for polyhedra               *
 *                                                                     *
 ***********************************************************************/
{
  if (m_ifaces1 == 0 || m_ifaces2 == 0) return 0;

  int    i, iface;
  double rmin1[3], rmax1[3];
  double rmin2[3], rmax2[3];

  //   F I N D   B O U N D I N G   B O X E S

  for (i=0; i<3; i++) {
    rmin1[i] = m_faces[m_ifaces1].rmin[i];
    rmax1[i] = m_faces[m_ifaces1].rmax[i];
    rmin2[i] = m_faces[m_ifaces2].rmin[i];
    rmax2[i] = m_faces[m_ifaces2].rmax[i];
  }

  iface = m_faces[m_ifaces1].inext;
  while(iface > 0) {
    ExtFace& face = m_faces[iface]; //G.Barrand
    for (i=0; i<3; i++) {
      if (rmin1[i] > face.rmin[i]) rmin1[i] = face.rmin[i];
      if (rmax1[i] < face.rmax[i]) rmax1[i] = face.rmax[i];
    }
    iface = face.inext;
  }

  iface = m_faces[m_ifaces2].inext;
  while(iface > 0) {
    ExtFace& face = m_faces[iface]; //G.Barrand
    for (i=0; i<3; i++) {
      if (rmin2[i] > face.rmin[i]) rmin2[i] = face.rmin[i];
      if (rmax2[i] < face.rmax[i]) rmax2[i] = face.rmax[i];
    }
    iface = face.inext;
  }

  //   F I N D   I N T E R S E C T I O N   O F   B O U N D I N G   B O X E S

  for (i=0; i<3; i++) {
    m_rmin[i] = (rmin1[i] > rmin2[i]) ? rmin1[i] : rmin2[i];
    m_rmax[i] = (rmax1[i] < rmax2[i]) ? rmax1[i] : rmax2[i];
  }

  //   F I N D   T O L E R A N C E

  double del1 = 0;
  double del2 = 0;
  for (i=0; i<3; i++) {
    if ((rmax1[i]-rmin1[i]) > del1) del1 = rmax1[i]-rmin1[i];
    if ((rmax2[i]-rmin2[i]) > del2) del2 = rmax2[i]-rmin2[i];
  }
  return ((del1 < del2) ? del1 : del2) / GRANULARITY;
}

void BooleanProcessor::selectOutsideFaces(int & ifaces, int & iout)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::selectOutsideFaces        Date:    10.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Preselection of outside faces                             *
 *                                                                     *
 ***********************************************************************/
{
  int i, outflag, iface = ifaces, *prev;
  HVPoint3D mmbox[8] = { HVPoint3D(m_rmin[0],m_rmin[1],m_rmin[2]),
                               HVPoint3D(m_rmax[0],m_rmin[1],m_rmin[2]),
                               HVPoint3D(m_rmin[0],m_rmax[1],m_rmin[2]),
                               HVPoint3D(m_rmax[0],m_rmax[1],m_rmin[2]),
                               HVPoint3D(m_rmin[0],m_rmin[1],m_rmax[2]),
                               HVPoint3D(m_rmax[0],m_rmin[1],m_rmax[2]),
                               HVPoint3D(m_rmin[0],m_rmax[1],m_rmax[2]),
                               HVPoint3D(m_rmax[0],m_rmax[1],m_rmax[2]) };
  prev = &ifaces;
  while (iface > 0) {

    //   B O U N D I N G   B O X   vs  B O U N D I N G   B O X

    outflag = 0;
    ExtFace& face = m_faces[iface]; //G.Barrand : optimize.
    for (i=0; i<3; i++) {
      if (face.rmin[i] > m_rmax[i] + m_del) { outflag = 1; break; }
      if (face.rmax[i] < m_rmin[i] - m_del) { outflag = 1; break; }
    }

    //   B O U N D I N G   B O X   vs  P L A N E

    if (outflag == 0) {
      int npos = 0, nneg = 0;
      double d;
      for (i=0; i<8; i++) {
        d = face.plane.distance(mmbox[i]); //G.Barrand : optimize
        if (d > +m_del) npos++;
        if (d < -m_del) nneg++;
      }
      if (npos == 8 || nneg == 8) outflag = 1;
    }

    //   U P D A T E   L I S T S

    if (outflag == 1) {
      *prev = face.inext;
      face.inext = iout;
      iout = iface;
    }else{
      prev = &face.inext;
    }

    iface = *prev;
  }
}

int BooleanProcessor::testFaceVsPlane(ExtEdge & edge)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::testFaceVsPlane           Date:    19.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Find result of intersection of face by plane              *
 *                                                                     *
 ***********************************************************************/
{
  int        iface = edge.iface1;
  HVPlane3D plane = m_faces[edge.iface2].plane;
  int        i, nnode, npos = 0, nneg = 0, nzer = 0;
  double     dd[5];

  //   F I N D   D I S T A N C E S

  nnode = (m_faces[iface].iedges[3] == 0) ? 3 : 4;
  for (i=0; i<nnode; i++) {
    dd[i] = plane.distance(m_nodes[m_edges[m_faces[iface].iedges[i]].i1].v);
    if (dd[i] > m_del) {
      npos++;
    }else if (dd[i] < -m_del) {
      nneg++;
    }else{
      nzer++; dd[i] = 0;
    }
  }

  //   S O M E   S I M P L E   C A S E S  ( N O   I N T E R S E C T I O N )

  if (npos == nnode || nneg == nnode)   return OUT_OF_PLANE;
  if (nzer == 1     && nneg == 0)       return OUT_OF_PLANE;
  if (nzer == 1     && npos == 0)       return OUT_OF_PLANE;
  if (nzer == nnode)                    return ON_PLANE;
  if (nzer == 3)                        return NON_PLANAR_FACE;

  //   F I N D   I N T E R S E C T I O N

  int       ie1 = 0, ie2 = 0, s1 = 0, s2 = 0, status, nint = 0;
  enum      { PLUS_MINUS, MINUS_PLUS, ZERO_ZERO, ZERO_PLUS, ZERO_MINUS };

  dd[nnode] = dd[0];
  for (i=0; i<nnode; i++) {
    if (dd[i] > 0) {
      if (dd[i+1] >= 0) continue;
      status = PLUS_MINUS;
    }else if (dd[i] < 0) {
      if (dd[i+1] <= 0) continue;
      status = MINUS_PLUS;
    }else{
      status = ZERO_ZERO;
      if (dd[i+1] > 0) status = ZERO_PLUS;
      if (dd[i+1] < 0) status = ZERO_MINUS;
    }
    switch (nint) {
    case 0:
      ie1 = i; s1 = status; nint++; break;
    case 1:
      ie2 = i; s2 = status; nint++; break;
    default:
      return NON_PLANAR_FACE;
    }
  }
  if (nint != 2)                        return NON_PLANAR_FACE;

  //   F O R M   I N T E R S E C T I O N   S E G M E N T

  if (s1 != ZERO_ZERO && s2 != ZERO_ZERO) {
    if (s1 == s2)                       return NON_PLANAR_FACE;
    int     iedge, i1 = 0, i2 = 0, ii[2];
    double  d1 = 0., d2 = 0., d3 = 0.;
    ii[0] = ie1; ii[1] = ie2;
    for (i=0; i<2; i++) {
      iedge = m_faces[iface].iedges[ii[i]];
      while (iedge > 0) {
        i1 = m_edges[iedge].i1;
        i2 = m_edges[iedge].i2;

        d1 = plane.distance(m_nodes[i1].v);
        d2 = plane.distance(m_nodes[i2].v);
        if (d1 > m_del) {
          if (d2 < -m_del) { ii[i] = m_nodes.size(); break; } // +-
        }else if (d1 < -m_del) {
          if (d2 >  m_del) { ii[i] = m_nodes.size(); break; } // -+
        }else{
          ii[i] = i1; break;                              // 0+ or 0-
        }
        iedge = m_edges[iedge].inext;
      }
      if (ii[i] == (int)m_nodes.size()) {
        d3 = d2-d1; d1 = d1/d3; d2 = d2/d3;
        m_nodes.push_back(ExtNode(d2*m_nodes[i1].v-d1*m_nodes[i2].v, iedge));
      }
    }
    edge.inext = 0;
    if (s1 == MINUS_PLUS || s1 == ZERO_PLUS) {
      edge.i1 = ii[1];
      edge.i2 = ii[0];
    }else{
      edge.i1 = ii[0];
      edge.i2 = ii[1];
    }
    return INTERSECTION;
  }else{
    if (npos == nneg)                   return NON_PLANAR_FACE;
    edge.inext = (s1 == ZERO_ZERO) ? ie1+1 : ie2+1;
    if (s1 == ZERO_PLUS || s2 == ZERO_MINUS) {
      edge.i1 = m_edges[m_faces[iface].iedges[ie2]].i1;
      edge.i2 = m_edges[m_faces[iface].iedges[ie1]].i1;
    }else{
      edge.i1 = m_edges[m_faces[iface].iedges[ie1]].i1;
      edge.i2 = m_edges[m_faces[iface].iedges[ie2]].i1;
    }
    return EDGE;
  }
}

void BooleanProcessor::renumberNodes(int & i1, int & i2, int & i3, int & i4)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::renumberNodes             Date:    19.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Renumber nodes and remove last temporary node.            *
 *           Remark: In principal this routine can be replaced just    *
 *           with i1 = i2;                                             *
 *           Removal of temporary nodes provides additional control    *
 *           on number of nodes, that is very useful for debugging.    *
 *                                                                     *
 ***********************************************************************/
{
  if (i1 == i2) return;
  if (m_nodes[i1].s == 0 || m_nodes.back().s == 0) { i1 = i2; return; }

  int ilast = m_nodes.size()-1;
  if (i1 == ilast) { i1 = i2; m_nodes.pop_back(); return; }
  if (i2 == ilast) { i2 = i1; }
  if (i3 == ilast) { i3 = i1; }
  if (i4 == ilast) { i4 = i1; }
  m_nodes[i1] = m_nodes.back(); i1 = i2; m_nodes.pop_back();
}

int BooleanProcessor::testEdgeVsEdge(ExtEdge & edge1, ExtEdge & edge2)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::testEdgeVsEdge            Date:    19.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Find common part of two edges                             *
 *                                                                     *
 ***********************************************************************/
{
  int    i, ii = 0;
  double d, dd = 0.;

  for (i=0; i<3; i++) {
    d = m_nodes[edge1.i1].v[i]-m_nodes[edge1.i2].v[i];
    if (d < 0.) d = -d;
    if (d > dd) { dd = d; ii = i; }
  }
  double t1 = m_nodes[edge1.i1].v[ii];
  double t2 = m_nodes[edge1.i2].v[ii];
  double t3 = m_nodes[edge2.i1].v[ii];
  double t4 = m_nodes[edge2.i2].v[ii];
  if (t2-t1 < 0.) { t1 = -t1; t2 = -t2; t3 = -t3; t4 = -t4; }

  if (t3 <= t1+m_del || t4 >= t2-m_del) return 0;
  if (t3 > t2+m_del) {
    renumberNodes(edge2.i1, edge1.i2, edge1.i1, edge2.i2);
  }else if (t3 < t2-m_del) {
    renumberNodes(edge1.i2, edge2.i1, edge1.i1, edge2.i2);
  }
  if (t4 < t1-m_del) {
    renumberNodes(edge2.i2, edge1.i1, edge1.i2, edge2.i1);
  }else if (t4 > t1+m_del) {
    renumberNodes(edge1.i1, edge2.i2, edge1.i2, edge2.i1);
  }
  return 1;
}

void BooleanProcessor::divideEdge(int & i1, int & i2)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::divideEdge                Date:    24.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Unify the nodes and divide edge on two parts by the node. *
 *                                                                     *
 ***********************************************************************/
{
  int iedges[2];
  iedges[0] = m_nodes[i1].s;
  iedges[1] = m_nodes[i2].s;

  //   U N I F Y   N O D E S

  if      (i1 < i2) { i2 = i1; }
  else if (i1 > i2) { i1 = i2; }
  else              { iedges[1] = 0; }
  if (iedges[0] == iedges[1]) return;

  int ie1, ie2, inode = i1;
  m_nodes[inode].s = 0;
  for (int i=0; i<2; i++) {

    //   F I N D   C O R R E S P O N D I N G   E D G E

    if ((ie1 = iedges[i]) == 0) continue;
    ie2 = m_faces[m_edges[ie1].iface2].iedges[0];
    while (ie2 > 0) {
      if (m_edges[ie2].i1 == m_edges[ie1].i2 &&
          m_edges[ie2].i2 == m_edges[ie1].i1) break;
      ie2 = m_edges[ie2].inext;
    }

    //   D I V I D E   E D G E S

    m_edges.push_back(m_edges[ie1]);
    m_edges[ie1].inext = m_edges.size() - 1;
    m_edges[ie1].i2    = inode;
    m_edges.back().i1  = inode;

    m_edges.push_back(m_edges[ie2]);
    m_edges[ie2].inext = m_edges.size() - 1;
    m_edges[ie2].i2    = inode;
    m_edges.back().i1  = inode;
  }
}

void BooleanProcessor::insertEdge(const ExtEdge & edge)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::insertEdge                Date:    24.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Insert edge to the list of new edges                      *
 *                                                                     *
 ***********************************************************************/
{
  int iface = edge.iface1;
  m_edges.push_back(edge);
  m_edges.back().inext = m_faces[iface].inew;
  m_faces[iface].inew  = m_edges.size() - 1;
}

void BooleanProcessor::caseII(ExtEdge & edge1, ExtEdge & edge2)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::caseII                    Date:    19.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Intersection/Intersection case                            *
 *                                                                     *
 ***********************************************************************/
{
  divideEdge(edge1.i1, edge2.i2);
  divideEdge(edge1.i2, edge2.i1);
  edge1.ivis = 1;
  edge2.ivis = 1;
  insertEdge(edge1);
  insertEdge(edge2);
}

void BooleanProcessor::caseIE(ExtEdge &, ExtEdge &)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::caseIE                    Date:    19.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Intersection/Edge-touch case                              *
 *                                                                     *
 ***********************************************************************/
{
  m_processor_error = 1;
#ifdef BP_DEBUG
  G4cout
    << "BooleanProcessor::caseIE : unimplemented case"
    << G4endl;
#endif
}

void BooleanProcessor::caseEE(ExtEdge &, ExtEdge &)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::caseEE                    Date:    19.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Edge-touch/Edge-touch case                                *
 *                                                                     *
 ***********************************************************************/
{
  m_processor_error = 1;
#ifdef BP_DEBUG
  G4cout
    << "BooleanProcessor::caseEE : unimplemented case"
    << G4endl;
#endif
}

void BooleanProcessor::testFaceVsFace(int iface1, int iface2)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::testFaceVsFace            Date:    11.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Find result (an edge) of intersection of two faces        *
 *                                                                     *
 ***********************************************************************/
{
  ExtEdge edge1, edge2;
  int     irep1, irep2;

  //   M I N - M A X

 {const ExtFace& face_1 = m_faces[iface1]; //G.Barrand : optimize
  const ExtFace& face_2 = m_faces[iface2];
  for (int i=0; i<3; i++) {
    if (face_1.rmin[i] > face_2.rmax[i] + m_del) return;
    if (face_1.rmax[i] < face_2.rmin[i] - m_del) return;
  }}

  //   F A C E - 1   vs   P L A N E - 2

  edge1.iface1 = iface1;
  edge1.iface2 = iface2;
  irep1        = testFaceVsPlane(edge1);
  if (irep1 == OUT_OF_PLANE || irep1 == ON_PLANE) {
    removeJunkNodes();
    return;
  }

  //   F A C E - 2   vs   P L A N E - 1

  edge2.iface1 = iface2;
  edge2.iface2 = iface1;
  irep2        = testFaceVsPlane(edge2);
  if (irep2 == OUT_OF_PLANE || irep2 == ON_PLANE) {
    removeJunkNodes();
    return;
  }

  //   C H E C K   F O R   N O N P L A N A R   F A C E

  if (irep1 == NON_PLANAR_FACE || irep2 == NON_PLANAR_FACE) {
    removeJunkNodes();
    return;
  }

  //   F I N D   I N T E R S E C T I O N   P A R T

  if (testEdgeVsEdge(edge1, edge2) == 0) return;

  //   C O N S I D E R   D I F F E R E N T   C A S E S

  if (irep1 == INTERSECTION && irep2 == INTERSECTION) caseII(edge1, edge2);
  if (irep1 == INTERSECTION && irep2 == EDGE)         caseIE(edge1, edge2);
  if (irep1 == EDGE         && irep2 == INTERSECTION) caseIE(edge2, edge1);
  if (irep1 == EDGE         && irep2 == EDGE)         caseEE(edge1, edge2);
  removeJunkNodes();

}

void BooleanProcessor::invertNewEdges(int iface)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::invertNewEdges            Date:    04.02.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Invert direction of new edges                             *
 *                                                                     *
 ***********************************************************************/
{
  int iedge = m_faces[iface].inew;
  while (iedge > 0) {
    m_edges[iedge].invert();
    iedge = m_edges[iedge].inext;
  }
}

void BooleanProcessor::checkDoubleEdges(int)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::checkDoubleEdges          Date:    04.02.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Eliminate duplication of edges                            *
 *                                                                     *
 ***********************************************************************/
{

}

void BooleanProcessor::assembleFace(int what, int iface)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::assembleFace              Date:    19.02.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Assemble face                                             *
 *                                                                     *
 ***********************************************************************/
{
  //   A S S E M B L E   N E W   F A C E

  int ihead;      // head of the list of edges for new face
  int icur;       // current edge in the list - last edge inserted to the list
  int *ilink;     // pointer to the current link
  int ifirst;     // first node of a contour
  int *i;         // pointer to the index of the current edge in a loop
  int ioldflag=0; // is set if an edge from iold has been taken

#define INSERT_EDGE_TO_THE_LIST(A) \
*ilink = A; ilink = &m_edges[A].inext; *ilink = 0

  ExtFace& face = m_faces[iface]; //G.Barrand : optimize.
  ilink = &ihead;
  for(;;) {
    if (face.inew == 0) break;

    //   S T A R T   N E W   C O N T O U R

    icur   = face.inew;
    face.inew = m_edges[icur].inext;
    INSERT_EDGE_TO_THE_LIST(icur);
    ifirst = m_edges[icur].i1;

    //   C O N S T R U C T   T H E   C O N T O U R

    for (;;) {
      i = &face.inew;
      ExtEdge& edge_cur = m_edges[icur];
      while(*i > 0) {
        ExtEdge& edge_i = m_edges[*i];
        if (edge_i.i1 == edge_cur.i2) break;
        i = &edge_i.inext;
      }
      if (*i == 0) {
        i = &face.iold;
        while(*i > 0) {
          ExtEdge& edge_i = m_edges[*i];
          if (edge_i.i1 == edge_cur.i2) {
            ioldflag = 1;
            break;
          }
          i = &edge_i.inext;
        }
      }
      if (*i > 0) {
        icur = *i;
        *i = m_edges[icur].inext;
        INSERT_EDGE_TO_THE_LIST(icur);
        if (m_edges[icur].i2 == ifirst) { break; } else { continue; }
      }else{
        m_processor_error = 1;
#ifdef BP_DEBUG
        G4cerr
          << "BooleanProcessor::assembleFace(" << iface << ") : "
          << "could not find next edge of the contour"
          << G4endl;
#endif
        face.inew = DEFECTIVE_FACE;
        return;
      }
    }
  }

  //   C H E C K   O R I G I N A L   C O N T O U R

  int iedge;
  iedge = face.iold;
  if (what == 0 && ioldflag == 0 && iedge > 0) {
    for (;;) {
      if (m_edges[iedge].inext > 0) {
        if (m_edges[iedge].i2 == m_edges[m_edges[iedge].inext].i1) {
          iedge = m_edges[iedge].inext;
        }else{
          break;
        }
      }else{
        if (m_edges[iedge].i2 == m_edges[face.iold].i1) {
          m_edges[iedge].inext = ihead;   // set new face
          return;
        }else{
          break;
        }
      }
    }
  }

  //   M A R K   U N S U I T A B L E   N E I G H B O U R I N G   F A C E S

  int iface2;
  iedge = face.iold;
  while(iedge > 0) {
    iface2 = m_edges[iedge].iface2;
    if (m_faces[iface2].inew == 0) m_faces[iface2].inew = UNSUITABLE_FACE;
    iedge = m_edges[iedge].inext;
  }
  face.iold = ihead;            // set new face
}

void BooleanProcessor::assembleNewFaces(int what, int ihead)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::assembleNewFaces          Date:    30.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Assemble internal or external parts of faces              *
 *                                                                     *
 ***********************************************************************/
{
  int iface = ihead;
  while(iface > 0) {
    if (m_faces[iface].inew > 0) {
      if (what != 0) invertNewEdges(iface);
      checkDoubleEdges(iface);
      assembleFace(what, iface);
      m_faces[iface].inew =
        (m_faces[iface].iold == 0) ? UNSUITABLE_FACE : NEW_FACE;
    }
    iface = m_faces[iface].inext;
  }
}

void BooleanProcessor::initiateLists()
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::initiateLists             Date:    28.02.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Initiate lists of faces.                                  *
 *                                                                     *
 ***********************************************************************/
{
  int i, iface;

  //   R E S E T   L I S T S   O F   F A C E S

  m_result_faces.clean();
  m_suitable_faces.clean();
  m_unsuitable_faces.clean();
  m_unknown_faces.clean();

  //   I N I T I A T E   T H E   L I S T S

  iface = m_iout1;
  while (iface > 0) {
    i     = iface;
    iface = m_faces[i].inext;
    if (m_operation == OP_INTERSECTION) {
      m_unsuitable_faces.push_back(i);
      m_faces[i].inew = UNSUITABLE_FACE;
    }else{
      m_suitable_faces.push_back(i);
      m_faces[i].inew = ORIGINAL_FACE;
    }
  }
  iface = m_iout2;
  while (iface > 0) {
    i     = iface;
    iface = m_faces[i].inext;
    if (m_operation == OP_UNION) {
      m_suitable_faces.push_back(i);
      m_faces[i].inew = ORIGINAL_FACE;
    }else{
      m_unsuitable_faces.push_back(i);
      m_faces[i].inew = UNSUITABLE_FACE;
    }
  }

  iface = m_iunk1;
  while (iface > 0) {
    i     = iface;
    iface = m_faces[i].inext;
    m_unknown_faces.push_back(i);
  }
  iface = m_iunk2;
  while (iface > 0) {
    i     = iface;
    iface = m_faces[i].inext;
    if (m_operation == OP_SUBTRACTION) m_faces[i].invert();
    m_unknown_faces.push_back(i);
  }

  iface = m_ifaces1;
  while (iface > 0) {
    i     = iface;
    iface = m_faces[i].inext;
    switch(m_faces[i].inew) {
    case UNKNOWN_FACE:
      m_unknown_faces.push_back(i);
      break;
    case ORIGINAL_FACE: case NEW_FACE:
      m_suitable_faces.push_back(i);
      break;
    case UNSUITABLE_FACE:
      m_unsuitable_faces.push_back(i);
      break;
    default:
      m_faces[i].iprev = 0;
      m_faces[i].inext = 0;
      break;
    }
  }
  iface = m_ifaces2;
  while (iface > 0) {
    i     = iface;
    iface = m_faces[i].inext;
    if (m_operation == OP_SUBTRACTION) m_faces[i].invert();
    switch(m_faces[i].inew) {
    case UNKNOWN_FACE:
      m_unknown_faces.push_back(i);
      break;
    case ORIGINAL_FACE: case NEW_FACE:
      m_suitable_faces.push_back(i);
      break;
    case UNSUITABLE_FACE:
      m_unsuitable_faces.push_back(i);
      break;
    default:
      m_faces[i].iprev = 0;
      m_faces[i].inext = 0;
      break;
    }
  }
  m_ifaces1 = m_ifaces2 = m_iout1 = m_iout2 = m_iunk1 = m_iunk2 = 0;
}

void BooleanProcessor::assemblePolyhedra()
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::assemblePolyhedra()       Date:    10.12.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Collect suitable faces and remove unsuitable ones.        *
 *                                                                     *
 ***********************************************************************/
{
  int i, iedge, iface;

  //   L O O P   A L O N G   S U I T A B L E   F A C E S

  iface = m_suitable_faces.front();
  while(iface > 0) {
    i = iface;
    iedge = m_faces[i].iold;
    while(iedge > 0) {
      iface = m_edges[iedge].iface2;
      if (m_faces[iface].inew == UNKNOWN_FACE) {
        m_unknown_faces.remove(iface);
        m_suitable_faces.push_back(iface);
        m_faces[iface].inew = ORIGINAL_FACE;
      }
      iedge = m_edges[iedge].inext;
    }
    iface = m_faces[i].inext;
    m_suitable_faces.remove(i);
    m_result_faces.push_back(i);
  }
  if (m_unknown_faces.front() == 0) return;

  //   L O O P   A L O N G   U N S U I T A B L E   F A C E S

  iface = m_unsuitable_faces.front();
  while(iface > 0) {
    i = iface;
    iedge = m_faces[i].iold;
    while(iedge > 0) {
      iface = m_edges[iedge].iface2;
      if (m_faces[iface].inew == UNKNOWN_FACE) {
        m_unknown_faces.remove(iface);
        m_unsuitable_faces.push_back(iface);
        m_faces[iface].inew = UNSUITABLE_FACE;
      }
      iedge = m_edges[iedge].inext;
    }
    iface = m_faces[i].inext;
    m_unsuitable_faces.remove(i);
  }

  //G.Barrand : begin
  /* From S.Ponce
   At last, there is a problem in the assemblePolyhedra method. At least, I
  think it is there. The problem deals with boolean operations on solids,
  when one of the two contains entirely the other one. It has no sense for
  intersection and union but still has sense for subtraction. In this
  case, faces from the inner solid are stored in the m_unknown_faces
  FaceList. And an error occurs in the execute method. This may be correct
  for intersection and union but in the case of subtraction, one should do
  that in assemblePolyhedra :
  */
  //   Unknown faces are actually suitable face !!!
   iface = m_unknown_faces.front();
   while(iface > 0) {
     i = iface;
     m_faces[i].inew = ORIGINAL_FACE;
     iface = m_faces[i].inext;
     m_unknown_faces.remove(i);
     m_result_faces.push_back(i);
   }
  /*
   Otherwise, the inner hole that the second solid was building in the
  first one does not exist. I'm not very clear on what to do for unions
  and intersections. I think this kind of situation should be detected and
  one of the solid should simply be ignored.
  */
  //G.Barrand : end
}

inline void
BooleanProcessor::findABC(double x1, double y1, double x2, double y2,
                          double &a, double &b, double &c) const
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::findABC                   Date:    07.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Find line equation Ax+By+C=0                              *
 *                                                                     *
 ***********************************************************************/
{
  double w;
  a  = y1 - y2;
  b  = x2 - x1;
  //G.Barrand : w  = std::abs(a)+std::abs(b);
  w  = ::fabs(a)+::fabs(b); //G.Barrand
  a /= w;
  b /= w;
  c  = -(a*x2 + b*y2);
}

int BooleanProcessor::checkDirection(double *x, double *y) const
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::checkDirection            Date:    06.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Check direction of line 1-4                               *
 *                                                                     *
 ***********************************************************************/
{
  double a1, b1, c1, a2, b2, c2, d1, d2;

  //   T E S T   L I N E   1 - 4   V S   E X T E R N A L   C O N T O U R

  findABC(x[0], y[0], x[1], y[1], a1, b1, c1);
  findABC(x[1], y[1], x[2], y[2], a2, b2, c2);
  d1 = a1*x[4] + b1*y[4] + c1;
  d2 = a2*x[4] + b2*y[4] + c2;
  if (d1 <= m_del && d2 <= m_del)            return 1;
  if (! (d1 > m_del && d2 > m_del)) {
    if ( a1*x[2] + b1*y[2] + c1 >= -m_del) return 1;
  }

  //   T E S T   L I N E   1 - 4   V S   I N T E R N A L   C O N T O U R

  findABC(x[3], y[3], x[4], y[4], a1, b1, c1);
  findABC(x[4], y[4], x[5], y[5], a2, b2, c2);
  d1 = a1*x[1] + b1*y[1] + c1;
  d2 = a2*x[1] + b2*y[1] + c2;
  if (d1 <= m_del && d2 <= m_del)            return 1;
  if (!(d1 > m_del && d2 > m_del)) {
    if ( a1*x[5] + b1*y[5] + c1 >= -m_del) return 1;
  }
  return 0;
}

int BooleanProcessor::checkIntersection(int ix, int iy, int i1, int i2) const
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::checkDirection            Date:    06.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Check line i1-i2 on intersection with contours            *
 *                                                                     *
 ***********************************************************************/
{
  //  F I N D   L I N E   E Q U A T I O N

  double x1, y1, x2, y2, a1, b1, c1;
  x1 = m_nodes[i1].v[ix];
  y1 = m_nodes[i1].v[iy];
  x2 = m_nodes[i2].v[ix];
  y2 = m_nodes[i2].v[iy];
  findABC(x1, y1, x2, y2, a1, b1, c1);

  //  L O O P   A L O N G   E X T E R N A L   C O N T O U R S

  int icontour, iedge, k1, k2;
  double x3, y3, x4, y4, a2, b2, c2, d1, d2;
  for(icontour=0; icontour<(int)m_external_contours.size(); icontour++) {
    iedge = m_external_contours[icontour];
    while(iedge > 0) {
      k1 = m_edges[iedge].i1;
      k2 = m_edges[iedge].i2;
      iedge = m_edges[iedge].inext;
      if (k1 == i1 || k2 == i1) continue;
      if (k1 == i2 || k2 == i2) continue;
      x3 = m_nodes[k1].v[ix];
      y3 = m_nodes[k1].v[iy];
      x4 = m_nodes[k2].v[ix];
      y4 = m_nodes[k2].v[iy];
      d1 = a1*x3 + b1*y3 + c1;
      d2 = a1*x4 + b1*y4 + c1;
      if (d1 >  m_del && d2 >  m_del) continue;
      if (d1 < -m_del && d2 < -m_del) continue;

      findABC(x3, y3, x4, y4, a2, b2, c2);
      d1 = a2*x1 + b2*y1 + c2;
      d2 = a2*x2 + b2*y2 + c2;
      if (d1 >  m_del && d2 >  m_del) continue;
      if (d1 < -m_del && d2 < -m_del) continue;
      return 1;
    }
  }

  //  L O O P   A L O N G   E X T E R N A L   C O N T O U R S

  for(icontour=0; icontour<(int)m_internal_contours.size(); icontour++) {
    iedge = m_internal_contours[icontour];
    while(iedge > 0) {
      k1 = m_edges[iedge].i1;
      k2 = m_edges[iedge].i2;
      iedge = m_edges[iedge].inext;
      if (k1 == i1 || k2 == i1) continue;
      if (k1 == i2 || k2 == i2) continue;
      x3 = m_nodes[k1].v[ix];
      y3 = m_nodes[k1].v[iy];
      x4 = m_nodes[k2].v[ix];
      y4 = m_nodes[k2].v[iy];
      d1 = a1*x3 + b1*y3 + c1;
      d2 = a1*x4 + b1*y4 + c1;
      if (d1 >  m_del && d2 >  m_del) continue;
      if (d1 < -m_del && d2 < -m_del) continue;

      findABC(x3, y3, x4, y4, a2, b2, c2);
      d1 = a2*x1 + b2*y1 + c2;
      d2 = a2*x2 + b2*y2 + c2;
      if (d1 >  m_del && d2 >  m_del) continue;
      if (d1 < -m_del && d2 < -m_del) continue;
      return 1;
    }
  }
  return 0;
}

void BooleanProcessor::mergeContours(int ix, int iy, int kext, int kint)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::mergeContours             Date:    06.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Attemp to merge internal contour with external one        *
 *                                                                     *
 ***********************************************************************/
{
  int    i1ext, i2ext, i1int, i2int, i, k[6];
  double x[6], y[6];

  //   L O O P   A L O N G   E X T E R N A L   C O N T O U R

  i1ext = m_external_contours[kext];
  while (i1ext > 0) {
    i2ext = m_edges[i1ext].inext;
    if (i2ext == 0) i2ext = m_external_contours[kext];
    k[0] = m_edges[i1ext].i1;
    k[1] = m_edges[i1ext].i2;
    k[2] = m_edges[i2ext].i2;
    for (i=0; i<3; i++) {
      x[i] = m_nodes[k[i]].v[ix];
      y[i] = m_nodes[k[i]].v[iy];
    }

    //   L O O P   A L O N G   I N T E R N A L   C O N T O U R

    i1int = m_internal_contours[kint];
    while (i1int > 0) {
      i2int = m_edges[i1int].inext;
      if (i2int == 0) i2int = m_internal_contours[kint];
      k[3] = m_edges[i1int].i1;
      k[4] = m_edges[i1int].i2;
      k[5] = m_edges[i2int].i2;
      for (i=3; i<6; i++) {
        x[i] = m_nodes[k[i]].v[ix];
        y[i] = m_nodes[k[i]].v[iy];
      }

      //   T E S T   L I N E   K1 - K4
      //   I F   O K   T H E N   M E R G E   C O N T O U R S

      if (checkDirection(x, y) == 0) {
        if (checkIntersection(ix, iy, k[1], k[4]) == 0) {
          i = i1int;
          for(;;) {
            if (m_edges[i].inext == 0) {
              m_edges[i].inext = m_internal_contours[kint];
              m_internal_contours[kint] = 0;
              break;
            }else{
              i = m_edges[i].inext;
            }
          }
          i = m_edges[i1int].iface1;
          m_edges.push_back(ExtEdge(k[1], k[4], i, -(int(m_edges.size())+1), -1));
          m_edges.back().inext = i2int;
          m_edges.push_back(ExtEdge(k[4], k[1], i, -(int(m_edges.size())-1), -1));
          m_edges.back().inext = m_edges[i1ext].inext;
          m_edges[i1ext].inext = m_edges.size()-2;
          m_edges[i1int].inext = m_edges.size()-1;
          return;
        }
      }
      i1int = m_edges[i1int].inext;
    }
    i1ext = m_edges[i1ext].inext;
  }
}

int BooleanProcessor::checkTriangle(int iedge1, int iedge2, int ix, int iy) const
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::checkTriangle             Date:    08.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Check triangle for correctness                            *
 *                                                                     *
 ***********************************************************************/
{
  int    k[3];
  double x[3], y[3];
  double a1, b1, c1;

  k[0] = m_edges[iedge1].i1;
  k[1] = m_edges[iedge1].i2;
  k[2] = m_edges[iedge2].i2;
  for (int i=0; i<3; i++) {
    x[i] = m_nodes[k[i]].v[ix];
    y[i] = m_nodes[k[i]].v[iy];
  }

  //  C H E C K   P R I N C I P A L   C O R R E C T N E S S

  findABC(x[2], y[2], x[0], y[0], a1, b1, c1);
  if (a1*x[1]+b1*y[1]+c1 <= 0.1*m_del) return 1;

  //   C H E C K   T H A T   T H E R E   I S   N O   P O I N T S   I N S I D E

  int    inode, iedge;
  double a2, b2, c2, a3, b3, c3;

  findABC(x[0], y[0], x[1], y[1], a2, b2, c2);
  findABC(x[1], y[1], x[2], y[2], a3, b3, c3);
  iedge = iedge2;
  for (;;) {
    iedge = m_edges[iedge].inext;
    if (m_edges[iedge].inext == iedge1) return 0;
    inode = m_edges[iedge].i2;
    if (inode == k[0])                continue;
    if (inode == k[1])                continue;
    if (inode == k[2])                continue;
    x[1]  = m_nodes[inode].v[ix];
    y[1]  = m_nodes[inode].v[iy];
    if (a1*x[1]+b1*y[1]+c1 < -0.1*m_del)    continue;
    if (a2*x[1]+b2*y[1]+c2 < -0.1*m_del)    continue;
    if (a3*x[1]+b3*y[1]+c3 < -0.1*m_del)    continue;
    return 1;
  }
  return 0; // default return
}

void BooleanProcessor::triangulateContour(int ix, int iy, int ihead)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::triangulateContour        Date:    06.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Triangulate external contour                              *
 *                                                                     *
 ***********************************************************************/
{

  //G4cout << "Next Countour" << G4endl;
  //int draw_flag = 0;
  //if (draw_flag) draw_contour(5, 3, ihead);

  //   C L O S E   C O N T O U R

  int ipnext = ihead, nnode = 1;
  for (;;) {
    if (m_edges[ipnext].inext > 0) {
      ipnext = m_edges[ipnext].inext;
      nnode++;
    }else{
      m_edges[ipnext].inext = ihead;
      break;
    }
  }

  //   L O O P   A L O N G   C O N T O U R

  //G4cerr << "debug : contour : begin : =================" << G4endl;
  //dump();//debug

  int iedge1, iedge2, iedge3, istart = 0;
  for (;;) {
    iedge1 = m_edges[ipnext].inext;
    iedge2 = m_edges[iedge1].inext;
/*
    G4cerr << "debug :"
              << " ipnext " << ipnext
              << " iedge1 " << iedge1
              << " iedge2 " << iedge2
              << " : istart " << istart
              << G4endl;
*/
    if (istart == 0) {
      istart = iedge1;
      if (nnode <= 3) {
        iedge3 = m_edges[iedge2].inext;
        m_edges[iedge1].iface1 = m_faces.size();
        m_edges[iedge2].iface1 = m_faces.size();
        m_edges[iedge3].iface1 = m_faces.size();
        m_edges[iedge3].inext = 0;
        m_faces.push_back(ExtFace(m_edges,0)); //G.Barrand : ok ?
        m_faces.back().iold = iedge1;
        m_faces.back().inew = ORIGINAL_FACE;

  //if (draw_flag) draw_contour(4, 2, iedge1);

        break;
      }
    }else if (istart == iedge1) {
      m_processor_error = 1;
#ifdef BP_DEBUG
      G4cerr
        << "BooleanProcessor::triangulateContour : "
        << "could not generate a triangle (infinite loop)"
        << G4endl;
#endif
      break;
    }

    //   C H E C K   C O R E C T N E S S   O F   T H E   T R I A N G L E

    if (checkTriangle(iedge1,iedge2,ix,iy) != 0) {
      ipnext  = m_edges[ipnext].inext;
      continue;
    }

    //   M O D I F Y   C O N T O U R

    int i1 = m_edges[iedge1].i1;
    int i3 = m_edges[iedge2].i2;
    int iface1 = m_edges[iedge1].iface1;
    int iface2 = m_faces.size();

    m_edges[ipnext].inext = m_edges.size();
    m_edges.push_back(ExtEdge(i1, i3, iface1, -(int(m_edges.size())+1), -1));
    m_edges.back().inext = m_edges[iedge2].inext;

    //   A D D   N E W   T R I A N G L E   T O   T H E   L I S T

    m_edges[iedge2].inext = m_edges.size();
    m_edges.push_back(ExtEdge(i3, i1, iface2, -(int(m_edges.size())-1), -1));
    m_faces.push_back(ExtFace(m_edges,0)); //G.Barrand : ok ?
    m_faces.back().iold   = iedge1;
    m_faces.back().inew   = ORIGINAL_FACE;
    m_edges[iedge1].iface1 = iface2;
    m_edges[iedge2].iface1 = iface2;
    ipnext  = m_edges[ipnext].inext;
    istart = 0;
    nnode--;

  //if (draw_flag)  draw_contour(4, 2, iedge1);

  }
}

void BooleanProcessor::modifyReference(int iface, int i1, int i2, int iref)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::modifyReference           Date:    13.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Modify reference to the neighbouring face                 *
 *                                                                     *
 ***********************************************************************/
{
  int iedge = m_faces[iface].iold;
  while (iedge > 0) {
    if (m_edges[iedge].i1 == i2 && m_edges[iedge].i2 == i1) {
      m_edges[iedge].iface2 = iref;
      return;
    }
    iedge = m_edges[iedge].inext;
  }
  m_processor_error = 1;
#ifdef BP_DEBUG
  G4cerr
    << "BooleanProcessor::modifyReference : could not find the edge, "
    << "iface=" << iface << ", i1,i2=" << i1 << "," << i2 << ", iref=" << iref
    << G4endl;
#endif
}

void BooleanProcessor::triangulateFace(int iface)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::triangulateFace           Date:    02.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Triangulation of an extended face                         *
 *                                                                     *
 ***********************************************************************/
{

  //   F I N D   M A X   C O M P O N E N T   O F   T H E   N O R M A L
  //   S E T  IX, IY, IZ

#ifdef BP_GEANT4 //G.Barrand
  HVNormal3D normal = m_faces[iface].plane.normal();
#else
  const HVNormal3D& normal = m_faces[iface].plane.getNormal();
#endif
  int ix, iy, iz = 0;
  //G.Barrand : if (std::abs(normal[1]) > std::abs(normal[iz])) iz = 1;
  //G.Barrand : if (std::abs(normal[2]) > std::abs(normal[iz])) iz = 2;
  if (::fabs(normal[1]) > ::fabs(normal[iz])) iz = 1; //G.Barrand
  if (::fabs(normal[2]) > ::fabs(normal[iz])) iz = 2; //G.Barrand
  if (normal[iz] > 0) {
    ix = (iz+1)%3; iy = (ix+1)%3;
  }else{
    iy = (iz+1)%3; ix = (iy+1)%3;
  }

  //   F I L L   L I S T S   O F   C O N T O U R S

  m_external_contours.clear();
  m_internal_contours.clear();
  double z;
  int    i1, i2, ifirst, iedge, icontour = m_faces[iface].iold;
  while (icontour > 0) {
    iedge  = icontour;
    ifirst = m_edges[iedge].i1;
    z      = 0.0;
    for(;;) {
      if (iedge > 0) {
        i1 = m_edges[iedge].i1;
        i2 = m_edges[iedge].i2;
        ExtNode& node_1 = m_nodes[i1];
        ExtNode& node_2 = m_nodes[i2];
        z += node_1.v[ix]*node_2.v[iy]-node_2.v[ix]*node_1.v[iy];
        if (ifirst != i2) {
          iedge = m_edges[iedge].inext;
          continue;
        }else{
          if (z > m_del*m_del) {
            m_external_contours.push_back(icontour);
          }else if (z < -m_del*m_del) {
            m_internal_contours.push_back(icontour);
          }else{
            m_processor_error = 1;
#ifdef BP_DEBUG
            G4cerr
              << "BooleanProcessor::triangulateFace : too small contour"
              << G4endl;
#endif
          }
          icontour = m_edges[iedge].inext;
          m_edges[iedge].inext = 0;
          break;
        }
      }else{
        m_processor_error = 1;
#ifdef BP_DEBUG
        G4cerr
          << "BooleanProcessor::triangulateFace : broken contour"
          << G4endl;
#endif
        icontour = 0;
        break;
      }
    }
  }

  //   G E T   R I D   O F   I N T E R N A L   C O N T O U R S

  int kint, kext;
  for (kint=0; kint < (int)m_internal_contours.size(); kint++) {
    for (kext=0; kext < (int)m_external_contours.size(); kext++) {
      mergeContours(ix, iy, kext, kint);
      if (m_internal_contours[kint] == 0) break;
    }
    if (kext == (int)m_external_contours.size()) {
      m_processor_error = 1;
#ifdef BP_DEBUG
      G4cerr
        << "BooleanProcessor::triangulateFace : "
        << "could not merge internal contour " << kint
        << G4endl;
#endif
    }
  }

  //   T R I A N G U L A T E   C O N T O U R S

  int nface = m_faces.size();
  for (kext=0; kext < (int)m_external_contours.size(); kext++) {
    triangulateContour(ix, iy, m_external_contours[kext]);
#ifdef BP_DEBUG
    if(m_processor_error) { //G.Barrand
      G4cerr
        << "BooleanProcessor::triangulateFace : "
        << "triangulateContour failed."
        << G4endl;
      break; //G.Barrand : ok ?
    }
#endif
  }
  m_faces[iface].inew = UNSUITABLE_FACE;

  //   M O D I F Y   R E F E R E N C E S

  for (int ifa=nface; ifa<(int)m_faces.size(); ifa++) {
    iedge = m_faces[ifa].iold;
    while (iedge > 0) {
      if (m_edges[iedge].iface1 != ifa) {
        m_processor_error = 1;
#ifdef BP_DEBUG
        G4cerr
          << "BooleanProcessor::triangulateFace : wrong reference to itself, "
          << "iface=" << ifa << ", iface1=" << m_edges[iedge].iface1
          << G4endl;
#endif
      }else if (m_edges[iedge].iface2 > 0) {
        modifyReference(m_edges[iedge].iface2,
                        m_edges[iedge].i1, m_edges[iedge].i2, ifa);
      }else if (m_edges[iedge].iface2 < 0) {
        m_edges[iedge].iface2 = m_edges[-m_edges[iedge].iface2].iface1;
      }
      iedge = m_edges[iedge].inext;
    }
  }
}

HepPolyhedron BooleanProcessor::createPolyhedron()
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::createPolyhedron()        Date:    14.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Create HepPolyhedron.                                     *
 *                                                                     *
 ***********************************************************************/
{
  int i, iedge, nnode = 0, nface = 0;

  //   R E N U M E R A T E   N O D E S   A N D   F A C E S

  for (i=1; i<(int)m_nodes.size(); i++) m_nodes[i].s = 0;

  for (i=1; i<(int)m_faces.size(); i++) {
    if (m_faces[i].inew == ORIGINAL_FACE) {
      m_faces[i].inew = ++nface;
      iedge = m_faces[i].iold;
      while (iedge > 0) {
        m_nodes[m_edges[iedge].i1].s = 1;
        iedge = m_edges[iedge].inext;
      }
    }else{
      m_faces[i].inew = 0;
    }
  }

  for (i=1; i<(int)m_nodes.size(); i++) {
    if (m_nodes[i].s == 1) m_nodes[i].s = ++nnode;
  }

  //   A L L O C A T E   M E M O R Y

  ExtPolyhedron polyhedron;
  if (nface == 0) return polyhedron;
  polyhedron.AllocateMemory(nnode, nface);

  //   S E T   N O D E S

  for (i=1; i<(int)m_nodes.size(); i++) {
    if (m_nodes[i].s != 0)  polyhedron.m_pV[m_nodes[i].s] = m_nodes[i].v;
  }

  //   S E T   F A C E S

  int k, v[4], f[4];
  for (i=1; i<(int)m_faces.size(); i++) {
    if (m_faces[i].inew == 0) continue;
    v[3] = f[3] = k = 0;
    iedge = m_faces[i].iold;
    while (iedge > 0) {
      if (k > 3) {
        std::cerr << "BooleanProcessor::createPolyhedron : too many edges" << std::endl;
        break;
      }
      v[k]  = m_nodes[m_edges[iedge].i1].s;
      if (m_edges[iedge].ivis < 0) v[k] = -v[k];
      f[k]  = m_faces[m_edges[iedge].iface2].inew;
      iedge = m_edges[iedge].inext;
      k++;
    }
    if (k < 3) {
      std::cerr << "BooleanProcessor::createPolyhedron : "
        << "face has only " << k << " edges"
        << std::endl;
    }
    polyhedron.m_pF[m_faces[i].inew] =
      G4Facet(v[0],f[0], v[1],f[1], v[2],f[2], v[3],f[3]);
  }
  return polyhedron;
}

int BooleanProcessor::s_ishift = 0; //G.Barrand
int BooleanProcessor::get_shift() { return s_ishift;} //G.Barrand
void BooleanProcessor::set_shift(int a_shift) { s_ishift = a_shift;} //G.Barrand
#define NUM_SHIFT 8
int BooleanProcessor::get_num_shift() { return NUM_SHIFT;} //G.Barrand

HepPolyhedron BooleanProcessor::execute(int op,
                                        const HepPolyhedron & a,
                                        const HepPolyhedron & b,
                                        int& err) //G.Barrand
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::execute                   Date:    10.12.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Execute boolean operation.                                *
 *                                                                     *
 ***********************************************************************/
{
  //static int ishift = 0; //G.Barrand
  //static double shift[8][3] = {
  static double shift[NUM_SHIFT][3] = { //G.Barrand
    {  31,  23,  17},
    { -31, -23, -17},
    { -23,  17,  31},
    {  23, -17, -31},
    { -17, -31,  23},
    {  17,  31, -23},
    {  31, -23,  17},
    { -31,  23, -17}
  };

/*
  G4cerr << "BooleanProcessor::execute : ++++++++++++++++++++++"
            << a.getName().getString()
            << b.getName().getString()
            << G4endl;
*/

  //   I N I T I A T E   P R O C E S S O R

  m_processor_error = 0;
  m_operation = op;
  m_nodes.clear(); m_nodes.push_back(CRAZY_POINT);
  m_edges.clear(); m_edges.push_back(ExtEdge());
  m_faces.clear(); m_faces.push_back(ExtFace(m_edges,0)); //G.Barrand : ok ?

  //   T A K E   P O L Y H E D R A

  m_ifaces1 = m_faces.size(); takePolyhedron(a,0,0,0);
  m_ifaces2 = m_faces.size(); takePolyhedron(b,0,0,0);

  if (m_processor_error) {             // corrupted polyhedron
    std::cerr
      << "BooleanProcessor: corrupted input polyhedron"
      << std::endl;
    err = m_processor_error; //G.Barrand
    return HepPolyhedron();
  }
  if (m_ifaces1 == m_ifaces2) {          // a is empty
    err = m_processor_error; //G.Barrand
    switch (m_operation) {
    case OP_UNION:
      return b;
    case OP_INTERSECTION:
    	std::cerr
        << "BooleanProcessor: intersection with empty polyhedron"
        << std::endl;
      return HepPolyhedron();
    case OP_SUBTRACTION:
    	std::cerr
        << "BooleanProcessor: subtraction from empty polyhedron"
        << std::endl;
      return HepPolyhedron();
    }
  }
  if (m_ifaces2 == (int)m_faces.size()) {     // b is empty
    err = m_processor_error; //G.Barrand
    switch (m_operation) {
    case OP_UNION:
      return a;
    case OP_INTERSECTION:
    	std::cerr
        << "BooleanProcessor: intersection with empty polyhedron"
        << std::endl;
      return HepPolyhedron();
    case OP_SUBTRACTION:
      return a;
    }
  }

  //   S E T   I N I T I A L   M I N - M A X   A N D   T O L E R A N C E

  m_del = findMinMax();

  //   W O R K A R O U N D   T O   A V O I D   I E   A N D   E E

/*
#define PROCESSOR_ERROR(a_what) \
  G4cerr << "BooleanProcessor: boolean operation problem (" << a_what \
            << "). Try again with other shifts."\
            << G4endl;
*/
#define PROCESSOR_ERROR(a_what)

  unsigned int try_count = 1;
  while(true) { //G.Barrand

  double ddxx = m_del*shift[s_ishift][0];
  double ddyy = m_del*shift[s_ishift][1];
  double ddzz = m_del*shift[s_ishift][2];
  s_ishift++; if (s_ishift == get_num_shift()) s_ishift = 0;

  m_processor_error = 0; //G.Barrand
  m_operation = op;
  m_nodes.clear(); m_nodes.push_back(CRAZY_POINT);
  m_edges.clear(); m_edges.push_back(ExtEdge());
  m_faces.clear(); m_faces.push_back(ExtFace(m_edges,0)); //G.Barrand : ok ?

  m_ifaces1 = m_faces.size(); takePolyhedron(a,0,0,0);
  m_ifaces2 = m_faces.size(); takePolyhedron(b,ddxx,ddyy,ddzz);

  if (m_processor_error) { PROCESSOR_ERROR(1) } //G.Barrand

  m_del = findMinMax();

  //   P R E S E L E C T   O U T S I D E   F A C E S

  m_iout1 = m_iout2 = 0;
  selectOutsideFaces(m_ifaces1, m_iout1);
  selectOutsideFaces(m_ifaces2, m_iout2);

  if (m_processor_error) { PROCESSOR_ERROR(2) } //G.Barrand

  //   P R E S E L E C T   N O   I N T E R S E C T I O N   F A C E S

  int ifa1, ifa2;
  m_iunk1 = m_iunk2 = 0;
  if (m_iout1 != 0 || m_iout2 != 0) {
    for(;;) {
      ifa1 = m_iunk1;
      ifa2 = m_iunk2;
      selectOutsideFaces(m_ifaces1, m_iunk1);
      selectOutsideFaces(m_ifaces2, m_iunk2);
      if (m_iunk1 == ifa1 && m_iunk2 == ifa2) break;
      findMinMax();
    }
  }

  if (m_processor_error) { PROCESSOR_ERROR(3) } //G.Barrand

  //   F I N D   N E W   E D G E S

  if (m_ifaces1 != 0 && m_ifaces2 != 0 ) {
    ifa1 = m_ifaces1;
    while (ifa1 > 0) {
      ifa2 = m_ifaces2;
      while (ifa2 > 0) {
        testFaceVsFace(ifa1, ifa2);
        ifa2 = m_faces[ifa2].inext;
      }
      ifa1 = m_faces[ifa1].inext;
    }
  }
  if (m_processor_error) { PROCESSOR_ERROR(4) } //G.Barrand

  //   C O N S T R U C T   N E W   F A C E S

  assembleNewFaces((m_operation == OP_INTERSECTION) ? 1 : 0, m_ifaces1);
  if (m_processor_error) { PROCESSOR_ERROR(5) } //G.Barrand
  assembleNewFaces((m_operation == OP_UNION) ? 0 : 1, m_ifaces2);
  if (m_processor_error) { PROCESSOR_ERROR(6) } //G.Barrand

  //   A S S E M B L E   S U I T A B L E   F A C E S

  initiateLists();
  assemblePolyhedra();
  if (m_unknown_faces.front() != 0) {
    m_processor_error = 1;
#ifdef BP_DEBUG
    G4cerr
      << "BooleanProcessor::execute : unknown faces !!!"
      << G4endl;
#endif
  }
  if (m_processor_error) { PROCESSOR_ERROR(7) } //G.Barrand

  //   T R I A N G U L A T E   A C C E P T E D   F A C E S

  ifa1 = m_result_faces.front();
  while (ifa1 > 0) {
    ifa2 = ifa1;
    ifa1 = m_faces[ifa2].inext;
    if (m_faces[ifa2].inew == NEW_FACE) triangulateFace(ifa2);
    if (m_processor_error) {
      PROCESSOR_ERROR(8) //G.Barrand
      break; //G.Barrand
    }
  }

  if(!m_processor_error) {
#ifdef BP_DEBUG
    if(try_count!=1) {
      G4cerr
         << "BooleanProcessor::execute : had converged."
         << G4endl;
    }
#endif
    break;
  }

  if((int)try_count>get_num_shift()) {
#ifdef BP_DEBUG
    /*** Commented out because HepPolyhedron does not have getName...?!
    G4cerr << "BooleanProcessor: "
              << " all shifts tried. Boolean operation (" << op << ") failure."
              << " a name \"" << a.getName().getString() << "\""
              << " b name \"" << b.getName().getString() << "\""
              << G4endl;
    ***/
#endif
    err = m_processor_error;
    return a;
  }

#ifdef BP_DEBUG
  G4cerr
     << "BooleanProcessor::execute : try another tilt..."
     << G4endl;
#endif

  try_count++;

  } //G.Barrand : end while shift.
#undef PROCESSOR_ERROR //G.Barrand

  //   C R E A T E   P O L Y H E D R O N

  err = m_processor_error;
  return createPolyhedron();
}


//#include <cfortran.h>
//#include <higz.h>
//#include "zbuf.h"
//void BooleanProcessor::draw()
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::draw                      Date:    10.12.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Draw                                                      *
 *                                                                     *
 ***********************************************************************/
/*
{
  int II;
  int   icol, i1, i2, iedge, iface, ilist[4];
  float p1[3], p2[3];

  ilist[0] = m_ifaces1;
  ilist[1] = m_ifaces2;
  ilist[2] = m_iout1;
  ilist[3] = m_iout2;

  for (int i=0; i<4; i++) {

    if (i == 0) G4cout << "========= Ifaces_1" << G4endl;
    if (i == 1) G4cout << "========= Ifaces_2" << G4endl;
    if (i == 2) G4cout << "========= Iout_1" << G4endl;
    if (i == 3) G4cout << "========= Iout_2" << G4endl;

    icol = i+1;
    iface = ilist[i];
    while (iface > 0) {

      G4cout << "iface = " << iface << G4endl;
      G4cout << "--- iold" << G4endl;

      iedge = m_faces[iface].iold;
      icol = 2;

      while (iedge > 0) {

        G4cout << "  iegde = " << iedge
             << " i1,i2 =" << m_edges[iedge].i1 << "," << m_edges[iedge].i2
             << " iface1,iface2 = "
             << m_edges[iedge].iface1 << "," << m_edges[iedge].iface2
             << G4endl;

        i1 = m_edges[iedge].i1;
        p1[0] = m_nodes[i1].v.x();
        p1[1] = m_nodes[i1].v.y();
        p1[2] = m_nodes[i1].v.z();
        IHWTON(p1,p1);
        i2 = m_edges[iedge].i2;
        p2[0] = m_nodes[i2].v.x();
        p2[1] = m_nodes[i2].v.y();
        p2[2] = m_nodes[i2].v.z();
        IHWTON(p2,p2);
//        icol =  (m_edges[iedge].ivis > 0) ? 1 : 2;
        IHZLIN(icol,p1[0],p1[1],p1[2], p2[0],p2[1],p2[2]);
        iedge = m_edges[iedge].inext;
      }

      G4cout << "--- inew" << G4endl;

      iedge = m_faces[iface].inew;
      icol = 3;

      while (iedge > 0) {

        G4cout << "  iegde = " << iedge
             << " i1,i2 =" << m_edges[iedge].i1 << "," << m_edges[iedge].i2
             << " iface1,iface2 = "
             << m_edges[iedge].iface1 << "," << m_edges[iedge].iface2
             << G4endl;

        i1 = m_edges[iedge].i1;
        p1[0] = m_nodes[i1].v.x();
        p1[1] = m_nodes[i1].v.y();
        p1[2] = m_nodes[i1].v.z();
        IHWTON(p1,p1);
        i2 = m_edges[iedge].i2;
        p2[0] = m_nodes[i2].v.x();
        p2[1] = m_nodes[i2].v.y();
        p2[2] = m_nodes[i2].v.z();
        IHWTON(p2,p2);
//        icol =  (m_edges[iedge].ivis > 0) ? 1 : 2;
        IHZLIN(icol,p1[0],p1[1],p1[2], p2[0],p2[1],p2[2]);
        iedge = m_edges[iedge].inext;
      }
      iface = m_faces[iface].inext;

      IHZTOX(0,100,100);
      ixupdwi(0);
      cin >> II;
      ixclrwi();
      IHZCLE(0);
    }
  }
}
*/

/*
//--------------------------------------------------------------------
void
BooleanProcessor::draw_edge(int icol, int iedge) {
  int   i1, i2;
  float p1[3], p2[3];

  i1 = m_edges[iedge].i1;
  p1[0] = m_nodes[i1].v.x();
  p1[1] = m_nodes[i1].v.y();
  p1[2] = m_nodes[i1].v.z();
  IHWTON(p1,p1);
  i2 = m_edges[iedge].i2;
  p2[0] = m_nodes[i2].v.x();
  p2[1] = m_nodes[i2].v.y();
  p2[2] = m_nodes[i2].v.z();
  IHWTON(p2,p2);
  IHZLIN(icol,p1[0],p1[1],p1[2], p2[0],p2[1],p2[2]);
}

//--------------------------------------------------------------------
void
BooleanProcessor::draw_contour(int i1col, int i2col, int ihead) {
  int iedge, icol;
  iedge = ihead;
  while (iedge > 0) {
    icol = (m_edges[iedge].ivis > 0) ? i1col : i2col;
    draw_edge(icol, iedge);
    iedge = m_edges[iedge].inext;
  }

  IHZTOX(0,100,100);
  ixupdwi(0);

  int i;
  std::cin >> i;
}

//--------------------------------------------------------------------
void
BooleanProcessor::print_face(int iface) {
  G4cout.precision(3);
  G4cout << "\n====== Face N " << iface << G4endl;
  G4cout << "iedges[4] = "
       << m_faces[iface].iedges[0] << ", "
       << m_faces[iface].iedges[1] << ", "
       << m_faces[iface].iedges[2] << ", "
       << m_faces[iface].iedges[3] << G4endl;
  G4cout << "rmin[3] = "
       << m_faces[iface].m_rmin[0] << ", "
       << m_faces[iface].m_rmin[1] << ", "
       << m_faces[iface].m_rmin[2] << G4endl;
  G4cout << "rmax[3] = "
       << m_faces[iface].m_rmax[0] << ", "
       << m_faces[iface].m_rmax[1] << ", "
       << m_faces[iface].m_rmax[2] << G4endl;
  G4cout << "iprev,inext = "
       << m_faces[iface].iprev << ", "
       << m_faces[iface].inext << G4endl;
  G4cout << "iold = " << m_faces[iface].iold << G4endl;
  for(int i = m_faces[iface].iold; i != 0;) {
    print_edge(i);
    i = m_edges[abs(i)].inext;
  }

  G4cout << "inew = ";
  switch (m_faces[iface].inew) {
  case UNKNOWN_FACE:
    G4cout << "UNKNOWN_FACE" << G4endl;
    break;
  case ORIGINAL_FACE:
    G4cout << "ORIGINAL_FACE" << G4endl;
    break;
  case NEW_FACE:
    G4cout << "NEW_FACE" << G4endl;
    break;
  case UNSUITABLE_FACE:
    G4cout << "UNSUITABLE_FACE" << G4endl;
    break;
  case DEFECTIVE_FACE:
    G4cout << "DEFECTIVE_FACE" << G4endl;
    break;
  default:
    G4cout << m_faces[iface].inew << G4endl;
    for(int k = m_faces[iface].inew; k != 0;) {
      print_edge(k);
      k = m_edges[abs(k)].inext;
    }
  }
}

//--------------------------------------------------------------------
void
BooleanProcessor::print_edge(int iedge) {
  G4cout << "==== Edge N " << iedge << G4endl;
  int i = std::abs(iedge);
  int i1 = m_edges[i].i1;
  int i2 = m_edges[i].i2;
  G4cout << "node[" << i1 << "] = "
       << m_nodes[i1].v.x() << ", "
       << m_nodes[i1].v.y() << ", "
       << m_nodes[i1].v.z() << G4endl;

  G4cout << "node[" << i2 << "] = "
       << m_nodes[i2].v.x() << ", "
       << m_nodes[i2].v.y() << ", "
       << m_nodes[i2].v.z() << G4endl;

  G4cout << "iface1,iface2,ivis,inext = "
       << m_edges[i].iface1 << ", "
       << m_edges[i].iface2 << ", "
       << m_edges[i].ivis   << ", "
       << m_edges[i].inext  << G4endl;
}
*/

void BooleanProcessor::dump() {//G.Barrand
  unsigned int number = m_nodes.size();
  std::cerr << "nodes : " << number << std::endl;
  for(unsigned int index=0;index<number;index++) {
    const ExtNode& node = m_nodes[index];
    std::cerr << " " << index
           << " x = " << node.v[0]
           << " y = " << node.v[1]
           << " z = " << node.v[2]
           << std::endl;
  }
}
