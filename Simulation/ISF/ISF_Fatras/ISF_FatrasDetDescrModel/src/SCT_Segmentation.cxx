/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    m_pitchY(0.),
    m_NcellX(0),
    m_NcellY(0),
    m_lengthXmin(0.),
    m_lengthXmax(0.),
    m_lengthY(0.) {}

  //Constructor with parameters:
  SCT_Segmentation::SCT_Segmentation(double lengthXmin, double lengthY, double lengthXmax, double pitchX, double pitchY):
    m_lengthXmin(lengthXmin),
    m_lengthXmax(lengthXmax),
    m_lengthY(lengthY)
  {
    
    if(pitchX == 0.0) m_pitchX = lengthXmin;
    else m_pitchX = pitchX;
    
    if(pitchY == 0.0) m_pitchY = lengthY;
    else m_pitchY = pitchY;
    
    m_NcellX = int(lengthXmin/m_pitchX);
    m_NcellY = int(lengthY/m_pitchY);
    
    // increase the pitch if the number of cells is not integer
    if (m_NcellX * m_pitchX != lengthXmin) 
      m_pitchX += (lengthXmin - float(m_NcellX) * m_pitchX)/float(m_NcellX);
    
    if (m_NcellY * m_pitchY != lengthY) 
      m_pitchY += (lengthY - float(m_NcellY) * m_pitchY)/float(m_NcellY);

    if (lengthXmax != 0.) {
      // It is the x pitch measured at the center of the module
      // --> It changes along Y
      double lengthXatCenter = (lengthXmax+lengthXmin)/2.;
      m_pitchXatCenter = lengthXatCenter/float(m_NcellX);
    
      // It is the pitch measured in phi 
      // --> This is constant while pitch X changes along Y
      double tanPhi = (lengthXmax-lengthXmin)/(2.*lengthY);
      m_pitchPhi = atan(tanPhi)/(float(m_NcellX)/2.);
    }
    else {
      m_pitchXatCenter = lengthXmin/float(m_NcellX);
      m_pitchPhi = 0.;
    }
  }
  
  /// Destructor:
  SCT_Segmentation::~SCT_Segmentation() {}

  bool SCT_Segmentation::cellOfPosition(const Amg::Vector2D& localPos, std::pair<int, int>& entryXY) const {
    
    // for debugging
    // std::cout << "-------------------------------------------" << std::endl;
    // std::cout << "SCT_Segmentation::cellOfPosition starting!" << std::endl;
    
    double halfLengthXatHit = phiPitch(localPos)*float(m_NcellX)/2.;

    Amg::Vector2D relativePos = localPos + Amg::Vector2D(halfLengthXatHit, m_lengthY/2.);
    int strip = (int)floor(relativePos[Trk::locX]/m_pitchX);

    entryXY = std::pair< int,  int> (strip, 1);

    // for debugging
    // std::cout << "checking local position = " << localPos << std::endl;
    // std::cout << "checking halfLengthXatHit = " << halfLengthXatHit << std::endl;
    // std::cout << "checking relative position = " << relativePos << std::endl;
    // std::cout << "returning strip = " << strip << std::endl; 
    // std::cout << "SCT_Segmentation::cellOfPosition ending!" << std::endl;
    // std::cout << "-----------------------------------------" << std::endl;

    return true;
           
  }

  double SCT_Segmentation::phiPitch(const Amg::Vector2D & localPos) const {

    using Trk::locX;
    using Trk::locY;
    
    if (m_lengthXmax!=0.) {
      double tanPhi = (m_lengthXmax-m_lengthXmin)/(2.*m_lengthY);
      double lengthXatHit = 2*(m_lengthY/2.*(m_lengthXmax+m_lengthXmin)/(m_lengthXmax-m_lengthXmin)+localPos[Trk::locY])*tanPhi;
      return lengthXatHit/float(m_NcellX);
    } 
    
    return m_lengthXmin/float(m_NcellX);
    
  }

  double SCT_Segmentation::stripLength(const Amg::Vector2D & localPos) const {
    
    using Trk::locX;
    using Trk::locY;

    double pitchXatHit = phiPitch(localPos);
    int absStrip = (int)ceil(fabs(localPos[Trk::locX])/pitchXatHit);
    
    double alpha = (absStrip -1/2.)*m_pitchPhi;
    
    return m_lengthY/cos(alpha);
    
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

    // for debugging
    // std::cout << "------------------------------------------------" << std::endl;
    // std::cout << "SCT_Segmentation::localPositionOfCell starting!" << std::endl;
    // std::cout << "checking SiCellId = " << cellId << std::endl;
    
    double halfLengthX = m_lengthXmax == 0. ? m_lengthXmin/2. : (m_lengthXmax+m_lengthXmin)/4.; 
    
    // NB. No check is made that cellId is valid or in the correct range.
    int strip =  cellId.strip();
        
    // center of cluster (in units of number of strips) from detector center
    double clusterCenter = strip+0.5;
    
    // for debugging
    // std::cout << "checking strip = " << strip << std::endl; 
    // std::cout << "returning local position = (" << clusterCenter*m_pitchXatCenter-halfLengthX << " , 0.)" << std::endl;
    // std::cout << "SCT_Segmentation::localPositionOfCell ending!" << std::endl;
    // std::cout << "----------------------------------------------" << std::endl;

    // position relative to the center
    return Amg::Vector2D(clusterCenter*m_pitchXatCenter-halfLengthX, 0.);
  }

}
