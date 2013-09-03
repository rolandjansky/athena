/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerRateTools/SlicesInfoTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/MsgStream.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>

SlicesInfoTool::SlicesInfoTool (const std::string& type, const std::string& name, const IInterface* parent) : 
        AlgTool( type, name, parent ), 
        m_log(0),
        m_storeGate("StoreGateSvc", name),
        m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
        m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name) {
    
    declareProperty("doMET", m_doMet = true);
    declareProperty("doBCM", m_doBCM = true);

    declareInterface<SlicesInfoTool>(this);
    declareInterface<ISlicesInfoTool>(this);
  
    declareProperty("BCMRdoContainer", m_bcmRdoContainerName = "BCM_RDOs");
}

/**************/
/* Destructor */
/**************/

SlicesInfoTool::~SlicesInfoTool() {

}

/**************/
/* Initialize */
/**************/

StatusCode SlicesInfoTool::initialize() {
    
    // get message service and print out properties
    m_log = new MsgStream(msgSvc(), name());

    (*m_log) << MSG::INFO << "In initialize()" << endreq;
    
    // get StoreGate
    StatusCode sc = m_storeGate.retrieve();

    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
        return sc;
    }

    // get TrigDecisionTool 
    sc = m_trigDec.retrieve();
    
    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not retrieve TrigDecisionTool!" << endreq;
        return sc;
    }
    
    
    // get HistSvc
    sc = service("THistSvc", m_histSvc);
   
    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not retrieve THistSvc!" << endreq;
        return sc;
    }
   


    return StatusCode::SUCCESS;
}

/************/
/* Finalize */
/************/

StatusCode SlicesInfoTool::finalize() {

    (*m_log) << MSG::INFO << "In finalize()" << endreq;

    return StatusCode::SUCCESS;
}

StatusCode SlicesInfoTool::bookSlices() {

    (*m_log) << MSG::INFO << "Booking output ROOT objects" << endreq;

    tree = new TTree("SlicesInfo", "Slices Info");
    (*m_log) << MSG::INFO << "tree" << endreq;
    
    StatusCode sc = m_histSvc->regTree(std::string("/TriggerRateTools/") + tree->GetName(), tree);
    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not register Slices Info Tree" << endreq;
        return sc;
    }
    tree->Branch("EventNumber", &evt_num, "evt_num/I");

    if (m_doMet) {
        sc = bookMet();
	if ( sc.isFailure() ) {
		(*m_log) << MSG::ERROR << "Could not book MET SliceInfo" << endreq;
		return sc;
	}
    }
    if (m_doBCM) {
	sc = bookBCM();
	if ( sc.isFailure() ) {
		(*m_log) << MSG::ERROR << "Could not book BCM SliceInfo" << endreq;
    		return sc;
	}
    }

    return StatusCode::SUCCESS;
}

StatusCode SlicesInfoTool::fillSlices() {

    (*m_log) << MSG::INFO << "Filling output ROOT objects" << endreq;
    
    const DataHandle<EventInfo> evt;
    StatusCode sc = m_storeGate->retrieve(evt);
    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not retrieve evt from StoreGate" << endreq;
        return sc;
    }

    evt_num = evt->event_ID()->event_number();
 
   
    if (m_doMet) {
	sc = fillMet();
	if ( sc.isFailure() ) {
        	(*m_log) << MSG::ERROR << "Could not fill MET slice Info" << endreq;
        	return sc;
    	}
    }
    if (m_doBCM) {
        sc = fillBCM();
	if ( sc.isFailure() ) {
        	(*m_log) << MSG::ERROR << "Could not BCM slice Info" << endreq;
        	return sc;
    	}
    }

    tree->Fill();

    return StatusCode::SUCCESS;
}

/* BCM Section */

StatusCode SlicesInfoTool::bookBCM() {
    (*m_log) << MSG::DEBUG << "Booking output ROOT objects for BCM " << tree << endreq;

    
    bcm = new TrigBCMObject();
    tree->Branch("BCM", &bcm, 128000);
    
    (*m_log) << MSG::DEBUG << "Booked output ROOT objects for BCM" << endreq;

    return StatusCode::SUCCESS;
}

StatusCode SlicesInfoTool::fillBCM() {
    (*m_log) << MSG::DEBUG << "Filling output ROOT objects for BCM" << endreq;

    bcm = new TrigBCMObject();
    
    // read the BCM hit container from persistecy storage
    const BCM_RDO_Container* bcmRdoCont = 0;
    StatusCode sc=m_storeGate->retrieve(bcmRdoCont, m_bcmRdoContainerName);
    if( sc.isFailure()  ||  !bcmRdoCont ) {
        (*m_log) << MSG::WARNING
            << "No BCM rdo container found in TDS"
            << endreq;
        return sc;
    }
    (*m_log) << MSG::DEBUG << "BCM Rdo Container successfully retrieved" << endreq;
    
    // loop & get hit information
    BCM_RDO_Container::const_iterator bcmItr  = bcmRdoCont->begin();
    BCM_RDO_Container::const_iterator bcmItrE = bcmRdoCont->end();
    for (; bcmItr != bcmItrE; ++bcmItr) {
        BCM_RDO_Collection* coll = (*bcmItr);
        BCM_RDO_Collection::const_iterator rdoItr = coll->begin();
        BCM_RDO_Collection::const_iterator rdoItrE = coll->end();
        for (; rdoItr!=rdoItrE; ++rdoItr) {
            BCM_RawData* rdo = (*rdoItr);
            if (rdo->getPulse1Width()!=0) {
                bcm->m_bcm_pulse_x.push_back(rdo->getPulse1Position());
                bcm->m_bcm_pulse_w.push_back(rdo->getPulse1Width());
                bcm->m_bcm_pulse_mod.push_back((rdo->getChannel())%8);
                if (rdo->getChannel()>=8) bcm->m_bcm_pulse_hithr.push_back(1);
                else bcm->m_bcm_pulse_hithr.push_back(0);
            }
            if (rdo->getPulse2Width()!=0) {
                bcm->m_bcm_pulse_x.push_back(rdo->getPulse2Position());
                bcm->m_bcm_pulse_w.push_back(rdo->getPulse2Width());
                bcm->m_bcm_pulse_mod.push_back((rdo->getChannel())%8);
                if (rdo->getChannel()>=8) bcm->m_bcm_pulse_hithr.push_back(1);
                else bcm->m_bcm_pulse_hithr.push_back(0);
            }
        }
    }
    
    bcm->m_bcm_A_toC_trigger = (int)(m_trigDec->isPassedBits("L1_BCM_CtoA") & TrigDefs::L1_isPassedBeforePrescale);
    bcm->m_bcm_C_toA_trigger =  (int)(m_trigDec->isPassedBits("L1_BCM_CtoA") & TrigDefs::L1_isPassedBeforePrescale);
    bcm->m_bcm_Wide_trigger = (int)(m_trigDec->isPassedBits("L1_BCM_Wide") & TrigDefs::L1_isPassedBeforePrescale);
    bcm->m_bcm_Ideal_trigger = (int)(m_trigDec->isPassedBits("L1_BCM_Ideal") & TrigDefs::L1_isPassedBeforePrescale);

    (*m_log) << MSG::DEBUG << "bcm : " << bcm << "\ttree : " << tree << endreq;

    return sc;

}

/* MET Section */

StatusCode SlicesInfoTool::bookMet() {

    (*m_log) << MSG::DEBUG << "Booking output ROOT objects for MET" << endreq;

    etMissL1 = new TrigL1MissingEtObject();
    tree->Branch("L1MissingEt", &etMissL1, 128000);
    etMissL2 = new TrigL2MissingEtObject();
    tree->Branch("L2MissingEt", &etMissL2, 128000);
    etMissEF = new TrigEFMissingEtObject();
    tree->Branch("EFMissingEt", &etMissEF, 128000);

    return StatusCode::SUCCESS;
}

StatusCode SlicesInfoTool::fillMet() {

    (*m_log) << MSG::INFO << "Filling output ROOT objects for MET" << endreq;

    // LVL 1
    
    float m_DigitScale = 1000.0;
    const LVL1_ROI* lvl1ROI;
    StatusCode sc = m_storeGate->retrieve(lvl1ROI);
    if (sc.isFailure() || lvl1ROI == 0) {
        (*m_log) << MSG::DEBUG << "Could not find LVL1_ROI in StoreGate." << endreq;
    }

    if ( lvl1ROI != 0 ) {
        LVL1_ROI::energysums_type::const_iterator itMiss = (lvl1ROI->getEnergySumROIs()).begin();
        LVL1_ROI::energysums_type::const_iterator itMiss_e = (lvl1ROI->getEnergySumROIs()).end();
        for( ; itMiss != itMiss_e ; ++itMiss  ) {

            //fillEtMissL1Object(*itMiss );
            etMissL1 = new TrigL1MissingEtObject();
  
            etMissL1->m_EtMissHit =0;  
            etMissL1->m_EtSumHit =0;
            etMissL1->m_Ex = itMiss->getExMiss();
            etMissL1->m_Ey = itMiss->getEyMiss();
            etMissL1->m_EtMiss = sqrt(etMissL1->m_Ex*etMissL1->m_Ex + etMissL1->m_Ey*etMissL1->m_Ey);
            etMissL1->m_EtSum = itMiss->getEnergyT() ;
            
        }
        
        (*m_log) << MSG::DEBUG << "EnergySum_ROI (AOD) object is found "<<endreq;
        
    } else {

        const LVL1::EnergyRoI* energyroi(0);
        sc = m_storeGate->retrieve(energyroi); 
        if(  !sc.isFailure() && energyroi != 0 ) {
            if( m_configSvc && m_configSvc->thresholdConfig() ) {
                m_DigitScale=m_configSvc->thresholdConfig()->caloInfo().globalScale();
                (*m_log) << MSG::DEBUG << "Read DigitScale " << m_DigitScale << " MeV/count" << endreq;
            }else{
                (*m_log) << MSG::DEBUG << " Digit scale is assumed at " <<  m_DigitScale << endreq;
            }

            //fillEtMissL1Object(*energyroi, m_DigitScale);

            etMissL1 = new TrigL1MissingEtObject();
            LVL1::JEPRoIDecoder conv;
            etMissL1->m_EtMissHit = (energyroi->roiWord2()>>16)&0xFF;
            etMissL1->m_EtSumHit =(energyroi->roiWord1()>>16)&0xF;
            etMissL1->m_Ex =conv.energyX(energyroi->roiWord0())*m_DigitScale;	   
            etMissL1->m_Ey =conv.energyY(energyroi->roiWord1())*m_DigitScale;	     
            etMissL1->m_EtMiss = static_cast<long>(sqrt(static_cast<double>(etMissL1->m_Ex*etMissL1->m_Ex + 
								etMissL1->m_Ey*etMissL1->m_Ey)));  
            etMissL1->m_EtSum =conv.energyT(energyroi->roiWord2())*m_DigitScale;   

            // Flag overflows
            if (conv.energyOverflow(energyroi->roiWord0())) {
                etMissL1->m_Ex += (1<<14)*etMissL1->m_Ex/std::abs(etMissL1->m_Ex)*m_DigitScale;
                etMissL1->m_EtMiss += (1<<14)*m_DigitScale;
            }
            if (conv.energyOverflow(energyroi->roiWord1())) {
                etMissL1->m_Ey += (1<<14)*etMissL1->m_Ey/std::abs(etMissL1->m_Ey)*m_DigitScale;
                etMissL1->m_EtMiss += (1<<14)*m_DigitScale;
            }
            if (conv.energyOverflow(energyroi->roiWord2())) {
                etMissL1->m_EtSum += (1<<15)*m_DigitScale;
            }

            (*m_log) << MSG::DEBUG << "L1  RDO EnergyRoI object is found "<<endreq;
        }else{

            (*m_log) << MSG::DEBUG << " No EnergyRoI neither EnergySum_ROI object is found "<<endreq;  
        }
    }


    // HLT

    int ntag=2;
    std::string METTags[]={"HLT_T2MissingET", "HLT_TrigEFMissingET"};

    etMissL2 = new TrigL2MissingEtObject();
    etMissEF = new TrigEFMissingEtObject();
 
    for (int itag=0; itag < ntag; itag++) { // loop over L2, EF

        const TrigMissingETContainer* trigMETcont;
        sc=m_storeGate->retrieve(trigMETcont , METTags[itag]);
        if( sc.isFailure() ){
            (*m_log) << MSG::DEBUG << "Failed to retrieve TrigMissingETContainer with key " << METTags[itag] << endreq;
            continue;
        }
        
        (*m_log) << MSG::DEBUG << "Got TrigMissingETContainer with key \"" << METTags[itag]<< "\"" << endreq;
 
        TrigMissingETContainer::const_iterator trigMETfirst  = trigMETcont->begin();
        TrigMissingETContainer::const_iterator trigMETlast = trigMETcont->end();
 
        for (int j=0; trigMETfirst !=  trigMETlast;  trigMETfirst++, j++  ) {

            unsigned int Nc = (*trigMETfirst)->getNumOfComponents();

            if (itag == 0) {
                etMissL2->m_Ex     =  (*trigMETfirst)->ex();
                etMissL2->m_Ey     =  (*trigMETfirst)->ey();
                etMissL2->m_Ez     =  (*trigMETfirst)->ez();
                etMissL2->m_EtMiss =  (*trigMETfirst)->et();
                etMissL2->m_EtSum  =  (*trigMETfirst)->sumEt();
                etMissL2->m_SumE   =  (*trigMETfirst)->sumE();
                etMissL2->m_flag   =  (*trigMETfirst)->getFlag();
                (*m_log) << MSG::DEBUG << "Found L2 MET object with sum E :  " << etMissL2->m_SumE << endreq;
            } else if (itag == 1) {
                etMissEF->m_Ex     =  (*trigMETfirst)->ex();
                etMissEF->m_Ey     =  (*trigMETfirst)->ey();
                etMissEF->m_Ez     =  (*trigMETfirst)->ez();
                etMissEF->m_EtMiss =  (*trigMETfirst)->et();
                etMissEF->m_EtSum  =  (*trigMETfirst)->sumEt();
                etMissEF->m_SumE   =  (*trigMETfirst)->sumE();
                etMissEF->m_flag   =  (*trigMETfirst)->getFlag();
                (*m_log) << MSG::DEBUG << "Found EF MET object with sum E :  " << etMissEF->m_SumE << endreq;
            }    

            if (itag == 0 && Nc == 1) {// aux info: only muons are supported
                etMissL2->m_c_name         = (*trigMETfirst)->getNameOfComponent(0).c_str();
                etMissL2->m_c_status       = (*trigMETfirst)->getStatus(0);
                etMissL2->m_c_ex           = (*trigMETfirst)->getExComponent(0);
                etMissL2->m_c_ey           = (*trigMETfirst)->getEyComponent(0);
                etMissL2->m_c_ez           = (*trigMETfirst)->getEzComponent(0);
                etMissL2->m_c_sumEt        = (*trigMETfirst)->getSumEtComponent(0);
                etMissL2->m_c_sumE         = (*trigMETfirst)->getSumEComponent(0);
                etMissL2->m_c_calib0       = (*trigMETfirst)->getComponentCalib0(0);
                etMissL2->m_c_calib1       = (*trigMETfirst)->getComponentCalib1(0);
                etMissL2->m_c_sumOfSigns   = (*trigMETfirst)->getSumOfSigns(0);
                etMissL2->m_c_usedChannels = (*trigMETfirst)->getUsedChannels(0);
                
            } else if (itag == 1) {
                etMissEF->m_comp_number = Nc;
                for (unsigned int i=0; i<Nc; ++i) { // loop over components
                    etMissEF->m_c_name[i]         = (*trigMETfirst)->getNameOfComponent(i).c_str();
                    etMissEF->m_c_status[i]       = (*trigMETfirst)->getStatus(i);
                    etMissEF->m_c_ex[i]           = (*trigMETfirst)->getExComponent(i);
                    etMissEF->m_c_ey[i]           = (*trigMETfirst)->getEyComponent(i);
                    etMissEF->m_c_ez[i]           = (*trigMETfirst)->getEzComponent(i);
                    etMissEF->m_c_sumEt[i]        = (*trigMETfirst)->getSumEtComponent(i);
                    etMissEF->m_c_sumE[i]         = (*trigMETfirst)->getSumEComponent(i);
                    etMissEF->m_c_calib0[i]       = (*trigMETfirst)->getComponentCalib0(i);
                    etMissEF->m_c_calib1[i]       = (*trigMETfirst)->getComponentCalib1(i);
                    etMissEF->m_c_sumOfSigns[i]   = (*trigMETfirst)->getSumOfSigns(i);
                    etMissEF->m_c_usedChannels[i] = (*trigMETfirst)->getUsedChannels(i);
                }
            }
            
        }

    }

    return StatusCode::SUCCESS;
}


