/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HECDetectorManager_h
#define HECDetectorManager_h 1
#include "CLIDSvc/CLASS_DEF.h"
#include "LArReadoutGeometry/HECLongBlock.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
class HECDetDescr;
class HECDetectorRegion;
class HECHVManager;
/**
 *     @brief A manager class providing access to readout geometry information
 *     for the hadronic endcap calorimeter.
 */

/**
 *	A manager for the HEC.  Provides access to:  HEC
 *	Regions, Descriptors to HEC Regions, Longitudinal HEC
 *	blocks, and top level physical volumes (or tree tops).
 */

class HECDetectorManager : public GeoVDetectorManager  
{
  
 public:

  typedef std::vector<const HECLongBlock *>::const_iterator BlockConstIterator;
  typedef std::vector<const HECDetectorRegion  *>::const_iterator DetectorRegionConstIterator;
  typedef HECDetectorRegion const * HECDetRegionArray[2][4][2];
  

 public:
    
  
  /**
   * @brief Constructor
   */
  HECDetectorManager(bool isTestBeam=false);
  
  /**
   * @brief Destructor
   */
  ~HECDetectorManager();
      
  /**
   * @brief	Iterate over detector regions
   */
  HECDetectorManager::DetectorRegionConstIterator beginDetectorRegion () const;
      
  /**
   * @brief	Iterate over detector regions
   */
  HECDetectorManager::DetectorRegionConstIterator endDetectorRegion () const;
      
  /**
   * @brief	Random Access to detector regions
   */
  const HECDetectorRegion * getDetectorRegion (unsigned int endcap, unsigned int sampling, unsigned int region) const;
      
  /**
   * @brief	Gets the number of detectors in the set of detector regions.
   */
  unsigned int getNumDetectorRegions () const;
      
  /**
   * @brief	Gets the ith tree top.
   */
  virtual PVConstLink getTreeTop (unsigned int i) const;
      
  /**
   * @brief	Gets the total number of tree tops.
   */
  virtual unsigned int getNumTreeTops () const;
      
  /**
   * @brief	Iterate over blocks.
   */
  HECDetectorManager::BlockConstIterator beginBlock () const;
      
  /**
   * @brief	Iterate over blocks
   */
  HECDetectorManager::BlockConstIterator endBlock () const;
      
  /**
   * @brief	Random Access to Blocks.
   */
  const HECLongBlock * getBlock (unsigned int i) const;
      
  /**
   * @brief	Gets the number of blocks in the set of blocks.
   */
  unsigned int getNumBlocks () const;
      
  /**
   * @brief	Add a new HEC Detector Region
   */
  void addDetectorRegion (const HECDetectorRegion *region);
      
  /**
   * @brief	Add a Tree Top
   */
  virtual void addTreeTop (PVLink treeTop);

      
  /**
   * @brief	
   *	
   *	Distance from focal point to front face of HEC1
   *	
   */
  double getFocalToRef1 () const;

  /**
   * @brief	
   *	
   *	Distance from focal point to front face of HEC2
   *	
   */
  double getFocalToRef2 () const;

  /**
   * @brief	Get the HV Manager
   */
  const HECHVManager * getHVManager() const;


 private:
    
  HECDetectorManager(const HECDetectorManager &right);
  HECDetectorManager & operator=(const HECDetectorManager &right);
  std::vector<const HECLongBlock *> m_HecLongBlock;
      
  /**
   * @brief	This is the set of tree tops managed by this detector
   *	node.  The tree top is a toplevel FCAL or Cryostat.
   */
  std::vector<PVLink> m_treeTop;
      

      
  /**
   * @brief	An array of Region pointers which exists in order to
   *	provide random access to any HECDetectorRegion  held by
   *	this manager.
   */
  HECDetRegionArray m_DetRegionsRandom;
      

      
  /**
   * @brief	A vector of Region pointer which exists in order to
   *	provide iterative  access to any HECDetectorRegion held
   *	by this manager.
   */
  std::vector<const HECDetectorRegion *>  m_DetRegionsIterative;
      
  double m_focalToRef1;
  double m_focalToRef2;

  mutable const HECHVManager * m_HVManager;

  bool m_isTestBeam;

};




inline double HECDetectorManager::getFocalToRef1 () const
{
  
  return m_focalToRef1;
  
}

inline double HECDetectorManager::getFocalToRef2 () const
{
  
  return m_focalToRef2;
  
}


CLASS_DEF(HECDetectorManager, 132338641,  1)



#endif
