/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
*/
// --------------------------------------------------
// 
// File:  GeneratorFilters/ParentsTracksFilter.h
// Description:
//   Allows the user to search for a few particles (parents)
//   decaying to final states containing stable charged (e+-, mu+-, pi+-, K+-, p+-)
//   particles (tracks): Parent_1/2/.. -> tracks_1 + track_2 + track_... + X.
//   Up to 4 consequitive decays are considered.
//
// AuthorList:         
//   B K Gjelsten   March 2006 (author of preceding ParentChildFilter)
//   L K Gladilin (gladilin@mail.cern.ch)  March 2016


#ifndef GENERATORFILTERSPARENTSTRACKSFILTER_H
#define GENERATORFILTERSPARENTSTRACKSFILTER_H

#include "GeneratorModules/GenFilter.h"




class ParentsTracksFilter:public GenFilter {
public:
        ParentsTracksFilter(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~ParentsTracksFilter();
        virtual StatusCode filterInitialize();
        virtual StatusCode filterFinalize();
        virtual StatusCode filterEvent();

private:
	// Setable Properties:-

	// Local Member Data:-
	std::vector<int> m_PDGParent;
	double m_PtMinParent;
        double m_PtMaxParent;
        double m_MassMinParent;
        double m_MassMaxParent;
	double m_EtaRangeParent;
	//
	double m_PtMinLeptons;
	double m_EtaRangeLeptons;
	double m_PtMinHadrons;
	double m_EtaRangeHadrons;
	//
        int m_NumMinTracks;
        int m_NumMaxTracks;
        int m_NumMinLeptons;
        int m_NumMaxLeptons;
        int m_NumMinOthers;
        int m_NumMaxOthers;

	// Private Methods:=

};

#endif
