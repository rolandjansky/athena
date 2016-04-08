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
    m_NcellY(0) {}

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

  bool PixelSegmentation::cellOfPosition(const Amg::Vector2D& localPos, std::pair<int, int>& entryXY) const{

    // for debugging
    // std::cout << "-------------------------------------------" << std::endl;
    // std::cout << "PixelSegmentation::cellOfPosition starting!" << std::endl;
    
    using Trk::locX;
    using Trk::locY;
    
    double halfLengthX = (m_pitchX*m_NcellX)/2.; 
    double halfLengthY = (m_pitchY*m_NcellY)/2.;
    
    // Check that we are in the bounds 
    if ( (std::abs(localPos[Trk::locX]) > halfLengthX) || 
	 (std::abs(localPos[Trk::locY]) > halfLengthY) ) {
      entryXY = std::pair< int,  int> (0, 0);
      return false;
    }

    // position relative to bottom left corner.
    Amg::Vector2D relativePos = localPos + Amg::Vector2D(halfLengthX, halfLengthY);
    entryXY = std::pair< int,  int> ((int)floor(relativePos[Trk::locX]/m_pitchX), (int)floor(relativePos[Trk::locY]/m_pitchY));

    // for debugging
    // std::cout << "checking local position = " << localPos << std::endl;
    // std::cout << "relative position = " << relativePos << std::endl;
    // std::cout << "returning XY = (" << entryXY.first << " ," << entryXY.second << ")" << std::endl;

    // std::cout << "PixelSegmentation::cellOfPosition ending!" << std::endl;
    // std::cout << "-----------------------------------------" << std::endl;

    return true;
  }

  Amg::Vector2D PixelSegmentation::localPositionOfCell(const InDetDD::SiCellId &cellId) const {
    
    // for debugging
    // std::cout << "------------------------------------------------" << std::endl;
    // std::cout << "PixelSegmentation::localPositionOfCell starting!" << std::endl;
    // std::cout << "checking SiCellId = " << cellId << std::endl;

    double halfLengthX = (m_pitchX*m_NcellX)/2.; 
    double halfLengthY = (m_pitchY*m_NcellY)/2.;
    
    // position relative to bottom left corner.
    Amg::Vector2D relativePos = Amg::Vector2D( (cellId.phiIndex()+0.5)*m_pitchX , (cellId.etaIndex()+0.5)*m_pitchY );

    // for debugging
    // std::cout << "relative position = " << relativePos << std::endl;
    // std::cout << "returning local position = " << relativePos - Amg::Vector2D(halfLengthX, halfLengthY) << std::endl;
    
    // std::cout << "PixelSegmentation::localPositionOfCell ending!" << std::endl;
    // std::cout << "----------------------------------------------" << std::endl;

    // position relative to the center
    return relativePos - Amg::Vector2D(halfLengthX, halfLengthY);
  }

}
