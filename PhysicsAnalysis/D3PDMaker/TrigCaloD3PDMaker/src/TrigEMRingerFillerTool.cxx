/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  D3PD::TrigEMRingerFillerTool.cxx
*/
#include "TrigEMRingerFillerTool.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "AthenaKernel/errorcheck.h"
#include <iostream>


namespace D3PD {


  TrigEMRingerFillerTool::TrigEMRingerFillerTool(const std::string& type, 
						   const std::string& name, 
						   const IInterface* parent) : 
    BlockFillerTool<TrigEMCluster>(type, name, parent),
    m_incSvc ("IncidentSvc", name),
    m_sgKeyName("HLT_TrigRingerNeuralFex")
  {
    m_WriteBasics = true;
    m_WriteHypoVars = true;
    book().ignore(); // avoid coverity warnings.

    declareProperty("WriteBasics", m_WriteBasics=false, "Write basic information");
    declareProperty("WriteHypoVars", m_WriteHypoVars=false, "Write variables used in hypo");
    declareProperty("WriteDetails", m_WriteDetails=false, "Write detailed variables");
    declareProperty ("IncidentService", m_incSvc, "Gaudi incident service.");
  }


  StatusCode TrigEMRingerFillerTool::initialize()
  {
    //We add a listener that will monitor when the store gate gets clear (at the end of each event)
    CHECK( m_incSvc.retrieve() ); 
    m_incSvc->addListener (this, "StoreCleared");
 
    //Initializing SG service for retrieving the NNoutput from it.    
    StatusCode sc = service("StoreGateSvc", m_pStoreGate);
    if (sc.isFailure()) return sc;
    
    return StatusCode::SUCCESS;
  }


  void TrigEMRingerFillerTool::handle (const Incident& inc)
  {
    //Reseting the cluster counter at the beginning of a new event.
    if (inc.type() == "StoreCleared") m_emClusCount = 0;
  }

  
  StatusCode TrigEMRingerFillerTool::book()
  {
    if (m_WriteBasics)
    {
      CHECK(addVariable("emClusIdx", m_emClusIdx));
      CHECK(addVariable("rings", m_rings));
    }
    
    if (m_WriteHypoVars) CHECK(addVariable("nnOutput", m_nnOut));

    m_emClusCount = 0;
 
    return StatusCode::SUCCESS;
  }


  StatusCode TrigEMRingerFillerTool::fill(const TrigEMCluster& x)
  {
    //If the EM cluster has a ring structure attached...
    if (x.rings())
    {
      if (m_WriteBasics)
      {
        *m_emClusIdx = m_emClusCount;
        const std::vector<float> rings = x.rings()->rings();
        m_rings->assign(rings.begin(), rings.end());
      }
      
      //We take from SG the NNOutputContainer, we we look for the NNOutput attached to the
      //same TrigEmCluster passed as a parameter to this method.
      if ( (m_WriteHypoVars) && (m_pStoreGate->contains<TrigRNNOutputContainer>(m_sgKeyName)) )
      {
        m_pStoreGate->retrieve(m_sgNNOut, m_sgKeyName);
        
        //We iterate over all NN output stored in SG, looking for the one attached to the EM Cluster apssed to this member.
        for(TrigRNNOutputContainer::const_iterator it = m_sgNNOut->begin(); it != m_sgNNOut->end(); ++it)
        { 
          const TrigEMCluster *emCluster = (*it)->cluster();
          if ((*emCluster) == x) // We save the NN output when we find a match.
          {
            const std::vector<float> nnOut = (*it)->output();
            m_nnOut->assign(nnOut.begin(), nnOut.end());
            break;
          }
        }
      }
    }
    else if (m_WriteBasics) *m_emClusIdx = -1; // Assigning -1 if a ring was not found within this cluster.
    
    m_emClusCount++;
    return StatusCode::SUCCESS;
  }
}
