/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
//  Plot and Fit Pseudorapidity dependence of the Sampling Fraction
//  with P0 and P1
//  - for whole range and after different Eta selection 
//
//  Use P0 fit for selected region without tie-rods to get
//  HEC sampling fractions (for front and rear wheels)
//

#include <iomanip>

TCanvas *can;
TH1     *frame;

void get_SF (
     Char_t const *G4Ver = "10.1",           //  Geant4 version
     Char_t const *PhLis = "FTFP_BERT_ATL",  //  Physics list
     Int_t   iPart = 0,    //  primary particle
     Int_t   IEner = 100 ) //  particle energy [GeV]
{
//
//
   gStyle->SetLabelSize(0.040,"Y");
//   
   gROOT->ForceStyle();
//
//  -------------------------------------------------------------------- 
//
//
   const Int_t NHEC = 2; //  HEC wheels
   Char_t const *THEC[NHEC] = { "HEC front wheel", "HEC  rear wheel" };
   Char_t const *CHEC[NHEC] = { "fwh", "rwh" };
   Char_t const *SHEC[NHEC] = { "sc1", "sc2" };
   Float_t  Cut1[NHEC] = { 1.65, 1.80 }; 
   Float_t  Cut2[NHEC] = { 2.95, 3.05 }; 
   Double_t SamFr[NHEC], eSamFr[NHEC];
//
//
   const Int_t NPart = 4;
   Char_t const *TPart[NPart] = { 
      "Electrons+Positrons+Gammas", 
      "Electron", "Positron", "Gamma" };
   Char_t const *CPart[NPart] = { "EM", "ele", "pos", "gam" };   
//
//
   const Int_t NVar = 1;
   const Char_t *TVar[NVar] = { "Sampling Fraction" };
   const Char_t *UVar[NVar] = { "SF = <E_{ACT}> /  <E_{TOT}>" };
   const Char_t *CVar[NVar] = { "samf" };
//
//  --------------------------------------------------------------------
// 
//
   TF1          *fuc1,  *fuc2;
   TGraphErrors *graf1, *graf2;
   TText        *txt;
//
//
   Int_t    *N;
   Float_t **X, **EX;
   Float_t **Y, **EY;
//
//
   Int_t    ib,it;
   Int_t    iHEC, n,nb, ipad;
   Int_t    inexp, inval;
   Float_t  Sele1,Sele2, xmin,xmax, ymin,ymax,yb, dmin,dmax,db, xtxt,ytxt;
   Float_t  A,B, eA,eB, X1,X2, sf,sf1,sf2, esf,esf1,esf2, r1,r2;
   Double_t expon, powr,mant, round;
   Char_t   gtit[999], utit[99], ytit[99];   
   Char_t   Tsel[111],Csel[111], Cfile[999], Text[111];
   Char_t   comm[111], pdffil[111];
   Char_t   datdir[999], datfil[999];
//
   FILE *fout;
   ifstream fin;
//
//
   Int_t NSlct = 3;
   Int_t IVar  = 0;
//
   Char_t const *Xtit = "#eta";   
//
//  -------------------------------------------------------------------- 
//
//  Arguments
//
   if (iPart < 0 || iPart >= NPart) {
      cout << " *** Bad iPart=" << iPart << " !!! *** " << endl;
      exit (-2);
   }
//
//
   sprintf (ytit, "%s", UVar[IVar]);
//
//  Canvas
//
   can = new TCanvas("can", "mypage", 980,0,600,900);
//
   can->SetFillColor(10);
   can->SetBorderSize(0);
   can->SetBorderMode(0);
//
//  -------------------------------------------------------------------- 
//
//  Loop over HEC wheels
//
   for (iHEC=0; iHEC<NHEC; iHEC++) {
       N = new Int_t     [NSlct];
       X = new Float_t * [NSlct];
      EX = new Float_t * [NSlct];
       Y = new Float_t * [NSlct];
      EY = new Float_t * [NSlct];
//
//  Read in sampling fractions without selection
//
      it = 0;
//
      sprintf (Cfile, 
               "%s-%s/%s_eta_%s-%s_e%04d_%s.dat",
               G4Ver,PhLis, CVar[IVar], CHEC[iHEC], 
	       CPart[iPart],IEner,SHEC[iHEC]);
      cout << " ===  File:  " << Cfile << endl;		 
//
      Sele1 = Sele2 = 0;
//
      read_var_cutX ( Cfile, Sele1,Sele2, N[it], X[it],EX[it], Y[it],EY[it]);
//
//  Common X-boundaries
//
      n = N[it]-1;
      xmin = X[it][0] - EX[it][0];
      xmax = X[it][n] + EX[it][n];
//
//  Read in sampling fractions after boundary cuts
//
      it = 1;
//   
      Sele1 = Cut1[iHEC];
      Sele2 = Cut2[iHEC];
//
      read_var_cutX ( Cfile, Sele1,Sele2, N[it], X[it],EX[it], Y[it],EY[it]);
//
//  Boundaries of the selected regions (not bin centers)
//  where to calculate P1-results
//
      X1 = Cut1[iHEC];
      X2 = Cut2[iHEC];
//
//  Select bins not affected by tie-rods (using data after boundary cuts)
//
      it = 2;
//   
      n = N[1];
       X[it] = new Float_t [n];
      EX[it] = new Float_t [n];
       Y[it] = new Float_t [n];
      EY[it] = new Float_t [n];
//
//
      nb = 0;
//
      if (iHEC == 0) {
         for (ib=0; ib<n; ib++) {
      	    if (X[1][ib] > 1.55 && X[1][ib] < 1.60) continue;
      	    if (X[1][ib] > 1.75 && X[1][ib] < 1.80) continue;
      	    if (X[1][ib] > 2.05 && X[1][ib] < 2.10) continue;
      	    if (X[1][ib] > 2.70 && X[1][ib] < 2.80) continue;
//
      	     X[it][nb] =  X[1][ib];	    
      	    EX[it][nb] = EX[1][ib];	    
      	     Y[it][nb] =  Y[1][ib];	    
      	    EY[it][nb] = EY[1][ib];
//
      	    nb ++;	    
         }
      }
//
      else if (iHEC == 1) {
         for (ib=0; ib<n; ib++) {
      	    if (X[1][ib] > 1.90 && X[1][ib] < 2.00) continue;
      	    if (X[1][ib] > 2.25 && X[1][ib] < 2.30) continue;
      	    if (X[1][ib] > 2.85 && X[1][ib] < 3.00) continue;
//
      	     X[it][nb] =  X[1][ib];	    
      	    EX[it][nb] = EX[1][ib];	    
      	     Y[it][nb] =  Y[1][ib];	    
      	    EY[it][nb] = EY[1][ib];
//
      	    nb ++;	    
         }
      }
//
      N[it] = nb;   
//
//  Prepare to draw
//
      sprintf (gtit, 
               "G4.%s, %s:  %s %s VS Pseudorapidity (%d GeV %s)", 
               G4Ver,PhLis, THEC[iHEC],TVar[IVar],  IEner,TPart[iPart]);
      cout << " ===  Title: " << gtit << endl;
//
      can->Clear();
//
      mypage (gtit, 2,NSlct);      
//
      sprintf (Tsel,"%.2f <= #eta < %.2f",Cut1[iHEC],Cut2[iHEC]);
//   
//  Loop over jobs: no selection and after selection
//
      for (it=0; it<NSlct; it++) {
         if (it == 0) 
            sprintf (utit, "Whole Range");
         else if (it == 1)	    
            sprintf (utit, "Selected Region: %s", Tsel);
         else
            sprintf (utit, "Selected Region without Tie-Rods");
// 
//     
         ymin =  1.e10;
         ymax = -1.e10;
//
         for (ib=0; ib<N[it]; ib++) {
            yb = Y[it][ib] - EY[it][ib];
            if (yb < ymin)
               ymin = yb;
            yb = Y[it][ib] + EY[it][ib];
            if (yb > ymax)
               ymax = yb;
         }	 
//
         yb = (ymax-ymin);
         ymin = ymin - yb*0.2;
         ymax = ymax + yb*0.5;      
//
//  Graph 1: plot and fit with constant
//
         ipad = it*2+1;
         pad->cd(ipad);
//
         myplot (xmin,xmax, ymin,ymax, Xtit,ytit, utit);
//      
         graf1 = new TGraphErrors (N[it], X[it],Y[it], EX[it],EY[it]);
         graf1->SetMarkerStyle(20);
         graf1->SetMarkerColor(4);
//
         graf1->Draw("P");	 
//
//
         fuc1 = new TF1 ("fuc1", "[0]", xmin,xmax);
         fuc1->SetParNames("SF0");	
         graf1->Fit("fuc1","EQ","",xmin,xmax);   
//
          sf = fuc1->GetParameter(0);      
         esf = fuc1->GetParError(0); 
//
         if (it == 2) {
             SamFr[iHEC] =  sf;
            eSamFr[iHEC] = esf;		   
         }
//
//  Deviation from the fit
//      
         dmin =  1.e10;
         dmax = -1.e10;
//
         for (ib=0; ib<N[it]; ib++) {
            db = Y[it][ib]/sf;
            if (db < dmin)
               dmin = db;
            if (db > dmax)
               dmax = db;
         } 
//
         txt = new TText ();
         txt->SetTextAlign(21);
//
         xtxt = 0.5*(xmin+xmax);
         ytxt = ymin + (ymax-ymin)*0.05;
         sprintf (Text, "%5.3f < SF/SF0 < %5.3f", dmin,dmax);
//
         txt->DrawText(xtxt,ytxt, Text);	
//
//  Graph 2: plot and linear fit
//   
         ipad = it*2+2;
         pad->cd(ipad);
//       
      	 myplot (xmin,xmax, ymin,ymax, Xtit,ytit, utit);
//       
         graf2 = new TGraphErrors (N[it], X[it],Y[it], EX[it],EY[it]);
         graf2->SetMarkerStyle(20);
         graf2->SetMarkerColor(4);
//       
      	 graf2->Draw("P");	 
//       
//
      	 fuc2 = new TF1 ("fuc2","[0]*x+[1]", xmin,xmax);
         graf2->Fit("fuc2","EQ","",xmin,xmax);
//       
//  Values at boundaries
//
          A = fuc2->GetParameter(0);	   
         eA = fuc2->GetParError(0);	  
          B = fuc2->GetParameter(1);
         eB = fuc2->GetParError(1);
//
         sf1 = A * X1 + B;
         esf1 = TMath::Sqrt( X1*X1 * eA*eA + eB*eB );	     
         sf2 = A * X2 + B;    
         esf2 = TMath::Sqrt( X2*X2 * eA*eA + eB*eB );	     
//
//
         r1 = sf1/sf;		    
         r2 = sf2/sf;		    
//
         txt = new TText ();
         txt->SetTextAlign(11);
//
         xtxt = xmin + 0.05*(xmax-xmin);
         ytxt = ymin + (ymax-ymin)*0.15;
         sprintf (Text, "SF(%.3f) = %8.6f   /SF0 = %5.3f", X1,sf1,r1);
//
         txt->DrawText(xtxt,ytxt, Text);	
//
//
         ytxt = ymin + (ymax-ymin)*0.05;
         sprintf (Text, "SF(%.3f) = %8.6f   /SF0 = %5.3f", X2,sf2,r2);
//
         txt->DrawText(xtxt,ytxt, Text);	     
//
      } //  over it
//
//
      can->Update();
      //
      //  Save PDF file
      //
      sprintf (pdffil, "%s-%s/SF_HEC%s_%s_e%04d.pdf", G4Ver,PhLis, CHEC[iHEC], CPart[iPart],IEner);
      can->SaveAs(pdffil);
//
//
      delete     N;
      delete []  X;	 
      delete [] EX;	 
      delete []  Y;	 
      delete [] EY;	 
//
   } //  over iHEC
//
//  --------------------------------------------------------------------
//
//  Write HEC sampling fractions for both wheels
//
   if (iPart == 0 && IEner == 100) 
      sprintf (datfil, "%s-%s/HEC_SF.txt", G4Ver,PhLis);
   else {
      sprintf (datfil, "%s-%s/HEC_SF_%s_e%04d.txt",
     	       G4Ver,PhLis, CPart[iPart],IEner);
   }
//
   fout = fopen (datfil,"w");  
//   
   for (iHEC=0; iHEC<NHEC; iHEC++) {
//
//  Make rounding of the sampling fraction 
//  (keeping number of digits corresponding to its error)
//
      if (SamFr[iHEC] <= 0 || eSamFr[iHEC] <= 0) {
         cerr << " *** Bad SamFr/eSamFr = " << SamFr[iHEC] << "/";
	 cerr << eSamFr[iHEC] << " for " << THEC[iHEC] << " !!! *** " << endl;
	 exit (-2);
      }
//
//  Get first meaningful digit of the error
//
      expon = log10( eSamFr[iHEC] );	     
//
      if (expon >= 0)
         inexp = int( expon );
      else
         inexp = int( expon ) - 1;
      powr  = pow( 10, inexp );
      mant  = eSamFr[iHEC]/powr;
      inval = int( mant + 0.5 );
      round = inval * powr;
//
      if (inval == 10) {
         inval = 1;
         inexp ++;
      }
//
//  Round sampling fraction
//
      powr  = pow( 10, inexp );
//
      mant  = SamFr[iHEC]/powr;
      inval = int( mant + 0.5 );
      round = inval * powr;
//
//  Write sampling fraction in txt-file
//
      fprintf (fout, " %s :  %f +- %f  ->  %g \n", 
               THEC[iHEC], SamFr[iHEC],eSamFr[iHEC], round);
      cout<<THEC[iHEC]<<" : "<<SamFr[iHEC]<<" +- "<<eSamFr[iHEC]<<"  ->  "<<round<<endl;
   }
//
   fclose (fout);
//
/* ------------------------------------------------------------------ */
//
//      
}
////////////////////////////////////////////////////////////////////////
