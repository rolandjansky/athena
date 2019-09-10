/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  base_class(t,n,p)
{
  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);
}

//================ Destructor =================================================

InDet::ZVTOP_SimpleVtxProbCalc::~ZVTOP_SimpleVtxProbCalc()
{}


//================ Initialisation =================================================

StatusCode InDet::ZVTOP_SimpleVtxProbCalc::initialize()
{
  ATH_CHECK( AlgTool::initialize() );
  ATH_MSG_INFO ( "initialize() successful in " << name() );
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::ZVTOP_SimpleVtxProbCalc::finalize()
{
  return AlgTool::finalize();
}

//============================================================================================
double InDet::ZVTOP_SimpleVtxProbCalc::calcVtxProb(double & trk_func_sum, double & trk_func_sum2) const
{
  double vtx_prob = 0.;
  if (trk_func_sum != 0.) vtx_prob = trk_func_sum - trk_func_sum2/trk_func_sum;
  return vtx_prob;
}

