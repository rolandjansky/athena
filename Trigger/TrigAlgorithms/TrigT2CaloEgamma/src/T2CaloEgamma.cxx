/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 NAME:     T2CaloEgamma.cxx
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma

 AUTHOR:   M.P. Casado
           S.R. Armstrong

 - Add variables for job option controlled region limits, set defaults
   to most likely values.
 - Add function EtaPhiRange to return the maximum and minimum eta or phi
   values to use when calculating energy sums over a region  - R. Soluk
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "AthLinks/ElementLink.h"

#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"

#include "TrigT2CaloEgamma/T2CaloEgamma.h"
#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "TrigT2CaloCalibration/IEgammaCalibration.h"

#include "TrigT2CaloCommon/phiutils.h"

class ISvcLocator;

T2CaloEgamma::T2CaloEgamma(const std::string & name, ISvcLocator* pSvcLocator)
  : T2CaloBase(name, pSvcLocator),  m_log(0), m_calibsBarrel(this), m_calibsEndcap(this), m_storeCells(false), m_egammaqweta2c("egammaqweta2c/egammaqweta2c",this)
{
    declareProperty("TrigEMClusterKey",m_trigEmClusterKey = "TrigT2CaloEgamma");
    declareProperty("L1ForceEta",m_l1eta = -10.0);
    declareProperty("L1ForcePhi",m_l1phi = -10.0);
    declareProperty("CalibListBarrel",m_calibsBarrel,"list of calib tools for the Barrel clusters");
    declareProperty("CalibListEndcap",m_calibsEndcap,"list of calib tools for the EndCap clusters");
    declareProperty("StoreCells",m_storeCells,"store cells in container attached to RoI");
    declareProperty("egammaqweta2c",m_egammaqweta2c,"Egamma Weta2 correction");
    declareProperty("RhoFirstLayer",m_rhoFirstLayer);
    declareProperty("RhoMiddleLayer",m_rhoMiddleLayer);
    declareProperty("ZFirstLayer",m_zFirstLayer);
    declareProperty("ZMiddleLayer",m_zMiddleLayer);
    declareProperty("RhoEta",m_rhoEta);
    declareProperty("ZEta",m_zEta);
}

T2CaloEgamma::~T2CaloEgamma()
{
  delete m_log;
}


HLT::ErrorCode T2CaloEgamma::hltInitialize()
{
  if (!m_log) m_log = new MsgStream(messageService(), name());

  // Support for new monitoring
  declareMonitoredVariable("Eta", m_MonEta);
  declareMonitoredVariable("Phi", m_MonPhi);
  declareMonitoredObject("Et", m_monitoredCluster,&xAOD::TrigEMCluster::et);
  declareMonitoredObject("Had1Et", m_monitoredCluster,&xAOD::TrigEMCluster::ehad1);
  declareMonitoredObject("weta2", m_monitoredCluster,&xAOD::TrigEMCluster::weta2);
  declareMonitoredObject("frac73", m_monitoredCluster,&xAOD::TrigEMCluster::fracs1);
  // More complicated variables to be monitored
  declareMonitoredVariable("Rcore", m_rCore );
  declareMonitoredVariable("Eratio",m_eRatio);
  declareMonitoredVariable("ConversionErrors", m_conversionError);
  declareMonitoredVariable("AlgorithmErrors", m_algorithmError);
  if ( m_calibsBarrel.retrieve().isFailure() ) (*m_log) << MSG::DEBUG << "No Calibration Available for the Barrel" << endreq;
  if ( m_calibsEndcap.retrieve().isFailure() ) (*m_log) << MSG::DEBUG << "No Calibration Available for the Endcap" << endreq;

  // Sets the name of the key container 
  ToolHandleArray<IAlgToolCalo>::iterator it = m_emAlgTools.begin();
  for (; it < m_emAlgTools.end(); it++)
    (*it)->setCellContainerPointer(&m_Container);

  if ( (m_egammaqweta2c.retrieve()).isFailure() ){
    (*m_log) << MSG::FATAL << "Could not find egammaqweta2c" << endreq;
    return HLT::TOOL_FAILURE;
  }
  return HLT::OK;
}


HLT::ErrorCode T2CaloEgamma::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE)
{
  // Time total T2CaloEgamma execution time.
  if ( m_timersvc ) m_timer[0]->start();
  m_conversionError=0;
  m_algorithmError=0;
  m_monitoredCluster=0;

  // Necessary to add RingerRings' ElementLink
  m_tmpOutputTE = outputTE;
  m_rings = 0;

  
#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) 
  (*m_log) << MSG::INFO << "in execute()" << endreq;
#endif
 
  // Some debug output:
#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) {
    (*m_log) << MSG::DEBUG
	  << "outputTE->getId(): "
	  << outputTE->getId()
	  << endreq;

    (*m_log) << MSG::DEBUG
	  << "inputTE->getId(): "
	  << inputTE->getId()
	  << endreq;
   }
#endif
    
  const TrigRoiDescriptor* roiDescriptor = 0;

  HLT::ErrorCode hltStatus = getFeature(inputTE, roiDescriptor);

  if ( hltStatus == HLT::OK ) {
#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) 
      (*m_log) << MSG::DEBUG  << *roiDescriptor << endreq;
#endif
  } else {
    (*m_log) <<  MSG::WARNING << " Failed to find RoiDescriptor " << endreq;
    return hltStatus;
  }

  // Some debug output:

#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) 
  (*m_log) << MSG::DEBUG  << "Message to count events. LVL1 phi="
	   << roiDescriptor->phi()
	   << " & LVL1 eta="
	   << roiDescriptor->eta()
	   << endreq;
#endif

  // This will be used to restrict hot cell search
  /// sadly, there will be real issues because this uses 
  /// *floats* rather than doubles
  /// - what *exactly* is going on here? Why is the etamin/phimin etc 
  ///   being changed? In the new scheme, this is getting an RoI from 
  ///   somewhere, so the size should be in the roiDescriptor
  ///   if we want to change the size, we create a new roiDescriptor, 
  ///   and then we should attach it to the navigation 
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
  /// NO!!! wrap to range -pi .. pi
  //  while (phimin < 0) phimin += 2. * M_PI;
  //  while (phimax > 2 * M_PI) phimax -= 2. * M_PI;
  /// maybe these following 2 lines are because of the 
  /// float -> double difference in pi?
  //  if (phiL1+0.001>=M_PI) phiL1-=0.001;
  //  if (phiL1-0.001<=-M_PI) phiL1+=0.001;

  /// Do we want to create a new roiDescriptor here or not ??

  /// if we do ...

  TrigRoiDescriptor newroi( roiDescriptor->eta(), etamin, etamax, 
			    roiDescriptor->phi(), phimin, phimax);

  /// NB: !!! this *hasn't* checked that this "max(2.5, eta+etawidth) etc still includes
  ///         the roi eta in the first place !!! this whole creation of limits and regions
  ///         should be tidied up 

#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) {
  (*m_log) << MSG::DEBUG  << " etamin = "<< etamin << endreq;
  (*m_log) << MSG::DEBUG  << " etamax = "<< etamax << endreq;
  (*m_log) << MSG::DEBUG  << " phimin = "<< phimin << endreq;
  (*m_log) << MSG::DEBUG  << " phimax = "<< phimax << endreq;
  }
#endif


#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) 
  (*m_log) << MSG::DEBUG  << " Making TrigEMCluster "<< endreq;
#endif


  xAOD::TrigEMCluster* ptrigEmCluster = new xAOD::TrigEMCluster();
  //ptrigEmCluster->setStore(ptrigEmClusterAux);
  ptrigEmCluster->makePrivateStore();
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
  ptrigEmCluster->setWstot(-999);
  ptrigEmCluster->setEta1(-999);
  ptrigEmCluster->setNCells(0);
  ptrigEmCluster->setRawEta(-999);
  ptrigEmCluster->setRawPhi(-999);
  m_monitoredCluster = ptrigEmCluster;

  // It is a good idea to clear the energies
  for(int i=0;i<CaloSampling::CaloSample::MINIFCAL0;i++){
    ptrigEmCluster->setEnergy((CaloSampling::CaloSample )i,0.);
    ptrigEmCluster->setRawEnergy((CaloSampling::CaloSample )i,0.);
  }
  // Initial cluster position is the LVL1 position
  ptrigEmCluster->setEta(etaL1);
  ptrigEmCluster->setPhi(phiL1);

  // Add RoI word to TrigEMCluster
  // Dangerous !!!! we need to define a *new* roiDescriptor if we want to 
  // change the size, so we should be careful about *which* roi descriptor
  // we save, and *which* "roiWord" (if any) we store if we need to use it 
  // again
  (*ptrigEmCluster).setRoIword(roiDescriptor->roiWord());
  const CaloDetDescrElement * caloDDE = 0;

  // zeros the container per RoI
  m_Container = 0;
  
  ToolHandleArray<IAlgToolCalo>::iterator it = m_emAlgTools.begin();
  if ( m_timersvc ) m_timer[1]->start();
  uint32_t error = 0;
  for (; it < m_emAlgTools.end(); it++)  {
    (*it)->setCaloDetDescrElement(caloDDE);
    //   if ((*it)->execute(*ptrigEmCluster,etamin,etamax,phimin,phimax).isFailure() ) {
    if ((*it)->execute(*ptrigEmCluster, newroi ).isFailure() ) {
      (*m_log) << MSG::WARNING << "T2Calo AlgToolEgamma returned Failure" << endreq;
      //      return HLT::TOOL_FAILURE;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,HLT::Reason::USERDEF_1);
    }
    uint32_t in_error = (*it)->report_error();
    if ( 0x0FFFFFFF & in_error ) m_conversionError++;
    if ( 0xF0000000 & in_error ) m_algorithmError++;
    if ( (*it)->getCaloDetDescrElement() != 0 ){
    	caloDDE = (*it)->getCaloDetDescrElement();
    }
    error|=in_error;
  }
  // support to new monitoring
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
  
  // Cluster quality is a collection of possible errors
  // No error quality=0
  ptrigEmCluster->setClusterQuality(error);
  if ( m_timersvc ) m_timer[1]->stop();

  if ( ( error & 0xC0000000 ) || ptrigEmCluster->phi() < -M_PI || ptrigEmCluster->phi() > +M_PI 
   || fabsf ( ptrigEmCluster->eta() ) > 10.0 ) {
    // Clustering failed. Transmit ahead L1
    ptrigEmCluster->setEta(etaL1);
    ptrigEmCluster->setPhi(phiL1);
    ptrigEmCluster->setEnergy(0.0);
    ptrigEmCluster->setEt(0.0);
  }

  if ( caloDDE != 0 ){
    if ( caloDDE->is_lar_em_barrel() ){
      for( ToolHandleArray<IEgammaCalibration>::const_iterator
  		  ical=m_calibsBarrel.begin();
  		  ical != m_calibsBarrel.end(); ++ical )
  	  (*ical)->makeCorrection(ptrigEmCluster,caloDDE);
    }else{
    for( ToolHandleArray<IEgammaCalibration>::const_iterator
  		ical=m_calibsEndcap.begin();
  		ical != m_calibsEndcap.end(); ++ical )
  	(*ical)->makeCorrection(ptrigEmCluster,caloDDE);
    }
  }

  // Final correction to weta only
  if ( caloDDE != 0 )
    ptrigEmCluster->setWeta2( m_egammaqweta2c->Correct(ptrigEmCluster->eta(),caloDDE->eta(),ptrigEmCluster->weta2()) );

  
  float calZ0 = 0;
  /// don't use calculated z position yet ...
  //  calZ0 = calculateZ0((*ptrigEmCluster).Eta1(), (*ptrigEmCluster).eta());;


  // Print out Cluster produced  
  if ( (*m_log).level() <= MSG::DEBUG ) {
    (*m_log) << MSG::DEBUG  << " Values of Cluster produced: "<< endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: emEnergy = "<< (*ptrigEmCluster).energy() << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: hadEnergy = "<< (*ptrigEmCluster).ehad1() << endreq;

    if ( ptrigEmCluster->e277()!=0. )
      (*m_log) << MSG::DEBUG  << " REGTEST: rCore = "
	             << ((*ptrigEmCluster).e237() )/ ((*ptrigEmCluster).e277()) << endreq;
    else (*m_log) << MSG::DEBUG  << " REGTEST: e277 equals to 0" << endreq;
    
    (*m_log) << MSG::DEBUG  << " REGTEST: energyRatio = "
	           << (((*ptrigEmCluster).emaxs1()-(*ptrigEmCluster).e2tsts1())/
		            ((*ptrigEmCluster).emaxs1()+(*ptrigEmCluster).e2tsts1()))
	           << endreq;

    (*m_log) << MSG::DEBUG  << " REGTEST: clusterWidth = " << (*ptrigEmCluster).weta2() << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: frac73 = " << (*ptrigEmCluster).fracs1() << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: e233 = " << (*ptrigEmCluster).e233() << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: wstot = " << (*ptrigEmCluster).wstot() << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: eta = "<< (*ptrigEmCluster).eta() << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: phi = "<< (*ptrigEmCluster).phi() << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: Eta1 = "<< (*ptrigEmCluster).eta1() << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: calZ0 = "<< calZ0 << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: quality = "<< (*ptrigEmCluster).clusterQuality() << endreq;
    (*m_log) << MSG::DEBUG  << std::hex << " REGTEST: roiWord = 0x" << (*ptrigEmCluster).RoIword() << std::dec <<endreq;
  }
  std::string key = "";

  hltStatus = recordAndAttachFeature(outputTE, ptrigEmCluster, key, m_trigEmClusterKey);

  if (hltStatus != HLT::OK){
#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) 
      (*m_log) << MSG::ERROR << "Write of TrigEMCluster into outputTE failed"
	    << endreq;
#endif
    if ( m_timersvc ) m_timer[0]->stop();
    return hltStatus;
  }

  if ( m_storeCells ) {
    if ( m_Container != 0 ) {
      hltStatus = recordAndAttachFeature(outputTE, m_Container, key, "TrigT2CaloEgammaCells");
      if (hltStatus != HLT::OK){
#ifndef NDEBUG
	if ( (*m_log).level() <= MSG::DEBUG ) 
	  (*m_log) << MSG::ERROR << "Write of TrigEMClusterCells into outputTE failed"
		   << endreq;
#endif
	if ( m_timersvc ) m_timer[0]->stop();
	return hltStatus;
      }
    } // End of if to check whether there is a container
  } // End of if to check the option is ok

  // Create a new RoiDescriptor with updated eta and phi.
  // Note that the steering will propagate l1Id and roiId automatically
  // so no need to set them. (is this true?)
  TrigRoiDescriptor* newRoiDescriptor = 0;

  if (calZ0 !=0 ) { 
    // add z position
    if ( calZ0<-225 ) calZ0 = -225;
    if ( calZ0> 225 ) calZ0 =  225;
  
    newRoiDescriptor = 
      new TrigRoiDescriptor(roiDescriptor->l1Id(), roiDescriptor->roiId(),
			    ptrigEmCluster->eta(), ptrigEmCluster->eta()-m_etaWidthForID, ptrigEmCluster->eta()+m_etaWidthForID,
			    ptrigEmCluster->phi(), 
			    HLT::wrap_phi(ptrigEmCluster->phi()-m_phiWidthForID), 
			    HLT::wrap_phi(ptrigEmCluster->phi()+m_phiWidthForID), 
			    calZ0 ); // don't specify widths - then we get the default values of  -225, +225 
  }
  else { 
    newRoiDescriptor = 
      new TrigRoiDescriptor(roiDescriptor->l1Id(), roiDescriptor->roiId(),
			    ptrigEmCluster->eta(), ptrigEmCluster->eta()-m_etaWidthForID, ptrigEmCluster->eta()+m_etaWidthForID,
			    ptrigEmCluster->phi(),
			    HLT::wrap_phi(ptrigEmCluster->phi()-m_phiWidthForID), 
			    HLT::wrap_phi(ptrigEmCluster->phi()+m_phiWidthForID) );  
  }
  
  hltStatus = attachFeature(outputTE,newRoiDescriptor,"TrigT2CaloEgamma");
 
  if ( hltStatus != HLT::OK ) {
     (*m_log) << MSG::ERROR << "Write of update TrigRoiDescriptor into outputTE failed"
	   << endreq;
     if ( m_timersvc ) m_timer[0]->stop();
     return hltStatus;
  }


 // record ringer feature if available
  if(m_rings){
    hltStatus = recordAndAttachRings(outputTE);
    if(hltStatus != HLT::OK){
      (*m_log)  << MSG::ERROR << "Can not attach the xAOD::TrigRingerRings features into StoreGate." << endreq;
      return hltStatus;
    }
    if( (*m_log).level() <= MSG::DEBUG){
      (*m_log) << MSG::DEBUG << "attach xAOD::TrigRingerRings with feature name " << m_ringerFeatureLabel
                             << " and with roiword 0x" << std::hex << m_rings->emCluster()->RoIword() << std::dec << endreq;
                             //<< " and with roiword 0x" << std::hex << m_rings->RoIword() << std::dec << endreq;
    }
  }


#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) {
    (*m_log) << MSG::DEBUG  << "Recorded an RoiDescriptor " << *newRoiDescriptor << endreq;
    (*m_log) << MSG::DEBUG << "We assume success, set TE with Id "
	  << outputTE->getId() << " active to signal positive result."
	  << endreq;
  }
#endif
 
  //  if ( error ) return HLT::TOOL_FAILURE;
  if ( error )
     return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::USERDEF_2);
  // Time total T2CaloEgamma execution time.
  if ( m_timersvc ) m_timer[0]->stop();
  
  return HLT::OK;
}


HLT::ErrorCode T2CaloEgamma::hltFinalize(){

#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) 
    (*m_log) << MSG::INFO << "in finalize()" << endreq;
#endif

  return HLT::OK;
}


void T2CaloEgamma::setRingsFeature(xAOD::TrigRingerRings *rings, std::string &key, const std::string &featureLabel){
  m_rings = rings;
  m_ringerKey = key;
  m_ringerFeatureLabel = featureLabel;
}


HLT::ErrorCode T2CaloEgamma::recordAndAttachRings(HLT::TriggerElement *outputTE) {
  HLT::ErrorCode hltStatus;

  if (m_tmpOutputTE) {
    ElementLink<xAOD::TrigEMClusterContainer> el_t2calo_clus;

    // retrieve TrigEMCluster using the Navigation methods
    hltStatus = getFeatureLink<xAOD::TrigEMClusterContainer,xAOD::TrigEMCluster>(outputTE, el_t2calo_clus);
    if (hltStatus != HLT::OK){
      return hltStatus;
    }
    if(el_t2calo_clus.isValid()){
      if ( (*m_log).level() <= MSG::DEBUG ) {
        (*m_log) << MSG::DEBUG << "ElementLink to xAOD::TrigEMClusterContainer is valid. set into xAOD::TrigRingerRings..." << endreq;
      }

      /* 
      m_rings->auxdata< ElementLink< xAOD::TrigEMClusterContainer >  >("emClusterLink") = el_t2calo_clus;// decoration for now.
      if( (*m_log).level() <= MSG::DEBUG){
        static SG::AuxElement::Accessor<ElementLink<xAOD::TrigEMClusterContainer>>orig("emClusterLink");
        if( !orig.isAvailable(*m_rings) || !orig(*m_rings).isValid() ){
          (*m_log) << MSG::DEBUG << "Problem with emClusterLink." << endreq;
        }
      }*/
      
      m_rings->setEmClusterLink( el_t2calo_clus  );
    }

    hltStatus = recordAndAttachFeature<xAOD::TrigRingerRings>(m_tmpOutputTE, m_rings, m_ringerKey, m_ringerFeatureLabel);
    if (hltStatus != HLT::OK) {
      return hltStatus;
    }
  } else {
    return HLT::OK; // Should return error code!
  }
  return HLT::OK;
}



float T2CaloEgamma::calculateZ0(const float etaLayer1, const float etaLayer2) {

  // etaLayer1: first layer emcl_etaLayer1, etaLayer2: middle layer emcl_eta
  // Z  = rho*sinh(eta)
  // Z0 = (z1*rho2 - z2*rho1) / (rho2 - rho1)
  //    = rho1*rho2 * (sinh(etaLayer1) - sinh(etaLayer2)) / (rho2-rho1)  // barrel
  //    = z1*z2 * (sinh(etaLayer1) - sinh(etaLayer2)) / (z2*sinh(etaLayer1) - z1*sinh(etaLayer2)) // endcap

  float Z0 = 1e7;
  if (m_zEta.size() == 0 ) return Z0;
  // crack region: 1.37<|eta(cluster)|<1.52
  if ( fabs(etaLayer1)>=1.37 && fabs(etaLayer1)<=1.52 ) return Z0;
  if ( fabs(etaLayer2)>=1.37 && fabs(etaLayer2)<=1.52 ) return Z0;

  float rho1(-1), rho2(-1); // barrel
  for (unsigned int i=1; i<m_rhoEta.size(); i++) {
    if ( fabs(etaLayer1)<=m_rhoEta[i] && fabs(etaLayer1)>m_rhoEta[i-1] ) rho1 = m_rhoFirstLayer [i-1];
    if ( fabs(etaLayer2)<=m_rhoEta[i] && fabs(etaLayer2)>m_rhoEta[i-1] ) rho2 = m_rhoMiddleLayer[i-1];
  }
  bool useRho = (rho1>0) && (rho2>0);

  float z1(-1), z2(-1);     // endcap
  for (unsigned int i=1; i<m_zEta.size(); i++) {
    if ( fabs(etaLayer1)<=m_zEta[i] && fabs(etaLayer1)>m_zEta[i-1] ) z1 = m_zFirstLayer [i-1];
    if ( fabs(etaLayer2)<=m_zEta[i] && fabs(etaLayer2)>m_zEta[i-1] ) z2 = m_zMiddleLayer[i-1];
  }
  bool useZ = (z1>0) && (z2>0);

  if (useRho && !useZ) {
    Z0 = rho1*rho2 * (sinh(etaLayer1) - sinh(etaLayer2)) / (rho2 - rho1);
  }

  if (!useRho && useZ) {
    Z0 = z1*z2* (sinh(etaLayer1)-sinh(etaLayer2)) / (z2*sinh(etaLayer1)-z1*sinh(etaLayer2));
    // need to be check why
    //Z0 += 1288;
    Z0 += (1288 + 92);
  }
  return Z0;
}
