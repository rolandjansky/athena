/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// Function to handle 2D maps of modules, one for each region
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/DBMMon2DMaps.h"
#include "InDetIdentifier/PixelID.h"
//#include "TH2I.h"
#include "TH2F.h"
#include "GaudiKernel/StatusCode.h"     
#include <string.h>

DBMMon2DMaps::DBMMon2DMaps(std::string name, std::string title)

{
  DBMA = new TH2F((name+"_A").c_str(),("Side A " + title + "; layer number; phi index of telescope").c_str(),3,-0.5,2.5,4,-.5,3.5);
  DBMC = new TH2F((name+"_C").c_str(),("Side C " + title + "; layer number; phi index of telescope").c_str(),3,-0.5,2.5,4,-.5,3.5);

  formatHist();
}

DBMMon2DMaps::~DBMMon2DMaps()
{
  delete DBMA;
  delete DBMC;
}

void DBMMon2DMaps::Fill(Identifier &id, const PixelID* pixID)
{
  int bec = pixID->barrel_ec(id);
   int ld  = pixID->layer_disk(id);
   int pm  = pixID->phi_module(id);

   if(bec==4){
     DBMA->Fill(ld,pm);
   }
   if(bec==-4){
     DBMC->Fill(ld,pm);
   }
}   


void DBMMon2DMaps::Scale (double number)
{
   if (number==0) return; //shouldn't happen the way function is called, but dummy check to avoid divide by zero

   DBMA->Scale((float) 1.0/number);
   DBMC->Scale((float) 1.0/number);
}

void DBMMon2DMaps::formatHist()
{
   const int ndisk = 3;
   const int nphi  = 4;
    const char *phi[nphi] = { "M1","M2","M3","M4"};
   const char *disk[ndisk] = { "Disk 0","Disk 1","Disk 2"};

   for (int i=0; i<nphi; i++) 
   {
      DBMA->GetYaxis()->SetBinLabel( i+1, phi[i] );
      DBMC->GetYaxis()->SetBinLabel( i+1, phi[i] );
   }
   for (int i=0; i<ndisk; i++) 
   {
      DBMA->GetXaxis()->SetBinLabel( i+1, disk[i] );
      DBMC->GetXaxis()->SetBinLabel( i+1, disk[i] );
   }

   //Make the text smaller
   DBMA->GetYaxis()->SetLabelSize(0.04);
   DBMC->GetYaxis()->SetLabelSize(0.04);
   DBMA->GetXaxis()->SetLabelSize(0.04);
   DBMC->GetXaxis()->SetLabelSize(0.04);
   //Move the lable so you can read it
   DBMA->GetYaxis()->SetTitleOffset(1.35);
   DBMC->GetYaxis()->SetTitleOffset(1.35);
   //put histograms in the easier to read colz format
   DBMA->SetOption("colz");
   DBMC->SetOption("colz");
   //force the minimum to be 0 so you can spot empty blocks easily
   DBMA->SetMinimum(0.);
   DBMC->SetMinimum(0.);
   //Remvoe the stats box because it's in the way
   DBMA->SetStats(0.);
   DBMC->SetStats(0.);
}

StatusCode DBMMon2DMaps::regHist(ManagedMonitorToolBase::MonGroup &group)
{
   StatusCode sc = StatusCode::SUCCESS;
   if (group.regHist(DBMA).isFailure()) sc = StatusCode::FAILURE;
   if (group.regHist(DBMC).isFailure()) sc = StatusCode::FAILURE;
  
   return sc;
}
