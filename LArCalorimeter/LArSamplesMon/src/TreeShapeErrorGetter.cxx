/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/TreeShapeErrorGetter.h"

#include "TH2D.h"
#include "TChain.h"
#include "LArSamplesMon/Residual.h"
#include "LArCafJobs/Geometry.h"
#include "LArSamplesMon/Interface.h"

#include <iomanip>
#include <fstream>
#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


TreeShapeErrorGetter::TreeShapeErrorGetter(const TString& fileName, bool recreate)
 : m_file(nullptr), m_cellTrees(3), m_ringTrees(3), m_cellCalc(nullptr), m_ringCalc(nullptr)
{
  m_file = TFile::Open(fileName, (recreate ? "RECREATE" : "READ"));
  if (!m_file || !m_file->IsOpen()) {
    cout << "File " << fileName << " is not accessible" << endl;
    return;
  }
  if (recreate) {
    m_cellTrees[0] = new TTree("cells_HIGH", "");
    m_cellTrees[1] = new TTree("cells_MED",  "");
    m_cellTrees[2] = new TTree("cells_LOW",  "");
    m_ringTrees[0] = new TTree("rings_HIGH", "");
    m_ringTrees[1] = new TTree("rings_MED",  "");
    m_ringTrees[2] = new TTree("rings_LOW",  "");
    for (unsigned int i = 0; i < 3; i++ ) {
      m_cellTrees[i]->Branch("calc", &m_cellCalc, 32000, 0);
      m_ringTrees[i]->Branch("calc", &m_ringCalc, 32000, 0);
    }
  }
  else {
    m_cellTrees[0] = (TTree*)m_file->Get("cells_HIGH");
    m_cellTrees[1] = (TTree*)m_file->Get("cells_MED");
    m_cellTrees[2] = (TTree*)m_file->Get("cells_LOW");
    if (!m_cellTrees[0] || !m_cellTrees[1] || !m_cellTrees[2]) {
      cout << "Trees with cell shape error data not found in " << fileName << endl;
      return;
    }
    m_ringTrees[0] = (TTree*)m_file->Get("rings_HIGH");
    m_ringTrees[1] = (TTree*)m_file->Get("rings_MED");
    m_ringTrees[2] = (TTree*)m_file->Get("rings_LOW");
    if (!m_ringTrees[0] || !m_ringTrees[1] || !m_ringTrees[2]) {
      cout << "Trees with phi symmetric shape error data not found in " << fileName << endl;
      return;
    }
    for (unsigned int i = 0; i < 3; i++ ) {
      m_cellTrees[i]->SetBranchAddress("calc", &m_cellCalc);
      m_ringTrees[i]->SetBranchAddress("calc", &m_ringCalc);
    }
  }
}
      

TreeShapeErrorGetter::~TreeShapeErrorGetter()
{
  if (m_file->GetOption() == TString("CREATE")) {
    if (!m_file) {
      cout << "No file, error writing data to TreeShapeErrorGetter" << endl;
      return;
    }
    m_file->cd();
    for (unsigned int g = 0; g < 3; g++ ) {
      if (!m_cellTrees[g] || !m_ringTrees[g]) {
        cout << "Missing trees, error writing data to TreeShapeErrorGetter" << endl;
        return;
      }
      m_cellTrees[g]->Write();
      m_ringTrees[g]->Write();
    }
  }
  delete m_file;
}


TTree* TreeShapeErrorGetter::cellTree(CaloGain::CaloGain gain) const
{
  switch (gain) {
    case CaloGain::LARHIGHGAIN   : return m_cellTrees[0];
    case CaloGain::LARMEDIUMGAIN : return m_cellTrees[1];
    case CaloGain::LARLOWGAIN    : return m_cellTrees[2];
    default : return nullptr;
  }
  return nullptr;
}


TTree* TreeShapeErrorGetter::ringTree(CaloGain::CaloGain gain) const
{
  switch (gain) {
    case CaloGain::LARHIGHGAIN   : return m_ringTrees[0];
    case CaloGain::LARMEDIUMGAIN : return m_ringTrees[1];
    case CaloGain::LARLOWGAIN    : return m_ringTrees[2];
    default : return nullptr;
  }
  return nullptr;
}


ShapeErrorData* TreeShapeErrorGetter::shapeErrorData(unsigned int hash, CaloGain::CaloGain gain, const Residual* toExclude) const
{
  if (!cellTree(gain) || hash >= cellTree(gain)->GetEntries()) return nullptr;
  cellTree(gain)->GetEntry(hash);

  if (toExclude) m_cellCalc->remove(*toExclude);
  if (m_cellCalc->size() < 2) return nullptr;
  return m_cellCalc->shapeErrorData();
}


ShapeErrorData* TreeShapeErrorGetter::phiSymShapeErrorData(short ring, CaloGain::CaloGain gain, const Residual* toExclude) const
{ 
  if (!ringTree(gain) || ring >= ringTree(gain)->GetEntries()) return nullptr;
  ringTree(gain)->GetEntry(ring);

  if (toExclude) m_ringCalc->remove(*toExclude);
  if (m_ringCalc->size() < 2) return nullptr;
  return m_ringCalc->shapeErrorData();
}


int TreeShapeErrorGetter::addCell(const ResidualCalculator& calc, CaloGain::CaloGain gain)
{
  if (!cellTree(gain)) return -1;
  if (m_cellCalc) delete m_cellCalc;
  m_cellCalc = new ResidualCalculator(calc);
  cellTree(gain)->Fill();
  return cellTree(gain)->GetEntries();
}


int TreeShapeErrorGetter::addRing(const ResidualCalculator& calc, CaloGain::CaloGain gain)
{
  if (!ringTree(gain)) return 0;
  if (m_ringCalc) delete m_ringCalc;
  m_ringCalc = new ResidualCalculator(calc);
  ringTree(gain)->Fill();
  return ringTree(gain)->GetEntries();
}


void TreeShapeErrorGetter::dump(CaloGain::CaloGain gain) const
{
  if (!cellTree(gain)) return;
  for (long long i = 0; i < cellTree(gain)->GetEntries(); i++) {
    ShapeErrorData* sed = shapeErrorData(i, gain);
    if (!sed) continue;
    cout << "-> " << i << endl;
    sed->xi().Print();
    sed->xip().Print();
    delete sed;
  }
}

TH2D* TreeShapeErrorGetter::correlate(const TreeShapeErrorGetter& other, CaloGain::CaloGain gain, unsigned short sample, bool xip, 
                                       unsigned int nBins, double xMin, double xMax) const
{
  TH2D* h = new TH2D(Form("%s_%d", xip ? "xip" : "xi", sample), "", nBins, xMin, xMax, nBins, xMin, xMax);
  for (long long i = 0; i < Definitions::nChannels; i++) {
    ShapeErrorData* data1 = shapeErrorData(i, gain);
    if (!data1) continue;
    ShapeErrorData* data2 = other.shapeErrorData(i, gain);
    if (!data2) { delete data1; continue; }
    cout << i << endl;
    unsigned int sample1 = sample + data1->xip().GetLwb(); // sample indices may not match, but we assume the range covered for the reference shape is the same
    unsigned int sample2 = sample + data2->xip().GetLwb();
    if (data1->isInRange(sample1) && data2->isInRange(sample2))
      h->Fill(xip ? data1->xip()(sample1) : data1->xi()(sample1), 
              xip ? data2->xip()(sample2) : data2->xi()(sample2));
    delete data1;
    delete data2;
  }
  return h;
} 


bool TreeShapeErrorGetter::merge(const TString& listFile, const TString& outputFile)
{
  std::ifstream f(listFile);
  if (!f) {
    cout << "file " << listFile << " not accessible" << endl;
    return 0;
  }
  
  std::string fileName;  
  unsigned int i = 0;
  std::vector<const TreeShapeErrorGetter*> getters;
  
  while (f >> fileName) {
    //gSystem->Exec("free");
    const TreeShapeErrorGetter* getter = new TreeShapeErrorGetter(fileName.c_str());
    if (!getter) {
      cout << "Skipping invalid file " << fileName << endl;
      continue;
    }
    cout << std::setw(2) << ++i << " - " << fileName << endl;
    getters.push_back(getter);
  }
  bool result = merge(getters, outputFile);

  for (const TreeShapeErrorGetter* getter : getters)
     delete getter;
  
  return result;
}


bool TreeShapeErrorGetter::merge(const std::vector<const TreeShapeErrorGetter*>& getters, const TString& outputFile)
{
  TreeShapeErrorGetter* output = new TreeShapeErrorGetter(outputFile, true);
  for (unsigned int i = 0; i < Definitions::nChannels; i++) {
    for (unsigned int g = 0; g < 3; g++) {
      bool gotResult = false;
      for (const TreeShapeErrorGetter* getter : getters) {
        if (getter->shapeErrorData(i, (CaloGain::CaloGain)g)) {
          if (gotResult) {
            cout << "TreeShapeErrorGetter::merge : input getters have non-zero overlap for cell " << i << " -- not supported, exiting." << endl;
            return false;          
          }
          gotResult = true;
          cout << "Adding " << i << " " << g << " " << getter->cellCalc()->size() << endl;
          output->addCell(*getter->cellCalc(), (CaloGain::CaloGain)g);
        }
      }
      if (!gotResult) output->addCell(ResidualCalculator(), (CaloGain::CaloGain)g);
    }
  }
  
  for (int i = 0; i < Geo::nPhiRings(); i++) {
    for (unsigned int g = 0; g < 3; g++) {
      bool gotResult = false;
      for (const TreeShapeErrorGetter* getter : getters) {
        if (getter->phiSymShapeErrorData(i, (CaloGain::CaloGain)g)) {
          if (gotResult) {
            cout << "TreeShapeErrorGetter::merge : input getters have non-zero overlap for ring " << i << " -- not supported, exiting." << endl;
            return false;    
          }
          gotResult = true;
          cout << "Adding ring " << i << " " << g << endl;
          output->addRing(*getter->ringCalc(), (CaloGain::CaloGain)g);
        }        
      }
      if (!gotResult) output->addRing(ResidualCalculator(), (CaloGain::CaloGain)g);
    }
  }
 
  delete output;
  return true; 
}


bool TreeShapeErrorGetter::compare(const TreeShapeErrorGetter& other, const TString& fileName, const Interface* tmpl) const
{
  TFile* f = TFile::Open(fileName, "RECREATE");
  TTree* tree = new TTree("tree", "");

  int hash, gain, lwb1, lwb2, nSamples;
  double xi1[99], xi2[99], xip1[99], xip2[99];
  int calo;
  unsigned int layer, ft, slot, channel;
  double eta, phi;

  tree->Branch("hash", &hash);

  if (tmpl) {
    tree->Branch("calo", &calo);
    tree->Branch("layer", &layer);
    tree->Branch("ft", &ft);
    tree->Branch("slot", &slot);
    tree->Branch("channel", &channel);
    tree->Branch("eta", &eta);
    tree->Branch("phi", &phi);
  }
  tree->Branch("gain", &gain);
  tree->Branch("lwb1", &lwb1);
  tree->Branch("lwb2", &lwb2);
  tree->Branch("nSamples", &nSamples);
  tree->Branch("xi1",  xi1,  "xi1[nSamples]/D");
  tree->Branch("xi2",  xi2,  "xi2[nSamples]/D");
  tree->Branch("xip1", xip1, "xip1[nSamples]/D");
  tree->Branch("xip2", xip2, "xip2[nSamples]/D");

  for (long long k = 0; k < Definitions::nChannels; k++) {
    if (k % 10000 == 0) cout << "Processing entry " << k << endl;
    hash = k;
    if (tmpl) {
      const CellInfo* cellInfo = tmpl->cellInfo(k);
      calo    = (cellInfo ? cellInfo->calo()    : -999 );
      layer   = (cellInfo ? cellInfo->layer()   : -999 );
      ft      = (cellInfo ? cellInfo->feedThrough() : -999 );
      slot    = (cellInfo ? cellInfo->slot()    : -999 );
      channel = (cellInfo ? cellInfo->channel() : -999 );
      eta     = (cellInfo ? cellInfo->eta()     : -999 );
      phi     = (cellInfo ? cellInfo->phi()     : -999 );
    }
    for (unsigned int g = 0; g < 3; g++) {
      gain = g;
      ShapeErrorData* data1 = shapeErrorData(k, (CaloGain::CaloGain)g);
      ShapeErrorData* data2 = other.shapeErrorData(k, (CaloGain::CaloGain)g);

      lwb1 = (data1 ? data1->lwb() : -1);
      lwb2 = (data2 ? data2->lwb() : -1);
      nSamples = (data1 ? data1->nSamples() : data2 ? data2->nSamples() : 0);
      int lwb = (lwb1 >= 0 ? lwb1 : lwb2);
      
      for (int j = lwb; j < lwb + nSamples; j++) {
        xi1[j]  = (data1 ? data1->xi()(j)  : -999);
        xip1[j] = (data1 ? data1->xip()(j) : -999);
        xi2[j]  = (data2 && data2->isInRange(j) ? data2->xi()(j)  : -999);
        xip2[j] = (data2 && data2->isInRange(j) ? data2->xip()(j) : -999);
      }

      tree->Fill();
      if (data1) delete data1;
      if (data2) delete data2;
    }
  }
  f->cd();
  tree->Write();
  delete f;
  return true;
}
