/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// Function to handle 2D maps of modules, one for each region
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMon2DMaps.h"
#include "InDetIdentifier/PixelID.h"
//#include "TH2I.h"
#include "TH2F.h"
#include "GaudiKernel/StatusCode.h"     
#include <string.h>

PixelMon2DMaps::PixelMon2DMaps(std::string name, std::string title)
{
   m_cnt = 0;
   IBL3D = new TH2F((name+"_IBL3D").c_str(),(title + ", IBL 3D modules " + ";eta index of module;phi index of module").c_str(), 8, -.5, 7.5, 14, -0.5, 13.5);
   IBL2D = new TH2F((name+"_IBL2D").c_str(),(title + ", IBL planar modules " + ";shifted eta index of module;phi index of module").c_str(), 12, -6.5, 5.5, 14, -0.5, 13.5);
   IBL = new TH2F((name+"_IBL").c_str(),    (title + ", IBL "        + ";shifted eta index of module;phi index of module").c_str(), 32, -16.5, 15.5, 14, -0.5, 13.5);
   B0 = new TH2F((name+"_B0").c_str(),      (title + ", B0 "  + ";eta index of module;phi index of module").c_str(),13,-6.5,6.5,22,-0.5,21.5);
   B1 = new TH2F((name+"_B1").c_str(),      (title + ", B1 "  + ";eta index of module;phi index of module").c_str(),13,-6.5,6.5,38,-0.5,37.5);
   B2 = new TH2F((name+"_B2").c_str(),      (title + ", B2 "  + ";eta index of module;phi index of module").c_str(),13,-6.5,6.5,52,-0.5,51.5);
   A  = new TH2F((name+"_ECA" ).c_str(),    (title + ", ECA "          + ";disk number;phi index of module").c_str(),         3,-0.5,2.5,48,-0.5,47.5);
   C  = new TH2F((name+"_ECC" ).c_str(),    (title + ", ECC "          + ";disk number;phi index of module").c_str(),         3,-0.5,2.5,48,-0.5,47.5);
   DBMA = new TH2F((name+"_DBMA" ).c_str(), (title + ", DBMA "         + ";layer number;phi index of telescope").c_str(),     3,-0.5,2.5,4,-0.5,3.5);
   DBMC = new TH2F((name+"_DBMC" ).c_str(), (title + ", DBMC "         + ";layer number;phi index of telescope").c_str(),     3,-0.5,2.5,4,-0.5,3.5);

   formatHist();
}

PixelMon2DMaps::~PixelMon2DMaps()
{
   delete IBL3D;
   delete IBL2D;
   delete IBL;
   delete B0;
   delete B1;
   delete B2;
   delete A;
   delete C;
   delete DBMA;
   delete DBMC;
}

void PixelMon2DMaps::Reset()
{
   IBL3D->Reset();
   IBL2D->Reset();
   IBL->Reset();
   B0->Reset();
   B1->Reset();
   B2->Reset();
   A->Reset();
   C->Reset();
   DBMA->Reset();
   DBMC->Reset();
}

void PixelMon2DMaps::Fill(Identifier &id, const PixelID* pixID, bool doIBL)
{
   int bec = pixID->barrel_ec(id);
   int ld  = pixID->layer_disk(id);
   int pm  = pixID->phi_module(id);

   if(bec==2) A->Fill(ld, pm);
   else if(bec==-2) C->Fill(ld, pm);
   else if(bec==4) DBMA->Fill(ld, pm);
   else if(bec==-4) DBMC->Fill(ld, pm);
   else if(bec==0)
   {
      if(doIBL){ld--;}
      int em  = pixID->eta_module(id);
      if(ld ==0){ 
	      B0->Fill(em, pm);
      }
      else if(ld ==1){ 
	      B1->Fill(em, pm);
      }
      else if(ld ==2){ 
	      B2->Fill(em, pm);
      }
      else if(ld ==-1){
	      int feid = 0;
	      int emf = 0;
	      if(em<6 && em>-7){
	         if(pixID->eta_index(id) >= 80) feid = 1;
	         emf = 2 * em + feid; 
	         IBL2D->Fill(em, pm);
	      }
	      else if(em<-6){
	         emf = em - 6;
	         IBL3D->Fill(em+10, pm);
	      }
	      else{
	         emf = em + 6;
	         IBL3D->Fill(em-2, pm);
	      }
	      IBL->Fill(emf, pm);
      }
   }
}

void PixelMon2DMaps::WeightingFill(Identifier &id, const PixelID* pixID, bool doIBL, float weight)
{
   int bec = pixID->barrel_ec(id);
   int ld  = pixID->layer_disk(id);
   int pm  = pixID->phi_module(id);

   if(bec==2) A->Fill(ld, pm, weight);
   else if(bec==-2) C->Fill(ld, pm, weight);
   else if(bec==4) DBMA->Fill(ld, pm, weight);
   else if(bec==-4) DBMC->Fill(ld, pm, weight);

   else if(bec==0)
   {
      if(doIBL){ld--;}
      int em  = pixID->eta_module(id);
      if(ld ==0){ 
	      B0->Fill(em, pm, weight);
      }
      else if(ld ==1){ 
	      B1->Fill(em, pm, weight);
      }
      else if(ld ==2){ 
	      B2->Fill(em, pm, weight);
      }
      else if(ld ==-1){
	      int feid = 0;
	      int emf = 0;
	      if(em<6 && em>-7){
	         if(pixID->eta_index(id) >= 80) feid = 1;
	         emf = 2 * em + feid; 
	         IBL2D->Fill(em, pm, weight);
	      }
	      else if(em<-6){
	         emf = em - 6;
	         IBL3D->Fill(em+10, pm, weight);
	      }
	      else{
	         emf = em + 6;
	         IBL3D->Fill(em-2, pm, weight);
	      }
	      IBL->Fill(emf, pm, weight);
      }
   }
}   


//void PixelMon2DMaps::Scale (double number)
void PixelMon2DMaps::Scale (double number, bool doIBL)
{
   if (number==0) return; //shouldn't happen the way function is called, but dummy check to avoid divide by zero

   A->Scale((float)  1.0/number);
   C->Scale((float)  1.0/number);
   DBMA->Scale((float)  1.0/number);
   DBMC->Scale((float)  1.0/number);
   B0->Scale((float) 1.0/number);
   B1->Scale((float) 1.0/number);
   B2->Scale((float) 1.0/number);
   if(doIBL){
      IBL2D->Scale((float) 1.0/number);
      IBL3D->Scale((float) 1.0/number);
      IBL->Scale((float) 1.0/number);
   }
}

void PixelMon2DMaps::ScaleBynPixnEvt(int nevent, bool doIBL)
//void PixelMon2DMaps::FillNormalized(PixelMon2DMaps* old, int nevent, int nActive_IBL2D, int nActive_IBL3D, int nActive_B0, int nActive_B1, int nActive_B2, int nActive_ECA, int nActive_ECC, bool doIBL)
//void PixelMon2DMaps::FillNormalized(PixelMon2DMaps* old, int nevent, int nActive_IBL2D, int nActive_IBL3D, int nActive_B0, int nActive_B1, int nActive_B2, int nActive_ECA, int nActive_ECC)
{
   double nactivechannels_DBMA   = 1.0*nevent*46080;
   double nactivechannels_DBMC   = 1.0*nevent*46080;
   double nactivechannels_ECA   = 1.0*nevent*46080;
   double nactivechannels_ECC   = 1.0*nevent*46080;
   double nactivechannels_IBL2D = 1.0*nevent*26880*2;
   double nactivechannels_IBL3D = 1.0*nevent*26880;
   double nactivechannels_IBL   = nactivechannels_IBL2D + nactivechannels_IBL3D;
   double nactivechannels_B0    = 1.0*nevent*46080;
   double nactivechannels_B1    = 1.0*nevent*46080;
   double nactivechannels_B2    = 1.0*nevent*46080;

   if (nevent==0) return; //shouldn't happen the way function is called, but dummy check to avoid divide by zero

   DBMA->Scale((float)  1.0/nactivechannels_DBMA);
   DBMC->Scale((float)  1.0/nactivechannels_DBMC);
   A->Scale((float)  1.0/nactivechannels_ECA);
   C->Scale((float)  1.0/nactivechannels_ECC);
   B0->Scale((float) 1.0/nactivechannels_B0);
   B1->Scale((float) 1.0/nactivechannels_B1);
   B2->Scale((float) 1.0/nactivechannels_B2);
   if(doIBL){
      IBL2D->Scale((float) 1.0/nactivechannels_IBL2D);
      IBL3D->Scale((float) 1.0/nactivechannels_IBL3D);
      IBL->Scale((float) 1.0/nactivechannels_IBL);
   }
   /*
   for(int x=1; x<=A->GetXaxis()->GetNbins(); x++){
      for(int y=1; y<=A->GetYaxis()->GetNbins(); y++){
         A->SetBinContent(x, y, old->A->GetBinContent(x, y)/nactivechannels_ECA );
      }
   }
   for(int x=1; x<=C->GetXaxis()->GetNbins(); x++){
      for(int y=1; y<=C->GetYaxis()->GetNbins(); y++){
         C->SetBinContent(x, y, old->C->GetBinContent(x, y)/nactivechannels_ECC );
      }
   }
   for(int x=1; x<=B0->GetXaxis()->GetNbins(); x++){
      for(int y=1; y<=B0->GetYaxis()->GetNbins(); y++){
         B0->SetBinContent(x, y, old->B0->GetBinContent(x, y)/nactivechannels_B0 );
      }
   }
   for(int x=1; x<=B1->GetXaxis()->GetNbins(); x++){
      for(int y=1; y<=B1->GetYaxis()->GetNbins(); y++){
         B1->SetBinContent(x, y, old->B1->GetBinContent(x, y)/nactivechannels_B1 );
      }
   }
   for(int x=1; x<=B2->GetXaxis()->GetNbins(); x++){
      for(int y=1; y<=B2->GetYaxis()->GetNbins(); y++){
         B2->SetBinContent(x, y, old->B2->GetBinContent(x, y)/nactivechannels_B2 );
      }
   }
   if(doIBL){
   for(int x=1; x<=IBL->GetXaxis()->GetNbins(); x++){
      for(int y=1; y<=IBL->GetYaxis()->GetNbins(); y++){
         IBL->SetBinContent(x, y, old->IBL->GetBinContent(x, y)/nactivechannels_IBL );
      }
   }
   for(int x=1; x<=IBL2D->GetXaxis()->GetNbins(); x++){
      for(int y=1; y<=IBL2D->GetYaxis()->GetNbins(); y++){
         IBL2D->SetBinContent(x, y, old->IBL2D->GetBinContent(x, y)/nactivechannels_IBL2D );
      }
   }
   for(int x=1; x<=IBL3D->GetXaxis()->GetNbins(); x++){
      for(int y=1; y<=IBL3D->GetYaxis()->GetNbins(); y++){
         IBL3D->SetBinContent(x, y, old->IBL3D->GetBinContent(x, y)/nactivechannels_IBL3D );
      }
   }
   }
   */
}

void PixelMon2DMaps::Fill2DMon(PixelMon2DMaps* oldmap)
{
   for(int x=1; x<=DBMA->GetNbinsX(); x++){
      for(int y=1; y<=DBMA->GetNbinsY(); y++){
         float content = oldmap->DBMA->GetBinContent(x, y);
         DBMA->SetBinContent(x, y, content);
         oldmap->DBMA->SetBinContent(x, y, 0);
      }
   }
   for(int x=1; x<=DBMC->GetNbinsX(); x++){
      for(int y=1; y<=DBMC->GetNbinsY(); y++){
         float content = oldmap->DBMC->GetBinContent(x, y);
         DBMC->SetBinContent(x, y, content);
         oldmap->DBMC->SetBinContent(x, y, 0);
      }
   }
   for(int x=1; x<=A->GetNbinsX(); x++){
      for(int y=1; y<=A->GetNbinsY(); y++){
         float content = oldmap->A->GetBinContent(x, y);
         A->SetBinContent(x, y, content);
         oldmap->A->SetBinContent(x, y, 0);
      }
   }
   for(int x=1; x<=C->GetNbinsX(); x++){
      for(int y=1; y<=C->GetNbinsY(); y++){
         float content = oldmap->C->GetBinContent(x, y);
         C->SetBinContent(x, y, content);
         oldmap->C->SetBinContent(x, y, 0);
      }
   }
   for(int x=1; x<=B0->GetNbinsX(); x++){
      for(int y=1; y<=B0->GetNbinsY(); y++){
         float content = oldmap->B0->GetBinContent(x, y);
         B0->SetBinContent(x, y, content);
         oldmap->B0->SetBinContent(x, y, 0);
      }
   }
   for(int x=1; x<=B1->GetNbinsX(); x++){
      for(int y=1; y<=B1->GetNbinsY(); y++){
         float content = oldmap->B1->GetBinContent(x, y);
         B1->SetBinContent(x, y, content);
         oldmap->B1->SetBinContent(x, y, 0);
      }
   }
   for(int x=1; x<=B2->GetNbinsX(); x++){
      for(int y=1; y<=B2->GetNbinsY(); y++){
         float content = oldmap->B2->GetBinContent(x, y);
         B2->SetBinContent(x, y, content );
         oldmap->B2->SetBinContent(x, y, 0);
      }
   }
   for(int x=1; x<=IBL->GetNbinsX(); x++){
      for(int y=1; y<=IBL->GetNbinsY(); y++){
         float content = oldmap->IBL->GetBinContent(x, y);
         IBL->SetBinContent(x, y, content);
         oldmap->IBL->SetBinContent(x, y, 0);
      }
   }
   for(int x=1; x<=IBL2D->GetNbinsX(); x++){
      for(int y=1; y<=IBL2D->GetNbinsY(); y++){
         float content = oldmap->IBL2D->GetBinContent(x, y);
         IBL2D->SetBinContent(x, y, content);
         oldmap->IBL2D->SetBinContent(x, y, 0);
      }
   }
   for(int x=1; x<=IBL3D->GetNbinsX(); x++){
      for(int y=1; y<=IBL3D->GetNbinsY(); y++){
         float content = oldmap->IBL3D->GetBinContent(x, y);
         IBL3D->SetBinContent(x, y, content);
         oldmap->IBL3D->SetBinContent(x, y, 0);
      }
   }
}


void PixelMon2DMaps::ScaleByNChannels(int nActive_IBL2D, int nActive_IBL3D, int nActive_B0, int nActive_B1, int nActive_B2, int nActive_ECA, int nActive_ECC, int nActive_DBMA, int nActive_DBMC)
{
   double nactivechannels_DBMA   = 1.0*26880*nActive_DBMA;
   double nactivechannels_DBMC   = 1.0*26880*nActive_DBMC;
   double nactivechannels_ECA   = 1.0*46080*nActive_ECA;
   double nactivechannels_ECC   = 1.0*46080*nActive_ECC;
   double nactivechannels_IBL2D = 1.0*26880*2*nActive_IBL2D;
   double nactivechannels_IBL3D = 1.0*26880*nActive_IBL3D;
   double nactivechannels_IBL   = nactivechannels_IBL2D + nactivechannels_IBL3D;
   double nactivechannels_B0    = 1.0*46080*nActive_B0;
   double nactivechannels_B1    = 1.0*46080*nActive_B1;
   double nactivechannels_B2    = 1.0*46080*nActive_B2;

   if(nActive_DBMA>0) DBMA->Scale((double) 1.0/nactivechannels_DBMA);
   if(nActive_DBMC>0) DBMC->Scale((double) 1.0/nactivechannels_DBMC);
   if(nActive_ECA>0)  A->Scale((double) 1.0/nactivechannels_ECA);
   if(nActive_ECC>0)  C->Scale((double) 1.0/nactivechannels_ECC);
   if(nActive_B0>0)   B0->Scale((double) 1.0/nactivechannels_B0);
   if(nActive_B1>0)   B1->Scale((double) 1.0/nactivechannels_B1);
   if(nActive_B2>0)   B2->Scale((double) 1.0/nactivechannels_B2);
   if(nActive_IBL2D>0) IBL2D->Scale((double) 1.0/nactivechannels_IBL2D);
   if(nActive_IBL3D>0) IBL3D->Scale((double) 1.0/nactivechannels_IBL3D);
   if(nActive_IBL3D>0 || nActive_IBL2D>0) IBL->Scale((double) 1.0/nactivechannels_IBL);
}

void PixelMon2DMaps::formatHist()
{
   const int ndisk = 3;
   const int nphi_dbm  = 4;
   const int nphi  = 48;
   const char *disk[ndisk] = { "Disk 1", "Disk 2", "Disk 3" };
   const char *phi_dbm[nphi_dbm] = { "M1","M2","M3","M4"};
   const int nmod = 13;
   const int nmodIBL2D = 12;
   const int nmodIBL3D = 8;
   const int nmodIBL = 32;
   const char *mod[nmod] = { "M6C", "M5C", "M4C", "M3C", "M2C", "M1C", "M0","M1A", "M2A", "M3A", "M4A", "M5A", "M6A" } ;
   const char *modIBL2D[nmodIBL2D] = { "M3_C6", "M3_C5", "M2_C4", "M1_C3", "M1_C2", "M1_C1", "M1_A1", "M1_A2", "M2_A3", "M2_A4", "M3_A5", "M3_A6" } ;
   const char *modIBL3D[nmodIBL3D] = {"M4_C8_2","M4_C8_1","M4_C7_2","M4_C7_1","M4_A7_1","M4_A7_2","M4_A8_1","M4_A8_2"};
   /*
   const char *modIBL[nmodIBL] = {"C8_2","C8_1","C7_2","C7_1",
				    "C6_2","C6_1","C5_2","C5_1",
				    "C4_2","C4_1","C3_2","C3_1",
				    "C2_2","C2_1","C1_2","C1_1",
				    "A1_1","A1_2","A2_1","A2_2",
				    "A3_1","A3_2","A4_1","A4_2",
				    "A5_1","A5_2","A6_1","A6_2",
				    "A7_1","A7_2","A8_1","A8_2"};
   */
   const char *modIBL[nmodIBL] = {"C8","","C7","",
				  "C6","","C5","",
				  "C4","","C3","",
				  "C2","","C1","",
				  "A1","","A2","",
				  "A3","","A4","",
				  "A5","","A6","",
				  "A7","","A8",""};

   const int nstaveb = 14;
   const char *staveb[nstaveb] = {
     "S01", "S02", "S03", "S04", "S05", "S06","S07",
     "S08", "S09", "S10", "S11", "S12", "S13","S14"};
   const int nstave0 = 22;
   const char *stave0[nstave0] = {                      "B11_S2", 
      "B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2",
      "B04_S1", "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2",
      "B07_S1", "B07_S2", "B08_S1", "B08_S2", "B09_S1", "B09_S2",
      "B10_S1", "B10_S2", "B11_S1" };
   const int nstave1 = 38;
   const char *stave1[nstave1] = {
      "B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2",
      "B04_S1", "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2",
      "B07_S1", "B07_S2", "B08_S1", "B08_S2", "B09_S1", "B09_S2",
      "B10_S1", "B10_S2", "B11_S1", "B11_S2", "B12_S1", "B12_S2",
      "B13_S1", "B13_S2", "B14_S1", "B14_S2", "B15_S1", "B15_S2",
      "B16_S1", "B16_S2", "B17_S1", "B17_S2", "B18_S1", "B18_S2",
      "B19_S1", "B19_S2" };
   const int nstave2 = 52;
   const char *stave2[nstave2] = {          
                "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2",
      "B04_S1", "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2",
      "B07_S1", "B07_S2", "B08_S1", "B08_S2", "B09_S1", "B09_S2",
      "B10_S1", "B10_S2", "B11_S1", "B11_S2", "B12_S1", "B12_S2",
      "B13_S1", "B13_S2", "B14_S1", "B14_S2", "B15_S1", "B15_S2",
      "B16_S1", "B16_S2", "B17_S1", "B17_S2", "B18_S1", "B18_S2",
      "B19_S1", "B19_S2", "B20_S1", "B20_S2", "B21_S1", "B21_S2",
      "B22_S1", "B22_S2", "B23_S1", "B23_S2", "B24_S1", "B24_S2",
      "B25_S1", "B25_S2", "B26_S1", "B26_S2", "B01_S1" };
   const char *nstaveA[nphi] = {
      "B01_S2_M1", "B01_S2_M6", "B01_S2_M2", "B01_S2_M5", "B01_S2_M3", "B01_S2_M4", 
      "B02_S1_M1", "B02_S1_M6", "B02_S1_M2", "B02_S1_M5", "B02_S1_M3", "B02_S1_M4", 
      "B02_S2_M1", "B02_S2_M6", "B02_S2_M2", "B02_S2_M5", "B02_S2_M3", "B02_S2_M4", 
      "B03_S1_M1", "B03_S1_M6", "B03_S1_M2", "B03_S1_M5", "B03_S1_M3", "B03_S1_M4", 
      "B03_S2_M1", "B03_S2_M6", "B03_S2_M2", "B03_S2_M5", "B03_S2_M3", "B03_S2_M4", 
      "B04_S1_M1", "B04_S1_M6", "B04_S1_M2", "B04_S1_M5", "B04_S1_M3", "B04_S1_M4", 
      "B04_S2_M1", "B04_S2_M6", "B04_S2_M2", "B04_S2_M5", "B04_S2_M3", "B04_S2_M4", 
      "B01_S1_M1", "B01_S1_M6", "B01_S1_M2", "B01_S1_M5", "B01_S1_M3", "B01_S1_M4"};
   const char *nstaveC[nphi] = {
      "B01_S2_M4", "B01_S2_M3", "B01_S2_M5", "B01_S2_M2", "B01_S2_M6", "B01_S2_M1", 
      "B02_S1_M4", "B02_S1_M3", "B02_S1_M5", "B02_S1_M2", "B02_S1_M6", "B02_S1_M1", 
      "B02_S2_M4", "B02_S2_M3", "B02_S2_M5", "B02_S2_M2", "B02_S2_M6", "B02_S2_M1", 
      "B03_S1_M4", "B03_S1_M3", "B03_S1_M5", "B03_S1_M2", "B03_S1_M6", "B03_S1_M1", 
      "B03_S2_M4", "B03_S2_M3", "B03_S2_M5", "B03_S2_M2", "B03_S2_M6", "B03_S2_M1", 
      "B04_S1_M4", "B04_S1_M3", "B04_S1_M5", "B04_S1_M2", "B04_S1_M6", "B04_S1_M1", 
      "B04_S2_M4", "B04_S2_M3", "B04_S2_M5", "B04_S2_M2", "B04_S2_M6", "B04_S2_M1", 
      "B01_S1_M4", "B01_S1_M3", "B01_S1_M5", "B01_S1_M2", "B01_S1_M6", "B01_S1_M1"};

   for (int i=0; i<nphi; i++) 
   {
      A->GetYaxis()->SetBinLabel( i+1, nstaveA[i] );
      C->GetYaxis()->SetBinLabel( i+1, nstaveC[i] );
   }
   for (int i=0; i<nphi_dbm; i++) 
   {
      DBMA->GetYaxis()->SetBinLabel( i+1, phi_dbm[i] );
      DBMC->GetYaxis()->SetBinLabel( i+1, phi_dbm[i] );
   }
   for (int i=0; i<ndisk; i++) 
   {
      A->GetXaxis()->SetBinLabel( i+1, disk[i] );
      C->GetXaxis()->SetBinLabel( i+1, disk[i] );
      DBMA->GetXaxis()->SetBinLabel( i+1, disk[i] );
      DBMC->GetXaxis()->SetBinLabel( i+1, disk[i] );
   }
   for (int i=0; i<nmod; i++) 
   {
     B0->GetXaxis()->SetBinLabel( i+1, mod[i] );  // bin 0 is underflow
      B1->GetXaxis()->SetBinLabel( i+1, mod[i] );
      B2->GetXaxis()->SetBinLabel( i+1, mod[i] );
   }

   for (int i=0; i<nmodIBL; i++)
     {
       IBL->GetXaxis()->SetBinLabel( i+1, modIBL[i] );
     }
   for (int i=0; i<nstaveb; i++)
   {
      IBL->GetYaxis()->SetBinLabel( i+1, staveb[i] );
   }

   for (int i=0; i<nmodIBL2D; i++)
     {
       IBL2D->GetXaxis()->SetBinLabel( i+1, modIBL2D[i] );
     }
   for (int i=0; i<nstaveb; i++)
   {
      IBL2D->GetYaxis()->SetBinLabel( i+1, staveb[i] );
   }

   for (int i=0; i<nmodIBL3D; i++)
     {
       IBL3D->GetXaxis()->SetBinLabel( i+1, modIBL3D[i] );
     }
   for (int i=0; i<nstaveb; i++)
   {
      IBL3D->GetYaxis()->SetBinLabel( i+1, staveb[i] );
   }

   for (int i=0; i<nstave0; i++) 
   {
      B0->GetYaxis()->SetBinLabel( i+1, stave0[i] ); 
   }
   for (int i=0; i<nstave1; i++) 
   {
      B1->GetYaxis()->SetBinLabel( i+1, stave1[i] ); 
   }
   for (int i=0; i<nstave2; i++) 
   {
      B2->GetYaxis()->SetBinLabel( i+1, stave2[i] ); 
   }

   //Make the text smaller
   IBL2D->GetYaxis()->SetLabelSize(0.03);
   IBL3D->GetYaxis()->SetLabelSize(0.03);
   IBL->GetYaxis()->SetLabelSize(0.03);
   B0->GetYaxis()->SetLabelSize(0.03);
   B1->GetYaxis()->SetLabelSize(0.03);
   B2->GetYaxis()->SetLabelSize(0.03);
   A->GetYaxis()->SetLabelSize(0.02);
   C->GetYaxis()->SetLabelSize(0.02);
   DBMA->GetYaxis()->SetLabelSize(0.02);
   DBMC->GetYaxis()->SetLabelSize(0.02);
   //Move the lable so you can read it
   IBL2D->GetYaxis()->SetTitleOffset(1.35);
   IBL3D->GetYaxis()->SetTitleOffset(1.35);
   IBL->GetYaxis()->SetTitleOffset(1.35);
   B0->GetYaxis()->SetTitleOffset(1.35);
   B1->GetYaxis()->SetTitleOffset(1.35);
   B2->GetYaxis()->SetTitleOffset(1.35);
   A->GetYaxis()->SetTitleOffset(1.35);
   C->GetYaxis()->SetTitleOffset(1.35);
   DBMA->GetYaxis()->SetTitleOffset(1.35);
   DBMC->GetYaxis()->SetTitleOffset(1.35);
   //put histograms in the easier to read colz format
   IBL2D->SetOption("colz");
   IBL3D->SetOption("colz");
   IBL->SetOption("colz");
   B0->SetOption("colz");
   B1->SetOption("colz");
   B2->SetOption("colz");
   A->SetOption("colz");
   C->SetOption("colz");
   DBMA->SetOption("colz");
   DBMC->SetOption("colz");
   //force the minimum to be 0 so you can spot empty blocks easily
   IBL2D->SetMinimum(0.);
   IBL3D->SetMinimum(0.);
   IBL->SetMinimum(0.);
   B0->SetMinimum(0.);
   B1->SetMinimum(0.);
   B2->SetMinimum(0.);
   A->SetMinimum(0.);
   C->SetMinimum(0.);
   DBMA->SetMinimum(0.);
   DBMC->SetMinimum(0.);
   //Remvoe the stats box because it's in the way
   IBL2D->SetStats(0.);
   IBL3D->SetStats(0.);
   IBL->SetStats(0.);
   B0->SetStats(0.);
   B1->SetStats(0.);
   B2->SetStats(0.);
   A->SetStats(0.);
   C->SetStats(0.);
   DBMA->SetStats(0.);
   DBMC->SetStats(0.);

}

StatusCode PixelMon2DMaps::regHist(ManagedMonitorToolBase::MonGroup &group)
{
   StatusCode sc = StatusCode::SUCCESS;
   if (group.regHist(IBL2D).isFailure()) sc = StatusCode::FAILURE;
   if (group.regHist(IBL3D).isFailure()) sc = StatusCode::FAILURE;
   if (group.regHist(IBL).isFailure()) sc = StatusCode::FAILURE;
   if (group.regHist(B0).isFailure()) sc = StatusCode::FAILURE;
   if (group.regHist(B1).isFailure()) sc = StatusCode::FAILURE;
   if (group.regHist(B2).isFailure()) sc = StatusCode::FAILURE;
   if (group.regHist(A).isFailure()) sc = StatusCode::FAILURE;
   if (group.regHist(C).isFailure()) sc = StatusCode::FAILURE;
   if (group.regHist(DBMA).isFailure()) sc = StatusCode::FAILURE;
   if (group.regHist(DBMC).isFailure()) sc = StatusCode::FAILURE;
   return sc;
}
