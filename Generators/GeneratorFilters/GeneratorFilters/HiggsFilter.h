/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
//  File:  GeneratorFilters/Higgsfilter.h
//  Description:
//
//  This filter requires the event to contain at least 
//  one Higgs with pt at or above Ptcut.
//
//  Events that do not contain H are rejected.
//
//  Only H decaying to b bbar are analyzed and counted in this algorithm. 
//
//  Originally created using GeneratorFilters templates for DC3 purposes.
//
//  AuthorList:
//  
//  M. Casolino:  February 5, 2014
//


#ifndef GENERATORFILTERSHIGGSFILTER_H
#define GENERATORFILTERSHIGGSFILTER_H

#include "GeneratorModules/GenFilter.h"

class HiggsFilter:public GenFilter {
public:
        HiggsFilter(const std::string& name, ISvcLocator* pSvcLocator);
	virtual StatusCode filterEvent();

private:
	// Setable Properties:-

	// Local Member Data:-
	double m_Ptmin;
	double m_Ptmax;
	// Private Methods:=

};

#endif
