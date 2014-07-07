//****************************************************************************
//
// TWave2NTuple.C - Convert ASCII file of FCal waves into a ROOT NTuple
// Author - Alan Robinson (fbfree@interchange.ubc.ca)
// Created - Aug 15, 2007
//
//****************************************************************************

{
// ASCII file preparations
  const int nPoints = 629;  // number of data points to read from the text file.
  // nBins (initialized later) is the number of ns associated with these data points
  ifstream infile("./signalshapes.dat");
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
    // The 2nd array index is chosen to facilitate later logic. See Int_t waveType.
    infile >> sample >> input[i][0] >> input[i][1] >> input[i][4] >> input[i][2] >> input[i][5] >> input[i][3] >> input[i][6];
  }
  infile.close();

  // Automatically find the length of the extracted wave in ns; assuming start at t=0.
  const int nBins = ceil(input[nPoints-1][0]);  

// Create ROOT File with TTree.
  TFile outfile("FCalWave.root", "RECREATE");

  Int_t side, chan, feb, gain, endTime;
  Double_t wave[nBins];
  const Int_t one = 1, six = 6;

  TTree outtree("outfit", "LArPhysWave");
  outtree.Branch("BarEC", &one, "Code/I");
  outtree.Branch("Side", &side, "Code/I");
  outtree.Branch("FT", &six, "Code/I");
  outtree.Branch("Channel", &chan, "Code/I");
  outtree.Branch("Slot", &feb, "Code/I");
  char code[16];
  sprintf(code, "Code[%i]/D", nBins);
  outtree.Branch("tphys", wave, code);
  outtree.Branch("Gain", &gain, "Code/I");

// Fill the TTree
  // Loop over all applicable channels
  Int_t timeBin;
  Int_t waveType = 1;
  Double_t deltaT = input[1][0] - input[0][0]; // Time correction for 
  // midpoint approximation.  It'd otherwise be a right Riemann approx.
// WaveType Table
// Gain \ FEB | 1-9 (FCal1) | 10-13 (FCal2) | 14 & 15 (FCal3)
// 0 or 1     |     1       |       2       |       3
// 2          |     4       |       5       |       6

  for (side=0; side<=1; side++){ // 0-EndCap C, 1-EndCap A
    for (gain=0; gain<3; gain++){ // 0-High Gain, 1-Medium Gain, 2-Low Gain
      switch (gain){
        case 0:
          waveType = 1; // Switched side. Restart
          break;
        case 1:
          waveType -= 2; // Undo feb increments
          break;
        case 2:
          waveType++; // Increment with high gain
          break;
      }
      for (feb=1; feb<=15; feb++){
        if(feb==8)
          feb++;
        if(feb==10 || feb==14)
          waveType++; // Increment with new module
        cout << waveType << " " << feb << " " << gain << endl;
        for (chan=0; chan<128; chan++){

  // Rebin data, and write out
          timeBin = 0;
          for (int i=1; i < nPoints; i++){
  // See note at initialization of deltaT
  // If changing bins
            if ( (int) floor((input[i][0]+deltaT/2)) != timeBin){
  // Finish filling current bin
              wave[timeBin] += input[i][waveType]*(floor(input[i][0]+deltaT/2)-(input[i-1][0]+deltaT/2));
  // Increment timeBin
              timeBin = (int) (input[i][0]+deltaT/2);
  // Start filling next bin
              wave[timeBin] = input[i][waveType]*(input[i][0]+deltaT/2-floor(input[i][0]+deltaT/2));
            } else {
  // Just fill the bin
              wave[timeBin] += input[i][waveType]*(deltaT);
            }
          }
  // Last bin will not be fully filled.  Set to 0.
          wave[nBins-1] = 0.0;
          outtree.Fill();
        }
      }
    }
  }
  outtree.Write();
  outfile.Close();
}
