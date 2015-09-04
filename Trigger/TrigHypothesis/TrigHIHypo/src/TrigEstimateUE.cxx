/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloEvent/CaloCellContainer.h"
#include "TrigSteeringEvent/Enums.h"

#include "TrigEstimateUE.h"

using namespace HLT;
TrigEstimateUE::TrigEstimateUE::TrigEstimateUE(const std::string& name, ISvcLocator* pSvcLocator)
  :AllTEAlgo(name, pSvcLocator),
   m_hasRun(false),
   m_cells(nullptr),
   m_EtaMin(-5),
   m_EtaMax(5),
   m_nEtaSlices(10)
{
  declareProperty("NEtaSlices",m_nEtaSlices,"Number of slices in which UE is estimated");
  declareProperty("EtaMin",m_EtaMin,"minimum eta");
  declareProperty("EtaMax",m_EtaMin,"maximum eta");
}

TrigEstimateUE::~TrigEstimateUE() {}

HLT::ErrorCode TrigEstimateUE::hltInitialize(){
  ATH_MSG_DEBUG("hltInitialize" << name());
  m_avg.setSize(m_nEtaSlices);
  m_avg.setEtaBins(m_EtaMin,m_EtaMax,m_nEtaSlices);
  return HLT::OK;
}

HLT::ErrorCode TrigEstimateUE::hltFinalize(){
  ATH_MSG_DEBUG("hltFinalize" << name() );
  return HLT::OK;
}

HLT::ErrorCode TrigEstimateUE::hltExecute(VecVecTE& tes_in, unsigned int type_out){
  {
    HLT::ErrorCode ec;
    if(( ec = verifyInput(tes_in)) != HLT::OK )
      return ec;
  }
  if( m_hasRun ){
    std::vector<double> collectedAvg;
    m_avg.getAverages(collectedAvg);
    for( auto el: collectedAvg){
      ATH_MSG_DEBUG("cell avg: " << el);
    }
    return HLT::OK;
  }
  m_hasRun=true;

  m_cells = getCells(tes_in);
  if( m_cells == nullptr){
    ATH_MSG_INFO("hltExecute:" << "calo cells can not be retrived" );
  }
  for( auto cellPtr: *m_cells ){
    m_avg.fillSlice(cellPtr->energy(), cellPtr->eta());
  }
  std::vector<double> collectedAvg;
  m_avg.getAverages(collectedAvg);
  for( auto el: collectedAvg){
    ATH_MSG_DEBUG("cell avg: " << el );
  }
  /*TriggerElement* out_te =*/ config()->getNavigation()->addNode(tes_in[0][0], type_out);

  return HLT::OK;
}

HLT::ErrorCode TrigEstimateUE::hltRest(){
  m_hasRun = false;
  return HLT::OK;
}
HLT::ErrorCode TrigEstimateUE::verifyInput(VecVecTE& vec){
  if( vec.size() != 1)
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  if( vec[0].size() !=1 )
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);

  return HLT::OK;
}

const CaloCellContainer* TrigEstimateUE::getCells(VecVecTE& vec){
  const HLT::TriggerElement* te = vec[0][0];
  const CaloCellContainer* ccc(0);
  if( getFeature(te, ccc) == HLT::OK)
    return ccc;

  return nullptr;
}

void TrigEstimateUE::Averages::setSize(size_t s){
  m_etSum.resize(s);
  m_nCells.resize(s);
  m_dEta.resize(s);

}


void TrigEstimateUE::Averages::fillSlice(double energy, double eta){
  typedef std::vector<double>::iterator etIter;
  etIter etIt = m_etSum.begin();
  typedef std::vector<etaBin>::iterator etaIter;
  etaIter etaIt= m_dEta.begin();
  typedef std::vector<double>::iterator cIter;
  cIter nIt= m_nCells.begin();


  for( ; nIt != m_nCells.end(); ++nIt, ++etIt, ++etaIt){
    if(eta > (*etaIt).etaMin && eta < (*etaIt).etaMax ){
      (*etIt) +=energy;
      (*nIt) += 1;
    }
  }

}
void TrigEstimateUE::Averages::setEtaBins(double etamin, double etamax, size_t s){
  typedef std::vector<etaBin>::iterator iter;
  iter aIter = m_dEta.begin();
  etaBin tmp;
  tmp.etaMin = etamin;
  double dEta = fabs(etamin - etamax)/s;
  for( ; aIter != m_dEta.end() && tmp.etaMin != etamax; ++aIter){
    tmp.etaMax = tmp.etaMin + dEta;
    (*aIter) = tmp;
    tmp.etaMin = tmp.etaMax;
  }
}
void TrigEstimateUE::Averages::getAverages(std::vector<double>& avg) const{
  avg = m_etSum;
  typedef std::vector<double>::iterator iter;
  iter aIter = avg.begin();
  typedef std::vector<double>::const_iterator citer;
  citer nIter = m_nCells.begin();

  for( ; nIter != m_nCells.end() ; ++nIter, ++aIter){
    if( *nIter != 0.0)
      (*aIter) /= (*nIter);
  }

}


void TrigEstimateUE::Averages::reset(){
  m_etSum.assign( m_etSum.size(), 0.);
  m_nCells.assign(m_nCells.size(), 0.);

}
