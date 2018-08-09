/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          DetAnnealingMaker.cxx - Description in header file
*********************************************************************/

#include "TrkVertexFitterUtils/DetAnnealingMaker.h"
#include <math.h>

namespace {
  double gauss(const double value2, const double error2) {
    return exp(-1./2.*value2/error2);
  }
}


namespace Trk
{
  
  DetAnnealingMaker::DetAnnealingMaker(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_SetOfTemperatures(1,3),
    m_isequilibrium(false),
    m_cutoff(9.),
    m_PtrToActualTemperature(0)
  {   
    declareProperty("SetOfTemperatures",     m_SetOfTemperatures);		    
    declareProperty("Cutoff", m_cutoff);    
    declareInterface<IVertexAnnealingMaker>(this);
  }

  DetAnnealingMaker::~DetAnnealingMaker() {}

  StatusCode DetAnnealingMaker::initialize() 
  { 
    msg(MSG::INFO)  << "Initialize successful" << endreq;
    return StatusCode::SUCCESS;
  }

  StatusCode DetAnnealingMaker::finalize() 
  {
    msg(MSG::INFO)  << "Finalize successful" << endreq;
    return StatusCode::SUCCESS;
  }

  void DetAnnealingMaker::reset() 
  {
    m_PtrToActualTemperature=0;
    m_isequilibrium=false;
    ATH_MSG_DEBUG("Annealing reset");
  }

  void DetAnnealingMaker::anneal() {

//check if there are some temperatures
    if (m_SetOfTemperatures.size()==0) 
    {
      msg(MSG::ERROR) << "The vector SetOfTemperatures for the annealing is empty" << std::endl;
    }
    //go one step further
    if (m_PtrToActualTemperature<m_SetOfTemperatures.size()-1)
    {
      m_PtrToActualTemperature++;
    }
    else
    {
      m_isequilibrium=true;
    }
  }

  double DetAnnealingMaker::getWeight(double chisq,const std::vector<double>& allchisq) const 
  {
    double allweight(0.);
    std::vector<double>::const_iterator begin=allchisq.begin();
    std::vector<double>::const_iterator end=allchisq.end();
    for (std::vector<double>::const_iterator iter=begin;iter!=end;++iter) 
    {
      allweight+=gauss(*iter,actualTemp());
    }
    double actualweight(gauss(chisq,actualTemp()));
    return actualweight/(gauss(m_cutoff,actualTemp())+allweight);
  }

  double DetAnnealingMaker::getWeight(double chisq) const {
    return gauss(chisq,actualTemp())/(gauss(m_cutoff,actualTemp())+gauss(chisq,actualTemp()));
  }

}

	
