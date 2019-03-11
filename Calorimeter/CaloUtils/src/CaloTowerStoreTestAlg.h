// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
class CaloDetDescrManager;


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
  void dump_tows (const test_tows_t& t);

private:
  const CaloDetDescrManager* m_caloDDM;

  void iterate_full (const CaloTowerStore& store,
                     test_tows_t& tows);
  void iterate_subseg (const CaloTowerStore& store,
                       const CaloTowerSeg::SubSeg& subseg,
                       test_tows_t& tows);

  void test_subseg_iter (const CaloTowerStore&,
                         const std::vector<CaloCell_ID::SUBCALO>&,
                         const CaloTowerSeg::SubSeg&);
  void test1();
};




#endif // not CALOUTILS_CALOTOWERSTORETESTALG_H
