/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
     
     @author Noemi Calace
     
  */

  class SCT_Segmentation : public ISegmentation {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

  public:

    /// Constructor:
    SCT_Segmentation();

    SCT_Segmentation(double lengthXmin, double lengthY, double lengthXmax = 0., double pitchX = 0., double pitchY = 0.);

    /// Destructor:
    ~SCT_Segmentation();

    //
    bool cellOfPosition(const Amg::Vector2D& localPos, std::pair<int, int>& entryXY) const;
    Amg::Vector2D localPositionOfCell(const InDetDD::SiCellId &cellId) const;
    
    double pitchX() const;
    double pitchY() const;
    double phiPitch() const; // return phi pitch at center (it is in mm)
    double phiPitch(const Amg::Vector2D & localPosition) const; // return phi picth at the given position(it is in mm)
    double stripLength(const Amg::Vector2D & localPos) const; 
    double sinStereoLocal(const Amg::Vector2D &localPos) const;
 
    int NcellX() const;
    int NcellY() const;

  protected:

    double m_pitchX;
    double m_pitchY;

    int m_NcellX;
    int m_NcellY;

    double m_lengthXmin;
    double m_lengthXmax;
    double m_lengthY;

    double m_pitchPhi;
    double m_pitchXatCenter;

  };

  inline double SCT_Segmentation::pitchX() const {
    return m_pitchX; 
  }

  inline double SCT_Segmentation::pitchY() const {
    return m_pitchY; 
  }

  inline int SCT_Segmentation::NcellX() const {
    return m_NcellX; 
  }

  inline int SCT_Segmentation::NcellY() const {
    return m_NcellY; 
  }

  inline double SCT_Segmentation::phiPitch() const {
    return m_pitchXatCenter;
  }
    
}


#endif // ISF_FATRASDETDESCRMODEL_SCT_SEGMENTATION_H
