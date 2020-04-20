/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_test_mocks_MockHistogramDef_h
#define AthenaMonitoringKernel_test_mocks_MockHistogramDef_h

#include "AthenaMonitoringKernel/HistogramDef.h"

struct MockHistogramDef : public Monitored::HistogramDef {
    MockHistogramDef() {
        name = {};
        alias = "";
        type = "TH1F";
        path = "";
        title = "";
        tld = "";
        weight = "";
        cutMask = "";

        convention = "";
        runmode = RunMode::Invalid;
        runperiod = RunPeriod::Invalid;

        Sumw2 = false;
        kLBNHistoryDepth = 0;
        kAddBinsDynamically = false;
        kRebinAxes = false;
        kCanRebin = false;
        kVec = false;
        kVecUO = false;
        kCumulative = false;

        xvar = "";
        xbins = 1;
        xmin = 0.0;
        xmax = 10.0;
        xlabels = {};
        xarray = {};

        yvar = "";
        ybins = 0;
        ymin = 0.0;
        ymax = 0.0;
        ylabels = {};
        yarray = {};

        zvar = "";
        zbins = 0;
        zmin = 0.0;
        zmax = 0.0;
        zlabels = {};

        ok = false;
    }
};

#endif /* AthenaMonitoringKernel_test_mocks_MockHistogramDef_h */