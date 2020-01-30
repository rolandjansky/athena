{
    gROOT->ProcessLine(".include ../ZdcAnalysis/");
    gROOT->ProcessLine(".L ../ZdcAnalysis/Root/ZDCFitWrapper.cxx++");
    gROOT->ProcessLine(".L ../ZdcAnalysis/Root/ZDCPulseAnalyzer.cxx++");
    gROOT->ProcessLine(".L ../ZdcAnalysis/Root/ZDCDataAnalyzer.cxx++");
    gROOT->ProcessLine(".L ZDCTreeAnalysis.C++");
    // gROOT->ProcessLine(".L RunZDCTreeAnalysis2016.C");
}
