/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Function to handle 2D profile histograms of modules, one for each region
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMonProfiles.h"
#include "InDetIdentifier/PixelID.h"
//#include "TProfile2D.h"
#include "LWHists/TProfile2D_LW.h"
#include "GaudiKernel/StatusCode.h"     
#include <string.h>

PixelMonProfiles::PixelMonProfiles(std::string name, std::string title )
{

  //IBL3D = new TProfile2D((name+"_IBL3D").c_str(),(title + ", IBL 3D module " + ";eta index of module;phi index of module").c_str(),8,-.5,7.5,14,-0.5,13.5);
  //IBL2D = new TProfile2D((name+"_IBL2D").c_str(),(title + ", IBL planar module " + ";eta index of module;phi index of module").c_str(),12,-6.5,5.5,14,-0.5,13.5);
  //IBL   = new TProfile2D((name+"_IBL").c_str(),  (title + ", IBL " + ";eta index of module;phi index of module").c_str(), 32, -16.5, 15.5, 14, -0.5, 13.5);
  //B0    = new TProfile2D((name+"_B0").c_str(),   (title + ", B0 " + ";eta index of module;phi index of module").c_str(),13,-6.5,6.5,22,-0.5,21.5);
  //B1    = new TProfile2D((name+"_B1").c_str(),   (title + ", B1 " + ";eta index of module;phi index of module").c_str(),13,-6.5,6.5,38,-0.5,37.5);
  //B2    = new TProfile2D((name+"_B2").c_str(),   (title + ", B2 " + ";eta index of module;phi index of module").c_str(),13,-6.5,6.5,52,-0.5,51.5);
  //A     = new TProfile2D((name+"_ECA" ).c_str(), (title + ", ECA "+ ";disk number;phi index of module").c_str(),         3,-0.5,2.5,48,-0.5,47.5);
  //C     = new TProfile2D((name+"_ECC" ).c_str(), (title + ", ECC "+ ";disk number;phi index of module").c_str(),         3,-0.5,2.5,48,-0.5,47.5);
  IBL3D = TProfile2D_LW::create((name+"_IBL3D").c_str(),(title + ", IBL 3D module " + ";eta index of module;phi index of module").c_str(),8,-.5,7.5,14,-0.5,13.5);
  IBL2D = TProfile2D_LW::create((name+"_IBL2D").c_str(),(title + ", IBL planar module " + ";eta index of module;phi index of module").c_str(),12,-6.5,5.5,14,-0.5,13.5);
  IBL   = TProfile2D_LW::create((name+"_IBL").c_str(),  (title + ", IBL " + ";eta index of module;phi index of module").c_str(), 32, -16.5, 15.5, 14, -0.5, 13.5);
  B0    = TProfile2D_LW::create((name+"_B0").c_str(),   (title + ", B0 " + ";eta index of module;phi index of module").c_str(),13,-6.5,6.5,22,-0.5,21.5);
  B1    = TProfile2D_LW::create((name+"_B1").c_str(),   (title + ", B1 " + ";eta index of module;phi index of module").c_str(),13,-6.5,6.5,38,-0.5,37.5);
  B2    = TProfile2D_LW::create((name+"_B2").c_str(),   (title + ", B2 " + ";eta index of module;phi index of module").c_str(),13,-6.5,6.5,52,-0.5,51.5);
  A     = TProfile2D_LW::create((name+"_ECA" ).c_str(), (title + ", ECA "+ ";disk number;phi index of module").c_str(),         3,-0.5,2.5,48,-0.5,47.5);
  C     = TProfile2D_LW::create((name+"_ECC" ).c_str(), (title + ", ECC "+ ";disk number;phi index of module").c_str(),         3,-0.5,2.5,48,-0.5,47.5);


  formatHist();
  
}

PixelMonProfiles::~PixelMonProfiles()
{
   //delete IBL3D;
   //delete IBL2D;
   //delete IBL;
   //delete B0;
   //delete B1;
   //delete B2;
   //delete A;
   //delete C;
   LWHist::safeDelete(IBL3D);
   LWHist::safeDelete(IBL2D);
   LWHist::safeDelete(IBL);
   LWHist::safeDelete(B0);
   LWHist::safeDelete(B1);
   LWHist::safeDelete(B2);
   LWHist::safeDelete(A);
   LWHist::safeDelete(C);
}

void PixelMonProfiles::SetMaxValue(float max)
{
   IBL->SetMaximum( max );
   IBL2D->SetMaximum( max );
   IBL3D->SetMaximum( max );
   B0->SetMaximum( max );
   B1->SetMaximum( max );
   B2->SetMaximum( max );
   A->SetMaximum( max ); 
   C->SetMaximum( max ); 
}

void PixelMonProfiles::Fill(Identifier &id, const PixelID* pixID, float Index, bool doIBL)
{
   int bec = pixID->barrel_ec(id);
   int ld  = pixID->layer_disk(id);
   int pm  = pixID->phi_module(id);
   
   if(bec==2) A->Fill(ld,pm,Index);
   else if(bec==-2) C->Fill(ld,pm,Index);
   else if(bec==0)
   {
      if(doIBL){ld--;}
      int em  = pixID->eta_module(id);
      if(ld ==0) B0->Fill(em,pm,Index);
      else if(ld ==1) B1->Fill(em,pm,Index);
      else if(ld ==2) B2->Fill(em,pm,Index);
      else if(ld ==-1){
	   int feid = 0;
	   int emf = 0;
      bool dep = false;
	if(em<6 && em>-7){
	   if(pixID->eta_index(id) >= 80) feid = 1;
	   emf = 2 * em + feid; 
	   IBL2D->Fill(em,pm,Index);
      dep = true;
	}else if(em<-6){
	   emf = em - 6;
	   IBL3D->Fill(em+10,pm,Index);
   }else{
	   emf = em + 6;
	   IBL3D->Fill(em-2,pm,Index);
	}
   //std::cout << "!!! " << emf << " " << pm << " " << Index << std::endl;
	IBL->Fill(emf, pm, Index);
	if(dep) IBL->Fill(emf+1, pm, Index); /// Temporary!!!! should be removed later
      }
   }
}

void PixelMonProfiles::formatHist()
{
  const int ndisk = 3;
  const int nphi  = 48;
  const char *disk[ndisk] = { "Disk 1", "Disk 2", "Disk 3" };
  const int nmod = 13;
  const int nmodIBL2D = 12;
  const int nmodIBL3D = 8;
  const int nmodIBL = 32;
  const char *mod[nmod] = { "M6C", "M5C", "M4C", "M3C", "M2C", "M1C","M0", "M1A", "M2A", "M3A", "M4A", "M5A", "M6A" } ;
  const char *modIBL2D[nmodIBL2D] = { "M3_C6", "M3_C5", "M2_C4", "M1_C3", "M1_C2", "M1_C1", "M1_A1", "M1_A2", "M2_A3", "M2_A4", "M3_A5", "M3_A6" } ;
  const char *modIBL3D[nmodIBL3D] = {"M4_C8_2","M4_C8_1","M4_C7_2","M4_C7_1","M4_A7_1","M4_A7_2","M4_A8_1","M4_A8_2"};

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
   for (int i=0; i<ndisk; i++) 
   {
      A->GetXaxis()->SetBinLabel( i+1, disk[i] );
      C->GetXaxis()->SetBinLabel( i+1, disk[i] );
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
   //Move the lable so you can read it
   //IBL2D->GetYaxis()->SetTitleOffset(1.35);
   //IBL3D->GetYaxis()->SetTitleOffset(1.35);
   //IBL->GetYaxis()->SetTitleOffset(1.35);
   //B0->GetYaxis()->SetTitleOffset(1.35);
   //B1->GetYaxis()->SetTitleOffset(1.35);
   //B2->GetYaxis()->SetTitleOffset(1.35);
   //A->GetYaxis()->SetTitleOffset(1.35);
   //C->GetYaxis()->SetTitleOffset(1.35);
   //put histograms in the easier to read colz format
   IBL->SetOption("colz");
   IBL2D->SetOption("colz");
   IBL3D->SetOption("colz");
   B0->SetOption("colz");
   B1->SetOption("colz");
   B2->SetOption("colz");
   A->SetOption("colz");
   C->SetOption("colz");
   //force the minimum to be 0 so you can spot empty blocks easily
   IBL->SetMinimum(0.);   
   IBL2D->SetMinimum(0.);
   IBL3D->SetMinimum(0.);
   B0->SetMinimum(0.);     
   B1->SetMinimum(0.);   
   B2->SetMinimum(0.);   
   A->SetMinimum(0.);     
   C->SetMinimum(0.);    
   //Remvoe the stats box because it's in the way
   //IBL->SetStats(0.);
   //IBL2D->SetStats(0.);
   //IBL3D->SetStats(0.);
   //B0->SetStats(0.);
   //B1->SetStats(0.);
   //B2->SetStats(0.);
   //A->SetStats(0.);
   //C->SetStats(0.);
   
}

StatusCode PixelMonProfiles::regHist(ManagedMonitorToolBase::MonGroup &group)
{
   sc = group.regHist(IBL2D); 
   sc = group.regHist(IBL3D); 
   sc = group.regHist(IBL); 
   sc = group.regHist(B0);
   sc = group.regHist(B1);
   sc = group.regHist(B2);
   sc = group.regHist(A);
   sc = group.regHist(C);
   
   return sc;
}
