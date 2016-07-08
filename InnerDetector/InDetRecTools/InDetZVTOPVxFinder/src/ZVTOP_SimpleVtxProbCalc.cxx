/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZVTOP_SimpleVtxProbCalc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetZVTOPVxFinder/ZVTOP_SimpleVtxProbCalc.h"

//================ Constructor =================================================

InDet::ZVTOP_SimpleVtxProbCalc::ZVTOP_SimpleVtxProbCalc(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p)
{
  declareInterface<IZVTOP_SimpleVtxProbCalc>(this);

  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);
}

//================ Destructor =================================================

InDet::ZVTOP_SimpleVtxProbCalc::~ZVTOP_SimpleVtxProbCalc()
{}


//================ Initialisation =================================================

StatusCode InDet::ZVTOP_SimpleVtxProbCalc::initialize()
{
  
  StatusCode sc = AlgTool::initialize();

  if (sc.isFailure()) return sc;

  msg (MSG::INFO) << "initialize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::ZVTOP_SimpleVtxProbCalc::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//============================================================================================
double InDet::ZVTOP_SimpleVtxProbCalc::calcVtxProb(double & trk_func_sum, double & trk_func_sum2)
{
  double vtx_prob = 0.;
  if (trk_func_sum != 0.) vtx_prob = trk_func_sum - trk_func_sum2/trk_func_sum;
  return vtx_prob;
}

