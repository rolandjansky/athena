#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include "TH1D.h"
#include "TCanvas.h"
#include "TRandom.h"

void RunDigiToy(int nRDO=1000000, int nRDOperFile=1000, int nHighPtMB=100000, int nHighPtMBperjob=4000, bool rndHighPtMBinfileOffset=true, bool veto_intime_reuse=false)
{
  gRandom->SetSeed(4321);
  
  int njobs=nRDO/nRDOperFile;
  int nHighPtMBfiles=nHighPtMB/nHighPtMBperjob;
  double mu=40;
  double highpTfrac=0.0017;
  int ntime=39;

  int nprint=njobs/100;

  std::vector< int > nuse(nHighPtMB+1,0);
  std::vector< int > nuse_outoftime(nHighPtMB+1,0);
  
  double prob=mu*nRDO*highpTfrac/nHighPtMB;
  std::cout<<"Probability for one high pT MB event to be taken="<<prob<<std::endl;
  
  TH1D* hist_poisson=new TH1D(Form("ReuseHighPtMB_intime_RDO%d_HpT%d_rndoff%d_veto%d_nRDO%d",nRDOperFile,nHighPtMBperjob,rndHighPtMBinfileOffset,veto_intime_reuse,nRDO),"In-time re-use high pT minbias",10,-0.5,9.5);
  TH1D* hist_poisson_ideal=new TH1D(Form("Expected_ReuseHighPtMB_intime_RDO%d_HpT%d_rndoff%d_veto%d_nRDO%d",nRDOperFile,nHighPtMBperjob,rndHighPtMBinfileOffset,veto_intime_reuse,nRDO),"Expected in-time re-use high pT minbias",10,-0.5,9.5);
  TH1D* hist_poisson_outoftime=new TH1D(Form("ReuseHighPtMB_total_RDO%d_HpT%d_rndoff%d_veto%d_nRDO%d",nRDOperFile,nHighPtMBperjob,rndHighPtMBinfileOffset,veto_intime_reuse,nRDO),"Total re-use high pT minbias",100,-0.5,99.5);

  for(int ijob=0;ijob<njobs;++ijob) {  
    //randomly pick one high pT minbias event
    int iMBfile=gRandom->Uniform(0,nHighPtMBfiles);
    if(iMBfile>=nHighPtMBfiles) iMBfile-=nHighPtMBfiles;
    int iMBbeg=iMBfile * nHighPtMBperjob;
    int iMBend=iMBbeg + nHighPtMBperjob;
    int iMB=iMBbeg;
    if(rndHighPtMBinfileOffset) {
      iMB=gRandom->Uniform(iMBbeg,iMBend);
      if(iMB>=iMBend) iMB=iMBbeg;
    }
    for(int iRDO=0;iRDO<nRDOperFile;++iRDO) 
    {
      std::vector< int > veto(nHighPtMBperjob,0);
      for(int itime=0;itime<ntime;++itime) {
        int nmu=gRandom->Poisson(mu);
        if(ijob%nprint==0 && iRDO<2 && itime<2) std::cout<<"job="<<ijob<<" event="<<iRDO<<", itime="<<itime<<": nmu="<<nmu<<" iMB="<<iMB<<" nuse[iMB]="<<nuse[iMB]<<" nuse_outoftime[iMB]="<<nuse_outoftime[iMB]<<std::endl;
        for(int imu=0;imu<nmu;++imu) {
          double rnd=gRandom->Rndm();
          if(rnd<highpTfrac) {
            //std::cout<<"event="<<iRDO<<": high pT event, rnd="<<rnd<<std::endl;
            if(itime==0) {
              if(veto[iMB-iMBbeg]>0) {
                for(int itest=0;itest<nHighPtMBperjob;++itest) {
                  ++iMB;
                  if(veto[iMB-iMBbeg]==0) break;
                }
              }
              ++nuse[iMB];
              ++veto[iMB-iMBbeg];
            }  
            ++nuse_outoftime[iMB];
            ++iMB;
            if(iMB>=iMBend) {
              iMB=iMBbeg;
              std::cout<<"event="<<iRDO<<": starting with MB events from the beginning iMB="<<iMB<<std::endl;
            }  
          }
        }  
      }  
    }
  }  

  for(int iMB=0;iMB<nHighPtMB;++iMB) {
    hist_poisson->Fill(nuse[iMB]);
    hist_poisson_ideal->Fill(gRandom->Poisson(prob));
    hist_poisson_outoftime->Fill(nuse_outoftime[iMB]);
  }

  TCanvas* c=new TCanvas(hist_poisson->GetName(),hist_poisson->GetTitle());
  hist_poisson->Draw();
  hist_poisson_ideal->SetLineColor(2);
  hist_poisson_ideal->Draw("same");
  c->SaveAs(".png");
  c->SaveAs(".root");

  c=new TCanvas(hist_poisson_outoftime->GetName(),hist_poisson_outoftime->GetTitle());
  hist_poisson_outoftime->Draw();
  c->SaveAs(".png");
  c->SaveAs(".root");
}

void RunRDOToy(int nRDO=1000000, int nRDOperFile=1000, int nMCperSample=10000, int nMCsamples=100, bool rndRDO=true)
{
  gRandom->SetSeed(4321);
  
  int nRDOfiles=nRDO/nRDOperFile;
  int nRDOfilesperMCSample=nMCperSample/nRDOperFile;
  
  std::cout<<"nRDOfiles="<<nRDOfiles<<std::endl;
  std::cout<<"nRDOfilesperMCSample="<<nRDOfilesperMCSample<<std::endl;
  
  std::vector< std::set< int > > usedRDO(nMCsamples);

  TH1D* hist_overlap=new TH1D(Form("Overlap_RDO%d_MC%d_rndRDO%d",nRDOperFile,nMCperSample,rndRDO),Form("Overlap of RDO files between %d samples",nMCsamples),100,0,1);
  
  int firstRDO=0;
  for(int iMC=0;iMC<nMCsamples;++iMC) {
    for(int iRDO=0;iRDO<nRDOfilesperMCSample;++iRDO) {
      int indexRDO;
      if(rndRDO || iRDO==0) {
        do {
          indexRDO=gRandom->Uniform(0,nRDOfiles);
          if(indexRDO>=nRDOfiles) indexRDO=0;
        } while (usedRDO[iMC].count(indexRDO)>0);
        firstRDO=indexRDO;
      } else {
        indexRDO=firstRDO+iRDO;
        if(indexRDO>=nRDOfiles) indexRDO-=nRDOfiles;
      }  
      usedRDO[iMC].insert(indexRDO);
    }
    for(int jMC=0;jMC<iMC;++jMC) {
      std::set<int> intersect;
      set_intersection(usedRDO[iMC].begin(), usedRDO[iMC].end(), usedRDO[jMC].begin(), usedRDO[jMC].end(), std::inserter(intersect, intersect.begin()));    
      double size=std::min(usedRDO[iMC].size(),usedRDO[jMC].size());
      double overlap=intersect.size()/size;
      hist_overlap->Fill(overlap);
      //std::cout<<"MC1="<<iMC<<" MC2="<<jMC<<" size="<<size<<" overlap="<<overlap<<std::endl;
    }
  }

  TCanvas* c=new TCanvas(hist_overlap->GetName(),hist_overlap->GetTitle());
  hist_overlap->SetMinimum(0.1);
  hist_overlap->Draw();
  c->SetLogy();
  c->SaveAs(".png");
  c->SaveAs(".root");

}

void RunPileUpOverlayReuseToys()
{
  //Run toys for the re-use of high pT minbias files

  int nRDO=1000000;
  int nHighPtMB=100000;
  int nRDOperFile=1000; //MC16=1000
  int nHighPtMBperjob=4000; //MC16=4000
  bool rndHighPtMBinfileOffset=true;
  bool veto_intime_reuse=false;
  
  //MC16 default
  nRDOperFile=1000;
  nHighPtMBperjob=4000;
  rndHighPtMBinfileOffset=false;
  veto_intime_reuse=false;
  RunDigiToy(nRDO, nRDOperFile, nHighPtMB, nHighPtMBperjob, rndHighPtMBinfileOffset, veto_intime_reuse);

  //MC20 proposal
  nRDOperFile=1000;
  nHighPtMBperjob=4000;
  rndHighPtMBinfileOffset=true;
  veto_intime_reuse=false;
  RunDigiToy(nRDO, nRDOperFile, nHighPtMB, nHighPtMBperjob, rndHighPtMBinfileOffset, veto_intime_reuse);

  //MC20 proposal + infile intime reuse veto
  nRDOperFile=1000;
  nHighPtMBperjob=4000;
  rndHighPtMBinfileOffset=true;
  veto_intime_reuse=true;
  RunDigiToy(nRDO, nRDOperFile, nHighPtMB, nHighPtMBperjob, rndHighPtMBinfileOffset, veto_intime_reuse);

  //MC20 2000events/RDO
  nRDOperFile=2000;
  nHighPtMBperjob=4000;
  rndHighPtMBinfileOffset=true;
  veto_intime_reuse=false;
  RunDigiToy(nRDO, nRDOperFile, nHighPtMB, nHighPtMBperjob, rndHighPtMBinfileOffset, veto_intime_reuse);

  //MC20 2000events/RDO, 10*more stat for toys
  nRDO=10000000;
  nHighPtMB=1000000;
  nRDOperFile=2000;
  nHighPtMBperjob=4000;
  rndHighPtMBinfileOffset=true;
  veto_intime_reuse=false;
  RunDigiToy(nRDO, nRDOperFile, nHighPtMB, nHighPtMBperjob, rndHighPtMBinfileOffset, veto_intime_reuse);

  //MC20 2000events/RDO, 5000events/minbias file
  nRDO=10000000;
  nHighPtMB=1000000;
  nRDOperFile=2000;
  nHighPtMBperjob=5000;
  rndHighPtMBinfileOffset=true;
  veto_intime_reuse=false;
  RunDigiToy(nRDO, nRDOperFile, nHighPtMB, nHighPtMBperjob, rndHighPtMBinfileOffset, veto_intime_reuse);

  //MC20 2000events/RDO, 10000events/minbias file
  nRDO=10000000;
  nHighPtMB=1000000;
  nRDOperFile=2000;
  nHighPtMBperjob=10000;
  rndHighPtMBinfileOffset=true;
  veto_intime_reuse=false;
  RunDigiToy(nRDO, nRDOperFile, nHighPtMB, nHighPtMBperjob, rndHighPtMBinfileOffset, veto_intime_reuse);

  //Run toys for the re-use of RDO files

  nRDO=1000000000;
  nRDOperFile=2000;
  int nMCperSample=10000000;
  int nMCsamples=100;
  bool rndRDO=false;
  
  RunRDOToy(nRDO, nRDOperFile, nMCperSample, nMCsamples, rndRDO);

  rndRDO=true;
  RunRDOToy(nRDO, nRDOperFile, nMCperSample, nMCsamples, rndRDO);

}

