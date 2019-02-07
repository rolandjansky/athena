/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_Segmentation.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASDETDESCRMODEL_SCT_SEGMENTATION_H
#define ISF_FATRASDETDESCRMODEL_SCT_SEGMENTATION_H

#include "ISF_FatrasDetDescrInterfaces/ISegmentation.h"

namespace iFatras {

  /**
     @class SCT_Segmentation
     Hold segmentation information for sct detector elements
     
     @author Noemi.Calace - at - cern.ch
     
  */

  class SCT_Segmentation : public ISegmentation {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

  public:

    /// Constructor:
    SCT_Segmentation();

    SCT_Segmentation(double lengthXmin, double lengthXmax, double lengthY, double pitchX);

    SCT_Segmentation(double lengthXmin, double lengthXmax, double Rmin, double Rmax, double pitchPhi, double avePhi=0.);

    /// Destructor:
    virtual ~SCT_Segmentation() = default;

    //
    bool cellOfPosition(const Amg::Vector2D& localPos, std::pair<int, int>& entryXY) const;
    Amg::Vector2D localPositionOfCell(const InDetDD::SiCellId &cellId) const;
    
    double pitchX() const;
    double pitchY() const;
    
    double phiPitch() const; // return phi pitch at center (it is in mm for PlanarSurfaces and in rads for DiscSurfaces)
    double phiPitch(const Amg::Vector2D & localPosition) const; // return phi picth at the given position (it is in mm for PlanarSurfaces and in rads for DiscSurfaces --> this is the same than phiPitch())
    
    double stripLength(const Amg::Vector2D & localPos) const; 
    
    double sinStereoLocal(const Amg::Vector2D &localPos) const;
 
    int NcellX() const;
    
  protected:

    double m_pitchX;
    double m_pitchPhi;
    double m_pitchXatCenter;

    int m_NcellX;
    int m_NcellPhi;
    
    double m_lengthXmin;
    double m_lengthXmax;
    double m_lengthY;

    double m_rMin;
    double m_rMax;     
    
    double m_hPhiSec;
    double m_averagePhi;
    
    bool m_discTrapezoidal;

  };

  inline double SCT_Segmentation::pitchX() const {
    return m_pitchX; 
  }

  inline int SCT_Segmentation::NcellX() const {
    return m_NcellX; 
  }

  inline double SCT_Segmentation::phiPitch() const {
    if (!m_discTrapezoidal)
      return m_pitchXatCenter;
    
    return m_pitchPhi;
  }

  inline double SCT_Segmentation::pitchY() const {
    if (m_discTrapezoidal)
      return phiPitch();
    return m_lengthY;
  }
}


#endif // ISF_FATRASDETDESCRMODEL_SCT_SEGMENTATION_H
