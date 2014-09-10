/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VertexPurityFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"

#include "TrkVertexFitterValidationUtils/TrkPriVxPurityTool.h"
#include "TrkVertexFitterValidationUtils/TrkPriVxPurity.h"
	
namespace D3PD {

VertexPurityFillerTool::VertexPurityFillerTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
  : VertexPurityFillerBase (type, name, parent),
    m_purityTool("Trk::TrkPriVxPurityTool")
{
  declareProperty("PurityTool", m_purityTool);

  book().ignore(); // Avoid coverity warnings.
}

StatusCode VertexPurityFillerTool::initialize()
{
  CHECK( VertexPurityFillerBase::initialize() );
  CHECK( m_purityTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode VertexPurityFillerTool::book()
{
  // purity
  CHECK( addVariable ("sumWeightInliers", m_sum_in) );
  CHECK( addVariable ("sumWeightOutliers", m_sum_out) );
  CHECK( addVariable ("sumWeightPileup", m_sum_pu) );
  CHECK( addVariable ("sumWeightFakes", m_sum_nt) );
  CHECK( addVariable ("nInliers", m_n_in) );
  CHECK( addVariable ("nOutliers", m_n_out) );
  CHECK( addVariable ("nPileUp", m_n_pu) );
  CHECK( addVariable ("nFakes", m_n_nt) );
  CHECK( addVariable ("purity", m_purity) );

  return StatusCode::SUCCESS;
}

StatusCode VertexPurityFillerTool::fill (const VertexPurityFillerObjectType& vertex)
{

  this->clearData();

  //vertex purity     
  const Trk::TrkPriVxPurity * pv_purity = m_purityTool->purity(&vertex);
	   
  if(pv_purity !=0){
    const std::vector<double> inl = pv_purity->inlierWeights();
    const std::vector<double> outl = pv_purity->outlierWeights();
    const std::vector<double> puv = pv_purity->pileUpWeights();
    const std::vector<double> nt = pv_purity->noTruthFound();
      
    *m_n_in = inl.size(); 
    *m_n_out = outl.size();
    *m_n_pu = puv.size();
    *m_n_nt = nt.size();   

    for(std::vector<double>::const_iterator i = inl.begin(); i != inl.end(); ++i) *m_sum_in  += *i;
    for(std::vector<double>::const_iterator i = outl.begin();i !=outl.end(); ++i) *m_sum_out += *i;
    for(std::vector<double>::const_iterator i = puv.begin(); i !=puv.end();  ++i) *m_sum_pu  += *i;
    for(std::vector<double>::const_iterator i = nt.begin();  i !=nt.end();   ++i) *m_sum_nt  += *i;

    int den = *m_sum_in + *m_sum_out + *m_sum_pu + *m_sum_nt;
    if (den > 0)
      *m_purity = (*m_sum_in - *m_sum_out - *m_sum_pu - *m_sum_nt) / den;

    delete pv_purity; 

  }//end of non-zero purity check

  return StatusCode::SUCCESS;
}

void VertexPurityFillerTool::clearData(){

  *m_sum_in = 0;
  *m_sum_out = 0;
  *m_sum_pu = 0;
  *m_sum_nt = 0;
  *m_n_in = 0;
  *m_n_out = 0;
  *m_n_pu = 0;
  *m_n_nt = 0;
  *m_purity = 0;
}

} // namespace D3PD
