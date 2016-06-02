/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Function to handle 2D maps of modules, one for each region
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMon2DLumiProfiles.h"
#include "InDetIdentifier/PixelID.h"
//#include "TH2I.h"
#include "TH2F.h"
#include "LWHists/TProfile2D_LW.h"
#include "GaudiKernel/StatusCode.h"     
#include <string.h>

PixelMon2DLumiProfiles::PixelMon2DLumiProfiles(std::string name, std::string title,std::string zlabel,bool doIBL,bool errorHist)
{
  const int lbRange = 3000;
  IBLlbp= TProfile2D_LW::create((name+"_2D_Profile_IBL").c_str(), (title + ", IBL " + title + " (Profile);LB;Module;" + zlabel).c_str(),lbRange,-0.5,-0.5+(float)lbRange,280,-0.5,279.5);
  B0lbp = TProfile2D_LW::create((name+"_2D_Profile_B0").c_str(),  (title + ", B0 "  + title + " (Profile);LB;Module;" + zlabel).c_str(),lbRange,-0.5,-0.5+(float)lbRange,286,-0.5,285.5);
  B1lbp = TProfile2D_LW::create((name+"_2D_Profile_B1").c_str(),  (title + ", B1 "  + title + " (Profile);LB;Module;" + zlabel).c_str(),lbRange,-0.5,-0.5+(float)lbRange,494,-0.5,493.5);
  B2lbp = TProfile2D_LW::create((name+"_2D_Profile_B2").c_str(),  (title + ", B2 "  + title + " (Profile);LB;Module;" + zlabel).c_str(),lbRange,-0.5,-0.5+(float)lbRange,676,-0.5,675.5);
  Albp  = TProfile2D_LW::create((name+"_2D_Profile_ECA" ).c_str(),(title + ", ECA " + title + " (Profile);LB;Module;" + zlabel).c_str(),lbRange,-0.5,-0.5+(float)lbRange,144,-0.5,143.5);
  Clbp  = TProfile2D_LW::create((name+"_2D_Profile_ECC" ).c_str(),(title + ", ECC " + title + " (Profile);LB;Module;" + zlabel).c_str(),lbRange,-0.5,-0.5+(float)lbRange,144,-0.5,143.5);

  formatHist(doIBL,errorHist);
}

PixelMon2DLumiProfiles::~PixelMon2DLumiProfiles()
{
   LWHist::safeDelete(IBLlbp);
   LWHist::safeDelete(B0lbp);
   LWHist::safeDelete(B1lbp);
   LWHist::safeDelete(B2lbp);
   LWHist::safeDelete(Albp);
   LWHist::safeDelete(Clbp);
}

void PixelMon2DLumiProfiles::Fill(double LB,Identifier &id, const PixelID* pixID,double weight,bool doIBL,bool errorHist)
{
   int bec = pixID->barrel_ec(id);
   int ld  = pixID->layer_disk(id);
   int pm  = pixID->phi_module(id);

   if(bec==2){
      Albp->Fill(LB,ld*48+pm,weight);
   }
   else if(bec==-2){
      Clbp->Fill(LB,ld*48+pm,weight);
   }
   else if(bec==0)
   {
     if(doIBL){ld--;}
      int em  = pixID->eta_module(id)+6;
      if(ld ==0){
         B0lbp->Fill(LB,em+13*pm,weight);
      }
      else if(ld ==1){
         B1lbp->Fill(LB,em+13*pm,weight);
      }
      else if(ld ==2){
         B2lbp->Fill(LB,em+13*pm,weight);
      }
      else if(ld ==-1 && !errorHist && doIBL){
	IBLlbp->Fill(LB,em+4+20*pm,weight);
      }
   }
}


void PixelMon2DLumiProfiles::formatHist(bool doIBL,bool errorHist)
{
   const int ndisk = 3;
   const int nphi  = 48;
   const char *disk[ndisk] = { "D1", "D2", "D3" };
   const int nmod = 13;
   const int nmodIBL = 20;
   const char *mod[nmod] = { "M6C", "M5C", "M4C", "M3C", "M2C", "M1C", "M0", "M1A", "M2A", "M3A", "M4A", "M5A", "M6A" } ;
   const char *modIBL[nmodIBL] = { "M4_C8_2","M4_C8_1","M4_C7_2","M4_C7_1","M3_C6", "M3_C5", "M2_C4", "M1_C3", "M1_C2", "M1_C1", "M1_A1", "M1_A2", "M2_A3", "M2_A4", "M3_A5", "M3_A6","M4_A7_1","M4_A7_2","M4_A8_1","M4_A8_2"};
   char label[30];

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

   int count = 1;
   for (int j=0;j<ndisk;j++){
      for (int i=0; i<nphi; i++){
         sprintf(label,"%sA_%s",disk[j],nstaveA[i]);
         Albp->GetYaxis()->SetBinLabel( count, label );
         sprintf(label,"%sC_%s",disk[j],nstaveC[i]);
         Clbp->GetYaxis()->SetBinLabel( count, label );
         count++;
      }
   }
   count = 1;
   for (int i=0; i<nstave0; i++){
      for (int j=0; j<nmod; j++){
         sprintf(label,"L0_%s_%s",stave0[i],mod[j]);
         B0lbp->GetYaxis()->SetBinLabel( count, label ); 
         count++;
      }
   }
   count = 1;
   for (int i=0; i<nstave1; i++) 
   {
      for (int j=0; j<nmod; j++){
         sprintf(label,"L1_%s_%s",stave1[i],mod[j]);
         B1lbp->GetYaxis()->SetBinLabel( count,label );
         count++; 
      }
   }
   count = 1;
   for (int i=0; i<nstave2; i++) 
   {
      for (int j=0; j<nmod; j++){
         sprintf(label,"L2_%s_%s",stave2[i],mod[j]);
         B2lbp->GetYaxis()->SetBinLabel( count, label );
         count++;
      } 
   }
   count = 1;
   if(!errorHist && doIBL){
     for (int i=0; i<nstaveb; i++)
       {
	 for (int j=0; j<nmodIBL; j++){
	   sprintf(label,"IBL_%s_%s",staveb[i],modIBL[j]);
	   IBLlbp->GetYaxis()->SetBinLabel( count, label );
	   count++;
	 }
       }
   }

   if(!errorHist && doIBL){
     IBLlbp->GetYaxis()->SetLabelSize(0.03);
     IBLlbp->SetOption("colz");
   }

   //Make the text smaller
   B0lbp->GetYaxis()->SetLabelSize(0.03);
   B1lbp->GetYaxis()->SetLabelSize(0.03);
   B2lbp->GetYaxis()->SetLabelSize(0.03);
    Albp->GetYaxis()->SetLabelSize(0.02);
    Clbp->GetYaxis()->SetLabelSize(0.02);
   //put histograms in the easier to read colz format
    B0lbp->SetOption("colz");
   B1lbp->SetOption("colz");
   B2lbp->SetOption("colz");
    Albp->SetOption("colz");
    Clbp->SetOption("colz");
   //force the minimum to be 0 so you can spot empty blocks easily
//   B0lbp->SetMinimum(0.);
//   B1lbp->SetMinimum(0.);
//   B2lbp->SetMinimum(0.);
//    Albp->SetMinimum(0.);
//    Clbp->SetMinimum(0.);
    //IBLlbp->SetStats(0.);
    //B0lbp->SetStats(0.);
    //B1lbp->SetStats(0.);
    //B2lbp->SetStats(0.);
    //Albp->SetStats(0.);
    //Clbp->SetStats(0.);
}

StatusCode PixelMon2DLumiProfiles::regHist(ManagedMonitorToolBase::MonGroup &group,bool doIBL,bool errorHist)
{
  if(!errorHist && doIBL)sc = group.regHist(IBLlbp);
   sc = group.regHist(B0lbp);
   sc = group.regHist(B1lbp);
   sc = group.regHist(B2lbp);
   sc = group.regHist(Albp);
   sc = group.regHist(Clbp);
   
   return sc;
}
