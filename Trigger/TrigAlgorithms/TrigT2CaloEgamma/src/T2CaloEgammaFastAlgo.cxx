/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 NAME:     T2CaloEgammaFastAlgo.cxx
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma

 AUTHOR:   Denis Oliveira Damazio
           Carlos Chavez Barajas

*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "AthLinks/ElementLink.h"


#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"

#include "TrigT2CaloEgamma/T2CaloEgammaFastAlgo.h"
#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "TrigT2CaloCalibration/IEgammaCalibration.h"
#include "TrigT2CaloCommon/ITrigDataAccess.h"

#include "TrigT2CaloCommon/phiutils.h"

class ISvcLocator;

T2CaloEgammaFastAlgo::T2CaloEgammaFastAlgo(const std::string & name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator), 
    m_log(0), 
    m_calibsBarrel(this), 
    m_calibsEndcap(this),
    m_storeCells(false),
    m_roiCollection("OutputRoIs"),
    m_trigEmClusterCollection("CaloClusters"),
    m_regionSelector("RegSelSvc", name)
{
    declareProperty("L1ForceEta",m_l1eta = -10.0);
    declareProperty("L1ForcePhi",m_l1phi = -10.0);
    declareProperty("EtaWidth", m_etaWidth = 0.1,"Eta Width of the Region of Interest");
    declareProperty("PhiWidth", m_phiWidth = 0.1,"Phi Width of the Region of Interest"); 
    declareProperty("CalibListBarrel",m_calibsBarrel,"list of calib tools for the Barrel clusters");
    declareProperty("CalibListEndcap",m_calibsEndcap,"list of calib tools for the EndCap clusters");
    declareProperty("StoreCells",m_storeCells,"store cells in container attached to RoI");
    declareProperty("RhoEta",m_rhoEta);
    declareProperty("ZEta",m_zEta);
    declareProperty("IAlgToolList", m_emAlgTools,"list of AlgToolCalo. These tools perform FEX.");
    declareProperty("RoIs", m_roiCollectionKey = std::string("OutputRoIs"), "RoIs to read in");
    declareProperty("ClustersName", m_clusterContainerKey = std::string("CaloClusters"), "Calo cluster container");
}

T2CaloEgammaFastAlgo::~T2CaloEgammaFastAlgo()
{
  delete m_log;
}


StatusCode T2CaloEgammaFastAlgo::initialize()
{
  if (!m_log) m_log = new MsgStream(msgSvc(), name());


  m_emAlgTools.retrieve().ignore();
  ATH_CHECK( m_regionSelector.retrieve());
  ATH_CHECK( m_clusterContainerKey.initialize() );
  ATH_CHECK( m_roiCollectionKey.initialize() );
  ATH_MSG_DEBUG( "Initialize done !" );
  return StatusCode::SUCCESS;
}


StatusCode T2CaloEgammaFastAlgo::execute()
{
  // Time total T2CaloEgamma execution time.
//  if ( m_timersvc ) m_timer[0]->start();
  m_conversionError=0;
  m_algorithmError=0;
  m_monitoredCluster=0;

#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) 
  (*m_log) << MSG::INFO << "in execute()" << endmsg;
#endif

  m_trigEmClusterCollection = SG::WriteHandle<xAOD::TrigEMClusterContainer>( m_clusterContainerKey, getContext() );
  ATH_CHECK( m_trigEmClusterCollection.record( CxxUtils::make_unique<xAOD::TrigEMClusterContainer>(), CxxUtils::make_unique<xAOD::TrigEMClusterAuxContainer>() ) );

  auto roisHandle = SG::makeHandle( m_roiCollectionKey );
  const TrigRoiDescriptorCollection* roiCollection = roisHandle.cptr();
  //  ATH_CHECK(m_roiCollectionKey.isValid());


  const TrigRoiDescriptor* roiDescriptor = 0;
  //TrigRoiDescriptor* roiDescriptor = 0;

  // datahandle 
  TrigRoiDescriptorCollection::const_iterator  roiCollectionIt  = roiCollection->begin(); 
  for(; roiCollectionIt != roiCollection->end(); ++roiCollectionIt){
	roiDescriptor = *roiCollectionIt;

  float etaL1, phiL1;
  // End LVL1 part
  double etamin, etamax, phimin, phimax;
  if ( (m_l1eta<-9.9)&&(m_l1phi<-9.9)){
    etamin = std::max( -2.5, roiDescriptor->eta() - m_etaWidth );
    etamax = std::min(  2.5, roiDescriptor->eta() + m_etaWidth );

    phimin = HLT::wrap_phi( roiDescriptor->phi() - m_phiWidth );
    phimax = HLT::wrap_phi( roiDescriptor->phi() + m_phiWidth );

    etaL1 = roiDescriptor->eta();
    phiL1 = roiDescriptor->phi();
  } 
  else {
    etamin = std::max( -2.5, m_l1eta-m_etaWidth );
    etamax = std::min(  2.5, m_l1eta+m_etaWidth );

    phimin = HLT::wrap_phi( m_l1phi-m_phiWidth );
    phimax = HLT::wrap_phi( m_l1phi+m_phiWidth );

    etaL1 =  m_l1eta;
    phiL1 =  m_l1phi;
  }

  /// if we do ...
  TrigRoiDescriptor newroi( roiDescriptor->eta(), etamin, etamax, 
			    roiDescriptor->phi(), phimin, phimax);

#ifndef NDEBUG

  if ( (*m_log).level() <= MSG::DEBUG ) {
  (*m_log) << MSG::DEBUG  << " etamin = "<< etamin << endmsg;
  (*m_log) << MSG::DEBUG  << " etamax = "<< etamax << endmsg;
  (*m_log) << MSG::DEBUG  << " phimin = "<< phimin << endmsg;
  (*m_log) << MSG::DEBUG  << " phimax = "<< phimax << endmsg;
  }
#endif


#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) 
  (*m_log) << MSG::DEBUG  << " Making TrigEMCluster "<< endmsg;
#endif


  xAOD::TrigEMCluster* ptrigEmCluster = new xAOD::TrigEMCluster();
  m_trigEmClusterCollection->push_back( ptrigEmCluster );
  ptrigEmCluster->setEnergy(0.0);
  ptrigEmCluster->setEt(0.0);
  ptrigEmCluster->setRawEnergy(0.0);
  ptrigEmCluster->setRawEt(0.0);
  ptrigEmCluster->setE277(0);
  ptrigEmCluster->setEmaxs1(0);
  ptrigEmCluster->setE2tsts1(0);
  ptrigEmCluster->setEhad1(-999);
  ptrigEmCluster->setWeta2(-999);
  ptrigEmCluster->setFracs1(-999);
  ptrigEmCluster->setE233(-999);
  ptrigEmCluster->setE237(-999);
  ptrigEmCluster->setWstot(-999);
  ptrigEmCluster->setEta1(-999);
  ptrigEmCluster->setNCells(0);
  ptrigEmCluster->setRawEta(-999);
  ptrigEmCluster->setRawPhi(-999);
  m_monitoredCluster = ptrigEmCluster;
  (*m_log) << MSG::INFO  << " Values of Cluster defined default: "<< endmsg;
  // It is a good idea to clear the energies
  for(int i=0;i<CaloSampling::CaloSample::MINIFCAL0;i++){
    ptrigEmCluster->setEnergy((CaloSampling::CaloSample )i,0.);
    ptrigEmCluster->setRawEnergy((CaloSampling::CaloSample )i,0.);
  }
  // Initial cluster position is the LVL1 position
  ptrigEmCluster->setEta(etaL1);
  ptrigEmCluster->setPhi(phiL1);
  (*m_log) << MSG::INFO  << " Initial cluster position is the LVL1 position : DONE  "<< endmsg;

  // Add RoI word to TrigEMCluster
  // Dangerous !!!! we need to define a *new* roiDescriptor if we want to 
  // change the size, so we should be careful about *which* roi descriptor
  // we save, and *which* "roiWord" (if any) we store if we need to use it 
  // again
  (*ptrigEmCluster).setRoIword(roiDescriptor->roiWord());
  const CaloDetDescrElement * caloDDE = 0;

  // zeros the container per RoI
  //m_Container = 0;
  (*m_log) << MSG::INFO  << " m_emAlgTools.begin():  "<< endmsg;
  
  ToolHandleArray<IAlgToolCalo>::iterator it = m_emAlgTools.begin();
////  if ( m_timersvc ) m_timer[1]->start();
  uint32_t error = 0;
  for (; it < m_emAlgTools.end(); it++)  {
    (*it)->setCaloDetDescrElement(caloDDE);
  (*m_log) << MSG::INFO  << "setCaloDetDescrElement(caloDDE) : DONE  "<< endmsg;
    if ((*it)->execute(*ptrigEmCluster, newroi ).isFailure() ) {
      (*m_log) << MSG::WARNING << "T2Calo AlgToolEgamma returned Failure" << endmsg;
      return StatusCode::FAILURE;
    }
//    uint32_t in_error = (*it)->report_error();
//    if ( 0x0FFFFFFF & in_error ) m_conversionError++;
//    if ( 0xF0000000 & in_error ) m_algorithmError++;
//    if ( (*it)->getCaloDetDescrElement() != 0 ){
//    	caloDDE = (*it)->getCaloDetDescrElement();
//    }
//    error|=in_error;
  }
//  // support to new monitoring
  m_rCore=0;
  m_eRatio=0;
  m_stripRatio=0;
  m_MonEta=ptrigEmCluster->eta();
  m_MonPhi=ptrigEmCluster->phi();
  if ( ptrigEmCluster->e277()!=0 )
	m_rCore =  ptrigEmCluster->e237()/ptrigEmCluster->e277();
  if ( ptrigEmCluster->emaxs1()+ptrigEmCluster->e2tsts1() !=0){
	m_eRatio  =  ptrigEmCluster->emaxs1()-ptrigEmCluster->e2tsts1();
	m_eRatio /=  ptrigEmCluster->emaxs1()+ptrigEmCluster->e2tsts1();
  }
  (*m_log) << MSG::INFO  << " set m_eRatio : DONE  "<< endmsg; 
  // Cluster quality is a collection of possible errors
  // No error quality=0
  ptrigEmCluster->setClusterQuality(error);
//  if ( m_timersvc ) m_timer[1]->stop();
  (*m_log) << MSG::INFO  << " ptrigEmCluster->setClusterQuality(error) : DONE  "<< endmsg;
  if ( ( error & 0xC0000000 ) || ptrigEmCluster->phi() < -M_PI || ptrigEmCluster->phi() > +M_PI 
   || fabsf ( ptrigEmCluster->eta() ) > 10.0 ) {
    // Clustering failed. Transmit ahead L1
    ptrigEmCluster->setEta(etaL1);
    ptrigEmCluster->setPhi(phiL1);
    ptrigEmCluster->setEnergy(0.0);
    ptrigEmCluster->setEt(0.0);
  }
  (*m_log) << MSG::INFO  << " ptrigEmCluster->setxxx : DONE  "<< endmsg;
  if ( caloDDE != 0 ){
    if ( caloDDE->is_lar_em_barrel() ){
      for( ToolHandleArray<IEgammaCalibration>::iterator
  		  ical=m_calibsBarrel.begin();
  		  ical != m_calibsBarrel.end(); ++ical )
  	  (*ical)->makeCorrection(ptrigEmCluster,caloDDE);
    }else{
    for( ToolHandleArray<IEgammaCalibration>::iterator
  		ical=m_calibsEndcap.begin();
  		ical != m_calibsEndcap.end(); ++ical )
  	(*ical)->makeCorrection(ptrigEmCluster,caloDDE);
    }
  }
  (*m_log) << MSG::INFO  << " IEgammaCalibration : DONE  "<< endmsg;
  float calZ0 = 0;


  // Print out Cluster produced  
  if ( (*m_log).level() <= MSG::DEBUG ) {
    (*m_log) << MSG::DEBUG  << " Values of Cluster produced: "<< endmsg;
    (*m_log) << MSG::DEBUG  << " REGTEST: emEnergy = "<< (*ptrigEmCluster).energy() << endmsg;
    (*m_log) << MSG::DEBUG  << " REGTEST: hadEnergy = "<< (*ptrigEmCluster).ehad1() << endmsg;

    //if ( ptrigEmCluster->e277()!=0. )
      (*m_log) << MSG::DEBUG  << " REGTEST: e237= "
	             << (*ptrigEmCluster).e237()  << endmsg;
     (*m_log) << MSG::DEBUG  << " REGTEST: e277= " << (*ptrigEmCluster).e277() << endmsg;
    //else (*m_log) << MSG::DEBUG  << " REGTEST: e277 equals to 0" << endmsg;
    
/*
    (*m_log) << MSG::DEBUG  << " REGTEST: energyRatio = "
	           << (((*ptrigEmCluster).emaxs1()-(*ptrigEmCluster).e2tsts1())/
		            ((*ptrigEmCluster).emaxs1()+(*ptrigEmCluster).e2tsts1()))
	           << endmsg;
*/

    (*m_log) << MSG::DEBUG  << " REGTEST: clusterWidth = " << (*ptrigEmCluster).weta2() << endmsg;
    (*m_log) << MSG::DEBUG  << " REGTEST: frac73 = " << (*ptrigEmCluster).fracs1() << endmsg;
    (*m_log) << MSG::DEBUG  << " REGTEST: e233 = " << (*ptrigEmCluster).e233() << endmsg;
    (*m_log) << MSG::DEBUG  << " REGTEST: wstot = " << (*ptrigEmCluster).wstot() << endmsg;
    (*m_log) << MSG::DEBUG  << " REGTEST: eta = "<< (*ptrigEmCluster).eta() << endmsg;
    (*m_log) << MSG::DEBUG  << " REGTEST: phi = "<< (*ptrigEmCluster).phi() << endmsg;
    (*m_log) << MSG::DEBUG  << " REGTEST: Eta1 = "<< (*ptrigEmCluster).eta1() << endmsg;
    (*m_log) << MSG::DEBUG  << " REGTEST: calZ0 = "<< calZ0 << endmsg;
    (*m_log) << MSG::DEBUG  << " REGTEST: quality = "<< (*ptrigEmCluster).clusterQuality() << endmsg;
    (*m_log) << MSG::DEBUG  << std::hex << " REGTEST: roiWord = 0x" << (*ptrigEmCluster).RoIword() << std::dec <<endmsg;
  }
  std::string key = "";
  (*m_log) << MSG::INFO  << " REGTEST : DONE  "<< endmsg;
  //hltStatus = recordAndAttachFeature(outputTE, ptrigEmCluster, key, m_trigEmClusterKey);


#ifdef DONTDO
  // Create a new RoiDescriptor with updated eta and phi.
  // Note that the steering will propagate l1Id and roiId automatically
  // so no need to set them. (is this true?)
  TrigRoiDescriptor* newRoiDescriptor = 
      new TrigRoiDescriptor(roiDescriptor->l1Id(), roiDescriptor->roiId(),
			    ptrigEmCluster->eta(), ptrigEmCluster->eta()-0.2, 
			    ptrigEmCluster->eta()+0.2,
			    ptrigEmCluster->phi(),
			    HLT::wrap_phi(ptrigEmCluster->phi()-0.2), 
			    HLT::wrap_phi(ptrigEmCluster->phi()+0.2) );  
  (*m_log) << MSG::INFO  << "  HLT::wrap_phi(ptrigEmCluster->phi()-0.2) : DONE  "<< endmsg;
  
  //  hltStatus = attachFeature(outputTE,newRoiDescriptor,"TrigT2CaloEgamma");
  (*m_log) << MSG::INFO  << " hltStatus = attachFeature : DONE  "<< endmsg;
  if ( hltStatus != HLT::OK ) {
     (*m_log) << MSG::ERROR << "Write of update TrigRoiDescriptor into outputTE failed"
	   << endmsg;
//     if ( m_timersvc ) m_timer[0]->stop();
     return hltStatus;
  }

#endif



  } // end of roiCollection iterator
  
  return StatusCode::SUCCESS;
}


StatusCode T2CaloEgammaFastAlgo::finalize(){

#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) 
    (*m_log) << MSG::INFO << "in finalize()" << endmsg;
#endif

  return StatusCode::SUCCESS;
}

