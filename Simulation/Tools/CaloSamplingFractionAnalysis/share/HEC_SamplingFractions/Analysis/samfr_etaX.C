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

#include "ClCaloHits.h"
#include "ClCaloHits.C"

#include "substa.C"

void samfr_etaX (
     Int_t   iHEC  = 0,   //  HEC wheel (0 - front, 1 - rear)
     Int_t   iPart = 0,   //  primary particle
     Int_t   IEner = 100, //  particle energy [GeV]
     Char_t const *G4Ver = "10.1",          //  Geant4 version
     Char_t const *PhLis = "FTFP_BERT_ATL", //  Physics list
     Char_t const *Dir   = "/eos/user/k/kiryunin/HEC_SF")
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
   ClCaloHits *clcalohits;
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
   chain = new TChain ("calohitD3PD");
//
//  Electrons + Positrons + Gammas
//   
   if (iPart == 0) {
      for (ip=1; ip<=3; ip++) {
         sprintf (job, "%s_e%04d_%s", CPart[ip],IEner,SHEC[iHEC]); 
//
         for (ia=1; ia<=NSamp; ia++) {
            sprintf (rootfile, 
	             "%s/%s/DumpHits/%s/%02d.root", 
		     Dir,JOB1, job,ia);
            if (ia == 1)
               cout << " ===  " << rootfile << "  === " << endl;  
            chain->Add(rootfile);
	 }   
      }      
   }
//
//  Single particle
//
   else {
      sprintf (job, "%s_e%04d_%s", CPart[iPart],IEner,SHEC[iHEC]); 
//
      for (ia=1; ia<=NSamp; ia++) {
         sprintf (rootfile, 
	          "%s/%s/DumpHits/%s/%02d.root", 
		  Dir,JOB1, job,ia);
         if (ia == 1)
            cout << " ===  " << rootfile << "  === " << endl;  
         chain->Add(rootfile);
      }      
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
   clcalohits = new ClCaloHits(chain);
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
   	 for (in=0; in<nentries; in++)
   	    Escan[iv][is][in] = 0;
      }  
   }
//
/* ------------------------------------------------------------------ */
//
//  Loop over events
//
   for (in=0; in<nentries; in++) {
      chain->GetEntry(in);
//
      nvtx = clcalohits->mcvtx_n;  
      if (nvtx < 1) {
         cerr << " *** Bad nvtx = " << nvtx;
	 cerr << " for in=" << in << " !!! *** " << endl;
         exit (-2);
      }
//
      npart = clcalohits->mcpart_n;  
      if (npart < 1) {
         cerr << " *** Bad npart = " << npart;
         cerr << " for in=" << in << " !!! *** " << endl;
         exit (-2);
      }
//
      Typ = (*clcalohits->mcpart_type)[0];
      if (PPart[iPart] != 0 && Typ != PPart[iPart]) {
         cerr << " *** Bad primary particles Typ = " << Typ;
         cerr << " for PPart=" << PPart[iPart];
         cerr << " for in=" << in << " !!! *** " << endl;
         exit (-2);
      }
//
//
      Sc = double( (*clcalohits->mcpart_eta)[0] );
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
      Nhits = clcalohits->laract_n;
//
      if (Nhits > 0) {
         for (ih=0; ih<Nhits; ih++) {
            //  any hit
            EHit[0] = double( (*clcalohits->laract_EnergyTot)[ih] );
            EHit[1] = double( (*clcalohits->laract_EnergyVis)[ih] );
            EHit[2] = double( (*clcalohits->laract_EnergyEm)[ih] );
            EHit[3] = double( (*clcalohits->laract_EnergyNonEm)[ih] );
            EHit[4] = double( (*clcalohits->laract_EnergyInv)[ih] );
            EHit[5] = double( (*clcalohits->laract_EnergyEsc)[ih] );
//
            //  total energy
            if ( EHit[0] == 0) {
	       cout << " xxx  Zero LAr hit in " << TMat[im];
	       cout << ":  " << TEHit[0];
	       cout << " = " << EHit[0];
	       cout << " for in/ih=" << in << "/" << ih << "  xxx " << endl;
	    }
//
            //  no negative energies (except invisible and, hence, total)
            for (ie=0; ie<NEHit; ie++) {
               if (ie==0 || ie==4) continue;
//	       
               if ( EHit[ie] < 0) {
	          cerr << " *** Bad LAr hit in " << TMat[im];
		  cerr << ":  " << TEHit[ie];
		  cerr << " = " << EHit[ie];
	          cerr << " for in/ih=" << in << "/" << ih;
		  cerr << " !!! *** " << endl;
                  exit (-2);
	       }
	    }
//
            //  sum for total energy
            diff = EHit[0] - (EHit[1] + EHit[4]+EHit[5]);
            if (EHit[0] != 0) 
               diff = TMath::Abs( diff / EHit[0] );
	    else   
               diff = TMath::Abs( diff );
	    if (diff > dCut) {
	       cerr << " *** Bad LAr hit in " << TMat[im];
	       cerr << ":  Sum for Etot - diff = ";
	       cerr << diff << ", Etot = " << EHit[0];
	       cerr << " for in/ih=" << in << "/" << ih << " !!! *** " << endl;
               exit (-2);
	    }	    
//
            //  sum for visible energy
            diff = EHit[1] - (EHit[2]+EHit[3]);
	    if (EHit[1] != 0)
               diff = TMath::Abs( diff ) / EHit[1];
            else	       
               diff = TMath::Abs( diff );
	    if (diff > dCut) {
	       cerr << " *** Bad LAr hit in " << TMat[im];
	       cerr << ":  Sum for Evis - diff = ";
	       cerr << diff << ", Evis = " << EHit[1];
	       cerr << " for in/ih=" << in << "/" << ih << " !!! *** " << endl;
               exit (-2);
	    }	    
//
//  HEC active hits
//
	    Det = (*clcalohits->laract_detector)[ih];
	    Sam = (*clcalohits->laract_sample)[ih];
	    Sid = (*clcalohits->laract_side)[ih];
//
            if (Det==1 && Sid==2) {
	       if (Sam < 8 || Sam > 11) {
	          cerr << " *** Bad HEC sample = " << Sam;
	          cerr << " in " << TMat[im];
	          cerr << " for in/ih=" << in << "/" << ih;
	          cerr << " !!! *** " << endl;
                  exit (-2);
	       }
//
               iw = int( Sam/2 ) - 4;
	       if (iw < 0 || iw >= NHEC) {
	          cerr << " *** Bad HEC wheel = " << iw;
		  cerr << " for Sam=" << Sam;
	          cerr << " in " << TMat[im];
	          cerr << " for in/ih=" << in << "/" << ih;
	          cerr << " !!! *** " << endl;
                  exit (-2);
	       }
//
               if (iw == iHEC)
                  Escan[iv][is][ NevtSc[is] ] += EHit[1];
	    }
//	    	    
         } //  over hits
      } //  Nhits > 0
//
//  --------------------------------------------------------------------
//
//  Check inactive (i.e. passive) LAr hits
//      
      iv = im = 1;
      Nhits = clcalohits->larinact_n;
//
      if (Nhits > 0) {
         for (ih=0; ih<Nhits; ih++) {
            //  any hit
            EHit[0] = double( (*clcalohits->larinact_EnergyTot)[ih] );
            EHit[1] = double( (*clcalohits->larinact_EnergyVis)[ih] );
            EHit[2] = double( (*clcalohits->larinact_EnergyEm)[ih] );
            EHit[3] = double( (*clcalohits->larinact_EnergyNonEm)[ih] );
            EHit[4] = double( (*clcalohits->larinact_EnergyInv)[ih] );
            EHit[5] = double( (*clcalohits->larinact_EnergyEsc)[ih] );
//
            //  total energy
            if ( EHit[0] == 0) {
	       cout << " xxx  Zero LAr hit in " << TMat[im];
	       cout << ":  " << TEHit[0];
	       cout << " = " << EHit[0];
	       cout << " for in/ih=" << in << "/" << ih << "  xxx " << endl;
	    }
//
            //  no negative energies (except invisible and, hence, total)
            for (ie=0; ie<NEHit; ie++) {
               if (ie==0 || ie==4) continue;
//	       
               if ( EHit[ie] < 0) {
	          cerr << " *** Bad LAr hit in " << TMat[im];
		  cerr << ":  " << TEHit[ie];
		  cerr << " = " << EHit[ie];
	          cerr << " for in/ih=" << in << "/" << ih;
		  cerr << " !!! *** " << endl;
                  exit (-2);
	       }
	    }
//
            //  sum for total energy
            diffa = TMath::Abs( EHit[0] - (EHit[1] + EHit[4]+EHit[5]) );
	    if (EHit[0] != 0)
               diffr = diffa / TMath::Abs( EHit[0] );
            else
               diffr = 0;
	    if (diffa > dCut && diffr > dCut) {
	       cerr << " *** Bad LAr hit in " << TMat[im];
	       cerr << ":  Sum for Etot ";
	       cerr << "- diffa = " << diffa;
	       cerr << ", diffr = " << diffr;
	       cerr << ", Etot = " << EHit[0];
	       cerr << " for in/ih=" << in << "/" << ih << " !!! *** " << endl;
               exit (-2);
	    }	    
//
            //  sum for visible energy
            diff = EHit[1] - (EHit[2]+EHit[3]);
	    if (EHit[1] != 0)
               diff = TMath::Abs( diff ) / EHit[1];
            else	       
               diff = TMath::Abs( diff );
	    if (diff > dCut) {
	       cerr << " *** Bad LAr hit in " << TMat[im];
	       cerr << ":  Sum for Evis - diff = ";
	       cerr << diff << ", Evis = " << EHit[1];
	       cerr << " for in/ih=" << in << "/" << ih << " !!! *** " << endl;
               exit (-2);
	    }	    
//
//  HEC passive hits
//
	    Det = (*clcalohits->larinact_detector)[ih];
	    Sam = (*clcalohits->larinact_sample)[ih];
	    Sid = (*clcalohits->larinact_side)[ih];
//
            if (Det==1 && Sid==2) {
	       if (Sam < 8 || Sam > 11) {
	          cerr << " *** Bad HEC sample = " << Sam;
	          cerr << " in " << TMat[im];
	          cerr << " for in/ih=" << in << "/" << ih;
	          cerr << " !!! *** " << endl;
                  exit (-2);
	       }
//
               iw = int( Sam/2 ) - 4;
	       if (iw < 0 || iw >= NHEC) {
	          cerr << " *** Bad HEC wheel = " << iw;
		  cerr << " for Sam=" << Sam;
	          cerr << " in " << TMat[im];
	          cerr << " for in/ih=" << in << "/" << ih;
	          cerr << " !!! *** " << endl;
                  exit (-2);
	       }
//
               if (iw == iHEC)
                  Escan[iv][is][ NevtSc[is] ] += EHit[1];
	    }
//	    	    
         } //  over hits
      } //  Nhits > 0
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
   	 Escan[iv][is][in] = Escan[0][is][in] + 
   			     Escan[1][is][in];
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
     	 }
     	 else {   
//
   	    if (AVE[2] != 0) {
   	       rat = AVE[0] / AVE[2];
   	       if (AVE[0] != 0) 
     		  d1 = ERR[0] / AVE[0];
   	       else
     		  d1 = 0;
   	       d2 = ERR[2] / AVE[2];
   	       ert = rat * TMath::Sqrt( d1*d1+d2*d2 );
   	    }  
   	    else {
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
         }
	 else { 
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
      cout << " ===  Written only:  " << TVar[IVar];
      cout << " as a function of Pseudorapidity  === " << endl;
//      
      sprintf (datfil, "%s/%s_eta_%s-%s.dat", 
               datdir, CVar[iv], CHEC[iHEC], JOB2);
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
