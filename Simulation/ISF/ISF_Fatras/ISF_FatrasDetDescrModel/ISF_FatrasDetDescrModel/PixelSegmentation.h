/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelSegmentation.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASDETDESCRMODEL_PIXELSEGMENTATION_H
#define ISF_FATRASDETDESCRMODEL_PIXELSEGMENTATION_H

#include "ISF_FatrasDetDescrInterfaces/ISegmentation.h"

namespace iFatras {

  /**
     @class PixelSegmentation
     Hold segmentation information for pixel detector elements
     
     @author Noemi Calace
     
  */

  class PixelSegmentation : public ISegmentation {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

  public:

    /// Constructor:
    PixelSegmentation();

    PixelSegmentation(double lengthX, double lengthY, double pitchX = 0., double pitchY = 0.);

    /// Destructor:
    ~PixelSegmentation();

    //
    std::pair<int, int> cellsOfPosition(const Amg::Vector2D &localPos) const;

    double pitchX() const;
    double pitchY() const;

    int NcellX() const;
    int NcellY() const;

    bool isIdValid() const;

  protected:

    double m_pitchX;
    double m_pitchY;

    int m_NcellX;
    int m_NcellY;

    mutable bool m_isIdValid;
        
  };

  inline double PixelSegmentation::pitchX() const {
    return m_pitchX; 
  }

  inline double PixelSegmentation::pitchY() const {
    return m_pitchY; 
  }

  inline int PixelSegmentation::NcellX() const {
    return m_NcellX; 
  }

  inline int PixelSegmentation::NcellY() const {
    return m_NcellY; 
  }

  inline bool PixelSegmentation::isIdValid() const {
    return m_isIdValid;
  }

}


#endif // ISF_FATRASDETDESCRMODEL_PIXELSEGMENTATION_H
