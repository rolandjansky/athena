/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * file: CommonLikelihood.cxx
 *
 * Author: Martin Flechl (mflechl@cern.ch)
 *         Marcin Wolter (marcin.wolter@cern.ch)
 *             updated 16 May 2011
 */

#include "TauDiscriminant/CommonLikelihood.h"
#include <cmath>


TString m="";
const double SMALL = 1.e-6;

bool CommonLikelihood::build(const string& filename) {

  if (filename != ""){ //otherwise try defaults from constructor
    vector<string> substrings;
    this->splitString(filename,substrings,",");
    if (substrings.size()==3){
      m_tauFilename=substrings[0];
      m_jetFilename=substrings[1];
      m_cutFilename=substrings[2];
    } else{
      //to do: otherwise, try defaults.....
      print("Exactly 3 files required, e.g. \"pdf_taus.root,pdf_jet.root,LMTCutsLLH.root\".",0);
    }
    m="PDF files: tau="+m_tauFilename+", jet="+m_jetFilename+", cuts="+m_cutFilename;
    print(m,0);
  }

  return (this->readPDFHistograms() && this->readLMTCuts() && this->smoothPDFHistograms());
}

int CommonLikelihood::response(int ilmt, int /*option*/) const {
  if(ilmt < 0 || ilmt > 2)
    return 0;
  std::vector<TGraph*> cuts;
  int prongcat = m_prongindex;
  if(prongcat==0)
    cuts=m_vLMTCuts1P;
  else if(prongcat==1)
    cuts=m_vLMTCuts3P;
  else
    return 0;
  if(cuts.size()!=3)
    return 0;
  double cut = cuts.at(ilmt)->Eval(m_et/1000.);
  if (  m_llh>cut ) return 1;
  else return 0;
}

bool CommonLikelihood::calcLLHValue(const map<string,const float*>* floatVariables,
				    const map<string,const int*>* intVariables, int option=0){
  //option 0: safe llh; 1: default llh; 2: use whatever vars given in maps above

  m="In calcLLHValue, option="; m+=option;
  print(m,2);

  m_llh=-999;

  map<string,const float*>::const_iterator it_floats;
  map<string,const int*>::const_iterator it_ints;

  map<string,float> allVariables;
  map<string,float>::const_iterator it_all;

  int author;
  it_ints = intVariables->find("AUTHOR");
  if (it_ints == intVariables->end()) {
    it_floats = floatVariables->find("AUTHOR");
    if (it_floats == floatVariables->end()) {
      print("Cannot find AUTHOR in booked float or int variables!",0);
      return false;
    } else{
      author=(int)*(it_floats->second);
    }
  } else{
    author=*(it_ints->second);
  }
  if ( author==2 ){
    print("Author==2, assigning llh value -1111!",1);
    m_llh=-1111; return true;
  } //as tauRec does

  it_floats = floatVariables->find("PT");
  if (it_floats == floatVariables->end()) {
    print("Cannot find PT in booked float variables!",0);
    return false;
  }
  float et=*(it_floats->second);


  it_floats = floatVariables->find("ETA");
  if (it_floats == floatVariables->end()) {
    print("Cannot find ETA in booked float variables!",0);
    return false;
  }
  float eta=*(it_floats->second);
  if ( fabs(eta)>2.5 ){ print("Not within eta range",2); m_llh=-96; return true;} //as tauRec does

  it_ints = intVariables->find("NUMTRACK");
  if (it_ints == intVariables->end()) {
    print("Cannot find NUMTRACK in booked int variables!",0);
    return false;
  }
  int numtrack=*(it_ints->second);
  if ( numtrack<1 ) { print("0 tracks",2); m_llh=-97; return true;} //as tauRec does
  m_prongindex=0; //1pr
  if (numtrack>=2) m_prongindex=1; //3pr
  m_ntau[m_prongindex]++;
/*
  it_ints = intVariables->find("NUMPI0");
  if (it_ints == intVariables->end()) {
    it_ints = intVariables->find("NPI0");
    if (it_ints == intVariables->end()) {
      print("Cannot find NUMPI0 in booked int variables!",0);
      return false;
    }
  }
  int numpi0=*(it_ints->second);
*/
  int nvtx=1;
  it_ints = intVariables->find("NUM_PILEUP_AND_PRIMARY_VERTICES");
  if (it_ints == intVariables->end()) {
    print("Cannot find NUM_PILEUP_AND_PRIMARY_VERTICES in booked int variables!",0);
    print("Setting value to 1 and will try running, but the output will not be valid.",0);
  } else nvtx=*(it_ints->second);
  m_nvtx=nvtx;

  m="author: "; m+=author; m+=", et:"; m+=et; m+=", eta:"; m+=eta;
  m+=", numtrack:"; m+=numtrack; //m+=", numpi0:"; m+=numpi0;
  print(m,1);


  allVariables = getVariables(floatVariables, intVariables, numtrack, author, option);



  m="nInts: "; m+=intVariables->size(); m+=", nFloats: "; m+=floatVariables->size();
  m+=", nAll: "; m+=allVariables.size();
  m+=", numtrack="; m+=numtrack;
  print(m,1);
  //print(m,0);

  //check if all vars are there
  if (option==0){ //safeLLH
    if (    ( numtrack==1 && (allVariables.size()!=NVAR_SAFE_1P) ) || ( numtrack>=2 && (allVariables.size()!=NVAR_SAFE_3P) )     ){
      print("Not all safe variables found... giving up! Use the llhall option to run over a specific set of variables.",0);
      return false;
    }
  }


  double calcLH=0;

  print("Start filling vars",2);

  for(it_all=allVariables.begin(); it_all!=allVariables.end(); ++it_all){
    m="Var: "+it_all->first;
    int ivar=this->varNameToNumber(it_all->first);
    if (ivar<0){ print("ERROR: Unknown Variable "+it_all->first,0+(m_ntau[m_prongindex]>1)); continue;  }
    m+=", var #"; m+=ivar; m+=", value="; m+=it_all->second;
    print(m,1);

    calcLH+=getSingleLLHRatio(ivar, numtrack, author, et, nvtx, (float) it_all->second);
  }

  m_et = et;
  m_llh=calcLH;
  return true;
}


map<string,float>  CommonLikelihood::getVariables(const map<string,const float*>* floatVariables, const map<string,const int*>* intVariables, const int numtrack,const int author, const int option) const{



  const string use_safe[2][NVAR_SAFE_3P]={
    {"CORRCENTFRAC","CORRFTRK","TRKAVGDIST","NUMWIDETRACK","IPSIGLEADTRK",""},  //1PR
    {"CORRCENTFRAC","CORRFTRK","TRKAVGDIST","DRMAX","MASSTRKSYS","TRFLIGHTPATHSIG"}}; //3PR

  const string use_def[2][NVAR_DEF]={
    {"TRKAVGDIST","CORRFTRK","IPSIGLEADTRK","DRMAX","MASSTRKSYS","TRFLIGHTPATHSIG","NUMWIDETRACK","CORRCENTFRAC"},
    {"TRKAVGDIST","CORRFTRK","IPSIGLEADTRK","DRMAX","MASSTRKSYS","TRFLIGHTPATHSIG","NUMWIDETRACK","CORRCENTFRAC"}
  };
/*    {"EMRADIUS","ISOLFRAC","STRIPWIDTH2","NSTRIP","ETHAD2ETTRACKS","ETEM2ETTRACKS","ETTRACKS2ET","",     "",     "",         "",
     "NISOLTRK","MVISEFLOW","IPZ0SINTHETASIGLEADTRK","IPSIGLEADLOOSETRK",""},
    {"EMRADIUS","ISOLFRAC","STRIPWIDTH2","NSTRIP","ETHAD2ETTRACKS","ETEM2ETTRACKS","ETTRACKS2ET","DRMIN","DRMAX","TRKWIDTH2","MASSTRKSYS",
     "NISOLTRK","MVISEFLOW","",                      "IPSIGLEADLOOSETRK","TRFLIGHTPATHSIG"}};
*/
  const string use_author3[NVAR_DEF] = {"","","","","","","","","","TRKWIDTH2","MASSTRKSYS","NISOLTRK","MVISEFLOW","IPZ0SINTHETASIGLEADTRK","","TRFLIGHTPATHSIG"};
  const string use_3trk[NVAR_DEF] = {"","","","","","","","","","TRKWIDTH2","MASSTRKSYS","","","","",""};


  map<string,const float*>::const_iterator it_floats;
  map<string,const int*>::const_iterator it_ints;

  int prong=m_prongindex; //1pr

  map<string, float> allVariables;


  float et = 1.;
  it_floats = floatVariables->find("ET");
  if (it_floats != floatVariables->end()) {
    et=*(it_floats->second);
  }
  else et=99.;


  switch (option){
  case 0: //safe
    for(unsigned int i=0; i<NVAR_SAFE_3P; i++) {
      if (use_safe[prong][i] != "") {
	it_floats = floatVariables->find(use_safe[prong][i]);
	if (it_floats != floatVariables->end()) {
	  if ( it_floats->first=="ETTRACKS2ET" ){
	    print("Modified etTracks2Et",2);
	    if (fabs(et)<SMALL)	allVariables[it_floats->first]= *(it_floats->second)/et;
	    else  allVariables[it_floats->first] = 999.;
	  }
	  else allVariables[it_floats->first]= *(it_floats->second);
	}

	it_ints = intVariables->find(use_safe[prong][i]);
	if (it_ints != intVariables->end()) {
	  allVariables[it_ints->first]= *(it_ints->second);
	}

      }
    }
    break;
  case 1: //def
    if ( author==1 ){ //no track-seed
      for(unsigned int i=0; i<NVAR_DEF; i++) {
	if (use_author3[i] != "") {
	  it_floats = floatVariables->find(use_author3[i]);
	  if (it_floats != floatVariables->end()) {
	    allVariables[it_floats->first]= *(it_floats->second);
	  }

	  it_ints = intVariables->find(use_author3[i]);
	  if (it_ints != intVariables->end()) {
	    allVariables[it_ints->first]= *(it_ints->second);
	  }
	}
      }
    }
    else if ( numtrack!=3 ){ //not exactly 3 tracks
      for(unsigned int i=0; i<NVAR_DEF; i++) {
	if (use_3trk[i] != "") {
	  it_floats = floatVariables->find(use_3trk[i]);
	  if (it_floats != floatVariables->end()) {
	    allVariables[it_floats->first]= *(it_floats->second);
	  }

	  it_ints = intVariables->find(use_3trk[i]);
	  if (it_ints != intVariables->end()) {
	    allVariables[it_ints->first]= *(it_ints->second);
	  }
	}
      }
    }
    else {
      for(unsigned int i=0; i<NVAR_DEF; i++) {
	if (use_def[prong][i] != "") {
	  it_floats = floatVariables->find(use_def[prong][i]);
	  if (it_floats != floatVariables->end()) {
	    allVariables[it_floats->first]= *(it_floats->second);
	  }

	  it_ints = intVariables->find(use_def[prong][i]);
	  if (it_ints != intVariables->end()) {
	    allVariables[it_ints->first]= *(it_ints->second);
	  }
	}
      }
    }
    break;
  default:
    print("ERROR: getVariables, option > 1",0);
    break;;//take any var
  }

  return allVariables;

}





float CommonLikelihood::getSingleLLHRatio(const int ivar, const int numtrack, const int author, const float et, const int nvtx, const float value){

  float LLHR = this->getSimpleSingleLLHRatio( ivar, numtrack, author, et, nvtx, value );

  //for now hard-coding the pt-bin boundaries (45,100), should be automatized...
  float border[]={45.,100.};
  for(int b=0;b<2;b++){
    float closetoborder=et/1000.-(border[b]);
	
	double uppBorder = 10.0;
	double lowBorder = 10.0;

	if( border[b] >= 100. && numtrack == 1 && !(m_doTrigger) ) {
		uppBorder = 60.0;
		lowBorder = 30.0;
	}
	
	if( closetoborder >= 0 && fabs(closetoborder) < uppBorder ) {
		float nextbin = (border[b]-lowBorder/2.0)*1000.0;
		float LLHRnext = this->getSimpleSingleLLHRatio( ivar, numtrack, author, nextbin, nvtx, value );
		LLHR = LLHR*((uppBorder+fabs(closetoborder))/(uppBorder*2.0)) + LLHRnext*((uppBorder-fabs(closetoborder))/(uppBorder*2.0));
	}


	if( closetoborder < 0 && fabs(closetoborder) < lowBorder ) {
		float nextbin = (border[b]+uppBorder/2.0)*1000.0;
		float LLHRnext = this->getSimpleSingleLLHRatio( ivar, numtrack, author, nextbin, nvtx, value );
		LLHR = LLHR*((lowBorder+fabs(closetoborder))/(lowBorder*2.0)) + LLHRnext*((lowBorder-fabs(closetoborder))/(lowBorder*2.0));
	}
  }
  return LLHR;
}



float CommonLikelihood::getSimpleSingleLLHRatio(const int ivar, const int numtrack, const int author, const float et, const int nvtx, const float value){

  const std::string prongs[]={"","1prong","","3prong"};
  const std::string classes[]={"both","calo"};

  int prongcat=1; int authorcat=0;
  if(numtrack >= 2){ prongcat=3; authorcat=0; }
  if(author==2) return -1111;
  std::string prongType=prongs[prongcat];
  std::string authorType=classes[authorcat];

  char text[100];
  sprintf(text,"_v%d",ivar);
  std::string hNameLong = "hpdf";
  hNameLong+=+"_"+prongType;
  hNameLong+=+"_"+authorType;
  hNameLong.append(text);
  TH3F *hratio = m_pdfHistogramsRatio[hNameLong];

  m="tau nentries: "; m+=hratio->GetEntries(); m+=", nbins: "; m+=hratio->GetNbinsX();
  print(m,2);
  TAxis *xaxis = hratio->GetXaxis();
  TAxis *yaxis = hratio->GetYaxis();
  TAxis *zaxis = hratio->GetZaxis();

  float xMin = xaxis->GetXmin();
  float xMax = xaxis->GetXmax(); //

  m=""; m+=xMin; m+=" -  "; m+=xMax;
  print(m,2);
  float etMax = yaxis->GetXmax();
  float vtxMax = zaxis->GetXmax();

  int p_nvtx=nvtx;
  float temp_et=et;
  if ( nvtx>vtxMax ) p_nvtx=(int) vtxMax;
  if ( temp_et/1000.0>etMax ) temp_et=(etMax*1000)-1;
  if (value<xMin) return 0;

  float varLL = this->Interpolate(hratio,value,temp_et/1000.0,p_nvtx);

  m="llhratio value of this var: "; m+=varLL;
  print(m,1);

  return varLL;
}



bool CommonLikelihood::readPDFHistograms() {
  print("in readPDFHistograms()",2);

  m_tauFile = new TFile(m_tauFilename.c_str());
  m_jetFile = new TFile(m_jetFilename.c_str());

  const int nProngdirs=2;
  const int nClassdirs=1;
  const std::string prongs[nProngdirs]={"1prong","3prong"};
  const std::string classes[nClassdirs]={"both"/*,"calo"*/};

  for (int iProng=0; iProng<nProngdirs; iProng++) {
    for (int iClass=0; iClass<nClassdirs; iClass++) {
      std::string prongDir = prongs[iProng];
      std::string classDir = classes[iClass];
      for (int ivar=0; ivar<=NVAR; ivar++) {

	char text[100];
	sprintf(text,"_v%d",ivar);
	std::string hNameLong = "hpdf";
	hNameLong+=+"_"+prongDir;
	hNameLong+=+"_"+classDir;
	hNameLong.append(text);
	std::string hname = prongDir+"/"+classDir+"/"+hNameLong;

	TH3F* hTmp_tau = (TH3F*)m_tauFile->Get(hname.c_str());
	TH3F* hTmp_jet = (TH3F*)m_jetFile->Get(hname.c_str());

	m_pdfHistogramsTau[hNameLong] = (TH3F*)hTmp_tau;
	m_pdfHistogramsJet[hNameLong] = (TH3F*)hTmp_jet;

      }
    }
  }

  return true;
}

bool CommonLikelihood::readLMTCuts() {
  print("in readLMTCuts()",2);

  m_cutFile = new TFile(m_cutFilename.c_str());
  if(m_cutFile==0) return false; //assert(m_cutFile!=0);
  tg_loose_cuts_1P = (TGraph*) m_cutFile->Get("1prong/loose");
  tg_medium_cuts_1P = (TGraph*) m_cutFile->Get("1prong/medium");
  tg_tight_cuts_1P = (TGraph*) m_cutFile->Get("1prong/tight");

  if(tg_loose_cuts_1P==0 || tg_medium_cuts_1P==0 || tg_tight_cuts_1P==0)
    return false;
  m_vLMTCuts1P.push_back(tg_loose_cuts_1P);
  m_vLMTCuts1P.push_back(tg_medium_cuts_1P);
  m_vLMTCuts1P.push_back(tg_tight_cuts_1P);
  tg_loose_cuts_3P = (TGraph*) m_cutFile->Get("3prong/loose");
  tg_medium_cuts_3P = (TGraph*) m_cutFile->Get("3prong/medium");
  tg_tight_cuts_3P = (TGraph*) m_cutFile->Get("3prong/tight");

  if(tg_loose_cuts_3P==0 || tg_medium_cuts_3P==0 || tg_tight_cuts_3P==0)
    return false;
  m_vLMTCuts3P.push_back(tg_loose_cuts_3P);
  m_vLMTCuts3P.push_back(tg_medium_cuts_3P);
  m_vLMTCuts3P.push_back(tg_tight_cuts_3P);

  return true;
}


bool CommonLikelihood::smoothPDFHistograms() {
  print("in smoothPDFHistograms()",2);

  const int nProngdirs=2;
  const int nClassdirs=1;
  const std::string prongs[nProngdirs]={"1prong","3prong"};
  const std::string classes[nClassdirs]={"both"/*,"calo"*/};

  for (int iProng=0; iProng<nProngdirs; iProng++) {
    for (int iClass=0; iClass<nClassdirs; iClass++) {
      std::string prongDir = prongs[iProng];
      std::string classDir = classes[iClass];
      for (int ivar=0; ivar<=NVAR; ivar++) {
	char text[100];
	sprintf(text,"_v%d",ivar);
	std::string hNameLong = "hpdf";
	hNameLong+=+"_"+prongDir;
	hNameLong+=+"_"+classDir;
	hNameLong.append(text);
	if (m_pdfHistogramsTau[hNameLong] && m_pdfHistogramsJet[hNameLong]){
	  if (m_smooth) {
	    this->smooth3D(m_pdfHistogramsTau[hNameLong]);
	    this->smooth3D(m_pdfHistogramsJet[hNameLong]);
	  }
	  m_pdfHistogramsRatio[hNameLong] = this->divideLog(m_pdfHistogramsTau[hNameLong],m_pdfHistogramsJet[hNameLong]);
	}
      }
    }
  }
  return true;
}

void CommonLikelihood::smooth3D(TH3F* hTmp){

  for (int i=1;i<=hTmp->GetNbinsY();i++){
    for (int j=1;j<=hTmp->GetNbinsZ();j++){
      TH1D* tmp = hTmp->ProjectionX("projX",i,i,j,j,"");
      tmp->Smooth(5);
      for (int k=1;k<=tmp->GetNbinsX();k++)
	hTmp->SetBinContent(k,i,j,tmp->GetBinContent(k));
      delete tmp; tmp=0;
    }
  }
}

TH3F* CommonLikelihood::divideLog(TH3F* hTmpTau, TH3F* hTmpJet){
  if (
      hTmpTau->GetNbinsX() != hTmpJet->GetNbinsX() ||
      hTmpTau->GetNbinsY() != hTmpJet->GetNbinsY() ||
      hTmpTau->GetNbinsZ() != hTmpJet->GetNbinsZ()
      ) return 0;
  TH3F* hTmpRatio = new TH3F(*hTmpTau);
  hTmpRatio->Reset();
  for (int i=0;i<=hTmpTau->GetNbinsY()+1;i++){
    for (int j=0;j<=hTmpTau->GetNbinsZ()+1;j++){
      for (int k=0;k<=hTmpTau->GetNbinsX()+1;k++) {
	float tauLL = hTmpTau->GetBinContent(k,i,j);
	float jetLL = hTmpJet->GetBinContent(k,i,j);
	if(fabs(jetLL)<SMALL) jetLL = SMALL;
	if(!tauLL) tauLL = SMALL;
	float varLL=TMath::Log(tauLL/jetLL);
	hTmpRatio->SetBinContent(k,i,j,varLL);
      }
    }
  }
  return hTmpRatio;
}

Double_t CommonLikelihood::Interpolate(TH3F* hist, Double_t x, Double_t y, Double_t z)
{
  // Given a point x, approximates the value via linear interpolation
  // based on the two nearest bin centers


  Int_t ibin = hist->FindBin(x,y,z);
  Int_t xbin, ybin, zbin;
  hist->GetBinXYZ(ibin, xbin, ybin, zbin);
  if (!m_smooth) return hist->GetBinContent(ibin);

  Double_t x0,x1,y0,y1;

  if(xbin<=1 || xbin>=hist->GetNbinsX()) {
    return hist->GetBinContent(ibin);
  } else {
    if(x<=hist->GetXaxis()->GetBinCenter(xbin)) {
      y0 = hist->GetBinContent(xbin-1, ybin, zbin);
      x0 = hist->GetXaxis()->GetBinCenter(xbin-1);
      y1 = hist->GetBinContent(xbin, ybin, zbin);
      x1 = hist->GetXaxis()->GetBinCenter(xbin);
    } else {
      y0 = hist->GetBinContent(xbin, ybin, zbin);
      x0 = hist->GetXaxis()->GetBinCenter(xbin);
      y1 = hist->GetBinContent(xbin+1, ybin, zbin);
      x1 = hist->GetXaxis()->GetBinCenter(xbin+1);
    }
    if (fabs(x1-x0)<SMALL && fabs(x1-x0)!=0.) return y0 + (x-x0)*((y1-y0)/(x1-x0));
    else return 999.;
  }
}

int CommonLikelihood::varNameToNumber(std::string varname) const {
  if(varname=="emRadius" || varname=="EMRADIUS" ) return 0;
  if(varname=="isolFrac" || varname=="ISOLFRAC" ) return 1;
  if(varname=="stripWidth2" || varname=="STRIPWIDTH2" ) return 2;
  if(varname=="nStrip" || varname=="NSTRIP" ) return 3;
  if(varname=="etHad2etTracks" || varname=="ETHAD2ETTRACKS" ) return 4;
  if(varname=="etEM2etTracks" || varname=="ETEM2ETTRACKS" ) return 5;
  if(varname=="etTracks2et" || varname=="ETTRACKS2ET" ) return 6;
  if(varname=="etEM2Et" || varname=="ETEM2ET" || varname=="EMFRACTIONCALIB" || varname=="EMFRACTIONATEMSCALE" ) return 7;
  if(varname=="etOverPtLeadTrk" || varname=="ETOVERPTLEADTRK" ) return 8;
  if(varname=="dRmin" || varname=="DRMIN" ) return 9;
  if(varname=="dRmax" || varname=="DRMAX" ) return 10;
  if(varname=="trkWidth2" || varname=="TRKWIDTH2" ) return 11;
  if(varname=="massTrkSys" || varname=="MASSTRKSYS" ) return 12;
  if(varname=="nIsolTrk" || varname=="NISOLTRK" ) return 13;
  if(varname=="MVisEflow" || varname=="MVISEFLOW" ) return 14;
  if(varname=="ipZ0SinThetaSigLeadTrk" || varname=="IPZ0SINTHETASIGLEADTRK" ) return 15;
  if(varname=="ipSigLeadLooseTrk" || varname=="IPSIGLEADLOOSETRK" ) return 16;
  if(varname=="trFlightPathSig" || varname=="TRFLIGHTPATHSIG" ) return 17;
  if(varname=="centFrac" || varname=="CENTFRAC" ) return 18;
  if(varname=="numEffClus" || varname=="NUMEFFCLUS" ) return 19;
  if(varname=="trkAvgDist" || varname=="TRKAVGDIST" ) return 20;
  if(varname=="topoInvMass" || varname=="TOPOINVMASS" ) return 21;
  if(varname=="calRadius" || varname=="CALRADIUS" ) return 22;

  if(varname=="ipSigLeadTrk" || varname=="IPSIGLEADTRK" ) return 23;
  if(varname=="benchdRLeadTrkLeadCluster" || varname=="BENCHDRLEADTRKLEADCLUSTER" ) return 24;
  if(varname=="benchdRLeadTrk2ndLeadCluster" || varname=="BENCHDRLEADTRK2NDLEADCLUSTER" ) return 25;
  if(varname=="benchdRLeadCluster2ndLeadCluster" || varname=="BENCHDRLEADCLUSTER2NDLEADCLUSTER" ) return 26;
  if(varname=="lead2ClusterEOverAllCusterE" || varname=="LEAD2CLUSTEREOVERALLCLUSTERE" ) return 27;
  if(varname=="geoTrackRadius" || varname=="GEOTRACKRADIUS" ) return 28;
  if(varname=="geoTrackMass" || varname=="GEOTRACKMASS" ) return 29;
  if(varname=="geoCentFrac" || varname=="GEOCENTFRAC" ) return 30;
  if(varname=="geoTopoInvMass" || varname=="GEOTOPOINVMASS" ) return 31;
  if(varname=="geoCoreEnergyFraction" || varname=="GEOCOREENERGYFRACTION" ) return 32;
  if(varname=="geoCoreClusterEMRadius" || varname=="GEOCORECLUSTEREMRADIUS" ) return 33;
  if(varname=="geodRLeadTrkLeadCluster" || varname=="GEODRLEADTRKLEADCLUSTER" ) return 34;
  if(varname=="geodRLeadTrk2ndLeadCluster" || varname=="GEODRLEADTRK2NDLEADCLUSTER" ) return 35;
  if(varname=="geodRLeadCluster2ndLeadCluster" || varname=="GEODRLEADCLUSTER2NDLEADCLUSTER" ) return 36;
  if(varname=="geoEClusterOverETau" || varname=="GEOECLUSTEROVERETAU" ) return 37;
  if(varname=="NClusters" || varname=="NCLUSTERS" ) return 38;
  if(varname=="NeffClusters" || varname=="NEFFCLUSTERS" ) return 39;
  if(varname=="effTopoInvMass" || varname=="EFFTOPOINVMASS" ) return 40;
  if(varname=="effdRLeadTrkLeadCluster" || varname=="EFFDRLEADTRKLEADCLUSTER" ) return 41;
  if(varname=="effdRLeadTrk2ndLeadCluster" || varname=="EFFDRLEADTRK2NDLEADCLUSTER" ) return 42;
  if(varname=="effdRLeadCluster2ndLeadCluster" || varname=="EFFDRLEADCLUSTER2NDLEADCLUSTER" ) return 43;
  if(varname=="effEClusterOverETau" || varname=="EFFECLUSTEROVERETAU" ) return 44;
  if(varname=="Eiso" || varname=="EISO" ) return 45;
  if(varname=="ClusterEMRadius" || varname=="CLUSTEREMRADIUS" ) return 46;
  if(varname=="ClusterCentFrac" || varname=="CLUSTERCENTFRAC" ) return 47;
  if(varname=="effEMRadius" || varname=="EFFEMRADIUS" ) return 48;
  if(varname=="effCentFrac" || varname=="EFFCENTFRAC" ) return 49;
  if(varname=="numWideTrack" || varname=="NUMWIDETRACK" ) return 50;
  if(varname=="charge" || varname=="CHARGE" ) return 51;
  if(varname=="nPi0" || varname=="NPI0" ) return 52;
  if(varname=="corrcentFrac" || varname=="CORRCENTFRAC" ) return 53;
  if(varname=="corrfTrk" || varname=="CORRFTRK" ) return 56;


  print("ERROR: Variable name not corresponding to a PDF variable: "+varname,2);
  return -1;
}

void CommonLikelihood::splitString(const string& str, vector<string>& substrings, const string& delimiters) const{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos){
    // Found a token, add it to the vector.
    substrings.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}
