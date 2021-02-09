/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
//  Get average HEC energies and sampling fractions
//  as functions of Scan parameter, namely:  Pseudorapidity
//
//  To execute:
//  .L samfr_etaX.C+
//  samfr_etaX()
//
#include <iomanip>
#include <iostream>
#include <fstream>

#include "TBranch.h"
#include "TChain.h"
#include "TMath.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include "substa.C"

void samfr_etaX (
     Int_t   iHEC  = 0,   //  HEC wheel (0 - front, 1 - rear)
     Int_t   iPart = 0,   //  primary particle
     Int_t   IEner = 100, //  particle energy [GeV]
     Char_t const *G4Ver = "10.1",          //  Geant4 version
     Char_t const *PhLis = "FTFP_BERT_ATL", //  Physics list
     Char_t const *Dir   = "/eos/atlas/atlascerngroupdisk/proj-simul/G4Run3/SamplingFractions/HEC")
{
//  
/* ------------------------------------------------------------------ */
//
//
   const Int_t NHEC = 2; //  HEC wheels
   Float_t ZHEC[NHEC] = { 431.95, 517.50 };
   Char_t const *THEC[NHEC] = { "HEC front wheel", "HEC rear wheel" };
   Char_t const *CHEC[NHEC] = { "fwh", "rwh" };
   Char_t const *SHEC[NHEC] = { "sc1", "sc2" };
//
   const Int_t NPart = 4;
   Int_t PPart[NPart] = { 0,  11, -11, 22 };
   Char_t const *TPart[NPart] = { 
      "EM-particle", "Electron", "Positron", "Gamma" };
   Char_t const *CPart[NPart] = { "EM", "ele", "pos", "gam" };   
//
//
   const Int_t NEHit = 6;
   Char_t const *TEHit[NEHit] = { "Total energy", "Visible energy",
      "EM energy", "Non-EM energy", "Invisible energy", "Escaped energy" };
   Double_t EHit[NEHit];
//
   const Int_t NMat = 2;
   Char_t const *TMat[NMat] = { "Active material", "Passive material" };
//
//
   const Int_t NVar = 5;
   Char_t const *TVar[NVar] = { 
      "Visible energy in active material", 
      "Visible energy in passive material",
      "Total visible energy",
      "Ratio of energies: Active-to-Total",
      "Sampling fraction" };
   Char_t const *CVar[NVar] = { "eact", "epas", "etot", "rate", "samf" };
   Double_t AVE[NVar], ERR[NVar];
   Double_t *Var[NVar], *eVar[NVar];
//
   const Int_t NVar1 = 4; //  NVar-1;
   Double_t **Escan[NVar1];   
//    
//  --------------------------------------------------------------------
//
//
   TChain *chain;
//
   Int_t *NevtSc;
   Int_t *NevtZE;
//   
//
   Int_t    ia,ie,ih,im,in,ip,is,iv,iw;
   Int_t    nentries, nvtx,npart, Nhits, NevtOut, n1,n2, Typ;
   UInt_t   Det,Sam,Sid;
   Double_t DScan, Sc;
   Double_t diff,diffa,diffr, esum, rms, rat,ert, d1,d2;
   Char_t   JOB1[111],JOB2[111], job[111];
   Char_t   rootfile[222];
   Char_t   datdir[222], datfil[222];
   Char_t   comm[222];
//
   FILE *fout;
//     
   //  Pseudorapidity -scan
   Int_t    Nscan = 36;
   Double_t Scan1 = 1.50; 
   Double_t Scan2 = 3.30;
// 
   Int_t NSamp = 25; //  number of root-files with DumpHits
//
   Int_t IVar = 4;
//
   Double_t dCut = 1.e-5;
   Double_t MeV2GeV = 0.001;
//
/* ------------------------------------------------------------------ */
//
//  Argument(s)
//
   if (iHEC<0 || iHEC>=NHEC) {
      cerr << " *** Bad argument iHEC = " << iHEC << " !!! *** " << endl;
      exit (-2);
   }
//
   if (iPart<0 || iPart>=NPart) {
      cerr << " *** Bad argument iPart = " << iPart << " !!! *** " << endl;
      exit (-2);
   }
//
//
   sprintf (JOB1, "%s-%s", G4Ver,PhLis); 
   sprintf (JOB2, "%s_e%04d_%s", CPart[iPart],IEner,SHEC[iHEC]); 
//
   cout << endl;
   cout << " ===  " << JOB1 << " " << JOB2;
   cout << ":  Pseudorapidity scan over " << THEC[iHEC] << "  === " << endl;     
//
/* ------------------------------------------------------------------ */
//      
//  Create chain from ROOT files
//
   chain = new TChain ("mytree");
//
//  Electrons + Positrons + Gammas
//   
   if (iPart == 0) {
      for (ip=1; ip<=3; ip++) {
         sprintf (rootfile, "%s/%s/mc.PG_pid%d_Mom100000_HEC%s.NTUP.0.pool.root", Dir, JOB1, PPart[ip], CHEC[iHEC]);
         cout << " ===  " << rootfile << "  === " << endl;  
         chain->Add(rootfile);
      }
   }
//
//  Single particle
//
   else {
      sprintf (rootfile, "%s/%s/mc.PG_pid%d_Mom100000_HEC%s.NTUP.0.pool.root", Dir, JOB1, PPart[iPart], CHEC[iHEC]);
      cout << " ===  " << rootfile << "  === " << endl;  
      chain->Add(rootfile);
   }
//
//
   nentries = chain->GetEntries();
   cout << " ===  Number of entries - " << nentries << "  === " << endl;
   if (nentries < 100) {
      cerr << " *** Two few nentries: " << nentries << " !!! *** "  << endl;
      exit (-2);
   }
//
  TTreeReader reader(chain);
  TTreeReaderValue< int > mc_pdg(reader, "mc_pdg");
  TTreeReaderValue< double > mc_e(reader, "mc_e");
  TTreeReaderValue< double > mc_pt(reader, "mc_pt");
  TTreeReaderValue< double > mc_eta(reader, "mc_eta");
  TTreeReaderValue< double > mc_phi(reader, "mc_phi");
  TTreeReaderValue< std::vector<float> > energy_inactive_total(reader, "energy_inactive_total");
  TTreeReaderValue< std::vector<float> > energy_inactive_em(reader, "energy_inactive_em");
  TTreeReaderValue< std::vector<float> > energy_inactive_nonem(reader, "energy_inactive_nonem");
  TTreeReaderValue< std::vector<float> > energy_inactive_inv(reader, "energy_inactive_inv");
  TTreeReaderValue< std::vector<float> > energy_inactive_esc(reader, "energy_inactive_esc");
  TTreeReaderValue< std::vector<float> > energy_active_total(reader, "energy_active_total");
  TTreeReaderValue< std::vector<float> > energy_active_em(reader, "energy_active_em");
  TTreeReaderValue< std::vector<float> > energy_active_nonem(reader, "energy_active_nonem");
  TTreeReaderValue< std::vector<float> > energy_active_inv(reader, "energy_active_inv");
  TTreeReaderValue< std::vector<float> > energy_active_esc(reader, "energy_active_esc");
  cout<<"Start with 1st event,"<<" index="<<reader.GetCurrentEntry()<<endl;
  reader.SetEntry(0);
  cout<<"Switched to 1st event,"<<" index="<<reader.GetCurrentEntry()<<endl;

//
//  Initialisation
//
   NevtOut = 0; //  number of events with Scan parameter out of boundaries
//
   DScan = (Scan2-Scan1) / double( Nscan );
//      
   NevtSc = new Int_t [Nscan]; //  number of stored events 
   NevtZE = new Int_t [Nscan]; //  number of events with zero energy
//
   for (is=0; is<Nscan; is++) {
      NevtSc[is] = 0;
      NevtZE[is] = 0;
   }   
//   
   for (iv=0; iv<NVar1; iv++) {
      Escan[iv] = new Double_t * [Nscan];
      for (is=0; is<Nscan; is++) {
   	    Escan[iv][is] = new Double_t [nentries];
      	for (in=0; in<nentries; in++) Escan[iv][is][in] = 0;
      }
   }
//
/* ------------------------------------------------------------------ */
//
//  Loop over events
//
   for (in=0; in<nentries; in++) {
      reader.SetEntry(in);
      if(in%1000==0) cout<<"entry="<<in<<" tree="<<chain->GetTreeNumber()<<" index="<<reader.GetCurrentEntry()<<endl;
//
      Sc = *mc_eta;
//     
      is = int( (Sc-Scan1) / DScan );
//
      if (is < 0 || is >= Nscan) {
         NevtOut ++;
         continue;
      }
//
//  --------------------------------------------------------------------
//
//  Check active LAr hits
//      
      iv = im = 0;
//
      for(Sam=8;Sam<=11;++Sam) {
        iw = int( Sam/2 ) - 4;
	      if (iw < 0 || iw >= NHEC) {
	        cerr << " *** Bad HEC wheel = " << iw;
	        cerr << " for Sam=" << Sam;
	        cerr << " in " << TMat[im];
	        cerr << " !!! *** " << endl;
          exit (-2);
        }
//
        EHit[0] = double( (*energy_active_total)[Sam] );
        EHit[2] = double( (*energy_active_em)[Sam] );
        EHit[3] = double( (*energy_active_nonem)[Sam] );
        EHit[4] = double( (*energy_active_inv)[Sam] );
        EHit[5] = double( (*energy_active_esc)[Sam] );
        EHit[1] = EHit[2]+EHit[3];
        if (iw == iHEC) Escan[iv][is][ NevtSc[is] ] += EHit[1];
      }
//
//  --------------------------------------------------------------------
//
//  Check inactive (i.e. passive) LAr hits
//      
      iv = im = 1;
//
      for(Sam=8;Sam<=11;++Sam) {
        iw = int( Sam/2 ) - 4;
	      if (iw < 0 || iw >= NHEC) {
	        cerr << " *** Bad HEC wheel = " << iw;
	        cerr << " for Sam=" << Sam;
	        cerr << " in " << TMat[im];
	        cerr << " !!! *** " << endl;
          exit (-2);
        }
//
        EHit[0] = double( (*energy_inactive_total)[Sam] );
        EHit[2] = double( (*energy_inactive_em)[Sam] );
        EHit[3] = double( (*energy_inactive_nonem)[Sam] );
        EHit[4] = double( (*energy_inactive_inv)[Sam] );
        EHit[5] = double( (*energy_inactive_esc)[Sam] );
        EHit[1] = EHit[2]+EHit[3];
        if (iw == iHEC) Escan[iv][is][ NevtSc[is] ] += EHit[1];
      }
//
//  --------------------------------------------------------------------
//
//  Skip event with zero total energy
//
      esum = Escan[0][is][ NevtSc[is] ] + 
      	     Escan[1][is][ NevtSc[is] ];
//
      if (esum == 0) {
         Escan[0][is][ NevtSc[is] ] = 0;
         Escan[1][is][ NevtSc[is] ] = 0;
         NevtZE[is] ++;
      }
      else
         NevtSc[is] ++;
//
   } //  over events
//
   if (NevtOut > 0) {
      cout << " xxx  " << NevtOut << " event(s)";
      cout << " out of scan boundaries  xxx " << endl;
   }
//
//
   chain->Reset();   
//
//  --------------------------------------------------------------------
//
//  Get total visible energy and ratio of energies event-by-event
//
   n1 = n2 = 0;
//   
   for (is=0; is<Nscan; is++) {
      n1 += NevtSc[is];
      n2 += NevtZE[is];
//
      if (NevtZE[is] != 0) {
         Sc = Scan1 + 0.5*DScan + is*DScan;
         cout << " xxx  Non-zero NevtZE = " << NevtZE[is];
         cout << " for is/Sc = " << is << "/" << Sc;
         cout << " (NevtSc=" << NevtSc[is] << ")  xxx " << endl;
      }      
//      
      if (NevtSc[is] < 1) continue;
// 	   
      for (in=0; in<NevtSc[is]; in++) {
         iv = 2;
         Escan[iv][is][in] = Escan[0][is][in] + Escan[1][is][in];
//
       	 if (Escan[iv][is][in] == 0) {
       	    cerr << " *** Bad total visible energy";
       	    cerr << " = " << Escan[iv][is][in];
       	    cerr << " for is=" << is;
       	    cerr << " in non-empty event nr.=" << in << " !!! *** " << endl;
       	    exit (-2);
       	 }
//
       	 iv = 3;
       	 Escan[iv][is][in] = Escan[0][is][in] / 
       			     Escan[2][is][in];
      }
   } //  over is      
//
//
   if (n1+n2+NevtOut != nentries) {
      cerr << " *** Bad number of events n1/n2/NevtOut/nentries = ";
      cerr << n1 << "/" << n2 << "/" << NevtOut << "/";
      cerr << nentries << " !!! *** " << endl;
      exit (-2);
   }
//
/* ------------------------------------------------------------------ */
//
//  Get Mean and Error of stored variables
//
   for (iv=0; iv<NVar; iv++) {
       Var[iv] = new Double_t [Nscan];
      eVar[iv] = new Double_t [Nscan]; 
//
      for (is=0; is<Nscan; is++) {
     	  Var[iv][is] = 0;
     	 eVar[iv][is] = 0;
      }           
   }
//
//   
   for (is=0; is<Nscan; is++) {
      if (NevtSc[is] < 1) continue;
//
      for (iv=0; iv<NVar; iv++) {
   	    if (iv < NVar1) {   
   	      substa (Escan[iv][is],NevtSc[is], AVE[iv],ERR[iv],rms);
        } else {
  //
     	    if (AVE[2] != 0) {
     	       rat = AVE[0] / AVE[2];
     	       if (AVE[0] != 0) d1 = ERR[0] / AVE[0];
     	        else d1 = 0;
     	       d2 = ERR[2] / AVE[2];
     	       ert = rat * TMath::Sqrt( d1*d1+d2*d2 );
     	    } else {
     	       rat = 0;
     	       ert = 0;
     	    }
  //     
     	    AVE[NVar1] = rat;
       	  ERR[NVar1] = ert;		 
     	  }
//
//
        if (iv > 2) {
          Var[iv][is] = AVE[iv];
          eVar[iv][is] = ERR[iv];
        } else { 
          Var[iv][is] = AVE[iv] * MeV2GeV;
	        eVar[iv][is] = ERR[iv] * MeV2GeV;
        }   
//
      } //  over iv
   } //  over is
//
//  --------------------------------------------------------------------
//
//  Write [average energies and] sampling fractions VS scan parameter
//
   sprintf (datdir, "%s", JOB1);
// 
   sprintf (comm, "mkdir -p %s", datdir);
   gSystem->Exec(comm);
//
//
   for (iv=0; iv<NVar; iv++) {
      if (iv != IVar) continue;
//
      sprintf (datfil, "%s/%s_eta_%s-%s.dat", 
               datdir, CVar[iv], CHEC[iHEC], JOB2);
      cout << " ===  Written only:  " << TVar[IVar];
      cout << " as a function of Pseudorapidity to "<< datfil <<" === " << endl;
//      
      fout = fopen (datfil,"w");  
//
      fprintf (fout, "%5d \n", Nscan);
      fprintf (fout, "%10.3f%10.3f%10.3f \n", Scan1,Scan2,DScan); 
//
      for (is=0; is<Nscan; is++) {
         Sc = Scan1 + 0.5*DScan + is*DScan;
         fprintf (fout, "%10.3f %8d", Sc, NevtSc[is]);
         fprintf (fout, " %15.7f %15.7f \n", Var[iv][is],eVar[iv][is]);
      }
//
      fclose (fout);
//
   } //  over iv
//
/* ------------------------------------------------------------------ */
//
//  End
//
   return;
}
////////////////////////////////////////////////////////////////////////
