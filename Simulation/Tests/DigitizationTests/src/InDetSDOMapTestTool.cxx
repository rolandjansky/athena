#include "InDetSDOMapTestTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <boost/io/ios_state.hpp>
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include <TH2D.h>
#include "AthenaKernel/errorcheck.h"

InDetSDOMapTestTool::InDetSDOMapTestTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
  : DigiTestToolBase(type, name, parent)
  , m_numberOfEventsSelected(0)
  , m_StopTreatment(0)
  , m_hHMPLcollection_all(0)
  , m_hHMPLcollection_valid(0)
  , m_hHMPLcollection_ratio(0)
  , m_hHMPLbarcodes_all(0)
  , m_hHMPLbarcodes_valid(0)
  , m_hHMPLbarcodes_ratio(0)
{
  declareProperty("SDO_Map_Name", m_SDO_MapName="SCT_SDO_Map");
  declareProperty("OutputAdditionalPath", m_addPath="InDet/SCT/SCT_SDO_Map");
}

StatusCode InDetSDOMapTestTool::initialize()
{
  m_path+= m_addPath + "/";

  _TH1D(m_hHMPLcollection_all, "HMPLcollection_all",4,-0.5,3.5);
  _SET_TITLE(m_hHMPLcollection_all, "McEventCollection associated to all hits through HMPL","McEventCollection: TruthEvent(0), TruthEvent_PU(1), TruthEvent_HighPtPU(2), TruthEvent_Cavern(3)", "Number of hits");
  _TH1D(m_hHMPLcollection_valid, "HMPLcollection_valid",4,-0.5,3.5);
  _SET_TITLE(m_hHMPLcollection_valid, "McEventCollection associated to hits through valid HMPL","McEventCollection: TruthEvent(0), TruthEvent_PU(1), TruthEvent_HighPtPU(2), TruthEvent_Cavern(3)", "Number of hits");
  _TH1D(m_hHMPLcollection_ratio, "HMPLcollection_ratio",4,-0.5,3.5);
  _SET_TITLE(m_hHMPLcollection_ratio, "Fraction of valid HMPL associated to hits by McEventCollection","McEventCollection: TruthEvent(0), TruthEvent_PU(1), TruthEvent_HighPtPU(2), TruthEvent_Cavern(3)", "Fraction of valid HMPL");
  _TH1D(m_hHMPLbarcodes_all, "HMPLbarcodes_all",3,-0.5,2.5);
  _SET_TITLE(m_hHMPLbarcodes_all, "Barcodes associated to all hits through HMPL","Barcodes: 0, 1-200000, 200001-", "Number of hits");
  _TH1D(m_hHMPLbarcodes_valid, "HMPLbarcodes_valid",3,-0.5,2.5);
  _SET_TITLE(m_hHMPLbarcodes_valid, "Barcodes associated to hits through valid HMPL","Barcodes: 0, 1-200000, 200001-", "Number of hits");
  _TH1D(m_hHMPLbarcodes_ratio, "HMPLbarcodes_ratio",3,-0.5,2.5);
  _SET_TITLE(m_hHMPLbarcodes_ratio, "Fraction of valid HMPL associated to hits by barcode","Barcodes: 0, 1-200000, 200001-", "Fraction of valid HMPL");


  return StatusCode::SUCCESS;
}

StatusCode InDetSDOMapTestTool::processEvent() {
  if (m_StopTreatment)
    return StatusCode::SUCCESS;

  ATH_CHECK(this->CheckSDOs());
  ++m_numberOfEventsSelected;

  return StatusCode::SUCCESS;
}

StatusCode InDetSDOMapTestTool::CheckSDOs() {
  boost::io::ios_all_saver ias( std::cout );

  const InDetSimDataCollection* simDataMap(nullptr);
  if(evtStore()->retrieve(simDataMap,m_SDO_MapName.c_str()).isFailure()) {
    //If this happens on the first event we deregister the histograms
    if(m_hHMPLcollection_all) {
      ATH_MSG_INFO ("Could not find SDO container called " << m_SDO_MapName);
      m_StopTreatment=true;
    }
    return StatusCode::SUCCESS;
  }
  InDetSimDataCollection::const_iterator itSDO(simDataMap->begin());
  const InDetSimDataCollection::const_iterator endOfSDOs(simDataMap->end());
  for(; itSDO!=endOfSDOs; ++itSDO) {
    const InDetSimData& sdo((*itSDO).second);
    // Loop over deposits for this SDO
    const std::vector< std::pair<HepMcParticleLink,float> >& deposits = sdo.getdeposits();
    std::vector< std::pair<HepMcParticleLink,float> >::const_iterator itDep(deposits.begin());
    const std::vector< std::pair<HepMcParticleLink,float> >::const_iterator endOfDeposits(deposits.end());
    for( ; itDep!=endOfDeposits; ++itDep)      {
      const HepMcParticleLink& particleLink = (*itDep).first;
      //NEEDS to be different for releases pre-HMPL-migration
      //In MIG13/post-HMPL-migration:
      unsigned int evColl = particleLink.getEventCollection();
      //unsigned int evColl=0;//In pre-HMPL-migration releases such as 20.20
      int bc = particleLink.barcode();
      unsigned int bcRange = (bc) ? ((bc<200000) ? 1 : 2) : 0;
      m_hHMPLcollection_all->Fill((unsigned int)evColl);
      m_hHMPLbarcodes_all->Fill(bcRange);
      if (particleLink.cptr()) {
        m_hHMPLcollection_valid->Fill((unsigned int)evColl);
        m_hHMPLbarcodes_valid->Fill(bcRange);
      }
    }
  }//end of loop over SDO map

  return StatusCode::SUCCESS;
}


StatusCode InDetSDOMapTestTool::finalize(){

  if (!m_StopTreatment) {
    for (int ibin=0; ibin<=(m_hHMPLcollection_all->GetNbinsX()+1); ibin++) {
      if (m_hHMPLcollection_all->GetBinContent(ibin))
        m_hHMPLcollection_ratio->SetBinContent(ibin,m_hHMPLcollection_valid->GetBinContent(ibin)/m_hHMPLcollection_all->GetBinContent(ibin));
      else
        m_hHMPLcollection_ratio->SetBinContent(ibin,0.);
    }
    for (int ibin=0; ibin<=(m_hHMPLbarcodes_all->GetNbinsX()+1); ibin++) {
      if (m_hHMPLbarcodes_all->GetBinContent(ibin))
        m_hHMPLbarcodes_ratio->SetBinContent(ibin,m_hHMPLbarcodes_valid->GetBinContent(ibin)/m_hHMPLbarcodes_all->GetBinContent(ibin));
      else
        m_hHMPLbarcodes_ratio->SetBinContent(ibin,0.);
    }
  }

  return StatusCode::SUCCESS;
}
