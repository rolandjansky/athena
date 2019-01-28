/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARREADOUTGEOMETRY_EMECDETECTORREGION_H
#define LARREADOUTGEOMETRY_EMECDETECTORREGION_H
#include "LArReadoutGeometry/EMECCellConstLink.h"
#include "LArReadoutGeometry/EMECDetDescr.h"
#include "GeoModelKernel/GeoVDetectorElement.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "CLHEP/Geometry/Point3D.h"

/**
 * @class EMECDetectorRegion
 * 
 *      @brief Description of a region of homogenous granularity in the 
 *      electromagnetic endcap calorimeter
 *
 * This class combines a description of the cell	
 * granularity with a physical volume to create a
 * description of an EMEC region positioned within ATLAS.
 * It provides access to the EMEC Cells.
 */


class EMECDetectorRegion : public GeoVDetectorElement  
{
  
 public:
  typedef enum {NEG=0,POS=1}     DetectorSide;
  typedef enum {OUTER=0,INNER=1} RadialPos;
  
      
  /**
   * @brief	Constructor.
   */
  EMECDetectorRegion(const GeoVFullPhysVol *physVol
		     , const EMECDetDescr *emecDescriptor
		     , DetectorSide endcap
		     , double projectivityDisplacement = 4*Gaudi::Units::cm);

  /**
   * @brief    Destructor    
   */
  ~EMECDetectorRegion();
      
  /**
   * @brief	Access to Cells.
   */
  EMECCellConstLink getEMECCell (unsigned int ieta, unsigned int iphi) const;
      
  /**
   * @brief	Returns the Descriptor for this region.
   */
  const EMECDetDescr * getDescriptor () const;
      
  /**
   * @brief	Returns the Sampling Layer Index.
   */
  unsigned int getSamplingIndex () const;
      
  /**
   * @brief	Returns the Region Index
   */
  unsigned int getRegionIndex () const;
      
  /**
   * @brief	Returns the Radial (Outer Wheel=0,InnerWheel=1)  Index
   */
  unsigned int getRadialIndex () const;
      
  /**
   * @brief	returns the first phi index in the region.
   */
  unsigned int beginPhiIndex () const;
      
  /**
   * @brief	returns the end phi index in the region.
   */
  unsigned int endPhiIndex () const;
      
  /**
   * @brief	returns the first eta index in the region.
   */
  unsigned int beginEtaIndex () const;
      
  /**
   * @brief	returns the end eta index in the region.
   */
  unsigned int endEtaIndex () const;
      
  /**
   * @brief Returns the absolute transform of this element.
   */
  const GeoTrf::Transform3D &  getAbsoluteTransform () const;
      
  /**
   * @brief Returns the absolute transform of this element.
   */
  const GeoTrf::Transform3D &  getDefAbsoluteTransform () const;
      
  /**
   * @brief Returns the position of the center of the reference
   *    plane of the EMEC (back of front tranverse bars).  This
   *    method is not fast, the result should be cached if
   *    frequently used.
   */
  HepGeom::Point3D<double> getRefPlanePos () const;

  /**
   * @brief Returns the  position of the  focal point of the EMEC.
   *    This method is not fast, result should be cached if
   *    used frequently.
   */
  HepGeom::Point3D<double> getFocalPointPos () const;
      
  /**
   * @brief Returns the absolute transform of this element.
   */
  const Amg::Transform3D  getAbsoluteTransformAmg () const;

  /**
   * @brief Returns the absolute transform of this element.
   */
  const Amg::Transform3D  getDefAbsoluteTransformAmg () const;

  /**
   * @brief Returns the position of the center of the reference
   *    plane of the EMEC (back of front tranverse bars).  This
   *    method is not fast, the result should be cached if
   *    frequently used.
   */
  Amg::Vector3D getRefPlanePosAmg () const;

  /**
   * @brief Returns the  position of the  focal point of the EMEC.
   *    This method is not fast, result should be cached if
   *    used frequently.
   */
  Amg::Vector3D getFocalPointPosAmg () const;




  /**
   * @brief	The endcap index.  0=negative, 1=positive.
   */
  EMECDetectorRegion::DetectorSide getEndcapIndex () const;

  double getProjectivityDisplacement() const;

 private:
    
  EMECDetectorRegion(const EMECDetectorRegion &right);
  EMECDetectorRegion & operator=(const EMECDetectorRegion &right);
  const EMECDetDescr *m_descriptor;
  EMECDetectorRegion::DetectorSide m_endcapIndex;
  double m_projectivityDisplacement;

};



inline const EMECDetDescr * EMECDetectorRegion::getDescriptor () const
{
  
  return m_descriptor;
  
}

inline unsigned int EMECDetectorRegion::getSamplingIndex () const
{
  
  return m_descriptor->getSamplingIndex();
  
}

inline unsigned int EMECDetectorRegion::getRegionIndex () const
{
  
  return m_descriptor->getRegionIndex();
  
}

inline unsigned int EMECDetectorRegion::getRadialIndex () const
{
  
  return m_descriptor->getRadialIndex();
  
}

inline unsigned int EMECDetectorRegion::beginPhiIndex () const
{
  
  return m_descriptor->getPhiBinning().getFirstDivisionNumber();
  
}

inline unsigned int EMECDetectorRegion::endPhiIndex () const
{
  
  return m_descriptor->getPhiBinning().getFirstDivisionNumber() + m_descriptor->getPhiBinning().getNumDivisions();
  
}

inline unsigned int EMECDetectorRegion::beginEtaIndex () const
{
  
  return m_descriptor->getEtaBinning().getFirstDivisionNumber();
  
}

inline unsigned int EMECDetectorRegion::endEtaIndex () const
{
  
  return m_descriptor->getEtaBinning().getFirstDivisionNumber() + m_descriptor->getEtaBinning().getNumDivisions();
  
}


inline EMECDetectorRegion::DetectorSide EMECDetectorRegion::getEndcapIndex () const
{
  
  return m_endcapIndex;
  
}

inline double EMECDetectorRegion::getProjectivityDisplacement() const
{
  return m_projectivityDisplacement;
}



#endif
