/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryDPDMaker_TimingFilterInformation_H
#define PrimaryDPDMaker_TimingFilterInformation_H

/// Simple helper struct used to obtain the timing information by the 
///   IMBTSTimeFilter and ICaloTimerFilterTool
struct TimingFilterInformation{
        
        bool passCut{false};
        
        double timeDiff{0};
        
        double timeA{0};
        
        double timeC{0};
        
        int ncellA{0};
        int ncellC{0};
        
};
#endif