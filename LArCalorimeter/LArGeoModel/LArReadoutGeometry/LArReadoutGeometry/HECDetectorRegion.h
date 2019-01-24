/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARREADOUTGEOMETRY_HECDETECTORREGION_H
#define LARREADOUTGEOMETRY_HECDETECTORREGION_H
#include "LArReadoutGeometry/HECDetDescr.h"
#include "LArReadoutGeometry/HECCellConstLink.h"
#include "GeoModelKernel/GeoVDetectorElement.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "CLHEP/Geometry/Point3D.h"

/**
 * @class HECDetectorRegion
 *
 * @brief Description of a region of homogenous granularity in the
 *        hadronic endcap calorimeter
 *
 *	This class combines a description of the cell
 *	granularity with a physical volume to create a
 *	description of a HEC region positioned within ATLAS.  It
 *	provides access to the HEC Cells.
 */



class HECDetectorRegion : public GeoVDetectorElement  
{
  
 public:
  typedef enum {NEG=0,POS=1} DetectorSide;
  

 public:
    
      
  /**
   * @brief Constructor.
   */
  HECDetectorRegion (const GeoVFullPhysVol *physVol
		     , const HECDetDescr *hecDescriptor
		     , DetectorSide endcap
		     , double projectivityDisplacement = 4*Gaudi::Units::cm);

  /**
   * @brief Destructor
   */
  ~HECDetectorRegion();
      
  /**
   * @brief Retrieve a cell with eta index and phi index.
   */
  HECCellConstLink getHECCell (unsigned int ieta, unsigned int iphi) const;
      
  /**
   * @brief Returns the Descriptor for this region.
   */
  const HECDetDescr * getDescriptor () const;
      
  /**
   * @brief Returns the Sampling Layer Index (0-3)
   */
  unsigned int getSamplingIndex () const;
      
  /**
   * @brief Returns the Region Index
   */
  unsigned int getRegionIndex () const;
      
  /**
   * @brief returns the first phi index in the region.
   */
  unsigned int beginPhiIndex () const;
      
  /**
   * @brief returns the end phi index in the region.
   */
  unsigned int endPhiIndex () const;
      
  /**
   * @brief returns the first eta index in the region.
   */
  unsigned int beginEtaIndex () const;
      
  /**
   * @brief returns the end eta index in the region.
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
   * @brief Returns the absolute transform of this element.
   */
  const Amg::Transform3D  getAbsoluteTransformAmg () const;

  /**
   * @brief Returns the absolute transform of this element.
   */
  const Amg::Transform3D  getDefAbsoluteTransformAmg () const;
      
  /**
   * @brief Returns the position of the center of the reference
   *    plane of the HEC (front face of the HEC volume).  This
   *    method is not fast, the result should be cached if
   *    frequently used.
   */
  HepGeom::Point3D<double> getRefPlanePos () const;
      
  /**
   * @brief Returns the  position of the  focal point of the HEC.
   *    This method is not fast, result should be cached if
   *    used frequently.
   */
  HepGeom::Point3D<double> getFocalPointPos () const;

  /**
   * @brief Returns the position of the center of the reference
   *    plane of the HEC (front face of the HEC volume).  This
   *    method is not fast, the result should be cached if
   *    frequently used.
   */
  Amg::Vector3D getRefPlanePosAmg () const;
      
  /**
   * @brief Returns the  position of the  focal point of the HEC.
   *    This method is not fast, result should be cached if
   *    used frequently.
   */
  Amg::Vector3D getFocalPointPosAmg () const;


  /**
   * @brief The endcap index.  0=negative, 1=positive.
   */
  HECDetectorRegion::DetectorSide getEndcapIndex () const;

  double getProjectivityDisplacement() const;

 private:
    
  HECDetectorRegion(const HECDetectorRegion &right);
  HECDetectorRegion & operator=(const HECDetectorRegion &right);
  const HECDetDescr *m_descriptor;
  HECDetectorRegion::DetectorSide m_endcapIndex;

  double m_projectivityDisplacement;
};




// Class HECDetectorRegion 



inline const HECDetDescr * HECDetectorRegion::getDescriptor () const
{
  
  return m_descriptor;
  
}

inline unsigned int HECDetectorRegion::getSamplingIndex () const
{
  
  return getDescriptor()->getSamplingIndex();
  
}

inline unsigned int HECDetectorRegion::getRegionIndex () const
{
  
  return getDescriptor()->getRegionIndex();
  
}

inline unsigned int HECDetectorRegion::beginPhiIndex () const
{
  
  return m_descriptor->getPhiBinning().getFirstDivisionNumber();
  
}

inline unsigned int HECDetectorRegion::endPhiIndex () const
{
  
  return m_descriptor->getPhiBinning().getFirstDivisionNumber() + m_descriptor->getPhiBinning().getNumDivisions();
  
}

inline unsigned int HECDetectorRegion::beginEtaIndex () const
{
  
  return m_descriptor->getEtaBinning().getFirstDivisionNumber();
  
}

inline unsigned int HECDetectorRegion::endEtaIndex () const
{
  
  return m_descriptor->getEtaBinning().getFirstDivisionNumber() + m_descriptor->getEtaBinning().getNumDivisions();
  
}



inline HECDetectorRegion::DetectorSide HECDetectorRegion::getEndcapIndex () const
{
  
  return m_endcapIndex;
  
}

inline double HECDetectorRegion::getProjectivityDisplacement() const
{
  return m_projectivityDisplacement;
}




#endif
