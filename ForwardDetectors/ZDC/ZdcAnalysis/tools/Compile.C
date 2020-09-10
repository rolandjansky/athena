{
    gROOT->ProcessLine(".include ../../ZdcAnalysis/");
    gROOT->ProcessLine(".L ../Root/ZDCFitWrapper.cxx++");
    gROOT->ProcessLine(".L ../Root/ZDCPulseAnalyzer.cxx++");
    gROOT->ProcessLine(".L ../Root/ZDCDataAnalyzer.cxx++");
    gROOT->ProcessLine(".L ZDCTreeAnalysis.C++");
}
