/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/ClusterFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for a four-momentum.
 */


#include "ClusterFillerTool.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloSamplingHelper.h"
#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"
#include "AthenaKernel/errorcheck.h"
#include "CaloEvent/CaloSamplingHelper.h"
#include <typeinfo>


namespace D3PD {


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ClusterFillerTool::ClusterFillerTool (const std::string& type,
					const std::string& name,
					const IInterface* parent)
    : BlockFillerTool<CaloCluster> (type, name, parent)
  {
    declareProperty ("SignalState",   m_signalState = P4SignalState::UNKNOWN,
		     "Signal state of the object to read.  "
		     "Use the default of UNKNOWN to not change the state.");
    declareProperty ("UseCaloSamplings", m_useCaloSamplings,
		     "Calorimeter sampling layers with which to calculate isolation.");

    book().ignore(); // Avoid coverity warnings
  }


  /**
   * @brief Book variables for this block.
   */
  StatusCode ClusterFillerTool::book()
  {
    CHECK( addVariable ("E",     m_E)  );
    CHECK( addVariable ("Et",    m_Et)  );

    CHECK( addVariable ("eta",   m_eta) );
    CHECK( addVariable ("phi",   m_phi) );

    // check to make sure the calorimeter samplings we've been
    // given are valid
    CaloSamplingHelper::getSamplingList(m_caloSamplings);
    for(std::vector<int>::iterator i=m_useCaloSamplings.begin(); 
	i != m_useCaloSamplings.end(); ){
      if(std::find(m_caloSamplings.begin(), m_caloSamplings.end(), 
		   (CaloSampling::CaloSample)*i) == m_caloSamplings.end()){
	REPORT_MESSAGE (MSG::WARNING)
	  << "Requested calorimeter sampling " << (int)(*i)
	  << " which is not a valid sampling number.  Ignoring it.";
	i = m_useCaloSamplings.erase(i);
      }
      else ++i;
    }

    return StatusCode::SUCCESS;
  }


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  StatusCode ClusterFillerTool::fill (const CaloCluster& p)
  {
    SignalStateHelper sshelper (static_cast<P4SignalState::State>(m_signalState));
    if (m_signalState != P4SignalState::UNKNOWN) {
      const ISignalState* ss = dynamic_cast<const ISignalState*> (&p);
      if (ss)
	sshelper.controlObject (ss);
      else {
	REPORT_MESSAGE (MSG::WARNING)
	  << "Requested signal state " << m_signalState
	  << " for object of type " << typeid(p).name()
	  << "; but it does not derive from ISignalState.";
      }
    }

    *m_eta   = static_cast<float> (p.eta());
    *m_phi   = static_cast<float> (p.phi());

    if(m_caloSamplings.size() == 0){
      *m_E     = static_cast<float> (p.e());
      *m_Et    = static_cast<float> (p.et());
    }
    else{
      float e=0., et=0.;
      
      for(unsigned int sampling=0; sampling<m_caloSamplings.size(); sampling++){
	float st = 1./cosh(*m_eta);
	e  += p.eSample(m_caloSamplings[sampling]);
	et += p.eSample(m_caloSamplings[sampling])*st;
      }

      *m_E = e;
      *m_Et = et;
    }

    return StatusCode::SUCCESS;
  }


} // namespace D3PD
