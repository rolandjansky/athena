//****************************************************************************
//
// LArPhysWaveCheck.C - Tool for checking the output of TWave2NTuple.C 
//           and the LArPhysWaveFromTuple algorithm.
// Author - Alan Robinson (fbfree@interchange.ubc.ca)
// Created - Aug 15, 2007
//
//****************************************************************************

{
// ASCII file preparations
  const int nPoints = 629;
  int timeOffset = 27; // Correction for Time offset introduced by LArPhysWaveFromTuple 

  ifstream infile("./signalshapes/signalshapes3.dat");
  if (!infile.good()){
    std::cerr << "File not found." << std::endl;
    return 2;
  }

  // ASCII file columns
  Double_t input[nPoints][7];
  Int_t sample;

  // Ignore header
  infile.ignore(64, '\n');
  infile.ignore(64, '\n');

// Read ASCII File
  for (int i=0; i<nPoints; i++){
    infile >> sample >> input[i][0] >> input[i][1] >> input[i][4] >> input[i][2] >> input[i][5] >> input[i][3] >> input[i][6];
  }
  infile.close();

  const int nBins = ceil(input[nPoints-1][0]);

// Open ROOT File with TTree.
  TFile checkfile("FCalWave.root");

  Int_t side, chan, feb, gain, endTime;
  Double_t wave[nBins];
  const Int_t one = 1, six = 6;

  TTree *checktree = (TTree*) checkfile.Get("outfit");
  checktree.SetBranchAddress("Slot", &feb);
  if (checktree.FindBranch("tphys")){
    checktree.SetBranchAddress("tphys", wave);  // If checking T2Ntuple.C
    timeOffset = 0;
  } else {
    checktree.SetBranchAddress("Amplitude", wave); // If checking LArPhysWaveFromTuple
  }
  checktree.SetBranchAddress("Gain", &gain);

// Fill the TTree
  // Loop over all applicable channels
  Int_t waveType = 1;
// WaveType Table
// Gain \ FEB | 1-9 (FCal1) | 10-13 (FCal2) | 14 & 15 (FCal3)
// 0 or 1     |     1       |       2       |       3
// 2          |     4       |       5       |       6

  TCanvas *c1 = new TCanvas("c1","c1");
  TH1D *h1 = new TH1D("h1","h1",nBins, timeOffset, nBins+timeOffset);
  TH1D *h2 = new TH1D("h2","h2",nPoints, 0., (nPoints)*0.3/25*24);
  h2->SetLineColor(kRed);
  Int_t event = 0;

  //while(event >= 0){
    cout << "Event (-1 to quit):" << endl;
    cin >> event;

    checktree->GetEvent(event);
    waveType = 1;
    if (gain == 2)
      waveType+=3;
    if (feb >= 10){
      waveType++;
      if (feb >= 14)
        waveType ++;
    }
    cout << waveType << " " << feb << " " << gain << " " << event << endl;

    for (int i=0; i<nBins; i++)
      h1->SetBinContent(i+1, wave[i]);
    for (int i=timeOffset; i < nPoints; i++)
      h2->SetBinContent(i+1, input[i][waveType]);

    h1->Draw();
    h2->Draw("SAME");

    c1->Modified();
    c1->Update();

  //}
  //checkfile.Close();
}
