// Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_TESTRANDOMSEQALG_H
#define ATHENASERVICES_TESTRANDOMSEQALG_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
class IAtRndmGenSvc;
namespace CLHEP { class HepRandomEngine; }
/** @class TestRandomSegAlg
   * @brief a trivial algorithm to test the sequence of random numbers
   * produced by an IAtRndmGenSvc
   * 
   * @author srinir@bnl.gov
   * $Id: TestRandomSegAlg.h,v 1.12 2008-12-15 19:14:58 binet Exp $
   */
class TestRandomSeqAlg : public AthAlgorithm {

public:
  TestRandomSeqAlg(const std::string& name, ISvcLocator* pSvcLocator);
  /// \name implement IAlgorithm
  //@{
  virtual StatusCode initialize();
  virtual StatusCode execute();
  //virtual StatusCode finalize();
  //@}
  
private:
  /// handle to the @c IAtRndmGenSvc we want to test
  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
  StringProperty m_streamName;
  /// number of random numbers to shoot (and print)
  IntegerProperty           m_noOfNo;
  CLHEP::HepRandomEngine* m_pEng;
};

#endif
