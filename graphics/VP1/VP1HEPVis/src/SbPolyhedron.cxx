////////////////////////////////////////////////////////////////
//                                                            //
//  Source file for class SbPolyhedron                        //
//                                                            //
//  Update:
//          Riccardo-Maria BIANCHI (rbianchi@cern.ch)         //
//          13.12.2012                                        //
//                                                            //
////////////////////////////////////////////////////////////////


// this :
#include <VP1HEPVis/SbPolyhedron.h>
#include <cassert>

#define perMillion 0.000001
#define deg (M_PI/180.0)

#include <float.h> //G.Barrand : to have DBL_EPSILON on Windows.

// G.Barrand : introduce iabs to avoid a mess with cmath and some compiler.
inline int iabs(int a) {
  return a < 0 ? -a : a;
}
inline float ffabs(float a) {
  return a < 0.0f ? -a : a;
}


// rbianchi
HVPoint3D::HVPoint3D(): SbVec3d(0,0,0){}
HVPoint3D::HVPoint3D(double x,double y,double z): SbVec3d(x,y,z){}
HVPoint3D::HVPoint3D(const HVPoint3D& v): SbVec3d(v){}
HVPoint3D::HVPoint3D(const SbVec3d& v): SbVec3d(v){}
HVPoint3D& HVPoint3D::operator=(const HVPoint3D& v) {
  setValue(v[0],v[1],v[2]);
  return *this;
}
HVPoint3D& HVPoint3D::operator=(const SbVec3d& v) {
  setValue(v[0],v[1],v[2]);
  return *this;
}
HVPoint3D operator +(const HVPoint3D& v1,const HVPoint3D& v2) {
  return HVPoint3D(v1[0] + v2[0],v1[1] + v2[1],v1[2] + v2[2]);
}
//---



//--------------------------------------------------------------------//
// JFB:                                                               //
// SbPolyhedron was SbPolyhedron, retrofitted to Open Inventor       //
// infrastructure:                                                    //
//--------------------------------------------------------------------//


//
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
// $Id: SbPolyhedron.cxx,v 1.3 2008-09-14 19:04:40 tkittel Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
//
// G4 Polyhedron library
//
// History:
// 23.07.96 E.Chernyaev <Evgueni.Tcherniaev@cern.ch> - initial version
//
// 30.09.96 E.Chernyaev
// - added GetNextVertexIndex, GetVertex by Yasuhide Sawada
// - added GetNextUnitNormal, GetNextEdgeIndeces, GetNextEdge
//
// 15.12.96 E.Chernyaev
// - added GetNumberOfRotationSteps, RotateEdge, RotateAroundZ, SetReferences
// - rewritten G4PolyhedronCons;
// - added G4PolyhedronPara, ...Trap, ...Pgon, ...Pcon, ...Sphere, ...Torus
//
// 01.06.97 E.Chernyaev
// - modified RotateAroundZ, added SetSideFacets
//
// 19.03.00 E.Chernyaev
// - implemented boolean operations (add, subtract, intersect) on polyhedra;
//
// 25.05.01 E.Chernyaev
// - added GetSurfaceArea() and GetVolume();
//


/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron operator <<                   Date:    09.05.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Print contents of G4 polyhedron                           *
 *                                                                     *
 ***********************************************************************/
std::ostream & operator<<(std::ostream & ostr, const SbFacet & facet) {
  for (int k=0; k<4; k++) {
    ostr << " " << facet.edge[k].v << "/" << facet.edge[k].f;
  }
  return ostr;
}

std::ostream & operator<<(std::ostream & ostr, const SbPolyhedron & ph) {
  ostr << std::endl;
  ostr << "Nverteces=" << ph.nvert << ", Nfacets=" << ph.nface << std::endl;
  int i;
  for (i=1; i<=ph.nvert; i++) {
     ostr << "xyz(" << i << ")="
          << ph.pV[i][0] << ' ' << ph.pV[i][1] << ' ' << ph.pV[i][2]
          << std::endl;
  }
  for (i=1; i<=ph.nface; i++) {
    ostr << "face(" << i << ")=" << ph.pF[i] << std::endl;
  }
  return ostr;
}

SbPolyhedron::SbPolyhedron(const SbPolyhedron &from)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron copy constructor             Date:    23.07.96  *
 * Author: E.Chernyaev (IHEP/Protvino)              Revised:           *
 *                                                                     *
 ***********************************************************************/
{
  if (from.nvert > 0 && from.nface > 0) {
    nvert = from.nvert;
    nface = from.nface;
    pV = new HVPoint3D[nvert + 1];
    pF = new SbFacet[nface + 1];
    int i;
    for (i=1; i<=nvert; i++) pV[i] = from.pV[i];
    for (i=1; i<=nface; i++) pF[i] = from.pF[i];
  }else{
    nvert = 0; nface = 0; pV = 0; pF = 0;
  }
}

SbPolyhedron & SbPolyhedron::operator=(const SbPolyhedron &from)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron operator =                   Date:    23.07.96  *
 * Author: E.Chernyaev (IHEP/Protvino)              Revised:           *
 *                                                                     *
 * Function: Copy contents of one GEANT4 polyhedron to another         *
 *                                                                     *
 ***********************************************************************/
{
  if (this == &from) return *this;
  delete [] pV;
  delete [] pF;
  if (from.nvert > 0  && from.nface > 0) {
    nvert = from.nvert;
    nface = from.nface;
    pV = new HVPoint3D[nvert + 1];
    pF = new SbFacet[nface + 1];
    int i;
    for (i=1; i<=nvert; i++) pV[i] = from.pV[i];
    for (i=1; i<=nface; i++) pF[i] = from.pF[i];
  }else{
    nvert = 0; nface = 0; pV = 0; pF = 0;
  }
  return *this;
}

int
SbPolyhedron::FindNeighbour(int iFace, int iNode, int iOrder) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::FindNeighbour                Date:    22.11.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Find neighbouring face                                    *
 *                                                                     *
 ***********************************************************************/
{
  int i;
  for (i=0; i<4; i++) {
    if (iNode == iabs(pF[iFace].edge[i].v)) break;
  }
  if (i == 4) {
    std::cerr
      << "SbPolyhedron::FindNeighbour: face " << iFace
      << " has no node " << iNode
      << std::endl;
    return 0;
  }
  if (iOrder < 0) {
    if ( --i < 0) i = 3;
    if (pF[iFace].edge[i].v == 0) i = 2;
  }
  return (pF[iFace].edge[i].v > 0) ? 0 : pF[iFace].edge[i].f;
}

HVNormal3D SbPolyhedron::FindNodeNormal(int iFace, int iNode) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::FindNodeNormal               Date:    22.11.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Find normal at given node                                 *
 *                                                                     *
 ***********************************************************************/
{
  HVNormal3D  normal = GetUnitNormal(iFace);
  int          k = iFace, iOrder = 1, n = 1;

  for(;;) {
    k = FindNeighbour(k, iNode, iOrder);
    if (k == iFace) break;
    if (k > 0) {
      n++;
      normal += GetUnitNormal(k);
    }else{
      if (iOrder < 0) break;
      k = iFace;
      iOrder = -iOrder;
    }
  }
  normal.normalize();
  return normal;
}

void SbPolyhedron::SetNumberOfRotationSteps(int n)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::SetNumberOfRotationSteps     Date:    24.06.97 *
 * Author: J.Allison (Manchester University)         Revised:          *
 *                                                                     *
 * Function: Set number of steps for whole circle                      *
 *                                                                     *
 ***********************************************************************/
{
  const int nMin = 3;
  if (n < nMin) {
    std::cerr
      << "SbPolyhedron::SetNumberOfRotationSteps: attempt to set the\n"
      << "number of steps per circle < " << nMin << "; forced to " << nMin
      << std::endl;
    fNumberOfRotationSteps = nMin;
  }else{
    fNumberOfRotationSteps = n;
  }
}

void SbPolyhedron::AllocateMemory(int Nvert, int Nface)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::AllocateMemory               Date:    19.06.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Allocate memory for GEANT4 polyhedron                     *
 *                                                                     *
 * Input: Nvert - number of nodes                                      *
 *        Nface - number of faces                                      *
 *                                                                     *
 ***********************************************************************/
{
  nvert = Nvert;
  nface = Nface;
  pV    = new HVPoint3D[nvert+1];
  pF    = new SbFacet[nface+1];
}

void SbPolyhedron::CreatePrism()
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::CreatePrism                  Date:    15.07.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Set facets for a prism                                    *
 *                                                                     *
 ***********************************************************************/
{
  enum {DUMMY, BOTTOM, LEFT, BACK, RIGHT, FRONT, TOP};

  pF[1] = SbFacet(1,LEFT,  4,BACK,  3,RIGHT,  2,FRONT);
  pF[2] = SbFacet(5,TOP,   8,BACK,  4,BOTTOM, 1,FRONT);
  pF[3] = SbFacet(8,TOP,   7,RIGHT, 3,BOTTOM, 4,LEFT);
  pF[4] = SbFacet(7,TOP,   6,FRONT, 2,BOTTOM, 3,BACK);
  pF[5] = SbFacet(6,TOP,   5,LEFT,  1,BOTTOM, 2,RIGHT);
  pF[6] = SbFacet(5,FRONT, 6,RIGHT, 7,BACK,   8,LEFT);
}

void SbPolyhedron::RotateEdge(int k1, int k2, double r1, double r2,
                              int v1, int v2, int vEdge,
                              bool ifWholeCircle, int ns, int &kface)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::RotateEdge                   Date:    05.12.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Create set of facets by rotation of an edge around Z-axis *
 *                                                                     *
 * Input: k1, k2 - end vertices of the edge                            *
 *        r1, r2 - radiuses of the end vertices                        *
 *        v1, v2 - visibility of edges produced by rotation of the end *
 *                 vertices                                            *
 *        vEdge  - visibility of the edge                              *
 *        ifWholeCircle - is true in case of whole circle rotation     *
 *        ns     - number of discrete steps                            *
 *        r[]    - r-coordinates                                       *
 *        kface  - current free cell in the pF array                   *
 *                                                                     *
 ***********************************************************************/
{
  if (r1 == 0. && r2 == 0) return;

  int i;
  int i1  = k1;
  int i2  = k2;
  int ii1 = ifWholeCircle ? i1 : i1+ns;
  int ii2 = ifWholeCircle ? i2 : i2+ns;
  int vv  = ifWholeCircle ? vEdge : 1;

  if (ns == 1) {
    if (r1 == 0.) {
      pF[kface++]   = SbFacet(i1,0,    v2*i2,0, (i2+1),0);
    }else if (r2 == 0.) {
      pF[kface++]   = SbFacet(i1,0,    i2,0,    v1*(i1+1),0);
    }else{
      pF[kface++]   = SbFacet(i1,0,    v2*i2,0, (i2+1),0, v1*(i1+1),0);
    }
  }else{
    if (r1 == 0.) {
      pF[kface++]   = SbFacet(vv*i1,0,    v2*i2,0, vEdge*(i2+1),0);
      for (i2++,i=1; i<ns-1; i2++,i++) {
        pF[kface++] = SbFacet(vEdge*i1,0, v2*i2,0, vEdge*(i2+1),0);
      }
      pF[kface++]   = SbFacet(vEdge*i1,0, v2*i2,0, vv*ii2,0);
    }else if (r2 == 0.) {
      pF[kface++]   = SbFacet(vv*i1,0,    vEdge*i2,0, v1*(i1+1),0);
      for (i1++,i=1; i<ns-1; i1++,i++) {
        pF[kface++] = SbFacet(vEdge*i1,0, vEdge*i2,0, v1*(i1+1),0);
      }
      pF[kface++]   = SbFacet(vEdge*i1,0, vv*i2,0,    v1*ii1,0);
    }else{
      pF[kface++]   = SbFacet(vv*i1,0,    v2*i2,0, vEdge*(i2+1),0,v1*(i1+1),0);
      for (i1++,i2++,i=1; i<ns-1; i1++,i2++,i++) {
        pF[kface++] = SbFacet(vEdge*i1,0, v2*i2,0, vEdge*(i2+1),0,v1*(i1+1),0);
      }
      pF[kface++]   = SbFacet(vEdge*i1,0, v2*i2,0, vv*ii2,0,      v1*ii1,0);
    }
  }
}

void SbPolyhedron::SetSideFacets(int ii[4], int vv[4],
                                 int *kk, double *r,
                                 double dphi, int ns, int &kface)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::SetSideFacets                Date:    20.05.97 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Set side facets for the case of incomplete rotation       *
 *                                                                     *
 * Input: ii[4] - indeces of original verteces                         *
 *        vv[4] - visibility of edges                                  *
 *        kk[]  - indeces of nodes                                     *
 *        r[]   - radiuses                                             *
 *        dphi  - delta phi                                            *
 *        ns     - number of discrete steps                            *
 *        kface  - current free cell in the pF array                   *
 *                                                                     *
 ***********************************************************************/
{
  int k1, k2, k3, k4;
  if (fabs(dphi-M_PI) < perMillion) {          // half a circle
    for (int i=0; i<4; i++) {
      k1 = ii[i];
      k2 = (i == 3) ? ii[0] : ii[i+1];
      if (r[k1] == 0. && r[k2] == 0.) vv[i] = -1;
    }
  }

  if (ii[1] == ii[2]) {
    k1 = kk[ii[0]];
    k2 = kk[ii[2]];
    k3 = kk[ii[3]];
    pF[kface++] = SbFacet(vv[0]*k1,0, vv[2]*k2,0, vv[3]*k3,0);
    if (r[ii[0]] != 0.) k1 += ns;
    if (r[ii[2]] != 0.) k2 += ns;
    if (r[ii[3]] != 0.) k3 += ns;
    pF[kface++] = SbFacet(vv[2]*k3,0, vv[0]*k2,0, vv[3]*k1,0);
  }else if (kk[ii[0]] == kk[ii[1]]) {
    k1 = kk[ii[0]];
    k2 = kk[ii[2]];
    k3 = kk[ii[3]];
    pF[kface++] = SbFacet(vv[1]*k1,0, vv[2]*k2,0, vv[3]*k3,0);
    if (r[ii[0]] != 0.) k1 += ns;
    if (r[ii[2]] != 0.) k2 += ns;
    if (r[ii[3]] != 0.) k3 += ns;
    pF[kface++] = SbFacet(vv[2]*k3,0, vv[1]*k2,0, vv[3]*k1,0);
  }else if (kk[ii[2]] == kk[ii[3]]) {
    k1 = kk[ii[0]];
    k2 = kk[ii[1]];
    k3 = kk[ii[2]];
    pF[kface++] = SbFacet(vv[0]*k1,0, vv[1]*k2,0, vv[3]*k3,0);
    if (r[ii[0]] != 0.) k1 += ns;
    if (r[ii[1]] != 0.) k2 += ns;
    if (r[ii[2]] != 0.) k3 += ns;
    pF[kface++] = SbFacet(vv[1]*k3,0, vv[0]*k2,0, vv[3]*k1,0);
  }else{
    k1 = kk[ii[0]];
    k2 = kk[ii[1]];
    k3 = kk[ii[2]];
    k4 = kk[ii[3]];
    pF[kface++] = SbFacet(vv[0]*k1,0, vv[1]*k2,0, vv[2]*k3,0, vv[3]*k4,0);
    if (r[ii[0]] != 0.) k1 += ns;
    if (r[ii[1]] != 0.) k2 += ns;
    if (r[ii[2]] != 0.) k3 += ns;
    if (r[ii[3]] != 0.) k4 += ns;
    pF[kface++] = SbFacet(vv[2]*k4,0, vv[1]*k3,0, vv[0]*k2,0, vv[3]*k1,0);
  }
}

void SbPolyhedron::RotateAroundZ(int nstep, double phi, double dphi,
                                 int np1, int np2,
                                 const double *z, double *r,
                                 int nodeVis, int edgeVis)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::RotateAroundZ                Date:    27.11.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Create SbPolyhedron for a solid produced by rotation of  *
 *           two polylines around Z-axis                               *
 *                                                                     *
 * Input: nstep - number of discrete steps, if 0 then default          *
 *        phi   - starting phi angle                                   *
 *        dphi  - delta phi                                            *
 *        np1   - number of points in external polyline                *
 *                (must be negative in case of closed polyline)        *
 *        np2   - number of points in internal polyline (may be 1)     *
 *        z[]   - z-coordinates (+z >>> -z for both polylines)         *
 *        r[]   - r-coordinates                                        *
 *        nodeVis - how to Draw edges joing consecutive positions of   *
 *                  node during rotation                               *
 *        edgeVis - how to Draw edges                                  *
 *                                                                     *
 ***********************************************************************/
{
  static double wholeCircle   = 2*M_PI;

  //   S E T   R O T A T I O N   P A R A M E T E R S

  bool ifWholeCircle = (fabs(dphi-wholeCircle) < perMillion) ?
    true : false;
  double   delPhi  = ifWholeCircle ? wholeCircle : dphi;
  int        nSphi    = (nstep > 0) ?
    nstep : int(delPhi*GetNumberOfRotationSteps()/wholeCircle+.5);
  if (nSphi == 0) nSphi = 1;
  int        nVphi    = ifWholeCircle ? nSphi : nSphi+1;
  bool ifClosed = np1 > 0 ? false : true;

  //   C O U N T   V E R T E C E S

  int absNp1 = iabs(np1);
  int absNp2 = iabs(np2);
  int i1beg = 0;
  int i1end = absNp1-1;
  int i2beg = absNp1;
  int i2end = absNp1+absNp2-1;
  int i, j, k;

  for(i=i1beg; i<=i2end; i++) {
    if (fabs(r[i]) < perMillion) r[i] = 0.;
  }

  j = 0;                                                // external nodes
  for (i=i1beg; i<=i1end; i++) {
    j += (r[i] == 0.) ? 1 : nVphi;
  }

  bool ifSide1 = false;                           // internal nodes
  bool ifSide2 = false;

  if (r[i2beg] != r[i1beg] || z[i2beg] != z[i1beg]) {
    j += (r[i2beg] == 0.) ? 1 : nVphi;
    ifSide1 = true;
  }

  for(i=i2beg+1; i<i2end; i++) {
    j += (r[i] == 0.) ? 1 : nVphi;
  }

  if (r[i2end] != r[i1end] || z[i2end] != z[i1end]) {
    if (absNp2 > 1) j += (r[i2end] == 0.) ? 1 : nVphi;
    ifSide2 = true;
  }

  //   C O U N T   F A C E S

  k = ifClosed ? absNp1*nSphi : (absNp1-1)*nSphi;       // external faces

  if (absNp2 > 1) {                                     // internal faces
    for(i=i2beg; i<i2end; i++) {
      if (r[i] > 0. || r[i+1] > 0.)       k += nSphi;
    }

    if (ifClosed) {
      if (r[i2end] > 0. || r[i2beg] > 0.) k += nSphi;
    }
  }

  if (!ifClosed) {                                      // side faces
    if (ifSide1 && (r[i1beg] > 0. || r[i2beg] > 0.)) k += nSphi;
    if (ifSide2 && (r[i1end] > 0. || r[i2end] > 0.)) k += nSphi;
  }

  if (!ifWholeCircle) {                                 // phi_side faces
    k += ifClosed ? 2*absNp1 : 2*(absNp1-1);
  }

  //   A L L O C A T E   M E M O R Y

  AllocateMemory(j, k);

  //   G E N E R A T E   V E R T E C E S

  int *kk;
  kk = new int[absNp1+absNp2];

  k = 1;
  for(i=i1beg; i<=i1end; i++) {
    kk[i] = k;
    if (r[i] == 0.) { pV[k++] = HVPoint3D(0, 0, z[i]); } else { k += nVphi; }
  }

  i = i2beg;
  if (ifSide1) {
    kk[i] = k;
    if (r[i] == 0.) { pV[k++] = HVPoint3D(0, 0, z[i]); } else { k += nVphi; }
  }else{
    kk[i] = kk[i1beg];
  }

  for(i=i2beg+1; i<i2end; i++) {
    kk[i] = k;
    if (r[i] == 0.) { pV[k++] = HVPoint3D(0, 0, z[i]); } else { k += nVphi; }
  }

  if (absNp2 > 1) {
    i = i2end;
    if (ifSide2) {
      kk[i] = k;
      if (r[i] == 0.) pV[k] = HVPoint3D(0, 0, z[i]);
    }else{
      kk[i] = kk[i1end];
    }
  }

  double cosPhi, sinPhi;

  for(j=0; j<nVphi; j++) {
    cosPhi = cos(phi+j*delPhi/nSphi);
    sinPhi = sin(phi+j*delPhi/nSphi);
    for(i=i1beg; i<=i2end; i++) {
      if (r[i] != 0.) pV[kk[i]+j] = HVPoint3D(r[i]*cosPhi,r[i]*sinPhi,z[i]);
    }
  }

  //   G E N E R A T E   E X T E R N A L   F A C E S

  int v1,v2;

  k = 1;
  v2 = ifClosed ? nodeVis : 1;
  for(i=i1beg; i<i1end; i++) {
    v1 = v2;
    if (!ifClosed && i == i1end-1) {
      v2 = 1;
    }else{
      v2 = (r[i] == r[i+1] && r[i+1] == r[i+2]) ? -1 : nodeVis;
    }
    RotateEdge(kk[i], kk[i+1], r[i], r[i+1], v1, v2,
               edgeVis, ifWholeCircle, nSphi, k);
  }
  if (ifClosed) {
    RotateEdge(kk[i1end], kk[i1beg], r[i1end],r[i1beg], nodeVis, nodeVis,
               edgeVis, ifWholeCircle, nSphi, k);
  }

  //   G E N E R A T E   I N T E R N A L   F A C E S

  if (absNp2 > 1) {
    v2 = ifClosed ? nodeVis : 1;
    for(i=i2beg; i<i2end; i++) {
      v1 = v2;
      if (!ifClosed && i==i2end-1) {
        v2 = 1;
      }else{
        v2 = (r[i] == r[i+1] && r[i+1] == r[i+2]) ? -1 :  nodeVis;
      }
      RotateEdge(kk[i+1], kk[i], r[i+1], r[i], v2, v1,
                 edgeVis, ifWholeCircle, nSphi, k);
    }
    if (ifClosed) {
      RotateEdge(kk[i2beg], kk[i2end], r[i2beg], r[i2end], nodeVis, nodeVis,
                 edgeVis, ifWholeCircle, nSphi, k);
    }
  }

  //   G E N E R A T E   S I D E   F A C E S

  if (!ifClosed) {
    if (ifSide1) {
      RotateEdge(kk[i2beg], kk[i1beg], r[i2beg], r[i1beg], 1, 1,
                 -1, ifWholeCircle, nSphi, k);
    }
    if (ifSide2) {
      RotateEdge(kk[i1end], kk[i2end], r[i1end], r[i2end], 1, 1,
                 -1, ifWholeCircle, nSphi, k);
    }
  }

  //   G E N E R A T E   S I D E   F A C E S  for the case of incomplete circle

  if (!ifWholeCircle) {

    int  ii[4], vv[4];

    if (ifClosed) {
      for (i=i1beg; i<=i1end; i++) {
        ii[0] = i;
        ii[3] = (i == i1end) ? i1beg : i+1;
        ii[1] = (absNp2 == 1) ? i2beg : ii[0]+absNp1;
        ii[2] = (absNp2 == 1) ? i2beg : ii[3]+absNp1;
        vv[0] = -1;
        vv[1] = 1;
        vv[2] = -1;
        vv[3] = 1;
        SetSideFacets(ii, vv, kk, r, dphi, nSphi, k);
      }
    }else{
      for (i=i1beg; i<i1end; i++) {
        ii[0] = i;
        ii[3] = i+1;
        ii[1] = (absNp2 == 1) ? i2beg : ii[0]+absNp1;
        ii[2] = (absNp2 == 1) ? i2beg : ii[3]+absNp1;
        vv[0] = (i == i1beg)   ? 1 : -1;
        vv[1] = 1;
        vv[2] = (i == i1end-1) ? 1 : -1;
        vv[3] = 1;
        SetSideFacets(ii, vv, kk, r, dphi, nSphi, k);
      }
    }
  }

  delete [] kk;

  if (k-1 != nface) {
    std::cerr
      << "Polyhedron::RotateAroundZ: number of generated faces ("
      << k-1 << ") is not equal to the number of allocated faces ("
      << nface << ")"
      << std::endl;
  }
}

void SbPolyhedron::SetReferences()
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::SetReferences                Date:    04.12.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: For each edge set reference to neighbouring facet         *
 *                                                                     *
 ***********************************************************************/
{
  if (nface <= 0) return;

  struct edgeListMember {
    edgeListMember *next;
    int v2;
    int iface;
    int iedge;
  } *edgeList, *freeList, **headList;


  //   A L L O C A T E   A N D   I N I T I A T E   L I S T S

  edgeList = new edgeListMember[2*nface];
  headList = new edgeListMember*[nvert];

  int i;
  for (i=0; i<nvert; i++) {
    headList[i] = 0;
  }
  freeList = edgeList;
  for (i=0; i<2*nface-1; i++) {
    edgeList[i].next = &edgeList[i+1];
  }
  edgeList[2*nface-1].next = 0;

  //   L O O P   A L O N G   E D G E S

  int iface, iedge, nedge, i1, i2, k1, k2;
  edgeListMember *prev, *cur;

  for(iface=1; iface<=nface; iface++) {
    nedge = (pF[iface].edge[3].v == 0) ? 3 : 4;
    for (iedge=0; iedge<nedge; iedge++) {
      i1 = iedge;
      i2 = (iedge < nedge-1) ? iedge+1 : 0;
      i1 = iabs(pF[iface].edge[i1].v);
      i2 = iabs(pF[iface].edge[i2].v);
      k1 = (i1 < i2) ? i1 : i2;          // k1 = ::min(i1,i2);
      k2 = (i1 > i2) ? i1 : i2;          // k2 = ::max(i1,i2);

      // check head of the List corresponding to k1
      cur = headList[k1];
      if (cur == 0) {
        headList[k1] = freeList;
        freeList = freeList->next;
        cur = headList[k1];
        cur->next = 0;
        cur->v2 = k2;
        cur->iface = iface;
        cur->iedge = iedge;
        continue;
      }

      if (cur->v2 == k2) {
        headList[k1] = cur->next;
        cur->next = freeList;
        freeList = cur;
        pF[iface].edge[iedge].f = cur->iface;
        pF[cur->iface].edge[cur->iedge].f = iface;
        i1 = (pF[iface].edge[iedge].v < 0) ? -1 : 1;
        i2 = (pF[cur->iface].edge[cur->iedge].v < 0) ? -1 : 1;
        if (i1 != i2) {
          std::cerr
            << "Polyhedron::SetReferences: different edge visibility "
            << iface << "/" << iedge << "/"
            << pF[iface].edge[iedge].v << " and "
            << cur->iface << "/" << cur->iedge << "/"
            << pF[cur->iface].edge[cur->iedge].v
            << std::endl;
        }
        continue;
      }

      // check List itself
      for (;;) {
        prev = cur;
        cur = prev->next;
        if (cur == 0) {
          prev->next = freeList;
          freeList = freeList->next;
          cur = prev->next;
          cur->next = 0;
          cur->v2 = k2;
          cur->iface = iface;
          cur->iedge = iedge;
          break;
        }

        if (cur->v2 == k2) {
          prev->next = cur->next;
          cur->next = freeList;
          freeList = cur;
          pF[iface].edge[iedge].f = cur->iface;
          pF[cur->iface].edge[cur->iedge].f = iface;
          i1 = (pF[iface].edge[iedge].v < 0) ? -1 : 1;
          i2 = (pF[cur->iface].edge[cur->iedge].v < 0) ? -1 : 1;
            if (i1 != i2) {
              std::cerr
                << "Polyhedron::SetReferences: different edge visibility "
                << iface << "/" << iedge << "/"
                << pF[iface].edge[iedge].v << " and "
                << cur->iface << "/" << cur->iedge << "/"
                << pF[cur->iface].edge[cur->iedge].v
                << std::endl;
            }
          break;
        }
      }
    }
  }

  //  C H E C K   T H A T   A L L   L I S T S   A R E   E M P T Y

  for (i=0; i<nvert; i++) {
    if (headList[i] != 0) {
      std::cerr
        << "Polyhedron::SetReferences: List " << i << " is not empty"
        << std::endl;
    }
  }

  //   F R E E   M E M O R Y

  delete [] edgeList;
  delete [] headList;
}

void SbPolyhedron::InvertFacets()
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::InvertFacets                Date:    01.12.99  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Invert the order of the nodes in the facets               *
 *                                                                     *
 ***********************************************************************/
{
  if (nface <= 0) return;
  int i, k, nnode, v[4],f[4];
  for (i=1; i<=nface; i++) {
    nnode =  (pF[i].edge[3].v == 0) ? 3 : 4;
    for (k=0; k<nnode; k++) {
      v[k] = (k+1 == nnode) ? pF[i].edge[0].v : pF[i].edge[k+1].v;
      if (v[k] * pF[i].edge[k].v < 0) v[k] = -v[k];
      f[k] = pF[i].edge[k].f;
    }
    for (k=0; k<nnode; k++) {
      pF[i].edge[nnode-1-k].v = v[k];
      pF[i].edge[nnode-1-k].f = f[k];
    }
  }
}


// rbianchi change
//- original transform method
/*
SbPolyhedron & SbPolyhedron::Transform(const HVRotation & rotation, const HVVector3D & translation)
// ***********************************************************************
// *                                                                     *
// * Name: SbPolyhedron::Transform                    Date:    01.12.99  *
// * Author: E.Chernyaev                              Revised:           *
// *                                                                     *
// * Function: Make transformation of the polyhedron                     *
// *                                                                     *
// ***********************************************************************
{
  if (nvert > 0) {
    for (int i=1; i<=nvert; i++) {
      HVVector3D tmp;
      rotation.multVec(pV[i],tmp);
      pV[i] = tmp+translation;
    }

    //  C H E C K   D E T E R M I N A N T   A N D
    //  I N V E R T   F A C E T S   I F   I T   I S   N E G A T I V E

    HVVector3D x;  rotation.multVec(HVVector3D(1,0,0),x);
    HVVector3D y;  rotation.multVec(HVVector3D(0,1,0),y);
    HVVector3D z;  rotation.multVec(HVVector3D(0,0,1),z);
    if ((x.cross(y)).dot(z) < 0) InvertFacets();
  }
  return *this;
}
*/
//- new transform method from the newest OpenScientist/HEPVis
SbPolyhedron & SbPolyhedron::Transform(
 const HEPVis::SbRotation& rotation
,const SbVec3d& translation
)
{
  if (nvert > 0) {
    SbVec3d tmp;
    for (int i=1; i<=nvert; i++) {
      rotation.multVec(pV[i],tmp);
      pV[i] = tmp+translation;
    }
    SbVec3d x;  rotation.multVec(SbVec3d(1,0,0),x);
    SbVec3d y;  rotation.multVec(SbVec3d(0,1,0),y);
    SbVec3d z;  rotation.multVec(SbVec3d(0,0,1),z);
    if ((x.cross(y)).dot(z) < 0) InvertFacets();
  }
  return *this;
}
//---





bool SbPolyhedron::GetNextVertexIndex(int &index, int &edgeFlag) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextVertexIndex          Date:    03.09.96  *
 * Author: Yasuhide Sawada                          Revised:           *
 *                                                                     *
 * Function:                                                           *
 *                                                                     *
 ***********************************************************************/
{
  static int iFace = 1;
  static int iQVertex = 0;
  int vIndex = pF[iFace].edge[iQVertex].v;

  edgeFlag = (vIndex > 0) ? 1 : 0;
  index = iabs(vIndex);

  if (iQVertex >= 3 || pF[iFace].edge[iQVertex+1].v == 0) {
    iQVertex = 0;
    if (++iFace > nface) iFace = 1;
    return false;  // Last Edge
  }else{
    ++iQVertex;
    return true;  // not Last Edge
  }
}

HVPoint3D SbPolyhedron::GetVertex(int index) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetVertex                   Date:    03.09.96  *
 * Author: Yasuhide Sawada                          Revised: 17.11.99  *
 *                                                                     *
 * Function: Get vertex of the index.                                  *
 *                                                                     *
 ***********************************************************************/
{
  if (index <= 0 || index > nvert) {
    std::cerr
      << "SbPolyhedron::GetVertex: irrelevant index " << index
      << std::endl;
    return HVPoint3D();
  }
  return pV[index];
}


// rbianchi - 14.12.2012
const HVPoint3D& SbPolyhedron::GetVertexFast(int index) const { //G.Barrand
  return pV[index];
}
//---


bool
SbPolyhedron::GetNextVertex(HVPoint3D &vertex, int &edgeFlag) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextVertex               Date:    22.07.96  *
 * Author: John Allison                             Revised:           *
 *                                                                     *
 * Function: Get vertices of the quadrilaterals in order for each      *
 *           face in face order.  Returns false when finished each     *
 *           face.                                                     *
 *                                                                     *
 ***********************************************************************/
{
  int index;
  bool rep = GetNextVertexIndex(index, edgeFlag);
  vertex = pV[index];
  return rep;
}

bool SbPolyhedron::GetNextVertex(HVPoint3D &vertex, int &edgeFlag,
                                       HVNormal3D &normal) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextVertex               Date:    26.11.99  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get vertices with normals of the quadrilaterals in order  *
 *           for each face in face order.                              *
 *           Returns false when finished each face.                    *
 *                                                                     *
 ***********************************************************************/
{
  static int iFace = 1;
  static int iNode = 0;

  if (nface == 0) return false;  // empty polyhedron

  int k = pF[iFace].edge[iNode].v;
  if (k > 0) { edgeFlag = 1; } else { edgeFlag = -1; k = -k; }
  vertex = pV[k];
  normal = FindNodeNormal(iFace,k);
  if (iNode >= 3 || pF[iFace].edge[iNode+1].v == 0) {
    iNode = 0;
    if (++iFace > nface) iFace = 1;
    return false;                // last node
  }else{
    ++iNode;
    return true;                 // not last node
  }
}

bool SbPolyhedron::GetNextEdgeIndeces(int &i1, int &i2, int &edgeFlag,
                                            int &iface1, int &iface2) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextEdgeIndeces          Date:    30.09.96  *
 * Author: E.Chernyaev                              Revised: 17.11.99  *
 *                                                                     *
 * Function: Get indeces of the next edge together with indeces of     *
 *           of the faces which share the edge.                        *
 *           Returns false when the last edge.                         *
 *                                                                     *
 ***********************************************************************/
{
  static int iFace    = 1;
  static int iQVertex = 0;
  static int iOrder   = 1;
  int  k1, k2, kflag, kface1, kface2;

  if (iFace == 1 && iQVertex == 0) {
    k2 = pF[nface].edge[0].v;
    k1 = pF[nface].edge[3].v;
    if (k1 == 0) k1 = pF[nface].edge[2].v;
    if (iabs(k1) > iabs(k2)) iOrder = -1;
  }

  do {
    k1     = pF[iFace].edge[iQVertex].v;
    kflag  = k1;
    k1     = iabs(k1);
    kface1 = iFace;
    kface2 = pF[iFace].edge[iQVertex].f;
    if (iQVertex >= 3 || pF[iFace].edge[iQVertex+1].v == 0) {
      iQVertex = 0;
      k2 = iabs(pF[iFace].edge[iQVertex].v);
      iFace++;
    }else{
      iQVertex++;
      k2 = iabs(pF[iFace].edge[iQVertex].v);
    }
  } while (iOrder*k1 > iOrder*k2);

  i1 = k1; i2 = k2; edgeFlag = (kflag > 0) ? 1 : 0;
  iface1 = kface1; iface2 = kface2;

  if (iFace > nface) {
    iFace  = 1; iOrder = 1;
    return false;
  }else{
    return true;
  }
}

bool
SbPolyhedron::GetNextEdgeIndeces(int &i1, int &i2, int &edgeFlag) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextEdgeIndeces          Date:    17.11.99  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get indeces of the next edge.                             *
 *           Returns false when the last edge.                         *
 *                                                                     *
 ***********************************************************************/
{
  int kface1, kface2;
  return GetNextEdgeIndeces(i1, i2, edgeFlag, kface1, kface2);
}

bool
SbPolyhedron::GetNextEdge(HVPoint3D &p1,
                           HVPoint3D &p2,
                           int &edgeFlag) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextEdge                 Date:    30.09.96  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get next edge.                                            *
 *           Returns false when the last edge.                         *
 *                                                                     *
 ***********************************************************************/
{
  int i1,i2;
  bool rep = GetNextEdgeIndeces(i1,i2,edgeFlag);
  p1 = pV[i1];
  p2 = pV[i2];
  return rep;
}

bool
SbPolyhedron::GetNextEdge(HVPoint3D &p1, HVPoint3D &p2,
                          int &edgeFlag, int &iface1, int &iface2) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextEdge                 Date:    17.11.99  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get next edge with indeces of the faces which share       *
 *           the edge.                                                 *
 *           Returns false when the last edge.                         *
 *                                                                     *
 ***********************************************************************/
{
  int i1,i2;
  bool rep = GetNextEdgeIndeces(i1,i2,edgeFlag,iface1,iface2);
  p1 = pV[i1];
  p2 = pV[i2];
  return rep;
}

void SbPolyhedron::GetFacet(int iFace, int &n, int *iNodes,
                            int *edgeFlags, int *iFaces) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetFacet                    Date:    15.12.99  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get face by index                                         *
 *                                                                     *
 ***********************************************************************/
{
  if (iFace < 1 || iFace > nface) {
    std::cerr
      << "SbPolyhedron::GetFacet: irrelevant index " << iFace
      << std::endl;
    n = 0;
  }else{
    int i, k;
    for (i=0; i<4; i++) {
      k = pF[iFace].edge[i].v;
      if (k == 0) break;
      if (iFaces != 0) iFaces[i] = pF[iFace].edge[i].f;
      if (k > 0) {
        iNodes[i] = k;
        if (edgeFlags != 0) edgeFlags[i] = 1;
      }else{
        iNodes[i] = -k;
        if (edgeFlags != 0) edgeFlags[i] = -1;
      }
    }
    n = i;
  }
}

void SbPolyhedron::GetFacet(int index, int &n, HVPoint3D *nodes,
                            int *edgeFlags, HVNormal3D *normals) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetFacet                    Date:    17.11.99  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get face by index                                         *
 *                                                                     *
 ***********************************************************************/
{
  int iNodes[4];
  GetFacet(index, n, iNodes, edgeFlags);
  if (n != 0) {
    for (int i=0; i<4; i++) {
      nodes[i] = pV[iNodes[i]];
      if (normals != 0) normals[i] = FindNodeNormal(index,iNodes[i]);
    }
  }
}

bool
SbPolyhedron::GetNextFacet(int &n, HVPoint3D *nodes,
                           int *edgeFlags, HVNormal3D *normals) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextFacet                Date:    19.11.99  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get next face with normals of unit length at the nodes.   *
 *           Returns false when finished all faces.                    *
 *                                                                     *
 ***********************************************************************/
{
  static int iFace = 1;

  if (edgeFlags == 0) {
    GetFacet(iFace, n, nodes);
  }else if (normals == 0) {
    GetFacet(iFace, n, nodes, edgeFlags);
  }else{
    GetFacet(iFace, n, nodes, edgeFlags, normals);
  }

  if (++iFace > nface) {
    iFace  = 1;
    return false;
  }else{
    return true;
  }
}

HVNormal3D SbPolyhedron::GetNormal(int iFace) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNormal                    Date:    19.11.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Get normal of the face given by index                     *
 *                                                                     *
 ***********************************************************************/
{
  if (iFace < 1 || iFace > nface) {
    std::cerr
      << "SbPolyhedron::GetNormal: irrelevant index " << iFace
      << std::endl;
    return HVNormal3D();
  }

  int i0  = iabs(pF[iFace].edge[0].v);
  int i1  = iabs(pF[iFace].edge[1].v);
  int i2  = iabs(pF[iFace].edge[2].v);
  int i3  = iabs(pF[iFace].edge[3].v);
  if (i3 == 0) i3 = i0;
  return (pV[i2] - pV[i0]).cross(pV[i3] - pV[i1]);
}

HVNormal3D SbPolyhedron::GetUnitNormal(int iFace) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNormal                    Date:    19.11.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Get unit normal of the face given by index                *
 *                                                                     *
 ***********************************************************************/
{
  if (iFace < 1 || iFace > nface) {
    std::cerr
      << "SbPolyhedron::GetUnitNormal: irrelevant index " << iFace
      << std::endl;
    return HVNormal3D();
  }

  int i0  = iabs(pF[iFace].edge[0].v);
  int i1  = iabs(pF[iFace].edge[1].v);
  int i2  = iabs(pF[iFace].edge[2].v);
  int i3  = iabs(pF[iFace].edge[3].v);
  if (i3 == 0) i3 = i0;
  HVNormal3D nm = (pV[i2] - pV[i0]).cross(pV[i3] - pV[i1]);
  nm.normalize();
  return nm;
}

bool SbPolyhedron::GetNextNormal(HVNormal3D &normal) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextNormal               Date:    22.07.96  *
 * Author: John Allison                             Revised: 19.11.99  *
 *                                                                     *
 * Function: Get normals of each face in face order.  Returns false    *
 *           when finished all faces.                                  *
 *                                                                     *
 ***********************************************************************/
{
  static int iFace = 1;
  normal = GetNormal(iFace);
  if (++iFace > nface) {
    iFace = 1;
    return false;
  }else{
    return true;
  }
}

bool SbPolyhedron::GetNextUnitNormal(HVNormal3D &normal) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextUnitNormal           Date:    16.09.96  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get normals of unit length of each face in face order.    *
 *           Returns false when finished all faces.                    *
 *                                                                     *
 ***********************************************************************/
{
  bool rep = GetNextNormal(normal);
  normal.normalize();
  return rep;
}

double SbPolyhedron::GetSurfaceArea() const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetSurfaceArea              Date:    25.05.01  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Returns area of the surface of the polyhedron.            *
 *                                                                     *
 ***********************************************************************/
{
  double s = 0.;
  for (int iFace=1; iFace<=nface; iFace++) {
    int i0 = iabs(pF[iFace].edge[0].v);
    int i1 = iabs(pF[iFace].edge[1].v);
    int i2 = iabs(pF[iFace].edge[2].v);
    int i3 = iabs(pF[iFace].edge[3].v);
    if (i3 == 0) i3 = i0;
    s += ((pV[i2] - pV[i0]).cross(pV[i3] - pV[i1])).length();
  }
  return s/2.;
}

double SbPolyhedron::GetVolume() const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetVolume                   Date:    25.05.01  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Returns volume of the polyhedron.                         *
 *                                                                     *
 ***********************************************************************/
{
  double v = 0.;
  for (int iFace=1; iFace<=nface; iFace++) {
    int i0 = iabs(pF[iFace].edge[0].v);
    int i1 = iabs(pF[iFace].edge[1].v);
    int i2 = iabs(pF[iFace].edge[2].v);
    int i3 = iabs(pF[iFace].edge[3].v);
    HVPoint3D g;
    if (i3 == 0) {
      i3 = i0;
      g  = (pV[i0]+pV[i1]+pV[i2]) * (1.0f/3.0f);
    }else{
      g  = (pV[i0]+pV[i1]+pV[i2]+pV[i3]) * 0.25f;
    }
    v += ((pV[i2] - pV[i0]).cross(pV[i3] - pV[i1])).dot(g);
  }
  return v/6.;
}

SbPolyhedronTrd2::SbPolyhedronTrd2(double Dx1, double Dx2,
                                     double Dy1, double Dy2,
                                     double Dz)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedronTrd2                           Date:    22.07.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Create GEANT4 TRD2-trapezoid                              *
 *                                                                     *
 * Input: Dx1 - half-length along X at -Dz           8----7            *
 *        Dx2 - half-length along X ay +Dz        5----6  !            *
 *        Dy1 - half-length along Y ay -Dz        !  4-!--3            *
 *        Dy2 - half-length along Y ay +Dz        1----2               *
 *        Dz  - half-length along Z                                    *
 *                                                                     *
 ***********************************************************************/
{
  AllocateMemory(8,6);

  pV[1] = HVPoint3D(-Dx1,-Dy1,-Dz);
  pV[2] = HVPoint3D( Dx1,-Dy1,-Dz);
  pV[3] = HVPoint3D( Dx1, Dy1,-Dz);
  pV[4] = HVPoint3D(-Dx1, Dy1,-Dz);
  pV[5] = HVPoint3D(-Dx2,-Dy2, Dz);
  pV[6] = HVPoint3D( Dx2,-Dy2, Dz);
  pV[7] = HVPoint3D( Dx2, Dy2, Dz);
  pV[8] = HVPoint3D(-Dx2, Dy2, Dz);

  CreatePrism();
}

SbPolyhedronTrd2::~SbPolyhedronTrd2() {}

SbPolyhedronTrd1::SbPolyhedronTrd1(double Dx1, double Dx2,
                                     double Dy, double Dz)
  : SbPolyhedronTrd2(Dx1, Dx2, Dy, Dy, Dz) {}

SbPolyhedronTrd1::~SbPolyhedronTrd1() {}

SbPolyhedronBox::SbPolyhedronBox(double Dx, double Dy, double Dz)
  : SbPolyhedronTrd2(Dx, Dx, Dy, Dy, Dz) {}

SbPolyhedronBox::~SbPolyhedronBox() {}

SbPolyhedronTrap::SbPolyhedronTrap(double Dz,
                                     double Theta,
                                     double Phi,
                                     double Dy1,
                                     double Dx1,
                                     double Dx2,
                                     double Alp1,
                                     double Dy2,
                                     double Dx3,
                                     double Dx4,
                                     double Alp2)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedronTrap                           Date:    20.11.96 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Create GEANT4 TRAP-trapezoid                              *
 *                                                                     *
 * Input: DZ   - half-length in Z                                      *
 *        Theta,Phi - polar angles of the line joining centres of the  *
 *                    faces at Z=-Dz and Z=+Dz                         *
 *        Dy1  - half-length in Y of the face at Z=-Dz                 *
 *        Dx1  - half-length in X of low edge of the face at Z=-Dz     *
 *        Dx2  - half-length in X of top edge of the face at Z=-Dz     *
 *        Alp1 - angle between Y-axis and the median joining top and   *
 *               low edges of the face at Z=-Dz                        *
 *        Dy2  - half-length in Y of the face at Z=+Dz                 *
 *        Dx3  - half-length in X of low edge of the face at Z=+Dz     *
 *        Dx4  - half-length in X of top edge of the face at Z=+Dz     *
 *        Alp2 - angle between Y-axis and the median joining top and   *
 *               low edges of the face at Z=+Dz                        *
 *                                                                     *
 ***********************************************************************/
{
  double DzTthetaCphi = Dz*tan(Theta)*cos(Phi);
  double DzTthetaSphi = Dz*tan(Theta)*sin(Phi);
  double Dy1Talp1 = Dy1*tan(Alp1);
  double Dy2Talp2 = Dy2*tan(Alp2);

  AllocateMemory(8,6);

  pV[1] = HVPoint3D(-DzTthetaCphi-Dy1Talp1-Dx1,-DzTthetaSphi-Dy1,-Dz);
  pV[2] = HVPoint3D(-DzTthetaCphi-Dy1Talp1+Dx1,-DzTthetaSphi-Dy1,-Dz);
  pV[3] = HVPoint3D(-DzTthetaCphi+Dy1Talp1+Dx2,-DzTthetaSphi+Dy1,-Dz);
  pV[4] = HVPoint3D(-DzTthetaCphi+Dy1Talp1-Dx2,-DzTthetaSphi+Dy1,-Dz);
  pV[5] = HVPoint3D( DzTthetaCphi-Dy2Talp2-Dx3, DzTthetaSphi-Dy2, Dz);
  pV[6] = HVPoint3D( DzTthetaCphi-Dy2Talp2+Dx3, DzTthetaSphi-Dy2, Dz);
  pV[7] = HVPoint3D( DzTthetaCphi+Dy2Talp2+Dx4, DzTthetaSphi+Dy2, Dz);
  pV[8] = HVPoint3D( DzTthetaCphi+Dy2Talp2-Dx4, DzTthetaSphi+Dy2, Dz);

  CreatePrism();
}

SbPolyhedronTrap::~SbPolyhedronTrap() {}

SbPolyhedronPara::SbPolyhedronPara(double Dx, double Dy, double Dz,
                                     double Alpha, double Theta,
                                     double Phi)
  : SbPolyhedronTrap(Dz, Theta, Phi, Dy, Dx, Dx, Alpha, Dy, Dx, Dx, Alpha) {}

SbPolyhedronPara::~SbPolyhedronPara() {}

SbPolyhedronCons::SbPolyhedronCons(double Rmn1,
                                     double Rmx1,
                                     double Rmn2,
                                     double Rmx2,
                                     double Dz,
                                     double Phi1,
                                     double Dphi)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedronCons::SbPolyhedronCons        Date:    15.12.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised: 15.12.96 *
 *                                                                     *
 * Function: Constructor for CONS, TUBS, CONE, TUBE                    *
 *                                                                     *
 * Input: Rmn1, Rmx1 - inside and outside radiuses at -Dz              *
 *        Rmn2, Rmx2 - inside and outside radiuses at +Dz              *
 *        Dz         - half length in Z                                *
 *        Phi1       - starting angle of the segment                   *
 *        Dphi       - segment range                                   *
 *                                                                     *
 ***********************************************************************/
{
  static double wholeCircle=2*M_PI;

  //   C H E C K   I N P U T   P A R A M E T E R S

  int k = 0;
  if (Rmn1 < 0. || Rmx1 < 0. || Rmn2 < 0. || Rmx2 < 0.)        k = 1;
  if (Rmn1 > Rmx1 || Rmn2 > Rmx2)                              k = 1;
  if (Rmn1 == Rmx1 && Rmn2 == Rmx2)                            k = 1;

  if (Dz <= 0.) k += 2;

  double phi1, phi2, dphi;
  if (Dphi < 0.) {
    phi2 = Phi1; phi1 = phi2 - Dphi;
  }else if (Dphi == 0.) {
    phi1 = Phi1; phi2 = phi1 + wholeCircle;
  }else{
    phi1 = Phi1; phi2 = phi1 + Dphi;
  }
  dphi  = phi2 - phi1;
  if (fabs(dphi-wholeCircle) < perMillion) dphi = wholeCircle;
  if (dphi > wholeCircle) k += 4;

  if (k != 0) {
    std::cerr << "SbPolyhedronCone(s)/Tube(s): error in input parameters";
    if ((k & 1) != 0) std::cerr << " (radiuses)";
    if ((k & 2) != 0) std::cerr << " (half-length)";
    if ((k & 4) != 0) std::cerr << " (angles)";
    std::cerr << std::endl;
    std::cerr << " Rmn1=" << Rmn1 << " Rmx1=" << Rmx1;
    std::cerr << " Rmn2=" << Rmn2 << " Rmx2=" << Rmx2;
    std::cerr << " Dz=" << Dz << " Phi1=" << Phi1 << " Dphi=" << Dphi
              << std::endl;
    return;
  }

  //   P R E P A R E   T W O   P O L Y L I N E S

  double zz[4], rr[4];
  zz[0] =  Dz;
  zz[1] = -Dz;
  zz[2] =  Dz;
  zz[3] = -Dz;
  rr[0] =  Rmx2;
  rr[1] =  Rmx1;
  rr[2] =  Rmn2;
  rr[3] =  Rmn1;

  //   R O T A T E    P O L Y L I N E S

  RotateAroundZ(0, phi1, dphi, 2, 2, zz, rr, -1, -1);
  SetReferences();
}

SbPolyhedronCons::~SbPolyhedronCons() {}

SbPolyhedronCone::SbPolyhedronCone(double Rmn1, double Rmx1,
                                     double Rmn2, double Rmx2,
                                     double Dz) :
  SbPolyhedronCons(Rmn1, Rmx1, Rmn2, Rmx2, Dz, 0*deg, 360*deg) {}

SbPolyhedronCone::~SbPolyhedronCone() {}

SbPolyhedronTubs::SbPolyhedronTubs(double Rmin, double Rmax,
                                     double Dz,
                                     double Phi1, double Dphi)
  :   SbPolyhedronCons(Rmin, Rmax, Rmin, Rmax, Dz, Phi1, Dphi) {}

SbPolyhedronTubs::~SbPolyhedronTubs() {}

SbPolyhedronTube::SbPolyhedronTube (double Rmin, double Rmax,
                                      double Dz)
  : SbPolyhedronCons(Rmin, Rmax, Rmin, Rmax, Dz, 0*deg, 360*deg) {}

SbPolyhedronTube::~SbPolyhedronTube () {}

SbPolyhedronPgon::SbPolyhedronPgon(double phi,
                                     double dphi,
                                     int    npdv,
                                     int    nz,
                                     const double *z,
                                     const double *rmin,
                                     const double *rmax)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedronPgon                           Date:    09.12.96 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Constructor of polyhedron for PGON, PCON                  *
 *                                                                     *
 * Input: phi  - initial phi                                           *
 *        dphi - delta phi                                             *
 *        npdv - number of steps along phi                             *
 *        nz   - number of z-planes (at least two)                     *
 *        z[]  - z coordinates of the slices                           *
 *        rmin[] - smaller r at the slices                             *
 *        rmax[] - bigger  r at the slices                             *
 *                                                                     *
 ***********************************************************************/
{
  //   C H E C K   I N P U T   P A R A M E T E R S

  if (dphi <= 0. || dphi > 2*M_PI) {
    std::cerr
      << "SbPolyhedronPgon/Pcon: wrong delta phi = " << dphi
      << std::endl;
    return;
  }

  if (nz < 2) {
    std::cerr
      << "SbPolyhedronPgon/Pcon: number of z-planes less than two = " << nz
      << std::endl;
    return;
  }

  if (npdv < 0) {
    std::cerr
      << "SbPolyhedronPgon/Pcon: error in number of phi-steps =" << npdv
      << std::endl;
    return;
  }

  int i;
  for (i=0; i<nz; i++) {
    if (rmin[i] < 0. || rmax[i] < 0. || rmin[i] > rmax[i]) {
      std::cerr
        << "SbPolyhedronPgon: error in radiuses rmin[" << i << "]="
        << rmin[i] << " rmax[" << i << "]=" << rmax[i]
        << std::endl;
      return;
    }
  }

  //   P R E P A R E   T W O   P O L Y L I N E S

  double *zz, *rr;
  zz = new double[2*nz];
  rr = new double[2*nz];

  if (z[0] > z[nz-1]) {
    for (i=0; i<nz; i++) {
      zz[i]    = z[i];
      rr[i]    = rmax[i];
      zz[i+nz] = z[i];
      rr[i+nz] = rmin[i];
    }
  }else{
    for (i=0; i<nz; i++) {
      zz[i]    = z[nz-i-1];
      rr[i]    = rmax[nz-i-1];
      zz[i+nz] = z[nz-i-1];
      rr[i+nz] = rmin[nz-i-1];
    }
  }

  //   R O T A T E    P O L Y L I N E S

  RotateAroundZ(npdv, phi, dphi, nz, nz, zz, rr, -1, (npdv == 0) ? -1 : 1);
  SetReferences();

  delete [] zz;
  delete [] rr;
}

SbPolyhedronPgon::~SbPolyhedronPgon() {}

SbPolyhedronPcon::SbPolyhedronPcon(double phi, double dphi, int nz,
                                     const double *z,
                                     const double *rmin,
                                     const double *rmax)
  : SbPolyhedronPgon(phi, dphi, 0, nz, z, rmin, rmax) {}

SbPolyhedronPcon::~SbPolyhedronPcon() {}

SbPolyhedronSphere::SbPolyhedronSphere(double rmin, double rmax,
                                       double phi, double dphi,
                                       double the, double dthe)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedronSphere                         Date:    11.12.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Constructor of polyhedron for SPHERE                      *
 *                                                                     *
 * Input: rmin - internal radius                                       *
 *        rmax - external radius                                       *
 *        phi  - initial phi                                           *
 *        dphi - delta phi                                             *
 *        the  - initial theta                                         *
 *        dthe - delta theta                                           *
 *                                                                     *
 ***********************************************************************/
{
  //   C H E C K   I N P U T   P A R A M E T E R S

  if (dphi <= 0. || dphi > 2*M_PI) {
    std::cerr
      << "SbPolyhedronSphere: wrong delta phi = " << dphi
      << std::endl;
    return;
  }

  if (the < 0. || the > M_PI) {
    std::cerr
      << "SbPolyhedronSphere: wrong theta = " << the
      << std::endl;
    return;
  }

  if (dthe <= 0. || dthe > M_PI) {
    std::cerr
      << "SbPolyhedronSphere: wrong delta theta = " << dthe
      << std::endl;
    return;
  }

  if ( (the+dthe >= M_PI) && (the+dthe < M_PI + 2*DBL_EPSILON) )
    dthe = M_PI - the; //G.Barrand : coming from LHCb/S.Ponce.

  if (the+dthe > M_PI) {
    std::cerr
      << "SbPolyhedronSphere: wrong theta + delta theta = "
      << the << " " << dthe
      << std::endl;
    return;
  }

  if (rmin < 0. || rmin >= rmax) {
    std::cerr
      << "SbPolyhedronSphere: error in radiuses"
      << " rmin=" << rmin << " rmax=" << rmax
      << std::endl;
    return;
  }

  //   P R E P A R E   T W O   P O L Y L I N E S

  int ns = (GetNumberOfRotationSteps() + 1) / 2;
  int np1 = int(dthe*ns/M_PI+.5) + 1;
  if (np1 <= 1) np1 = 2;
  int np2 = rmin < perMillion ? 1 : np1;

  double *zz, *rr;
  zz = new double[np1+np2];
  rr = new double[np1+np2];

  double a = dthe/(np1-1);
  double cosa, sina;
  for (int i=0; i<np1; i++) {
    cosa  = cos(the+i*a);
    sina  = sin(the+i*a);
    zz[i] = rmax*cosa;
    rr[i] = rmax*sina;
    if (np2 > 1) {
      zz[i+np1] = rmin*cosa;
      rr[i+np1] = rmin*sina;
    }
  }
  if (np2 == 1) {
    zz[np1] = 0.;
    rr[np1] = 0.;
  }

  //   R O T A T E    P O L Y L I N E S

  RotateAroundZ(0, phi, dphi, np1, np2, zz, rr, -1, -1);
  SetReferences();

  delete [] zz;
  delete [] rr;
}

SbPolyhedronSphere::~SbPolyhedronSphere() {}

SbPolyhedronTorus::SbPolyhedronTorus(double rmin,
                                       double rmax,
                                       double rtor,
                                       double phi,
                                       double dphi)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedronTorus                          Date:    11.12.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Constructor of polyhedron for TORUS                       *
 *                                                                     *
 * Input: rmin - internal radius                                       *
 *        rmax - external radius                                       *
 *        rtor - radius of torus                                       *
 *        phi  - initial phi                                           *
 *        dphi - delta phi                                             *
 *                                                                     *
 ***********************************************************************/
{
  //   C H E C K   I N P U T   P A R A M E T E R S

  if (dphi <= 0. || dphi > 2*M_PI) {
    std::cerr
      << "SbPolyhedronTorus: wrong delta phi = " << dphi
      << std::endl;
    return;
  }

  if (rmin < 0. || rmin >= rmax || rmax >= rtor) {
    std::cerr
      << "SbPolyhedronTorus: error in radiuses"
      << " rmin=" << rmin << " rmax=" << rmax << " rtorus=" << rtor
      << std::endl;
    return;
  }

  //   P R E P A R E   T W O   P O L Y L I N E S

  int np1 = GetNumberOfRotationSteps();
  int np2 = rmin < perMillion ? 1 : np1;

  //double *zz(nullptr), *rr;
  const auto totNumPts = np1+np2;
  auto zz = new double[totNumPts]{};
  auto rr = new double[totNumPts]{};
  for (unsigned int i(0);i!=(unsigned int) totNumPts;++i){
    rr[i]=0.0;
    zz[i]=0.0;
  }

  double a = 2*M_PI/np1;
  double cosa, sina;
  for (int i=0; i<np1; i++) {
    cosa  = cos(i*a);
    sina  = sin(i*a);
    zz[i] = rmax*cosa;
    rr[i] = rtor+rmax*sina;
    if (np2 > 1) {
      zz[i+np1] = rmin*cosa;
      rr[i+np1] = rtor+rmin*sina;
    }
  }
  if (np2 == 1) {
    zz[np1] = 0.;
    rr[np1] = rtor;
    np2 = -1;
  }

  //   R O T A T E    P O L Y L I N E S

  RotateAroundZ(0, phi, dphi, -np1, -np2, zz, rr, -1,-1);
  SetReferences();

  delete [] zz;
  delete [] rr;
}

SbPolyhedronTorus::~SbPolyhedronTorus() {}

int SbPolyhedron::fNumberOfRotationSteps = DEFAULT_NUMBER_OF_STEPS;
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::fNumberOfRotationSteps       Date:    24.06.97 *
 * Author: J.Allison (Manchester University)         Revised:          *
 *                                                                     *
 * Function: Number of steps for whole circle                          *
 *                                                                     *
 ***********************************************************************/

#include "BooleanProcessor.h"
static HEPVis_BooleanProcessor processor;

SbPolyhedron SbPolyhedron::add(const SbPolyhedron & p) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::add                          Date:    19.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Boolean "union" of two polyhedra                          *
 *                                                                     *
 ***********************************************************************/
{
  // rbianchi change
  //return processor.execute(OP_UNION, *this, p);
  HEPVis_BooleanProcessor processor; //G.Barrand
  int err;
  return processor.execute(OP_UNION, *this, p, err);
  //---
}

SbPolyhedron SbPolyhedron::intersect(const SbPolyhedron & p) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::intersect                    Date:    19.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Boolean "intersection" of two polyhedra                   *
 *                                                                     *
 ***********************************************************************/
{
  // rbianchi change
  //return processor.execute(OP_INTERSECTION, *this, p);
  HEPVis_BooleanProcessor processor; //G.Barrand
  int err;
  return processor.execute(OP_INTERSECTION, *this, p, err);
  //---
}

SbPolyhedron SbPolyhedron::subtract(const SbPolyhedron & p) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::add                          Date:    19.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Boolean "subtraction" of "p" from "this"                  *
 *                                                                     *
 ***********************************************************************/
{
  // rbianchi change
  //return processor.execute(OP_SUBTRACTION, *this, p);
  HEPVis_BooleanProcessor processor; //G.Barrand
  int err;
  return processor.execute(OP_SUBTRACTION, *this, p, err);
  //---
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// Now follows code for SbPolyhedronPolygonXSect /////////////////////////
/////////////////////////// Author: Thomas Kittelmann, 2007. //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "PolygonTriangulator.h"
#include <map>
#include <set>

class SbPolyhedronPolygonXSect::Internals {
public:

  typedef std::pair<int,int> Edge;
  typedef PolygonTriangulator::Triangle Triangle;
  typedef PolygonTriangulator::Triangles Triangles;

  // Methods:
  Internals(SbPolyhedronPolygonXSect* sbp) :dz(0),x(0),y(0),n(0),ntriangles(0),sbpolyhedron(sbp),poly(0),
					    nextraexternalvertices(0),
					    nextrainternalvertices(0){};
  ~Internals() { delete poly; }

  void setData(const std::vector<double> * xx,const std::vector<double>* yy, const double& dz);
  void triangulate();
  void initEdgeClassificationsAndNeighbours();
  void addExtraVertices();
  void allocateMemoryAndDefineVertexCoordinates();
  void defineFacesTopology();
  //Data:
  double dz;
  const std::vector<double> * x;
  const std::vector<double> * y;
  unsigned n;
  unsigned ntriangles;
  SbPolyhedronPolygonXSect* sbpolyhedron;

  //The triangulation:
  PolygonTriangulator * poly;
  //Various helper maps, sets, ...:
  std::set<Edge> edges_internal;
  std::set<Edge> edges_external;
  std::map<Edge,const Triangle*> neighbourmap;//In this map the Edges are oriented!
  std::set<Edge> edges_with_extra_vertex;
  unsigned nextraexternalvertices;
  unsigned nextrainternalvertices;
  std::map<Edge,unsigned> edgewithextravertex_2_id;
  std::map<Edge,unsigned> externaledgewithextravertex_2_2ndedgeid;

  //Helper methods:

  bool isinternaledge(const Edge& edge, const std::map<Edge,std::vector<Triangles::const_iterator> >& edge2triangles_map) const;
  Edge GetEdge(const Triangle * tr, const unsigned& i, const bool& oriented=false) const;//If oriented, 2-4 and 4-2 are different. If not, 4-2 is returned as 2-4.
  void setupface(const unsigned& face_id, const std::vector<unsigned>&v) const;
  void setupExternalFace(const unsigned& id_extedge,const unsigned& prev_edgeface,const unsigned& next_edgeface,
			 const unsigned& idfirstvertex,const unsigned& idsecondvertex,
			 const unsigned& id_triangleface,const unsigned& n, std::vector<unsigned>& faceinfo) const;

  void getSurroundingValues(const std::vector<unsigned>& numbercycle, const unsigned& centralValue,
			    unsigned& prevValue, unsigned& nextValue) const;

  unsigned top2bottomvertexid(const unsigned& topid) const;
  unsigned top2bottomfaceid(const unsigned& topid) const;
};


//_______________________________________________________________________________________________________________________
SbPolyhedronPolygonXSect::SbPolyhedronPolygonXSect(const std::vector<double>& x,const std::vector<double>& y, const double& dz)
{

  Internals * internals = new Internals(this);
  internals->setData(&x,&y,dz);
  internals->triangulate();

  internals->initEdgeClassificationsAndNeighbours();
  internals->addExtraVertices();
  internals->allocateMemoryAndDefineVertexCoordinates();
  internals->defineFacesTopology();

  delete internals;
}

//_______________________________________________________________________________________________________________________
SbPolyhedronPolygonXSect::~SbPolyhedronPolygonXSect() {}

//_______________________________________________________________________________________________________________________
void SbPolyhedronPolygonXSect::Internals::setData(const std::vector<double> * xx,const std::vector<double>* yy, const double& _dz)
{
  n = xx->size();
  ntriangles = n-2;
  assert (n==yy->size()&&n>3);//fixme n>2, and special code for n==3.
  dz = _dz;
  x = xx;
  y = yy;
  nextraexternalvertices = nextrainternalvertices = 0;
}

//_______________________________________________________________________________________________________________________
void SbPolyhedronPolygonXSect::Internals::triangulate()
{
  poly = new PolygonTriangulator(*x,*y);
}

//_______________________________________________________________________________________________________________________
void SbPolyhedronPolygonXSect::Internals::initEdgeClassificationsAndNeighbours() {
  //Goal: classify edges as external/internal, get ability to find neighbours.

  //As a help, we create a map of edges->triangles. We define an edge
  //as a pair of int's, where the first should always be lower than
  //the second (otherwise the edges 2-4 and 4-2 would seem different):
  std::map<Edge,std::vector<PolygonTriangulator::Triangles::const_iterator> > edge2triangles_map;
  std::set<Edge> alledges;
  Triangles::const_iterator itt=poly->triangles()->begin();
  Triangles::const_iterator ittE=poly->triangles()->end();
  for(; itt!=ittE; ++itt)
    for (unsigned iedge=0;iedge<3;++iedge) {
      Edge eun = GetEdge(&(*itt),iedge,false/*unoriented!*/);
      if (edge2triangles_map.find(eun)==edge2triangles_map.end()) {
	edge2triangles_map[eun] = std::vector<Triangles::const_iterator>();
	alledges.insert(eun);
      }
      edge2triangles_map[eun].push_back(itt);
    }


  //set of external/internal edges:
  std::set<Edge>::const_iterator itedges = alledges.begin();
  std::set<Edge>::const_iterator itedgesE = alledges.end();
  for (;itedges!=itedgesE;++itedges) {
    if (isinternaledge((*itedges),edge2triangles_map))
      edges_internal.insert(*itedges);
    else
      edges_external.insert(*itedges);
  }

  //Neighbourmap:
  itt=poly->triangles()->begin();
  for(; itt!=ittE; ++itt)
    for (unsigned iedge=0;iedge<3;++iedge) {
      Edge e = GetEdge(&(*itt),iedge,true/*oriented!*/);
      Edge eun =  GetEdge(&(*itt),iedge,false/*unoriented!*/);
      assert(edge2triangles_map.find(eun)!=edge2triangles_map.end());//fixme
      if (edges_external.find(eun)!=edges_external.end()) {
	neighbourmap[e]=0;
	neighbourmap[eun]=0;
      } else {
	assert(edge2triangles_map[eun].size()==2);//fixme
	const Triangle* triangle = static_cast<const Triangle*>( &(*itt) );
	const Triangle* triangle1 = static_cast<const Triangle*>( &(*(edge2triangles_map[eun].front())) );
	const Triangle* triangle2 = static_cast<const Triangle*>( &(*(edge2triangles_map[eun].back())) );
	neighbourmap[e]=(triangle==triangle1?triangle2:triangle1);
      }
    }
}

//_______________________________________________________________________________________________________________________
void SbPolyhedronPolygonXSect::Internals::addExtraVertices() {

  //////////////////////////////////////////////////////////////////////////////////////
  ///////////// Next we need to assign extra vertices to some of the ///////////////////
  ///////////// edges, in order for all faces to have four corners   ///////////////////
  /////////////                                                      ///////////////////
  ///////////// NB: one might wish to gain performance by merging    ///////////////////
  ///////////// neighbouring triangles into quadrilaterals, but the  ///////////////////
  ///////////// HEPVis classes does not handles this well in all     ///////////////////
  ///////////// cases)                                               ///////////////////
  //////////////////////////////////////////////////////////////////////////////////////

  std::set<const Triangle*> triangles_with_extra_vertex;//Eventually, all triangles should end up here.

  /////////// First we must deal with all triangles that have only internal edges ///////////////
  Triangles::const_iterator itt=poly->triangles()->begin();
  Triangles::const_iterator ittE=poly->triangles()->end();
  for(; itt!=ittE; ++itt) {
    unsigned ninternaledges(0);
    for (unsigned iedge=0;iedge<3;++iedge) {
      Edge eun = GetEdge(&(*itt),iedge,false/*unoriented!*/);
      if (edges_internal.find(eun)!=edges_internal.end())
	++ninternaledges;
    }
    if (ninternaledges<3)
      continue;


    //Check the three edges to find a neighbour that has not already become a quadrilateral.
    unsigned iedge(0);
    for (;iedge<3;++iedge) {
      Edge e = GetEdge(&(*itt),iedge,true/*oriented!*/);
      const Triangle* trneighbour = neighbourmap[e];
      assert(trneighbour);//fixme
      if (triangles_with_extra_vertex.find(trneighbour)==triangles_with_extra_vertex.end()) {
	triangles_with_extra_vertex.insert(trneighbour);
	triangles_with_extra_vertex.insert(&(*itt));
	Edge eun = GetEdge(&(*itt),iedge,false/*unoriented*/);
	edges_with_extra_vertex.insert(eun);
	++nextrainternalvertices;
	break;
      }
    }
    assert(iedge<3);//If we cannot add a vertex to a triangle with 3 internal edges, then we have a problem...
  }

  /////////// And now, the rest of the triangles will get extra vertices added to their external edges /////////
  itt=poly->triangles()->begin();
  for(; itt!=ittE; ++itt) {
    Triangle* triangle = const_cast<Triangle*>( &(*itt) );
    if (triangles_with_extra_vertex.find(triangle)!=triangles_with_extra_vertex.end())
      continue;
    for (unsigned iedge=0;iedge<3;++iedge) {
      Edge eun = GetEdge(&(*itt),iedge,false/*unoriented!*/);
      //only look at external edges
      if (edges_external.find(eun)==edges_external.end())
	continue;
      assert(edges_with_extra_vertex.find(eun)==edges_with_extra_vertex.end());//fixme
      triangles_with_extra_vertex.insert(triangle);
      edges_with_extra_vertex.insert(eun);
      ++nextraexternalvertices;
      break;
    }
  }
  assert(ntriangles==triangles_with_extra_vertex.size());
}

//_______________________________________________________________________________________________________________________
void SbPolyhedronPolygonXSect::Internals::allocateMemoryAndDefineVertexCoordinates() {

   const unsigned nfaces = 2*ntriangles+n+nextraexternalvertices;
   const unsigned nvertices = 2*(n+nextraexternalvertices+nextrainternalvertices);
   sbpolyhedron->AllocateMemory(nvertices,nfaces);

  ///////////////////////////////// Make the vertices /////////////////////////

  //Make edgewithextravertex_2_id map.
  std::set<Edge>::const_iterator itl = edges_with_extra_vertex.begin();
  std::set<Edge>::const_iterator itlE = edges_with_extra_vertex.end();
  unsigned il = 2*n;
  unsigned ielev = 3*n-4;
  for (;itl!=itlE;++itl) {
    edgewithextravertex_2_id[*itl]=++il;
    if (edges_external.find(*itl)!=edges_external.end()) {
      externaledgewithextravertex_2_2ndedgeid[*itl]=++ielev;
    }
  }
  //Now place the vertices:

  //First the original n vertices at +dz
  for (unsigned i = 0; i<n; ++i)
    sbpolyhedron->pV[i+1] = HVPoint3D(x->at(i),y->at(i),dz);
  //Then the original n vertices at -dz
  for (unsigned i = 0; i<n; ++i)
    sbpolyhedron->pV[i+1+n] = HVPoint3D(x->at(i),y->at(i),-dz);
  //Extra vertices at +dz.
  std::map<Edge,unsigned>::const_iterator itlid = edgewithextravertex_2_id.begin();
  std::map<Edge,unsigned>::const_iterator itlidE = edgewithextravertex_2_id.end();
  for (; itlid!=itlidE; ++itlid) {
    double xx = 0.5*(x->at(itlid->first.first-1)+x->at(itlid->first.second-1));
    double yy = 0.5*(y->at(itlid->first.first-1)+y->at(itlid->first.second-1));
    sbpolyhedron->pV[itlid->second] = HVPoint3D(xx,yy,dz);
  }
  //Extra vertices at -dz.
  itlid = edgewithextravertex_2_id.begin();
  for (; itlid!=itlidE; ++itlid) {
    double xx = 0.5*(x->at(itlid->first.first-1)+x->at(itlid->first.second-1));
    double yy = 0.5*(y->at(itlid->first.first-1)+y->at(itlid->first.second-1));
    sbpolyhedron->pV[itlid->second+edgewithextravertex_2_id.size()] = HVPoint3D(xx,yy,-dz);
  }

}

//_______________________________________________________________________________________________________________________
bool  SbPolyhedronPolygonXSect::Internals::isinternaledge(const Edge& edge,
						  const std::map<Edge,std::vector<Triangles::const_iterator> >& edge2triangles_map) const {

  Edge reverse_edge(edge.second,edge.first);
  unsigned k = 0;
  std::map<Edge,std::vector<Triangles::const_iterator> >::const_iterator it = edge2triangles_map.find(edge);
  if (it!=edge2triangles_map.end()) k += it->second.size();
  it = edge2triangles_map.find(reverse_edge);
  if (it!=edge2triangles_map.end()) k += it->second.size();
  return k==2;

}

//_______________________________________________________________________________________________________________________
SbPolyhedronPolygonXSect::Internals::Edge  SbPolyhedronPolygonXSect::Internals::GetEdge(const Triangle * tr, const unsigned& i, const bool& oriented ) const {
  assert(i<=2);
  if (i==2)
    return oriented ? Edge((*tr)[2],(*tr)[0]) : Edge((*tr)[0],(*tr)[2]);
  else
    return Edge((*tr)[i],(*tr)[i+1]);
}

//____________________________________________________________________________________________________
void SbPolyhedronPolygonXSect::Internals::setupface(const unsigned& face_id, const std::vector<unsigned>&v) const {
  assert(v.size()==8);
  sbpolyhedron->pF[face_id] = SbFacet(v.at(0),v.at(1),v.at(2),v.at(3),v.at(4),v.at(5),v.at(6),v.at(7));
}


//____________________________________________________________________________________________________
void SbPolyhedronPolygonXSect::Internals::setupExternalFace(const unsigned& id_extedge,const unsigned& prev_edgeface,const unsigned& next_edgeface,
						    const unsigned& idfirstvertex,const unsigned& idsecondvertex,
						    const unsigned& id_triangleface,const unsigned& n, std::vector<unsigned>& faceinfo) const {
  faceinfo.clear();
  faceinfo.push_back(idfirstvertex);//point
  faceinfo.push_back(prev_edgeface);//neighbour face
  faceinfo.push_back(top2bottomvertexid(idfirstvertex));//point
  faceinfo.push_back(id_triangleface+n-2);//neighbour face
  faceinfo.push_back(top2bottomvertexid(idsecondvertex));//point
  faceinfo.push_back(next_edgeface);//neighbour face
  faceinfo.push_back(idsecondvertex);//point
  faceinfo.push_back(id_triangleface);//neighbour face
  setupface(id_extedge,faceinfo);
}


//____________________________________________________________________________________________________
void SbPolyhedronPolygonXSect::Internals::getSurroundingValues(const std::vector<unsigned>& numbercycle, const unsigned& centralValue,
						       unsigned& prevValue, unsigned& nextValue) const
{
  //First, find the index of the centralValue
  unsigned int i;
  for (  i=0/*fixme, not O(n)*/;i<numbercycle.size();++i)
    if (numbercycle.at(i)==centralValue)
      break;
  //Sanity check that the central value was found
  assert(i<numbercycle.size());//fixme
  prevValue = ( i==0 ? numbercycle.back() : numbercycle.at(i-1) );
  nextValue = ( i==numbercycle.size()-1 ? numbercycle.front() : numbercycle.at(i+1) );

}


//_______________________________________________________________________________________________________________________
unsigned SbPolyhedronPolygonXSect::Internals::top2bottomvertexid(const unsigned& topid) const {
  if (topid<=n)
    return topid+n;//original vertex
  return topid+nextraexternalvertices+nextrainternalvertices;//added extra vertex;
}

//_______________________________________________________________________________________________________________________
unsigned SbPolyhedronPolygonXSect::Internals::top2bottomfaceid(const unsigned& topid) const {
  if (topid<=ntriangles)
    return topid+ntriangles;//triangle
  return topid;//side face
}


//_______________________________________________________________________________________________________________________
void SbPolyhedronPolygonXSect::Internals::defineFacesTopology() {
  //This is definitely the most tricky part to get right...

  //Make triangle_2_id map.
  std::map<const Triangle*,unsigned> triangle_2_id;
  Triangles::const_iterator itt=poly->triangles()->begin();
  Triangles::const_iterator ittE=poly->triangles()->end();
  unsigned itri = 0;
  for(; itt!=ittE; ++itt) {
    Triangle* triangle = const_cast<Triangle*>( &(*itt) );
    triangle_2_id[triangle]=++itri;
  }
  assert(triangle_2_id.size()==ntriangles);

  //Loop over the external edges, and get a list of edge face id's.
  std::vector<unsigned> edgeface_ids;
  std::map<Edge,unsigned> edge_2_firstid;
  std::map<Edge,unsigned> edge_2_secondid;
  for (unsigned i = 1; i<=n;++i) {
    Edge eun((i==n?1:i),(i==n?n:i+1));
    edgeface_ids.push_back(2*n-4+i);
    edge_2_firstid[eun]=edgeface_ids.back();
    bool hasextravertex=
      (edges_with_extra_vertex.find(eun)!=edges_with_extra_vertex.end());
    if (hasextravertex) {
      edgeface_ids.push_back(externaledgewithextravertex_2_2ndedgeid[eun]);
      edge_2_secondid[eun]=edgeface_ids.back();
    }
  }

  std::vector<unsigned> faceinfo_top, faceinfo_bottom(8), faceinfo_sides;
  itt = poly->triangles()->begin();
  for(; itt!=ittE; ++itt) {
    unsigned triangleid = triangle_2_id[&(*itt)];

    ////////// Construct faceinfo for the top triangles + the faces at the external sides.
    for (unsigned iedge=0;iedge<3;++iedge) {
      Edge e = GetEdge(&(*itt),iedge,true/*oriented!*/);
      Edge eun = GetEdge(&(*itt),iedge,false/*unoriented!*/);

      bool hasextravertex(edges_with_extra_vertex.find(eun)!=edges_with_extra_vertex.end());
      bool isinternal(edges_external.find(eun)==edges_external.end());
      //add for the triangle.
      faceinfo_top.push_back(e.first);
      if (isinternal) {
	assert(neighbourmap[e]);//fixme
	assert(triangle_2_id.find(neighbourmap[e])!=triangle_2_id.end());
	faceinfo_top.push_back(triangle_2_id[neighbourmap[e]]);
	if (hasextravertex) {
	  faceinfo_top.push_back(edgewithextravertex_2_id[eun]);
	  faceinfo_top.push_back(triangle_2_id[neighbourmap[e]]);//NB: Same neighbourface again!
	  // 	  assert(false&&"need to implement extravertex for internal edges");
	  //fixme!
	}
      } else {
	assert(edge_2_firstid.find(eun)!=edge_2_firstid.end());
	unsigned firstsideid = edge_2_firstid[eun];
	faceinfo_top.push_back(firstsideid);
	if (hasextravertex) {
	  faceinfo_top.push_back(edgewithextravertex_2_id[eun]);
	  assert(edge_2_secondid.find(eun)!=edge_2_secondid.end());
	  faceinfo_top.push_back(edge_2_secondid[eun]);
	}
	////////Add faceinfo for the faces at the external sides:
	unsigned prev_edgeface,next_edgeface;
	getSurroundingValues(edgeface_ids, firstsideid,prev_edgeface, next_edgeface);
	unsigned firstvertexid = e.first;
	unsigned secondvertexid = (hasextravertex? edgewithextravertex_2_id[eun] : e.second);
	setupExternalFace(firstsideid,prev_edgeface, next_edgeface,firstvertexid,secondvertexid,
			  /*firstvertexid+n,secondvertexid + (hasextravertex?externaledgewithextravertex_2_2ndedgeid.size():n),*/
			triangleid,n,faceinfo_sides);
	if (hasextravertex) {
	  firstsideid=next_edgeface;
	  getSurroundingValues(edgeface_ids, firstsideid,prev_edgeface, next_edgeface);
	  firstvertexid=secondvertexid;
	  secondvertexid=e.second;
	  setupExternalFace(firstsideid,prev_edgeface, next_edgeface,
			    edgewithextravertex_2_id[eun],secondvertexid,
			    /*edgewithextravertex_2_id[eun]+externaledgewithextravertex_2_2ndedgeid.size(),secondvertexid+n,*/
			    triangleid,n,faceinfo_sides);
	}

      }
    }//End edge loop

    //////////add for the opposite of the triangle.
    assert(faceinfo_bottom.size()==8);//fixme
    assert(faceinfo_top.size()==8);//fixme
    faceinfo_bottom[0]=top2bottomvertexid(faceinfo_top[0]);
    faceinfo_bottom[1]=top2bottomfaceid(faceinfo_top[7]);
    faceinfo_bottom[2]=top2bottomvertexid(faceinfo_top[6]);
    faceinfo_bottom[3]=top2bottomfaceid(faceinfo_top[5]);
    faceinfo_bottom[4]=top2bottomvertexid(faceinfo_top[4]);
    faceinfo_bottom[5]=top2bottomfaceid(faceinfo_top[3]);
    faceinfo_bottom[6]=top2bottomvertexid(faceinfo_top[2]);
    faceinfo_bottom[7]=top2bottomfaceid(faceinfo_top[1]);

    //Actually setup triangle faces.
    setupface(triangleid,faceinfo_top);
    setupface(triangleid+ntriangles,faceinfo_bottom);
    faceinfo_top.clear();
  }//End triangle loop

}

// ************************************************************************************************ //
// *                                                                                              * //
// *                 Class SbPolyhedronArbitrary for Tessellated Solids                           * //
// *                                                                                              * //
// ************************************************************************************************ //

SbPolyhedronArbitrary::SbPolyhedronArbitrary(const int nVertices, const int nFacets)
{
  AllocateMemory(nVertices, nFacets);
  nVertexCount = 0;
  nFacetCount  = 0;
}

SbPolyhedronArbitrary::~SbPolyhedronArbitrary()
{
}

void SbPolyhedronArbitrary::AddVertex(const double v1, const double v2, const double v3)
{
  if(nVertexCount==nvert+1) {
    std::cerr <<"ERROR in SbPolyhedronArbitrary::AddVertex. Attempt to exceed maximum number of vertices: " 
	      << nVertexCount << std::endl;
  }
  else {
    nVertexCount++;
    pV[nVertexCount] = HVPoint3D(v1,v2,v3);
  }
}
 
void SbPolyhedronArbitrary::AddFacet(const int iv1, const int iv2, const int iv3, const int iv4)
{
  if(nFacetCount==nface) {
    std::cerr <<"ERROR in SbPolyhedronArbitrary::AddFacet. Attempt to exceed maximum number of facets: "
	      << nFacetCount << std::endl;
  }
  else if(iv1 < 1 
	  || iv1 > nvert 
	  || iv2 < 1 
	  || iv2 > nvert 
	  || iv3 < 1 
	  || iv3 > nvert 
	  || iv4 > nvert) {
    std::cerr <<"ERROR in SbPolyhedronArbitrary::AddFacet. Attempt to index vertex number which is out-of-range: ("
	      << iv1 << "," << iv2 << "," << iv3 << "," << iv4 << ")" << std::endl; 
  }
  else if(iv1 > nVertexCount 
	  || iv2 > nVertexCount 
	  || iv3 > nVertexCount 
	  || iv4 > nVertexCount) {
    std::cerr <<"ERROR in SbPolyhedronArbitrary::AddFacet. Vertex needs to be defined first : ("
	      << iv1 << "," << iv2 << "," << iv3 << "," << iv4 << ")" << std::endl;
  }
  else {
    nFacetCount++;
    pF[nFacetCount] = SbFacet(iv1, 0, iv2, 0, iv3, 0, iv4, 0);
  }
}

void SbPolyhedronArbitrary::Finalize()
{
  SetReferences();
}

SbPolyhedronGenericTrap::SbPolyhedronGenericTrap(double Dz, const std::vector<std::pair<double,double> > Vertices)
{
  AllocateMemory(8,6);

  pV[1] = HVPoint3D(Vertices[0].first,Vertices[0].second,-Dz);
  pV[2] = HVPoint3D(Vertices[1].first,Vertices[1].second,-Dz);
  pV[3] = HVPoint3D(Vertices[2].first,Vertices[2].second,-Dz);
  pV[4] = HVPoint3D(Vertices[3].first,Vertices[3].second,-Dz);
  pV[5] = HVPoint3D(Vertices[4].first,Vertices[4].second,Dz);
  pV[6] = HVPoint3D(Vertices[5].first,Vertices[5].second,Dz);
  pV[7] = HVPoint3D(Vertices[6].first,Vertices[6].second,Dz);
  pV[8] = HVPoint3D(Vertices[7].first,Vertices[7].second,Dz);

  CreatePrism();
}

SbPolyhedronGenericTrap::~SbPolyhedronGenericTrap()
{

}
