/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArDetectorManager_h
#define LArDetectorManager_h 1
#include "CLIDSvc/CLASS_DEF.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
class EMBDetectorManager;
class EMECDetectorManager;
class HECDetectorManager;
class FCALDetectorManager;
/** 
 *      @brief Stored in storegate.  Provides access to EMB, EMEC, HEC and FCAL
 *      Detector Managers.
 */

/**
 *	This manager for the whole LAr can purvey not only
 *	physical volumes but also pointers to managers for the
 *	sub-calorimeters:EMB, EMEC,  HEC and FCAL.
 */

class LArDetectorManager : public GeoVDetectorManager  
{
  
  

 public:
    
  /**
   * @brief Constructor
   */
  LArDetectorManager();
      
  /**
   * @brief Constructor.
   */
  LArDetectorManager (const EMBDetectorManager* emb, const EMECDetectorManager* emec, const HECDetectorManager* hec, const FCALDetectorManager* fcal);

  /**
   * @brief Destructor
   */
  ~LArDetectorManager();
      
  /**
   * @brief	Gets the ith tree top.
   */
  virtual PVConstLink getTreeTop (unsigned int i) const;
      
  /**
   * @brief	Gets the number of tree tops.
   */
  virtual unsigned int getNumTreeTops () const;
      
  /**
   * @brief	Add a Tree Top
   */
  virtual void addTreeTop (PVLink treeTop);
      
  /**
   * @brief	Pointer to the manager for the Electromagnetic Barrel. May be NULL.
   */
  const EMBDetectorManager* getEmbManager () const;

      
  /**
   * @brief	Pointer to the manager for the Electromagnetic Endcap. May be NULL.
   */
  const EMECDetectorManager* getEmecManager () const;

      
  /**
   * @brief	Pointer to the manager for the Hadronic Endcap.  May be NULL.
   */
  const HECDetectorManager* getHecManager () const;

      
  /**
   * @brief	Pointer to the manager for the Forward Calorimeter.  Maybe NULL.
   */
  const FCALDetectorManager* getFcalManager () const;


  /**
   * @brief      Set Test Beam flag
   */
  void isTestBeam(bool flag);

  /**
   * @brief      Get Test Beam flag
   */
  bool isTestBeam() const;


 private:
    
  LArDetectorManager(const LArDetectorManager &right);
  LArDetectorManager & operator=(const LArDetectorManager &right);
  const EMBDetectorManager* m_embManager;
  const EMECDetectorManager* m_emecManager;
  const HECDetectorManager* m_hecManager;
  const FCALDetectorManager* m_fcalManager;
  std::vector<PVLink> m_treeTop;

  bool m_isTestBeam;
};







inline const EMBDetectorManager* LArDetectorManager::getEmbManager () const
{
  
  return m_embManager;
  
}

inline const EMECDetectorManager* LArDetectorManager::getEmecManager () const
{
  
  return m_emecManager;
  
}

inline const HECDetectorManager* LArDetectorManager::getHecManager () const
{
  
  return m_hecManager;
  
}

inline const FCALDetectorManager* LArDetectorManager::getFcalManager () const
{
  
  return m_fcalManager;
  
}

inline void LArDetectorManager::isTestBeam(bool flag)
{
  m_isTestBeam = flag;
}

inline bool LArDetectorManager::isTestBeam() const
{
  return m_isTestBeam;
}


CLASS_DEF(LArDetectorManager, 14436910, 1)



#endif
