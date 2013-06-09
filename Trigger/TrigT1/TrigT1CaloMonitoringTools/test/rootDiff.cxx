/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Modified for l1calo by pjwf
// Absolute differences changed to relative differences.
//----------------------------------------
// A Hacky histogram comparison code
// not optimized in anyway and probably incomplete
// use at your own risk
// Sami Kama<sami.kama _you_know_what_ cern.ch>
//----------------------------------------

#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstring>
#include <map>
#include <set>
#include <getopt.h>
#include <cstdlib>
#include <cerrno>
#include <cfloat>
#include <cmath>
#include "TFile.h"
#include "TKey.h"
#include "TBuffer.h"
#include "TH1.h"
#include "TDirectory.h"
#include "TBufferFile.h"
#include "TMath.h"
#include "TArrayD.h"
#include "TClass.h"
#include "THashList.h"


TBufferFile buff1(TBuffer::kWrite),buff2(TBuffer::kWrite);
double epsMatch=1.0e-5;
TFile *outFile=0;
std::string outFileName="";

void saveToFile(const std::string& histName,TH1* h1,TH1* h2,TH1* hc=0){
  if(!outFile)return;
  std::string baseDir=histName.substr(0,histName.rfind("/"));
  //if(baseDir.at(0)=='/')baseDir=baseDir.substr(1);
  std::string rootDir=outFileName+":/";
  std::vector<std::string> dirs;
  size_t pos=0,len=baseDir.size();
  for(size_t t=0;t<len;t++){
    if(baseDir[t]=='/'){
      if(t>pos+1){
	dirs.push_back(baseDir.substr(pos+1,t-pos-1));
	pos=t;
      }
    }
  }
  if(pos<len-1){
    dirs.push_back(baseDir.substr(pos+1));
  }
  outFile->cd("/");
  TDirectory* currDir=outFile->GetDirectory("/file1");
  std::vector<std::string>::iterator it=dirs.begin(),itend=dirs.end();
  while(it!=itend){
    currDir->cd();
    TDirectory* subDir=currDir->GetDirectory(it->c_str());
    if(!subDir){
      subDir=currDir->mkdir(it->c_str());
    }
    currDir=subDir;
    ++it;
  }
  currDir->cd();
  h1->Write();
  outFile->cd("/");
  currDir=outFile->GetDirectory("/file2");
  it=dirs.begin();
  while(it!=itend){
    currDir->cd();
    TDirectory* subDir=currDir->GetDirectory(it->c_str());
    if(!subDir){
      subDir=currDir->mkdir(it->c_str());
    }
    currDir=subDir;
    ++it;
  }
  currDir->cd();
  h2->Write();
  if(hc){
    outFile->cd("/");
    currDir=outFile->GetDirectory("/subtracted");
    it=dirs.begin();
    while(it!=itend){
      currDir->cd();
      TDirectory* subDir=currDir->GetDirectory(it->c_str());
      if(!subDir){
	subDir=currDir->mkdir(it->c_str());
      }
      currDir=subDir;
      ++it;
    }
    currDir->cd();
    hc->Write();
  }
}

void printUsage(std::string myName){
  if(myName.rfind('/')!=std::string::npos)myName=myName.substr(myName.rfind('/')+1);
  std::cout<<"Usage: "
	   <<myName<<" [OPTIONS] file1.root file2.root"<<std::endl
	   <<"Compare histograms in two root files"<<std::endl<<std::endl
	   <<"      -b,--buffers              Compare object buffers, (perfect identity)"<<std::endl
	   <<"      -e,--epsilon  <value>     Assume difference less than <value> as equal (default "
	   <<epsMatch<<")"<<std::endl
	   <<"      -f,--diff-file <file>     save output to file"<<std::endl
	   <<"      -h,--help                 display this message"<<std::endl
	   <<std::endl;
}

void scanDirectory(std::map<std::string,TH1*>* m,TDirectory* dir){
  if(!dir)return;
  if(!m) return;
  TIter n(dir->GetListOfKeys());
  TKey *k=0;
  std::string currPath=dir->GetPath();
  currPath=currPath.substr(currPath.rfind(':')+1);
  while((k=(TKey*)n())){
    TObject* o=k->ReadObj();
    TDirectory *sd=dynamic_cast<TDirectory*>(o);
    if(sd){
      scanDirectory(m,sd);
      continue;
    }
    TH1* h=dynamic_cast<TH1*>(o);
    if(h){
      TH1* hc=dynamic_cast<TH1*>(h->Clone(h->GetName()));
      hc->SetDirectory(0);
      m->operator[](currPath+"/"+std::string(hc->GetName()))=hc;
    }
    delete o;
  }
}

bool checkAxisLabels(const THashList *l1,const THashList* l2,std::string& errMsg){
  if(!l1||!l2)return false;
  TIter lx1(l1);
  TIter lx2(l2);
  std::set<std::string> lx1labels,lx2labels;
  TObjString *obj=0;
  while((obj=(TObjString*)lx1())){
    lx1labels.insert(obj->GetString().Data());
  }
  std::set<std::string>::iterator it,itend2,itend=lx1labels.end();
	
  while((obj=(TObjString*)lx2())){
    std::string strData(obj->GetString().Data());
    if(lx1labels.find(strData)==itend){
      errMsg=strData+"\" in file2 but not in file1";
      return false;
    }
    lx2labels.insert(strData);
  }
  it=lx1labels.begin();
  itend2=lx2labels.end();
  while(it!=itend){
    if(lx2labels.find(*it)==itend2){
      errMsg=(*it)+"\" in file1 but not in file2";
      return false;
    }
    it++;
  }
  return true;
}

bool checkEquivalence(TH1* h1,TH1* h2,const std::string& hname){
  if(!h1||!h2)return false;
  int dim=h1->GetDimension();
  if(dim!=h2->GetDimension()){
    std::cout<<"\""<<hname<<"\" has different dimensions in different files ( "
	     <<dim<<" vs "<<h2->GetDimension()<<" )"
	     <<std::endl;
    saveToFile(hname,h1,h2);    
    return false;
  }
  int nbinsX=h1->GetNbinsX();
  int nbinsY=h1->GetNbinsY();
  int nbinsZ=h1->GetNbinsZ();
  
  if(nbinsX!=h2->GetNbinsX()
     ||nbinsY!=h2->GetNbinsY()
     ||nbinsZ!=h2->GetNbinsZ()
     ){
    std::cout<<"\""<<hname<<"\" has different binning in different files ( ";

    if(dim==1){
      std::cout<<nbinsX<<" vs "<<h2->GetNbinsX();
    }else if(dim==2){
      std::cout<<nbinsX<<","<<nbinsY<<" vs "<<h2->GetNbinsX()
	       <<","<<h2->GetNbinsY();
    }else{
      std::cout<<nbinsX<<","<<nbinsY<<","<<nbinsZ<<" vs "<<h2->GetNbinsX()
	       <<","<<h2->GetNbinsY()
	       <<","<<h2->GetNbinsZ();

    }
    std::cout<<" )"<<std::endl;
    saveToFile(hname,h1,h2);
    return false;
  }
  
  //if((TMath::Abs(h1->GetEntries()-h2->GetEntries())>epsMatch)
  //   ||(TMath::Abs(h1->GetEffectiveEntries()-h2->GetEffectiveEntries())>epsMatch)
  if((TMath::Abs(1.-h2->GetEntries()/h1->GetEntries())>epsMatch)
     ||(TMath::Abs(1.-h2->GetEffectiveEntries()/h1->GetEffectiveEntries())>epsMatch)
     ){
    std::cout<<"\""<<hname<<"\" has different number of entries in different files ( entries= " 
	     <<std::setprecision(16)<<h1->GetEntries()<<" vs "<<std::setprecision(16)<<h2->GetEntries()
	     <<", effective entries= "
      	     <<std::setprecision(16)<<h1->GetEffectiveEntries()<<" vs "
	     <<std::setprecision(16)<<h2->GetEffectiveEntries()
	     <<" )"<<std::endl;
    saveToFile(hname,h1,h2);
    //return false;
  }
  
  
  
  if((h1->GetSumw2()!=0) xor (h2->GetSumw2()!=0)){//if either has sumw2()
    std::cout<<"\""<<hname<<"\" has Sumw2 array not available in one of the files "
	     <<"h1= "<<h1->GetSumw2()<<" h2="<<h2->GetSumw2()
	     << std::endl;
    saveToFile(hname,h1,h2);
    return false;
  }
  
  int nhs=h1->GetSumw2N();
  if(nhs!=h2->GetSumw2N()){
    std::cout<<"\""<<hname<<"\" has Sumw2 array sizes are different ( "
	     <<std::setprecision(16)<<nhs<<" vs "<<std::setprecision(16)<<h2->GetSumw2N() <<" )"
	     <<std::endl;
    saveToFile(hname,h1,h2);
    return false;
  }
  
  if(nhs>0){
    const double * a1=h1->GetSumw2()->GetArray();
    const double * a2=h2->GetSumw2()->GetArray();
    for(int i=0;i<nhs;i++){
      //if(TMath::Abs(a1[i]-a2[i])>epsMatch){
      if(TMath::Abs(1.-a2[i]/a1[i])>epsMatch){
	std::cout<<"\""<<hname<<"\" has Sumw2 arrays has a difference greater than "<<epsMatch
		 <<" at bin "<<i<<" ( "
		 <<std::setprecision(16)<<a1[i]<<" vs "
		 <<std::setprecision(16)<<a2[i]<<" )"
		 <<std::endl;
	saveToFile(hname,h1,h2);
	//return false;
	break;
      }
    }
  }

  TH1 *hc=dynamic_cast<TH1*>(h1->Clone());
  if(!hc){
    std::cout<<"Cloning failed "<<hname<<std::endl;
    return false;
  }
  TH1 *hc2=dynamic_cast<TH1*>(h2->Clone());
  if(!hc2){
    std::cout<<"Cloning failed "<<hname<<std::endl;
    return false;
  }
  double norm = 1./hc->Integral();
  hc->Scale(norm);
  hc2->Scale(norm);
  hc->SetDirectory(0);
  hc->Add(hc2,-1.0);
  //What else to check?
  bool isEquivalent=true;
  if((TMath::Abs(hc->Integral())>epsMatch)||(TMath::Abs(hc->Integral("w"))>epsMatch)){
    std::cout<<"\""<<hname<<"\" Integrals don't match, integral of difference histogram is "
	     <<std::setprecision(16)<<hc->Integral()
	     <<std::endl;
    saveToFile(hname,h1,h2,hc);
    //delete hc;
    //delete hc2;
    //return false;
  }
  delete hc;
  delete hc2;
  hc = 0;
  hc2 = 0;

  if(dim==1){
    int nbins=h1->GetNbinsX()+2;//0-nbins+1
    for(int i=0;i<nbins;i++){
      //if(TMath::Abs(h1->GetBinContent(i)-h2->GetBinContent(i))>epsMatch){
      if(TMath::Abs(1.-h2->GetBinContent(i)/h1->GetBinContent(i))>epsMatch){
	std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()
		 <<") has bin difference greater than "
		 <<epsMatch<<" in bin "<<i<<" ( "
		 <<std::setprecision(16)<<h1->GetBinContent(i)<<" vs "
		 <<std::setprecision(16)<<h2->GetBinContent(i)<<" )"
		 <<std::endl;
	saveToFile(hname,h1,h2);
	delete hc;
	//return false;
	break;
      }
    }
    //check axis labels
    THashList* h1Labels=h1->GetXaxis()->GetLabels();
    THashList* h2Labels=h2->GetXaxis()->GetLabels();
    if( (h1Labels!=0) ^ (h2Labels!=0)){
      std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()<<") "
	       <<" has bin labels in "<<((h1Labels!=0)?" file1 but not in file2 ":" file2 but not in file1 ")
	       <<std::endl;
      saveToFile(hname,h1,h2);
      delete hc;
      return false;
      
    }
    if(h1Labels){
      std::string errMsg;
      if(!checkAxisLabels(h1Labels,h2Labels,errMsg)){
	std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()<<") "
		 <<" has bin label \""<<errMsg
		 <<std::endl;
	saveToFile(hname,h1,h2);
	delete hc;
	return false;
      }
    }
  }else if(dim==2){
    int nbinsx=h1->GetNbinsX()+2;//0-nbins+1
    int nbinsy=h1->GetNbinsY()+2;//0-nbins+1
    bool pleaseBreak = false;
    for(int j=0;j<nbinsy;j++){
      int offset=j*nbinsx;
      for(int i=0;i<nbinsx;i++){
	//if(TMath::Abs(h1->GetBinContent(offset+i)-h2->GetBinContent(offset+i))>epsMatch){
	if(TMath::Abs(1.-h2->GetBinContent(offset+i)/h1->GetBinContent(offset+i))>epsMatch){
	  std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()
		   <<") has bin difference greater than "
		   <<epsMatch<<" in bin "<<std::setprecision(16)<<offset+i<<" ( "
	    //		   <<hc->GetBinContent(offset+i)
		   <<std::setprecision(16)<<h1->GetBinContent(offset+i)<<" vs "
		   <<std::setprecision(16)<<h2->GetBinContent(offset+i)<<" )"
		   <<std::endl;

	  saveToFile(hname,h1,h2);
	  delete hc;
	  //return false;
	  pleaseBreak = true;
	  break;
	}
      }
      if (pleaseBreak) break;
    }
    THashList* h1Labels=h1->GetXaxis()->GetLabels();
    THashList* h2Labels=h2->GetXaxis()->GetLabels();
    if( (h1Labels!=0) ^ (h2Labels!=0)){
      std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()<<") "
	       <<" has x-axis bin labels in "<<((h1Labels!=0)?" file1 but not in file2 ":" file2 but not in file1 ")
	       <<std::endl;
      saveToFile(hname,h1,h2);
      delete hc;
      return false;
    }
    if(h1Labels){
      std::string errMsg;
      if(!checkAxisLabels(h1Labels,h2Labels,errMsg)){
	std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()<<") "
		 <<" has bin label \""<<errMsg
		 <<std::endl;
	saveToFile(hname,h1,h2);
	delete hc;
	return false;
      }
    }
    h1Labels=h1->GetYaxis()->GetLabels();
    h2Labels=h2->GetYaxis()->GetLabels();
    if( (h1Labels!=0) ^ (h2Labels!=0)){
      std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()<<") "
	       <<" has y-axis bin labels in "<<((h1Labels!=0)?" file1 but not in file2 ":" file2 but not in file1 ")
	       <<std::endl;
      saveToFile(hname,h1,h2);
      delete hc;
      return false;
    }
    if(h1Labels){
      std::string errMsg;
      if(!checkAxisLabels(h1Labels,h2Labels,errMsg)){
	std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()<<") "
		 <<" has y-axis bin label \""<<errMsg
		 <<std::endl;
	saveToFile(hname,h1,h2);
	delete hc;
	return false;
      }
    }
    
  }else{
    int nbinsx=h1->GetNbinsX()+2;//0-nbins+1
    int nbinsy=h1->GetNbinsY()+2;//0-nbins+1
    int nbinsz=h1->GetNbinsZ()+2;//0-nbins+1
    bool pleaseBreak = false;
    for(int k=0;k<nbinsz;k++){
      int offsetz=k*nbinsy*nbinsx;
      for(int j=0;j<nbinsy;j++){
	int offset=offsetz+j*nbinsx;
	for(int i=0;i<nbinsx;i++){
	  //if(TMath::Abs(h1->GetBinContent(offset+i)-h2->GetBinContent(offset+i))>epsMatch){
	  if(TMath::Abs(1.-h2->GetBinContent(offset+i)/h1->GetBinContent(offset+i))>epsMatch){
	    std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()
		     <<") has bin difference greater than "
		     <<epsMatch<<" in bin "<<std::setprecision(16)<<offset+i<<" ( "
	      //<<hc->GetBinContent(offset+i)
		     <<std::setprecision(16)<<h1->GetBinContent(offset+i)<<" vs "
		     <<std::setprecision(16)<<h2->GetBinContent(offset+i)<<" )"
		     <<std::endl;
	    saveToFile(hname,h1,h2);
	    delete hc;
	    //return false;
	    pleaseBreak = true;
	    break;
	  }
	}
	if (pleaseBreak) break;
      }
      if (pleaseBreak) break;
    }
    THashList* h1Labels=h1->GetXaxis()->GetLabels();
    THashList* h2Labels=h2->GetXaxis()->GetLabels();
    if( (h1Labels!=0) ^ (h2Labels!=0)){
      std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()<<") "
	       <<" has x-axis bin labels in "<<((h1Labels!=0)?" file1 but not in file2 ":" file2 but not in file1 ")
	       <<std::endl;
      saveToFile(hname,h1,h2);
      delete hc;
      return false;
    }
    if(h1Labels){
      std::string errMsg;
      if(!checkAxisLabels(h1Labels,h2Labels,errMsg)){
	std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()<<") "
		 <<" has x-axis bin label \""<<errMsg
		 <<std::endl;
	saveToFile(hname,h1,h2);
	delete hc;
	return false;
      }
    }
    h1Labels=h1->GetYaxis()->GetLabels();
    h2Labels=h2->GetYaxis()->GetLabels();
    if( (h1Labels!=0) ^ (h2Labels!=0)){
      std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()<<") "
	       <<" has y-axis bin labels in "<<((h1Labels!=0)?" file1 but not in file2 ":" file2 but not in file1 ")
	       <<std::endl;
      saveToFile(hname,h1,h2);
      delete hc;
      return false;
    }
    if(h1Labels){
      std::string errMsg;
      if(!checkAxisLabels(h1Labels,h2Labels,errMsg)){
	std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()<<") "
		 <<" has y-axis bin label \""<<errMsg
		 <<std::endl;
	saveToFile(hname,h1,h2);
	delete hc;
	return false;
      }
    }
    h1Labels=h1->GetZaxis()->GetLabels();
    h2Labels=h2->GetZaxis()->GetLabels();
    if( (h1Labels!=0) ^ (h2Labels!=0)){
      std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()<<") "
	       <<" has z-axis bin labels in "<<((h1Labels!=0)?" file1 but not in file2 ":" file2 but not in file1 ")
	       <<std::endl;
      saveToFile(hname,h1,h2);
      delete hc;
      return false;
    }
    if(h1Labels){
      std::string errMsg;
      if(!checkAxisLabels(h1Labels,h2Labels,errMsg)){
	std::cout<<"\""<<hname<<"\" ("<<h1->IsA()->GetName()<<") "
		 <<" has z-axis bin label \""<<errMsg
		 <<std::endl;
	saveToFile(hname,h1,h2);
	delete hc;
	return false;
      }
    }

  }
  return isEquivalent;
}

bool compareBuffers(TH1* h1,TH1 *h2){
  if(!h1 || !h2)return false;
  buff1.Reset();
  buff2.Reset();
  h1->Streamer(buff1);
  h2->Streamer(buff2);
  int size1=buff1.BufferSize();
  int size2=buff2.BufferSize();
  if(size1!=size2)return false;
  char* buffMem1=buff1.Buffer();
  char* buffMem2=buff2.Buffer();
  return ( ::memcmp(buffMem1,buffMem2,size1)==0 );
  //return true;
}

std::map<std::string,TH1*>* getHistMap(TFile* inp){
  if(!inp)return 0;
  if(!inp->IsOpen() || inp->IsZombie())return 0;
  std::map<std::string,TH1*>* histos=new std::map<std::string,TH1*>();
  inp->cd("/");
  scanDirectory(histos,inp);
  return histos;
}

void compareHistos(std::map<std::string,TH1*> *m1,
		    std::map<std::string,TH1*> *m2,
		    bool compBuffers){
  if(!m1||!m2)return;
  std::map<std::string,TH1*>::iterator it1,it2,it1end(m1->end()),it2end(m2->end());
  int numMissing=0,numDifferent=0,numBuffDifferent=0,numBothFail=0;
  if(compBuffers){
    bool buffFail,checkFail;
    for(it1=m1->begin();it1!=it1end;++it1){
      buffFail=false;
      checkFail=false;
      std::cout<<std::flush;
      if((it2=m2->find(it1->first))==it2end){// histo missing in file2
	std::cout<<"\""<<it1->first<<"\" missing in file2"<<std::endl;
	numMissing++;
      }else{
	if(!compareBuffers(it1->second,it2->second)){
	  std::cout<<"\""<<it1->first<<"\" buffers are different between files"<<std::endl;
	  numBuffDifferent++;
	  buffFail=true;
	}
	if(!checkEquivalence(it1->second,it2->second,it1->first)){
	  numDifferent++;
	  checkFail=true;
	}
	if(checkFail&&!buffFail){
	  std::cout<<"--!!!--- "<<it1->first
		   <<" buffer check passed while equivalence check failed!!!"<<std::endl;
	}
	if(checkFail&&buffFail)numBothFail++;
      }
    }
  }else{
    for(it1=m1->begin();it1!=it1end;++it1){
      if((it2=m2->find(it1->first))==it2end){// histo missing in file2
	std::cout<<"\""<<it1->first<<"\" missing in file2"<<std::endl;
      }else{
	if(!checkEquivalence(it1->second,it2->second,it1->first)){
	  numDifferent++;
	}
      }
    }
    
  }
  int numF1only=numMissing;
  for(it2=m2->begin();it2!=it2end;++it2){
    if((it1=m1->find(it2->first))==it1end){// histo missing in file2
      std::cout<<"\""<<it2->first<<"\" missing in file1"<<std::endl;
      numMissing++;
    }
  }
  std::cout<<std::endl<<"========================================"<<std::endl;
  std::cout<<numDifferent<<" Different histograms between files"<<std::endl;
  if(compBuffers){
    std::cout<<numBuffDifferent<<" differs in buffer comparison";
    if(numBothFail>0){
      std::cout<<" ("<<numBothFail<<" of them failed both checks)";
    }
    std::cout<<std::endl;
  }
  if(numMissing==0){
    std::cout<<"Both files contain same histograms ("<<m1->size()
	     <<" in total)"<<std::endl;
  }else{
    std::cout<<numMissing<<" histograms do not exist in one of files"<<std::endl
	     <<"File1 has "<<m1->size()-numF1only<<"/"<<m1->size()
	     <<" common histograms"<<std::endl
	     <<"File2 has "<<m2->size()+numF1only-numMissing<<"/"<<m2->size()
	     <<" common histograms"
	     <<std::endl;
  }
}

int main(int argc,char* argv[]){
  int c;
  bool compareBuffers=false;
  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
      {"buffers", 0, 0, 0},
      {"help", 0, 0, 'h'},
      {"epsilon", 1, 0, 'e'},
      {"diff-file", 1, 0, 'f'},
      {0, 0, 0, 0}
    };

    c = getopt_long(argc, argv, "bhe:f:",
		    long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
    case 'h':
      printUsage(argv[0]);
      exit(EXIT_SUCCESS);
      break;
    case 'b':
      compareBuffers=true;
      break;
    case 'e':
      char* endptr;
      errno=0;
      epsMatch=strtod(optarg,&endptr);
      if((errno==ERANGE && (epsMatch == HUGE_VAL || epsMatch == -HUGE_VAL))
	 || (errno !=0 && epsMatch==0.0)){
	perror("strtod");
	exit(EXIT_FAILURE);	
      }
	 //compareBuffers=true;
      break;
    case 'f':
      outFileName=std::string(optarg);
      break;
    default:
      printf("?? getopt returned character code 0%o ??\n", c);
    }
  }
  if(argc-optind!=2){
    std::cerr<<"Need two file names"<<std::endl;
    exit(EXIT_FAILURE);
  }
  std::string fName1=argv[optind];
  std::string fName2=argv[optind+1];
  TFile * f1=TFile::Open(fName1.c_str());
  if(!f1||!f1->IsOpen()||f1->IsZombie()){
    std::cerr<<"Can't open file "<<fName1<<std::endl;
    exit(EXIT_FAILURE);
  }
  TFile * f2=TFile::Open(fName2.c_str());
  if(!f2||!f2->IsOpen()||f2->IsZombie()){
    std::cerr<<"Can't open file "<<fName2<<std::endl;
    exit(EXIT_FAILURE);
  }
  if(!outFileName.empty()){
    outFile=TFile::Open(outFileName.c_str(),"RECREATE");
    if(!outFile||!outFile->IsOpen()||outFile->IsZombie()){
      std::cerr<<"Can't open output file "<<outFileName<<std::endl;
      exit(EXIT_FAILURE);
    }
    outFile->cd("/");
    outFile->mkdir("file1");
    outFile->cd("/");
    outFile->mkdir("file2");
    outFile->cd("/");
    outFile->mkdir("subtracted");
    
  }
  std::cout<<"Scanning "<<fName1<<std::endl;
  std::map<std::string,TH1*>* f1Map=getHistMap(f1);
  std::cout<<"Scanning "<<fName2<<std::endl;
  std::map<std::string,TH1*>* f2Map=getHistMap(f2);
  std::cout<<"file1 contains "<<f1Map->size()<<" histograms"<<std::endl;
  std::cout<<"file2 contains "<<f2Map->size()<<" histograms"<<std::endl;
  compareHistos(f1Map,f2Map,compareBuffers);
  if(outFile){
    outFile->Flush();
    outFile->Close();
  }

}
