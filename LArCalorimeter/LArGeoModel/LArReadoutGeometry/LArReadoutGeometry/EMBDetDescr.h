/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBDetDescr_h
#define EMBDetDescr_h 1
#include <vector>
#include "GeoModelKernel/CellBinning.h"
#include "GeoModelKernel/RCBase.h"
class EMBDetectorManager;
/** 
 * @brief Descriptor for regions of the electromagnetic barrel calorimeter
 */

/**
 *	This class describes completely the segmentation in eta
 *	and phi, and also the depth boundaries of cells within a
 *	region of the EMB.   This descriptor does not need to
 *	ever be accessed directly.  If you are tempted to use it
 *	directly, look first at the EMBDetectorRegion and the
 *	EMBCell class.  EMBCells obtained from a EMBDetector
 *	Region is normally a preferred way to access all of the
 *	information in the EMBDetDescr class.
 */

class EMBDetDescr : public RCBase  
{
  
  

  public:
    
      
  /**
   * @brief Constructor
   */
  EMBDetDescr (const EMBDetectorManager *detManager, unsigned int sampling, unsigned int region, const CellBinning &phiBinning);
  
  
  /**
   * @brief Destructor
   */
  ~EMBDetDescr();
  

    
  /**
   * @brief	Gets the offset position (cell centers with respect to
   * 	the position of the front of the active volume of the
   *	EMEC, or middle of the presamper.).
   */
  double getOffset (unsigned int etaIndex) const;

      
  /**
   * @brief	gets the cell half-length in z.
   */
  double getHalfLength (unsigned int etaIndex) const;
  
  /**
   * @brief	The manager.
   */
  const EMBDetectorManager * getManager () const;
  
  
  /**
   * @brief	The sampling index.
   */
  unsigned int getSamplingIndex () const;
  
  
  /**
   * @brief	The region index.
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

 private:
  
  EMBDetDescr(const EMBDetDescr &right);
  EMBDetDescr & operator=(const EMBDetDescr &right);

  const EMBDetectorManager *m_manager;
  unsigned int m_samplingIndex;
  unsigned int m_regionIndex;
  CellBinning m_phiBinning;
  CellBinning m_etaBinning;
  
  
  
  /**
   * @brief	Vector of half-lengths for the cells, one for each eta	index.
   */
  
  std::vector<double> m_halfLength;
  
      
  /**    
   *	 @brief Vector of cell center positions for the cells, one for
   *	 each eta index. These numbers are relative to the front
   *	 face of the EMEC active volume.  This happens to be the
   *	 z=0 face of the local coordinate system in the EMEC, but
   *	 that little detail depends critically on the way the Geo
   *	 Model description is implemented and may be false by the
   *	 time you read this comment. For the presampler the
   *	 offset is relative to the middle of the presampler
   *	 active volume, which is also z=0 for the presampler
   *	 local coordinate (with the same caveats as above).
   *	 
   *	 (From the database table EmecSamplingSep and other
   *	 related tables.)
   */
  std::vector<double> m_offset;
      

      
      

};







inline double EMBDetDescr::getOffset (unsigned int etaIndex) const
{
  
  if (m_offset.size()==1) return m_offset[0];
  return m_offset[etaIndex];
  
}

inline double EMBDetDescr::getHalfLength (unsigned int etaIndex) const
{
  
  if (m_halfLength.size()==1) return m_halfLength[0];
  return m_halfLength[etaIndex];
  
}



inline const EMBDetectorManager * EMBDetDescr::getManager () const
{
  
  return m_manager;
  
}

inline unsigned int EMBDetDescr::getSamplingIndex () const
{
  
  return m_samplingIndex;
  
}

inline unsigned int EMBDetDescr::getRegionIndex () const
{
  
  return m_regionIndex;
  
}

inline const CellBinning& EMBDetDescr::getPhiBinning () const
{
  
  return m_phiBinning;
  
}

inline const CellBinning& EMBDetDescr::getEtaBinning () const
{
  
  return m_etaBinning;
  
}





#endif
