/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <stdio.h>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include "TChain.h"
#include "TRegexp.h"
#include "TH1F.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TApplication.h"
#include "TROOT.h"

using namespace std;

// Input information
TString def_treeName="susy";
TString def_outputFile="output.root";
TString def_triggers="";
bool def_doElectron=true;
bool def_doMuon=true;
bool def_doPhoton=true;
bool def_doJet=true;
bool def_doMET=true;
bool def_doTrack=true;
bool def_doMC=true;
bool def_doMCSlim=true;
bool def_doBH=true;
bool def_doMCSkim=false;


void help(char*exe){
  cout << "Usage: " << exe << " " << "[options] [inputFile1 [inputFile2 [inputFile3...]]" << endl;
  cout << endl;
  cout << "Arguments:" << endl;
  cout << "  -n  Set tree name (default:" << def_treeName << ")" << endl;
  cout << "  -o  Set output file name (default:" << def_outputFile << ")" << endl;
  cout << "  -i  Set input file name, multiple input can be used" << endl;
  cout << "      Multiple inputs can be set in one argument with \",\" as a separator (-i file1,file2)" << endl;
  cout << "  -E  Do Electron (default:" <<  def_doElectron << ")" << endl;
  cout << "  -M  Do Muon (default: " << def_doMuon << ")" << endl;
  cout << "  -P  Do Photon (default: " << def_doPhoton << ")" << endl;
  cout << "  -J  Do Jet (default: " << def_doJet << ")" << endl;
  cout << "  -I  Do MET (default: " << def_doMET << ")" << endl;
  cout << "  -T  Do Track (default: " << def_doTrack << ")" << endl;
  cout << "  -t  Set trigger name for skimming, can set multiple triggers (default: no skim)" << endl;
  cout << "  -m  Use MC information or not (default:" << def_doMC << ")" << endl;
  cout << "  -s  Do MC slimming or not (default:" << def_doMCSlim << ")" << endl;
  cout << "  -b  Do BH truth reconstruction or not (default:" << def_doBH << ")" << endl;
  cout << "  -e  Set max events (default: all (-1))" << endl;
  cout << "  -S  Set skip events (default: (0))" << endl;
  cout << "  -f  Force skimming even for MC (default:" << def_doMCSkim << ")" << endl;
  cout << "  -h  Print Help (this message) and exit" << endl;
  cout << "  Additional arguments w/o option will be used as input file name" << endl;
}

int main (int argc, char *argv[]){

  // Include for ROOT
  gROOT->ProcessLine("#include <vector>");


  // Input information
  TString treeName=def_treeName;
  TString outputFile=def_outputFile;
  TString inputFiles="";
  TString triggerNames="";
  bool doElectron=def_doElectron;
  bool doMuon=def_doMuon;
  bool doPhoton=def_doPhoton;
  bool doJet=def_doJet;
  bool doMET=def_doMET;
  bool doTrack=def_doTrack;
  bool doMC=def_doMC;
  bool doMCSlim=def_doMCSlim;
  bool doBH=def_doBH;
  bool doMCSkim=def_doMCSkim;
  int maxEvents=-1;
  int skipEvents=0;

  // Argument check
  int c;
  while((c=getopt(argc,argv,"n:o:i:E:M:P:J:I:T:t:m:s:b:f:e:S:h"))!=-1){
    //cout << "option=" << (char)c << ", optarg=" << optarg << endl;
    switch(c){
      case 'n':
        treeName=optarg;
        break;
      case 'o':
        outputFile=optarg;
        break;
      case 'i':
        if (inputFiles!="")inputFiles+=TString(",")+optarg;
        else inputFiles=optarg;
        break;
      case 'E':
        if (atoi(optarg)==0)doElectron=false;
        else doElectron=true;
        break;
      case 'M':
        if (atoi(optarg)==0)doMuon=false;
        else doMuon=true;
        break;
      case 'P':
        if (atoi(optarg)==0)doPhoton=false;
        else doPhoton=true;
        break;
      case 'J':
        if (atoi(optarg)==0)doJet=false;
        else doJet=true;
        break;
      case 'I':
        if (atoi(optarg)==0)doMET=false;
        else doMET=true;
        break;
      case 'T':
        if (atoi(optarg)==0)doTrack=false;
        else doTrack=true;
        break;
      case 't':
        if (triggerNames!="")triggerNames+=TString(",")+optarg;
        else triggerNames=optarg;
        break;
      case 'm':
        if (atoi(optarg)==0)doMC=false;
        else doMC=true;
        break;
      case 's':
        if (atoi(optarg)==0)doMCSlim=false;
        else doMCSlim=true;
        break;
      case 'b':
        if (atoi(optarg)==0)doBH=false;
        else doBH=true;
        break;
      case 'f':
        if (atoi(optarg)==0)doMCSkim=false;
        else doMCSkim=true;
        break;
      case 'e':
        maxEvents = atoi(optarg);
        break;
      case 'S':
        skipEvents = atoi(optarg);
        break;
      default:
        cout << "Unknown option: " << (char)c << endl;
        help(argv[0]);
        exit(1);
    }
  }
  while(optind<argc){
    if (inputFiles!="")inputFiles+=TString(",")+argv[optind++];
    else inputFiles=argv[optind++];
  }
  if(inputFiles==""){
    help(argv[0]);
    exit(1);
  }

  char*str;
  std::vector<TString>inputs;
  str=strtok (const_cast<char*>(inputFiles.Data()),",");
  while (str!=NULL){
    inputs.push_back(str);
    str=strtok(NULL,",");
  }
  std::vector<TString>triggers;
  std::vector<TString>triggersInD3PD;
  str=strtok (const_cast<char*>(triggerNames.Data()),",");
  while (str!=NULL){
    triggers.push_back(str);
    str=strtok(NULL,",");
  }

  // Parameter dump
  cout << "ExoticsMultiJetSkim: treeName   = " << treeName << endl;
  cout << "ExoticsMultiJetSkim: outputFile = " << outputFile << endl;
  cout << "ExoticsMultiJetSkim: inputFiles =";
  for(unsigned int i=0;i<inputs.size();i++)cout << " " << inputs[i];
  cout << endl;
  cout << "ExoticsMultiJetSkim: triggers   =";
  for(unsigned int i=0;i<triggers.size();i++)cout << " " << triggers[i];
  cout << endl;
  cout << "ExoticsMultiJetSkim: doElectron = " << doElectron << endl;
  cout << "ExoticsMultiJetSkim: doMuon     = " << doMuon << endl;
  cout << "ExoticsMultiJetSkim: doPhoton   = " << doPhoton << endl;
  cout << "ExoticsMultiJetSkim: doJet      = " << doJet << endl;
  cout << "ExoticsMultiJetSkim: doMET      = " << doMET << endl;
  cout << "ExoticsMultiJetSkim: doMC       = " << doMC << endl;
  cout << "ExoticsMultiJetSkim: doMCSlim   = " << doMCSlim << endl;
  cout << "ExoticsMultiJetSkim: doBH       = " << doBH << endl;
  cout << "ExoticsMultiJetSkim: doMCSkim   = " << doMCSkim << endl;
  cout << "ExoticsMultiJetSkim: maxEvents  = " << maxEvents << endl;
  cout << "ExoticsMultiJetSkim: skipEvents = " << skipEvents << endl;

  // Get main tree
  TChain* ch= new TChain(treeName);
  for(unsigned int i=0;i<inputs.size();i++){
    ch->Add(inputs[i]);
  }

  // Exclude/include list of branches
  // All branches match to words include list are enabled
  // Exclude list are checked after include list
  vector<TRegexp> includes;
  vector<TRegexp> excludes;

  // Run info
  includes.push_back(TRegexp("RunNumber",kTRUE));
  includes.push_back(TRegexp("EventNumber",kTRUE));
  includes.push_back(TRegexp("lbn",kTRUE));
  includes.push_back(TRegexp("bcid",kTRUE));
  includes.push_back(TRegexp("actualIntPerXing",kTRUE));
  includes.push_back(TRegexp("averageIntPerXing",kTRUE));
  includes.push_back(TRegexp("pixelFlags",kTRUE));
  includes.push_back(TRegexp("sctFlags",kTRUE));
  includes.push_back(TRegexp("trtFlags",kTRUE));
  includes.push_back(TRegexp("larFlags",kTRUE));
  includes.push_back(TRegexp("tileFlags",kTRUE));
  includes.push_back(TRegexp("muonFlags",kTRUE));
  includes.push_back(TRegexp("fwdFlags",kTRUE));
  includes.push_back(TRegexp("coreFlags",kTRUE));
  includes.push_back(TRegexp("pixelError",kTRUE));
  includes.push_back(TRegexp("sctError",kTRUE));
  includes.push_back(TRegexp("trtError",kTRUE));
  includes.push_back(TRegexp("larError",kTRUE));
  includes.push_back(TRegexp("tileError",kTRUE));
  includes.push_back(TRegexp("muonError",kTRUE));
  includes.push_back(TRegexp("fwdError",kTRUE));
  includes.push_back(TRegexp("coreError",kTRUE));
  includes.push_back(TRegexp("bunch_configID",kTRUE));
  includes.push_back(TRegexp("detmask0",kTRUE));
  includes.push_back(TRegexp("detmask1",kTRUE));
  includes.push_back(TRegexp("isSimulation",kTRUE));
  includes.push_back(TRegexp("isCalibration",kTRUE));
  includes.push_back(TRegexp("isTestBeam",kTRUE));
  //includes.push_back(TRegexp("Eventshape_rhoKt4EM",kTRUE));
  includes.push_back(TRegexp("Eventshape_rhoKt4LC",kTRUE));

  // Vertex
  includes.push_back(TRegexp("vx_n",kTRUE));
  includes.push_back(TRegexp("vx_nTracks",kTRUE));
  includes.push_back(TRegexp("vx_sumPt",kTRUE));
  includes.push_back(TRegexp("vxp_n",kTRUE));
  includes.push_back(TRegexp("vxp_nTracks",kTRUE));
  includes.push_back(TRegexp("vxp_sumPt",kTRUE));

  // Electron
  if(doElectron){
    includes.push_back(TRegexp("el_n",kTRUE));
    includes.push_back(TRegexp("el_pt",kTRUE));
    includes.push_back(TRegexp("el_eta",kTRUE));
    includes.push_back(TRegexp("el_phi",kTRUE));
    includes.push_back(TRegexp("el_E",kTRUE));
    includes.push_back(TRegexp("el_m",kTRUE));
    includes.push_back(TRegexp("el_cl_pt",kTRUE));
    includes.push_back(TRegexp("el_cl_eta",kTRUE));
    includes.push_back(TRegexp("el_cl_phi",kTRUE));
    includes.push_back(TRegexp("el_cl_E",kTRUE));
    includes.push_back(TRegexp("el_etas2",kTRUE));
    includes.push_back(TRegexp("el_tracketa",kTRUE));
    includes.push_back(TRegexp("el_trackphi",kTRUE));
    includes.push_back(TRegexp("el_nPixHits",kTRUE));
    includes.push_back(TRegexp("el_nSCTHits",kTRUE));
    includes.push_back(TRegexp("el_author",kTRUE));
    includes.push_back(TRegexp("el_charge",kTRUE));
    includes.push_back(TRegexp("el_ptcone20",kTRUE));
    includes.push_back(TRegexp("el_ptcone30",kTRUE));
    includes.push_back(TRegexp("el_OQ",kTRUE));
    includes.push_back(TRegexp("el_goodOQ",kTRUE));
    includes.push_back(TRegexp("el_is*",kTRUE));
    includes.push_back(TRegexp("el_medium*",kTRUE));
    includes.push_back(TRegexp("el_loose*",kTRUE));
    includes.push_back(TRegexp("el_tight*",kTRUE));
    includes.push_back(TRegexp("el_trackIPEstimate_z0_unbiasedpvunbiased",kTRUE));
    includes.push_back(TRegexp("el_trackIPEstimate_d0_unbiasedpvunbiased",kTRUE));
    includes.push_back(TRegexp("el_trackIPEstimate_sigd0_unbiasedpvunbiased",kTRUE));
    includes.push_back(TRegexp("el_Etcone30",kTRUE));
    includes.push_back(TRegexp("el_topoEtcone30_corrected",kTRUE));

  }



  // Muon
  if(doMuon){
    includes.push_back(TRegexp("mu_staco_n",kTRUE));
    includes.push_back(TRegexp("mu_staco_pt",kTRUE));
    includes.push_back(TRegexp("mu_staco_eta",kTRUE));
    includes.push_back(TRegexp("mu_staco_phi",kTRUE));
    includes.push_back(TRegexp("mu_staco_E",kTRUE));
    includes.push_back(TRegexp("mu_staco_m",kTRUE));
    includes.push_back(TRegexp("mu_staco_ptcone20",kTRUE));
    includes.push_back(TRegexp("mu_staco_ptcone30",kTRUE));
    includes.push_back(TRegexp("mu_staco_isCombinedMuon",kTRUE));
    includes.push_back(TRegexp("mu_staco_isSegmentTaggedMuon",kTRUE));
    includes.push_back(TRegexp("mu_staco_isLowPtReconstructedMuon",kTRUE));
    includes.push_back(TRegexp("mu_staco_loose",kTRUE));
    includes.push_back(TRegexp("mu_staco_medium",kTRUE));
    includes.push_back(TRegexp("mu_staco_tight",kTRUE));
    includes.push_back(TRegexp("mu_staco_expectBLayerHit",kTRUE));
    includes.push_back(TRegexp("mu_staco_nBLHits",kTRUE));
    includes.push_back(TRegexp("mu_staco_nPixHits",kTRUE));
    includes.push_back(TRegexp("mu_staco_nPixelDeadSensors",kTRUE));
    includes.push_back(TRegexp("mu_staco_nPixHoles",kTRUE));
    includes.push_back(TRegexp("mu_staco_nSCTHits",kTRUE));
    includes.push_back(TRegexp("mu_staco_nSCTDeadSensors",kTRUE));
    includes.push_back(TRegexp("mu_staco_nSCTHoles",kTRUE));
    includes.push_back(TRegexp("mu_staco_nTRTHits",kTRUE));
    includes.push_back(TRegexp("mu_staco_nTRTOutliers",kTRUE));
    includes.push_back(TRegexp("mu_staco_charge",kTRUE));
    includes.push_back(TRegexp("mu_staco_me_qoverp_exPV",kTRUE));
    includes.push_back(TRegexp("mu_staco_id_qoverp_exPV",kTRUE));
    includes.push_back(TRegexp("mu_staco_me_theta_exPV",kTRUE));
    includes.push_back(TRegexp("mu_staco_id_theta_exPV",kTRUE));
    includes.push_back(TRegexp("mu_staco_id_theta",kTRUE));
    includes.push_back(TRegexp("mu_staco_z0_exPV",kTRUE));
    includes.push_back(TRegexp("mu_staco_d0_exPV",kTRUE));
    includes.push_back(TRegexp("mu_staco_me_cov_qoverp_exPV",kTRUE));
    includes.push_back(TRegexp("mu_staco_id_cov_qoverp_exPV",kTRUE));

    includes.push_back(TRegexp("mu_staco_nRPCLayer*PhiHits",kTRUE));
    includes.push_back(TRegexp("mu_staco_nTGCLayer*PhiHits",kTRUE));
    includes.push_back(TRegexp("mu_staco_nMDT*",kTRUE));
    includes.push_back(TRegexp("mu_staco_nCSCEtaHits",kTRUE));


    includes.push_back(TRegexp("mu_staco_ms_qoverp",kTRUE));
    includes.push_back(TRegexp("mu_staco_ms_theta",kTRUE));
    includes.push_back(TRegexp("mu_staco_ms_phi",kTRUE));
  }

  // Photon
  if(doPhoton){
    includes.push_back(TRegexp("ph_n",kTRUE));
    includes.push_back(TRegexp("ph_pt",kTRUE));
    includes.push_back(TRegexp("ph_eta",kTRUE));
    includes.push_back(TRegexp("ph_phi",kTRUE));
    includes.push_back(TRegexp("ph_E",kTRUE));
    includes.push_back(TRegexp("ph_m",kTRUE));
    includes.push_back(TRegexp("ph_cl_pt",kTRUE));
    includes.push_back(TRegexp("ph_cl_eta",kTRUE));
    includes.push_back(TRegexp("ph_cl_phi",kTRUE));
    includes.push_back(TRegexp("ph_cl_E",kTRUE));

    includes.push_back(TRegexp("ph_loose",kTRUE));
    includes.push_back(TRegexp("ph_tight",kTRUE));
    includes.push_back(TRegexp("ph_isEM",kTRUE));
    includes.push_back(TRegexp("ph_isConv",kTRUE));

    includes.push_back(TRegexp("ph_etas1",kTRUE));
    includes.push_back(TRegexp("ph_etas2",kTRUE));
    includes.push_back(TRegexp("ph_OQ",kTRUE));
    includes.push_back(TRegexp("ph_deltaEs",kTRUE));

    includes.push_back(TRegexp("ph_reta",kTRUE));
    includes.push_back(TRegexp("ph_rphi",kTRUE));
    includes.push_back(TRegexp("ph_Ethad1",kTRUE));
    includes.push_back(TRegexp("ph_Ethad",kTRUE));
    includes.push_back(TRegexp("ph_E277",kTRUE));
    includes.push_back(TRegexp("ph_E237",kTRUE));
    includes.push_back(TRegexp("ph_E233",kTRUE));
    includes.push_back(TRegexp("ph_weta2",kTRUE));
    includes.push_back(TRegexp("ph_f1",kTRUE));
    includes.push_back(TRegexp("ph_emaxs1",kTRUE));
    includes.push_back(TRegexp("ph_Emax2",kTRUE));
    includes.push_back(TRegexp("ph_Emins1",kTRUE));
    includes.push_back(TRegexp("ph_fside",kTRUE));
    includes.push_back(TRegexp("ph_wstot",kTRUE));
    includes.push_back(TRegexp("ph_ws3",kTRUE));
    includes.push_back(TRegexp("ph_convFlag",kTRUE));

    includes.push_back(TRegexp("ph_Etcone40",kTRUE));
    includes.push_back(TRegexp("ph_Etcone40_corrected",kTRUE));
    includes.push_back(TRegexp("ph_topoEtcone40_corrected",kTRUE));
    includes.push_back(TRegexp("ph_etap",kTRUE));
    includes.push_back(TRegexp("ph_ED_median",kTRUE));
  }

  // Jet
  vector<TString> jets;
  //jets.push_back("jet_AntiKt4TopoNewEM_");
  jets.push_back("jet_AntiKt4LCTopo_");
  jets.push_back("jet_AntiKt4Truth_");
  jets.push_back("jet_AntiKt4TruthJets_");
  jets.push_back("jet_AntiKt4TruthWithMuNoInt_");
  if(doJet){
    for(unsigned int i=0;i<jets.size();i++){
      includes.push_back(TRegexp(jets[i]+"n",kTRUE));
      includes.push_back(TRegexp(jets[i]+"E",kTRUE));
      includes.push_back(TRegexp(jets[i]+"pt",kTRUE));
      includes.push_back(TRegexp(jets[i]+"m",kTRUE));
      includes.push_back(TRegexp(jets[i]+"eta",kTRUE));
      includes.push_back(TRegexp(jets[i]+"phi",kTRUE));
      includes.push_back(TRegexp(jets[i]+"emscale*",kTRUE));
      includes.push_back(TRegexp(jets[i]+"constscale*",kTRUE));
      includes.push_back(TRegexp(jets[i]+"hecf",kTRUE));
      includes.push_back(TRegexp(jets[i]+"tgap3f",kTRUE));
      includes.push_back(TRegexp(jets[i]+"LArQuality",kTRUE));
      includes.push_back(TRegexp(jets[i]+"HECQuality",kTRUE));
      includes.push_back(TRegexp(jets[i]+"NegativeE",kTRUE));
      includes.push_back(TRegexp(jets[i]+"emfrac",kTRUE));
      includes.push_back(TRegexp(jets[i]+"Timing",kTRUE));
      includes.push_back(TRegexp(jets[i]+"sumPtTrk*",kTRUE));
      includes.push_back(TRegexp(jets[i]+"AverageLArQF",kTRUE));
      includes.push_back(TRegexp(jets[i]+"SamplingMax",kTRUE));
      includes.push_back(TRegexp(jets[i]+"fracSamplingMax",kTRUE));
      includes.push_back(TRegexp(jets[i]+"nTrk",kTRUE));
      includes.push_back(TRegexp(jets[i]+"WIDTH",kTRUE));
      includes.push_back(TRegexp(jets[i]+"BCH_CORR_JET*",kTRUE));
      includes.push_back(TRegexp(jets[i]+"BCH_CORR_CELL",kTRUE));
      includes.push_back(TRegexp(jets[i]+"BCH_CORR_DOTX",kTRUE));
      includes.push_back(TRegexp(jets[i]+"is*",kTRUE));
      includes.push_back(TRegexp(jets[i]+"Active*",kTRUE));
      includes.push_back(TRegexp(jets[i]+"OriginIndex",kTRUE));
      includes.push_back(TRegexp(jets[i]+"EtaOrigin",kTRUE));
      includes.push_back(TRegexp(jets[i]+"PhiOrigin",kTRUE));
      includes.push_back(TRegexp(jets[i]+"MOrigin",kTRUE));
      includes.push_back(TRegexp(jets[i]+"jvtxf",kTRUE));
      includes.push_back(TRegexp(jets[i]+"jvtx_x",kTRUE));
      includes.push_back(TRegexp(jets[i]+"jvtx_y",kTRUE));
      includes.push_back(TRegexp(jets[i]+"jvtx_z",kTRUE));
    }
  }

  // MET
  if(doMET){
    includes.push_back(TRegexp("MET_*",kTRUE));

    excludes.push_back(TRegexp("MET_Egamma10Loose*",kTRUE));
    excludes.push_back(TRegexp("MET_Egamma10NoTauLoose*",kTRUE));
    excludes.push_back(TRegexp("MET_Final*",kTRUE));
    excludes.push_back(TRegexp("MET_LooseEgamma*",kTRUE));
    excludes.push_back(TRegexp("*_CentralReg",kTRUE));
    excludes.push_back(TRegexp("*_EndcapRegion",kTRUE));
    excludes.push_back(TRegexp("*_ForwardReg",kTRUE));

    vector<TString> objForMet;
    objForMet.push_back("el_");
    objForMet.push_back("mu_staco_");
    objForMet.push_back("ph_");
    objForMet.push_back("jet_AntiKt4LCTopo_");
    objForMet.push_back("tau_");

    vector<TString> metTypeForObj;
    metTypeForObj.push_back("MET_");
    metTypeForObj.push_back("MET_Egamma10NoTau_");

    vector<TString> objMetBranch;
    objMetBranch.push_back("n");
    objMetBranch.push_back("statusWord");
    objMetBranch.push_back("wet");
    objMetBranch.push_back("wpx");
    objMetBranch.push_back("wpy");

    for (unsigned int i=0;i<objForMet.size();i++){
      for (unsigned int j=0;j<metTypeForObj.size();j++){
        for (unsigned int k=0;k<objMetBranch.size();k++){
          includes.push_back(TRegexp(objForMet[i]+metTypeForObj[j]+objMetBranch[k],kTRUE));
        }
      }
    }
  }

  // Track
  if(doTrack){
    includes.push_back(TRegexp("trk_eta",kTRUE));
  }

  // Trigger
  includes.push_back(TRegexp("trig_L1_TAV",kTRUE));
  includes.push_back(TRegexp("trig_L2_passedPhysics",kTRUE));
  includes.push_back(TRegexp("trig_EF_passedPhysics",kTRUE));
  includes.push_back(TRegexp("trig_L1_TBP",kTRUE));
  includes.push_back(TRegexp("trig_L1_TAP",kTRUE));
  includes.push_back(TRegexp("trig_L2_passedRaw",kTRUE));
  includes.push_back(TRegexp("trig_EF_passedRaw",kTRUE));
  includes.push_back(TRegexp("trig_DB_SMK",kTRUE));
  includes.push_back(TRegexp("trig_DB_L1PSK",kTRUE));
  includes.push_back(TRegexp("trig_DB_HLTPSK",kTRUE));

  if(doElectron){
    includes.push_back(TRegexp("trig_EF_el_n",kTRUE));
    includes.push_back(TRegexp("trig_EF_el_eta",kTRUE));
    includes.push_back(TRegexp("trig_EF_el_phi",kTRUE));
    includes.push_back(TRegexp("trig_EF_el_EF_e24vhi_medium1",kTRUE));
    includes.push_back(TRegexp("trig_EF_el_EF_e60_medium1",kTRUE));
  }

  if(doMuon){
    includes.push_back(TRegexp("mu_staco_EFCB_index",kTRUE));
    includes.push_back(TRegexp("mu_staco_EFMG_index",kTRUE));

    includes.push_back(TRegexp("trig_EF_trigmuonef_EF_mu24i_tight",kTRUE));
    includes.push_back(TRegexp("trig_EF_trigmuonef_EF_mu36_tight",kTRUE));
    includes.push_back(TRegexp("trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight",kTRUE));
  }

  includes.push_back(TRegexp("trig_bgCode",kTRUE));

  for(unsigned int i=0;i<triggers.size();i++){
    includes.push_back(TRegexp(triggers[i],kTRUE));
  }

  // MC
  includes.push_back(TRegexp("mc_channel_number",kTRUE));
  includes.push_back(TRegexp("mc_event_number",kTRUE));
  includes.push_back(TRegexp("mc_event_weight",kTRUE));
  includes.push_back(TRegexp("mcevt*",kTRUE));
  if(doMC){
    includes.push_back(TRegexp("mc_n",kTRUE));
    includes.push_back(TRegexp("mc_barcode",kTRUE));
    includes.push_back(TRegexp("mc_pt",kTRUE));
    includes.push_back(TRegexp("mc_eta",kTRUE));
    includes.push_back(TRegexp("mc_phi",kTRUE));
    includes.push_back(TRegexp("mc_m",kTRUE));
    includes.push_back(TRegexp("mc_pdgId",kTRUE));
    includes.push_back(TRegexp("mc_status",kTRUE));
    includes.push_back(TRegexp("mc_parents",kTRUE));
  }

  // Flag for truth check
  bool hasMC=false;

  // Set Branch Status to 0 first.
  // Then enable branches in include list
  // branches in exclude list will be excluded 
  // even it is in included list
  ch->SetBranchStatus("*",0);

  // Loop over all branches in tree
  for (int i = 0; i < ch->GetListOfBranches()->GetEntries(); i++) {
    TString branchName( ((TBranch*)ch->GetListOfBranches()->At(i))->GetName() );

    // Check include list
    for(unsigned int j=0;j<includes.size();j++){
      if(branchName.Index(includes[j])!=kNPOS){
        ch->SetBranchStatus(branchName,1);
        break;
      }
    }
    // Check exclude list
    for(unsigned int j=0;j<excludes.size();j++){
      if(branchName.Index(excludes[j])!=kNPOS){
        ch->SetBranchStatus(branchName,0);
        break;
      }
    }

    // Check triggers
    for(unsigned int j=0;j<triggers.size();j++){
      if(branchName==triggers[j]){
        triggersInD3PD.push_back(triggers[j]);
      }
    }

    // Check mc branches
    if ( branchName.BeginsWith( "mc_") ){
      hasMC=true;
    }

  }

  // Get trigger tree
  TChain* chTri = new TChain(treeName+"Meta/TrigConfTree");
  for(unsigned int i=0;i<inputs.size();i++){
    chTri->Add(inputs[i]);
  }
  if(chTri->LoadTree(0)<0){
    delete chTri;chTri=0;
  }
  if(chTri==0)cout << "ExoticsMultiJetSkim: trigger tree (" << treeName+"Meta/TrigConfTree" << " is not available!" << endl;

  // Get cut flow tree
  TChain* chCutFlow = new TChain(treeName+"Meta/CutFlowTree");
  for(unsigned int i=0;i<inputs.size();i++){
    chCutFlow->Add(inputs[i]);
  }
  if(chCutFlow->LoadTree(0)<0){
    delete chCutFlow;chCutFlow=0;
  }
  if(chCutFlow==0)cout << "ExoticsMultiJetSkim: cutflow tree (" << treeName+"Meta/CutFlowTree" << " is not available!" << endl;

  // Event counter
  vector<TString> cutName;
  cutName.push_back("preD3PD");
  cutName.push_back("postD3PD");
  cutName.push_back("postSkim");
  int evnum[]={0,0,0};
  double weight[]={0,0,0};

  // Branches for event count/event selection, etc...
  float mc_event_weight;
  if(hasMC)ch->SetBranchAddress("mc_event_weight", &mc_event_weight);

  Bool_t * triggerBranches = new Bool_t[triggersInD3PD.size()];
  for(unsigned int i=0;i<triggersInD3PD.size();i++){
    ch->SetBranchAddress(triggersInD3PD[i], &(triggerBranches[i]));
  }

  int mc_n;
  vector<int>* mc_barcode=0;
  vector<float>* mc_pt=0;
  vector<float>* mc_eta=0;
  vector<float>* mc_phi=0;
  vector<float>* mc_m=0;
  vector<int>* mc_pdgId=0;
  vector<int>* mc_status=0;
  vector<vector<int> > *mc_parents=0;

  Int_t           mcevt_n;
  vector<int>     *mcevt_signal_process_id=0;
  vector<int>     *mcevt_event_number=0;
  vector<double>  *mcevt_event_scale=0;
  vector<double>  *mcevt_alphaQCD=0;
  vector<double>  *mcevt_alphaQED=0;
  vector<int>     *mcevt_pdf_id1=0;
  vector<int>     *mcevt_pdf_id2=0;
  vector<double>  *mcevt_pdf_x1=0;
  vector<double>  *mcevt_pdf_x2=0;
  vector<double>  *mcevt_pdf_scale=0;
  vector<double>  *mcevt_pdf1=0;
  vector<double>  *mcevt_pdf2=0;
  vector<vector<double> > *mcevt_weight=0;
  vector<int>     *mcevt_nparticle=0;
  vector<short>   *mcevt_pileUpType=0;
 if(hasMC && doMC && (doMCSlim||doBH)){
    ch->SetBranchAddress("mc_n",&mc_n);
    ch->SetBranchAddress("mc_barcode",&mc_barcode);
    ch->SetBranchAddress("mc_pt",&mc_pt);
    ch->SetBranchAddress("mc_eta",&mc_eta);
    ch->SetBranchAddress("mc_phi",&mc_phi);
    ch->SetBranchAddress("mc_m",&mc_m);
    ch->SetBranchAddress("mc_pdgId",&mc_pdgId);
    ch->SetBranchAddress("mc_status",&mc_status);
    ch->SetBranchAddress("mc_parents",&mc_parents);

    ch->SetBranchAddress("mcevt_n",&mcevt_n);
    ch->SetBranchAddress("mcevt_signal_process_id",&mcevt_signal_process_id);
    ch->SetBranchAddress("mcevt_event_number",&mcevt_event_number);
    ch->SetBranchAddress("mcevt_event_scale",&mcevt_event_scale);
    ch->SetBranchAddress("mcevt_alphaQCD",&mcevt_alphaQCD);
    ch->SetBranchAddress("mcevt_alphaQED",&mcevt_alphaQED);
    ch->SetBranchAddress("mcevt_pdf_id1",&mcevt_pdf_id1);
    ch->SetBranchAddress("mcevt_pdf_id2",&mcevt_pdf_id2);
    ch->SetBranchAddress("mcevt_pdf_x1",&mcevt_pdf_x1);
    ch->SetBranchAddress("mcevt_pdf_x2",&mcevt_pdf_x2);
    ch->SetBranchAddress("mcevt_pdf_scale",&mcevt_pdf_scale);
    ch->SetBranchAddress("mcevt_pdf1",&mcevt_pdf1);
    ch->SetBranchAddress("mcevt_pdf2",&mcevt_pdf2);
    ch->SetBranchAddress("mcevt_weight",&mcevt_weight);
    ch->SetBranchAddress("mcevt_nparticle",&mcevt_nparticle);
    ch->SetBranchAddress("mcevt_pileUpType",&mcevt_pileUpType);
  }

  // Tmp branches for output
  int mc_n_new=0;
  vector<int>* mc_barcode_new=0;
  vector<float>* mc_pt_new=0;
  vector<float>* mc_eta_new=0;
  vector<float>* mc_phi_new=0;
  vector<float>* mc_m_new=0;
  vector<int>* mc_pdgId_new=0;
  vector<int>* mc_status_new=0;
  vector<vector<int> > *mc_parents_new=0;

  if(doMCSlim){
    mc_barcode_new=new vector<int>;
    mc_pt_new=new vector<float>;
    mc_eta_new=new vector<float>;
    mc_phi_new=new vector<float>;
    mc_m_new=new vector<float>;
    mc_pdgId_new=new vector<int>;
    mc_status_new=new vector<int>;
    mc_parents_new=new vector<vector<int> >;
  }

  // Write to new file
  TFile* outFile = new TFile(outputFile, "RECREATE");

  // New tree
  TTree* ch_new = ch->CloneTree(0);

  // New branches for output
  double bh_pt;
  double bh_eta;
  double bh_phi;
  double bh_m;
  if(hasMC&&doMC&&doBH){
    ch_new->Branch("bh_pt",&bh_pt,"bh_pt/D");
    ch_new->Branch("bh_eta",&bh_eta,"bh_eta/D");
    ch_new->Branch("bh_phi",&bh_phi,"bh_phi/D");
    ch_new->Branch("bh_m",&bh_m,"bh_m/D");
  }


  // Event selection
  int nEntries = ch->GetEntries();

  for( int i=0;i< nEntries;i++){
    // Skip events
    if(i<skipEvents)continue;

    // Get new entry
    ch->GetEntry(i);
    evnum[1]+=1;

    // Get weight
    double w;
    if(hasMC){
      w=mc_event_weight;
    }else{
      w=1;
    }
    weight[1]+=w;

    // Trigger skimming
    if((doMCSkim||(!hasMC)) && triggersInD3PD.size()>0){
      bool isPassed=false;
      for(unsigned int itrig=0;itrig<triggersInD3PD.size();itrig++){
        if(triggerBranches[itrig]){
          isPassed=true;
          break;
        }
      }
      if(!isPassed)continue;
    }

    // MC
    if(hasMC && doMC && (doMCSlim||doBH)){
      map<int,int> barcodeIndex;
      for (int j=0;j<mc_n;j++){
        barcodeIndex.insert(pair<int, int>((*mc_barcode)[i],i));
      }
      if(doBH){
        bh_pt=0;
        bh_eta=0;
        bh_phi=0;
        bh_m=0;
      }
      if(doMCSlim){
        mc_n_new=0;
        mc_barcode_new->clear();
        mc_pt_new->clear();
        mc_eta_new->clear();
        mc_phi_new->clear();
        mc_m_new->clear();
        mc_pdgId_new->clear();
        mc_status_new->clear();
        mc_parents_new->clear();
      }
      TLorentzVector p(0,0,0,0);
      TLorentzVector pBH(0,0,0,0);
      for (int imc=0;imc<mc_n;imc++){
        if(
           // choose stable particle
           ((*mc_status)[imc]==1&&(*mc_barcode)[imc]<200000)
           ||
           // choose hard process decay, only valid for pythia8
           ((*mc_parents)[imc].size()==2 && 
           (((*mc_parents)[imc][0]==3&&(*mc_parents)[imc][1]==4)||
            ((*mc_parents)[imc][1]==3&&(*mc_parents)[imc][0]==4)))
        ){
          p.SetPtEtaPhiM((*mc_pt)[imc],(*mc_eta)[imc],(*mc_phi)[imc],(*mc_m)[imc]);
          if(doBH&&(*mc_status)[imc]!=1){
            pBH+=p;
          }
          if(doMCSlim){
            mc_n_new++;
            mc_barcode_new->push_back((*mc_barcode)[imc]);
            mc_pt_new->push_back((*mc_pt)[imc]);
            mc_eta_new->push_back((*mc_eta)[imc]);
            mc_phi_new->push_back((*mc_phi)[imc]);
            mc_m_new->push_back((*mc_m)[imc]);
            mc_pdgId_new->push_back((*mc_pdgId)[imc]);
            mc_status_new->push_back((*mc_status)[imc]);
            mc_parents_new->push_back((*mc_parents)[imc]);
          }
        }
      }
      if(doBH){
        bh_pt=pBH.Pt();
        bh_m=pBH.M();
        if(pBH.Pt()!=0){
          bh_eta=pBH.Eta();
          bh_phi=pBH.Phi();
        }else{
          bh_eta=0.;
          bh_phi=0.;
        }
      }
      if(doMCSlim){
        mc_n=mc_n_new;

        mc_barcode->clear();
        mc_pt->clear();
        mc_eta->clear();
        mc_phi->clear();
        mc_m->clear();
        mc_pdgId->clear();
        mc_status->clear();
        mc_parents->clear();

        for(unsigned int imcnew=0;imcnew<mc_barcode_new->size();imcnew++)mc_barcode->push_back((*mc_barcode_new)[imcnew]);
        for(unsigned int imcnew=0;imcnew<mc_pt_new->size();imcnew++)mc_pt->push_back((*mc_pt_new)[imcnew]);
        for(unsigned int imcnew=0;imcnew<mc_eta_new->size();imcnew++)mc_eta->push_back((*mc_eta_new)[imcnew]);
        for(unsigned int imcnew=0;imcnew<mc_phi_new->size();imcnew++)mc_phi->push_back((*mc_phi_new)[imcnew]);
        for(unsigned int imcnew=0;imcnew<mc_m_new->size();imcnew++)mc_m->push_back((*mc_m_new)[imcnew]);
        for(unsigned int imcnew=0;imcnew<mc_pdgId_new->size();imcnew++)mc_pdgId->push_back((*mc_pdgId_new)[imcnew]);
        for(unsigned int imcnew=0;imcnew<mc_status_new->size();imcnew++)mc_status->push_back((*mc_status_new)[imcnew]);
        for(unsigned int imcnew=0;imcnew<mc_parents_new->size();imcnew++)mc_parents->push_back((*mc_parents_new)[imcnew]);

        if(mcevt_n>0){
          mcevt_n=1;
          vector<int>::iterator it_mcevt_signal_process_id = mcevt_signal_process_id->begin();it_mcevt_signal_process_id++;mcevt_signal_process_id->erase(it_mcevt_signal_process_id,mcevt_signal_process_id->end());
          vector<int>::iterator it_mcevt_event_number=mcevt_event_number->begin();it_mcevt_event_number++;mcevt_event_number->erase(it_mcevt_event_number,mcevt_event_number->end());
          vector<double>::iterator it_mcevt_event_scale=mcevt_event_scale->begin();it_mcevt_event_scale++;mcevt_event_scale->erase(it_mcevt_event_scale,mcevt_event_scale->end());
          vector<double>::iterator it_mcevt_alphaQCD=mcevt_alphaQCD->begin();it_mcevt_alphaQCD++;mcevt_alphaQCD->erase(it_mcevt_alphaQCD,mcevt_alphaQCD->end());
          vector<double>::iterator it_mcevt_alphaQED=mcevt_alphaQED->begin();it_mcevt_alphaQED++;mcevt_alphaQED->erase(it_mcevt_alphaQED,mcevt_alphaQED->end());
          vector<int>::iterator it_mcevt_pdf_id1=mcevt_pdf_id1->begin();it_mcevt_pdf_id1++;mcevt_pdf_id1->erase(it_mcevt_pdf_id1,mcevt_pdf_id1->end());
          vector<int>::iterator it_mcevt_pdf_id2=mcevt_pdf_id2->begin();it_mcevt_pdf_id2++;mcevt_pdf_id2->erase(it_mcevt_pdf_id2,mcevt_pdf_id2->end());
          vector<double>::iterator it_mcevt_pdf_x1=mcevt_pdf_x1->begin();it_mcevt_pdf_x1++;mcevt_pdf_x1->erase(it_mcevt_pdf_x1,mcevt_pdf_x1->end());
          vector<double>::iterator it_mcevt_pdf_x2=mcevt_pdf_x2->begin();it_mcevt_pdf_x2++;mcevt_pdf_x2->erase(it_mcevt_pdf_x2,mcevt_pdf_x2->end());
          vector<double>::iterator it_mcevt_pdf_scale=mcevt_pdf_scale->begin();it_mcevt_pdf_scale++;mcevt_pdf_scale->erase(it_mcevt_pdf_scale,mcevt_pdf_scale->end());
          vector<double>::iterator it_mcevt_pdf1=mcevt_pdf1->begin();it_mcevt_pdf1++;mcevt_pdf1->erase(it_mcevt_pdf1,mcevt_pdf1->end());
          vector<double>::iterator it_mcevt_pdf2=mcevt_pdf2->begin();it_mcevt_pdf2++;mcevt_pdf2->erase(it_mcevt_pdf2,mcevt_pdf2->end());
          vector<vector<double> >::iterator it_mcevt_weight=mcevt_weight->begin();it_mcevt_weight++;mcevt_weight->erase(it_mcevt_weight,mcevt_weight->end());
          vector<int>::iterator it_mcevt_nparticle=mcevt_nparticle->begin();it_mcevt_nparticle++;mcevt_nparticle->erase(it_mcevt_nparticle,mcevt_nparticle->end());
          vector<short>::iterator it_mcevt_pileUpType=mcevt_pileUpType->begin();it_mcevt_pileUpType++;mcevt_pileUpType->erase(it_mcevt_pileUpType,mcevt_pileUpType->end());
        }
      }
    }


    // Finalize
    evnum[2]+=1;
    weight[2]+=w;
    ch_new->Fill();

    // Max event check
    if(evnum[2]==maxEvents)break;

  }

  outFile->cd();
  ch_new->Write();
  //cout << "nEntriesForNewFile = "<< nEntriesNew << endl;

  // Check cut flow at D3PD level
  if(chCutFlow!=0 && chCutFlow->GetEntries() != 0){
    std::vector<unsigned long> *nAcceptedEvents = new std::vector<unsigned long>;
    std::vector<double> *nWeightedAcceptedEvents = new std::vector<double>;
    std::vector<std::string> *name = new std::vector<std::string>;
    chCutFlow->SetBranchAddress("nAcceptedEvents", &nAcceptedEvents);
    chCutFlow->SetBranchAddress("nWeightedAcceptedEvents", &nWeightedAcceptedEvents);
    chCutFlow->SetBranchAddress("name", &name);
    for(int i=0;i<chCutFlow->GetEntries();i++){
      chCutFlow->GetEntry(i);
      for(unsigned int j=0;j<name->size();j++){
        if((*name)[j] == "AllExecutedEvents"){
          evnum[0]+=(*nAcceptedEvents)[j];
          weight[0]+=(*nWeightedAcceptedEvents)[j];
          break;
        }
      }
    }

  }else{
    evnum[0]=evnum[1];
    weight[0]=weight[1];
  }

  // Copy meta data
  outFile->cd();
  TDirectory* newdir = outFile->mkdir( treeName+"Meta", treeName+"Meta" );
  newdir->cd();
  TChain* chTri_new=0;
  if(chTri != 0){
    chTri_new = (TChain*)chTri->CloneTree();
    chTri_new->Write();
  }
  TChain* chCutFlow_new=0;
  if(chCutFlow != 0){
    chCutFlow_new = (TChain*)chCutFlow->CloneTree();
    chCutFlow_new->Write();
  }

  // Cut flow histgrams
  outFile->cd();
  TH1F* h_evnum = new TH1F("evnum","evnum",cutName.size(),0,cutName.size());
  TH1F* h_weight = new TH1F("weight","weight",cutName.size(),0,cutName.size());
  cout << "" << endl;
  cout << "******Cut Flow for " << outputFile << "******" << endl;
  cout << setw(10) << "CutName" << setw(10) << "Events" << setw(12) << "Weights" << endl;
  for(unsigned int i=0;i<cutName.size();i++){
    cout << setw(10) << cutName[i] << setw(10) << evnum[i] << setw(12) << weight[i] << endl;
    h_evnum->GetXaxis()->SetBinLabel(i+1,cutName[i]);
    h_evnum->SetBinContent(i+1,evnum[i]);
    h_weight->GetXaxis()->SetBinLabel(i+1,cutName[i]);
    h_weight->SetBinContent(i+1,weight[i]);
    cout << "*************************" << endl;
    cout << "" << endl;
  }
  h_evnum->Write();
  h_weight->Write();


  // Close file
  delete outFile;

  // Delete original chains
  if(ch!=0)delete ch;
  if(chTri!=0)delete chTri;
  if(chCutFlow!=0)delete chCutFlow;

  return 0;
}
