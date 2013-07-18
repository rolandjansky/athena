/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMECDetectorManager_h
#define EMECDetectorManager_h 1
#include "CLIDSvc/CLASS_DEF.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/EMECPresamplerHVManager.h"
class EMECDetDescr;
class EMECDetectorRegion;
/**
 *     @brief A manager class providing access to readout geometry information
 *     for the electromagnetic endcap calorimeter.
 */

/**
 *	The EMECDetectorManager provides access to EMEC Regions,
 *	to Descriptors for EMEC regions, to the physical volumes
 *	(tree tops) within the EMEC, and to a number of
 *	important engineering numbers (like the focal lengths)
 *	within the EMEC.
 *  
 */



class EMECDetectorManager : public GeoVDetectorManager  
{
  
 public:
  typedef std::vector<const EMECDetectorRegion  *>::const_iterator DetectorRegionConstIterator;
  typedef EMECDetectorRegion const * EMECDetRegionArray[6][4][2][2]; // x 2 endcaps

 private:
  class EMECMagicNumbers;

 public:
    
  /**
   * @brief  Constructor
   */
  EMECDetectorManager();

  /**
   * @brief  Destructor
   */
  ~EMECDetectorManager();
      
  /**
   * @brief 	Iterate over detector regions
   */
  EMECDetectorManager::DetectorRegionConstIterator beginDetectorRegion () const;
      
  /**
   * @brief 	Iterate over detector regions
   */
  EMECDetectorManager::DetectorRegionConstIterator endDetectorRegion () const;
      
  /**
   * @brief 	Random Access to detector regions
   */
  const EMECDetectorRegion * getDetectorRegion (unsigned int endcap, unsigned int sampling, unsigned int region, bool inner) const;
      
  /**
   * @brief 	Gets the number of detectors in the set of detector regions.
   */
  unsigned int getNumDetectorRegions () const;
      
  /**
   * @brief 	Gets the ith tree top.
   */
  virtual PVConstLink getTreeTop (unsigned int i) const;
      
  /**
   * @brief 	Gets the total number of tree tops.
   */
  virtual unsigned int getNumTreeTops () const;
      
  /**
   * @brief 	This provides the distance in CLHEP::mm from the Focal Plane to
   *	the reference plane.  The reference plane is at the back
   *	of the  transverse bar.  The Focal distance is the point
   *	where cell boundaries cross.  It is an engineering
   *	number used in the fabrication of the emec.  Its value
   *	is 3689.5.
   */
  double getFocalToRef () const;
      
  /**
   * @brief 	This provides the distance in CLHEP::mm from the Active EMEC
   *	volume  to the reference plane.  The reference plane is
   *	at the back of the  transverse bar.  Active EMEC volume
   *	is called LArOuterWheel or LArInnerWheel.  Its value is
   *	11 CLHEP::mm.
   */
  double getRefToActive () const;
      
  /**
   * @brief 	Gives the overall active length of the EMEC outer and inner wheel volumes. 
   */

  double getActiveLength () const;
      
  /**
   * @brief 	This provides the distance in CLHEP::mm from the Active Endcap
   *	Presampler volume (center)  to the reference plane.  The
   *	reference plane is at the back of the  transverse bar.
   */
  double getRefToPresampler () const;
      
  /**
   * @brief 	Gives the overall active length of the presampler volume.
   */
  double getPresamplerFullLength () const;
      
  /**
   * @brief 	Gets the length of the ZIW Array.
   */
  unsigned int getNumZIW () const;
      
  /**
   * @brief 	This method provides access to the Array ZIW, which in
   *	turn comes from the database.  This array provides a
   *	list of positions where the two sampling layers in the
   *	inner wheel of the EMEC separate.  These positions are
   *	measured from the  focal plane.
   */
  double getZIW (unsigned int i) const;
      
  /**
   * @brief 	Gets the length of the ZSEP12 Array.
   */
  unsigned int getNumZSEP12 () const;
      
  /**
   * @brief 	This method provides access to the Array ZSEP12, which
   *	in turn comes from the database (EmecSamplingSep
   *	table).  This array provides a list of positions where
   *	the first two sampling layers in the outer wheel of the
   *	EMEC separate.  These positions are measured from the
   *	focal plane.
   */
  double getZSEP12 (unsigned int i) const;
      
  /**
   * @brief 	Gets the length of the ZSEP23Array.
   */
  unsigned int getNumZSEP23 () const;
      
  /**
   * @brief 	This method provides access to the Array ZSEP23, which
   *	in turn comes from the database (EmecSamplingSep
   *	table).  This array provides a list of positions where
   *	the second two sampling layers in the outer wheel of the
   *	EMEC separate.  These positions are measured from the
   *	focal plane.
   */
  double getZSEP23 (unsigned int i) const;
      
  /**
   * @brief 	Add a new HEC Detector Region
   */
  void addDetectorRegion (const EMECDetectorRegion *region);
      
  /**
   * @brief 	Add a Tree Top
   */
  virtual void addTreeTop (PVLink treeTop);

  /**
   * @brief 	Get the HV Managers
   */
  const EMECHVManager * getHVManager(EMECHVManager::IOType io) const;

  /**
   * @brief 	Get the HV Manager (presampler)
   */
  const EMECPresamplerHVManager * getPresamplerHVManager() const;

 private:
    
  EMECDetectorManager(const EMECDetectorManager &right);
  EMECDetectorManager & operator=(const EMECDetectorManager &right);

  /**
   * @brief 	This is the set of tree tops managed by this detector
   *	node.  The tree top is a toplevel FCAL or Cryostat.
   */
  std::vector<PVLink> m_treeTop;
      
  /**
   * @brief 	A vector of Region pointers which exists in order to
   *	provide iterative  access to any EMECDetectorRegion held
   *	by this manager.
   */
  std::vector<const EMECDetectorRegion *>  m_DetRegionsIterative;
      
  /**
   * @brief 	A vector of Region pointers which exists in order to
   *	provide iterative  access to any EMECDetectorRegion held
   *	by this manager.
   */
  EMECDetRegionArray m_DetRegionsRandom;
      
  /**
   * @brief 	A vector of doubles used to hold an array of sampling
   *	separations for the EMEC inner wheel.  Note, these
   *	separations are relative to the focal point of the EMEC.
   */
  std::vector<double> m_ziw;
      

      
  /**
   * @brief 	A vector of doubles used to hold an array of sampling
   *	separations for the EMEC outer wheel between sampling
   *	layer 1 and 2.  Note, these separations are relative to
   *	the focal point of the EMEC.
   */
  std::vector<double> m_zsep12;
      
  /**
   * @brief 	A vector of doubles used to hold an array of sampling
   *	separations for the EMEC outer wheel between sampling
   *	layer 2 and 3.  Note, these separations are relative to
   *	the focal point of the EMEC.
   */
  std::vector<double> m_zsep23;

  /**
   * @brief 	Clockwork for the EMECDetectorManager.
   */
  EMECMagicNumbers *m_MagicNumbers;

  /**
   * @brief 	The HV Managers for the emec;
   */
  mutable const EMECHVManager *m_HVManager[2];

  mutable const EMECPresamplerHVManager *m_presamplerHVManager;
};

/**
 * @brief This is a collection of numbers used to specify the construction
 * of the electromagnetic endcap calorimeter and its cells.
 */

class EMECDetectorManager::EMECMagicNumbers {

 public:
  /**
   * @brief Distance from focal length to reference plane 	
   */
  double focalToRef;

  /**
   * @brief Distance from reference plane to active volume
   */
  double refToActive;

  /**
   * @brief Total active length
   */
  double activeLength;

  /**
   * @brief Distance from reference plane to presampler
   */
  double refToPresampler;

  /**
   * @brief Overall length of the presampler
   */
  double presamplerLength;
};




inline double EMECDetectorManager::getFocalToRef () const
{
  
  return m_MagicNumbers->focalToRef;
  
}

inline double EMECDetectorManager::getRefToActive () const
{
  
  return m_MagicNumbers->refToActive;
  
}

inline double EMECDetectorManager::getActiveLength () const
{
  
  return m_MagicNumbers->activeLength;;
  
}

inline double EMECDetectorManager::getRefToPresampler () const
{
  
  return m_MagicNumbers->refToPresampler;
  
}

inline double EMECDetectorManager::getPresamplerFullLength () const
{
  
  return m_MagicNumbers->presamplerLength;
  
}

inline unsigned int EMECDetectorManager::getNumZIW () const
{
  
  return m_ziw.size();
  
}

inline double EMECDetectorManager::getZIW (unsigned int i) const
{
  
  return m_ziw[i];
  
}

inline unsigned int EMECDetectorManager::getNumZSEP12 () const
{
  
  return m_zsep12.size();
  
}

inline double EMECDetectorManager::getZSEP12 (unsigned int i) const
{
  
  return m_zsep12[i];
  
}

inline unsigned int EMECDetectorManager::getNumZSEP23 () const
{
  
  return m_zsep23.size();
  
}

inline double EMECDetectorManager::getZSEP23 (unsigned int i) const
{
  
  return m_zsep23[i];
  
}


CLASS_DEF(EMECDetectorManager, 74566646, 1)



#endif
