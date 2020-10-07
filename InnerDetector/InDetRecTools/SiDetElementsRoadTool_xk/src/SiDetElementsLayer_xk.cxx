/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class SiDetElemenstLauerLxk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include <cmath>

#include "SiDetElementsRoadTool_xk/SiDetElementsComparison.h"
#include "SiDetElementsRoadTool_xk/SiDetElementsLayer_xk.h"

///////////////////////////////////////////////////////////////////
// Get barrel detector elements
// Input parameters: startPoint[0] - X     searchDirection[0] - Ax 
//                   startPoint[1] - Y     searchDirection[1] - Ay
//                   startPoint[2] - Z     searchDirection[2] - Az
//                   startPoint[3] - R
//                   startPoint[4] - width
//                   startPoint[5] - step
///////////////////////////////////////////////////////////////////

void InDet::SiDetElementsLayer_xk::getBarrelDetElements
(const std::array<float,6> & startingPoint,
 const std::array<float,3> & searchDirection,
 std::vector<InDet::SiDetElementLink_xk::ElementWay> &lDE,
 std::vector<InDet::SiDetElementRoadMakerData_xk::UsedFlag>   &used) const
{

  /// In the following, identify where we cross the layer in r
  /// by solving the quadratic equation 
  /// ( startingPoint_xy + s * searchDirection_xy )² = r_layer² 
  float minusB  =2*( searchDirection[0]*startingPoint[0]
                    +searchDirection[1]*startingPoint[1]); 
  float C  = (m_r-startingPoint[0]-startingPoint[1])
            *(m_r+startingPoint[0]+startingPoint[1])
            +2.*startingPoint[0]*startingPoint[1];
  float twoA  = 2.*( searchDirection[0]*searchDirection[0]
                    +searchDirection[1]*searchDirection[1]); 
  if(twoA == 0.) return;
  float sq = minusB*minusB+2.*C*twoA;  
  if (sq > 0){ 
    sq=std::sqrt(sq);
  }
  else {
    sq=0.;
  }
  /// solutions of our equation
  float s1 =-(minusB+sq)/twoA;
  float s2 =-(minusB-sq)/twoA; 
  float s;
  /// pick one:
  /// if both solution occur for the same direction, 
  /// we pick the crossing that occurs first 
  if((s1*s2) > 0.) {
    s = (std::abs(s1) < std::abs(s2) ? s1 : s2); 
  }
  /// otherwise, pick the one in the positive direction
  else{     
    s = (s1  > 0. ? s1 : s2); 
  }  
  /// Z-coordinate of the layer crossing 
  float zc   = startingPoint[2]+searchDirection[2]*s;
  /// radial component of the search direction 
  float At   = std::sqrt(1.-searchDirection[2]*searchDirection[2]);
  
  /// Check if we miss the layer completely: 
  /// If the distance of our crossing point to the layer centre along z exceeds the half width 
  /// in z of the layer by more than a tolerance obtained as 
  /// the z movement expected when traversing along the layer radial half-width (dr_half / tan(theta_searchDirection)) 
  /// plus the road width divided by sin(theta)
  if(At != 0. && std::abs(zc-m_z) > (m_dz+(m_dr*std::abs(searchDirection[2])+startingPoint[4])/At)) return;
  /// Phi coordinate of the crossing
  float phiCrossing   = std::atan2(startingPoint[1]+searchDirection[1]*s,startingPoint[0]+searchDirection[0]*s);
  /// road width divided by the radius of the layer
  float reducedRoadWidth   = startingPoint[4]/m_r;
  getDetElements(startingPoint,searchDirection,phiCrossing,reducedRoadWidth,lDE,used);

}

///////////////////////////////////////////////////////////////////
// Get endcap detector elements
// Input parameters: startingPoint[0] - X     searchDirection[0] - Ax 
//                   startingPoint[1] - Y     searchDirection[1] - Ay
//                   startingPoint[2] - Z     searchDirection[2] - Az
//                   startingPoint[3] - R
//                   startingPoint[4] - width
//                   startingPoint[5] - step
///////////////////////////////////////////////////////////////////

void InDet::SiDetElementsLayer_xk::getEndcapDetElements
(const std::array<float,6> & startingPoint,
 const std::array<float,3> & searchDirection,
 std::vector<InDet::SiDetElementLink_xk::ElementWay> &lDE,
 std::vector<InDet::SiDetElementRoadMakerData_xk::UsedFlag>   &used) const
{
  /// solve the linear equation 
  /// z_layer = z_startingPont + s * z_searchDirection
  float s   =(m_z-startingPoint[2])/searchDirection[2];
  /// obtain x,y,r coordinates of the layer crossing in z
  float xc  = startingPoint[0]+searchDirection[0]*s;
  float yc  = startingPoint[1]+searchDirection[1]*s;
  float rc  = std::sqrt(xc*xc+yc*yc);
  /// search direction z (== cos(theta_search)) * radius of starting point
  float A23 = searchDirection[2]*startingPoint[3];
  /// Kick out cases where we do not expect to cross the layer at all. 
  /// Do this by checking if the distance of the radial location of the z-crossing 
  /// from the layer centre in r exceeds the r-half-width by more than 
  /// the r-movement expected when traversing the half-width in z + the search road width
  if(A23 != 0. && std::abs(rc-m_r) > m_dr+std::abs(2.*(startingPoint[0]*searchDirection[0]+startingPoint[1]*searchDirection[1])*m_dz/A23)+startingPoint[4]) return;
  float phiCrossing  = std::atan2(yc,xc);
  float reducedRoadWidth  = startingPoint[4]/rc;
  getDetElements(startingPoint,searchDirection,phiCrossing,reducedRoadWidth,lDE,used);

}

///////////////////////////////////////////////////////////////////
// Get detector elements
// Input parameters: startingPoint[0] - X     searchDirection[0] - Ax 
//                   startingPoint[1] - Y     searchDirection[1] - Ay
//                   startingPoint[2] - Z     searchDirection[2] - Az
//                   startingPoint[3] - R
//                   startingPoint[4] - width
//                   startingPoint[5] - step
///////////////////////////////////////////////////////////////////

void InDet::SiDetElementsLayer_xk::getDetElements
(const std::array<float,6> & startingPoint,
 const std::array<float,3> & searchDirection,
 float phiCrossing,
 float reducedRoadWidth,
 std::vector<InDet::SiDetElementLink_xk::ElementWay> &lDE,
 std::vector<InDet::SiDetElementRoadMakerData_xk::UsedFlag>   &used) const
{
  constexpr float pi = M_PI;
  constexpr float pi2 = 2.*pi; 
  int im  = int(m_elements.size())-1; 
  if(im<0) return;
  int i0  = 0;
  int i1  = im;

  /// iteratively search for the index of the crossing by splitting the remaining search region in half
  if (phiCrossing> m_elements[i0].phi() && phiCrossing< m_elements[i1].phi()) {
    while((i1-i0)>1) {
      int i = (i0+i1)/2; 
      if (m_elements[i].phi() > phiCrossing){
        i1=i;
      }
      else{
        i0=i;
      }
    }
    i0 = i1;
  }
  //
  std::array<float,3> intersectionOutcome;
  int i = i0;
  /// first, rotate in the positive phi direction
  while(1) {
    assert( static_cast<unsigned int>(i)<m_elements.size() );
    /// if detector element i on this layer is not already used for this road
    if(!used[i].used()) {
      /// 
      float dPhi =std::abs(m_elements[i].phi()-phiCrossing); 
      if(dPhi>pi) dPhi=std::abs(dPhi-pi2);    /// project delta phi into -pi..pi

      /// dPhi must be compatible with the phi half-width within a tolerance
      /// specified by the road width divided by the radius 
      if((dPhi-reducedRoadWidth)>m_dfe) break;

      /// intersect our projection with the detector element. 
      /// Output: intersectionOutcome[0] - close distance in azimuthal direction
      ///         intersectionOutcome[1] - close distance in r or z    direction
      ///         intersectionOutcome[2] - step to detector element 
      m_elements[i].intersect(&(startingPoint[0]),&(searchDirection[0]),&(intersectionOutcome[0]));

      /// closest distance in both directions has to be compatible within the road width
      if  (     intersectionOutcome[0]<=startingPoint[4] 
            && (intersectionOutcome[1]<=startingPoint[4])
          ) {
         /// we found a compatible detector element - add to our list  
         lDE.push_back(InDet::SiDetElementLink_xk::ElementWay(&m_elements[i],startingPoint[5]+intersectionOutcome[2],std::max(intersectionOutcome[0],intersectionOutcome[1]))); 
         used[i].setUsed();
      }
    }
    ++i; 
    if(i>im) i=0; /// loop around if we have to 
    if(i==i0) return; /// stop when we have tried all detector elements
  }
  /// we get here by triggering the 'break' clause in the positive-direction loop above 
  i1 = i; i = i0;
  while(1) {
    /// now rotate in the negative phi direction
    --i; 
    /// loop around at zero
    if(i<0) i=im; 
    /// stop at full circle
    if(i==i1) return;
    assert( static_cast<unsigned int>(i)<m_elements.size() );
    if(!used[i].used()) {
      float dPhi =std::abs(m_elements[i].phi()-phiCrossing); 
      if(dPhi>pi) dPhi=std::abs(dPhi-pi2);
      if((dPhi-reducedRoadWidth)>m_dfe) return;
      m_elements[i].intersect(&(startingPoint[0]),&(searchDirection[0]),&(intersectionOutcome[0]));

      if((intersectionOutcome[0]-startingPoint[4])<=0 && (intersectionOutcome[1]-startingPoint[4])<=0.) {
         lDE.push_back(InDet::SiDetElementLink_xk::ElementWay(&m_elements[i],startingPoint[5]+intersectionOutcome[2],std::max(intersectionOutcome[0],intersectionOutcome[1]))); 
         used[i].setUsed();
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
// Sort detector elements in azimuthal angle order
///////////////////////////////////////////////////////////////////

void InDet::SiDetElementsLayer_xk::sortDetectorElements()
{
  std::sort(m_elements.begin(),m_elements.end(),InDet::compDetElements_Azimuthal());
}

