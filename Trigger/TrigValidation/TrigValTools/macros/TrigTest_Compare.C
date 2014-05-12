/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* checks differences in chains
   between two sets of expert monitoring files */

#include <TTree.h>
#include <TFile.h>
#include <TMath.h>
#include <TH1.h>
#include <iostream> 
#include <iomanip>
#include <Riostream.h> 


Bool_t pri = true;

void  diff(TString *fname, TString *hname, TString *ahname, Float_t toler, ofstream *output, ofstream *outc)
{
  ifstream newdfn;
  ifstream olddfn;
  ifstream slnames;
  ifstream ndates;
  ifstream odates;
  ifstream cputms;
  ifstream maxmms;
  newdfn.open("new_stest_list.txt");
  olddfn.open("old_stest_list.txt");
  slnames.open("slicenames.txt");
  ndates.open("new_dates.txt");
  odates.open("old_dates.txt");
  cputms.open("cpu_time.txt");
  maxmms.open("max_memory.txt");
  TString **new_test_list = new TString[12];
  TString **old_test_list = new TString[12];
  TString **new_dates = new TString[12];
  TString **old_dates = new TString[12];
  TString **cpu_times = new TString[12];
  TString **max_mems = new TString[12]; 
  Bool_t *neolm = new Bool_t[12];
  TString **t_names = new TString[12];
  TString *test_name = new TString("unknown"); 
  char *ltrs = new char[12];
  char *buf = new char[400];
  char *month = new char[200];
  char *dom = new char[200];
  char *tim = new char[200];
  char *dstri = new char[400];
  char *sink = new char[400];
  Int_t ind = 0;
  Int_t oin = 0;
  Int_t maxnb = 300;
  if(!newdfn.is_open())
  {
      cout << "no valid new RTT jobs for this slice found ! Aborting!" << endl;
      *output << "no valid new RTT jobs for this slice found ! Aborting!" << endl;
      *outc << "no valid new RTT jobs for this slice found ! Aborting!" << endl;
      return;
  }        
  if(!olddfn.is_open())
  {
      cout << "no valid reference files found ! Aborting!" << endl;
      return;
  }
  if(!slnames.is_open())
  {
      cout << "can't find file with slice names ! Aborting!" << endl;
      return;
  }
  if(!ndates.is_open())
  {
      cout << "can't find file with test dates ! Aborting!" << endl;
      return;
  }
  if(!odates.is_open())
  {
      cout << "can't find file with reference file dates ! Aborting!" << endl;
      return;
  }
  if(!cputms.is_open())
  {
      cout << "no CPU time data file ! Aborting!" << endl;
      return;
  }
  if(!maxmms.is_open())
  {
      cout << "no max memory usage data file! Aborting!" << endl;
      return;
  }           
  ind=0;
  while(!slnames.eof())
  {
   slnames >> buf;
   if(slnames.eof()) break;
   t_names[ind] = new TString(buf);
   cout << *t_names[ind] << endl;
   ind++;
  }
  ind=0;
  while(!cputms.eof())
  {
   cputms >> sink;
   if(cputms.eof()) break;
   cputms >> sink;
   cputms >> sink;
   cputms >> buf;
   cpu_times[ind] = new TString(buf);
   cputms >> sink;
   ind++;   
  }
  ind=0;
  while(!maxmms.eof())
  {
      maxmms >> sink;
      if(maxmms.eof()) break;
      maxmms >> sink;
      maxmms >> sink;
      maxmms >> buf;
      max_mems[ind] = new TString(buf);
      maxmms >> sink;
      ind++;
  }
  ind=0;                
  for(Int_t i=0; i<12; i++) neolm[i]=false;
  Int_t vind=0;
  cout << "new files: " << endl;        
  if(pri) *output << "Comparing: " << endl;
  if(pri) *outc << "Trigger counts from file: " << endl;
  if(pri) *output << "  Date       File name" << endl;
  if(pri) *outc << "  Date       File name " << endl;
  while(!ndates.eof())
  {
   ndates >> month;
   if(ndates.eof()) break;
   ndates >> dom;
   ndates >> tim;
   ndates >> buf;
   sprintf(dstri,"%s %s %s %s\n",month,dom,tim,buf);
   TString *ns = new TString(dstri);
   new_dates[ind]=ns;
   ind++;   
   if(ind > 11) break;
  }
  ind=0;        
  while(!odates.eof())
  {
   odates >> month;
   if(odates.eof()) break;
   odates >> dom;
   odates >> tim;
   odates >> buf;
   sprintf(dstri,"%s %s %s %s\n",month,dom,tim,buf);
   TString *ns = new TString(dstri);
   old_dates[ind]=ns;
   ind++;   
   if(ind > 11) break;
  }
  ind=0;
  while(!newdfn.eof())
  {
   newdfn >> buf;
   if(newdfn.eof()) break;
   TString *ns = new TString(buf);
   Int_t inon = ns->Index("none");
   if((vind==0) && (inon == -1))
   {
    vind++;       
   }
   new_test_list[ind] = ns;
   if(inon == -1)
   {
    neolm[ind]=true;
    if(pri)
    {
     *output << *new_dates[ind] << endl;
     *outc << *new_dates[ind] << endl;
     *outc << *t_names[ind] << " job CPU time: " << *cpu_times[ind] << " sec." << endl;
     *outc << *t_names[ind] << " job Max Memory: " << *max_mems[ind] << " MB " << endl << endl;
     cout << *new_dates[ind] << endl;
    }
   }   
   ind++;
   if(ind > 11) break;
  }
  newdfn.close();
  if(vind==0)
  {
      cout << "no valid new RTT jobs for this slice found ! Aborting!" << endl;
      *output << "no valid new RTT jobs for this slice found ! Aborting!" << endl;
      *outc << "no valid new RTT jobs for this slice found ! Aborting!" << endl;
      return;
  }        
  Int_t newtn = ind;
  if(pri) *output << "To: Date       File name" << endl;
  if(pri) cout << "old files: " << endl;
  ind=0;    
  while(!olddfn.eof())
  {
   olddfn >> buf;
   if(olddfn.eof()) break;
   TString *ns = new TString(buf);
   if((pri) && neolm[ind]) *output << *old_dates[ind] << endl;
   if((pri) && neolm[ind]) cout << *old_dates[ind] << endl;
   old_test_list[ind] = ns;   
   ind++;
   if(ind > 10) break;
  }
  Int_t oldtn = ind;
  olddfn.close();
  pri = false;   
  cout << endl << *hname << endl << endl;  
  *output << endl << *hname << endl << endl;  
  *outc << endl << *hname << endl << endl;    
  TString **xlabels;  
  Double_t *newbc;
  Double_t *oldbc;
  Double_t *newbct;
  Double_t *oldbct;
  Bool_t *manam;
  Int_t nbins = 0;
  Double_t *nrnnew = new Double_t[12];
  Double_t *nrnold = new Double_t[12];
  for(ind=0; ind<12; ind++)
  {
   nrnnew[ind]=0.;
   nrnold[ind]=0.;    
  }       
  Int_t nexlb=0;
  oin=0;
  vind=0;
  for(ind=0; ind<newtn; ind++)
  {
   cout << "list index is: " << ind << endl;   
   TString *ns = new_test_list[ind];
   Int_t inon = ns->Index("none");
   if(inon != -1)
   {
    cout << "no valid new RTT jobs found for " << *t_names[ind] << endl;     
    *output << endl << "no valid new RTT jobs found for " << *t_names[ind] << endl;
    *outc << endl << "no valid new RTT jobs found for " << *t_names[ind] << endl;
    *output << "----------------------------------------------------------------" << endl;
    *outc << "----------------------------------------------------------------" << endl;
    continue;
   } 
   if(inon == -1)
   {
    cout << "Processing " << *t_names[ind] << endl;     
    TFile *rf = new TFile(*new_test_list[ind]);
    rf->cd(*fname);
    TH1 *tchain;
    gDirectory->GetObject(*hname,tchain);
    if(tchain==0)
    {
     gDirectory->GetObject(*ahname,tchain);
    }
    if(tchain==0)
    {
      cout << "can't find histogram name " << *ahname << "in the root directory " << *fname << endl;      
      continue;
    }  
    nbins = tchain->GetNbinsX()+1;
    if(vind==0)
    {
     newbc = new Double_t[nbins+maxnb];
     oldbc = new Double_t[nbins+maxnb];
     xlabels = new TString[nbins+maxnb];
     manam = new Bool_t[nbins];
     for(Int_t i=0; i<nbins+maxnb; i++)
     {    
      if(i<nbins)
      {
       TString *blab = new TString(tchain->GetXaxis()->GetBinLabel(i));
       if(blab->Length() > 1) blab->Resize(24);     
       xlabels[i] = blab;
       manam[i]=false;
      }
     }
     vind++;    
    }
    for(Int_t i=0; i<nbins+maxnb; i++)
    {    
     newbc[i]=0.;
     oldbc[i]=0.;
    }
    for(Int_t i=0; i<nbins; i++) newbc[i]=tchain->GetBinContent(i);
    TH1 *echist;
    gDirectory->GetObject("NInitialRoIsPerEvent",echist);   
    if(echist==0)
    {    
     if(fname->Contains("_EF"))
     {
      gDirectory->GetObject("N_Initial_RoI_in_Event_EF",echist);   
     }            
     if(fname->Contains("_L2"))
     {
      gDirectory->GetObject("N_Initial_RoI_in_Event_L2",echist);   
     }
    }
    if(echist==0) continue;
    nrnnew[ind]=echist->GetEntries();
    rf->Close();
    TFile *orf = new TFile(*old_test_list[ind]);
    orf->cd(*fname);
    TH1 *otchain;
    gDirectory->GetObject(*hname,otchain);
    if(otchain==0)
    {
     gDirectory->GetObject(*ahname,otchain);   
    }        
    if(otchain==0) continue;
    Int_t onbins = otchain->GetNbinsX()+1;
    if(nbins != onbins)
    {     
     cout << "Different number of bins in histos! Test: " << nbins << " ref: " << onbins << endl;
    }
    nexlb=0;
    for(Int_t i=0; i<onbins; i++)
    {
     Int_t bi = -1;
     TString *obna = new TString(otchain->GetXaxis()->GetBinLabel(i));
     if(obna->Length() > 1) obna->Resize(24);
     for(Int_t si=0; si<nbins+nexlb; si++)
     {
      if(xlabels[si]->CompareTo(*obna) == 0) bi=si;   
     }
     if(bi!=-1)
     {               
      oldbc[bi]=otchain->GetBinContent(i);
      if(bi < nbins) manam[bi]=true;
     }
     if((bi==-1) && (oin==0))
     {     
      *output << "ref chain name " << *obna << "  does not exists in test" << endl;
      xlabels[nbins+nexlb] = obna;
      oldbc[nbins+nexlb]=otchain->GetBinContent(i);
      nexlb++;
      if(nexlb >= maxnb)
      {    
       nexlb = maxnb-1;
       cout << "need more bins for new labels" << endl;
      }
     }
    }
    oin++;  
    TH1 *echist;
    gDirectory->GetObject("NInitialRoIsPerEvent",echist);
    if(echist==0)
    {       
     if(fname->Contains("_EF"))
     {
      gDirectory->GetObject("N_Initial_RoI_in_Event_EF",echist);   
     }            
     if(fname->Contains("_L2"))
     {
      gDirectory->GetObject("N_Initial_RoI_in_Event_L2",echist);   
     }
    }
    if(echist==0) continue;
    nrnold[ind]=echist->GetEntries();
    cout << "Number of events in reference: " << nrnold[ind] << " and in test: " << nrnnew[ind] << endl;               
    orf->Close();                      
    *output << endl;
    *output << setw(24) << "Slice_sample" << setw(18) << "N events in ref" << setw(18) << "N events in test" << endl << endl;         
    *outc << endl;
    *outc << setw(24) << "Slice_sample" << setw(26) << "N events in test" << endl << endl;         
    *outc << setw(24) << *t_names[ind] << setw(16) << nrnnew[ind] << endl << endl;
    cout << setw(24) << *t_names[ind] << "  " << nrnold[ind] << "  " << setw(15) <<  nrnnew[ind] << endl;
    *output <<setw(24) << *t_names[ind] << setw(8) << nrnold[ind] << setw(20) << nrnnew[ind]
            << endl << endl;
    *outc << setw(16) << "Label" << setw(20) << "Numb.passed"  << endl;
    cout << "Number of bins in test histogram: " << nbins << endl;
    for(Int_t i=0; i<nbins; i++)
    {
     TString *label = xlabels[i];
     if(label->Length() > 1)
     {    
      label->Resize(24);
	  *outc << *label << setw(10) << newbc[i] << endl;
     }
    }
    if(abs(nrnnew[ind]-nrnold[ind]) < 2.)
    {       
     Int_t ngood=0;
     Int_t nbad=0;
     for(Int_t i=0; i<nbins; i++)
     {
      Double_t rat = 1.;
      Double_t dif = newbc[i]-oldbc[i];      
      if(oldbc[i] > 0.5)
      {
       rat =  newbc[i]/oldbc[i];      
       if(((rat <= 1.0-toler) || (rat >= 1.0+toler)) && (abs(dif) > 2.)) nbad++;
       else ngood++;
      }    
      else 
      {
	   if((newbc[i]!= 0) && (abs(dif) > 2.)) nbad++;
       else ngood++;
      }
     }
     if(nbad !=0)
     {    
//      cout << "Label" << setw(24) << 
//         "Ref n" << setw(10) << "Test" << setw(15) << "test/ref" << endl;  
      *output << setw(16) << "Label" << setw(18) << 
          "Ref n" << setw(10) << "Test" << setw(15) << "test/ref" << endl;
     }  
     for(Int_t i=1; i<nbins+nexlb; i++)
     {
      TString *label = xlabels[i];
      if(label->Length() > 1) label->Resize(24);
      Double_t rat = 1.;
      Double_t dif = 0.;
      if(i<nbins)
      {
       dif = newbc[i]-oldbc[i];      
       if(oldbc[i] > 0.5)
       {       
        rat =  newbc[i]/oldbc[i];
        if(((rat <= 1.0-toler) || (rat >= 1.0+toler)) && (abs(dif) > 2.) && (label->Length() > 1)) 
        {
//	     cout << *label<< setw(10) << oldbc[i] << setw(10) << newbc[i] << setw(15) << rat << endl;
	     *output << *label<< setw(10) << oldbc[i] << setw(10) << newbc[i] << setw(15) << rat << endl;
	    }
       } 
       else 
       {
        if((newbc[i]!= 0) && manam[i] && (abs(dif)>2.) && (label->Length() > 1)) 
        {
//         cout << *label<< setw(10) << oldbc[i] << setw(10) << newbc[i]  << endl;
         *output << *label<< setw(10) << oldbc[i] << setw(10) << newbc[i]  << endl;
	    }
        if(!manam[i] && (label->Length() > 1))
        {
//         cout << *label << setw(10) << "no_ref" << setw(10) << newbc[i]  << endl;  
	     *output << *label<< setw(10) << "no_ref" << setw(10) << newbc[i]  << endl;
        }        
       }
      }
      else   
      {
       if((label->Length() > 1))
	   *output << *label<< setw(10) << oldbc[i] << setw(10) << "no data"  << endl;
      }       
     } 
     if(toler > 0 )   cout << "Fraction inside tolerance: " 
       << " "  <<  ngood << "/" 
       <<  ngood+nbad << endl;
     cout << "----------------------------------------------------------------" << endl;
     if(toler > 0 )   *output << "Fraction inside tolerance: "  << " "  <<  ngood << "/" 
       <<  ngood+nbad << endl;
     *output << "----------------------------------------------------------------" << endl;
    }
   }
  }
}

void TrigTest_Compare(Float_t toler=0.0,Int_t reln){

  cout << endl;
  cout << "**************************************************************************"  <<endl;
  cout << "Trigger differnece that exceeed a tolerance of " << toler << " are printed"  << endl;
  cout << "Comparing only matching datasets and only datasets with" << endl; 
  cout << "the same number(+_2) of processed events will be compared" << endl;
  cout << "**************************************************************************" <<endl; 
  cout << endl;
  
  TTimeStamp *ts = new TTimeStamp();

  char *ofnam = new char[100];
  sprintf(ofnam,"%d-rel_%d.txt",ts->GetDate(),reln);
  ofstream *output = new ofstream(ofnam);
  sprintf(ofnam,"%d-rel_%d_cnts.txt",ts->GetDate(),reln);
  ofstream *output2 = new ofstream(ofnam);  
  ts->Print();
  *output << endl;
  *output << "**************************************************************************"  <<endl;
  *output << "Trigger differnece that exceeed a tolerance of " << toler << " are printed"  << endl;
  *output << "Comparing only matching datasets and only datasets with" << endl; 
  *output << "the same number(+_2) of processed events will be compared" << endl;
  *output << "**************************************************************************" <<endl; 
  *output << endl;
  *output << endl;
  *output2 << "**************************************************************************"  <<endl;
  *output2 << "Trigger counts for all chains processed in this slice are printed"  << endl;
  *output2 << "**************************************************************************" <<endl; 
  *output2 << endl;
  TString *hnm = new TString("ChainAcceptance");
  TString *fnm = new TString("TrigSteer_L2");
  TString *ahnm = new TString("ChainAcceptance_runsummary");
  diff(fnm,hnm,ahnm,toler,output,output2);
  fnm = new TString("TrigSteer_EF");
  hnm = new TString("ChainAcceptance_EF");
  diff(fnm,hnm,ahnm,toler,output,output2);
  output->close();
  output2->close();
}

