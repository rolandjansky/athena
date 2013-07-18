/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FCALDetectorManager_h
#define FCALDetectorManager_h 1
#include "LArReadoutGeometry/FCAL_ChannelMap.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "CLIDSvc/CLASS_DEF.h"
#include <vector>
#include "GeoModelKernel/GeoVDetectorManager.h"
class FCALHVManager;
class IMessageSvc;

/**
 *     @brief A manager class providing access to readout geometry information
 *     for the forward calorimeter.
 */

/**
 *	Manager class for the FCAL which provides access to the
 *	FCAL Modules, the Physical Volumes, and the FCAL Channel
 *	Map.  The FCAL Channel Map is a descriptor for the
 *	FCAL.
 */


class FCALDetectorManager : public GeoVDetectorManager  
{
  
 private:
  typedef const FCALModule * ModulePtr;
 public:
  typedef std::vector<const FCALModule *>::const_iterator ConstIterator;
  

  public:
      
  /**
   * @brief Constructor
   */
  FCALDetectorManager ();
    
  /**
   * @brief Desctructor
   */
  virtual ~FCALDetectorManager();
      
  /**
   * @brief Iterate over FCAL Modules
   */
  FCALDetectorManager::ConstIterator beginFCAL () const;
      
  /**
   * @brief Iterate over FCAL Modules
   */
  FCALDetectorManager::ConstIterator endFCAL () const;
      
  /**
   * @brief Retreive a specific FCAL Module
   */
  const FCALModule* getFCAL (FCALModule::Module module, FCALModule::Endcap endcap) const;
      
  /**
   * @brief Gets the number of tree tops.
   */
  virtual unsigned int getNumTreeTops () const;
      
  /**
   * @brief Gets the ith tree top.
   */
  virtual PVConstLink getTreeTop (unsigned int i) const;
      
  /**
   * @brief Returns the Channel Map.
   */
  const FCAL_ChannelMap * getChannelMap () const;
      
  /**
   * @brief Adds an FCAL Module.
   */
  void addModule (FCALModule* fcalModule);
      
  /**
   * @brief Add a Tree Top
   */
  virtual void addTreeTop (PVLink treeTop);

  /**
   * @brief Get the HV Manager:
   */
  const FCALHVManager * getHVManager() const;

  private:
    
      FCALDetectorManager(const FCALDetectorManager &right);
      FCALDetectorManager & operator=(const FCALDetectorManager &right);
      
      /**
       * @brief This is the set of tree tops managed by this detector
       *	node.  The tree top is a toplevel FCAL or Cryostat.
       */
      std::vector<PVLink> m_treeTop;
      
      /**
       * @brief This is the array in which three modules in two sides
       *	are kept. This is for random access to the modules.
       */
      ModulePtr m_Module[2][3];
      
      /**
       * @brief Vector of FCAL Modules.  This is for iterative access to
       *	the modules.
       */
      std::vector<const FCALModule *> m_DetList;

      const FCAL_ChannelMap *m_fcal_channel_map;

      mutable const FCALHVManager *m_HVManager;
      
};




inline const FCAL_ChannelMap * FCALDetectorManager::getChannelMap () const
{
  
  return m_fcal_channel_map;
  
}


CLASS_DEF(FCALDetectorManager, 12868358, 1)



#endif
