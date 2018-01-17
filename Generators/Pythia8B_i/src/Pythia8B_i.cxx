/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ======================================================================
// Pythia8B_i
// James.Catmore@cern.ch, Eva.Bouhova@cern.ch, Maria.Smizanska@cern.ch (July 2011)
// Inherits from Pythia8_i (jmonk@hep.ucl.ac.uk)
// Implementation of repeated hadronization of b-quark events in Pythia8
// Based on pythia8150/examples/main15.cc in the Pythia8 distribution,
// T. Sjostrand et al,
// http://home.thep.lu.se/~torbjorn/pythiaaux/present.html
// ======================================================================
#include "Pythia8B_i/Pythia8B_i.h"
#include "Pythia8B_i/UserSelections.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

// Pointer On AtRndmGenSvc
IAtRndmGenSvc*  Pythia8B_i::p_AtRndmGenSvc  = 0;

////////////////////////////////////////////////////////////////////////////////
// User properties not defined by Pythia8_i
Pythia8B_i::Pythia8B_i
(const std::string &name, ISvcLocator *pSvcLocator): Pythia8_i(name,pSvcLocator) {
    declareProperty("useRndmGenSvc", m_useRndmGenSvc = true);
    declareProperty("NHadronizationLoops", m_had=1);
    declareProperty("NDecayLoops", m_dec=1);
    declareProperty("SelectBQuarks",m_selectBQuarks=true);
    declareProperty("SelectCQuarks",m_selectCQuarks=false);
    declareProperty("QuarkPtCut", m_qPtCut= 0.0);
    declareProperty("AntiQuarkPtCut", m_aqPtCut= 0.0);
    declareProperty("QuarkEtaCut", m_qEtaCut=102.5);
    declareProperty("AntiQuarkEtaCut", m_aqEtaCut=102.5);
    declareProperty("RequireBothQuarksPassCuts", m_and=false);
    declareProperty("VetoDoubleBEvents", m_vetoDoubleB=true);
    declareProperty("VetoDoubleCEvents", m_vetoDoubleC=true);
    declareProperty("BPDGCodes", m_bcodes);
    declareProperty("TriggerPDGCode", m_trigCode=0); // 0 = no selection applied
    declareProperty("TriggerStatePtCut", m_trigPtCut);
    declareProperty("TriggerStateEtaCut", m_trigEtaCut=2.5);
    declareProperty("MinimumCountPerCut", m_cutCount);
    declareProperty("TriggerInvariantMassCut",m_invMass=0.0);
    declareProperty("TriggerOppositeCharges",m_oppCharges=false);
    declareProperty("SignalPDGCodes", m_sigCodes);
    declareProperty("SignalPtCuts", m_sigPtCuts);
    declareProperty("SignalEtaCuts", m_sigEtaCuts);
    declareProperty("NumberOfSignalsRequiredPerEvent", m_nSignalRequired=1); 
    declareProperty("UserSelection", m_userString="NONE");
    declareProperty("UserSelectionVariables", m_userVar);
    declareProperty("SuppressSmallPT", m_doSuppressSmallPT=false);
    declareProperty("pT0timesMPI", m_pt0timesMPI=1.0);
    declareProperty("numberAlphaS", m_numberAlphaS=3.0);
    declareProperty("useSameAlphaSasMPI", m_sameAlphaSAsMPI=false);
    
    m_totalBQuark = 0;
    m_totalBBarQuark = 0;
    m_totalCQuark = 0;
    m_totalCBarQuark = 0;
    m_totalPythiaCalls = 0;
    m_totalHard = 0;
    m_totalClone = 0;
    m_atLeastOneAcc = 0;
    m_passingTriggerCuts = 0;
    m_internal_event_number = 0;
    m_speciesCount.clear();
    m_SuppressSmallPT = 0;
    for (std::vector<int>::iterator iit=m_bcodes.begin(); iit!=m_bcodes.end(); ++iit) {
        m_speciesCount[*iit] = 0;
    }
    
    
}

////////////////////////////////////////////////////////////////////////////////
Pythia8B_i::~Pythia8B_i(){
}
////////////////////////////////////////////////////////////////////////////////

StatusCode Pythia8B_i::genInitialize() {
    
    // Logic checks
    unsigned int trigPtCutsSize = m_trigPtCut.size();
    if (trigPtCutsSize!=m_cutCount.size()) {
        ATH_MSG_ERROR("You are requesting " << trigPtCutsSize << " trigger-like pt cuts but are providing required counts for " << m_cutCount.size() << " cuts. This doesn't make sense.");
        return StatusCode::FAILURE;
    }
	
    /// @todo Isn't this already integrated into GenModule?
    static const bool CREATEIFNOTTHERE(true);
    StatusCode RndmStatus = service("AtRndmGenSvc",
                                    Pythia8B_i::p_AtRndmGenSvc,
                                    CREATEIFNOTTHERE);
    if (!RndmStatus.isSuccess() || 0 == Pythia8B_i::p_AtRndmGenSvc)
    {
        ATH_MSG_ERROR(" Could not initialize Random Number Service");
        return RndmStatus;
    }
    
    // This over-rides the genInitialize in the base class Pythia8_i, but then calls it
    // Sets the built-in UserHook called SuppressLowPT
    // FOR ONIA USE ONLY
    ATH_MSG_INFO("genInitialize() from Pythia8B_i");
    if (m_doSuppressSmallPT) {
        m_SuppressSmallPT = new Pythia8::SuppressSmallPT(m_pt0timesMPI,m_numberAlphaS,m_sameAlphaSAsMPI);
        Pythia8_i::m_pythia.setUserHooksPtr(m_SuppressSmallPT);
    }

    // Call the base class genInitialize()
    if (Pythia8_i::genInitialize().isSuccess()) {
        return StatusCode::SUCCESS;
    } else {return StatusCode::FAILURE;}
    
}


////////////////////////////////////////////////////////////////////////////////
StatusCode Pythia8B_i::genuserInitialize(){
    
    // Just counter setting
    ATH_MSG_INFO("Pythia8B_i from genuserInitialize()");
    
    // Initialize global counters
    m_totalBQuark = 0;
    m_totalBBarQuark = 0;
    m_totalCQuark = 0;
    m_totalCBarQuark = 0;
    m_totalPythiaCalls = 0;
    m_totalHard = 0;
    m_totalClone = 0;
    m_atLeastOneAcc = 0;
    m_passingTriggerCuts = 0;
    m_internal_event_number = 0;
    m_speciesCount.clear();
    for (std::vector<int>::iterator iit=m_bcodes.begin(); iit!=m_bcodes.end(); ++iit) {
        m_speciesCount[*iit] = 0;
    }
    
    return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
StatusCode Pythia8B_i::callGenerator(){
    
    ATH_MSG_DEBUG(">>> Pythia8B_i from callGenerator");
    
    // Check the buffers are empty - if not return so that the GenModule
    // calls fillEvt again and takes another event from the buffer
    if (m_BEventBuffer.size()!=0) {
        ATH_MSG_DEBUG("BEventBuffer not empty; skipping callGenerator");
        return StatusCode::SUCCESS;
    }
    
    if(m_useRndmGenSvc && Pythia8B_i::p_AtRndmGenSvc){
        // Save the random number seeds in the event
        CLHEP::HepRandomEngine*  engine  = Pythia8B_i::p_AtRndmGenSvc->GetEngine(Pythia8_i::pythia_stream);
        const long* s =  engine->getSeeds();
        m_seeds.clear();
        m_seeds.push_back(s[0]);
        m_seeds.push_back(s[1]);
    }
    
    // Generator. Shorthand for event.
    Pythia8::Event& event = Pythia8_i::m_pythia.event;
    
    // Begin event loop.
    int iEvent(0);
    while (iEvent < 1) { // keep going until an event is accepted
        
        ++m_totalPythiaCalls;
        ATH_MSG_DEBUG("Throwing the dice....");
        if (!Pythia8_i::m_pythia.next()) continue;
        
        // Find b(c)/antib(c) quarks and enforce cuts as required
        int nbBeforeSelection(0);
        int nbbarBeforeSelection(0);
        int ncBeforeSelection(0);
        int ncbarBeforeSelection(0);
        int nbQuark(0);
        int nbbarQuark(0);
        int ncQuark(0);
        int ncbarQuark(0);
        int stat;
        for (int i = 0; i < event.size(); ++i) {
            stat = event[i].statusAbs();
            bool isBQuark(false);
            bool isCQuark(false);
            bool isAntiBQuark(false);
            bool isAntiCQuark(false);
            // b-quarks (=anti-B hadrons)
            if (event[i].id() == 5 && (stat == 62 || stat == 63)) {isBQuark=true; ++nbBeforeSelection;}
            if (event[i].id() == 4 && (stat == 62 || stat == 63)) {isCQuark=true; ++ncBeforeSelection;}
            if ( (isBQuark && m_selectBQuarks) || ((isCQuark && m_selectCQuarks)) ) {
                bool passesPtCut(false); bool passesEtaCut(false);
                std::string accString = " : REJECTED";
                double qpt = event[i].pT(); double qeta = fabs(event[i].eta());
                if (qpt>m_qPtCut) passesPtCut=true;
                if (qeta<m_qEtaCut) passesEtaCut=true;
                if (passesPtCut && passesEtaCut) {
                    if (isBQuark) ++nbQuark;
                    if (isCQuark) ++ncQuark;
                    accString = " : ACCEPTED";
                }
                if (isBQuark) ATH_MSG_DEBUG("bQuark pt/eta = " << qpt << "/" << qeta << accString);
                if (isCQuark) ATH_MSG_DEBUG("cQuark pt/eta = " << qpt << "/" << qeta << accString);
            }
            // anti-b quarks (=B-hadrons)
            if (event[i].id() == -5 && (stat == 62 || stat == 63)) {isAntiBQuark=true; ++nbbarBeforeSelection;}
            if (event[i].id() == -4 && (stat == 62 || stat == 63)) {isAntiCQuark=true; ++ncbarBeforeSelection;}
            if ( (isAntiBQuark && m_selectBQuarks) || ((isAntiCQuark && m_selectCQuarks)) ) {
                bool passesPtCut(false); bool passesEtaCut(false);
                std::string accString = " : REJECTED";
                double aqpt = event[i].pT(); double aqeta = fabs(event[i].eta());
                if (aqpt>m_aqPtCut) passesPtCut=true;
                if (aqeta<m_aqEtaCut) passesEtaCut=true;
                if (passesPtCut && passesEtaCut) {
                    if (isAntiBQuark) ++nbbarQuark;
                    if (isAntiCQuark) ++ncbarQuark;
                    accString = " : ACCEPTED";
                }
                if (isAntiBQuark) ATH_MSG_DEBUG("bbarQuark pt/eta = " << aqpt << "/" << aqeta << accString);
                if (isAntiCQuark) ATH_MSG_DEBUG("ccbarQuark pt/eta = " << aqpt << "/" << aqeta << accString);
            }
        }
        if (nbBeforeSelection+nbbarBeforeSelection>=4 && m_vetoDoubleB) {
            ATH_MSG_DEBUG("At user request, rejecting double b-bbar event; throwing dice again");
            continue;
        }
        if (ncBeforeSelection+ncbarBeforeSelection>=4 && m_vetoDoubleC) {
            ATH_MSG_DEBUG("At user request, rejecting double c-cbar event; throwing dice again");
            continue;
        }
        bool rejectBBbar(false);
        bool rejectCCbar(false);
        // Enforce user selections
        if (!m_and && (nbbarQuark==0 && nbQuark==0) && m_selectBQuarks) rejectBBbar=true;
        if (m_and && (nbbarQuark==0 || nbQuark==0) &&  m_selectBQuarks) rejectBBbar=true;
        if (!m_and && (ncbarQuark==0 && ncQuark==0) && m_selectCQuarks) rejectCCbar=true;
        if (m_and && (ncbarQuark==0 || ncQuark==0) &&  m_selectCQuarks) rejectCCbar=true;
        if (m_selectBQuarks && m_selectCQuarks && rejectBBbar && rejectCCbar) {
            ATH_MSG_DEBUG("No b- or c- quarks accepted; throwing the dice again");
            continue;
        }
        if (m_selectBQuarks && !m_selectCQuarks && rejectBBbar) {
            ATH_MSG_DEBUG("No b-quarks accepted; throwing the dice again");
            continue;
        }
        if (!m_selectBQuarks && m_selectCQuarks && rejectCCbar) {
            ATH_MSG_DEBUG("No c-quarks accepted; throwing the dice again");
            continue;
        }
        m_totalBQuark += nbQuark;
        m_totalBBarQuark += nbbarQuark;
        m_totalCQuark += ncQuark;
        m_totalCBarQuark += ncbarQuark;
        ++m_totalHard;
        
        // Switch off decays of species of interest if requested (only for repeated decays)
        bool doRepeatedDecays(false); if (m_dec>1) doRepeatedDecays=true;
        if (doRepeatedDecays) {
            for (unsigned int iC = 0; iC < m_bcodes.size(); ++iC) Pythia8_i::m_pythia.particleData.mayDecay( m_bcodes[iC], false);
        }
        
        //  REPEATED HADRONIZATION LOOP
        //  Make a spare copy of event
        Pythia8::Event eventCopy;
        std::vector<Pythia8::Event> repeatHadronizedEvents;
        std::vector<Pythia8::Event>::iterator eventItr;
        eventCopy = event;
        // Begin loop over rounds of hadronization for same event.
        for (unsigned int iRepeat = 0; iRepeat < m_had; ++iRepeat) {
            ++m_totalClone;
            // Repeated hadronization: restore saved event record.
            if (iRepeat > 0) event = eventCopy;
            // Repeated hadronization: do HadronLevel (repeatedly).
            if (!Pythia8_i::m_pythia.forceHadronLevel()) continue;
            repeatHadronizedEvents.push_back(event); // save the events for selections
        }
        
        // REPEATED DECAY LOOP
        std::vector<Pythia8::Event> savedEvents;
        if (doRepeatedDecays) {
            // switch decays back on
            for (unsigned int iC = 0; iC < m_bcodes.size(); ++iC) Pythia8_i::m_pythia.particleData.mayDecay( m_bcodes[iC], true);
            // Loop over repeatedly hadronized events
            for (eventItr=repeatHadronizedEvents.begin(); eventItr!=repeatHadronizedEvents.end(); ++eventItr) {
                for (unsigned int iRepeat = 0; iRepeat < m_dec; ++iRepeat) {
                    event = (*eventItr);
                    if (!Pythia8_i::m_pythia.moreDecays()) break;
                    savedEvents.push_back(event);
                }
            }
        }
        if (!doRepeatedDecays) savedEvents = repeatHadronizedEvents;
        
        //event.list();
        
        // Make final selections on savedEvents
        std::vector<Pythia8::Event> finalEvents;
        bool signalSelect(false);
        if (m_sigCodes.size()>0) signalSelect=true;
        for (eventItr=savedEvents.begin(); eventItr!=savedEvents.end(); ++eventItr) {
            // Does event contain basic leptons needed to fire triggers?
            if (!leptonSelect(*eventItr,m_trigPtCut,m_trigEtaCut,m_cutCount,m_trigCode,m_invMass,m_oppCharges)) continue;
            ++m_passingTriggerCuts;
            // Does the events contain a signal decay as required by the user
            if (signalSelect) {
                if (!cleanUndecayed(*eventItr,m_bcodes)) continue;
                if (signalSelect) {if (!signalAccept(*eventItr,m_sigCodes,m_sigPtCuts,m_sigEtaCuts,m_nSignalRequired)) continue;}
            }
            // User-defined selections if any
            if (m_userString!="NONE") {
                if (!userSelection(*eventItr,m_userString,m_userVar)) continue;
            }
            finalEvents.push_back(*eventItr);
            ++iEvent;
        }
        savedEvents.clear();
        if (finalEvents.size()>0) ++m_atLeastOneAcc;
        
        // Species counting and preparing vector for filling
        for (eventItr=finalEvents.begin(); eventItr!=finalEvents.end(); ++eventItr) {
            for (int i = 0; i < (*eventItr).size(); ++i) {
                int id = (*eventItr)[i].id();
                std::map<int,int>::iterator it;
                it = m_speciesCount.find(id);
                if (it != m_speciesCount.end()) {
                    int count = it->second+1;
                    m_speciesCount[id]=count;
                }
            }
            m_BEventBuffer.push_back(*eventItr);
            ++m_internal_event_number;
            m_internalEventNumbers.push_back(m_internal_event_number);
            //(*eventItr).list();
        }
        
        // End of event loop.
    }
    
    StatusCode returnCode = StatusCode::SUCCESS;
    
    return returnCode;
}

///////////////////////////////////////////////////////////////////////////////
StatusCode Pythia8B_i::fillEvt(HepMC::GenEvent *evt){
    
    ATH_MSG_DEBUG(">>> Pythia8B_i from fillEvt");
    ATH_MSG_DEBUG("BEventBuffer contains " << m_BEventBuffer.size() << " events ");
    if (m_BEventBuffer.size()==0 ) {
        ATH_MSG_DEBUG("BEventBuffer now empty - going to next Pythia event");
        return StatusCode::SUCCESS;
    }
    
    Pythia8::Event &pyev = *(m_BEventBuffer.begin());
    evt->set_event_number(*(m_internalEventNumbers.begin()));
    m_pythiaToHepMC.fill_next_event(pyev, evt, 1);
    
    // fill the pdf information
    //m_pythiaToHepMC.put_pdf_info(evt, m_pythia, false);
    
    // set the randomseeds
    if(m_useRndmGenSvc && Pythia8B_i::p_AtRndmGenSvc)
        evt->set_random_states(m_seeds);
    
    // set the event weight
    evt->weights().push_back(m_pythia.info.weight());
    
    // Units correction
#ifdef HEPMC_HAS_UNITS
    if(Pythia8_i::pythiaVersion() < 8.170 ){
        GeVToMeV(evt);
    }
#else
    ATH_MSG_DEBUG("Not scaling HepMC energies for Py8 > 8.153!");
#endif
    
#ifndef HEPMC_HAS_UNITS
    ATH_MSG_DEBUG();
#endif
    
    // Remove event/number from buffer
    m_BEventBuffer.erase(m_BEventBuffer.begin());
    m_internalEventNumbers.erase(m_internalEventNumbers.begin());
    
    //HepMC::GenEvent *evtCopy = new HepMC::GenEvent(*evt);
    
    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
StatusCode Pythia8B_i::genFinalize(){
    
    ATH_MSG_INFO(">>> Pythia8B_i from genFinalize");
    
    Pythia8_i::m_pythia.stat();
    
    Pythia8::Info info = Pythia8_i::m_pythia.info;
    double xs = info.sigmaGen();// in mb
    xs *= 1000. * 1000.;//convert to nb
    
    std::cout <<   "MetaData: cross-section (nb)= " << xs << std::endl;
    std::cout <<   "Total events from Pythia = " << m_totalPythiaCalls<< std::endl;
    std::cout <<   "Number of accepted b quarks = " << m_totalBQuark<< std::endl;
    std::cout <<   "Number of accepted bbar quarks = " << m_totalBBarQuark<< std::endl;
    std::cout <<   "Number of accepted c quarks = " << m_totalCQuark<< std::endl;
    std::cout <<   "Number of accepted cbar quarks = " << m_totalCBarQuark<< std::endl;
    std::cout <<   "Number of accepted b/c events before hadronization = " << m_totalHard<< std::endl;
    std::cout <<   "Number of hadronization loops per b/c-event = " << m_had<< std::endl;
    std::cout <<   "Total number of hadronization loops = " << m_totalClone<< std::endl;
    std::cout <<   "Number of accepted b/c events yielding at least one finally accepted event = " << m_atLeastOneAcc<< std::endl;
    std::cout <<   "Average number of accepted events per hard event = " << static_cast<float>(m_internal_event_number)/static_cast<float>(m_atLeastOneAcc)<< std::endl;
    std::cout <<   "Number of events passing trigger cuts = " << m_passingTriggerCuts<< std::endl;
    std::cout <<   "Number of accepted events = " << m_internal_event_number<< std::endl;
    std::cout <<   "Summary of cuts: " << std::endl;
    if (m_selectBQuarks || m_selectCQuarks) {
        if (m_selectBQuarks) std::cout << "Quarks cuts apply to b-quarks" << std::endl;
        if (m_selectCQuarks) std::cout << "Quarks cuts apply to c-quarks" << std::endl;
        std::cout <<   "Quark pt > " << m_qPtCut << std::endl;
        std::cout <<   "Antiquark pt > " << m_aqPtCut << std::endl;
        std::cout <<   "Quark eta < " << m_qEtaCut << std::endl;
        std::cout <<   "Antiquark eta < " << m_aqEtaCut << std::endl;
        if (m_and) std::cout << "Require quark and anti-quark pass cuts" << std::endl;
    }
    std::cout <<   "Trigger lepton type = " << m_trigCode << std::endl;
    std::cout <<   "Trigger lepton pt cuts: ";
    for (unsigned int prCntr=0; prCntr<m_trigPtCut.size(); ++prCntr) {std::cout << m_trigPtCut[prCntr] << " ";}
    std::cout << std::endl;
    std::cout <<   "Trigger lepton eta cut: " << m_trigEtaCut << std::endl;
    std::cout <<   "Required number of leptons passing each trigger cut = ";
    for (unsigned int prCntr=0; prCntr<m_cutCount.size(); ++prCntr) {std::cout << m_cutCount[prCntr] << " ";}
    std::cout << std::endl;
    std::cout <<   "Invariant mass of trigger leptons > " << m_invMass << std::endl;
    if (m_oppCharges) std::cout << "Trigger leptons required to have opposite charges" << std::endl;
    if (m_sigCodes.size() > 0) printSignalSelections(m_sigCodes,m_sigPtCuts,m_sigEtaCuts,m_nSignalRequired);
    if (m_userString!="NONE") std::cout << "User selection: " << m_userString << std::endl;
    
    if (m_speciesCount.size()>0) {
        std::cout <<"\nSpecies\tCount\n"<< std::endl;
        for ( std::map<int,int>::iterator iter = m_speciesCount.begin();
             iter != m_speciesCount.end(); ++iter )
            std::cout << iter->first << '\t' << iter->second << '\n'<< std::endl;
    }
    if (m_dec>1) std::cout << "Number of times each of these states were copied and decayed: " << m_dec << std::endl;
    
    float cloningFactor = (float)(m_internal_event_number)/(float)(m_atLeastOneAcc);
    float finalXS = ((float)xs*((float)m_internal_event_number/(float)m_totalPythiaCalls))/(float)m_had;
    
    std::cout << " I===================================================================================== " << std::endl;
    std::cout << " I   CROSSSECTION OF YOUR B-CHANNEL IS                   I    " << std::endl;
    std::cout << " I                       BX= PX*NB/AC/MHAD=              I    " << finalXS << " nb" << std::endl;
    std::cout << " I                                                       I    " << std::endl;
    std::cout << " I   IN CASE YOU FORCED ANY DECAY YOU SHOULD             I    " << std::endl;
    std::cout << " I   CORRECT CROSS SECTION BX FURTHER, MULTIPLYING       I    " << std::endl;
    std::cout << " I   BX BY BRANCHING RATIO(S) OF YOUR FORCED             I    " << std::endl;
    std::cout << " I   DECAY(S) AND BY A FACTOR OF 2 FOR SYMMETRY          I    " << std::endl;
    std::cout << " I                                                       I    " << std::endl;
    std::cout << " I   MORE DETAILS ON CROSS SECTION                       I    " << std::endl;
    std::cout << " I   PYTHIA CROSS SECTION IS  	 	 	    PX=           I    " << xs <<" nb" << std::endl;
    std::cout << " I   NUMBER OF   ACCEPTED  HARD EVENTS     AC=           I    " << m_totalPythiaCalls << std::endl;
    std::cout << " I   NUMBER OF   ACCEPTED    B-EVENTS   IS NB=           I    " << m_internal_event_number<< std::endl;
    std::cout << " I   REPEATED HADRONIZATIONS IN EACH EVENT MHAD=         I    " << m_had << std::endl;
    std::cout << " I   AVERAGE NUM OF ACCEPTED EVTS IN HADRONIZATION LOOP  I    " << cloningFactor << std::endl;
    std::cout << " I   IN CASE YOU FORCED ANY DECAY YOU SHOULD             I    " << std::endl;
    std::cout << " I   CORRECT CROSS SECTION BX FURTHER, MULTIPLYING       I    " << std::endl;
    std::cout << " I   BX BY BRANCHING RATIO(S) OF YOUR FORCED             I    " << std::endl;
    std::cout << " I   DECAY(S) AND BY A FACTOR OF 2 FOR SYMMETRY          I    " << std::endl;
    std::cout << " I                                                       I    " << std::endl;
    std::cout << " I===================================================================================== " << std::endl;
    std::cout << "" << std::endl;
    std::cout << "MetaData: cross-section (nb)= " << finalXS << std::endl;
    
    return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// Enforce trigger-like selections on final state leptons
// User can require pt and eta cuts on a given number of leptons.
// User can enforce at least one oppositely charged pair
// User can enforce a minimum invariant mass on a pair

bool Pythia8B_i::leptonSelect(Pythia8::Event &theEvent, std::vector<double> ptCut, double etaCut, std::vector<int> counts, int type_id, double massCut, bool opposite) {
    
    if (type_id==0) return true;
    bool passed(false);
    std::string accString(" : REJECTED");
    std::vector<int> countGood;
    std::vector<int> leptonIDs;
    int nCuts=ptCut.size();
    for (int cutCntr=0; cutCntr<nCuts; ++cutCntr) {
        countGood.push_back(0);
    }
    for (int i = 0; i<theEvent.size(); ++i) {
        Pythia8::Particle theParticle = theEvent[i];
        int id = theParticle.idAbs();
        if ( (id == type_id) ) { // find lepton flavour requested by user
            double pt = theParticle.pT();
            double eta = theParticle.eta();
            ATH_MSG_DEBUG("Lepton of type " << id << " with pt/eta " << pt << "/" << eta);
            for (int cutCntr=0; cutCntr<nCuts; ++cutCntr) {
                if ( (pt>ptCut[cutCntr]) && (fabs(eta)<etaCut) ) {
                    countGood[cutCntr] += 1;
                    leptonIDs.push_back(i); // remember leptons
                }
            }
        }
        // can the loop stop?
        int nPassed(0);
        for (int cutCntr=0; cutCntr<nCuts; ++cutCntr) {
            if (countGood[cutCntr] >= counts[cutCntr]) ++nPassed;
        }
        if (nPassed==nCuts) break;
    } // end of loop over particles
    
    // Check the accumulated counts
    int nPassed(0);
    for (int cutCntr=0; cutCntr<nCuts; ++cutCntr) {
        if (countGood[cutCntr] >= counts[cutCntr]) ++nPassed;
    }
    if (nPassed >= nCuts && nCuts==1) {accString=" : ACCEPTED"; passed = true;} // only 1 lepton required so no pair properties
    if (nPassed >= nCuts && nCuts>1 && pairProperties(theEvent,leptonIDs,massCut,opposite)) {accString=" : ACCEPTED"; passed = true;}
    ATH_MSG_DEBUG("Trigger-like selection" << accString);
    return passed;
}

///////////////////////////////////////////////////////////////////////////////
// Require that an event contains no undecayed particles of the types listed by
// user
bool Pythia8B_i::cleanUndecayed(Pythia8::Event &theEvent, std::vector<int> bCodes) {
    
    bool cleanEvent(true);
    std::string accString(" : ACCEPTED");
    for (int i = 0; i<theEvent.size(); ++i) {
        Pythia8::Particle theParticle = theEvent[i];
        int id = theParticle.id();
        int status = theParticle.status();
        for (std::vector<int>::iterator iItr = bCodes.begin(); iItr!=bCodes.end(); ++iItr) {
            if ( (id == *iItr) && (status>0) ) {accString=" : REJECTED"; cleanEvent = false;}
        }
    }
    ATH_MSG_DEBUG("Check event for undecayed signal particles" << accString);
    
    return cleanEvent;
    
}

///////////////////////////////////////////////////////////////////////////////
// Requires that a set of leptons represented by (index numbers in event record)
// contains at least one pair with
// - invariant mass above massCut
// - opposite charges if user requires

bool Pythia8B_i::pairProperties(Pythia8::Event &theEvent, std::vector<int> leptonIDs, double massCut, bool opposite) {
    
    std::vector<int>::iterator iit,iit2;
    bool passesCuts(false);
    std::string accString(" : REJECTED");
    for (iit = leptonIDs.begin(); iit<leptonIDs.end(); ++iit) {
        for (iit2 = iit+1; iit2<leptonIDs.end(); ++iit2) {
            int q1=theEvent[*iit].charge();
            int q2=theEvent[*iit2].charge();
            if (opposite && (q1*q2>0)) continue;
            double px1=theEvent[*iit].px();
            double py1=theEvent[*iit].py();
            double pz1=theEvent[*iit].pz();
            double mass1=theEvent[*iit].mSel();
            double e1=sqrt(px1*px1+py1*py1+pz1*pz1+mass1*mass1);
            double px2=theEvent[*iit2].px();
            double py2=theEvent[*iit2].py();
            double pz2=theEvent[*iit2].pz();
            double mass2=theEvent[*iit2].mSel();
            double e2=sqrt(px2*px2+py2*py2+pz2*pz2+mass2*mass2);
            double eSum=e1+e2;
            double pxSum=px1+px2;
            double pySum=py1+py2;
            double pzSum=pz1+pz2;
            double M=sqrt(eSum*eSum-pxSum*pxSum-pySum*pySum-pzSum*pzSum);
            if (M>massCut) {
                passesCuts=true;
                ATH_MSG_DEBUG("Acceptable lepton pair with invariant mass : " << M);
                break;
            }
        }
        if (passesCuts) {accString=" : ACCEPTED"; break;}
    }
    ATH_MSG_DEBUG("Dilepton selection" << accString);
    return passesCuts;
}

///////////////////////////////////////////////////////////////////////////////
// Iteratively descends through a decay chain, recording all particles it comes
// across
void Pythia8B_i::descendThroughDecay(Pythia8::Event &theEvent, std::vector<Pythia8::Particle> &list, int i) {
    
    list.push_back(theEvent[i]);
    std::vector<int> childrenIndices = theEvent.daughterList(i);
    std::vector<int>::iterator iItr;
    for (iItr = childrenIndices.begin(); iItr != childrenIndices.end(); ++iItr) {
        descendThroughDecay(theEvent,list,*iItr);
    }
    
    return;
    
}

///////////////////////////////////////////////////////////////////////////////
// Given a vector of Pythia8 particles, returns their PDG codes
std::vector<int> Pythia8B_i::getCodes(std::vector<Pythia8::Particle> theParticles) {
    
    std::vector<int> codes;
    for (std::vector<Pythia8::Particle>::iterator pItr = theParticles.begin(); pItr!=theParticles.end(); ++pItr ) {
        codes.push_back( (*pItr).id() );
    }
    return codes;
    
}

///////////////////////////////////////////////////////////////////////////////
// Compares two vectors of integers (any order) and if they are the same, returns
// true
bool Pythia8B_i::compare(std::vector<int> vect1, std::vector<int> vect2) {
    
    if (vect1.size()!=vect2.size()) return false;
    
    bool isTheSame(true);
    int size = vect1.size();
    std::sort(vect1.rbegin(), vect1.rend());
    std::sort(vect2.rbegin(), vect2.rend());
    for (int i=0; i<size; ++i) {
        if (vect1[i] != vect2[i]) {isTheSame = false; break;}
    }
    return isTheSame;
    
}

///////////////////////////////////////////////////////////////////////////////
// Checks a vector of particles against a vector of pt and eta cuts and if all
// particles do not pass the cuts, returns false
// Last argument controls whether the cut is on pt or eta
bool Pythia8B_i::passesCuts(std::vector<Pythia8::Particle> theParticles, std::vector<double> cuts, std::string cutType) {
    
    bool pass(true);
    std::vector<Pythia8::Particle>::iterator pItr;
    unsigned int i(0);
    for (pItr=theParticles.begin(); pItr!=theParticles.end(); ++pItr,++i) {
        if (cutType=="PT") {if ((*pItr).pT() < cuts[i]) pass = false;}
        if (cutType=="ETA") {if (fabs((*pItr).eta()) > cuts[i]) pass = false;}
        if (!pass) break;
    }
    
    return pass;
    
}

///////////////////////////////////////////////////////////////////////////////
// Given an event, checks whether it contains a decay process as defined by the
// user and if so whether the final states pass user-defined cuts
bool Pythia8B_i::signalAccept(Pythia8::Event &theEvent, std::vector<int> requiredDecay,
                              std::vector<double> signalPtCuts,
                              std::vector<double> signalEtaCuts,
			      unsigned int nRequired) {
    
    bool acceptEvent(false);
    std::vector<int> parentsIndices;
    for (int i = 0; i<theEvent.size(); ++i) {
        Pythia8::Particle theParticle = theEvent[i];
        int id = theParticle.id();
        if (id==requiredDecay[0]) parentsIndices.push_back(i);
    }
    
    // For the special case where the user merely requires a particular
    // species to be in the event (mainly for EvtGen use)
    if ( (requiredDecay.size()==1) && (parentsIndices.size()>0) ) {
        acceptEvent = true;
        return acceptEvent;
    }
    
    unsigned int goodDecayCounter(0);
    std::vector<Pythia8::Particle>::iterator pItr;
    for (std::vector<int>::iterator iItr = parentsIndices.begin(); iItr!=parentsIndices.end(); ++iItr) {
        std::vector<Pythia8::Particle> decayMembers;
        descendThroughDecay(theEvent,decayMembers,*iItr);
        std::vector<int> pdgCodes = getCodes(decayMembers);
        if (!compare(requiredDecay,pdgCodes)) {
            ATH_MSG_DEBUG("Signal event REJECTED as does not contain required decay chain");
            continue;
        }
        ATH_MSG_DEBUG("Event contains required signal decay chain");
        
        if (decayMembers.size()==signalPtCuts.size()) {
            if (!passesCuts(decayMembers,signalPtCuts,"PT")) {
                ATH_MSG_DEBUG("Signal event REJECTED as signal chain does not pass pt cuts");
                continue;
            }
        }
        
        if (decayMembers.size()==signalEtaCuts.size()) {
            if (!passesCuts(decayMembers,signalEtaCuts,"ETA")) {
                ATH_MSG_DEBUG("Signal event REJECTED as signal chain does not pass eta cuts");
                continue;
            }
        }
        if (nRequired==1) {
        	ATH_MSG_DEBUG("Signal decay good; event ACCEPTED");
        	acceptEvent = true;
        	break;
	}
	else {++goodDecayCounter;}
    }
    if (nRequired>1) {
	if (goodDecayCounter>=nRequired) {
		ATH_MSG_DEBUG(nRequired << "signal decays required; " << goodDecayCounter << " found; event ACCEPTED");
                acceptEvent = true;
        }
	else if (goodDecayCounter<nRequired) {
                ATH_MSG_DEBUG(nRequired << "signal decays required; " << goodDecayCounter << " found; event REJECTED");
                acceptEvent = false;
	}
    }
	
    return acceptEvent;
    
}

///////////////////////////////////////////////////////////////////////////////
//// Prints out signal cuts for display at the end of the job

void Pythia8B_i::printSignalSelections(std::vector<int> signalProcess, std::vector<double> ptCuts, std::vector<double> etaCuts, unsigned int nRequired ) {
        std::cout << "Signal PDG codes required: ";
        for (unsigned int k=0; k<m_sigCodes.size(); ++k) std::cout << signalProcess[k] << " ";
        if (signalProcess.size()==ptCuts.size()) {
            std::cout << "Cuts on the pt of the signal states: " << std::endl;
            for (unsigned int l=0; l<ptCuts.size(); ++l) std::cout << ptCuts[l] << " ";
        }
        if (signalProcess.size()==etaCuts.size()) {
            std::cout << "Cuts on the eta of the signal states: " << std::endl;
            for (unsigned int l=0; l<etaCuts.size(); ++l) std::cout << etaCuts[l] << " ";
        }
	std::cout << "Number of such decays required per event: " << nRequired << std::endl;
        std::cout << std::endl;
}


