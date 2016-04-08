/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HECLongBlock_h
#define HECLongBlock_h 1
#include <vector>
#include "LArReadoutGeometry/HECRadialSegment.h"

/**
 *      @brief Description of HEC Longitudinal Block.
 */

/**
 *	The HEC has longitudinal segments known as blocks.  Each
 *	block is sliced radially.  For a good picture of this,
 *	go here:
 *
 *	hepweb.phys.uvic.ca/atlas/hec-emec/geometry/
 *	hec_eta_100dpi.jpg             (Up to the directory for
 *	related drawings).
 *
 *	HECLongBlock obect describes both the longitudinal
 *	segmentation of the HEC into wheels and the radial
 *	division of each wheel into Radial Segments (see
 *	HECRadialSegment class); these radial segments are the
 *	building blocks of the (nonprojective) HEC Cells (see
 *	the HECCell class).
 */

class HECLongBlock 
{
  
  

 public:
    
  /**
   * @brief Copy Constructor
   */
  HECLongBlock(const HECLongBlock &right);
      
  /**
   * @brief Constructor.
   */
  HECLongBlock (unsigned int blockNum, double innerRad, double outerRad, double totalDepth, unsigned int nLarGaps, double frontPlateThick, double backPlateThick, const double etaRange[15]);

  /**
   * @brief Destructor
   */
  ~HECLongBlock();

  /**
   * @brief Assignment
   */
  HECLongBlock & operator=(const HECLongBlock &right);
      
  /**
   * @brief Returns a radial segment if it exists.
   */
  const HECRadialSegment* getRadialSegment (unsigned int i) const;
      
  /**
   * @brief returns the number of radial segments  (14)
   */
  unsigned int getNumRadialSegments () const;
      
  /**
   * @brief Block Number (1-7)
   */
  unsigned int getBlockNumber () const;
      
  /**
   * @brief Inner Radius of LongBlock (CLHEP::mm)
   */
  double getInnerRadius () const;
      
  /**
   * @brief Outer Radius of LongBlock (CLHEP::mm)
   */
  double getOuterRadius () const;
      
  /**
   * @brief Depth of block (CLHEP::mm)
   */
  double getDepth () const;
      
  /**
   * @brief Number of LArGaps in Block
   */
  unsigned int getNumLArGaps () const;
      
  /**
   * @brief Front plate thickness (or zero if no front plate)
   */
  double getFrontPlateThickness () const;
      
  /**
   * @brief Back plate thickness (or zero if no back plate)
   */
  double getBackPlateThickness () const;

 private:
      
  std::vector<HECRadialSegment *> m_RadialSeg;
  unsigned int m_blockNumber;
  double m_innerRadius;
  double m_outerRadius;
  double m_depth;
  unsigned int m_numLArGaps;
  double m_frontPlateThickness;
  double m_backPlateThickness;
};





inline const HECRadialSegment* HECLongBlock::getRadialSegment (unsigned int i) const
{
  
  return m_RadialSeg[i];
  
}

inline unsigned int HECLongBlock::getNumRadialSegments () const
{
  
  return m_RadialSeg.size();
  
}



inline unsigned int HECLongBlock::getBlockNumber () const
{
  
  return m_blockNumber;
  
}

inline double HECLongBlock::getInnerRadius () const
{
  
  return m_innerRadius;
  
}

inline double HECLongBlock::getOuterRadius () const
{
  
  return m_outerRadius;
  
}

inline double HECLongBlock::getDepth () const
{
  
  return m_depth;
  
}

inline unsigned int HECLongBlock::getNumLArGaps () const
{
  
  return m_numLArGaps;
  
}

inline double HECLongBlock::getFrontPlateThickness () const
{
  
  return m_frontPlateThickness;
  
}

inline double HECLongBlock::getBackPlateThickness () const
{
  
  return m_backPlateThickness;
  
}





#endif
