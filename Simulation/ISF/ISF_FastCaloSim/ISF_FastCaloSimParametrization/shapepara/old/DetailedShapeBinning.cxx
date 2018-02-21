/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

using namespace std;

#include "TMatrixD.h"
#include "TVectorD.h"
#include "TH1.h"
#include "TFile.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TApplication.h"
#include "TTree.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TH2D.h"
#include "TLine.h"
#include "TPrincipal.h"
#include "TBranch.h"
#include "TChain.h"
#include "TMath.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TBrowser.h"
#include "ISF_FastCaloSimParametrization/DetailedShapeBinning.h"
#include "tools/CaloGeometryFromFile.cxx"
#include "tools/FCS_Cell.h"
#include "TLorentzVector.h"
#include <sstream>

#include <iostream>
#include <fstream>

DetailedShapeBinning::DetailedShapeBinning()
{
}

bool DetailedShapeBinning::run(bool doNNHit, bool ismatched, TChain* T, int nRBinsMAX, float layer, string particle, int PCAbin, string /*labeltitle*/, string labeloutput, float& /*mincalosize*/, float& cell_deta, float& cell_dphi, float& cellr, float& cellz, float& celleta, vector<float> etaslice)
{

  // input file 
  //cout<<"--- Open input file "<<inputfile<<endl;
  //TFile* file = TFile::Open(inputfile.c_str());
  //if(!file) cout<<"ERROR: Inputfile could not be opened"<<endl;
  //string mytrees="";
  //if(ismatched && !doNNHit)  mytrees = "FCS_ParametrizationInput";
  //if(!ismatched && !doNNHit) mytrees = "ISF_HitAnalysis/CaloHitAna";
  //if(doNNHit) mytrees = "NNSimHit";
  //TTree* T = (TTree*)file->Get(mytrees.c_str());

  cout << "Number of events : " << T->GetEntries() << std::endl;

  // output file 
  string outFile = "output/DetailedShape_"+labeloutput+".root";
  if(doNNHit) outFile = "output/DetailedShape_"+labeloutput+"_NNHits.root";

  // Pi/8 rotation to contain the calo crack within a bin
  float AlphaMin = TMath::Pi()/8.;
  float AlphaMax = 2*TMath::Pi()+AlphaMin;
  float Rmax = 10000 ; //random value 

  TH1F* halpha = new TH1F("halpha","halpha",630000,AlphaMin,AlphaMax) ;
  TH1F* hr = new TH1F("hr","hr",75000000,0,Rmax);
  //Float_t xdummy[3];
  //xdummy[0]=0;
  //xdummy[1]=1;
  //xdummy[2]=2;

  float tolerance = 0.00000001; // for empty bin check
  std::cout << "---Tolerance : "<< tolerance << std::endl;

  // float cellr=0;
  // float cellz=0;
  // float celleta=0;
  // float cell_deta=0;
  // float cell_dphi=0;

  // PCA file 
  string myPCAfile = "output/firstPCA_w0_160406_5mmMergebins1st5_bins2nd1.root";
  if(particle=="pi")
    myPCAfile = "output/firstPCA_pions.root";

  if(particle=="e")
    //myPCAfile = "output/firstPCA_electrons.root";
    myPCAfile = "/afs/cern.ch/user/s/schaarsc/public/fastcalo/pca/tools/output/s2864/firstPCA.root";

  if(particle=="gamma")
    myPCAfile = "output/firstPCA_photons.root";

  std::cout << "--- Open PCA file : " << myPCAfile << std::endl;
  TFile* FPCA           = TFile::Open(myPCAfile.c_str());

  TTree* TPCA; 
  if(particle=="e")   TPCA           = (TTree*)FPCA->Get("tree_1stPCA");
  if(particle=="pi")  TPCA           = (TTree*)FPCA->Get("firstPCA");

  vector<int> vbinPCA;

  if(particle=="e"){
    Int_t firstPCAbin = 1 ;
    TPCA->SetBranchAddress("firstPCAbin",&firstPCAbin);
    for(Int_t i=0;i<TPCA->GetEntries();i++){
      TPCA->GetEntry(i);
      vbinPCA.push_back(firstPCAbin);
    }
  }

  if(particle=="pi"){
    Int_t bin = 0 ; 
    TPCA->SetBranchAddress("bin",&bin);
    for(Int_t i=0;i<TPCA->GetEntries();i++){
      TPCA->GetEntry(i);
      vbinPCA.push_back(bin);
    }
  }

  //Fill initial histos for binning: halpha, hr
  //bool temp = fill(doNNHit,T,3,xdummy,3,xdummy,layer,particle,PCAbin,halpha,hr,tolerance,false,outFile,cellr,cellz,celleta,cell_deta,cell_dphi,vbinPCA,ismatched,labeloutput,etaslice);

  // Alpha binning
  cout << "--- Alpha calculation" << std::endl;
  int nAlphaBins           = 8 ; 
  int nAlphaboundaries     = nAlphaBins + 1 ;
  vector<float> vAlphaBins = fVecBin(halpha,nAlphaBins,false);

  Float_t AlphaBins[nAlphaboundaries];
  for(Int_t i=0;i<nAlphaboundaries;i++){
    AlphaBins[i] = vAlphaBins.at(i) ;
    std::cout << "BIN ALPHA : " << i << "  " << AlphaBins[i] << std::endl;
  }

  // R binning - iterative
  cout << "--- R calculation" << std::endl;
  float nRBins    = 0 ;
  int incr        = 0 ; 
  bool isEmptyBin = true ; 
  //float rgran     = 0 ; 

  while(isEmptyBin)
  {
    nRBins               = nRBinsMAX-incr ;
    int nRboundaries     = nRBins+1;
    cout << "Test " << nRBins << " r bins" << std::endl;

    vector<float> vRBins = fVecBin(hr,nRBins,true) ;

    // try to fusion up to the 5 last bins
    for(Int_t fusionindex=1;fusionindex<5;fusionindex++){

      if(fusionindex>1) nRboundaries = nRboundaries-1;
      Float_t RBins[nRboundaries];
      for(Int_t i=0;i<nRboundaries;i++){
        RBins[i] = vRBins.at(i) ;
        if(fusionindex>1){// start fusioning bins
          RBins[nRboundaries-1] = vRBins.at(vRBins.size()-1);
        }
      }

      isEmptyBin = fill(doNNHit,T,nAlphaboundaries,AlphaBins,nRboundaries,RBins,layer,particle,PCAbin,*halpha,*hr,tolerance,true,outFile,cellr,cellz,celleta,cell_deta,cell_dphi,vbinPCA,ismatched,labeloutput,etaslice);
      //rgran = (RBins[3]-RBins[0]) ;
      TH1F hGradient(("hGradient_"+labeloutput).c_str(),"hGradient",nRBins,RBins) ;

      if(!isEmptyBin){
        // final R bins
        if(fusionindex>1) cout << "Fusion of last bins : " << fusionindex << ", total R bins : " << nRboundaries-1 <<  std::endl;
        for(Int_t i=0;i<nRboundaries;i++){
          std::cout << "BIN R : " << i << "  " << RBins[i] << std::endl;
        }

        // check gradient
        std::cout << "--- CHECKING GRADIENT" << std::endl;
        //bool gradientOK = checkgradient(outFile,hGradient,labeloutput,labeltitle,doNNHit);

        // check granularity
        std::cout << "--- CHECKING GRANULARITY" << std::endl;
        //bool granularityOK = checkgranularity(cell_deta,cell_dphi,cellr,cellz,celleta,rgran,AlphaBins,nAlphaBins,mincalosize);

        // validation plots 
        std::cout << "--- Validation plots" << std::endl;
        //fillValidationPlot(layer,PCAbin,gradientOK,granularityOK,particle,labeloutput);

        break;
      }//checks and validation plots
    }//fusion index
    incr++;
  }//while empty bin

  return 0;
}

bool DetailedShapeBinning::fill(bool doNNHit,TTree* T, int nxbinsAlpha, Float_t *xbinsAlpha, int nybinsR, Float_t *ybinsR, float layer, string /*particle*/, float PCAbin, TH1F& halpha, TH1F& hr, float tolerance, bool testBinning, string outFile, float& cellr, float& cellz, float& celleta, float& cell_deta, float& cell_dphi, vector<int> vbinPCA, bool ismatched,string labeloutput, vector<float> etaslice)
{

  TH1::SetDefaultSumw2(kTRUE);

  bool isEmptyBin = false;

  std::ostringstream os;
  os<<layer;
  TString layerS = "Sampling_"+os.str();

  // for matched ntuples 
  FCS_matchedcellvector *vec=0;
  std::vector<FCS_truth> *truth=0;

  std::vector<float>* TruthPx = 0;
  std::vector<float>* TruthPy = 0;
  std::vector<float>* TruthPz = 0;
  std::vector<float>* TruthE  = 0;

  if(ismatched && !doNNHit)
  {
    T->SetBranchAddress(layerS,&vec);
    T->SetBranchAddress("TruthCollection",&truth);
    T->SetBranchAddress("TruthPx",&TruthPx);
    T->SetBranchAddress("TruthPy",&TruthPy);
    T->SetBranchAddress("TruthPz",&TruthPz);
    T->SetBranchAddress("TruthE" ,&TruthE);
  }

  // for unmatched ntuples 
  std::vector<float>* HitE = 0;
  std::vector<float>* HitX=0;
  std::vector<float>* HitY=0;
  std::vector<float>* HitZ = 0;
  vector< vector<double> >* TTC_entrance_eta = 0 ;
  vector< vector<double> >* TTC_entrance_phi = 0 ;
  vector< vector<double> >* TTC_entrance_r = 0 ;
  std::vector<int>* HitSampling =0;
  TBranch* bHitE = 0 ;
  TBranch* bHitX = 0 ;
  TBranch* bHitY = 0 ;
  TBranch* bHitZ = 0 ;
  TBranch* bTTC_entrance_eta = 0 ;
  TBranch* bTTC_entrance_phi = 0 ;
  TBranch* bTTC_entrance_r = 0 ;
  TBranch* bHitSampling = 0 ;

  if(!ismatched  && !doNNHit)
  {
    std::cout << "BEFORE BRANCH" << std::endl;
    bHitE = T->GetBranch("HitE");
    bHitE->SetAddress(&HitE);
    bHitX = T->GetBranch("HitX");
    bHitX->SetAddress(&HitX);
    bHitY = T->GetBranch("HitY");
    bHitY->SetAddress(&HitY);
    bHitZ = T->GetBranch("HitZ");
    bHitZ->SetAddress(&HitZ);
    bTTC_entrance_eta = T->GetBranch("TTC_entrance_eta");
    bTTC_entrance_eta->SetAddress(&TTC_entrance_eta);
    bTTC_entrance_phi = T->GetBranch("TTC_entrance_phi");
    bTTC_entrance_phi->SetAddress(&TTC_entrance_phi);
    bTTC_entrance_r = T->GetBranch("TTC_entrance_r");
    bTTC_entrance_r->SetAddress(&TTC_entrance_r);
    bHitSampling = T->GetBranch("HitSampling");
    bHitSampling->SetAddress(&HitSampling);
    std::cout <<"AFTER BRANCH" << std::endl;
  }

  TBranch* bNNHitR = 0 ;
  TBranch* bNNHitAlpha = 0 ;
  TBranch* bNNHitLNEfrac = 0 ;
  Float_t NNHitR = 0 ; 
  Float_t NNHitAlpha = 0 ; 
  Float_t NNHitLNEfrac = 0 ; 

  if(doNNHit){
    bNNHitR = T->GetBranch("NNHitR");
    bNNHitR->SetAddress(&NNHitR);
    bNNHitAlpha = T->GetBranch("NNHitAlpha");
    bNNHitAlpha->SetAddress(&NNHitAlpha);
    bNNHitLNEfrac = T->GetBranch("NNHitLNEfrac");
    bNNHitLNEfrac->SetAddress(&NNHitLNEfrac);
  }

  // histograms ##############

  vector<TH2F*> halphadrEvec;
  // needed for the empty bin check
  TH2F* hEnergyDensity = new TH2F("hEnergyDensity","hEnergyDensity",nxbinsAlpha-1,xbinsAlpha,nybinsR-1,ybinsR) ;
  TH2F* hEnergy = new TH2F("hEnergy","hEnergy",nxbinsAlpha-1,xbinsAlpha,nybinsR-1,ybinsR) ;
  TH2F* hNNHit  = new TH2F("hNNHit","hNNHit",nxbinsAlpha-1,xbinsAlpha,nybinsR-1,ybinsR) ;

  // needed to compare with the NN fit output
  TH2F* hLNEnergy = new TH2F("hLNEnergy","hLNEnergy",nxbinsAlpha-1,xbinsAlpha,nybinsR-1,ybinsR) ;
  TH2F* hLNEnergyDensity = new TH2F("hLNEnergyDensity","hLNEnergyDensity",nxbinsAlpha-1,xbinsAlpha,nybinsR-1,ybinsR) ;

  // cell position ###########

  float etacentre=0;
  float phicentre=0;
  int nhits=0;
  float eta = 0 ; 
  float phi = 0 ; 

  ofstream ofML;
  ofML.open(("output/MLHits_"+labeloutput+".txt").c_str());

  int i_pcafile=-1;
  if(!testBinning && !doNNHit)
  {
    int nsel=0;
    //for(int i=0;i<T->GetEntries();i++)
    for(int i=0;i<2000;i++)
    {
      if(i%200==0) std::cout << "Event: " << i << std::endl;
      T->GetEntry(i);
      if(ismatched)
      {
        TLorentzVector TruthTLV;
        TruthTLV.SetPxPyPzE(TruthPx->at(0),TruthPy->at(0),TruthPz->at(0),TruthE->at(0));
        float TruthEta=TruthTLV.Eta();
        if(fabs(TruthEta)>etaslice.at(1) || fabs(TruthEta)<etaslice.at(0)) continue;
        i_pcafile++;
        if(vbinPCA.at(i_pcafile)!=PCAbin) continue;
        for (UInt_t j=0; j<(*vec).size(); j++)
        {
          for (unsigned int ihit=0; ihit<((FCS_matchedcell)(*vec)[j]).hit.size(); ihit++)
          {
            float posx   = ((FCS_matchedcell)(*vec)[j]).hit[ihit].hit_x;
            float posy   = ((FCS_matchedcell)(*vec)[j]).hit[ihit].hit_y;
            float posz   = ((FCS_matchedcell)(*vec)[j]).hit[ihit].hit_z;
            TVector3 * pos         = new TVector3(posx,posy,posz);	  
            eta              = pos->PseudoRapidity() ;
            phi              = pos->Phi() ;
            etacentre+=eta;
            phicentre+=phi;
            nhits++;
          }
        }
      }
      if(!ismatched)
      {
        bHitX->GetEntry(i);
        bHitY->GetEntry(i);
        bHitZ->GetEntry(i);
        bHitSampling->GetEntry(i);
        for(unsigned int j=0;j<HitX->size();j++)
        {
          float layer_id = HitSampling->at(j) ;
          if(layer_id != layer) continue ;
          TVector3 * pos         = new TVector3(HitX->at(j), HitY->at(j), HitZ->at(j)) ;
          eta              = pos->PseudoRapidity() ;
          phi              = pos->Phi() ;
          etacentre+=eta;
          phicentre+=phi;
          nhits++;
        }
      }//end unmatched ntuple
      nsel++;
    }

    std::cout << "Number of selected events : " << nsel << std::endl;

    if (nhits > 0) {
      etacentre=etacentre/nhits;
      phicentre=phicentre/nhits;
    } else {
      std::cout << "DetailedShapeBinning() nhits==0; unable to divide etacentre,phicentre!";
    }
    //std::cout << "ETA,PHI CENTRES : " << etacentre << " " << phicentre << "  " << nhits << std::endl;

    getcellgeom(layer,etacentre,phicentre,cellr,cellz,celleta,cell_deta,cell_dphi);
    std::cout << "--- CELL POSITION (r,z,eta,deta,dphi) : " << cellr << "  " << cellz << "  " << celleta << " " <<  cell_deta << " " << cell_dphi << std::endl;
  }

  // loop on events ##########

  i_pcafile=-1;
  //for(int i=0;i<T->GetEntries();i++)  
  for(int i=0;i<2000;i++)  
  {
    std::vector<float> vMLalpha;
    std::vector<float> vMLr;
    std::vector<float> vMLE;

    if(i%200==0) std::cout << "Entry 2nd loop : " << i << std::endl;

    if(ismatched || doNNHit)
    {
      T->GetEntry(i);
    }

    if(ismatched && !doNNHit)
    {    
      // Cut on eta of particle 
      TLorentzVector TruthTLV;
      TruthTLV.SetPxPyPzE(TruthPx->at(0),TruthPy->at(0),TruthPz->at(0),TruthE->at(0));
      float TruthEta=TruthTLV.Eta();
      if(fabs(TruthEta)>etaslice.at(1) || fabs(TruthEta)<etaslice.at(0)) continue;
      i_pcafile++;
    }


    // veto on PCA bin 
    if(!doNNHit && (i_pcafile >=0))
      if(vbinPCA.at(i_pcafile)!=PCAbin) continue; 

    if(!ismatched && !doNNHit){
      bHitX->GetEntry(i);
      bHitY->GetEntry(i);
      bHitZ->GetEntry(i);
      bHitE->GetEntry(i);
      bHitSampling->GetEntry(i);
      bTTC_entrance_r->GetEntry(i);
      bTTC_entrance_eta->GetEntry(i);
      bTTC_entrance_phi->GetEntry(i);
    }

    // histos to cumulate energy
    if(testBinning && !doNNHit)
    {
      cout<<"TESTBIN 1"<<endl;
      std::ostringstream histname;
      histname << "halphadrEvec_";
      histname << i;
      TH2F* hnew = new TH2F(histname.str().c_str(),histname.str().c_str(),nxbinsAlpha-1,xbinsAlpha,nybinsR-1,ybinsR);
      halphadrEvec.push_back(hnew);
    }


    if(doNNHit){
      float alpha = NNHitAlpha;
      float d_r   = NNHitR;
      float LNenergy= NNHitLNEfrac;
      float energy = TMath::Exp(LNenergy); // GERALDINE - IS THAT OK ? 
      if(testBinning){
        hLNEnergy->Fill(alpha,d_r,LNenergy);
        hEnergy->Fill(alpha,d_r,energy);
        hNNHit->Fill(alpha,d_r,1);
      }
      halpha.Fill(alpha);
      float myweight;
      if(energy>1)
        myweight = energy;
      else
        myweight = 1 ;
      hr.Fill(d_r,myweight);

    }

    if(!ismatched && !doNNHit)
    {
      //loop on hits
      for(unsigned int j=0;j<HitX->size();j++){ 

        // veto on layer
        float layer_id = HitSampling->at(j) ;
        if(layer_id != layer)
          continue ;

        float energy           = HitE->at(j); 
        TVector3 * pos         = new TVector3(HitX->at(j), HitY->at(j), HitZ->at(j)) ;  
        float eta              = pos->PseudoRapidity() ;
        //float phi              = pos->Phi() ;
        float eta_correction   = (TTC_entrance_eta->at(0)).at(layer_id) ;
        float d_eta            = eta - eta_correction ;

        float myy = (TTC_entrance_r->at(0)).at(layer_id) * TMath::Sin((TTC_entrance_phi->at(0)).at(layer_id)); 
        float myx = (TTC_entrance_r->at(0)).at(layer_id) * TMath::Cos((TTC_entrance_phi->at(0)).at(layer_id)); 

        TVector2 * myv2= new TVector2(myx,myy);
        TVector2 * myv3= new TVector2(pos->X(),pos->Y());
        float d_phi = myv3->DeltaPhi(*myv2) ;

        // changes in mm 
        unitsmm(eta,d_eta,d_phi,cellr,cellz);

        float d_r   = TMath::Sqrt(d_phi*d_phi + d_eta*d_eta);

        float alpha = TMath::ATan2(d_phi,d_eta);
        if(alpha<(TMath::Pi()/8.))
          alpha = 2*TMath::Pi()+alpha ;

        if(testBinning)
        {
          cout<<"TESTBIN 2"<<endl;
          halphadrEvec.at(i)->Fill(alpha,d_r,energy);
        }
        halpha.Fill(alpha);
        float myweight;
        if(energy>1)
          myweight = energy;
        else
          myweight = 1 ;
        hr.Fill(d_r,myweight);

        vMLr.push_back(d_r);
        vMLalpha.push_back(alpha);
        vMLE.push_back(energy);
      }
    }// end unmatched 
    if(ismatched && !doNNHit)
    {

      // loop on cells 
      for (UInt_t j=0; j<(*vec).size(); j++){
        // loop on hits
        for (unsigned int ihit=0; ihit<((FCS_matchedcell)(*vec)[j]).hit.size(); ihit++){

          // r and alpha calculation
          float energy = ((FCS_matchedcell)(*vec)[j]).hit[ihit].hit_energy;
          float posx   = ((FCS_matchedcell)(*vec)[j]).hit[ihit].hit_x;
          float posy   = ((FCS_matchedcell)(*vec)[j]).hit[ihit].hit_y;
          float posz   = ((FCS_matchedcell)(*vec)[j]).hit[ihit].hit_z;
          TVector3 * pos         = new TVector3(posx,posy,posz) ;  

          float eta              = pos->PseudoRapidity() ;
          //float phi              = pos->Phi() ;
          float eta_correction   = (*truth)[0].TTC_entrance_eta[0];
          float d_eta            = eta - eta_correction ;

          float myy = ((*truth)[0].TTC_entrance_r[0]) * TMath::Sin((*truth)[0].TTC_entrance_phi[0]);
          float myx = ((*truth)[0].TTC_entrance_r[0]) * TMath::Cos((*truth)[0].TTC_entrance_phi[0]);

          TVector2 * myv2= new TVector2(myx,myy);
          TVector2 * myv3= new TVector2(pos->X(),pos->Y());
          float d_phi = myv3->DeltaPhi(*myv2) ;

          // change in mm 
          unitsmm(eta,d_eta,d_phi,cellr,cellz);

          float d_r   = TMath::Sqrt(d_phi*d_phi + d_eta*d_eta);

          float alpha = TMath::ATan2(d_phi,d_eta);
          if(alpha<(TMath::Pi()/8.)) 
            alpha = 2*TMath::Pi()+alpha ;

          if(testBinning)
          {
            cout<<"TESTBIN 3"<<endl;
            halphadrEvec.at(i)->Fill(alpha,d_r,energy);
            cout<<"TESTBIN 3 done"<<endl;
          }
          halpha.Fill(alpha);
          float myweight;
          if(energy>1)
            myweight = energy;
          else
            myweight = 1 ;
          hr.Fill(d_r,myweight);

          if(fabs(eta)>=etaslice.at(0) && fabs(eta)<=etaslice.at(1)){
            vMLr.push_back(d_r);
            vMLalpha.push_back(alpha);
            vMLE.push_back(energy);
          }
          //std::cout << d_r << " " << alpha << " " << energy << std::endl; // GILLES 
        }// loop on cells
      }// loop on hits
    }// end matched ntuple

    if(testBinning && !doNNHit)
    {
      cout<<"TESTBIN 4"<<endl;
      //float MLint = halphadrEvec.at(i)->Integral();
      if(halphadrEvec.at(i)->Integral()>0){
        halphadrEvec.at(i)->Scale(1./halphadrEvec.at(i)->Integral());
      }
      hEnergy->Add(halphadrEvec.at(i));

      for(unsigned int w=0;w<vMLr.size();w++){
        ofML << vMLr.at(w) << " " << vMLalpha.at(w) << " " << vMLE.at(w) << std::endl;
      }
    }
  }// loop on events

  cout<<"loop done"<<endl;

  ofML.close();

  if(doNNHit)
  {
    hLNEnergy->Divide(hNNHit);
    hEnergy->Divide(hNNHit);
  }

  if(testBinning)
  {
    TH2F* hAllnorm = (TH2F*) hEnergy->Clone("hAllnorm");
    hAllnorm->Scale(1./hAllnorm->Integral());

    TFile* f2 = new TFile(outFile.c_str(),"recreate");

    // output variables for NN fit of the shower shape 
    TTree* T2 = new TTree("NNfit","NNfit");
    //float   fAlphaBinCenter=0;
    //float   fRBinCenter=0;
    //float   fenergyLN=0;
    //float   fenergydensityLN=0;
    //TBranch *b0 ;
    //b0 =T2->Branch("alpha", &fAlphaBinCenter);
    //TBranch *b1;
    //b1 =T2->Branch("r", &fRBinCenter);
    //TBranch *b2;
    //b2 =T2->Branch("energydensityLN", &fenergydensityLN);
    //TBranch *b22;
    //b22 =T2->Branch("energyLN", &fenergyLN);


    // output variables for NN fit of E(alpha,r) generation
    TTree* T3 = new TTree("NNinv","NNinv");
    //float   fWeight=0;
    //float   fbina=0;
    //float   fbinrLN=0;
    //float   fprobr=0;
    //float   fprobalpha=0;

    //TBranch *b3;
    //b3=T3->Branch("probr", &fprobr);
    //TBranch *b4;
    //b4=T3->Branch("probalpha", &fprobalpha);
    //TBranch *b5;
    //b5=T3->Branch("bina",&fbina);
    //TBranch *b6;
    //b6=T3->Branch("binrLN",&fbinrLN);
    //TBranch *b7;
    //b7=T3->Branch("Weight",&fWeight);

    // loop on the histogram to fill tree variables & normalize per bin area 
    // starts on purpose at i=0 for the bin boundary case for probr and proba
    for(Int_t i=0;i<hEnergy->GetNbinsX()+1;i++){

      float xbinmin = hEnergy->GetXaxis()->GetBinLowEdge(i) ;
      float xbinmax = hEnergy->GetXaxis()->GetBinUpEdge(i) ;

      // starts on purpse at j=0 for the bin boundary case 
      for(Int_t j=0;j<hEnergy->GetNbinsY()+1;j++){

        float ybinmax = hEnergy->GetYaxis()->GetBinUpEdge(j);
        float ybinmin = hEnergy->GetYaxis()->GetBinLowEdge(j);

        float area = TMath::Pi() * (ybinmax*ybinmax - ybinmin*ybinmin) * ((xbinmax - xbinmin)/(2*TMath::Pi())) ;

        if(i>0 && j>0){
          if(!doNNHit){
            hEnergyDensity->SetBinContent(i,j,hEnergy->GetBinContent(i,j)/area);
            //std::cout << "ENERGY DENSITY : " << hEnergy->GetBinContent(i,j)/area << std::endl;
            hLNEnergyDensity->SetBinContent(i,j,TMath::Log(hEnergyDensity->GetBinContent(i,j)));
            hLNEnergy->SetBinContent(i,j,TMath::Log(hEnergy->GetBinContent(i,j)));
            if(hEnergyDensity->GetBinContent(i,j)<tolerance){
              std::cout << "EMPTY BIN : " << i << " " << j << "  " << hEnergyDensity->GetBinContent(i,j) << std::endl;
              isEmptyBin = true;
              return isEmptyBin;
            }
            // variables for NN fit 
            //fAlphaBinCenter          = hEnergyDensity->GetXaxis()->GetBinCenter(i);
            //fRBinCenter              = hEnergyDensity->GetYaxis()->GetBinCenter(j);
            //fenergyLN                = TMath::Log(hEnergy->GetBinContent(i,j));
            //fenergydensityLN         = TMath::Log(hEnergyDensity->GetBinContent(i,j));
          }
          if(doNNHit){
            float toleranceLN = TMath::Log(tolerance);
            hLNEnergyDensity->SetBinContent(i,j,(TMath::Exp(hLNEnergy->GetBinContent(i,j))/area));
            if(hLNEnergy->GetBinContent(i,j)<toleranceLN && doNNHit){
              std::cout << "EMPTY BIN FOR NN HIT : " << i << " " << j << "  " << hLNEnergy->GetBinContent(i,j) << std::endl;
              isEmptyBin = true;
              return isEmptyBin;
            }
            // variables for NN fit 
            //fAlphaBinCenter          = hLNEnergy->GetXaxis()->GetBinCenter(i); 
            //fRBinCenter              = hLNEnergy->GetYaxis()->GetBinCenter(j); 
            //fenergyLN                = hLNEnergy->GetBinContent(i,j);  
            //fenergydensityLN         = hLNEnergyDensity->GetBinContent(i,j);
          }
          T2->Fill();
        }

        // variables for NN inv fit - starts at i,j=0 because of using the bin boundaries (not the centres)
        //fbina = xbinsAlpha[i];
        //fbinrLN=TMath::Log(ybinsR[j]);
        //if(j==0) fbinrLN=TMath::Log(ybinsR[1])-1.;

        // r probability calculation
        //if(j==0) fprobr    = 0 ;
        //if(j>0)  fprobr    = hAllnorm->Integral(1,hAllnorm->GetNbinsX(),1,j);

        // alpha probability calculation
        //float totalpha = 0 ; 

        //if(i==0) fprobalpha= 0 ;
        //if(j==0){
          //totalpha = hAllnorm->Integral(1,hAllnorm->GetNbinsX(),1,1);
          //if(i>0) fprobalpha = hAllnorm->Integral(1,i,1,1)/totalpha ;
        //}
        //if(i>0 && j>0){
          //totalpha   = hAllnorm->Integral(1,hAllnorm->GetNbinsX(),j,j);
          //fprobalpha = hAllnorm->Integral(1,i,j,j)/totalpha ;
        //}

        // weight 
        if(j==0) area = 1./15.;
        else if(j==hAllnorm->GetNbinsY()) area = 1.;
        //fWeight = area;

        T3->Fill();
      }
    }

    TObjArray Hlist(0);
    Hlist.Add(hEnergyDensity);
    Hlist.Add(hLNEnergyDensity);
    Hlist.Add(hEnergy);
    Hlist.Add(hLNEnergy);
    hr.Rebin(100);
    Hlist.Add(&hr);
    Hlist.Write();

    T2->Write();
    T2->Reset();
    T3->Write();
    T3->Reset();
    f2->Close();


  }//testBinning

  delete(hEnergyDensity);
  delete(hEnergy);
  delete(hNNHit);
  delete(hLNEnergyDensity);
  delete(hLNEnergy);

  return isEmptyBin; 

} //fill()


void DetailedShapeBinning::unitsmm(float eta, float& d_eta, float& d_phi, float cellr, float cellz){

  float phi_dist2r = 1.0 ;
  float dist000    = TMath::Sqrt(cellr*cellr + cellz*cellz);

  float eta_jakobi = TMath::Abs(2.0*TMath::Exp(-eta)/(1.0+TMath::Exp(-2*eta)));
  d_eta       = d_eta * eta_jakobi * dist000 ;
  d_phi       = d_phi * cellr      * phi_dist2r ;


}


void DetailedShapeBinning::getcellgeom(float layer, float etacentre, float phicentre, float& cellr, float& cellz, float& /*celleta*/, float& cell_deta, float& cell_dphi){

  CaloGeometryFromFile* geo=new CaloGeometryFromFile();
  geo->LoadGeometryFromFile("/afs/cern.ch/atlas/groups/Simulation/FastCaloSim/ATLAS-GEO-20-00-01.root","ATLAS-GEO-20-00-01");
  const CaloDetDescrElement* cell;
  cell=geo->getDDE(layer, etacentre, phicentre);

  cellr      = cell->r();
  cellz      = cell->z();
  cell_dphi   = cell->dphi();
  cell_deta   = cell->deta();

}

vector<float> DetailedShapeBinning::fVecBin(TH1F* myh, int nBins, bool doR){

  vector<float> vbin;
  float PerBinValue    = myh->Integral()/nBins ;

  // first bin boundary
  if(doR)  vbin.push_back(0) ;
  else     vbin.push_back(TMath::Pi()/8.);

  float mybincumul = 0 ;
  for(Int_t g=1;g<myh->GetNbinsX()+1;g++){
    if(mybincumul<PerBinValue){
      mybincumul = mybincumul + myh->GetBinContent(g) ;
    }
    if(mybincumul>=PerBinValue){
      vbin.push_back(myh->GetBinLowEdge(g)+myh->GetBinWidth(g)) ;
      mybincumul = 0 ;
    }
  }

  int nRmax = myh->FindLastBinAbove(0);
  float Rmax= myh->GetBinLowEdge(nRmax)+myh->GetBinWidth(nRmax);
  //if(doR) std::cout << "Rmax : " << Rmax << std::endl;

  // last bin boundary
  if(doR) vbin.push_back(Rmax);
  else    vbin.push_back(2*TMath::Pi()+TMath::Pi()/8.);

  return vbin ;
}

bool DetailedShapeBinning::checkgradient(string outfile, TH1F& hGradient, string labeloutput, string labeltitle, bool doNNHit){

  TFile* file =TFile::Open(outfile.c_str());
  TH2F* hist  = (TH2F*)file->Get("hEnergyDensity");
  if(doNNHit)
    hist  = (TH2F*)file->Get("hLNEnergy");

  bool isStatFluc = true;

  for(Int_t j=2;j<hist->GetNbinsY();j++){

    float relvar1 = 0 ; 
    float relvar2 = 0 ; 
    float errGradient = 0 ; 

    for(Int_t i=1;i<hist->GetNbinsX()+1;i++){
      if(!doNNHit){
        relvar1    += (TMath::Log(hist->GetBinContent(i,j))  -TMath::Log(hist->GetBinContent(i,j-1)));///TMath::Log(hist->GetBinContent(i,j-1));
        relvar2    += (TMath::Log(hist->GetBinContent(i,j+1))-TMath::Log(hist->GetBinContent(i,j)));///TMath::Log(hist->GetBinContent(i,j));
        errGradient += (TMath::Log(hist->GetBinError(i,j))  -TMath::Log(hist->GetBinError(i,j-1)));
      }
      if(doNNHit){
        relvar1    += (hist->GetBinContent(i,j)-hist->GetBinContent(i,j-1));
        relvar2    += (hist->GetBinContent(i,j+1)-hist->GetBinContent(i,j));
        errGradient += (hist->GetBinError(i,j)-hist->GetBinError(i,j-1));
      }
    }
    relvar1 = relvar1/hist->GetNbinsX();
    relvar2 = relvar2/hist->GetNbinsX(); 

    hGradient.Fill(hist->GetYaxis()->GetBinCenter(j),relvar1);
    hGradient.SetBinError(j,errGradient);

    // gradient tolerances
    float gradtol = 0 ; 
    if(j<int(hist->GetNbinsY()/3.))
      gradtol = 0.1 ;
    if(j>=int(hist->GetNbinsY()/3.) && j<2*(int(hist->GetNbinsY()/3.)))
      gradtol = 0.3 ;
    if(j>=2*(int(hist->GetNbinsY()/3.)))
      gradtol = 0.8 ;

    if(fabs(relvar1)>gradtol && fabs(relvar2)>gradtol){
      isStatFluc = false;
      std::cout << "GRADIENT PROBLEM (biny,relvar1,relvar2): " << j << " " << relvar1 << "  " << relvar2 << std::endl;
    }
  }  

  TCanvas c0("c0","Gradient",10,10,900,800);
  c0.SetRightMargin(0.13);
  hGradient.SetMarkerStyle(20);
  hGradient.GetYaxis()->SetRangeUser(-1,0);
  hGradient.GetYaxis()->SetTitle("log(#rho_{E}) variation");
  hGradient.GetXaxis()->SetTitle("r [mm]");
  hGradient.Draw("P");
  c0.Update();

  int nzoom1 = int((hGradient.GetNbinsX())/3.*2.)-1;
  int nzoom2 = int((hGradient.GetNbinsX())/3.)-1;

  std::cout << "ZOOM VALUES : " << nzoom1 << " " << nzoom2 << std::endl;

  float Rzoom2 = hGradient.GetXaxis()->GetBinLowEdge(nzoom2);
  float Rzoom1 = hGradient.GetXaxis()->GetBinLowEdge(nzoom1);
  float Rzoom0 = hGradient.GetXaxis()->GetBinLowEdge(hGradient.GetNbinsX()) + hGradient.GetXaxis()->GetBinWidth(hGradient.GetNbinsX());

  TLine lg1p(0,-0.1,Rzoom2,-0.1);
  lg1p.SetLineWidth(2);
  lg1p.SetLineColor(kBlue);
  lg1p.Draw();
  TLine lg2(Rzoom2,-0.1,Rzoom2,-0.3);
  lg2.SetLineWidth(2);
  lg2.SetLineStyle(7);
  lg2.SetLineColor(kBlue);
  lg2.Draw();
  TLine lg2p(Rzoom2,-0.3,Rzoom1,-0.3);
  lg2p.SetLineWidth(2);
  lg2p.SetLineColor(kBlue);
  lg2p.Draw();
  TLine lg3(Rzoom1,-0.3,Rzoom1,-0.8);
  lg3.SetLineWidth(2);
  lg3.SetLineStyle(7);
  lg3.SetLineColor(kBlue);
  lg3.Draw();
  TLine lg3p(Rzoom1,-0.8,Rzoom0,-0.8);
  lg3p.SetLineWidth(2);
  lg3p.SetLineColor(kBlue);
  lg3p.Draw();

  TLatex l;
  l.DrawLatex(0,0.01,(labeltitle).c_str());
  TLatex l2;
  l2.DrawLatex(-1,-1.15,"ATLAS Simulation Preliminary");

  c0.SetLogx();
  c0.SetRightMargin(0.13);
  c0.Update();
  c0.SaveAs(("output/gradient_"+labeloutput+".pdf").c_str());

  return isStatFluc;
}

bool DetailedShapeBinning::checkgranularity(float cell_deta, float cell_dphi, float cellr, float cellz, float cell_eta,float rgran, Float_t* AlphaBins, int nAlphaBins, float& mincellgran){

  cout << "--- Delta(r) [mm] of 3 first bins : " << rgran << std::endl;

  bool isGoodGranularity = true;
  unitsmm(cell_eta,cell_deta,cell_dphi,cellr,cellz);

  mincellgran   = TMath::Min(cell_deta,cell_dphi);
  cout << "--- Cell geometry in mm : " << cell_deta << " " << cell_dphi << std::endl;
  //double mygranres = -1000 ; 

  std::vector<double> va;
  for(Int_t i=0;i<nAlphaBins;i++){
    va.push_back(AlphaBins[i+1]-AlphaBins[i]);
  }

  double alphaMAX = *std::max_element(va.begin(),va.end());
  double alphaMIN = *std::min_element(va.begin(),va.end());

  double etaMAX   = TMath::Sqrt( (rgran*rgran)/(1+pow(TMath::Tan(alphaMAX),2)) );
  double phiMAX   = fabs(etaMAX * TMath::Tan(alphaMAX));

  double etaMIN   = TMath::Sqrt( (rgran*rgran)/(1+pow(TMath::Tan(alphaMIN),2)) );
  double phiMIN   = fabs(etaMIN * TMath::Tan(alphaMIN));

  double minparamgran = 0 ;
  if(mincellgran==cell_deta)
    minparamgran = TMath::Max(etaMIN,etaMAX) ;
  if(mincellgran==cell_dphi)
    minparamgran = TMath::Max(phiMIN,phiMAX) ;

  if(mincellgran<minparamgran){
    isGoodGranularity = false ;
    //mygranres = minparamgran-mincellgran;

    std::cout << "GRANULARITY PROBLEM " << std::endl;
    std::cout << "CELL MINIMUM GRANULARITY ETA/PHI: " << cell_deta << "  " << cell_dphi << "  " << mincellgran << std::endl;
    std::cout << "ETA MIN,MAX FROM PARAM : [" << etaMIN << "," << etaMAX << "]" << std::endl;
    std::cout << "PHI MIN,MAX FROM PARAM : [" << phiMIN << "," << phiMAX << "]" << std::endl;
    std::cout << "MAXIMUM VALUE FOR PARAM : " << minparamgran << std::endl;
  }
  if(mincellgran>=minparamgran){
    std::cout << "Granularity OK : " << minparamgran << " < " << mincellgran << std::endl;
  }


  return isGoodGranularity;
}

void DetailedShapeBinning::fillValidationPlot(float layer,float PCAbin,bool gradientOK, bool granularityOK, string particle, string labeloutput){

  gStyle->SetPalette(1);

  TH2F* htest = new TH2F("htest","htest",10,-0.5,9.5,14,0.5,14.5);
  htest->GetXaxis()->SetTitle("PCA bin");
  htest->GetYaxis()->SetTitle("Calorimeter layer");

  if(granularityOK && gradientOK)
    htest->Fill(PCAbin,layer,1);
  if(granularityOK && !gradientOK)
    htest->Fill(PCAbin,layer,2);
  if(!granularityOK && gradientOK)
    htest->Fill(PCAbin,layer,3);
  if(!granularityOK && !gradientOK)
    htest->Fill(PCAbin,layer,4);

  TCanvas ctest("ctest","dr versus alpha",10,10,900,800);
  htest->Draw("COLZ");

  TLatex l;
  l.SetTextSize(0.03);
  l.DrawLatex(-2,-1,"1= all OK ; 2= bad gradient ; 3= bad granularity");
  l.DrawLatex(-2,-2,"4= bad gradient & granularity");
  TLatex l2;
  string sparticle="";
  if(particle=="pi")
    sparticle="50 GeV pions";
  if(particle=="e")
    sparticle="50 GeV electrons";
  if(particle=="gamma")
    sparticle="50 GeV photons";
  l2.DrawLatex(0,14.5,sparticle.c_str());

  ctest.SetRightMargin(0.13);
  ctest.Update();
  ctest.SaveAs(("output/validation_"+labeloutput+".pdf").c_str());

}
