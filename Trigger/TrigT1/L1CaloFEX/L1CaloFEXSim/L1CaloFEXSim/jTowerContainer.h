///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Stolen shamelessly from CaloCellContainer.h and modified as required

#ifndef JTOWERCONTAINER_H
#define JTOWERCONTAINER_H

/** 
   @class jTowerContainer
   @brief Container class for jTower

   jTowerContainer is a container of all jFEX jTowers.
   It derives from DataVector<jTower>.

 */

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/jTower.h"

#include "Identifier/IdentifierHash.h"
#include "CxxUtils/PackedArray.h"
#include "CxxUtils/CachedValue.h"
#include "AthLinks/tools/findInContainer.h"

namespace LVL1 {

class jTowerContainer : public DataVector<LVL1::jTower>
{

 public:


 /** @brief type to be used for the internal lookup table, and to return list of towers */
  typedef std::vector<const LVL1::jTower> jTowerVector;
  
 /** @brief Return from non-const findTowerVector. */
  typedef std::vector<LVL1::jTower> MutablejTowerVector;
  
  /** @brief Main constructor */
  jTowerContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) ;
  //jTowerContainer() ;

  /** @brief Sized constructor */
  jTowerContainer(size_t n, SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS );

  /**  @brief destructor */  
  virtual ~jTowerContainer() { };

  /** @brief reimplementation of const push_back */
  void push_back(float eta, float phi, float keybase, int posneg, float centre_et = 0.0, float centre_phi = 0.0, int fcal_layer = -1);

  /** @brief utility function to help speed up accessing towers */
  bool fillContainerMap();

  /** @brief dump (obsolete) */
  void print() const;

  /** @brief fast find method given identifier. */ 
  const LVL1::jTower * findTower(int towerID) const;
  
  /** @brief fast find method given identifier. */ 
  LVL1::jTower * findTower(int towerID);

  /** @brief clear map */
  void clearContainerMap();

 private:
  /** @brief get message service */
  IMessageSvc* msgSvc() const;

  //* @brief Keeps track of the towerID of each jTower associated to each MAP index *.
  std::map<int,int> m_map_towerID_containerIndex;
};

}
CLASS_DEF( LVL1::jTowerContainer , 1141097037 , 1 )
SG_BASE(LVL1::jTowerContainer, DataVector<LVL1::jTower> );

#endif

