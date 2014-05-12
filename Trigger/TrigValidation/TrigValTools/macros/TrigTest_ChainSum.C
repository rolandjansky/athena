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
Int_t psn = 0;

void  diff(TString *fname, TString *hname, TString *ahname, Float_t toler, ofstream *output, ofstream *output2, Int_t setn)
{
  cout << endl << *hname << endl << endl;  
  *output << endl << *hname << endl << endl;  
  *output2 << endl << *hname << endl << endl;  
  ifstream newdfn;
  ifstream olddfn;
  ifstream ndates;
  ifstream odates;
  Int_t maxnl = 400;
  newdfn.open("new_test_list.txt");
  olddfn.open("old_test_list.txt");
  ndates.open("new_dates.txt");
  odates.open("old_dates.txt");
  TString **new_test_list = new TString[12];
  TString **old_test_list = new TString[12];
  TString **t_names = new TString[12]; 
  TString **new_dates = new TString[12];
  TString **old_dates = new TString[12]; 
  char *ltrs = new char[12];
  char *buf = new char[200];
  char *month = new char[200];
  char *dom = new char[200];
  char *tim = new char[200];
  char *dstri = new char[300];
  Int_t ind = 0;
  Int_t nfilesn = 0;
  Int_t nfileso = 0;
  pri=true;
  if(setn==psn) pri=false;
  if(!newdfn.is_open())
  {
      cout << "no valid new RTT jobs on enhanced bias data found ! Aborting!" << endl;
      *output << "no valid new RTT jobs on enhanced bias data found ! Aborting!" << endl;
      return;
  }        
  if(!olddfn.is_open())
  {
      cout << "no valid reference files found ! Aborting!" << endl;
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
  nfilesn=ind;
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
  nfileso=ind;
  ind=0;
  while(!newdfn.eof())
  {
   newdfn >> buf;
   if(newdfn.eof()) break;
   TString *ns = new TString(buf);
   new_test_list[ind] = ns;   
   ind++;
   if(ind > 11) break;
  }
  newdfn.close();
  Int_t newtn = ind;
  ind=0;    
  while(!olddfn.eof())
  {
   olddfn >> buf;
   if(olddfn.eof()) break;
   TString *ns = new TString(buf);
   Int_t ious=ns->Index("as_");
   if((ious > -1) && (ious < ns->Length()-3))
   {    
    ious+=3;
    ltrs[ind] = buf[ious];
   }
   else
   {
    ious = ns->Index("_top_");        
    if((ious > -1) && (ious < ns->Length()-3)) ltrs[ind]='T';
    else ltrs[ind]='N';
   }   
   printf("%c\n",ltrs[ind]);
   old_test_list[ind] = ns;   
   ind++;
   if(ind > 11) break;
  }
  Int_t oldtn = ind;
  olddfn.close();
  
  TString **xlabelst;
  TString **xlabelse;  
  Double_t *newbce;
  Double_t *oldbce;
  Double_t *newbct;
  Double_t *oldbct;
  Bool_t *manamt;
  Bool_t *maname;
  Int_t nbinst = 0;
  Double_t *nrnnew = new Double_t[12];
  Double_t *nrnold = new Double_t[12];
  Int_t nbinse = 0;
  Bool_t notine = true;
  for(ind=0; ind<12; ind++)
  {
   nrnnew[ind]=0.;
   nrnold[ind]=0.;    
  }
  Int_t nbinsc=0;       
  for(ind=0; ind<newtn; ind++)
  {
   TFile *rf = new TFile(*new_test_list[ind]);
   if((rf == 0)&& (ind!= 0)) break;
   if((rf == 0) && (ind==0)) return;
   rf->cd(*fname);
   TH1 *tchain;
   gDirectory->GetObject(*hname,tchain);
   if(tchain==0)
   {
    gDirectory->GetObject(*ahname,tchain);
   }        
   if(tchain==0)
   {
    cout << "Could not find chainsum histo in test file" << endl; 
    continue;
   }
   if(ltrs[ind]=='T')
   {
    nbinst = tchain->GetNbinsX()+1;
    newbct = new Double_t[nbinst+maxnl];
    xlabelst = new TString[nbinst+maxnl];
    manamt = new Bool_t[nbinst];
    for(Int_t i=0; i<nbinst+maxnl; i++)
    {    
     newbct[i]=0.;
     if(i<nbinst)
     {
      TString *blab = new TString(tchain->GetXaxis()->GetBinLabel(i));
      blab->Resize(24);              
      xlabelst[i] = blab;
      manamt[i]=false;
     }
    }
   }
   nbinsc = tchain->GetNbinsX()+1;
   if((ltrs[ind] != 'T') && (nbinsc > nbinse))
   {
    nbinse=nbinsc;
    newbce = new Double_t[nbinse+maxnl];
    xlabelse = new TString[nbinse+maxnl];
    maname = new Bool_t[nbinse];
    for(Int_t i=0; i<nbinse+maxnl; i++)
    {    
     newbce[i]=0.;
     if(i<nbinse)
     {
      TString *blab = new TString(tchain->GetXaxis()->GetBinLabel(i));
      blab->Resize(24);              
      xlabelse[i] = blab;
      maname[i]=false;
     }
    }           
   }
   TH1 *echist;
   gDirectory->GetObject("NInitialRoIsPerEvent",echist);   
   if(echist==0)
   {
    cout << "Could not find NInitialRoIsPerEvent hist in test file" << endl; 
    if(fname->Contains("_EF"))
    {
     gDirectory->GetObject("N_Initial_RoI_in_Event_EF",echist);   
    }            
    if(fname->Contains("_L2"))
    {
     gDirectory->GetObject("N_Initial_RoI_in_Event_L2",echist);   
    }       
   }        
   if(echist==0)
   {    
    cout << "Could not find N_Initial_RoI_in_Event...hist in test file" << endl; 
    continue;
   }
   nrnnew[ind]=echist->GetEntries();            
   rf->Close();       
  }        
  for(ind=0; ind<oldtn; ind++)
  {
   TFile *rf = new TFile(*old_test_list[ind]);
   rf->cd(*fname);
   TH1 *tchain;
   gDirectory->GetObject(*hname,tchain);
   if(tchain==0)
   {
    gDirectory->GetObject(*ahname,tchain);
   }              
   if(tchain==0)
   {
    cout << "Could not find chainsum histo in reference file" << endl; 
    continue;
   }
   if(ltrs[ind]=='T')
   {
    oldbct = new Double_t[nbinst+maxnl];
    for(Int_t i=0; i<nbinst+maxnl; i++)
    {    
     oldbct[i]=0.;
    }    
   }
   if((ltrs[ind]!='T') && notine)
   {
    notine=false;   
    oldbce = new Double_t[nbinse+maxnl];
    for(Int_t i=0; i<nbinse+maxnl; i++)
    {    
     oldbce[i]=0.;
    }    
   }
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
   rf->Close();       
  }
  if(pri)
  {    
   *output << "Dates and file names of test outputs: " << endl;
   *output2 << "Dates and file names of test outputs: " << endl;
   for(Int_t i=0; i<nfilesn; i++)
   {
    if(((ltrs[i]=='T') && (setn==1)) || ((ltrs[i]!='T') && (setn==2)))
    {           
     *output << *new_dates[i] << endl;
     *output2 << *new_dates[i] << endl;
    }
   }     
   *output << endl << "Dates and file names of reference files: " << endl;
   for(Int_t i=0; i<nfileso; i++)
   {    
    if(((ltrs[i]=='T') && (setn==1)) || ((ltrs[i]!='T') && (setn==2)))
    {           
     *output << *old_dates[i] << endl;
    }
   }
  }     
  if(pri)
  {    
   cout << endl;
   cout << "Ref nevnts" << setw(15) << "Test nevnts" << endl;
   *output << "-------  Successfully finished jobs ---------   " << endl;
   *output << endl;
   *output << "Job name" << setw(26) << "N events in ref" << setw(20) << "N events in test" << endl << endl;
  }         
  for(ind=0; ind<newtn; ind++)
  {
   char *jn = new char[24];
   if((ltrs[ind] != 'T') && (ltrs[ind] != 'N'))
    sprintf(jn,"Enhanced bias %c",ltrs[ind]);
   else
   {
    if((ltrs[ind] == 'T') || ((nrnold[ind] < 1500) && (nrnnew[ind] < 1500)))
    {       
     sprintf(jn,"top test-10**31 menu");
     ltrs[ind]='T';
    }
    else sprintf(jn,"matching dataset %d",ind);
   }
   TString *jns = new TString(jn);
   jns->Resize(22);       
   if(pri && (abs(nrnnew[ind]-nrnold[ind]) < 3.))
   {       
    cout << setw(5) << nrnold[ind] << setw(15) <<  nrnnew[ind] 
            << setw(10) << "+" << endl;
    if((ltrs[ind] != 'T')&&(setn==2)) *output << *jns << setw(8) << nrnold[ind] << setw(20) << nrnnew[ind]
            << setw(10) << "+" << endl;
    if((ltrs[ind]=='T') && (setn==1)) *output << *jns << setw(8) << nrnold[ind] << setw(20) << nrnnew[ind]
            << setw(10) << "*" << endl << endl;        
   }
   else
   {    
    if(pri)
    {       
     cout << setw(5) << nrnold[ind] << setw(15) <<  nrnnew[ind] << endl;
     if((setn==2)&&(ltrs[ind]!='T'))  *output << setw(15) << *jns << setw(8) << nrnold[ind] << setw(20) << nrnnew[ind] << endl;
     if((ltrs[ind] == 'T')&&(setn==1)) *output << setw(15) << *jns << setw(8) << nrnold[ind] << setw(20) << nrnnew[ind] << endl;
    }
   }
  }
  if(pri)
  {
   *output << endl;
   *output2 << endl;   
  }       
  Int_t ncoms = 0;
  Int_t ncomst = 0;
  psn=setn;
  for(ind=0; ind<newtn; ind++)
  {
   if(abs(nrnnew[ind]-nrnold[ind]) < 3.)
   {    
    if(ltrs[ind] != 'T') ncoms+=nrnnew[ind];
    else ncomst+=nrnnew[ind];          
    TFile *rf = new TFile(*new_test_list[ind]);
    rf->cd(*fname);
    TH1 *tchain;
    gDirectory->GetObject(*hname,tchain);
    if(tchain==0)
    {
     gDirectory->GetObject(*ahname,tchain);
    }
    if(tchain==0) continue;              
    if(ltrs[ind] == 'T')
    {    
     for(Int_t i=0; i<nbinst; i++)
     {
      newbct[i]=tchain->GetBinContent(i);
     }
    }
    else
    {    
     for(Int_t i=0; i<nbinse; i++)
     {             
      newbce[i]+= tchain->GetBinContent(i); 
     }
    }
   } 
   rf->Close();       
  }
  Int_t nexlbt = 0;
  Int_t nexlbe = 0;
  for(ind=0; ind<oldtn; ind++)
  {
   if(abs(nrnnew[ind]-nrnold[ind]) < 3.)
   {       
    TFile *rf = new TFile(*old_test_list[ind]);
    rf->cd(*fname);
    TH1 *tchain;
    gDirectory->GetObject(*hname,tchain);
    if(tchain==0)
    {
     gDirectory->GetObject(*ahname,tchain);
    }        
    nbinsc = tchain->GetNbinsX()+1;
    if(ltrs[ind]=='T')
    {    
     for(Int_t i=0; i<nbinsc; i++)
     {
      Int_t bi = -1;
      TString *obna = new TString(tchain->GetXaxis()->GetBinLabel(i));
      if(obna->Length() < 2) continue;
      obna->Resize(24);
      for(Int_t si=0; si<nbinst+nexlbt; si++)
      {
       if(xlabelst[si]->CompareTo(*obna) == 0) 
       { 
        bi=si;
        break;
       }   
      }
      if(bi!=-1)
      {               
       oldbct[bi]=tchain->GetBinContent(i);
       if(bi < nbinst) manamt[bi]=true;
      }
      if(bi==-1)
      {     
       *output << "ref chain name " << *obna << "  does not exists in test" << endl;
       xlabelst[nbinst+nexlbt] = obna;
       oldbct[nbinst+nexlbt] = tchain->GetBinContent(i);
       nexlbt++;
       if(nexlbt > maxnl-1)
       {    
        nexlbt = maxnl-1;
        cout << "need more bins for new labels" << endl;
       }
      }  
     }
    }
    else
    {    
     for(Int_t i=0; i<nbinse; i++)
     {
      Int_t bi = -1;
      TString *obna = new TString(tchain->GetXaxis()->GetBinLabel(i));
      if(obna->Length() < 2) continue;
      obna->Resize(24);
      for(Int_t si=0; si<nbinse+nexlbe; si++)
      {
       if(xlabelse[si]->CompareTo(*obna) == 0) 
       { 
        bi=si;
        break;
       }   
      }
      if(bi!=-1)
      {               
       oldbce[bi]+=tchain->GetBinContent(i);
       if(bi < nbinse) maname[bi]=true;
      }
      if(bi==-1)
      {     
       *output << "ref chain name " << *obna << "  does not exists in test" << endl;
       xlabelse[nbinse+nexlbe] = obna;
       oldbce[nbinse+nexlbe]+= tchain->GetBinContent(i);
       nexlbe++;
       if(nexlbe > maxnl-1)
       {    
        nexlbe = maxnl-1;
        cout << "need more bins for new labels" << endl;
       }
      }   
     }
    } 
    rf->Close();           
   }
  }
  Int_t ngood =0;
  Int_t nbad=0;
  if((ncomst == 0) && (setn==1))
  {
   if(pri)
   {    
     *output << " Top test job was not succesfull!" << endl;
     *output2 << " Top test job was not succesfull!" << endl;
   }    
  }
  if((ncomst!=0) && (setn==1))
  {
   cout << endl;       
   *output << endl;
   if(pri)
   {           
    *output << " Comparing outputs of top test - 10**31 menu jobs" << endl << endl;   
    *output2 << " Trigger counts for top test - 10**31 menu job" << endl << endl;   
    *output2 << " Number of events in test sample: " << ncomst << endl << endl;
   }   
   for(Int_t i=0; i<nbinst; i++)
   {
    Double_t dif = abs(newbct[i]-oldbct[i]);
    Double_t rat = 1.;
    if(oldbct[i] > 0.5)
    {
     rat =  newbct[i]/oldbct[i];      
     if(((rat <= 1.0-toler) || (rat >= 1.0+toler)) && (dif > 2.)) nbad++;
     else ngood++;
    } 
    else 
    {
	 if(newbct[i] > 2.) nbad++;
     else ngood++;
    }
   }
   if(nbad !=0)
   {    
    *output << "Label" << setw(30) << 
          "Ref n" << setw(10) << "Test" << setw(15) << "test/ref" << endl;
   }  
   for(Int_t i=0; i<nbinst+nexlbt; i++)
   {
    TString *label = xlabelst[i];
    label->Resize(24);
    Double_t rat = 1.;
    Double_t dif = abs(newbct[i]-oldbct[i]);
    if(i < nbinst)
    {
	 *output2 << *label<< setw(10) << newbct[i] << endl;   
     if(oldbct[i] > 0.5)
     {       
      rat =  newbct[i]/oldbct[i];      
      if(((rat <= 1.0-toler) || (rat >= 1.0+toler)) && (dif > 2.)) 
      {
	   *output << *label<< setw(10) << oldbct[i] << setw(10) << newbct[i] << setw(15) << rat << endl;
	  }
     }
     else 
     {
	  if((newbct[i] > 2.) && manamt[i]) 
      {
	   *output << *label<< setw(10) << oldbct[i] << setw(10) << newbct[i]  << endl;
	  }
      if(!manamt[i])
      {
	   *output << *label<< setw(10) << "no_ref" << setw(10) << newbct[i]  << endl;
      }        
     }     
    } 
    else
    {
	  *output << *label<< setw(10) << oldbct[i] << setw(10) << "no data"  << endl;
    }       
   }
   if(toler > 0 )   cout << "Fraction inside tolerance: " 
       << " "  <<  ngood << "/" 
       <<  ngood+nbad << endl;
   cout << "----------------------------------------------------------------" << endl;
   if(toler > 0 )   *output << "Fraction inside tolerance: " 
        << " "  <<  ngood << "/" 
        <<  ngood+nbad << endl;
   *output << "----------------------------------------------------------------" << endl;
   *output2 << "----------------------------------------------------------------" << endl;
  }               
  if((ncoms == 0) && (setn==2))
  {
   if(pri)
   {       
    cout << "No matching jobs to compare !" << endl;
    *output << "No matching jobs to compare in enhanced bias sample!" << endl;
    *output << "----------------------------------------------------------------" << endl;
    *output2 << "No matching jobs in enchanced bias sample !" << endl;
    *output2 << "----------------------------------------------------------------" << endl;
   }
   return;
  }
  if(setn==2)
  {
   if(pri)
   {           
    *output << " Comparing sum of all enhanced bias jobs" << endl << endl;   
    *output2 << " Trigger count sums of all enhanced bias jobs" << endl << endl;
    *output2 << " Number of events in test sample: " << ncoms << endl << endl;
   }   
   ngood=0;
   nbad=0;
   for(Int_t i=0; i<nbinse; i++)
   {
    Double_t dif = abs(newbce[i]-oldbce[i]);
    Double_t rat = 1.;
    if(oldbce[i] > 0.5)
    {
     rat =  newbce[i]/oldbce[i];      
     if(((rat <= 1.0-toler) || (rat >= 1.0+toler)) && (dif>2.)) nbad++;
     else ngood++;
    }    
    else 
    {
	 if(newbce[i] > 2.) nbad++;
     else ngood++;
    }
   }
   if(nbad !=0)
   {    
    *output << "Label" << setw(30) << 
          "Ref n" << setw(10) << "Test" << setw(15) << "test/ref" << endl;
   }  
   for(Int_t i=0; i<nbinse+nexlbe; i++)
   {
    TString *label = xlabelse[i];
    label->Resize(24);
    Double_t rat = 1.;
    Double_t dif = 0.;
    if(i<nbinse)
    {
	 *output2 << *label<< setw(10) << newbce[i] << endl;
     dif = abs(newbce[i]-oldbce[i]);   
     if(oldbce[i] > 0.5)
     {       
      rat =  newbce[i]/oldbce[i];      
      if(((rat <= 1.0-toler) || (rat >= 1.0+toler)) && (dif > 2.)) 
      {
	   *output << *label<< setw(10) << oldbce[i] << setw(10) << newbce[i] << setw(15) << rat << endl;
	  }
     } 
     else 
     {
      if((newbce[i] > 2.) && maname[i]) 
      {
       *output << *label<< setw(10) << oldbce[i] << setw(10) << newbce[i]  << endl;
	  }
      if(!maname[i])
      {
	   *output << *label<< setw(10) << "no_ref" << setw(10) << newbce[i]  << endl;
      }        
     }
    }
    else   
    {
	 *output << *label<< setw(10) << oldbce[i] << setw(10) << "no data"  << endl;
    }       
   } 
   if(toler > 0 )   cout << "Fraction inside tolerance: " 
       << " "  <<  ngood << "/" 
       <<  ngood+nbad << endl;
   cout << "----------------------------------------------------------------" << endl;
   if(toler > 0 )   *output << "Fraction inside tolerance: " 
       << " "  <<  ngood << "/" 
       <<  ngood+nbad << endl;
   *output << "----------------------------------------------------------------" << endl;
  }
  pri = false;
}

void TrigTest_ChainSum(Float_t toler=0.0,Int_t reln){

  
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
  *output2 << "**************************************************************************"  <<endl;
  *output2 << "Trigger counts for trigger chains "  << endl;
  *output2 << "**************************************************************************"  <<endl;
  TString *hnm = new TString("ChainAcceptance");
  TString *fnm = new TString("TrigSteer_L2");
  TString *ahnm = new TString("ChainAcceptance_runsummary");
  diff(fnm,hnm,ahnm,toler,output,output2,1);
  fnm = new TString("TrigSteer_EF");
  hnm = new TString("ChainAcceptance");
  diff(fnm,hnm,ahnm,toler,output,output2,1);
  *hnm = new TString("ChainAcceptance");
  *fnm = new TString("TrigSteer_L2");
  diff(fnm,hnm,ahnm,toler,output,output2,2);
  fnm = new TString("TrigSteer_EF");
  hnm = new TString("ChainAcceptance");
  diff(fnm,hnm,ahnm,toler,output,output2,2);
  output->close();
  output2->close();
}

