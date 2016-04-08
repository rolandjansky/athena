{
TH1F* h = (TH1F*)gDirectory->Get("MTTHist/h1ftest");
if ( h == 0 ) {
    cout << "TESTMSG ERROR Missing histogram " << endl;
    gROOT->ProcessLine(".q");
}
if ( (int)h->GetEntries() != 250 ) {
    cout << "TESTMSG ERROR wrong number of entries " << endl;
    gROOT->ProcessLine(".q");
}
cout << "TESTMSG SUCCESS correct histogram exist " << endl;
gROOT->ProcessLine(".q");
}
