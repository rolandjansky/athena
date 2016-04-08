/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HECDetDescr_h
#define HECDetDescr_h 1
#include "LArReadoutGeometry/HECDetectorManager.h"
#include "GeoModelKernel/CellBinning.h"
#include "GeoModelKernel/RCBase.h"
/** 
 * @brief Descriptor for regions of the hadronic endcap calorimeter
 */

/**
 *	This class describes completely the segmentation in
 *	"eta" and phi.  Eta appears in quotes because the HEC
 *	does not have eta boundaries, it has a staircase of
 *	radial boundaries that roughly adhere to an eta value.
 *	Keeping that in mind, this region describes the phi
 *	granularity of a region of the HEC and the radial and
 *	longitudinal division of the cells within the HEC.  This
 *	descriptor does not need to ever be accessed directly.
 *	If you are tempted to use it directly, look first at the
 *	HECDetectorRegion and the HECCell class.  HECCells
 *	obtained from a HECDetectorRegion is normally a
 *	preferred way to access all of the information in the
 *	HECDetDescr class.
 */
class HECDetDescr : public RCBase  
{

  public:
    
      
  /**
   * @brief	Constructor
   */
  HECDetDescr (const HECDetectorManager *detManager, unsigned int sampling, unsigned int region, const CellBinning &phiBinning, bool isTestBeam = false);

  /**
   * @brief        Destructor
   */
  ~HECDetDescr();
      
  /**
   * @brief	Returns min  z for the ith block at eta index
   */
  double getZMin (unsigned int block) const;
      
  /**
   * @brief	Returns max z for the ith block at eta index.
   */
  double getZMax (unsigned int block) const;
      
  /**
   * @brief	Returns min r for block i.
   */
  double getRMin (unsigned int block, unsigned int eta) const;
      
  /**
   * @brief	Returns max r at block i.
   */
  double getRMax (unsigned int block, unsigned int eta) const;
      
  /**
   * @brief	Returns the radial segment of in block b with eta index i, or else NULL
   */
  const HECRadialSegment * getRadialSegment (unsigned int block, unsigned int eta) const;
      
  /**
   * @brief	Gets the block index for the ith block (in case it is ever needed).
   */
  unsigned int getBlockIndex (unsigned int block) const;
      
  /**
   * @brief	The manager.
   */
  const HECDetectorManager * getManager () const;
      
  /**
   * @brief	The sampling index.
   */
  unsigned int getSamplingIndex () const;

  /**
   * @brief	The region Index
   */
  unsigned int getRegionIndex () const;
      
  /**
   * @brief	The Binning in Phi.
   */
  const CellBinning& getPhiBinning () const;
      
  /**
   * @brief	The Binning in Eta.
   */
  const CellBinning& getEtaBinning () const;
      
  /**
   * @brief	Number of blocks in the region.
   */
  unsigned int getNumBlocks () const;

  private:
    
      HECDetDescr(const HECDetDescr &right);
      HECDetDescr & operator=(const HECDetDescr &right);
      const HECDetectorManager *m_manager;
      unsigned int m_samplingIndex;
      unsigned int m_regionIndex;
      CellBinning m_phiBinning;
      unsigned int m_outerRadSegNumber;
      CellBinning m_etaBinning;
      unsigned int m_numBlocks;
      unsigned int m_firstBlock;
      std::vector<double> m_zMin;
      std::vector<double> m_zMax;
};







inline double HECDetDescr::getZMin (unsigned int block) const
{
  
  return m_zMin[block];
  
}

inline double HECDetDescr::getZMax (unsigned int block) const
{
  
  return m_zMax[block];
  
}

inline double HECDetDescr::getRMin (unsigned int block, unsigned int eta) const
{
  

  // Check that it exists:
  const HECRadialSegment *segment = getRadialSegment(block,eta);
  if (!segment) throw std::range_error("Error in HECDetDescr, nonexistent segment.");

  // Return value:
  return segment->getMinVal();

  
}

inline double HECDetDescr::getRMax (unsigned int block, unsigned int eta) const
{
  

  // Check that it exists:
  const HECRadialSegment *segment = getRadialSegment(block,eta);
  if (!segment) throw std::range_error("Error in HECDetDescr, nonexistent segment.");

  // Return value:
  return segment->getMaxVal();

  
}

inline const HECRadialSegment * HECDetDescr::getRadialSegment (unsigned int block, unsigned int eta) const
{
  
  // Get the block.
  const HECLongBlock *bPtr = getManager()->getBlock(getBlockIndex(block));
  // Check that it exists:
  if (!bPtr) throw std::range_error("Error in HECDetDescr, nonexistent block.");

  // From the block, get the radial segment:
   unsigned int index = m_outerRadSegNumber-eta;
  
  const HECRadialSegment *segment=bPtr->getRadialSegment(index);
  return segment;
  
}

inline unsigned int HECDetDescr::getBlockIndex (unsigned int block) const
{
  
  return m_firstBlock+block;
  
}



inline const HECDetectorManager * HECDetDescr::getManager () const
{
  
  return m_manager;
  
}

inline unsigned int HECDetDescr::getSamplingIndex () const
{
  
  return m_samplingIndex;
  
}

inline unsigned int HECDetDescr::getRegionIndex () const
{
  
  return m_regionIndex;
  
}

inline const CellBinning& HECDetDescr::getPhiBinning () const
{
  
  return m_phiBinning;
  
}

inline const CellBinning& HECDetDescr::getEtaBinning () const
{
  
  return m_etaBinning;
  
}

inline unsigned int HECDetDescr::getNumBlocks () const
{
  
  return m_numBlocks;
  
}





#endif
