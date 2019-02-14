/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_Segmentation.cxx
// Implementation file class SCT_Segmentation
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Class header
#include "ISF_FatrasDetDescrModel/SCT_Segmentation.h"

#include "TrkEventPrimitives/ParamDefs.h"

namespace iFatras {

  //Constructor w/o parameters:
  SCT_Segmentation::SCT_Segmentation() :
    m_pitchX(0.),
    m_pitchPhi(0.),
    m_pitchXatCenter(0.),
    m_NcellX(0),
    m_NcellPhi(0),
    m_lengthXmin(0.),
    m_lengthXmax(0.),
    m_lengthY(0.),
    m_rMin(0.),
    m_rMax(0.),
    m_hPhiSec(0.),
    m_averagePhi(0.),
    m_discTrapezoidal(false)
  {}

  //Constructor with parameters:
  SCT_Segmentation::SCT_Segmentation(double lengthXmin, double lengthXmax, double lengthY, double pitchX):
    m_NcellPhi(0),
    m_lengthXmin(lengthXmin),
    m_lengthXmax(lengthXmax),
    m_lengthY(lengthY),
    m_rMin(0.),
    m_rMax(0.),
    m_hPhiSec(0.),
    m_averagePhi(0.),
    m_discTrapezoidal(false)
  {
    
    if(pitchX == 0.0) m_pitchX = lengthXmin;
    else m_pitchX = pitchX;
    
    m_NcellX = int(lengthXmin/m_pitchX);
    
    // increase the pitch if the number of cells is not integer
    if (m_NcellX * m_pitchX != lengthXmin) 
      m_pitchX += (lengthXmin - float(m_NcellX) * m_pitchX)/float(m_NcellX);
    
    if ( m_lengthXmax == m_lengthXmin )
      m_lengthXmax = 0.;
      
    if (m_lengthXmax != 0.) {
      // It is the x pitch measured at the center of the module
      // --> It changes along Y
      double lengthXatCenter = (m_lengthXmax+m_lengthXmin)/2.;
      m_pitchXatCenter = lengthXatCenter/float(m_NcellX);
    
      // It is the pitch measured in phi 
      // --> This is constant while pitch X changes along Y
      double tanPhi = (m_lengthXmax-m_lengthXmin)/(2.*lengthY);
      m_pitchPhi = atan(tanPhi)/(float(m_NcellX)/2.);
    } else {
      m_pitchXatCenter = m_lengthXmin/float(m_NcellX);
      m_pitchPhi = 0.;
    }
  }

  //Constructor with parameters:
  SCT_Segmentation::SCT_Segmentation(double lengthXmin, double lengthXmax, double Rmin, double Rmax, double pitchPhi, double avePhi):
    m_pitchX(0.),
    m_pitchPhi(pitchPhi),
    m_pitchXatCenter(0.),
    m_NcellX(0),
    m_lengthXmin(lengthXmin),
    m_lengthXmax(lengthXmax),
    m_rMin(Rmin),
    m_rMax(Rmax),
    m_averagePhi(avePhi),
    m_discTrapezoidal(true)
  {
    double hmax = sqrt(m_rMax*m_rMax+m_lengthXmax/2.*m_lengthXmax/2.);
    double hmin = sqrt(m_rMin*m_rMin+m_lengthXmin/2.*m_lengthXmin/2.);

    m_lengthY = hmax-hmin;

    m_hPhiSec = asin(m_lengthXmax/2./m_rMax);
    m_NcellPhi = int(2.*m_hPhiSec/m_pitchPhi);
    
    // increase the pitch if the number of cells is not integer
    if (m_NcellPhi * m_pitchPhi != 2.*m_hPhiSec) 
      m_pitchPhi += (2.*m_hPhiSec - float(m_NcellPhi) * m_pitchPhi)/float(m_NcellPhi);
  }
  
  bool SCT_Segmentation::cellOfPosition(const Amg::Vector2D& localPos, std::pair<int, int>& entryXY) const {
    
    // for debugging
    // std::cout << "-------------------------------------------" << std::endl;
    // std::cout << "SCT_Segmentation::cellOfPosition starting!" << std::endl;

    // std::cout << "SCT_Segmentation::checking all parameters first!!!" << std::endl;
    
    // std::cout << "SCT_Segmentation -->  m_pitchX = " << m_pitchX << std::endl;
    // std::cout << "SCT_Segmentation -->  m_NcellX = " << m_NcellX << std::endl;
    
    // std::cout << "SCT_Segmentation -->  m_lengthXmin = " << m_lengthXmin << std::endl;
    // std::cout << "SCT_Segmentation -->  m_lengthXmax =" << m_lengthXmax << std::endl;
    // std::cout << "SCT_Segmentation -->  m_lengthY = " << m_lengthY << std::endl;
    
    // std::cout << "SCT_Segmentation -->  m_pitchXatCenter = " << m_pitchXatCenter << std::endl;
    // std::cout << "SCT_Segmentation -->  m_pitchPhi = " << m_pitchPhi << std::endl;
    
    // std::cout << "SCT_Segmentation -->  m_discTrapezoidal(false)" << std::endl;
      

    if (!m_discTrapezoidal) {
      
      double pitchXatHit = phiPitch(localPos);
      double halfLengthXatHit = 0.5*pitchXatHit*float(m_NcellX);
      Amg::Vector2D relativePos = localPos + Amg::Vector2D(halfLengthXatHit, 0.5*m_lengthY);

      int strip = (int)floor(relativePos[Trk::locX]/pitchXatHit);
      
      entryXY = std::pair< int,  int> (strip, 1);
    
      // for debugging
      // std::cout << "checking local position = " << localPos << std::endl;
      // std::cout << "checking halfLengthXatHit = " << halfLengthXatHit << std::endl;
      // std::cout << "checking relative position = " << relativePos << std::endl;
      // std::cout << "returning strip = " << strip << std::endl; 
      
      // std::cout << "SCT_Segmentation::cellOfPosition ending!" << std::endl;
      // std::cout << "-----------------------------------------" << std::endl;
      
      return true;
    
    } else {
      double phiPos = localPos[Trk::locPhi] - (m_averagePhi - m_hPhiSec);
      int strip = (int)floor(phiPos/m_pitchPhi);
      
      entryXY = std::pair< int, int> (strip, 1);
      
      // for debugging
      // std::cout << "checking local position = " << localPos[Trk::locPhi] << std::endl;
      // std::cout << "checking m_averagePhi = " << m_averagePhi << std::endl;
      // std::cout << "checking m_hPhiSec = " << m_hPhiSec << std::endl;
      // std::cout << "checking phiPos = " << phiPos << std::endl;
      // std::cout << " returning strip = " << strip << std::endl;
      
      // std::cout << "SCT_Segmentation::cellOfPosition ending!" << std::endl;
      // std::cout << "-----------------------------------------" << std::endl;

      return true;
    }

    return true;
           
  }

  double SCT_Segmentation::phiPitch(const Amg::Vector2D & localPos) const {

    if (m_discTrapezoidal) {
      return m_pitchPhi;
    } else {    
      if (m_lengthXmax!=0.) { // this is in mm for the X coordinate
	using Trk::locX;
	using Trk::locY;
	double tanPhi = (m_lengthXmax-m_lengthXmin)/m_lengthY;
	double lengthXatHit = (m_lengthY/2.*(m_lengthXmax+m_lengthXmin)/(m_lengthXmax-m_lengthXmin)+localPos[Trk::locY])*tanPhi;
	return lengthXatHit/float(m_NcellX);
      } 
      return m_lengthXmin/float(m_NcellX);
    } 
    
  }
  
  double SCT_Segmentation::stripLength(const Amg::Vector2D & localPos) const {
    // for debugging
    // std::cout << "-------------------------------------------" << std::endl;
    // std::cout << "SCT_Segmentation::stripLength starting!" << std::endl;
    
    if (!m_discTrapezoidal) {
      using Trk::locX;
      using Trk::locY;
      double pitchXatHit = phiPitch(localPos);
      int absStrip = (int)ceil(fabs(localPos[Trk::locX])/pitchXatHit);
      double alpha = (absStrip - 0.5)*m_pitchPhi;
      
      return m_lengthY/cos(alpha);
      
    } else {
      
      using Trk::locPhi;
      using Trk::locX;
      using Trk::locY;
      
      // this is the phi sector corresponding to Rmin
      double hMinPhi = asin(m_lengthXmin/2./m_rMin);
      
      // first check if the strip is inside phiMin
      // (PhiMin is phi corresponding the Rmin and Xmin)
      double phiPos = localPos[Trk::locPhi] - (m_averagePhi-m_hPhiSec);
      int strip = (int)floor(phiPos/m_pitchPhi);
      double alpha = (strip + 0.5)*m_pitchPhi - m_hPhiSec;
            
      double hmax = sqrt(m_rMax*m_rMax+m_lengthXmax/2.*m_lengthXmax/2.);
      double hmin = sqrt(m_rMin*m_rMin+m_lengthXmin/2.*m_lengthXmin/2.);
      
      if (fabs(alpha) < hMinPhi) {
	double length = (hmax/cos(alpha) - hmin/cos(alpha));
	// std::cout << "returning strip length = " << length << std::endl; 
	return length;	
      }
      
      // evaluate the strip length in case it is smaller than the normal size
      // this happens for weird trapezoidal shapes
      
      double Xmax = hmax*tan(alpha);
      double Xmin = hmin*tan(alpha);
      
      // step 1 -> intersect the oblique side with the pseudo-strip
      // first equation
      double m1 = 2.*(hmax-hmin)/(m_lengthXmax-m_lengthXmin);
      double q1 = (hmin*m_lengthXmax-hmax*m_lengthXmin)/(m_lengthXmax-m_lengthXmin);
      // second equation
      double m2 = (hmax-hmin)/(Xmax-Xmin);
      double q2 = (hmin*Xmax-hmax*Xmin)/(Xmax-Xmin);

      double x = (q2-q1)/(m1-m2);
      double y = (m1*q2-m2*q1)/(m1-m2);

      // step 2 -> evaluate the part to subtract
      
      double lmax = hmax/cos(alpha);
      double lmin = sqrt(x*x+y*y);

      // std::cout << "found intersection with coordinate = " << x << ", " << y << std::endl; 

      // std::cout << "lmax = " << lmax << ", lmin = " << lmin << std::endl; 

      double length = lmax-lmin;
      // std::cout << "returning strip length = " << length << std::endl; 
      return length;	
    }    
  }

  double SCT_Segmentation::sinStereoLocal(const Amg::Vector2D &localPos) const {
    using Trk::locX;
    using Trk::locY;
    // The equation below will work for rectangle detectors as well in which
    // case it will return 0. But we return zero immediately as there is no point doing the calculation.
    if (m_lengthXmax == 0.) return 0;
    double oneOverRadius = 2./m_lengthY*(m_lengthXmax - m_lengthXmin)/(m_lengthXmax + m_lengthXmin);
    double x = localPos[Trk::locX];
    double y = localPos[Trk::locY];
    return -x*oneOverRadius / sqrt( (1+y*oneOverRadius)*(1+y*oneOverRadius) + x*oneOverRadius*x*oneOverRadius );  
  }

  Amg::Vector2D SCT_Segmentation::localPositionOfCell(const InDetDD::SiCellId &cellId) const {

    //for debugging
    // std::cout << "------------------------------------------------" << std::endl;
    // std::cout << "SCT_Segmentation::localPositionOfCell starting!" << std::endl;
    // std::cout << "checking SiCellId = " << cellId << std::endl;
    
    // NB. No check is made that cellId is valid or in the correct range.
    int strip =  cellId.strip();
    
    if (!m_discTrapezoidal) {
      double halfLengthX = m_lengthXmax == 0. ? m_lengthXmin/2. : (m_lengthXmax+m_lengthXmin)/4.; 
      
      // center of cluster (in units of number of strips) from detector center
      double clusterCenter = strip+0.5;
    
      // for debugging
      // std::cout << "checking strip = " << strip << std::endl; 
      // std::cout << "returning local position = (" << clusterCenter*m_pitchXatCenter-halfLengthX << " , 0.)" << std::endl;
      // std::cout << "SCT_Segmentation::localPositionOfCell ending!" << std::endl;
      // std::cout << "----------------------------------------------" << std::endl;
      
      // position relative to the center
      return Amg::Vector2D(clusterCenter*m_pitchXatCenter-halfLengthX, 0.);
    } else {
      
      // center of cluster (in units of number of strips) from detector center
      double clusterCenter = strip+0.5;
      
      // for debugging
      // std::cout << "checking strip = " << strip << std::endl; 
    
      double alpha = clusterCenter*m_pitchPhi - m_hPhiSec;
      double phi = m_averagePhi + alpha;
      
      double length = stripLength(Amg::Vector2D(0., phi));
      
      double hmax = sqrt(m_rMax*m_rMax+m_lengthXmax/2.*m_lengthXmax/2.);
            
      double R = hmax/cos(fabs(alpha))-length/2.;
                  
      // std::cout << "returning local position (R, phi) = (" << R << " , " << phi <<")" << std::endl;
      
      // std::cout << "SCT_Segmentation::localPositionOfCell ending!" << std::endl;
      // std::cout << "----------------------------------------------" << std::endl;
      return Amg::Vector2D(R,phi);
    }

  }

}
