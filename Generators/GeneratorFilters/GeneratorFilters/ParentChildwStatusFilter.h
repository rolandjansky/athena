/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// File:  GeneratorFilters/ParentChildwStatusFilter.h
// Description:
//   Allows the user to search for any given decay  Parent -> Child + X with particle status
//
// AuthorList:
//   Tatsuya Masubuchi   April 2013


#ifndef GENERATORFILTERSPARENTCHILDWSTATUSFILTER_H
#define GENERATORFILTERSPARENTCHILDWSTATUSFILTER_H

#include "GeneratorModules/GenFilter.h"




class ParentChildwStatusFilter:public GenFilter {
public:
        ParentChildwStatusFilter(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~ParentChildwStatusFilter();
        virtual StatusCode filterInitialize();
        virtual StatusCode filterFinalize();
        virtual StatusCode filterEvent();

private:
	// Setable Properties:-

	// Local Member Data:-
	std::vector<int> m_PDGParent;
	std::vector<int> m_StatusParent;
	double m_PtMinParent;
        double m_PtMaxParent;
        double m_MassMinParent;
        double m_MassMaxParent;
	double m_EtaRangeParent;
	std::vector<int> m_PDGChild;
	double m_PtMinChild;
	double m_EtaRangeChild;


	// Private Methods:=

};

#endif
