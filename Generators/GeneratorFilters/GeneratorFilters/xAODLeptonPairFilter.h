/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// File:  GeneratorFilters/xAODLeptonPairFilter.h
// Description:
// Classify Pairs of leptons according to their flavor and sign combinations
// and filter on these classifications
// Same-Flavor Opposite-Sign (SFOS)
// Same-Flavor Same-Sign (SFSS)
// Opposite-Flavor Opposite-Sign (OFOS)
// Opposite-Flavor Same-Sign(OFSS)
// AuthorList:
//         A Long:  May 2014


#ifndef GENERATORFILTERSXAODLEPTONPAIRFILTER_H
#define GENERATORFILTERSXAODLEPTONPAIRFILTER_H

#include "GeneratorModules/GenFilter.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"




class xAODLeptonPairFilter:public GenFilter {
public:
        xAODLeptonPairFilter(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~xAODLeptonPairFilter();
        virtual StatusCode filterInitialize();
        virtual StatusCode filterFinalize();
        virtual StatusCode filterEvent();

private:
	// Setable Properties:-
	//--------------------------------------------------------------------------    
	// //by default the filter demands that there are exactly 0 SFOS lepton pairs
	// //if both Maximum and Minimum are negative, then no requirements
	// //are made on that pair type

	// //Same-Flavor Opposite-Sign (SFOS)
	Gaudi::Property<int> m_nSFOS_Max{this,"NSFOS_Max",0,"Number of Same Flavor Opposite Sign Max"};
	Gaudi::Property<int> m_nSFOS_Min{this,"NSFOS_Min",0,"Number of Same Flavor Opposite Sign Min"};

	// //Same-Flavor Same-Sign (SFSS)
	Gaudi::Property<int> m_nSFSS_Max{this,"NSFSS_Max",-1,"Number of Same Flavor Same Sign Max"};
	Gaudi::Property<int> m_nSFSS_Min{this,"NSFSS_Min",-1,"Number of Same Flavor Same Sign Min"};

	// //Opposite-Flavor Opposite-Sign (OFOS)
	Gaudi::Property<int> m_nOFOS_Max{this,"NOFOS_Max",-1,"Number of Opposite Flavor Opposite Sign Max"};
	Gaudi::Property<int> m_nOFOS_Min{this,"NOFOS_Min",-1,"Number of Opposite Flavor Opposite Sign Min"};

	// //Opposite-Flavor Same-Sign (OFSS)
	Gaudi::Property<int> m_nOFSS_Max{this,"NOFSS_Max",-1,"Number of Opposite Flavor Same Sign Max"};
	Gaudi::Property<int> m_nOFSS_Min{this,"NOFSS_Min",-1,"Number of Opposite Flavor Same Sign Min"};

	// //Pair Sum (ask for some combination of different 
	// //lepton pairs based on configuration)
	Gaudi::Property<int> m_nPairSum_Max{this,"NPairSum_Max",-1,"Number of Pair sum Max"};
	Gaudi::Property<int> m_nPairSum_Min{this,"NPairSum_Min",-1,"Number of Pair sum Min"};

	// //configure which pairs to consider in sum 
	// //by default don't consider any 
	Gaudi::Property<bool> m_bUseSFOSInSum{this,"UseSFOSInSum",false,"SFOS to be considered in sum"};
	Gaudi::Property<bool> m_bUseSFSSInSum{this,"UseSFSSInSum",false,"SFSS to be considered in sum"};
	Gaudi::Property<bool> m_bUseOFOSInSum{this,"UseOFOSInSum",false,"OFOS to be considered in sum"};
	Gaudi::Property<bool> m_bUseOFSSInSum{this,"UseOFSSInSum",false,"OFSS to be considered in sum"};
	
	// // only count leptons with a parent with M>20 GeV
	// declareProperty("OnlyMassiveParents",m_onlyMassiveParents = false);
	Gaudi::Property<bool> m_onlyMassiveParents{this,"OnlyMassiveParents",false,"only count leptons with a parent with M>20 GeV"};
	
	// //kinematic requirements on leptons to consider
	// //this is NOT a veto on the leptons outside the requirements
	Gaudi::Property<double> m_Ptmin{this,"Ptcut",10000.,"min Pt"};
	Gaudi::Property<double> m_EtaRange{this,"Etacut",10.,"Eta range max"};

	// //the filter is only applied if the 
	// //if the number of leptons falls within this range
	// //otherwise the filter automatically passes
	// //if max < 0 it is ignored
	Gaudi::Property<int> m_nLeptons_Max{this,"NLeptons_Max",-1," max num leptons"};
	Gaudi::Property<int> m_nLeptons_Min{this,"NLeptons_Min",0," min num leptons"};


};

#endif
