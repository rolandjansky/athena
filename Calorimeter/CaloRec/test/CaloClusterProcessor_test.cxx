/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterProcessor_test.cxx,v 1.3 2009-04-18 02:56:19 ssnyder Exp $
/**
 * @file  CaloClusterProcessor_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date March, 2006
 * @brief Unit test for CaloClusterProcessor.
 */

#undef NDEBUG

#include "CaloRec/CaloClusterProcessor.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "TestTools/initGaudi.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include <cassert>
#include <iostream>


class TAlg
  : public AthAlgorithm
{
public:
  TAlg( const std::string& name, ISvcLocator *svcloc)
    : AthAlgorithm (name, svcloc) {}

  virtual StatusCode execute() { return StatusCode::SUCCESS; }
};


class TTool
  : public CaloClusterProcessor
{
public:
  TTool (const std::string& type,
         const std::string& name,
         const IInterface* parent)
    : CaloClusterProcessor (type, name, parent) {}

  using CaloClusterProcessor::execute;
  virtual StatusCode execute (xAOD::CaloCluster* cluster);
};


StatusCode TTool::execute (xAOD::CaloCluster* cluster)
{
  std::cout << "execute " << cluster->eta0() << " " << cluster->phi0() << "\n";
  return StatusCode::SUCCESS;
}


int main()
{
  ISvcLocator* svcloc;
  if (!Athena_test::initGaudi("", svcloc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }  

  TAlg talg ("talg", svcloc); talg.addRef();
  TTool t ("type", "name", &talg); t.addRef();
  xAOD::CaloClusterContainer c;
  xAOD::CaloClusterAuxContainer aux;
  c.setStore (&aux);
  c.push_back (new xAOD::CaloCluster());
  c.push_back (new xAOD::CaloCluster());
  c[0]->setEta0(1);
  c[0]->setPhi0(2);
  c[1]->setEta0(3);
  c[1]->setPhi0(4);
  assert (t.execute (&c).isSuccess());
  return 0;
}
