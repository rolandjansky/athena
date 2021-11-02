void processSample() {
    gROOT->ProcessLine(".L validateDigitization.C++");
    gROOT->ProcessLine("validateDigitization myValidator;");
    gROOT->ProcessLine("myValidator.Loop()");

    //  gROOT->ProcessLine(".x showResHistos.C")
}
