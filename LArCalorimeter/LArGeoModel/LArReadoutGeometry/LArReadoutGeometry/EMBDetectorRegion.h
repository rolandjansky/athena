/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARREADOUTGEOMETRY_EMBDETECTORREGION_H
#define LARREADOUTGEOMETRY_EMBDETECTORREGION_H
#include "LArReadoutGeometry/EMBCellConstLink.h"
#include "LArReadoutGeometry/EMBDetDescr.h"
#include "GeoModelKernel/GeoVDetectorElement.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoPrimitives/GeoPrimitives.h"

/**
 * @class EMBDetectorRegion
 *
 *      @brief Description of a region of homogenous granularity in the 
 *      electromagnetic barrel calorimeter
 *
 *	This class combines a description of the cell
 *	granularity with a physical volume to create a
 *	description of an EMB region positioned within ATLAS.
 *	It provides access to the EMB Cells.
 */

class GeoAlignmentStore;

class EMBDetectorRegion : public GeoVDetectorElement  
{
  
 public:

  typedef enum {NEG=0,POS=1}     DetectorSide;

      
  /**
   *	@brief Constructor.
   */
  EMBDetectorRegion (const GeoVFullPhysVol *physVol, const EMBDetDescr *embDescriptor, DetectorSide endcap);
  
  /**
   *    @brief Destructor    
   */
  ~EMBDetectorRegion();
   
  /**
   *	@brief Access to Cells.
   */
  EMBCellConstLink getEMBCell (unsigned int ieta, unsigned int iphi) const;
  
  /**
   *	@brief Returns the Descriptor for this region.
   */
  const EMBDetDescr * getDescriptor () const;
  
  /**
   *	@brief Returns the Sampling Layer Index.
   */
  unsigned int getSamplingIndex () const;
  
  /**
   *	@brief Returns the Region Index
   */
  unsigned int getRegionIndex () const;
  
  /**
   *	@brief Returns the first phi index in the region.
   */
  unsigned int beginPhiIndex () const;
  
  /**
   *	@brief Returns the end phi index in the region.
   */
  unsigned int endPhiIndex () const;
    
  /**
   *	@brief Returns the first eta index in the region.
   */
  unsigned int beginEtaIndex () const;
   
  /**
   *	@brief Returns the end eta index in the region.
   */
  unsigned int endEtaIndex () const;
   
  /**
   *    @brief Returns the absolute transform of this element.
   */
  const Amg::Transform3D&  getAbsoluteTransform (const GeoAlignmentStore* alignStore=nullptr) const;

  /**
   *    @brief Returns the absolute transform of this element.
   */
  const Amg::Transform3D&  getDefAbsoluteTransform (const GeoAlignmentStore* alignStore=nullptr) const;
   
  /**
   *	@brief The endcap index.  0=negative, 1=positive.
   */
  EMBDetectorRegion::DetectorSide getEndcapIndex () const;
  
 private:
  
  EMBDetectorRegion(const EMBDetectorRegion &right);
  EMBDetectorRegion & operator=(const EMBDetectorRegion &right);
  const EMBDetDescr *m_descriptor;
  EMBDetectorRegion::DetectorSide m_endcapIndex;
};





inline const EMBDetDescr * EMBDetectorRegion::getDescriptor () const
{
  
  return m_descriptor;
  
}

inline unsigned int EMBDetectorRegion::getSamplingIndex () const
{
  
  return m_descriptor->getSamplingIndex();
  
}

inline unsigned int EMBDetectorRegion::getRegionIndex () const
{
  
  return m_descriptor->getRegionIndex();
  
}

inline unsigned int EMBDetectorRegion::beginPhiIndex () const
{
  
  return m_descriptor->getPhiBinning().getFirstDivisionNumber();
  
}

inline unsigned int EMBDetectorRegion::endPhiIndex () const
{
  
  return m_descriptor->getPhiBinning().getFirstDivisionNumber() + m_descriptor->getPhiBinning().getNumDivisions();
  
}

inline unsigned int EMBDetectorRegion::beginEtaIndex () const
{
  
  return m_descriptor->getEtaBinning().getFirstDivisionNumber();
  
}

inline unsigned int EMBDetectorRegion::endEtaIndex () const
{
  
  return m_descriptor->getEtaBinning().getFirstDivisionNumber() + m_descriptor->getEtaBinning().getNumDivisions();
  
}



inline EMBDetectorRegion::DetectorSide EMBDetectorRegion::getEndcapIndex () const
{
  
  return m_endcapIndex;
  
}





#endif
