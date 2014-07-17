/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Function to handle 2D maps of modules, one for each region
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMon2DLumiMaps.h"
#include "InDetIdentifier/PixelID.h"
//#include "TH2I.h"
#include "LWHists/TH2F_LW.h"
#include "GaudiKernel/StatusCode.h"     
#include <string.h>

PixelMon2DLumiMaps::PixelMon2DLumiMaps(std::string name, std::string title,std::string zlabel)
{
  IBLlbm = TH2F_LW::create((name+"_2D_Map_IBL").c_str(),("IBL " + title + " (Map);LB;Module;" + zlabel).c_str(),2500,-0.5,2499.5,224,-0.5,223.5);
  B0lbm = TH2F_LW::create((name+"_2D_Map_B0").c_str(),("Barrel layer 0 " + title + " (Map);LB;Module;" + zlabel).c_str(),2500,-0.5,2499.5,286,-0.5,285.5);
  B1lbm = TH2F_LW::create((name+"_2D_Map_B1").c_str(),("Barrel layer 1 " + title + " (Map);LB;Module;" + zlabel).c_str(),2500,-0.5,2499.5,494,-0.5,493.5);
  B2lbm = TH2F_LW::create((name+"_2D_Map_B2").c_str(),("Barrel layer 2 " + title + " (Map);LB;Module;" + zlabel).c_str(),2500,-0.5,2499.5,676,-0.5,675.5);
  Albm  = TH2F_LW::create((name+"_2D_Map_A" ).c_str(),("ECA "            + title + " (Map);LB;Module;" + zlabel).c_str(),2500,-0.5,2499.5,144,-0.5,143.5);
  Clbm  = TH2F_LW::create((name+"_2D_Map_C" ).c_str(),("ECC "            + title + " (Map);LB;Module;" + zlabel).c_str(),2500,-0.5,2499.5,144,-0.5,143.5);

  formatHist();
}

PixelMon2DLumiMaps::~PixelMon2DLumiMaps()
{
   LWHist::safeDelete(IBLlbm);
   LWHist::safeDelete(B0lbm);
   LWHist::safeDelete(B1lbm);
   LWHist::safeDelete(B2lbm);
   LWHist::safeDelete(Albm);
   LWHist::safeDelete(Clbm);
}

void PixelMon2DLumiMaps::Fill(double LB,Identifier &id, const PixelID* pixID,double weight, bool doIBL)
{
   int bec = pixID->barrel_ec(id);
   int ld  = pixID->layer_disk(id);
   int pm  = pixID->phi_module(id);

   if(bec==2){
      Albm->Fill(LB,ld*48+pm,weight);
   }
   else if(bec==-2){
      Clbm->Fill(LB,ld*48+pm,weight);
   }
   else if(bec==0)
   {
     if(doIBL){ld--;}
      int em  = pixID->eta_module(id)+6;
      if(ld ==0){
         B0lbm->Fill(LB,em+13*pm,weight);
      }
      else if(ld ==1){
         B1lbm->Fill(LB,em+13*pm,weight);
      }
      else if(ld ==2){
         B2lbm->Fill(LB,em+13*pm,weight);
      }
      else if(ld ==-1){
	IBLlbm->Fill(LB,em+2+16*pm,weight);
      }
   }
}

void PixelMon2DLumiMaps::Scale (double number)
{
   if (number==0) return; //shouldn't happen the way function is called, but dummy check to avoid divide by zero

   Albm->scaleContentsAndErrors((float) 1.0/number);
   Clbm->scaleContentsAndErrors((float) 1.0/number);
   B0lbm->scaleContentsAndErrors((float) 1.0/number);
   B1lbm->scaleContentsAndErrors((float) 1.0/number);
   B2lbm->scaleContentsAndErrors((float) 1.0/number);
   IBLlbm->scaleContentsAndErrors((float) 1.0/number);

}

void PixelMon2DLumiMaps::formatHist()
{
   const int ndisk = 3;
   const int nphi  = 48;
   const char *disk[ndisk] = { "D1", "D2", "D3" };
   const int nmod = 13;
   const int nmodIBL = 16;
   const char *mod[nmod] = { "M6C", "M5C", "M4C", "M3C", "M2C", "M1C", "M0", "M1A", "M2A", "M3A", "M4A", "M5A", "M6A" } ;
   const char *modIBL[nmodIBL] = {"M8C", "M7C", "M6C", "M5C", "M4C", "M3C", "M2C", "M1C", "M0", "M1A", "M2A", "M3A", "M4A", "M5A", "M6A", "M7A" } ;
   char label[30];

   const int nstaveb = 14;
   const char *staveb[nstaveb] = {
     "B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2",
     "B04_S1", "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2",
     "B07_S1", "B07_S2"};
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
         Albm->GetYaxis()->SetBinLabel( count, label );
         sprintf(label,"%sC_%s",disk[j],nstaveC[i]);
         Clbm->GetYaxis()->SetBinLabel( count, label );
         count++;
      }
   }
   count = 1;
   for (int i=0; i<nstave0; i++){
      for (int j=0; j<nmod; j++){
         sprintf(label,"L0_%s_%s",stave0[i],mod[j]);
         B0lbm->GetYaxis()->SetBinLabel( count, label ); 
         count++;
      }
   }
   count = 1;
   for (int i=0; i<nstave1; i++) 
   {
      for (int j=0; j<nmod; j++){
         sprintf(label,"L1_%s_%s",stave1[i],mod[j]);
         B1lbm->GetYaxis()->SetBinLabel( count,label );
         count++; 
      }
   }
   count = 1;
   for (int i=0; i<nstave2; i++) 
   {
      for (int j=0; j<nmod; j++){
         sprintf(label,"L2_%s_%s",stave2[i],mod[j]);
         B2lbm->GetYaxis()->SetBinLabel( count, label );
         count++;
      } 
   }
   count = 1;
   for (int i=0; i<nstaveb; i++)
     {
       for (int j=0; j<nmodIBL; j++){
         sprintf(label,"IBL_%s_%s",staveb[i],modIBL[j]);
         IBLlbm->GetYaxis()->SetBinLabel( count, label );
         count++;
       }
     }


   //Make the text smaller
   IBLlbm->GetYaxis()->SetLabelSize(0.04);
   B0lbm->GetYaxis()->SetLabelSize(0.04);
   B1lbm->GetYaxis()->SetLabelSize(0.04);
   B2lbm->GetYaxis()->SetLabelSize(0.04);
   Albm->GetYaxis()->SetLabelSize(0.04);
   Clbm->GetYaxis()->SetLabelSize(0.04);
   //put histograms in the easier to read colz format
   IBLlbm->SetOption("colz");
   B0lbm->SetOption("colz");
   B1lbm->SetOption("colz");
   B2lbm->SetOption("colz");
   Albm->SetOption("colz");
   Clbm->SetOption("colz");
   //force the minimum to be 0 so you can spot empty blocks easily
//   B0lbm->SetMinimum(0.);
//   B1lbm->SetMinimum(0.);
//   B2lbm->SetMinimum(0.);
//    Albm->SetMinimum(0.);
//    Clbm->SetMinimum(0.);

}

StatusCode PixelMon2DLumiMaps::regHist(ManagedMonitorToolBase::MonGroup &group)
{
   sc = group.regHist(IBLlbm);
   sc = group.regHist(B0lbm);
   sc = group.regHist(B1lbm);
   sc = group.regHist(B2lbm);
   sc = group.regHist(Albm);
   sc = group.regHist(Clbm);
   
   return sc;
}
