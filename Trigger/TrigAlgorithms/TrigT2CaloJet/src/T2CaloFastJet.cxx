/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2CaloFastJet.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet
//
// AUTHOR:   Matthew Tamsett, tamsett@cern.ch
//
// Description: Level 2 FastJet FEX algorithm.
// ********************************************************************
#include "TrigT2CaloJet/T2CaloFastJet.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "TrigT2CaloJet/T2CaloJetBaseTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include <algorithm>
#include <iterator>

T2CaloFastJet::T2CaloFastJet(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_cellMinEnergy(0.),
    m_tools(this),
    m_pTimerService(0),
    //m_total_timer(0),
    m_unpack_timer(0),
    m_fastjet_timer(0),
    m_pu_subtraction_timer(0),
    m_cleaning_timer(0),
    m_calibration_timer(0),
    m_RoI_timer(0),
    m_jet_def(0),
    m_cluster_sequence(0),
    m_pu_cluster_sequence(0),
    m_pu_area_cluster_sequence(0),
    m_jet(0)
{   
    // configurables
    declareProperty("jetInputKey",             m_jetInputKey  = "T2L1Towers");
    declareProperty("jetOutputKey",            m_jetOutputKey = "TrigT2CaloJet");
    declareProperty("distanceParameter",       m_distanceParameter = 0.4);
    declareProperty("pTmin",                   m_pTmin = 10000.);
    declareProperty("inputType",               m_inputType = 1);
    declareProperty("outputType",              m_outputType = 1);
    declareProperty("T2JetTools",              m_tools, "list of Jet tools");
    declareProperty("cellMinEnergy", m_cellMinEnergy=0.0);
    // cleaning:
    declareProperty("doCleaning",              m_doCleaning = false);
    declareProperty("doT2L1Cleaning",          m_doT2L1Cleaning = true);
    declareProperty("cellQualityThresholdLAr", m_cellQualityThresholdLAr = 4000);  
    declareProperty("leadingCellFraction",     m_leadingCellFraction = 0.9);  // for jet cleaning, e.g.: n90 -> leadingCellFraction = 0.9
    // navigation
    declareProperty("writeMultipleOutputTEs",  m_writeMultipleOutputTEs = false);
    declareProperty("secondOutputType",        m_secondOutputType = 0); 
    
    // pileup subtraction
    declareProperty("doPileupSubtraction",     m_doPileupSubtraction = false);
    
    // monitoring
    //declareMonitoredVariable("OverallTime",    m_TotalTime);
    declareMonitoredVariable("UnpackTime",     m_UnpackTime);
    declareMonitoredVariable("FastJetTime",    m_FastJetTime);
    declareMonitoredVariable("puSubTime",      m_puSubTime);
    declareMonitoredVariable("CleaningTime",   m_CleaningTime);
    declareMonitoredVariable("CalibrationTime",m_CalibrationTime);
    declareMonitoredVariable("RoITime",        m_RoITime);
    declareMonitoredVariable("nJets",          m_nJets);
    declareMonitoredVariable("nGrid",          m_nGrid);
    declareMonitoredStdContainer("Et",         m_et);
    declareMonitoredStdContainer("EtEM",       m_et_em);
    declareMonitoredStdContainer("EtHAD",      m_et_had);
    declareMonitoredStdContainer("E",          m_e);
    declareMonitoredStdContainer("EEM",        m_e_em);
    declareMonitoredStdContainer("EHAD",       m_e_had);
    declareMonitoredStdContainer("Eta",        m_eta);
    declareMonitoredStdContainer("Phi",        m_phi);
    declareMonitoredStdContainer("Emf",        m_em_frac);
    declareMonitoredStdContainer("nLeadingTowers", m_nLeadingTowers);
}

HLT::ErrorCode T2CaloFastJet::hltInitialize()
{
    m_log = new MsgStream(msgSvc(), name());
    
    if((*m_log).level() <= MSG::INFO){
        (*m_log) << MSG::INFO << " Initalizing FEX algorithm: " << name() << endmsg;
        (*m_log) << MSG::DEBUG << "Options: " << endmsg;
        (*m_log) << MSG::DEBUG << "   distanceParameter:       " << m_distanceParameter << endmsg;
        (*m_log) << MSG::DEBUG << "   pTmin:                   " << m_pTmin << endmsg;
        (*m_log) << MSG::DEBUG << "   doCleaning:              " << m_doCleaning << endmsg;
        (*m_log) << MSG::DEBUG << "   doT2L1Cleaning:          " << m_doT2L1Cleaning << endmsg;
        (*m_log) << MSG::DEBUG << "   leadingCellFraction:     " << m_leadingCellFraction << endmsg;
        (*m_log) << MSG::DEBUG << "   cellQualityThresholdLAr: " << m_cellQualityThresholdLAr << endmsg;
        (*m_log) << MSG::DEBUG << "   input type:              " << m_inputType << endmsg;
        (*m_log) << MSG::DEBUG << "   output type:             " << m_outputType << endmsg;
        (*m_log) << MSG::DEBUG << "   write multiple TEs:      " << m_writeMultipleOutputTEs << endmsg;
        (*m_log) << MSG::DEBUG << "   secondary output type:   " << m_secondOutputType << endmsg;
        (*m_log) << MSG::DEBUG << "   do pileup subtraction:   " << m_doPileupSubtraction << endmsg;
        
    }
        
    // initalise T2L1 helper tools
    m_t2l1_tools = T2L1Tools();    
    
    // Create helper tools
    if ( m_tools.retrieve().isFailure() ) {
        (*m_log) << MSG::ERROR << "Failed to retreive helper tools: " << m_tools << endmsg;
    } else {
        (*m_log) << MSG::INFO << "Retrieved " << m_tools << endmsg;
    }    
    
    // Initialize timing service
    if( service( "TrigTimerSvc", m_pTimerService).isFailure() ) {
        (*m_log) << MSG::WARNING << name() << ": Unable to locate TrigTimer Service" << endmsg;
    } 
    
    if (m_pTimerService){
        (*m_log) << MSG::DEBUG << "Adding timers" << endmsg;
        //Add timers
        //m_total_timer          = addTimer("total_time");
        m_unpack_timer         = addTimer("unpack_time");
        m_fastjet_timer        = addTimer("fastjet_time");
        m_pu_subtraction_timer = addTimer("pu_subtraction_time");
        m_cleaning_timer       = addTimer("cleaing_time");
        m_calibration_timer    = addTimer("calibration_time");
        m_RoI_timer            = addTimer("roi_time");
    }    
    
    // initialise fast jet
    m_particles.reserve(100);
    m_constituents.reserve(100);
    m_jets.reserve(100);
    
    (*m_log) << MSG::INFO << "Setting up fastjet jet definition" << endmsg;
    m_jet_def = new fastjet::JetDefinition(fastjet::antikt_algorithm, m_distanceParameter); // this should be made configurable in the future
    // dummy call to fast jet so it's internal initalize methods are set (and we don't print the banner during execute)
    (*m_log) << MSG::INFO << "Making dummy call to fast jet cluster sequence" << endmsg;
    m_particles.clear();
    m_cluster_sequence = new fastjet::ClusterSequence(m_particles, *m_jet_def);
    delete m_cluster_sequence;
    // pileup subtraction
    if (m_doPileupSubtraction) {
        (*m_log) << MSG::INFO << "Setting up fastjet pileup subtraction" << endmsg;
        // define the jet algorithm
        double ktRadius(0.4);
        m_pu_jet_def = new fastjet::JetDefinition(fastjet::kt_algorithm,ktRadius,fastjet::E_scheme,fastjet::Best);
         // define jet area algorithm
        m_pu_area_def = new fastjet::AreaDefinition(fastjet::VoronoiAreaSpec(0.9));
    }
    
    // cleaning
    m_cellenergies.reserve(100);
    
    // monitored vectors
    m_et.reserve(100);
    m_et_em.reserve(100);
    m_et_had.reserve(100);
    m_e.reserve(100);
    m_e_em.reserve(100);
    m_e_had.reserve(100);
    m_eta.reserve(100);
    m_phi.reserve(100);
    m_em_frac.reserve(100);
    m_nLeadingTowers.reserve(100);
    
    return HLT::OK;
}

T2CaloFastJet::~T2CaloFastJet(){
}


HLT::ErrorCode T2CaloFastJet::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
                                        unsigned int type_out)
{
    // since this is an AllTEAlgo, we have to call the monitoring ourselves:
    beforeExecMonitors().ignore();
    
    // ==== pre-execute setup
    if(doTiming()) {
        //m_total_timer->reset();
        m_unpack_timer->reset();
        m_fastjet_timer->reset();
        m_pu_subtraction_timer->reset();
        m_cleaning_timer->reset();
        m_calibration_timer->reset();
        m_RoI_timer->reset();
        //m_total_timer->start();
    }
    m_particles.clear();
    m_constituents.clear();
    m_jets.clear();
    m_nJets = -99;
    m_nGrid = -99;
    //m_TotalTime = -99;
    m_UnpackTime = -99;
    m_FastJetTime = -99;
    m_CleaningTime = -99;
    m_CalibrationTime = -99;
    m_RoITime = -99;
    m_et.clear();
    m_et_em.clear();
    m_et_had.clear();
    m_e.clear();
    m_e_em.clear();
    m_e_had.clear();
    m_eta.clear();
    m_phi.clear();
    m_em_frac.clear();
    m_nLeadingTowers.clear();  
    

#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "================= Executing T2CaloFastJet FEX " << name() << endmsg;
        
    }
#endif
    
    // === get the input particles
    //HLT::TriggerElement* seednode(0);
    const TrigT2Jet* inputGrid(0);
    
    
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << tes_in.size() << " input TEs found" << endmsg;        
    }
#endif
    // error check, we should only have a single input TE
    if ( tes_in.size() != 1 ) {
        msg() << MSG::WARNING << "Got more than one input TE" << endmsg;
        afterExecMonitors().ignore();
        return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::USERDEF_1); 
    }
    
    for ( unsigned type = 0; type < tes_in.size(); ++ type ) {
        std::vector<HLT::TriggerElement*>& tes = tes_in.at(type);
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG << "  - TE[" << type << "]: " << tes.size() << " sub TEs found" << endmsg;        
        }
#endif
        // error check, we should only have a single sub TE
        if ( tes.size() != 1 ) {
            msg() << MSG::WARNING << "Got more than one sub TE" << endmsg;
            afterExecMonitors().ignore();
            return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::USERDEF_1); 
        }
    
        for ( unsigned teIdx = 0; teIdx < tes.size(); ++teIdx ) {
            HLT::ErrorCode ec = getFeature(tes.at(teIdx), inputGrid, m_jetInputKey);
            //seednode = tes.at(teIdx);
            if(ec!=HLT::OK) {
                (*m_log) << MSG::WARNING << "Failed to get the input particles" << endmsg;
                return ec;
            }          
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
                (*m_log) << MSG::DEBUG  << "  - A total of " << inputGrid->grid()->size()  << " particles found in this TE" << endmsg;
            }
#endif
        } // end of loop on sub TEs
    } // end of loop on TEs
    
    const std::vector<Trig3Momentum>* grid = inputGrid->grid();
    
#ifndef NDEBUG
   if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG  << "A total of " << grid->size()  << " particles are to be clustered" << endmsg;
    }
#endif
    
    // === Unpack the grid
    if(doTiming()) m_unpack_timer->start();
    std::vector<Trig3Momentum>::const_iterator begin_grid, begin_grid_fixed, end_grid;
    begin_grid = grid->begin();
    begin_grid_fixed = grid->begin();
    end_grid = grid->end();  
    int jet_counter = 0; 
    for (; begin_grid != end_grid; ++begin_grid){
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){  
            (*m_log) << MSG::DEBUG << "Trig3Momentum["  << jet_counter
                                   << "]: Et: "         << begin_grid->e()/cosh(begin_grid->eta())
                                   << " MeV, eta: "     << begin_grid->eta() 
                                   << ", phi: "         << begin_grid->phi() 
                                   << ", sampling: "    << std::hex << begin_grid->caloSample() << std::dec << endmsg;
        }
#endif
        
        // now fill fast jet particles
        double energy(begin_grid->e());
        if(energy<m_cellMinEnergy) continue;
        double eta(begin_grid->eta());
        double phi(begin_grid->phi());
        double transverse_energy = energy/cosh(eta);
        double px = transverse_energy*cos(phi);
        double py = transverse_energy*sin(phi);
        double pz = transverse_energy*sinh(eta);
        
        fastjet::PseudoJet pseudo_jet( px, py, pz, energy);
        pseudo_jet.set_user_index(std::distance(begin_grid_fixed, begin_grid));
        m_particles.push_back( pseudo_jet );
        
        jet_counter++;
    }
    m_nGrid = grid->size();
    if(doTiming()) m_unpack_timer->stop();
    
    // === Run fast jet
    if(doTiming()) m_fastjet_timer->start();
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG <<  "=== Performing jet finding " << endmsg;
    }
#endif        
    // run the clustering, extract the jets
    m_cluster_sequence = 0;   
    m_pu_cluster_sequence = 0;     
    if (m_doPileupSubtraction){           
        m_pu_cluster_sequence = new fastjet::ClusterSequenceArea(m_particles, *m_jet_def, *m_pu_area_def);
        m_jets = fastjet::sorted_by_pt(m_pu_cluster_sequence->inclusive_jets(m_pTmin)); // sorted by decreasing pt
    } else {
        m_cluster_sequence = new fastjet::ClusterSequence(m_particles, *m_jet_def);
        m_jets = fastjet::sorted_by_pt(m_cluster_sequence->inclusive_jets(m_pTmin)); // sorted by decreasing pt
    }
    
    //std::reverse(m_jets.begin(),m_jets.end());  // sorted by increasing pt
    m_nJets = m_jets.size();
    if(doTiming()) m_fastjet_timer->stop();
    
    // pileup subtraction
    if(doTiming()) m_pu_subtraction_timer->start();
    if (m_doPileupSubtraction){
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG <<  "=== Performing pileup subtraction jet finding " << endmsg;
        }
#endif 
        m_pu_area_cluster_sequence = 0;
        m_pu_area_cluster_sequence = new fastjet::ClusterSequenceArea(m_particles, *m_pu_jet_def, *m_pu_area_def);
        // get rho in central eta range
        m_rho = 0.;
        m_sigma = 0.;
        m_area = 0.;
        fastjet::RangeDefinition etaRange(-2.,2.);
        m_pu_area_cluster_sequence->get_median_rho_and_sigma(etaRange,false,m_rho,m_sigma,m_area);
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG <<  "   completed pileup subtraction jet finding" << endmsg;
            (*m_log) << MSG::DEBUG <<  "   median rho:   " << m_rho << endmsg;
            (*m_log) << MSG::DEBUG <<  "   median sigma: " << m_sigma << endmsg;
            (*m_log) << MSG::DEBUG <<  "   area:         " << m_area << endmsg;
            
        }
#endif 
    }
    if(doTiming()) m_pu_subtraction_timer->pause();
    
    // === Store output
    if(doTiming()) m_RoI_timer->start();
    std::string key = "";
    HLT::TriggerElement* outputTE = 0;
    if (!m_writeMultipleOutputTEs){
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG <<  "Writing all jets to a single output TE" << endmsg;
        }
#endif
        outputTE = addRoI(type_out, new TrigRoiDescriptor(true)); // make a single output TE
        outputTE->setActiveState(true);
    } else {
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG <<  "Writing each jet to it's own output TE" << endmsg;
        }
#endif
    }
    int recorded_jet_counter = 0;
    
    
    
    // inspect the output jets, compute some cleaning variables then store
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG <<  "Found "<< m_jets.size() << " jets " << endmsg;
        if (m_jets.size()) (*m_log) << MSG::DEBUG <<  "== Resultant jets " << endmsg;
    }
#endif
    for (unsigned i = 0; i < m_jets.size(); ++i) {
        if (m_doPileupSubtraction){
            m_constituents = fastjet::sorted_by_pt(m_pu_cluster_sequence->constituents(m_jets[i]));
        } else {
            m_constituents = fastjet::sorted_by_pt(m_cluster_sequence->constituents(m_jets[i]));
        }
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
           (*m_log) << MSG::DEBUG << "  fastjet jet [" << i 
                    << "]: Et: "            << m_jets[i].e()/cosh(m_jets[i].eta()) 
                    << " MeV, eta: "        << m_jets[i].eta() 
                    << ", phi: "            << m_jets[i].phi_std() 
                    << ", rapidity: "       << m_jets[i].rapidity() 
                    << ", n consituents: "  << m_constituents.size() << endmsg;
        }
#endif
        // === pileup subtraction
        if(doTiming()) m_pu_subtraction_timer->resume();
        double et_offset = 0.;
        double e_offset = 0.;
        if (m_doPileupSubtraction){
            double jet_area = m_pu_cluster_sequence->area(m_jets[i]);
            et_offset = jet_area * m_rho;
            e_offset = et_offset * cosh(m_jets[i].eta());
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
               (*m_log) << MSG::DEBUG << "    - area: " << jet_area << ", offset (median m_rho * m_area): " << et_offset << endmsg;
               (*m_log) << MSG::DEBUG << "    - offset ET: " << (m_jets[i].e()/cosh(m_jets[i].eta())) - et_offset << ", e: " << m_jets[i].e()-e_offset << endmsg;
               
            }
#endif
        }
        if(doTiming()) m_pu_subtraction_timer->stop();
        // === cleaning 
        if(doTiming()) m_cleaning_timer->start();
        // taken from Tobias' L2 cleaning code in T2CaloJetConeTool
        // initialize cleaning variables
        int nLeadingCells = 0.;     // number of leading cells
        double em_energy = 0.;        // EM energy
        double had_energy = 0.;       // hadronic energy
        double emf = -9.;             // fraction of jet energy in emc
        double hecf = -9.;            // fraction of jet energy in hec
        float jetQuality = -9.;      // quality of the jet
        float jetTimeCells = -9.;    // jet time calculated from cells
  
        // helper functions / variables for cleaning:
        m_cellenergies.clear();
        float HECEnergy = 0.;
        float EinBadCells = 0.;
        bool isTile = false;
        bool isLAr = false;
        bool qualityTimeIsDefined = false;
        float totaltime = 0.;
        float norm = 0.;
        float cellNorm = 0.;
        float totCellE = 0.;     // sum of energy in LAr cells with quality defined
        double eTot=0, eta=0, phi=0, tmpE=0, eThresh=0, tmpPhi=0;
        double tmpdPhi=0, dphi=0;
        
        if (m_doCleaning) {
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
               (*m_log) << MSG::DEBUG  << "  Computing cleaning variables "<< endmsg;
            }
#endif
            for (unsigned ii = 0; ii < m_constituents.size(); ++ii){
                int grid_id = m_constituents[ii].user_index();
                Trig3Momentum grid_element = grid->at(grid_id);
#ifndef NDEBUG
                if((*m_log).level() <= MSG::DEBUG){
                    (*m_log) << MSG::DEBUG << "    - Constituent["<< ii
                                           << "], grid id["     << grid_id
                                           << "]: Et: "         << grid_element.e()/cosh(grid_element.eta())
                                           << " MeV, eta: "     << grid_element.eta() 
                                           << ", phi: "         << grid_element.phi() 
                                           << ", sampling: "    << std::hex << grid_element.caloSample() << std::dec << endmsg;
                    if (!m_doT2L1Cleaning){
                        (*m_log) << MSG::DEBUG << "        provenance: " << grid_element.provenance()
                                               << ", quality: "          << grid_element.quality()
                                               << ", time: "             << grid_element.time() << endmsg;
                    }
                }
#endif
                // reset flags:
                isTile = false;
                isLAr = false;
                
                tmpE = grid_element.e();    
                eTot += tmpE;     // maybe this should be Et weighted  
                
                // Summ contribution to hadronic/EM energy:  
                const CaloSampling::CaloSample s = grid_element.caloSample();
                if( s == CaloSampling::PreSamplerB || s == CaloSampling::PreSamplerE || 
                    s == CaloSampling::EMB1 || s == CaloSampling::EME1 || 
                    s == CaloSampling::EMB2 || s == CaloSampling::EME2 || 
                    s == CaloSampling::EMB3 || s == CaloSampling::EME3 ||
                    s == CaloSampling::FCAL0){
                    em_energy += tmpE;
                    isLAr = true;  // for cleaning
                }
                if( s == CaloSampling::HEC0 || s ==  CaloSampling::HEC1|| 
                    s == CaloSampling::HEC2 || s == CaloSampling::HEC3 || 
                    s == CaloSampling::TileBar0 || s == CaloSampling::TileExt0 || 
                    s == CaloSampling::TileBar1 || s == CaloSampling::TileExt1 || 
                    s == CaloSampling::TileBar2 || s == CaloSampling::TileExt2 || 
                    s == CaloSampling::TileGap1 || s == CaloSampling::TileGap2 || 
                    s == CaloSampling::TileGap3 || s == CaloSampling::FCAL1 || 
                    s == CaloSampling::FCAL2)
                    had_energy += tmpE;
                
                if (tmpE>0){ // threshold from job options?
                    eThresh += tmpE;    
                    eta += tmpE*grid_element.eta();
                    tmpPhi = grid_element.phi();      
                    
                    tmpdPhi = tmpPhi - m_jets[i].phi_std();   // w.r.t Jet
                    if (tmpdPhi > M_PI) tmpdPhi -= 2*M_PI;    
                    if (tmpdPhi < -1*M_PI) tmpdPhi += 2*M_PI; 
                    
                    dphi += tmpE*tmpdPhi; // this needs the wrap-around check
                
                }
                
                // cleaning:
                m_cellenergies.push_back( tmpE );
                if(	s == CaloSampling::HEC0 || s == CaloSampling::HEC1 || 
                    s == CaloSampling::HEC2 || s == CaloSampling::HEC3 ) {
                    HECEnergy += tmpE;
                    isLAr = true;
                }
                if( s == CaloSampling::FCAL1 || s == CaloSampling::FCAL2 ){
                    isLAr = true;
                }
                if( s == CaloSampling::TileBar0 || s == CaloSampling::TileBar1 || 
                    s == CaloSampling::TileBar2 || s == CaloSampling::TileGap1 ||
                    s == CaloSampling::TileGap2 || s == CaloSampling::TileGap3 || 
                    s == CaloSampling::TileExt0 || s == CaloSampling::TileExt1 || 
                    s == CaloSampling::TileExt2 ) {
                    isTile = true;
                }
                
                if (!m_doT2L1Cleaning){
                    uint16_t cellprovenance = grid_element.provenance();
                    // grab cell quality and cell time.
                    uint16_t cellquality =  grid_element.quality();
                    float celltime = grid_element.time();
                    qualityTimeIsDefined = false;
                    
                    // check for tile
                    if(isTile){
                        // cells with bad quality are masked -> don't take tile into account
                        //   for jet quality calculation.
                        // provenance not set for tile cells. but time is defined.
                        // timing:
                        cellNorm = tmpE * tmpE;
                        totaltime += cellNorm * celltime;
                        norm += cellNorm;
                    }
                    // check for LAr. only take cells into accout which have time and quality defined
                    if( (isLAr) && (cellprovenance & 0x2000) ){
                        qualityTimeIsDefined = true;
                        totCellE += tmpE;
                        // timing:
                        cellNorm = tmpE * tmpE;
                        totaltime += cellNorm * celltime;
                        norm += cellNorm;
                    }
                            
                    if( (isLAr) && (qualityTimeIsDefined) && (cellquality > m_cellQualityThresholdLAr) ){
                        EinBadCells += tmpE;
                    }
                } // end of non L1.5 cleaning
            } // end loop over constituents 
            
            // calculate the jet cleaning variables:
            // -------------------------------------
            if(eThresh != 0) {
                eta /= eThresh;
                phi = dphi/eThresh + m_jets[i].phi_std(); // recover absolute value
                
                // phi must be between -pi and pi :
                if(phi > M_PI) phi -= 2.0* M_PI;
                if(phi < -1.0*M_PI) phi += 2.0*M_PI;
            }         
            // sort cell-energy vector. (most energetic cell at end of vector)
            std::sort(m_cellenergies.begin(), m_cellenergies.end());
            float sumE = 0.;
            for(int k = m_cellenergies.size()-1; k > -1; k--){
                sumE += m_cellenergies[k];
                nLeadingCells++;
                if(sumE > m_leadingCellFraction*eTot){
                    break;
                }
            }
                        
            float DELTA = 0.001;     // distance to compare floats
            if(fabs(eTot)>DELTA){
                hecf = HECEnergy/eTot;
                emf = em_energy/eTot;
            }
            if (!m_doT2L1Cleaning){
                if(fabs(totCellE)>DELTA){
                    jetQuality = EinBadCells/totCellE;
                }
                if(fabs(norm)>DELTA){
                    jetTimeCells = totaltime/norm;
                }  
            }
            
            // === cleaning completed
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
                (*m_log) << MSG::DEBUG  << "  - EM energy [MeV]:  " << em_energy  << ", (ET: " << em_energy/cosh(m_jets[i].eta())  << ")" << endmsg;
                (*m_log) << MSG::DEBUG  << "  - HAD energy [MeV]: " << had_energy << ", (ET: " << had_energy/cosh(m_jets[i].eta()) << ")" << endmsg;
                (*m_log) << MSG::DEBUG  << "  - n leading cells:  " << nLeadingCells << endmsg;
                (*m_log) << MSG::DEBUG  << "  - EM fraction:      " << emf << endmsg;
                (*m_log) << MSG::DEBUG  << "  - HEC fraction:     " << hecf << endmsg;
                (*m_log) << MSG::DEBUG  << "  - Jet quality:      " << jetQuality << endmsg;
                (*m_log) << MSG::DEBUG  << "  - Jet time:         " << jetTimeCells << endmsg;
            }
#endif

        } // end of cleaning
        if(doTiming()) m_cleaning_timer->stop();
        // Set properties
        m_jet = new TrigT2Jet();
        if (m_doPileupSubtraction){
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG)
                (*m_log) << MSG::DEBUG  << "  Recording with offset energy" << endmsg;
#endif
            m_jet->setE(m_jets[i].e() - e_offset);
        } else {
            m_jet->setE(m_jets[i].e());
        }
        m_jet->setEta(m_jets[i].eta());
        m_jet->setPhi(m_jets[i].phi_std());  
        m_jet->setEem0(em_energy);
        m_jet->setEhad0(had_energy);
        // cleaning:
        m_jet->setNLeadingCells(nLeadingCells);
        m_jet->setHecf(hecf);
        m_jet->setJetQuality(jetQuality);
        m_jet->setEmf(emf);
        m_jet->setJetTimeCells(jetTimeCells);     
        // RoI word
        m_jet->set_RoIword(m_t2l1_tools.BLANKWORD+m_t2l1_tools.SET_INPUT*m_inputType+m_t2l1_tools.SET_OUTPUT*m_outputType+i);
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG)
            (*m_log) << MSG::DEBUG  << "  RoI word set to: " << m_jet->RoIword() << endmsg;
#endif

        // === calibration 
        if(doTiming()) m_calibration_timer->start();
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
           (*m_log) << MSG::DEBUG  << "  " << m_tools.size() << " calibration tools to be run"<< endmsg;
           
        }
#endif
        // do loop over tools
        ToolHandleArray< T2CaloJetBaseTool >::iterator it = m_tools.begin(),
        itToolEnd = m_tools.end();
        int current=1;
        for (; it != itToolEnd; ++it) {
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
               (*m_log) << MSG::DEBUG  << "  Executing tool [" << current << "]"<< endmsg;
            }
#endif
            if ((*it)->execute(m_jet, TrigRoiDescriptor(true), caloDDENull ).isFailure()){    // the zeros are the unused eta phi coordinates used by many base tools to define the RoI region
                msg() << MSG::WARNING << "T2CaloFastJet AlgToolJets returned Failure" << endmsg;
                return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,HLT::Reason::USERDEF_1);
            }
            current++;
        }
        
#ifndef NDEBUG
        if( (m_tools.size()>0) && ((*m_log).level() <= MSG::DEBUG) ){
           (*m_log) << MSG::DEBUG << "  calibrated jet [" << i 
                    << "]: Et: "            << m_jet->et()
                    << " MeV, eta: "        << m_jet->eta() 
                    << ", phi: "            << m_jet->phi() 
                    << endmsg;
        }
#endif
        if(doTiming()) m_calibration_timer->stop();
        // === monitor
        m_et.push_back( m_jet->et()); 
        m_et_em.push_back( (m_jet->eem0()/cosh(m_jet->eta()))); 
        m_et_had.push_back( (m_jet->ehad0()/cosh(m_jet->eta()))); 
        m_e.push_back( m_jet->e()); 
        m_e_em.push_back( m_jet->eem0()); 
        m_e_had.push_back( m_jet->ehad0()); 
        m_eta.push_back( m_jet->eta() ); 
        m_phi.push_back( m_jet->phi() );
        m_em_frac.push_back( m_jet->emf() ); 
        m_nLeadingTowers.push_back( m_jet->nLeadingCells() );
          
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG << "  recording T2CaloJet["<<recorded_jet_counter<<"]"<<endmsg;
        }
#endif
        if (m_writeMultipleOutputTEs){
            unsigned int secondary_roi_word = m_jet->RoIword();
            if (m_secondOutputType != 0){
                secondary_roi_word = m_t2l1_tools.BLANKWORD+m_t2l1_tools.SET_INPUT*m_outputType+m_t2l1_tools.SET_OUTPUT*m_secondOutputType+i;
            } 
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
                (*m_log) << MSG::DEBUG << "  preparing an output TE, using RoI word: " << secondary_roi_word <<endmsg;
            }
#endif
            // addRoI implementation
            //outputTE = addRoI(type_out, m_jet->eta(), m_jet->phi()); // make an output TE
            // alternative implementation
            //outputTE = config()->getNavigation()->addNode(seednode,type_out);
            //outputTE->setActiveState(true);
            HLT::TriggerElement* initialTE = config()->getNavigation()->getInitialNode();
            outputTE = config()->getNavigation()->addNode(initialTE,type_out);
            outputTE->setActiveState(true);
            
            // setup a trig RoI descriptor
            unsigned int dummy_l1id = -9; // NO NO!!! THE L1ID IS AN UNSIGNED INT!!!!
            int dummy_id = -9;            // NO NO!!! THE ID IS AN UNSIGNED INT!!!!
            TrigRoiDescriptor* newRoiDescriptor = 
	      new TrigRoiDescriptor(unsigned(secondary_roi_word), unsigned(dummy_l1id), unsigned(dummy_id), 
				    m_jet->eta(), m_jet->eta(), m_jet->eta(),
				    m_jet->phi(), m_jet->phi(), m_jet->phi()
				    );
	    /// HOW BIG IS THE ROI ???
            HLT::ErrorCode hltStatus = attachFeature(outputTE, newRoiDescriptor, m_jetOutputKey);            
            if ( hltStatus != HLT::OK ) {
                (*m_log) << MSG::ERROR << "recording of RoiDescriptor into StoreGate failed" << endmsg;
                return hltStatus;
            }

            
            hltStatus = recordAndAttachFeature(outputTE, m_jet, key, m_jetOutputKey);
            if (hltStatus != HLT::OK){
                (*m_log) << MSG::ERROR << "recording of TrigT2Jet into StoreGate failed" << endmsg;
                return hltStatus;
            }
        } else {
            HLT::ErrorCode hltStatus = recordAndAttachFeature(outputTE, m_jet, key, m_jetOutputKey);
            if (hltStatus != HLT::OK){
                (*m_log) << MSG::ERROR << "recording of TrigT2Jet into StoreGate failed" << endmsg;
                return hltStatus;
            }
        }
        
  
        recorded_jet_counter++;
    }        
  
    m_RoI_timer->stop();
    
    
    // === post-execute admin
    if (m_doPileupSubtraction){
        delete m_pu_cluster_sequence; 
        delete m_pu_area_cluster_sequence; 
    } else {
        delete m_cluster_sequence; 
    }
    if(doTiming()) {
        //m_total_timer->stop();
        m_UnpackTime = m_unpack_timer->elapsed();
        m_FastJetTime = m_fastjet_timer->elapsed();
        m_puSubTime = m_pu_subtraction_timer->elapsed();
        m_CleaningTime = m_cleaning_timer->elapsed();
        m_CalibrationTime = m_calibration_timer->elapsed();
        m_RoITime = m_RoI_timer->elapsed();
        //m_TotalTime = m_total_timer->elapsed();    
    
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG << "Unpacking completed in      "     << m_unpack_timer->elapsed()         << " ms " << endmsg;
            (*m_log) << MSG::DEBUG << "FastJet completed in        "     << m_fastjet_timer->elapsed()        << " ms " << endmsg;
            (*m_log) << MSG::DEBUG << "PU subtraction completed in "     << m_pu_subtraction_timer->elapsed() << " ms " << endmsg;
            (*m_log) << MSG::DEBUG << "Cleaning completed in       "     << m_cleaning_timer->elapsed()       << " ms " << endmsg;
            (*m_log) << MSG::DEBUG << "Calibration completed in    "     << m_calibration_timer->elapsed()    << " ms " << endmsg;
            (*m_log) << MSG::DEBUG << "RoI making completed in     "     << m_RoI_timer->elapsed()            << " ms " << endmsg;
            //(*m_log) << MSG::DEBUG << "T2CaloFastJet completed in  "     << m_total_timer->elapsed()          << " ms " << endmsg;
        }
#endif
    }
    
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG) (*m_log) << MSG::DEBUG << "================= Finished T2CaloFastJet " << name() << endmsg;
#endif
    // since this is an AllTEAlgo, we have to call the monitoring ourselves:
    afterExecMonitors().ignore();
    
    return HLT::OK;
}


HLT::ErrorCode T2CaloFastJet::hltFinalize()
{
    if ( (*m_log).level() <= MSG::DEBUG )
        (*m_log) << MSG::DEBUG << "Finalizing T2CaloFastJet FEX " << name() << endmsg;
    
    delete m_log;
    m_particles.clear();
    m_jets.clear();
    m_constituents.clear();
    m_et.clear();
    m_et_em.clear();
    m_et_had.clear();
    m_e.clear();
    m_e_em.clear();
    m_e_had.clear();
    m_eta.clear();
    m_phi.clear();
    m_em_frac.clear();
    m_nLeadingTowers.clear();
    
    if ( bool(m_jet_def) ){
        delete m_jet_def;
    }
    
    if (m_doPileupSubtraction){
        delete m_pu_jet_def;
        delete m_pu_area_def;
    }
    
          
    return HLT::OK;
}
