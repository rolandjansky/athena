/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/OccupancyMonitor.h"

#include "LArSamplesMon/Interface.h"
#include "LArSamplesMon/History.h"
#include "LArSamplesMon/Data.h"
#include "LArCafJobs/Geometry.h"

#include "TH1I.h"
#include "TH2D.h"
#include <map>
#include <vector>

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


TH1I* OccupancyMonitor::runOccupancyHistory() const
{
  std::map<unsigned int, unsigned int> occ;
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history) continue;
    for (unsigned int k = 0; k < history->nData(); k++)
      occ[history->data(k)->run()]++;
  }
  
  unsigned int runMin = 0;
  unsigned int runMax = 1;
  if (!occ.empty()) {
    runMin = occ.begin()->first;
    runMax = occ.rbegin()->first;
  }

  TH1I* h = new TH1I("occ", "Run occupancies", runMax - runMin + 1, runMin - 0.5, runMax + 0.5);
  h->GetXaxis()->SetTitle("Run number");
  h->GetYaxis()->SetTitle("Occupancy");
  for (std::map<unsigned int, unsigned int>::const_iterator run = occ.begin();
       run != occ.end(); ++run) 
  {
    //cout << event->first << " " << event->second << endl;
    int bin = run->first - runMin + 1;
    h->SetBinContent(bin, run->second);
    h->GetXaxis()->SetBinLabel(bin, Form("%d", run->first));
  }
  return h;
}


TH1I* OccupancyMonitor::eventOccupancyHistory(unsigned int occForDump) const
{
  std::map<std::pair<unsigned int, unsigned int>, unsigned int> occ;
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history) continue;
    for (unsigned int k = 0; k < history->nData(); k++)
      occ[std::make_pair(history->data(k)->run(),history->data(k)->event())]++;
  }
  
  TH1I* h = new TH1I("occ", "Event occupancies", occ.size(), 0, occ.size());
  h->GetXaxis()->SetTitle("Recorded Event");
  h->GetYaxis()->SetTitle("Occupancy");
  
  unsigned int bin = 1;
  for (std::map<std::pair<unsigned int, unsigned int>, unsigned int>::const_iterator event = occ.begin();
       event != occ.end(); ++event, ++bin) 
  {
    if (occForDump > 0 && occForDump <= event->second) 
      cout << "run = " << event->first.first << " event = " << event->first.second << " occ = " << event->second << endl;
    h->SetBinContent(bin, event->second);
  }
  return h;
}


TH1I* OccupancyMonitor::eventOccupancy() const
{
  std::map<unsigned int, unsigned int> occ;
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history) continue;
    for (unsigned int k = 0; k < history->nData(); k++)
      occ[history->data(k)->event()]++;
  }
  
  unsigned int oMin = 999999999, oMax = 0;
  
  for (std::map<unsigned int, unsigned int>::const_iterator event = occ.begin();
       event != occ.end(); ++event) 
  {
    if (event->second < oMin) oMin = event->second;
    if (event->second > oMax) oMax = event->second;
  }
  
  TH1I* h = new TH1I("occ", "Event occupancies", oMax - oMin + 1, oMin - 0.5, oMax + 0.5);
  h->GetXaxis()->SetTitle("Occupancy");
  h->GetYaxis()->SetTitle("Number of events");
  for (std::map<unsigned int, unsigned int>::const_iterator event = occ.begin();
       event != occ.end(); ++event) 
    h->Fill(event->second);
  
  return h;
}


TH1I* OccupancyMonitor::cellOccupancy(int minForPrintout) const
{
  std::vector<unsigned int> occ(nChannels());
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);  
    if (history) {
      occ[i] = occ[i] + history->nData();
    }
  }
  
  unsigned int oMin = 999999999, oMax = 0;
  
  for (std::vector<unsigned int>::const_iterator cell = occ.begin();
       cell != occ.end(); ++cell) 
  {
    if (*cell < oMin) oMin = *cell;
    if (*cell > oMax) oMax = *cell;
  }
  
  //std::map<std::pair<unsigned int, std::string>, unsigned int> names;
  std::map<std::pair<unsigned int, unsigned int>, std::string> names;
  TH1I* h = new TH1I("occ", "Cell occupancies", oMax - oMin + 1, oMin - 0.5, oMax + 0.5);
  h->GetXaxis()->SetTitle("Occupancy");
  h->GetYaxis()->SetTitle("Number of cells");

  unsigned int i = 0;
  for (std::vector<unsigned int>::const_iterator cellOcc = occ.begin();
       cellOcc != occ.end(); ++cellOcc, ++i) {
    h->Fill(*cellOcc);
    const History* history = cellHistory(i);
    if (!history) continue;
    if (minForPrintout >= 0) {
      TString heading = history->cellInfo()->location(2);
      if (history->nData() > 0) heading += " " + history->data(0)->problems();
      names[ std::pair<unsigned int, unsigned int>(*cellOcc, i) ] = heading.Data();
    }
  }
  
  if (minForPrintout >= 0) {
/*    for (std::map<std::pair<unsigned int, std::string>, unsigned int>::reverse_iterator cell = names.rbegin();
    cell != names.rend(); ++cell)
    {
    if (cell->first.first < (unsigned int)minForPrintout) break;
    cout << cell->first.first << " : " << cell->second << ", " << cell->first.second << endl;
  }*/
    for (std::map<std::pair<unsigned int, unsigned int>, std::string>::reverse_iterator cell = names.rbegin();
         cell != names.rend(); ++cell)
    {
      if (cell->first.first < (unsigned int)minForPrintout) break;
      cout << cell->first.first << " : " << cell->first.second << ", " << cell->second << endl;
    }
  }
 
  return h;
}
    

TH1I* OccupancyMonitor::febOccupancy(CaloId calo) const
{
  std::map<unsigned int, unsigned int> occ;
  FilterParams f; 
  f.addCalo(calo);
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = pass(i, f);
    if (!history) continue;
    for (unsigned int k = 0; k < history->nData(); k++)
      occ[history->cellInfo()->feb()]++;
  }
  
  unsigned int oMin = 999999999, oMax = 0;
  
  for (std::map<unsigned int, unsigned int>::const_iterator feb = occ.begin();
       feb != occ.end(); ++feb) 
  {
    if (feb->second < oMin) oMin = feb->second;
    if (feb->second > oMax) oMax = feb->second;
  }
  
  TH1I* h = new TH1I("occ", "FEB occupancies", oMax - oMin + 1, oMin - 0.5, oMax + 0.5);
  h->GetXaxis()->SetTitle("Occupancy");
  h->GetYaxis()->SetTitle("Number of FEBs");
  for (std::map<unsigned int, unsigned int>::const_iterator feb = occ.begin();
       feb != occ.end(); ++feb) 
    h->Fill(feb->second);
  
  return h;
}


TH2D* OccupancyMonitor::partitionOccupancy(PartitionId part) const
{
  TH2D* occ = Geo::partitionHist(part, TString("occupancy_") + Id::str(part), "Occupancies for partition " +  Id::str(part));
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history) continue;
    if (Id::partition(history->cellInfo()->calo()) != part) continue;
    occ->Fill(history->cellInfo()->feb(), history->cellInfo()->channel(), history->nData());
  }

  return occ;
}


TH2D* OccupancyMonitor::etaPhiOccupancy(CaloId calo, short layer, bool useEnergy) const
{
  DataFuncSet func = (useEnergy ? MonitorBase::func("energy") : MonitorBase::func("count"));
  return etaPhiMap(func, DataFuncArgs(), Form("occupancy_%s_%d", Id::str(calo).Data(), layer), 
                   calo, layer, 
                   Form("Occupancies for partition %s, layer %d", Id::str(calo).Data(), layer),
                   TotalValue);                   
}


void OccupancyMonitor::dump(short verbosity) const
{
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history) continue;
    cout << history->cellInfo()->location(verbosity);
    if (verbosity < 5) 
      cout << " : " << history->nData() << " pulse" << endl;
    else {
      cout << " : ";
      for (unsigned int i = 0; i < history->nData(); i++)
	cout << "E = " << history->data(i)->energy() << " MeV, " << endl;
    }
  }
}


void OccupancyMonitor::cellAndRingOccupancy(CaloId calo, unsigned int nMin) const
{
  const unsigned int nPhiRings = 5516; // == Geo::nPhiRings()
  unsigned int nCells[4][3], nRings[4][3];
  unsigned int ringOccupancy[nPhiRings][4][3];
  
  for (unsigned int i = 0; i < 4; i++)
    for (unsigned int j = 0; j < 3; j++) {
      for (unsigned int k = 0; k < nPhiRings; k++) ringOccupancy[k][i][j] = 0;
      nCells[i][j] = 0;
      nRings[i][j] = 0;
    }
    
  for (unsigned int i = 0; i < nChannels(); i++) {
    if (i%10000 == 0) cout << "Processing hash = " << i << endl;
    const History* history = cellHistory(i);
    if (!history) continue;
    if (!Id::matchCalo(history->cellInfo()->calo(), calo)) continue;
    unsigned int n[3] = {0,0,0};
    int layer = history->cellInfo()->layer();
    for (unsigned int k = 0; k < history->nData(); k++) {
      ringOccupancy[history->cellInfo()->globalPhiRing()][layer][(int)history->data(k)->gain()]++;
      n[(int)history->data(k)->gain()]++;
    }
    for (unsigned int g = 0; g < 3; g++) if (n[g] >= nMin) nCells[layer][g]++;
    //cout << n[1] << " " << nCells[layer][1] << " " << layer << endl;
  }
  for (unsigned int i = 0; i < 4; i++)
    for (unsigned int j = 0; j < 3; j++) {
      for (unsigned int k = 0; k < nPhiRings; k++)
        if (ringOccupancy[k][i][j] >= nMin) nRings[i][j]++;
  }
  
  for (unsigned int i = 0; i < 4; i++) {
    cout << "layer " << i << endl;
    for (unsigned int j = 0; j < 3; j++) {
      cout << " gain " << j << " : " 
           << nCells[i][j] << " cells of " << Geo::nChannels(calo,i) << " (" << nCells[i][j]*100.0/Geo::nChannels(calo,i) << " %), " 
           << nRings[i][j] << " rings of " << Geo::nEta(calo,i) << " (" << nRings[i][j]*100.0/Geo::nEta(calo,i) << " %)." << endl;
    }
  }
  for (unsigned int i = 0; i < 4; i++) {
    cout << i << " & " << Geo::nChannels(calo,i) << " & " << Geo::nEta(calo,i) << " & ";
    for (unsigned int j = 0; j < 3; j++)
       cout << Form("%4.1f\\%% & ", nCells[i][j]*100.0/Geo::nChannels(calo,i));
    for (unsigned int j = 0; j < 3; j++)
       cout << Form("%4.1f\\%% & ", nRings[i][j]*100.0/Geo::nEta(calo,i));
    cout << endl;
  }
}
