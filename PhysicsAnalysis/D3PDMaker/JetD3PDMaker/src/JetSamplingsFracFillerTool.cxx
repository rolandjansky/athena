/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file   JetD3PDMaker/src/JetSamplingsFracFillerTool.cxx
 * @author Haifeng Li <Haifeng.Li@cern.ch>
 * @date   Sep. 22, 2010
 * @brief Block filler tool for Jet SamplingMax
 */


#include "JetSamplingsFracFillerTool.h"
#include "checkJet.h"
#include "JetEvent/Jet.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "AthenaKernel/errorcheck.h"



namespace {


double fracSamplingMax(const Jet* jet, int& SamplingMax)
{
  std::string base="energy_";
  double fracSamplingMax=-999999999.;    
  double sumE_samplings=0.;
  unsigned int nsamp = CaloSampling::getNumberOfSamplings();
  for ( unsigned int i = 0; i < nsamp; i++)
    {
      double e = jet->getShape(base+ CaloSampling::getSamplingName(i));
      sumE_samplings+=e;
      if (e>fracSamplingMax)
	{
	  fracSamplingMax=e;
	  SamplingMax=i;
	}
    }

  if(sumE_samplings!=0)
    fracSamplingMax/=sumE_samplings;
  else fracSamplingMax=0;
  
  return fracSamplingMax;
}


}


namespace D3PD {
  
  
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetSamplingsFracFillerTool::JetSamplingsFracFillerTool (const std::string& type,
							const std::string& name,
							const IInterface* parent)
  : BlockFillerTool<Jet> (type, name, parent),
    m_checked (false),
    m_goodjets (true)
{
  book().ignore(); // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetSamplingsFracFillerTool::book()
{
  
  CHECK( addVariable ( "SamplingMax",     m_SamplingMax     )  );
  CHECK( addVariable ( "fracSamplingMax", m_fracSamplingMax )  );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object. The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode JetSamplingsFracFillerTool::fill(const Jet& jet)
{
  if (!m_checked) {
    m_goodjets = checkJet (jet);
    m_checked = true;
  }

  if (m_goodjets) {
    *m_SamplingMax     = CaloSampling::Unknown; 
    *m_fracSamplingMax = fracSamplingMax(&jet, *m_SamplingMax);
  }
  

  return StatusCode::SUCCESS;

}


} // namespace D3PD
