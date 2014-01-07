/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  TH1F *myProfile;
  if ( myProfile ) myProfile->Reset();
  else myProfile=new TH1F("ClusterNum","Number of cluster",100,0.,Track_Chain->GetMaximum("EventNumber"));
  int detType   = 0;
  int layer     = 0;
  int phi_index = 0;
  int eta_index = 0;
  cout << "Identify module:" << endl;
  cout << "End-Cap A=1, Barrel=0, End-Cap C=-1" << endl;
  cin >> detType;
  if (detType==0) {
    cout << "Layer (0,1,2)"<< endl;
    cin >> detType;
    cout << "Module phi index " ;
    if ( detType==0 ) cout << "(0,21)" << endl;
    else if ( detType==1 ) cout << "(0,37)" << endl; 
    else cout << "(0,51)" << endl; 
    cin >> phi_index;
    cout << "Module eta index (-6,6)" << endl;
    cin >> eta_index;
  } else {
    cout << "Disk (1,2,3)"<< endl;
    cin >> layer;
    detType*=(99+layer);
    cout << "Module phi index (0-47)" << endl;
    cin >> phi_index;
  }
  char cut[255];
  sprintf (cut,"PixDetType==%d && PixPhiModule==%d && PixEtaModule==%d",detType,phi_index,eta_index);
  Track_Chain->Draw("EventNumber>>ClusterNum",cut);
}
