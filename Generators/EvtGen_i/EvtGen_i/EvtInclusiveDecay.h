/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//
// Generators/EvtGen_i/EvtInclusiveDecay.h
//
// $Id: EvtInclusiveDecay.h,v 1.5 2007-03-01 23:23:44 binet Exp $
//
// EvtInclusiveDecay is a TopAlg that takes HepMC events from StoreGate and
// generates particle decays using EvtGen. Depending on job options either all or
// only a subset of the particles which have decays defined in the EvtGen
// decay files will be handed to EvtGen. Both undecayed particles and particles
// with an existing decay tree can be handled (in the latter case,
// EvtInclusiveDecay will remove the existing decay tree).
//
// Written in February 2006 by Juerg Beringer, based in part on the existing
// EvtDecay module.
//
//*****************************************************************************

#ifndef GENERATORMODULESEVTDECAYINCLUSIVE_H
#define GENERATORMODULESEVTDECAYINCLUSIVE_H

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"

#include "HepPDT/ParticleDataTable.hh"

#include "EvtGenBase/EvtParticle.hh"
#include "EvtGen/EvtGen.hh"
#include "EvtGenBase/EvtRandomEngine.hh"

#include "GeneratorModules/GenBase.h"
//#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ISvcLocator.h"

//#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GeneratorObjects/McEventCollection.h"


class IAtRndmGenSvc;

class EvtInclusiveAtRndmGen : public EvtRandomEngine {
	public:
  EvtInclusiveAtRndmGen(IAtRndmGenSvc* atRndmGenSvc, std::string streamName);
		virtual ~EvtInclusiveAtRndmGen();
		double random();
	
	private:
		IAtRndmGenSvc*    m_atRndmGenSvc;
		std::string            m_streamName;
};



class EvtInclusiveDecay:public GenBase {
	
        public:
		EvtInclusiveDecay(const std::string& name, ISvcLocator* pSvcLocator);
		virtual ~EvtInclusiveDecay();

		StatusCode initialize();
		StatusCode execute();
		StatusCode finalize();
		std::string xmlpath(void); 		
	private:
	
#ifdef HEPMC3
		StatusCode traverseDecayTree(HepMC::GenParticlePtr p,
					     bool isToBeRemoved,
					     std::set<HepMC::GenVertexPtr>& visited,
					     std::set<HepMC::GenParticlePtr>& toBeDecayed);
#else
		StatusCode traverseDecayTree(HepMC::GenParticlePtr p,
					     bool isToBeRemoved,
					     std::set<HepMC::GenVertexPtr>& visited,
					     std::set<int>& toBeDecayed);
#endif
		void removeDecayTree(HepMC::GenEvent* hepMC, HepMC::GenParticlePtr p);
		void decayParticle(HepMC::GenEvent* hepMC, HepMC::GenParticlePtr p);
		void addEvtGenDecayTree(HepMC::GenEvent* hepMC, HepMC::GenParticlePtr part,
					EvtParticle* evtPart, EvtVector4R treeStart,
                                        double momentumScaleFactor = 1.0);

		bool isToBeDecayed(const HepMC::GenParticlePtr p, bool doCrossChecks);
		bool isDefaultB(const int pId) const;
    
    bool passesUserSelection(HepMC::GenEvent* hepMC);
    double invMass(const HepMC::GenParticlePtr p1, const HepMC::GenParticlePtr p2);

		// Utility functions to print HepMC record for debugging with optional
		// coloring by status code and highlighting of particles in a specific list of barcodes
#ifdef HEPMC3
		void printHepMC(HepMC::GenEvent* hepMC, std::set<HepMC::GenParticlePtr>* barcodeList = nullptr);
		unsigned int printTree(HepMC::GenParticlePtr p, std::set<HepMC::GenVertexPtr>& visited, int level, std::set<HepMC::GenParticlePtr>* barcodeList = nullptr);
		std::string pdgName(const HepMC::GenParticlePtr p, bool statusHighlighting = false, std::set<HepMC::GenParticlePtr>* barcodeList = nullptr);
#else
		void printHepMC(HepMC::GenEvent* hepMC, std::set<int>* barcodeList = nullptr);
		unsigned int printTree(HepMC::GenParticlePtr p, std::set<HepMC::GenVertexPtr>& visited,
				       int level, std::set<int>* barcodeList = 0);
		std::string pdgName(const HepMC::GenParticlePtr p, bool statusHighlighting = false, std::set<int>* barcodeList = nullptr);
#endif
      
		// StoreGate access
		//		StoreGateSvc* m_sgSvc;
		McEventCollection* m_mcEvtColl;
  
		// Particle properties service
		//const HepPDT::ParticleDataTable* m_pdt;

		// EvtGen interface
		IAtRndmGenSvc*          m_atRndmGenSvc;
		EvtInclusiveAtRndmGen*  m_evtAtRndmGen;
		EvtGen*                 m_myEvtGen;
	
		// jobOption params
		std::string m_pdtFile;
		std::string m_decayFile;
		std::string m_userDecayFile;
		std::string m_randomStreamName;
		std::string m_inputKeyName;
		std::string m_outputKeyName;
  
    bool m_readExisting;
		bool m_prohibitFinalStateDecay;
		bool m_prohibitReDecay;
		bool m_prohibitUnDecay;
		bool m_prohibitRemoveSelfDecay;
		std::vector<int> m_blackList;
		std::set<int> m_blackListSet;   // filed from m_blackList for speed optimization

		bool m_allowAllKnownDecays;
		bool m_allowDefaultBDecays;
		std::vector<int> m_whiteList;
		std::set<int> m_whiteListSet;   // filed from m_whilteList for speed optimization

    /// The status of decayed particles
    int m_decayedStatus;
  
		bool m_printHepMCBeforeEvtGen;
		bool m_printHepMCAfterEvtGen;
		bool m_printHepMCHighlighted;
		bool m_printHepMCHighLightTopLevelDecays;

		bool m_checkDecayTree;
		bool m_checkDecayChannels;
		std::map<int,long> m_noDecayChannels;
    
    int m_nRepeatedDecays;
    
    int m_maxNRepeatedDecays;
    
    bool m_applyUserSelection;
    bool m_userSelRequireOppositeSignedMu;
    double m_userSelMu1MinPt;
    double m_userSelMu2MinPt;
    double m_userSelMu1MaxEta;
    double m_userSelMu2MaxEta;
    double m_userSelMinDimuMass;
    double m_userSelMaxDimuMass;
    bool m_isfHerwig;    
};

#endif
