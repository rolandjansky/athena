// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloTowerStoreTestAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2013
 * @brief Regression test for CaloTowerStore.
 */

#ifndef CALOUTILS_CALOTOWERSTORETESTALG_H
#define CALOUTILS_CALOTOWERSTORETESTALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloUtils/CaloTowerStore.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

class CaloTowerStoreTestAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Constructor.
   */
  CaloTowerStoreTestAlg (const std::string& name,
                         ISvcLocator* pSvcLocator);

  /**
   * @brief Standard Gaudi initialize method.
   */
  virtual StatusCode initialize() override;

  /** 
   * @brief Standard Gaudi execute method.
   */
  virtual StatusCode execute() override;


  typedef std::vector<std::pair<unsigned, double> > test_tow_t;
  typedef std::vector<test_tow_t> test_tows_t;
  static void dump_tows (const test_tows_t& t);

private:
  static void iterate_full (const CaloTowerStore& store,
                     test_tows_t& tows);
  static void iterate_subseg (const CaloTowerStore& store,
                       const CaloTowerSeg::SubSeg& subseg,
                       test_tows_t& tows);

  void test_subseg_iter (const CaloDetDescrManager*,
			 const CaloTowerStore&,
                         const std::vector<CaloCell_ID::SUBCALO>&,
                         const CaloTowerSeg::SubSeg&);
  void test1();

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };
};




#endif // not CALOUTILS_CALOTOWERSTORETESTALG_H
