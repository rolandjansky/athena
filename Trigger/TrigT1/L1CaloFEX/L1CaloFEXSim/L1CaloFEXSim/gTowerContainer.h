///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gTowerContainer - Container for the gFEX towers
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#ifndef GTOWERCONTAINER_H
#define GTOWERCONTAINER_H
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/gTower.h"
#include <map>
#include <vector>


namespace LVL1 {

class gTowerContainer : public DataVector<LVL1::gTower>
{

 public:


 /** @brief type to be used for the internal lookup table, and to return list of towers */
  typedef std::vector<const LVL1::gTower> gTowerVector;

 /** @brief Return from non-const findTowerVector. */
  typedef std::vector<LVL1::gTower> MutablegTowerVector;

  /** @brief Main constructor */
  gTowerContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) ;
  //gTowerContainer() ;

  /** @brief Sized constructor */
  gTowerContainer(size_t n, SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS );

  /**  @brief destructor */
  virtual ~gTowerContainer() { };

  /** @brief reimplementation of const push_back */
  void push_back(int eta, int phi, int nphi, int keybase, int posneg);

  /** @brief utility function to help speed up accessing towers */
  bool fillContainerMap();

  /** @brief dump (obsolete) */
  void print() const;

  /** @brief fast find method given identifier. */
  const LVL1::gTower * findTower(int towerID) const;

  /** @brief fast find method given identifier. */
  LVL1::gTower * findTower(int towerID);

  /** @brief clear map */
  void clearContainerMap();

 private:
  /** @brief get message service */
  IMessageSvc* msgSvc() const;

  //* @brief Keeps track of the towerID of each gTower associated to each MAP index *.
  std::unordered_map<int,int> m_map_towerID_containerIndex;
};

}
CLASS_DEF( LVL1::gTowerContainer , 1009563874 , 1 )
SG_BASE(LVL1::gTowerContainer, DataVector<LVL1::gTower> );

#endif

