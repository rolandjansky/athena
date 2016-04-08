/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          DummyAnnealingMaker.cxx - Description in header file
*********************************************************************/

#include "TrkVertexFitterUtils/DummyAnnealingMaker.h"

namespace Trk
{
  
  DummyAnnealingMaker::DummyAnnealingMaker(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p)
  {   
    declareInterface<IVertexAnnealingMaker>(this);
  }

  DummyAnnealingMaker::~DummyAnnealingMaker() {}

  StatusCode DummyAnnealingMaker::initialize() 
  { 
    msg(MSG::INFO)  << "Initialize successful" << endreq;
    return StatusCode::SUCCESS;
  }

  StatusCode DummyAnnealingMaker::finalize() 
  {
    msg(MSG::INFO)  << "Initialize successful" << endreq;
    return StatusCode::SUCCESS;
  }

  void DummyAnnealingMaker::reset() {
  }

  void DummyAnnealingMaker::anneal() {
  }

  double DummyAnnealingMaker::getWeight(double /* chisq */) const {
    return 0.5;
  }

  double DummyAnnealingMaker::getWeight(double /* chisq */ ,const std::vector<double>& /* allchisq */) const {
    return 0.5;
  }


}

	
