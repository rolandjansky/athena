/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     T2L1CaloJetFullScanFastJetTool.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Matthew Tamsett <tamsett@cern.ch>
CREATED:  July, 2011

PURPOSE:  FastJet Tool for full scan reconstruction of jets from Lvl-1
          inputs.

********************************************************************/
#include "TrigT2CaloJet/T2L1CaloJetFullScanFastJetTool.h"
#include "TrigCaloEvent/TrigT2Jet.h"

#include "TrigT1CaloByteStream/ITrigT1CaloDataAccess.h"
#include "TrigT1CaloToolInterfaces/IL1JetTools.h"
#include "TrigT1CaloEvent/TriggerTower.h"

#include "fastjet/ClusterSequence.hh"

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/IToolSvc.h"


T2L1CaloJetFullScanFastJetTool::T2L1CaloJetFullScanFastJetTool(const std::string& type,
                    const std::string& name,
                    const IInterface* parent):
    T2L1CaloJetFullScanBaseTool(type, name, parent),
    m_dataL1("LVL1BS__TrigT1CaloDataAccess/TrigT1CaloDataAccess",0),
    m_jet(0),
    m_total_timer(0),
    m_unpacking_timer(0),
    m_load_collection_timer(0),
    m_jet_finding_timer(0),
    m_jet_def(0)
  
{
    declareInterface<T2L1CaloJetFullScanFastJetTool>( this );
    // configurables
    declareProperty("TrigT1CaloDataAccess", m_dataL1, "L1 Calo Data Access");
    declareProperty("doTriggerTowers",      m_doTriggerTowers = false);      
    declareProperty("coneRadius",           m_coneRadius = 0.4);
    declareProperty("pTmin",                m_pTmin = 5000.);
    declareProperty("inputType",            m_inputType = T2L1::NONE);
    // cleaning:
    declareProperty("doCleaning",           m_doCleaning = true);
    declareProperty("leadingCellFraction",  m_leadingCellFraction = 0.9);  // for jet cleaning, e.g.: n90 -> leadingCellFraction = 0.9
    // debug
    declareProperty("doJetFinding",         m_doJetFinding = true);
}


T2L1CaloJetFullScanFastJetTool::~T2L1CaloJetFullScanFastJetTool()
{
}

StatusCode T2L1CaloJetFullScanFastJetTool::initialize()
{ 
    m_log = new MsgStream(msgSvc(), name());
    
    
    (*m_log) << MSG::INFO << " Initalizing Tool: " << name () << endmsg;
    (*m_log) << MSG::DEBUG << "Options: " << endmsg;
    (*m_log) << MSG::DEBUG << "   doTriggerTowers:     " << m_doTriggerTowers << endmsg;
    (*m_log) << MSG::DEBUG << "   coneRadius:          " << m_coneRadius << endmsg;
    (*m_log) << MSG::DEBUG << "   pTmin:               " << m_pTmin << endmsg;
    (*m_log) << MSG::DEBUG << "   inputType (hex):     " << std::hex << m_inputType << std::dec << endmsg;
    (*m_log) << MSG::DEBUG << "   doCleaning:          " << m_doCleaning << endmsg;
    (*m_log) << MSG::DEBUG << "   leadingCellFraction: " << m_leadingCellFraction << endmsg;
    (*m_log) << MSG::DEBUG << "   doJetFinding:        " << m_doJetFinding << endmsg;
    
    // Initialize timing service
    if( service( "TrigTimerSvc", m_pTimerService).isFailure() ) {
        (*m_log) << MSG::WARNING << name() << ": Unable to locate TrigTimer Service" << endmsg;
    } 
    
    if (m_pTimerService){
        (*m_log) << MSG::DEBUG << " Adding timers" << endmsg;
        //Add timers
        m_total_timer            = m_pTimerService->addItem("LVL1_5_total_time");
        m_unpacking_timer        = m_pTimerService->addItem("LVL1_5_FS_unpacking_time");
        m_load_collection_timer  = m_pTimerService->addItem("LVL1_5_FS_load_collection_time");
        m_jet_finding_timer      = m_pTimerService->addItem("LVL1_5_FS_jet_finding_time");
        m_fastjet_timer          = m_pTimerService->addItem("LVL1_5_FS_fastjet_time");
    }
    
    // Create helper tools
    if ( m_dataL1.retrieve().isFailure() ) {
        (*m_log) << MSG::ERROR << "Failed to retreive helper tools: " << m_dataL1 << endmsg;
        m_retrievedJetTool = false;
    } else {
        m_retrievedJetTool = true;
        (*m_log) << MSG::DEBUG << "Retrieved " << m_dataL1 << endmsg;
    }
    
    // settings for Trigger tower retrieval
    m_etaMin = -5.;
    m_etaMax = 5.;
    m_phiMin = 0.;
    m_phiMax = 2 * M_PI;
    m_fullScan = true; 
    
    // initialise storage for l1 towers
    m_l1_tower_information.reserve(1000);
    m_cellenergies.reserve(100);
    
    // initialise fast jet
    m_particles.reserve(100);
    m_constituents.reserve(100);
    m_jets.reserve(100);
    
    if (m_doJetFinding){
        (*m_log) << MSG::INFO << " Setting up fastjet jet definition" << endmsg;
        m_jet_def = new fastjet::JetDefinition(fastjet::antikt_algorithm, m_coneRadius);
        // dummy call to fast jet so it's internal initalize methods are set (and we don't print the banner during execute)
        (*m_log) << MSG::INFO << " Making dummy call to fast jet cluster sequence" << endmsg;
        m_particles.clear();
        fastjet::ClusterSequence* cs(0);
        cs = new fastjet::ClusterSequence(m_particles, *m_jet_def);
        delete cs;
    }
    
    
    
    return StatusCode::SUCCESS;
}

StatusCode T2L1CaloJetFullScanFastJetTool::execute()
{
    return StatusCode::SUCCESS;
}


StatusCode T2L1CaloJetFullScanFastJetTool::execute(DataVector<TrigT2Jet>* output_t2_jets,int& n_towers,float& l1_unpack_time, float& all_unpack_time, float& fastjet_time, float& all_jet_finding_time)
{ 
    m_total_timer->reset();
    m_unpacking_timer->reset();
    m_load_collection_timer->reset();
    m_jet_finding_timer->reset();
    m_fastjet_timer->reset();
    m_total_timer->start();
    
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "===== Executing: T2L1CaloJetFullScanFastJetTool " << name() << endmsg;
    }
#endif
    
    // output_t2_jets->clear(); // this should be cleared when it arrives
    m_particles.clear();
    m_constituents.clear();
    m_jets.clear();
    m_l1_tower_information.clear(); 
    m_cellenergies.clear();
    
    n_towers = -9;
    all_unpack_time = -9;
    l1_unpack_time = -9;
    fastjet_time = -9;
    all_jet_finding_time = -9;
    // === now fill the particles vector, this is different for jet elements and trigger towers
    // eventually this could be split off into a seperate FEX to be reused by this, vl1 etc.
    m_unpacking_timer->start();
    
    int counter = 0;
    if ( m_doTriggerTowers ){
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG << "Trigger tower version" <<endmsg;
        }
#endif

        DataVector<LVL1::TriggerTower>::const_iterator tt_begj, tt_endj;
        
        // now get TT's    
        if ( m_retrievedJetTool ){
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
                (*m_log) << MSG::DEBUG   << "Loading the L1 data using ITrigT1CaloDataAccess tool"<< endmsg;
                (*m_log) << MSG::DEBUG   << "Getting trigger towers using m_etaMin: " << m_etaMin 
                         << ", etaMax: " << m_etaMax
                         << ", phiMin: " << m_phiMin
                         << ", phiMax: " << m_phiMax
                         << ", full: "   << m_fullScan << endmsg;
            }
#endif
            m_load_collection_timer->start();
            StatusCode sc = m_dataL1->loadCollection(tt_begj,tt_endj,m_etaMin,m_etaMax,m_phiMin,m_phiMax,m_fullScan);
            if (sc.isFailure()) {
                (*m_log) << MSG::WARNING << "Error accessing trigger tower data" << endmsg;
                return sc;
            }
            m_load_collection_timer->stop();
        }
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG  << "Loaded trigger towers in "<< m_load_collection_timer->elapsed() << " ms" << endmsg;
            (*m_log) << MSG::DEBUG  << "Looping over trigger towers"<< endmsg;
        }
#endif

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
            // change to a more standard phi definition
            float tt_phi = TT->phi();
            if( tt_phi >= M_PI ) tt_phi -= 2 * M_PI;
            float transverse_energy = (TT->emEnergy() + TT->hadEnergy())*1000.;
            float cosh_eta = cosh(TT->eta());
            float energy = transverse_energy*cosh_eta;
            // possibly do some calibration
        
            // now fill fast jet particles
            float px = transverse_energy*cos(tt_phi);
            float py = transverse_energy*sin(tt_phi);
            float pz = transverse_energy*sinh(TT->eta());
        
            if (m_doJetFinding){
                if (transverse_energy > 0.99){ // some Jet Elements have zero energy (this may also be true for trigger towers), there is no need to give these to fastjet
                    fastjet::PseudoJet pseudo_jet( px, py, pz, energy);
                    pseudo_jet.set_user_index(counter);
                    m_particles.push_back( pseudo_jet );
                }
        
                // now fill these into a vector of floats so we can construct cleaning variables later
                if (m_doCleaning) {
                    m_l1_tower_information.push_back(energy);
                    m_l1_tower_information.push_back(TT->eta());
                    m_l1_tower_information.push_back(TT->phi());
                    m_l1_tower_information.push_back(TT->emEnergy()*cosh_eta*1000.);
                    m_l1_tower_information.push_back(TT->hadEnergy()*cosh_eta*1000.);
                }
            }
        
            counter++;
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
            m_load_collection_timer->start();
            StatusCode sc = m_dataL1->loadCollection(je_begj,je_endj);
            if (sc.isFailure()) {
                (*m_log) << MSG::WARNING << "Error accessing jet element data" << endmsg;
                return sc;
            }
            m_load_collection_timer->stop();
        }

#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG  << "Loaded jet elements in "<< m_load_collection_timer->elapsed() << " ms" << endmsg;
            (*m_log) << MSG::DEBUG  << "Looping over the jet elements"<< endmsg;
        }
#endif

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
            
            // change to a more standard phi definition
            float je_phi = JE->phi();
            if( je_phi >= M_PI ) je_phi -= 2 * M_PI;
            float transverse_energy = JE->energy()*1000.;
            float cosh_eta = cosh(JE->eta());
            float energy = transverse_energy*cosh_eta;
            // possibly do some calibration
        
            // now fill fast jet particles
            float px = transverse_energy*cos(je_phi);
            float py = transverse_energy*sin(je_phi);
            float pz = transverse_energy*sinh(JE->eta());
            
            if (m_doJetFinding){
                if (JE->energy() > 0.99){ // some Jet Elements have zero energy, there is no need to give these to fastjet
                    fastjet::PseudoJet pseudo_jet( px, py, pz, energy);
                    pseudo_jet.set_user_index(counter);
                    m_particles.push_back( pseudo_jet );
                }                   
                // now fill these into a vector of floats so we can construct cleaning variables later
                if (m_doCleaning) {
                    m_l1_tower_information.push_back(energy);
                    m_l1_tower_information.push_back(JE->eta());
                    m_l1_tower_information.push_back(JE->phi());
                    m_l1_tower_information.push_back(JE->emEnergy()*cosh_eta*1000.);
                    m_l1_tower_information.push_back(JE->hadEnergy()*cosh_eta*1000.);
                }
            }
            counter++;
        }
    } // end of particle creation step
    m_unpacking_timer->stop();
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG  << "A total of " << m_particles.size()  << " L1 particles are to be clustered" << endmsg;
        if (m_doCleaning) { (*m_log) << MSG::DEBUG  << "A total of " << m_l1_tower_information.size()/5. << " are saved as L1 towers for the cleaning" << endmsg; }
    }
#endif
    // ===== now find the jets
    m_jet_finding_timer->start();
    if (m_doJetFinding){
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG <<  "Performing jet finding " << endmsg;
        }
#endif        
        // run the clustering, extract the jets
        m_fastjet_timer->start();
        fastjet::ClusterSequence* cs(0);
        cs = new fastjet::ClusterSequence(m_particles, *m_jet_def);
          
        m_jets = fastjet::sorted_by_pt(cs->inclusive_jets(m_pTmin)); // sorted by increasing pt
        m_fastjet_timer->stop();
        std::reverse(m_jets.begin(),m_jets.end());  // sorted by decreasing pt
          
        // inspect the output jets and make some cleaning variables
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG <<  "== Resultant jets " << endmsg;
        }
#endif
        for (unsigned i = 0; i < m_jets.size(); ++i) {
            m_constituents = fastjet::sorted_by_pt(cs->constituents(m_jets[i]));
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
               (*m_log) << MSG::DEBUG << " fastjet jet " << i 
                        << ": Et: "             << m_jets[i].e()/cosh(m_jets[i].eta()) 
                        << " MeV, eta: "        << m_jets[i].eta() 
                        << ", phi: "            << m_jets[i].phi_std() 
                        << ", rapidity: "       << m_jets[i].rapidity() 
                        << ", n constituents: "  << m_constituents.size() << endmsg;
            }
#endif
            int nLeadingCells = -9;
            float em_energy = -9;
            float em_frac = -9;
            float had_energy = -9;
            
            if (m_doCleaning) {
#ifndef NDEBUG
                if((*m_log).level() <= MSG::DEBUG){
                   (*m_log) << MSG::DEBUG  << "Computing cleaning variables "<< endmsg;
                }
#endif
                float total_energy = 0.;
                //float had_frac = 0.;
                em_energy = 0.;
                em_frac = 0.;
                had_energy = 0.;
                //had_frac = 0.;
                for (unsigned ii = 0; ii < m_constituents.size(); ++ii){
                    int l1_id = m_constituents[ii].user_index();
                    float energy = m_l1_tower_information.at((l1_id*5));
                    float emEnergy = m_l1_tower_information.at((l1_id*5)+3);
                    float hadEnergy = m_l1_tower_information.at((l1_id*5)+4);
        
#ifndef NDEBUG
                    float eta = m_l1_tower_information.at((l1_id*5)+1);      
                    float phi = m_l1_tower_information.at((l1_id*5)+2);
                    float transverse_energy = energy / cosh(eta);
                    
                    if((*m_log).level() <= MSG::DEBUG){
                       (*m_log) << MSG::DEBUG << "      constituent[" << std::setw(3) << ii 
                         << "]: Et: "         << std::setw(8) << m_constituents[ii].e()/cosh(m_constituents[ii].eta()) 
                         //<< " MeV (em,had) (" << std::setw(8) << emEnergy/cosh(eta)
                         //<<","                << std::setw(8) << hadEnergy/cosh(eta)
                         << ", eta: "         << std::setw(8) << m_constituents[ii].eta() 
                         << ", phi: "         << std::setw(8) << m_constituents[ii].phi_std() 
                         << ", rapidity: "    << std::setw(6) << m_constituents[ii].rapidity() 
                         << ", user index: "  << std::setw(5) << m_constituents[ii].user_index() << endmsg;
                         
                       (*m_log) << MSG::VERBOSE << "  L1 tower[" << l1_id 
                         << "]: Et: "          << transverse_energy
                         << " GeV (em,had) ("  << emEnergy / cosh(eta)
                         << ","                << hadEnergy / cosh(eta)
                         << "), energy: "      << energy
                         << ", eta: "          << eta 
                         << ", phi: "          << phi << endmsg;
                    }
#endif
            
                    // cleaning:        
                    total_energy+=energy;
                    em_energy+=emEnergy;
                    had_energy+=hadEnergy;      
                    m_cellenergies.push_back( energy );  
            
                }
            
                if (total_energy > 0.){
                    em_frac = em_energy / total_energy;
                    //had_frac= had_energy / total_energy;
                } 
                
                // sort cell-energy vector. (most energetic cell at end of vector)
                nLeadingCells = 0;
                std::sort(m_cellenergies.begin(), m_cellenergies.end());
                float running_energy_sum = 0.;
                for(int k = m_cellenergies.size()-1; k > -1; --k){
                    running_energy_sum += m_cellenergies[k];
                    nLeadingCells++;
                    if(running_energy_sum > m_leadingCellFraction*total_energy){
                        break;
                    }
                }
#ifndef NDEBUG
                if((*m_log).level() <= MSG::DEBUG){
                   (*m_log) << MSG::DEBUG << " total Et: " << total_energy/cosh(m_jets[i].eta()) 
                     << " MeV (em,had) ("   << em_energy/cosh(m_jets[i].eta()) 
                     << ","                 << had_energy/cosh(m_jets[i].eta()) 
                     << "), em frac: "      << em_frac 
                     << ", had frac: "      << (total_energy ? had_energy / total_energy : 0)
                     << ", nLeadingCells: " << nLeadingCells
                     << endmsg;
                }
#endif
            }

            // =========================
            //  Record the final jet
            // =========================
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
                 (*m_log) << MSG::DEBUG  << " making TrigT2Jet "<< endmsg;
            }
#endif
    
            m_jet = new TrigT2Jet();
                
            // Add RoI word to Jet
            int outputType = T2L1::UNKNOWN;
            if (m_doTriggerTowers){
                if (m_coneRadius==0.4){ 
                    outputType = T2L1::A4TT; 
                } else if (m_coneRadius==1.0){
                    outputType = T2L1::A10TT; 
                } else {
#ifndef NDEBUG
                    if((*m_log).level() <= MSG::DEBUG)
                        (*m_log) << MSG::DEBUG  << " unable to determine RoI type from m_coneRadius = " << m_coneRadius << ", setting output RoI type to UNKNOWN" << endmsg;
#endif
                    outputType = T2L1::UNKNOWN;
                }
            } else {
                if (m_coneRadius==0.4){ 
                    outputType = T2L1::A4JE; 
                } else if (m_coneRadius==1.0){
                    outputType = T2L1::A10JE; 
                } else {
#ifndef NDEBUG
                    if((*m_log).level() <= MSG::DEBUG)
                        (*m_log) << MSG::DEBUG  << " unable to determine RoI type from m_coneRadius = " << m_coneRadius << ", setting output RoI type to UNKNOWN" << endmsg;
#endif
                    outputType = T2L1::UNKNOWN;
                }
            }
                
            m_jet->set_RoIword(T2L1::BLANKWORD+T2L1::INPUT*m_inputType+T2L1::OUTPUT*outputType+i);
        
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG)
                (*m_log) << MSG::DEBUG  << " RoI word set to: " << std::hex << m_jet->RoIword() << std::dec << endmsg;
#endif
         
            // Set properties
            m_jet->setE(m_jets[i].e());
            m_jet->setEta(m_jets[i].eta());
            m_jet->setPhi(m_jets[i].phi_std());    
            m_jet->setEem0(em_energy);
            m_jet->setEhad0(had_energy);
            m_jet->setEmf(em_frac);
            m_jet->setNLeadingCells(nLeadingCells);
            
            // look at the jet produced
#ifndef NDEBUG
            if((*m_log).level() <= MSG::VERBOSE){
               (*m_log) << MSG::VERBOSE << " Values of T2CaloJet produced: " << endmsg;
               (*m_log) << MSG::VERBOSE << " REGTEST: Jet eta = " << m_jet->eta() << endmsg;
               (*m_log) << MSG::VERBOSE << " REGTEST: Jet phi = " << m_jet->phi() << endmsg;
               (*m_log) << MSG::VERBOSE << " REGTEST: Jet energy = " << m_jet->e() << endmsg;
               (*m_log) << MSG::VERBOSE << " REGTEST: Jet et = " << m_jet->et() << endmsg;
            }
#endif
            output_t2_jets->push_back(m_jet);
        } // end of loop on found jets
        
        // delete the fast jet cluster sequence
        delete cs;
    }
    m_jet_finding_timer->stop();
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG <<  "FastJet completed in "  << m_fastjet_timer->elapsed() << " ms " << endmsg;
        (*m_log) << MSG::DEBUG <<  "Jet finding completed in "  << m_jet_finding_timer->elapsed() << " ms " << endmsg;
    }
#endif
    m_total_timer->stop();
    // fill monitoring variables
    n_towers = counter;
    l1_unpack_time = m_load_collection_timer->elapsed();
    all_unpack_time = m_unpacking_timer->elapsed();
    fastjet_time = m_fastjet_timer->elapsed();
    all_jet_finding_time = m_jet_finding_timer->elapsed();
  
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "================= Finished T2L1CaloFullScanFastJetTool " << name() << " in " << m_total_timer->elapsed() << " ms " << endmsg;
    }
#endif

    return StatusCode::SUCCESS;
}


StatusCode T2L1CaloJetFullScanFastJetTool::finalize()
{
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << " Finalising: T2L1CaloJetFullScanFastJetTool " << name() << endmsg;
    }
    delete m_log;
    m_particles.clear();
    m_jets.clear();
    m_constituents.clear();
    m_l1_tower_information.clear();
    m_cellenergies.clear();
    
    if ( bool(m_jet_def) ){
        delete m_jet_def;
    }
    return StatusCode::SUCCESS;
}




