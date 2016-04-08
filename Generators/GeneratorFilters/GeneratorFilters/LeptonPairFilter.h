/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// File:  GeneratorFilters/LeptonPairFilter.h
// Description:
// Classify Pairs of leptons according to their flavor and sign combinations
// and filter on these classifications
// Same-Flavor Opposite-Sign (SFOS)
// Same-Flavor Same-Sign (SFSS)
// Opposite-Flavor Opposite-Sign (OFOS)
// Opposite-Flavor Same-Sign(OFSS)
// AuthorList:
//         A Long:  May 2014


#ifndef GENERATORFILTERSLEPTONPAIRFILTER_H
#define GENERATORFILTERSLEPTONPAIRFILTER_H

#include "GeneratorModules/GenFilter.h"




class LeptonPairFilter:public GenFilter {
public:
        LeptonPairFilter(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~LeptonPairFilter();
        virtual StatusCode filterInitialize();
        virtual StatusCode filterFinalize();
        virtual StatusCode filterEvent();

private:
	// Setable Properties:-

	// Local Member Data:-
  	int m_nSFOS_Max;
  	int m_nSFOS_Min;
  	int m_nSFSS_Max;
  	int m_nSFSS_Min;
  	int m_nOFOS_Max;
  	int m_nOFOS_Min;
  	int m_nOFSS_Max;
  	int m_nOFSS_Min;
	int m_nPairSum_Max;
	int m_nPairSum_Min;
	bool m_onlyMassiveParents;
	bool m_bUseSFOSInSum;
	bool m_bUseSFSSInSum;
	bool m_bUseOFOSInSum;
	bool m_bUseOFSSInSum;
	double m_Ptmin;
	double m_EtaRange;
	int  m_nLeptons_Max;
	int  m_nLeptons_Min;
	// Private Methods:=

};

#endif
