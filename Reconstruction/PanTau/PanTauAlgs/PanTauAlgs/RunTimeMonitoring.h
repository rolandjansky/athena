/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAU_RUNTIMEMONITORING_H
#define PANTAU_RUNTIMEMONITORING_H

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "TH1F.h"
#include "TString.h"

#include "GaudiKernel/ITHistSvc.h"


namespace PanTau {

    typedef	std::map<std::string, TH1F*>	HistMap;
    
    class RunTimeMonitoring {
            
        private:
            
            bool                m_IsActivated;      //if this is false, functions may be called but not executed
                                                    //not ideal, but maybe better than have an if statement in SeedBuilder in front of every call to m_RTH...
            ITHistSvc*          m_THistSvc;         //this is not owned by this class (externally created and passed)
            HistMap             m_HistMap;
            
            void                CreateHist1D(std::string Name, std::string xTitle, int nBins, double MinBin, double MaxBin);
            
        public:
            
            RunTimeMonitoring( bool isActive);
            RunTimeMonitoring( const RunTimeMonitoring& other );
            ~RunTimeMonitoring();
            
            RunTimeMonitoring operator=( RunTimeMonitoring rhs );
            
            void       SetTHistSvcLink(ITHistSvc* histSvc);
            void	    CreateHist1DMap(	
                                        std::vector<std::string> 	Names,
                                        std::vector<std::string> 	xTitles,
                                        std::vector<int> 			nBins,
                                        std::vector<double>		MinBins,
                                        std::vector<double>		MaxBins
                                        );
            bool       RegisterHistos();
            
            //call this for variables that are not added via AddFooBarVariable(...)
            void	    FillHisto(std::string itsName, double fillValue);
            
    
    };
}

#endif