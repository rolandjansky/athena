/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMbD3PDMaker/fillers/TrkCntsEfFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "TrigInDetEvent/TrigTrackCountsCollection.h"

namespace TrigMbD3PD {

TrkCntsEfFillerTool::TrkCntsEfFillerTool (const std::string& type,
					  const std::string& name,
					  const IInterface* parent)
  : TrkCntsEfFillerBase (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode TrkCntsEfFillerTool::initialize(){

  CHECK( TrkCntsEfFillerBase::initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TrkCntsEfFillerTool::book()
{

  CHECK( addVariable ("nElements", m_nElements) );

  CHECK( addVariable ("hZ0Bins", m_hZ0Bins) );
  CHECK( addVariable ("hZ0Min", m_hZ0Min) );
  CHECK( addVariable ("hZ0Max", m_hZ0Max) );

  CHECK( addVariable ("hPtBins", m_hPtBins) );
  CHECK( addVariable ("hPtMin", m_hPtMin) );
  CHECK( addVariable ("hPtMax", m_hPtMax) );

  CHECK( addVariable ("hEtaBins", m_hEtaBins) );
  CHECK( addVariable ("hEtaMin", m_hEtaMin) );
  CHECK( addVariable ("hEtaMax", m_hEtaMax) );

  CHECK( addVariable ("hPhiBins", m_hPhiBins) );
  CHECK( addVariable ("hPhiMin", m_hPhiMin) );
  CHECK( addVariable ("hPhiMax", m_hPhiMax) );

  CHECK( addVariable ("z0_pt_contents", m_z0_pt_contents) );
  CHECK( addVariable ("eta_phi_contents", m_eta_phi_contents) );

  return StatusCode::SUCCESS;
}

StatusCode TrkCntsEfFillerTool::fill (const TrkCntsEfFillerObjectType& trkCountsCollection)
{

  this->clearData();

  *m_nElements = trkCountsCollection.size();

  TrigTrackCountsCollection::const_iterator itr = trkCountsCollection.begin();
  TrigTrackCountsCollection::const_iterator itr_end = trkCountsCollection.end();
  TrigHisto2D z0_pt;
  TrigHisto2D eta_phi;

  if(itr != itr_end) {

    z0_pt = (*itr)->z0_pt();
    *m_hZ0Bins = z0_pt.nbins_x();
    *m_hZ0Min = z0_pt.min_x();
    *m_hZ0Max = z0_pt.max_x();
    *m_hPtBins = z0_pt.nbins_y();
    *m_hPtMin = z0_pt.min_y();
    *m_hPtMax = z0_pt.max_y();
    *m_z0_pt_contents = z0_pt.contents();
     
    eta_phi = (*itr)->eta_phi();
    *m_hEtaBins = eta_phi.nbins_x();
    *m_hEtaMin = eta_phi.min_x();
    *m_hEtaMax = eta_phi.max_x();
    *m_hPhiBins = eta_phi.nbins_y();
    *m_hPhiMin = eta_phi.min_y();
    *m_hPhiMax = eta_phi.max_y();
    *m_eta_phi_contents = eta_phi.contents();
  }

  return StatusCode::SUCCESS;
}

void TrkCntsEfFillerTool::clearData(){

  // Clear the ntuple variables
  *m_nElements = 0;
  *m_hZ0Bins = 0;
  *m_hZ0Min = 0;
  *m_hZ0Max = 0;
  *m_hPtBins = 0;
  *m_hPtMin = 0;
  *m_hPtMax = 0;
  *m_hEtaBins = 0;
  *m_hEtaMin = 0;
  *m_hEtaMax = 0;
  *m_hPhiBins = 0;
  *m_hPhiMin = 0;
  *m_hPhiMax = 0;
  m_z0_pt_contents->clear();
  m_eta_phi_contents->clear();
}


} // namespace TrigMbD3PD
