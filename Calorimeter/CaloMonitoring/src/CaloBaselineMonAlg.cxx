/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloBaselineMonAlg.h"

#include "AthenaMonitoring/DQBadLBFilterTool.h"
#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "AthenaKernel/Units.h"


using xAOD::CaloCluster;
using Athena::Units::GeV;

CaloBaselineMonAlg::CaloBaselineMonAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  CaloMonAlgBase(name, pSvcLocator),
  m_bool_pedestalMon(false),
  m_bool_bcidtoolMon(false),
  m_calo_id(nullptr)
{

}


StatusCode CaloBaselineMonAlg::initialize() {


  ATH_CHECK( detStore()->retrieve(m_calo_id) );

  ATH_CHECK( m_cellContainerKey.initialize() );

  StatusCode sc = m_bunchCrossingTool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Unable to retrieve bunchCrossingTool. Switch off CaloBaselineMon" );
    m_pedestalMon_BCIDmin = -1;
    m_bcidtoolMon_BCIDmax = -1; 
  }

  if (m_pedestalMon_BCIDmin > 0) m_bool_pedestalMon = true;
  else m_bool_pedestalMon = false;

  if (m_bcidtoolMon_BCIDmax > 0) m_bool_bcidtoolMon = true;
  else m_bool_bcidtoolMon = false;

  m_etaBinWidth.resize(m_partNames.size());
  m_inv_etaBinWidth.resize(m_partNames.size());

  m_partMap.resize(3);
  m_partMap = {-1,-1,-1};
  // 0:EM / 1:HEC / FCal:2
  for (uint iPart = 0;iPart <  m_partNames.size();iPart++){
    if (m_partNames[iPart] == "EM") m_partMap[0] = iPart;
    if (m_partNames[iPart] == "HEC") m_partMap[1] = iPart;
    if (m_partNames[iPart] == "FCal") m_partMap[2] = iPart;
    if (m_partNames[iPart] == "HEC+FCal"){
	m_partMap[1] = iPart;
	m_partMap[2] = iPart;
      }  
    m_etaBinWidth[iPart] = (m_etaMax[iPart]-m_etaMin[iPart])/((float) m_nbOfEtaBins[iPart]);
    m_inv_etaBinWidth[iPart] = ((float) m_nbOfEtaBins[iPart])/(m_etaMax[iPart]-m_etaMin[iPart]);
  }

  m_histoGroups.reserve(m_partNames.size());
  for (unsigned i=0; i<m_partNames.size(); ++i) {
     m_histoGroups.push_back( Monitored::buildToolMap<int>(m_tools,m_partNames[i], m_nbOfEtaBins[i]));
  }

  return CaloMonAlgBase::initialize();
}

////////////////////////////////////////////////////////////////////////////
StatusCode CaloBaselineMonAlg::fillHistograms(const EventContext& ctx) const {
  
  StatusCode sc = StatusCode::SUCCESS;

  bool ifPass = 1;
  bool passBeamBackgroundRemoval = 0;
  sc = checkFilters(ifPass, passBeamBackgroundRemoval,m_MonGroupName, ctx);
  if(sc.isFailure() || !ifPass) return StatusCode::SUCCESS;
  
  ATH_MSG_DEBUG("checkFilters() passed"); 

  int lumiBlock = ctx.eventID().lumi_block();
  int bcid = ctx.eventID().bunch_crossing_id();

  bool thisEvent_bool_pedestalMon = false;
  bool thisEvent_bool_bcidtoolMon = false;

  // Fill pedestalMon only when the bunch is empty and far away enough from the last train.
  if (m_bool_pedestalMon){
    if ((not m_bunchCrossingTool->isFilled(bcid)) and (m_bunchCrossingTool->gapAfterBunch(bcid) >= m_pedestalMon_BCIDmin*25.) and (m_bunchCrossingTool->gapBeforeBunch(bcid) >= m_pedestalMon_BCIDmin*25.)) thisEvent_bool_pedestalMon = true;
  }
  ATH_MSG_DEBUG("m_bool_pedestalMon passed"); 
  // Fill bcidtoolMon only when the bunch is in a bunch train and within accepted BCID range.
  if (m_bool_bcidtoolMon){
    if (m_bunchCrossingTool->isInTrain(bcid) and m_bunchCrossingTool->distanceFromFront(bcid) <= m_bcidtoolMon_BCIDmax*25.) thisEvent_bool_bcidtoolMon = true;
  }
  ATH_MSG_DEBUG("m_bool_bcidtoolMon passed"); 

  ATH_MSG_DEBUG(thisEvent_bool_pedestalMon<<" "<<thisEvent_bool_bcidtoolMon);

  if (not (thisEvent_bool_pedestalMon or thisEvent_bool_bcidtoolMon)) return sc;

  SG::ReadHandle<CaloCellContainer> cellCont{m_cellContainerKey,ctx};

  if(!cellCont.isValid() ) {
    ATH_MSG_WARNING("No CaloCell container found in TDS"); 
    return sc;
  }  

  // Initialize the sum per eta band for each partition
  std::vector<std::vector<float>> sum_partition_eta;
  sum_partition_eta.resize(m_partNames.size());
  for (uint iPart = 0;iPart <  m_partNames.size();iPart++){
    sum_partition_eta[iPart].resize(m_nbOfEtaBins[iPart]);
  }

  for (uint iPart = 0;iPart <  m_partNames.size();iPart++){
    for (uint iEta = 0; iEta < m_nbOfEtaBins[iPart]; iEta ++){
      sum_partition_eta[iPart][iEta] = 0.;
    }
  }

  CaloCellContainer::const_iterator it = cellCont->begin(); 
  CaloCellContainer::const_iterator it_e = cellCont->end(); 

  for ( ; it!=it_e;++it) { 

    // cell info
    const CaloCell* cell = *it; 
    Identifier id = cell->ID();
    float energy = cell->energy();
    double eta = cell->caloDDE()->eta_raw();

    int partThisAlgo = 0;
    if  (m_calo_id->is_em(id)) partThisAlgo = m_partMap[0];
    if  (m_calo_id->is_hec(id)) partThisAlgo = m_partMap[1]; 
    if  (m_calo_id->is_fcal(id)) partThisAlgo = m_partMap[2]; 

   
    if ((eta<m_etaMin[partThisAlgo]) or (eta>m_etaMax[partThisAlgo])) continue;
    int etaBin = std::floor((eta-m_etaMin[partThisAlgo])*m_inv_etaBinWidth[partThisAlgo]);

    sum_partition_eta[partThisAlgo][etaBin] += energy;
  } // cell iter loop


  // Loop on cells is over. Now fill histograms with sum per eta.
  auto bcidfill = Monitored::Scalar<int>("BCID",bcid);
  fill(m_MonGroupName,bcidfill);
  if (thisEvent_bool_pedestalMon ) {
    for (uint iPart = 0;iPart <  m_partNames.size();iPart++){ 
      auto sum = Monitored::Scalar<float>("sumPedEta_"+m_partNames[iPart],-1);
      auto etaToBeFilled = Monitored::Scalar<float>("pedEta_"+m_partNames[iPart],-1);
      auto lb = Monitored::Scalar<int>("LB_"+m_partNames[iPart],lumiBlock);
      auto bcideta = Monitored::Scalar<int>("etaBCID_"+m_partNames[iPart],bcid);
      for (uint iEta = 0; iEta < m_nbOfEtaBins[iPart]; iEta ++){
	// Normalize the sum by the \delta\eta.\delta\phi.\mu
	sum = sum_partition_eta[iPart][iEta]*(m_inv_etaBinWidth[iPart])/(2*M_PI)/lbAverageInteractionsPerCrossing(ctx);
	etaToBeFilled = ((float) iEta)*m_etaBinWidth[iPart] + m_etaMin[iPart];
        fill(m_MonGroupName,lb,etaToBeFilled,sum);
        fill(m_tools[m_histoGroups.at(iPart).at(iEta)],bcideta,sum);
      }
    }
  }

  if (thisEvent_bool_bcidtoolMon ) {
    for (uint iPart = 0;iPart <  m_partNames.size();iPart++){ 
      auto sum = Monitored::Scalar<float>("sumBCIDEta_"+m_partNames[iPart],-1);
      auto etaToBeFilled = Monitored::Scalar<float>("bcidEta_"+m_partNames[iPart],-1);
      auto lb = Monitored::Scalar<int>("LB_"+m_partNames[iPart],lumiBlock);
      auto bcideta = Monitored::Scalar<int>("etaBCID_"+m_partNames[iPart],bcid);
      for (uint iEta = 0; iEta < m_nbOfEtaBins[iPart]; iEta ++){
	// Normalize the sum by the \delta\eta.\delta\phi.\mu
	sum = sum_partition_eta[iPart][iEta]*(m_inv_etaBinWidth[iPart])/(2*M_PI)/lbAverageInteractionsPerCrossing(ctx);
	etaToBeFilled = ((float) iEta)*m_etaBinWidth[iPart] + m_etaMin[iPart];
        fill(m_MonGroupName,lb,etaToBeFilled,sum);
        fill(m_tools[m_histoGroups.at(iPart).at(iEta)],bcideta,sum);
      }
    }
  }

  return sc;
}


