/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelSegmentation.cxx
// Implementation file class PixelSegmentation
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Class header
#include "ISF_FatrasDetDescrModel/PixelSegmentation.h"

#include "TrkEventPrimitives/ParamDefs.h"

namespace iFatras {

  //Constructor w/o parameters:
  PixelSegmentation::PixelSegmentation() :
    m_pitchX(0),
    m_pitchY(0),
    m_NcellX(0),
    m_NcellY(0),
    m_isIdValid(false)  {}

  //Constructor with parameters:
  PixelSegmentation::PixelSegmentation(double lengthX, double lengthY, double pitchX, double pitchY) {
    
    if(pitchX == 0.0) m_pitchX = lengthX;
    else m_pitchX = pitchX;

    if(pitchY == 0.0) m_pitchY = lengthY;
    else m_pitchY = pitchY;

    m_NcellX = int(lengthX/m_pitchX);
    m_NcellY = int(lengthY/m_pitchY);
    
    // increase the pitch if the number of cells is not integer
    if (m_NcellX * m_pitchX != lengthX) 
      m_pitchX += (lengthX - m_NcellX * m_pitchX)/m_NcellX;
    
    if (m_NcellY * m_pitchY != lengthY) 
      m_pitchY += (lengthY - m_NcellY * m_pitchY)/m_NcellY;
  
  }
 
  /// Destructor:
  PixelSegmentation::~PixelSegmentation() {}

  std::pair< int,  int> PixelSegmentation::cellsOfPosition(const Amg::Vector2D &localPos) const {

    using Trk::locX;
    using Trk::locY;

    double halfLengthX = (m_pitchX*m_NcellX)/2.; 
    double halfLengthY = (m_pitchY*m_NcellY)/2.;
    
    // Check that we are in the bounds 
    if ( (abs(localPos[Trk::locX]) > halfLengthX) || 
	 (abs(localPos[Trk::locY]) > halfLengthY) ) {
      m_isIdValid = false;
      return std::pair< int,  int> (0, 0);
    }

    // position relative to bottom left corner.
    Amg::Vector2D relativePos = localPos + Amg::Vector2D(halfLengthX, halfLengthY);
    m_isIdValid = true;
    return std::pair< int,  int> ( (int)floor(relativePos[Trk::locX]/m_pitchX), (int)floor(relativePos[Trk::locY]/m_pitchY));
  
  }

}
