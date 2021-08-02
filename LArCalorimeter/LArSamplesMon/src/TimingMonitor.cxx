/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/TimingMonitor.h"

#include "LArSamplesMon/Interface.h"
#include "LArSamplesMon/History.h"
#include "LArSamplesMon/Data.h"
#include "LArSamplesMon/OFC.h"
#include "LArCafJobs/Geometry.h"

#include "TH1I.h"
#include "TH2D.h"
#include "TF1.h"
#include "TPaveText.h"
#include "TMath.h"
#include <map>
#include <vector>

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;

TH2D** TimingMonitor::timingPlots(PartitionId part, unsigned int tBins, double tMin, double tMax) const
{
  TH2D** hists = new TH2D*[Geo::nFeedThroughs(part)];
  for (int i = 0; i < Geo::nFeedThroughs(part); i++) hists[i] = 0;
  
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);

    if (!history) continue;
    if (history->cellInfo()->partition() != part) continue;
    unsigned int fT = history->cellInfo()->feedThrough();
    if (!hists[fT]) {
      hists[fT] = new TH2D(Form("timing_%d", fT), Form("Timing for %s FT=%d", Id::str(part).Data(), fT), 
			   Geo::nSlots(part), -0.5, Geo::nSlots(part) - 0.5,
			   tBins, tMin, tMax);
      hists[fT]->GetXaxis()->SetTitle("Slot");
      hists[fT]->GetYaxis()->SetTitle("Time [ns]");
    }
    for (unsigned int j = 0; j < history->nData(); j++) 
      hists[fT]->Fill(history->cellInfo()->slot(), history->data(j)->ofcTime());
    
    //   cout << history->data(0)->ofcTime() << "time" << endl;
    
    // cout << history->nData() << " eventi " << history->cellInfo()->slot() << " slot" << endl;

  }
  return hists;
}

TH1F* TimingMonitor::timing(PartitionId part, unsigned int ft, unsigned int FEB, 
                                        unsigned int tBins, double tMin, double tMax) const
{
  
  TH1F* hists;
  //  for (int i = 0; i < Geo::nFeedThroughs(part); i++){
  //  for (int u = 0; u < Geo::nSlots(part); u++){      
  hists= new TH1F(Form("timing_ft_%d,feb_%d",ft,FEB), Form("Timing for FT %d, feb %d", ft,FEB), tBins, tMin, tMax );
  //  }
  //}
  
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history) continue;
    if (history->cellInfo()->partition() != part) continue;
    unsigned int fT = history->cellInfo()->feedThrough();
    unsigned int feb = history->cellInfo()->slot();
    
    for (unsigned int j = 0; j < history->nData(); j++) 
      if (fT==ft && feb==FEB) hists->Fill(history->data(j)->ofcTime());
  }
  return hists;
}


// TH1F** TimingMonitor::time(PartitionId part, unsigned int tBins, double tMin, double tMax) const
// {  
  
//   max=4;
  
//   TH1F** hists = new TH1F*[max];
  

//   //  TH1F* hists[32][14];
//   for (int i = 0; i < Geo::nFeedThroughs(part); i++){
//     for (int u = 0; u < Geo::nSlots(part); u++){      
//       hists[i][u]= new TH1F(Form("timing_ft_%d,feb_%d",i,u), Form("Timing for FT %d, feb %d", i,u), tBins, tMin, tMax );
//     }
//   }
  
//   for (unsigned int i = 0; i < m_interface->nChannels(); i++) {
//     const History* history = m_interface->cellHistory(i);
//     if (!history) continue;
//     if (history->cellInfo()->partition() != part) continue;
//     unsigned int fT = history->cellInfo()->feedThrough();
//     unsigned int feb = history->cellInfo()->slot();
        
//     for (unsigned int j = 0; j < history->nData(); j++) 
//       hists[fT][feb]->Fill(history->data(j)->ofcTime());
//   }
//   return hists;
// }
