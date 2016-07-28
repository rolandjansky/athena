/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "dataflow.h"
#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"


int main(int argc, char **argv) {
  events = -1;
  ntower = MAXTOWER;

  try {
    std::string appName = boost::filesystem::basename(argv[0]);
    // Define and parse the program options
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
      ("help,h", "Print this help message")
      ("output,o", po::value<std::string>(&output)->default_value("./output.txt"), "The name of the output file")
      ("events,e", po::value<int>(&events)->default_value(-1), "The number of events to run over. Set to -1 to use all events")
      ("ntower,n", po::value<int>(&ntower)->default_value(MAXTOWER), "The number of towers")
      ("files", po::value<std::vector<std::string> >(&files)->multitoken(), "FTK NTUP files");

    po::variables_map vm;
    try
    {
      po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
      // --help option
      if (vm.count("help"))
      {
        cout << desc << "\n";
        return 1;
      }
      po::notify(vm);
    }
    catch(std::exception& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      return 1;
    }

    Init();

    for (unsigned int ientry = 0; ientry < nloop; ++ientry) {
      Process(ientry);
    }
    Terminate();
  } // end try
  catch(std::exception& e)
  {
    std::cerr << "Unhandled Exception reached the top of main: "
              << e.what() << ", application will now exit" << std::endl;
    return 1;
  }
  return 0;
}

void Init() {


  myfile.open (output);
  
  for (int i=0; i < MAXTOWER; i++) {
    stream[i] = 0;
    trackstream[i] = 0;
    nRoad[i] = 0;
    nFit[i] = 0;
    nFitRecovery[i] = 0;
    nFitRecoveryI[i] = 0;
    nFitMajority[i] = 0;
    nFitMajorityI[i] = 0;
    nFitMajorityPix[i] = 0;
    nFitMajorityPixI[i] = 0;
    nFitMajoritySCT[i] = 0;
    nFitMajoritySCTI[i] = 0;
    nTrack[i] = 0;
    nFitI[i] = 0;
    nTrackI[i] = 0;
    nTrackBeforeHW[i] = 0;
    for (int k=0; k<8; k++) { nCluster[k][i]=0; nSSID[k][i] = 0;}
  }
  

  ch = 0;
  ch = new TChain("ftkdata");

  // add the input files
  std::vector<std::string>::const_iterator files_it = files.begin();
  for (; files_it != files.end(); ++files_it) {
    ch->Add((*files_it).c_str());
  }
  
  
  nloop = ch->GetEntries();

  if (events > 0) {
    nloop = (unsigned int) events;
  }
  if (events > ch->GetEntries()) {
    nloop = ch->GetEntries();
  }

  divide = 1./nloop;

  for (int itower = 0; itower < ntower; itower++) {
    ch->SetBranchAddress(Form("FTKMergedTracksStream%d",itower),&trackstream[itower]);
    ch->SetBranchAddress(Form("FTKMergedRoadsStream%d",itower),&stream[itower]);
  }
}

void Process(unsigned int ientry) {
  
  ch->GetEntry(ientry);
  Int_t thisrun = stream[0]->runNumber();
  Int_t thisevt = stream[0]->eventNumber();
  if (ientry % 50 == 0) cerr << "ientry = " << ientry << " and run = " << thisrun << " and event = " << thisevt << endl;
  

  for (int itower = 0; itower < ntower; itower++) {
    if (trackstream[itower] == 0) { cerr << "ERROR: Track stream " << itower << " = 0 " << std::endl; return;}
    if (stream[itower] == 0) { cerr << "ERROR: Road stream " << itower << " = 0 " << std::endl; return;}
    
      nRoad[itower] += stream[itower]->naoGetNroadsAM()*divide;
      nFit[itower] += trackstream[itower]->getNFits()*divide;
      nFitI[itower] += trackstream[itower]->getNFitsI()*divide;
      nTrack[itower] += trackstream[itower]->getNTracks()*divide;
      nTrackI[itower] += trackstream[itower]->getNTracksI()*divide;

      Int_t ntrackBeforeHW = trackstream[itower]->getNTracks() + trackstream[itower]->getNFitsHWRejected();

      nTrackBeforeHW[itower] += ntrackBeforeHW*divide;

      nFitRecoveryI[itower] += trackstream[itower]->getNFitsRecoveryI()*divide;
      nFitRecovery[itower] += trackstream[itower]->getNFitsRecovery()*divide;

      nFitMajorityI[itower] += trackstream[itower]->getNFitsMajorityI()*divide;
      nFitMajority[itower] += trackstream[itower]->getNFitsMajority()*divide;

      nFitMajoritySCTI[itower] += trackstream[itower]->getNFitsMajorityI_SCT()*divide;
      nFitMajoritySCT[itower] += trackstream[itower]->getNFitsMajority_SCT()*divide;

      nFitMajorityPixI[itower] += trackstream[itower]->getNFitsMajorityI_pix()*divide;
      nFitMajorityPix[itower] += trackstream[itower]->getNFitsMajority_pix()*divide;

      for (int il = 0; il < 8; il++) {
         nCluster[il][itower] += stream[itower]->naoGetNclus(il)*divide;
         nSSID[il][itower] += stream[itower]->naoGetNss(il)*divide;
      }

  }
}

void Terminate() {
  myfile << "Type\t\tbarrelmean\t\tbarrelmax\t\tendcapmean\t\tendcapmax" << endl;
  myfile << "--------------" << endl;

  // kludge, can do this better but works for now
  float temp[MAXTOWER], temp2[MAXTOWER];
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < MAXTOWER; j++) {
      temp[j] = nCluster[i][j];
      temp2[j] = nSSID[i][j];
    }
    printinfo (temp, Form("NClusterL%d",i));
    printinfo (temp2, Form("NSSIDL%d",i));
  }
  myfile << "--------------" << endl;
  printinfo (nRoad, "NRoads");
  myfile << "--------------" << endl;
  printinfo (nFitI, "NFitAux (8/8 + 7/8)");
  printinfo (nFitRecoveryI, "NFitAux Recovery");
  printinfo (nTrackI, "NTrackAux");
  myfile << "--------------" << endl;
  printinfo (nFitMajorityI, "NFitAux Majority");
  printinfo (nFitMajoritySCTI, "NFitAux Majority missing SCT");
  printinfo (nFitMajorityPixI, "NFitAux Majority missing Pix");
  myfile << "--------------" << endl;
  printinfo (nFit, "NFitSSB (12/12 + Recovery)");
  printinfo (nFitRecovery, "NFitSSB Recovery");
  printinfo (nTrack, "NTrackSSB");
  printinfo (nTrackBeforeHW, "NTrackSSB Before HW");
  myfile << "--------------" << endl;
  printinfo (nFitMajority, "NFitSSB Majority");
  printinfo (nFitMajoritySCT, "NFitSSB Majority missing SCT");
  printinfo (nFitMajorityPix, "NFitSSB Majority missing Pix");
  myfile.close();
}


void printinfo(float towers[MAXTOWER], TString text) {

  float barrelmean(0), endcapmean(0), barrelmax(0), endcapmax(0);
  for (int i = 0; i<ntower; i++) {
    if (i < (ntower/4) || i >= 3*(ntower/4.)) { // kludge not always (?) guaranteed to work
      if (towers[i] > endcapmax) endcapmax = towers[i];
      endcapmean += towers[i]/(ntower/2.);
    }
    else {
      if (towers[i] > barrelmax) barrelmax = towers[i];
      barrelmean += towers[i]/(ntower/2.);
    }
  }
  myfile << text << "\t\t" << barrelmean << "\t\t" << barrelmax << "\t\t" << endcapmean << "\t\t" << endcapmax << endl;
}
