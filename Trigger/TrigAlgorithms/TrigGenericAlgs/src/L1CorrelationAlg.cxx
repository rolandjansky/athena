/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "L1CorrelationAlg.h"
#include "CTPfragment/CTPfragment.h"
#include "CTPfragment/CTPdataformat.h"

#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/WriteDecorHandle.h"

L1CorrelationAlg::L1CorrelationAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode L1CorrelationAlg::initialize() {

  ATH_CHECK(m_robDataProviderSvc.retrieve());
  ATH_CHECK(m_trigCompositeKey.initialize());
  ATH_CHECK(m_l1MenuKey.initialize());
  ATH_CHECK(m_passKey.initialize());
  ATH_CHECK(m_l1AKey.initialize());
  ATH_CHECK(m_otherTypeKey.initialize());
  ATH_CHECK(m_beforeAfterKey.initialize());
  ATH_CHECK(m_monTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode L1CorrelationAlg::start(){

  //get L1 menu and initialise bitMasks based on l1 em/j/mu triggers
  SG::ReadHandle<TrigConf::L1Menu> rh_l1Menu = SG::makeHandle(m_l1MenuKey);
  ATH_CHECK(rh_l1Menu.isValid());
  std::vector<int> ctpids_ele;
  std::vector<int> ctpids_mu;
  std::vector<int> ctpids_jets;
  std::vector<int> ctpids;
  for(const TrigConf::L1Item& item : *rh_l1Menu){
    for(uint nl1=0; nl1<m_l1itemlist.size(); nl1++){
      if(m_l1itemlist[nl1].compare(item.name()) == 0 ){
	ATH_MSG_DEBUG("L1CorrAlgInit: Configured to use item:" <<item.name().c_str()<< " CTPID:"<<item.ctpId());
	 ctpids.push_back( item.ctpId() );
	 if( m_l1itemlist[nl1].find( "L1_EM" )  != std::string::npos ){
	   ctpids_ele.push_back( item.ctpId() );
	 }
	 if( m_l1itemlist[nl1].find( "L1_MU" )  != std::string::npos ){
	   ctpids_mu.push_back( item.ctpId() );
	 }
	 if( m_l1itemlist[nl1].find( "L1_J" )  != std::string::npos ){
	   ctpids_jets.push_back( item.ctpId() );
	 }
      }
    }
  }
    
  // init with 0
  m_bitmasks.clear();
  m_bitmasks_ele.clear();
  m_bitmasks_mu.clear();
  m_bitmasks_jets.clear();
  for(int n=0; n<16 ; n++){
    uint32_t tmpword=0;
    m_bitmasks.push_back(tmpword);
    m_bitmasks_ele.push_back(tmpword);
    m_bitmasks_mu.push_back(tmpword);
    m_bitmasks_jets.push_back(tmpword);
  }

  //all ctpids
  for(unsigned int n=0; n<ctpids.size()  ; n++){
    // check in which word it belongs
    int cycle =  ctpids[n] / 32;
    int pos = ctpids[n] % 32;
     
    uint32_t currentmask = m_bitmasks[cycle];
    uint32_t tmpmask = 1;
    // shift this pos positions to left
    tmpmask = tmpmask << (pos);
      
    // OR this with the existing mask
    m_bitmasks[cycle] = tmpmask | currentmask;
  }

  //electrons
  for(unsigned int n=0; n<ctpids_ele.size()  ; n++){
    // check in which word it belongs
    int cycle =  ctpids_ele[n] / 32;
    int pos = ctpids_ele[n] % 32;
     
    uint32_t currentmask = m_bitmasks_ele[cycle];
    uint32_t tmpmask = 1;
    // shift this pos positions to left
    tmpmask = tmpmask << (pos);
     
    // OR this with the existing mask
    m_bitmasks_ele[cycle] = tmpmask | currentmask;
  }

  //muons
  for(unsigned int n=0; n<ctpids_mu.size()  ; n++){
    // check in which word it belongs
    int cycle =  ctpids_mu[n] / 32;
    int pos = ctpids_mu[n] % 32;
    
    uint32_t currentmask = m_bitmasks_mu[cycle];
    uint32_t tmpmask = 1;
    // shift this pos positions to left
    tmpmask = tmpmask << (pos);
    
    // OR this with the existing mask
    m_bitmasks_mu[cycle] = tmpmask | currentmask;
  }  

  //jets
  for(unsigned int n=0; n<ctpids_jets.size()  ; n++){
    // check in which word it belongs
    int cycle =  ctpids_jets[n] / 32;
    int pos = ctpids_jets[n] % 32;
    
    uint32_t currentmask = m_bitmasks_jets[cycle];
    uint32_t tmpmask = 1;
    // shift this pos positions to left
    tmpmask = tmpmask << (pos);
     
    // OR this with the existing mask
    m_bitmasks_jets[cycle] = tmpmask | currentmask;
  }  

  return StatusCode::SUCCESS;
}


StatusCode L1CorrelationAlg::execute(const EventContext& ctx) const {


  //TrigComposite Container to record
  SG::WriteHandle<xAOD::TrigCompositeContainer> wh_trigComposite(m_trigCompositeKey, ctx);
  ATH_CHECK(wh_trigComposite.record(std::make_unique<xAOD::TrigCompositeContainer>(), std::make_unique<xAOD::TrigCompositeAuxContainer>()));
  auto trigCompCont = wh_trigComposite.ptr();

  SG::WriteDecorHandle<xAOD::TrigCompositeContainer, int> trigCompL1A(m_l1AKey, ctx);
  SG::WriteDecorHandle<xAOD::TrigCompositeContainer, int> trigCompOther(m_otherTypeKey, ctx);
  SG::WriteDecorHandle<xAOD::TrigCompositeContainer, int> trigCompBeforeAfter(m_beforeAfterKey, ctx);
  SG::WriteDecorHandle<xAOD::TrigCompositeContainer, bool> trigCompPass(m_passKey, ctx);

  //CTP ROB
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> robFragments;
  std::vector<uint32_t> roblist;
  // magic number!
  roblist.push_back(0x770000); 
  m_robDataProviderSvc->addROBData(roblist);
  m_robDataProviderSvc->getROBData(roblist, robFragments);
  if (msgLvl(MSG::DEBUG)) {
    std::ostringstream os;
    for(auto rob : roblist){
      os << std::hex<<rob;
    }
    ATH_MSG_DEBUG(roblist.size() << "/" << robFragments.size()
		 << " ROBs requested/retrieved:" << os.str());
  }
  if (robFragments.size()<1){
    ATH_MSG_DEBUG("Could not retrieve ROB!");
    return StatusCode::SUCCESS;
  }
  const eformat::ROBFragment<const uint32_t*>* rbf = robFragments[0];

  // this is the l1a accept position
  uint32_t l1a = CTPfragment::lvl1AcceptBunch(rbf);

  std::vector<uint32_t> tbp;
  bool firedbc[3];
  firedbc[0]=0;
  firedbc[1]=0;
  firedbc[2]=0;

  bool firedbc_ele[3];
  firedbc_ele[0]=0;
  firedbc_ele[1]=0;
  firedbc_ele[2]=0;
  bool firedbc_mu[3];
  firedbc_mu[0]=0;
  firedbc_mu[1]=0;
  firedbc_mu[2]=0;
  bool firedbc_jet[3];
  firedbc_jet[0]=0;
  firedbc_jet[1]=0;
  firedbc_jet[2]=0;  

  // if currentBCincl flag is toggled don't ask for any specific trigger in the central BCID
  if(m_currentBCincl){
    firedbc[1]=1;
    firedbc_ele[1]=1;
    firedbc_mu[1]=1;
    firedbc_jet[1]=1;
  }

  //check BC-1 to BC+1
  unsigned int ibc=0;
  for(unsigned int bc=l1a-1; bc<l1a+2; bc++) {
    tbp = CTPfragment::triggerDecisionBeforePrescales(rbf,bc);
    unsigned ntbpwords(tbp.size());
    for(unsigned iw=0; iw<ntbpwords; iw++  ) {
      uint32_t tmpand = tbp[iw] & m_bitmasks[iw];
      uint32_t tmpand_ele = tbp[iw] & m_bitmasks_ele[iw];
      uint32_t tmpand_mu = tbp[iw] & m_bitmasks_mu[iw];
      uint32_t tmpand_jet = tbp[iw] & m_bitmasks_jets[iw];
      firedbc[ibc] = firedbc[ibc] || (tmpand > 0);
      firedbc_ele[ibc] = firedbc_ele[ibc] || (tmpand_ele > 0);
      firedbc_mu[ibc] = firedbc_mu[ibc] || (tmpand_mu > 0);
      firedbc_jet[ibc] = firedbc_jet[ibc] || (tmpand_jet > 0);
    }
    ibc++;
  }

  int l1a_type = 0;
  int beforeafterflag=0;
  int other_type=0;
  bool isPassed = false;
  if ((firedbc[0] && firedbc[1]) || (firedbc[1] && firedbc[2])){

    ATH_MSG_DEBUG("Event fired bc[0]: "<<firedbc[0]<<", bc[1]: "<<firedbc[1]<<", bc[2]: "<<firedbc[2]);
    isPassed = true;

    if(firedbc_ele[1])  l1a_type = 1;
    if(firedbc_mu[1])  l1a_type = 2;
    if(firedbc_jet[1])  l1a_type = 3;
    if(firedbc_ele[1] && firedbc_mu[1])  l1a_type = 4;
    if(firedbc_ele[1] && firedbc_jet[1])  l1a_type = 5;
    if(firedbc_mu[1] && firedbc_jet[1])  l1a_type = 6;
    if(firedbc_ele[1] && firedbc_mu[1] && firedbc_jet[1])  l1a_type = 7;
      
    if((firedbc[0] && firedbc[1])){
      beforeafterflag = -1;
 
      if(firedbc_ele[0])  other_type = 1;
      if(firedbc_mu[0])  other_type = 2;
      if(firedbc_jet[0])  other_type = 3;
      if(firedbc_ele[0] && firedbc_mu[0])  other_type = 4;
      if(firedbc_ele[0] && firedbc_jet[0])  other_type = 5;
      if(firedbc_mu[0] && firedbc_jet[0])  other_type = 6;
      if(firedbc_ele[0] && firedbc_mu[0] && firedbc_jet[0])  other_type = 7;
      
    }
    else if((firedbc[1] && firedbc[2])){
      beforeafterflag = 1;
      if(firedbc_ele[2])  other_type = 1;
      if(firedbc_mu[2])  other_type = 2;
      if(firedbc_jet[2])  other_type = 3;
      if(firedbc_ele[2] && firedbc_mu[2])  other_type = 4;
      if(firedbc_ele[2] && firedbc_jet[2])  other_type = 5;
      if(firedbc_mu[2] && firedbc_jet[2])  other_type = 6;
      if(firedbc_ele[2] && firedbc_mu[2] && firedbc_jet[2])  other_type = 7;
    
    }
    if ((firedbc[0] && firedbc[1]) && (firedbc[1] && firedbc[2])){
      beforeafterflag = 2;
     
    }

    auto trigComp = new xAOD::TrigComposite();
    trigCompCont->push_back(trigComp);
    trigComp->setName("mistimemon_L1Dec");
    trigCompL1A(*trigComp)  = l1a_type;
    trigCompOther(*trigComp) = other_type;
    trigCompBeforeAfter(*trigComp) = beforeafterflag;
    trigCompPass(*trigComp) = isPassed;
  }

  //setup monitoring
  auto mon_l1a = Monitored::Scalar<int>("l1Accept", l1a_type);
  auto mon_otherType = Monitored::Scalar<int>("otherType", other_type);
  auto mon_beforeAfter = Monitored::Scalar<int>("BeforeAfterFlag", beforeafterflag);
  auto monitorIt = Monitored::Group(m_monTool, mon_l1a, mon_otherType, mon_beforeAfter);

  return StatusCode::SUCCESS;

}
