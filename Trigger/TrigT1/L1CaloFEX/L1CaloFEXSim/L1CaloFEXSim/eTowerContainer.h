///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Stolen shamelessly from CaloCellContainer.h and modified as required

#ifndef ETOWERCONTAINER_H
#define ETOWERCONTAINER_H

/** 
   @class eTowerContainer
   @brief Container class for eTower

   eTowerContainer is a container of all eFEX eTowers.
   It derives from DataVector<eTower>.

 */

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/eTower.h"

#include "Identifier/IdentifierHash.h"
#include "CxxUtils/PackedArray.h"
#include "CxxUtils/CachedValue.h"
#include "AthLinks/tools/findInContainer.h"

namespace LVL1 {

class eTowerContainer : public DataVector<LVL1::eTower>
{

 public:


 /** @brief type to be used for the internal lookup table, and to return list of towers */
  typedef std::vector<const LVL1::eTower> eTowerVector;
  
 /** @brief Return from non-const findTowerVector. */
  typedef std::vector<LVL1::eTower> MutableeTowerVector;
  
  /** @brief Main constructor */
  eTowerContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) ;
  //eTowerContainer() ;

  /** @brief Sized constructor */
  eTowerContainer(size_t n, SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS );

  /**  @brief destructor */  
  virtual ~eTowerContainer() { };

  /** @brief reimplementation of const push_back */
  void push_back(float eta, float phi, float keybase, int posneg);

  /** @brief utility function to help speed up accessing towers */
  bool fillContainerMap();

  /** @brief dump (obsolete) */
  void print() const;

  /** @brief fast find method given identifier. */ 
  const LVL1::eTower * findTower(int towerID) const;
  
  /** @brief fast find method given identifier. */ 
  LVL1::eTower * findTower(int towerID);

  /** @brief clear map */
  void clearContainerMap();

 private:
  /** @brief get message service */
  IMessageSvc* msgSvc() const;

  //* @brief Keeps track of the towerID of each eTower associated to each MAP index *.
  std::map<int,int> m_map_towerID_containerIndex;
};

}
CLASS_DEF( LVL1::eTowerContainer , 1143075806 , 1 )
SG_BASE(LVL1::eTowerContainer, DataVector<LVL1::eTower> );

#endif

