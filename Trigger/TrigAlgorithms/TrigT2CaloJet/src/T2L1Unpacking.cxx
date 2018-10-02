/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2L1Unpacking.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet
//
// AUTHOR:   Matthew Tamsett, tamsett@cern.ch
//
// Description: Level 1.5 unpacking FEX algorithm
// ********************************************************************

#include "TrigT2CaloJet/T2L1Unpacking.h"
#include "TrigCaloEvent/Trig3Momentum.h"
#include "TrigCaloEvent/TrigT2Jet.h"
//#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "TrigT2CaloJet/T2CaloJetBaseTool.h"

#include "TrigT1CaloByteStream/ITrigT1CaloDataAccess.h"
#include "TrigT1CaloToolInterfaces/IL1JetTools.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"



T2L1Unpacking::T2L1Unpacking(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
  m_tools(this),
  //m_total_timer(0),
  m_l1_unpacking_timer(0),
  m_unpacking_timer(0),
  m_calibration_timer(0),
  m_RoI_timer(0),
  m_dataL1("LVL1BS__TrigT1CaloDataAccess/TrigT1CaloDataAccess",0),
  m_jet(0),
  m_grid_element(0)
{   
    // configurables
    declareProperty("TrigT1CaloDataAccess", m_dataL1, "L1 Calo Data Access");
    declareProperty("jetOutputKey",         m_jetOutputKey = "T2L1Towers");
    declareProperty("doTriggerTowers",      m_doTriggerTowers = false); 
    declareProperty("T2JetTools",           m_tools, "list of Jet tools");
        
    // monitoring
    declareMonitoredVariable("L1UnpckTime",    m_L1UnpckTime);
    declareMonitoredVariable("UnpckTime",      m_UnpckTime);
    declareMonitoredVariable("CalibrationTime",m_CalibrationTime);
    declareMonitoredVariable("RoITime",        m_RoITime);
    //declareMonitoredVariable("OverallTime",    m_TotalTime);
    declareMonitoredVariable("nTowers",        m_nTowers);
    declareMonitoredVariable("nEMTowers",      m_nEMTowers);
    declareMonitoredVariable("nHADTowers",     m_nHADTowers);
    declareMonitoredVariable("nTowers_zoom",   m_nTowers);
    declareMonitoredVariable("nEMTowers_zoom", m_nEMTowers);
    declareMonitoredVariable("nHADTowers_zoom",m_nHADTowers);
    declareMonitoredStdContainer("Et",         m_et);
    declareMonitoredStdContainer("EtEM",       m_et_em);
    declareMonitoredStdContainer("EtHAD",      m_et_had);
    declareMonitoredStdContainer("Eta",        m_eta);
    declareMonitoredStdContainer("EtaEM",      m_eta_em);
    declareMonitoredStdContainer("EtaHAD",     m_eta_had);
    declareMonitoredStdContainer("Phi",        m_phi);
    declareMonitoredStdContainer("PhiEM",      m_phi_em);
    declareMonitoredStdContainer("PhiHAD",     m_phi_had);
    
}

HLT::ErrorCode T2L1Unpacking::hltInitialize()
{
    m_log = new MsgStream(msgSvc(), name());
    
    if((*m_log).level() <= MSG::INFO){
        (*m_log) << MSG::INFO << " Initalizing FEX algorithm: " << name() << endmsg;
        //(*m_log) << MSG::DEBUG << "Options: " << endmsg;
    }
    
   // Initialize timing service
    if( service( "TrigTimerSvc", m_pTimerService).isFailure() ) {
        (*m_log) << MSG::WARNING << name() << ": Unable to locate TrigTimer Service" << endmsg;
    } 
    
    if (m_pTimerService){
        (*m_log) << MSG::DEBUG << " Adding timers" << endmsg;
        //Add timers
        //m_total_timer        = addTimer("total_time");
        m_l1_unpacking_timer = addTimer("l1_unpacking_time");
        m_unpacking_timer    = addTimer("unpacking_time");
        m_calibration_timer  = addTimer("calibration_time");
        m_RoI_timer          = addTimer("RoI_time");
    }
    
    // Create helper tools
    if ( m_dataL1.retrieve().isFailure() ) {
        (*m_log) << MSG::ERROR << "Failed to retreive helper tools: " << m_dataL1 << endmsg;
        m_retrievedJetTool = false;
    } else {
        m_retrievedJetTool = true;
        (*m_log) << MSG::DEBUG << "Retrieved " << m_dataL1 << endmsg;
    }
    
    if ( m_tools.retrieve().isFailure() ) {
        (*m_log) << MSG::ERROR << "Failed to retreive helper tools: " << m_tools << endmsg;
    } else {
        (*m_log) << MSG::INFO << "Retrieved " << m_tools << endmsg;
    }
    
    // settings for Trigger tower retrieval
    m_etaMin = -5.;
    m_etaMax = 5.;
    m_phiMin = 0.;
    m_phiMax = 2 * M_PI;
    m_fullScan = true; 
 
    /// aha! fullscan should be from -pi to pi
   
    // output
    m_grid_element = new Trig3Momentum();
    
    
    
    return HLT::OK;
}

T2L1Unpacking::~T2L1Unpacking(){
}

HLT::ErrorCode T2L1Unpacking::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*tes_in*/,
                                               unsigned int type_out)
{
    // since this is an AllTEAlgo, we have to call the monitoring ourselves:
    beforeExecMonitors().ignore();
    
    // ==== pre-execute setup
    if(doTiming()) {
        //m_total_timer->reset();
        m_l1_unpacking_timer->reset();
        m_unpacking_timer->reset();
        m_calibration_timer->reset();
        m_RoI_timer->reset();
        //m_total_timer->start();
    }
    m_nTowers = -99;
    m_nEMTowers = -99;
    m_nHADTowers = -99;
    m_UnpckTime = -99;
    m_RoITime = -99;
    //m_TotalTime = -99;
    m_et.clear();
    m_et_em.clear();
    m_et_had.clear();
    m_eta.clear();
    m_eta_em.clear();
    m_eta_had.clear();
    m_phi.clear();
    m_phi_em.clear();
    m_phi_had.clear();
    
   
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "================= Executing T2L1Unpacking FEX " << name() << endmsg;
        
    }
#endif
    
    // === setup the output trigger element
    if(doTiming()) m_RoI_timer->start();
    
    TrigRoiDescriptor* roi = new TrigRoiDescriptor( true );

    HLT::TriggerElement* outputTE = addRoI(type_out, roi  ); /// NB: a fullscan RoI !!!
    outputTE->setActiveState(true); 
    if(doTiming()) m_RoI_timer->stop();
    
    if(doTiming()) m_unpacking_timer->start();
    std::vector<Trig3Momentum>* grid = new std::vector<Trig3Momentum>();
    grid->reserve(8000);
    
    // === unpacking    
    int counter = 0;
    uint16_t tower_by_sampling_counter = 0;
    int recorded_counter = 0;
    int EM_recorded_counter = 0;
    int HAD_recorded_counter = 0;
    if ( m_doTriggerTowers ){
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG << "Trigger tower version" <<endmsg;
        }
#endif
        // now get TT's    
        DataVector<LVL1::TriggerTower>::const_iterator tt_begj, tt_endj;
        if ( m_retrievedJetTool ){        
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
                (*m_log) << MSG::DEBUG   << "Loading the L1 data using ITrigT1CaloDataAccess tool"<< endmsg;
		(*m_log) << MSG::DEBUG   << "Getting trigger towers using "
			 << " roi: " << *roi 
		   //         << "  etaMin: " << m_etaMin 
		   //         << ", etaMax: " << m_etaMax
		   //         << ", phiMin: " << m_phiMin
		   //         << ", phiMax: " << m_phiMax
                         << ", full: "   << m_fullScan << endmsg;
            }
#endif
            if(doTiming()) m_l1_unpacking_timer->start();
            StatusCode sc = m_dataL1->loadCollection( tt_begj, tt_endj, 
						      roi->etaMinus(), roi->etaPlus(),
						      roi->phiMinus(), roi->phiPlus(), roi->isFullscan() );
            if(doTiming()) m_l1_unpacking_timer->stop();
            if (sc.isFailure()) {
                (*m_log) << MSG::WARNING << "Error accessing trigger tower data" << endmsg;
                return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,HLT::Reason::USERDEF_1);
            }
            
#ifndef NDEBUG     
            if((*m_log).level() <= MSG::DEBUG){
                (*m_log) << MSG::DEBUG  << "Loaded trigger towers in "<< m_l1_unpacking_timer->elapsed() << " ms" << endmsg;
                (*m_log) << MSG::DEBUG  << "Looping over trigger towers"<< endmsg;
            }
#endif
        }
        for(; tt_begj!=tt_endj;++tt_begj){
            const LVL1::TriggerTower* TT = (*tt_begj);
#ifndef NDEBUG   
            if((*m_log).level() <= MSG::DEBUG){
                (*m_log) << MSG::DEBUG  << "TriggerTower[" << counter
                         << "]: ETs [GeV]: EM: " << TT->emEnergy()
                         << ", HAD: "            << TT->hadEnergy()
                         << ", eta: "            << TT->eta()
                         << ", phi: "            << TT->phi() << endmsg;
            }
#endif    
            counter++;
            // To remove ambiguities due to degenerate distance measures we deform the calorimeter: 
            // low eta, low phi cells are marginally closer together than high eta, high phi cells.
            //
            // It's best to use slightly different deformations for eta and phi.
            // taken from recomendation 5. from here: http://fastjet.fr/FAQ.html
            double tt_eta = TT->eta() + 1.000000e-6 * pow(TT->eta(),2);
            double tt_phi = TT->phi() + 1.047198e-6 * pow(TT->phi(),2);            
            // change to a more standard phi definition
            if( tt_phi >= M_PI ) tt_phi -= 2 * M_PI;
            double transverse_energy = (TT->emEnergy() + TT->hadEnergy())*1000.;
            //double cosh_eta = cosh(tt_eta);
            //double energy = transverse_energy*cosh_eta;
            // possibly do some calibration
        
            
            if (transverse_energy > 0.99){ // some Jet Elements have zero energy (this may also be true for trigger towers), there is no need to give these to fastjet
                // Set properties, EM and HAD seperately
                // EM
                if (TT->emEnergy()){                 
                    tower_by_sampling_counter++;
                    double this_eta = determineTriggerTowerEta(tt_eta,true);
                    double this_cosh_eta = cosh(this_eta);
                    m_grid_element->setE((TT->emEnergy()*1000.)*this_cosh_eta);
                    m_grid_element->setEta(this_eta); // map this eta to a physical eta
                    m_grid_element->setPhi(tt_phi);    
                    CaloSampling::CaloSample sampling = determineCaloSampling(fabs(tt_eta), true);
                    m_grid_element->setCaloSample(sampling);
                    if (TT->hadEnergy()){
                        // record provenance of this jet 
                        // in this context this means that if the tower was split into two we keep the index of its other half
                        m_grid_element->setProvenance(tower_by_sampling_counter);
                    } else {
                        m_grid_element->setProvenance(uint16_t(9999));
                    }
                    
                    grid->push_back(*m_grid_element);      
                    EM_recorded_counter++;
                    // Monitor
                    m_et_em.push_back( TT->emEnergy()*1000. ); 
                    m_eta_em.push_back( m_grid_element->eta() ); 
                    m_phi_em.push_back( m_grid_element->phi() ); 
                    
                }
                // HAD
                if (TT->hadEnergy()){
                    tower_by_sampling_counter++;
                    double this_eta = determineTriggerTowerEta(tt_eta,false);
                    double this_cosh_eta = cosh(this_eta);
                    m_grid_element->setE((TT->hadEnergy()*1000.)*this_cosh_eta);
                    m_grid_element->setEta(this_eta); // map this eta to a physical eta
                    m_grid_element->setPhi(tt_phi);    
                    CaloSampling::CaloSample sampling = determineCaloSampling(fabs(tt_eta), false);
                    m_grid_element->setCaloSample(sampling);
                    if (TT->emEnergy()){
                        // record provenance of this jet
                        // in this context this means that if the tower was split into two we keep the index of its other half
                        m_grid_element->setProvenance(tower_by_sampling_counter-2);
                    } else {
                        m_grid_element->setProvenance(uint16_t(9999));
                    }
                    
                    grid->push_back(*m_grid_element);      
                    HAD_recorded_counter++;
                    // Monitor
                    m_et_had.push_back( TT->hadEnergy()*1000. ); 
                    m_eta_had.push_back( m_grid_element->eta() ); 
                    m_phi_had.push_back( m_grid_element->phi() ); 
                }
                
                // Monitor
                recorded_counter++;
                m_et.push_back( transverse_energy ); 
                m_eta.push_back( tt_eta ); 
                m_phi.push_back( tt_phi ); 
                
            }
        }
    } else { // if we aren't running on trigger towers we use jet elements instead    
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG << "Jet element version" <<endmsg;
        }
#endif
        DataVector<LVL1::JetElement>::const_iterator je_begj, je_endj;
    
        if ( m_retrievedJetTool ){
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
                (*m_log) << MSG::DEBUG  << "Loading the L1 data using ITrigT1CaloDataAccess tool, this loads the jet elements for the entire event"<< endmsg;
            }
#endif
            if(doTiming()) m_l1_unpacking_timer->start();
            StatusCode sc = m_dataL1->loadCollection(je_begj,je_endj);
            if(doTiming()) m_l1_unpacking_timer->stop();
            if (sc.isFailure()) {
                (*m_log) << MSG::WARNING << "Error accessing jet element data" << endmsg;
                return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,HLT::Reason::USERDEF_1);
            }
            
#ifndef NDEBUG     
            if((*m_log).level() <= MSG::DEBUG){
                (*m_log) << MSG::DEBUG  << "Loaded jet elements in "<< m_l1_unpacking_timer->elapsed() << " ms" << endmsg;
                (*m_log) << MSG::DEBUG  << "Looping over jet elements"<< endmsg;
            }
#endif
        }
        
        for(; je_begj!=je_endj;++je_begj){
            const LVL1::JetElement* JE = (*je_begj); 
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
               (*m_log) << MSG::DEBUG  << "JetElement[" << counter
                        << "]: ETs [GeV]: EM: " << JE->emEnergy()
                        << ", HAD: "            << JE->hadEnergy()
                        << ", Total: "          << JE->energy()
                        << ", eta: "            << JE->eta()
                        << ", phi: "            << JE->phi() << endmsg;
            }        
#endif
            counter++;
            // To remove ambiguities due to degenerate distance measures we deform the calorimeter: 
            // low eta, low phi cells are marginally closer together than high eta, high phi cells.
            //
            // It's best to use slightly different deformations for eta and phi.
            // taken from recomendation 5. from here: http://fastjet.fr/FAQ.html
            double je_eta = JE->eta() + 1.000000e-6 * pow(JE->eta(),2);
            double je_phi = JE->phi() + 1.047198e-6 * pow(JE->phi(),2);            
            // change to a more standard phi definition
            if( je_phi >= M_PI ) je_phi -= 2 * M_PI;
            double transverse_energy = (JE->energy())*1000.;
            double cosh_eta = cosh(je_eta);
            //double energy = transverse_energy*cosh_eta;
            // possibly do some calibration
        
            
            if (transverse_energy > 0.99){ // some Jet Elements have zero energy, there is no need to give these to fastjet
                // Set properties, EM and HAD seperately
                // EM
                if (JE->emEnergy()){
                    tower_by_sampling_counter++;
                    m_grid_element->setE((JE->emEnergy()*1000.)*cosh_eta);
                    m_grid_element->setEta(je_eta);
                    m_grid_element->setPhi(je_phi);    
                    CaloSampling::CaloSample sampling = determineCaloSampling(fabs(je_eta), true);
                    m_grid_element->setCaloSample(sampling);
                    if (JE->hadEnergy()){
                        // record provenance of this jet
                        // in this context this means that if the tower was split into two we keep the index of its other half
                        m_grid_element->setProvenance(tower_by_sampling_counter);
                    } else {
                        m_grid_element->setProvenance(uint16_t(9999));
                    }
                    
                    grid->push_back(*m_grid_element);      
                    EM_recorded_counter++;
                    // Monitor
                    m_et_em.push_back( JE->emEnergy()*1000. ); 
                    m_eta_em.push_back( m_grid_element->eta() ); 
                    m_phi_em.push_back( m_grid_element->phi() ); 
                    
                }
                // HAD
                if (JE->hadEnergy()){
                    tower_by_sampling_counter++;
                    m_grid_element->setE((JE->hadEnergy()*1000.)*cosh_eta);
                    m_grid_element->setEta(je_eta);
                    m_grid_element->setPhi(je_phi);   
                    CaloSampling::CaloSample sampling = determineCaloSampling(fabs(je_eta), false);
                    m_grid_element->setCaloSample(sampling); 
                    if (JE->emEnergy()){
                        // record provenance of this jet
                        // in this context this means that if the tower was split into two we keep the index of its other half
                        m_grid_element->setProvenance(tower_by_sampling_counter-2);
                    } else {
                        m_grid_element->setProvenance(uint16_t(9999));
                    }
                    
                    grid->push_back(*m_grid_element);      
                    HAD_recorded_counter++;
                    // Monitor
                    m_et_had.push_back( JE->hadEnergy()*1000. ); 
                    m_eta_had.push_back( m_grid_element->eta() ); 
                    m_phi_had.push_back( m_grid_element->phi() ); 
                }
                
                // Monitor
                recorded_counter++;
                m_et.push_back( transverse_energy ); 
                m_eta.push_back( je_eta ); 
                m_phi.push_back( je_phi ); 
            }
        }
    }
            
    // monitor
    //m_nTowers = outJets->size();
    m_nTowers = recorded_counter;
    m_nEMTowers = EM_recorded_counter;
    m_nHADTowers = HAD_recorded_counter;
    if(doTiming()) m_unpacking_timer->stop();
    
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG  << "A total of " << grid->size()  << " L1 particles are to be clustered" << endmsg;
    }
    
    if((*m_log).level() <= MSG::VERBOSE){        
        std::vector<Trig3Momentum>::const_iterator begin_jet, end_jet; // iterators for the produced jets
        begin_jet = grid->begin();
        end_jet = grid->end();        
        int jet_counter = 0;
        for (; begin_jet != end_jet; ++begin_jet){
            (*m_log) << MSG::VERBOSE << "Trig3Momentum["  <<jet_counter
                                     << "]: Et: "         << begin_jet->e()/cosh(begin_jet->eta())
                                     << " MeV, "
                                     << "eta: "           << begin_jet->eta() 
                                     << ", phi: "         << begin_jet->phi() 
                                     << ", sampling: "    << std::hex << begin_jet->caloSample() << std::dec 
                                     << ", provenance: "  << begin_jet->provenance() << endmsg;
            jet_counter++;
        }
    }
#endif  
     
    if(doTiming()) m_RoI_timer->start();
    m_jet = new TrigT2Jet();
    m_jet->setGrid(grid);
    //// RoI word
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG  << "Making TrigT2Jet to save" << endmsg;
    }
#endif  
    if(doTiming()) m_RoI_timer->pause();
    
    // do loop over tools, e.g tower level correction tool
    // === calibration 
    if(doTiming()) m_calibration_timer->start();
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
       (*m_log) << MSG::DEBUG  << m_tools.size() << " calibration tools to be run"<< endmsg;
       
    }
#endif

    ToolHandleArray< T2CaloJetBaseTool >::iterator it = m_tools.begin(),
    itToolEnd = m_tools.end();
    int current=1;
    for (; it != itToolEnd; ++it) {
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
           (*m_log) << MSG::DEBUG  << "Executing tool [" << current << "]"<< endmsg;
        }
#endif
        if ((*it)->execute(m_jet, TrigRoiDescriptor(true), caloDDENull ).isFailure()){    // the zeros are the unused eta phi coordinates used by many base tools to define the RoI region
            msg() << MSG::WARNING << "T2CaloFastJet AlgToolJets returned Failure" << endmsg;
            return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,HLT::Reason::USERDEF_1);
        }
        current++;
    }
    if(doTiming()) m_calibration_timer->stop();
    
    // === finaly recored the output jet
    if(doTiming()) m_RoI_timer->resume();
    std::string key = "";
    HLT::ErrorCode stat = recordAndAttachFeature(outputTE, m_jet, key, m_jetOutputKey);
    if (stat != HLT::OK){
        (*m_log) << MSG::ERROR << "recording of TrigT2Jets into StoreGate failed" << endmsg;
        return stat;
    }
    if(doTiming()) m_RoI_timer->stop();
    
    
    // === post-execute admin
    if(doTiming()) {
        //m_total_timer->stop();
        m_L1UnpckTime = m_l1_unpacking_timer->elapsed();
        m_UnpckTime = m_unpacking_timer->elapsed();
        m_RoITime = m_RoI_timer->elapsed();
        //m_TotalTime = m_total_timer->elapsed();
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG << "L1 unpacking completed in  " << m_l1_unpacking_timer->elapsed() << " ms " << endmsg;
            (*m_log) << MSG::DEBUG << "All unpacking completed in " << m_unpacking_timer->elapsed()    << " ms " << endmsg;
            (*m_log) << MSG::DEBUG << "Calibration completed in   " << m_calibration_timer->elapsed()  << " ms " << endmsg;
            (*m_log) << MSG::DEBUG << "RoI making completed in    " << m_RoI_timer->elapsed()          << " ms " << endmsg;
            //(*m_log) << MSG::DEBUG << "T2L1Unpacking completed in " << m_total_timer->elapsed()        << " ms " << endmsg;
        }
#endif
    }
    
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "================= Finished T2L1Unpacking " << name() << endmsg;
    }
#endif
    
    // since this is an AllTEAlgo, we have to call the monitoring ourselves:
    afterExecMonitors().ignore();
    
    return HLT::OK;
}


HLT::ErrorCode T2L1Unpacking::hltFinalize()
{
    if ( (*m_log).level() <= MSG::DEBUG )
        (*m_log) << MSG::DEBUG << "Finalizing T2L1Unpacking FEX " << name() << endmsg;
    
    m_et.clear();
    m_et_em.clear();
    m_et_had.clear();
    m_eta.clear();
    m_eta_em.clear();
    m_eta_had.clear();
    m_phi.clear();
    m_phi_em.clear();
    m_phi_had.clear();
    delete m_grid_element;
    m_grid_element = 0;
    
    delete m_log;
          
    return HLT::OK;
}

double T2L1Unpacking::determineTriggerTowerEta(double eta, bool isEM)
{
    /* 
    Determines a more realistic eta coordinate for the forward trigger towers
    
    The mapping used is:
    * "EM"
    ----
    Tower "eta" :  -4.7  -4.3  -3.8  -3.4   3.4   3.8   4.3   4.7
    Physical eta:  -4.45 -3.75 -3.35 -3.15  3.15  3.35  3.75  4.45
    Module      :  FCAL1 FCAL1 FCAL1 FCAL1 FCAL1 FCAL1 FCAL1 FCAL1
    
    * "Had":
    ------
    Tower "eta" :  -4.7  -4.3  -3.8  -3.4   3.4   3.8  4.3   4.7
    Physical eta:  -4.2  -4.2  -3.4  -3.4   3.4   3.4  4.2   4.2
    Module      :  FCAL2 FCAL3 FCAL2 FCAL3 FCAL2 FCAL3 FCAL2 FCAL3
    
    You'll note that each L1 tower is either FCAL2 or FCAL3, not the sum of the two.
    */
    
    double fabs_eta = fabs(eta);
    
    if (fabs_eta < 3.3) return eta; // for non-forward towers return eta
    
    double sign_eta = 1;
    if (eta < 0.) sign_eta = -1;
    
    if (fabs_eta > 4.5){
        if (isEM) return sign_eta*4.45;
        return sign_eta*4.2;
                  
    } else if (fabs_eta > 4.1){
        if (isEM) return sign_eta*3.75;
        return sign_eta*4.2;
                  
    } else if (fabs_eta > 3.6){
        if (isEM) return sign_eta*3.35;
        return sign_eta*3.4;
                  
    } else {
        if (isEM) return sign_eta*3.15;
        return sign_eta*3.4;
    }
}

CaloSampling::CaloSample T2L1Unpacking::determineCaloSampling(double fabs_eta, bool isEM)
{
    /*
    Determine the calo sampling of trigger towers and jet elements based on their eta
    
    For Trigger towers this mapping is
        EM layer:  |eta| =  0  - 1.5   LArg EM Barrel
                   |eta| = 1.4 - 3.2   LArg EM Endcap: note the overlap here
    
        Had layer: |eta| =  0  - 0.9   Tile Long Barrel
                   |eta| = 0.9 - 1.5   Tile Extended Barrel
                   |eta| = 1.5 - 3.2   LArg Hadronic Endcap
    
        FCAL:      anything above |eta| = 3.2
     
    In addition, for jet elements:    
        [em]  |eta| = 1.4-1.6 spans EMB+EMEC (1.5 in JE eta)
        
        [had] |eta| = 0.8-1.0 partly tile barrel, partly extended barrel (0.9 in JE eta)
              |eta| = 1.4-1.6 Tile-HEC transition (1.5 in JE eta)
        
        [em?] |eta| =  2.4-2.7, i.e. the OW+IW boundary. (2.55 in JE eta)
     
    As these are summed over layers we'll return the middle sampling
    */
    
    if (fabs_eta > 3.2) {                     // FCAL
        if (isEM) return CaloSampling::FCAL0; // EM FCAL
        return CaloSampling::FCAL1;           // HAD FCAL
    } else if (isEM && (fabs_eta < 1.4)) {    // EM, cut at 1.4 assigns overlap region to the EME
        return CaloSampling::EMB1;
    } else if (isEM) {
        return CaloSampling::EME1;
    } else if (fabs_eta < 0.8) {              // HAD, cut at 0.8 assigns overlap region to TileExt
        return CaloSampling::TileBar1;
    } else if (fabs_eta < 1.4) {              // cut at 1.4 assigne overlap region to HEC
        return CaloSampling::TileExt1;
    } else {
        return CaloSampling::HEC1;
    }
}
