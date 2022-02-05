void go() {
    gROOT->ProcessLine(".L RPCpanelList.C++");

    gROOT->ProcessLine(".x displayRPCpanelEfficiency.C(2)");  // 2=2gaps&2views together, 1=2views together, 0=12maps

    // gROOT->ProcessLine(".x plotEffOnPanelMap.C(1)");  //for plots with boxes (12 for the entire detector)
    /////gROOT->ProcessLine(".x plotSelectedPanels.C(1)");  // buggy BOG panels
    // gROOT->ProcessLine(".x plotEffOn2Panels2Gaps_new.C(1)");  //for plots with triangles (3 for the entire detector)
    // gROOT->ProcessLine(".x plotEffOn2Panels.C(1)");  //for plots with triangles (3 for the entire detector)
}
