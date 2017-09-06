/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopAnalysis/MetadataTree.h"
#include <algorithm>
#include <iostream>

MetadataTree::MetadataTree(std::shared_ptr<TFile> outputFile){
  m_outputFile = outputFile.get();
  // Change to file
  m_outputFile->cd();
  // Create pointers
  m_sumWeights = new TTree("sumWeightsTree", "sumWeightsTree");
  m_sumPdfWeights  = new TTree("sumPdfWeights", "sumPdfWeights");
  // PDF off by default - switched on in AssignVariables
  savePDF = false;
  pdfMetadataExists = false;
  // Default for recalculation is False
  recalc_LHE3 = false;

}

MetadataTree::~MetadataTree(){

}

void MetadataTree::Clear(){
  // Reset all variables to default values
  dsid                = 0;
  dsidPdf             = 0;
  totalEvents         = 0;
  totalEventsWeighted = 0;
  totalEventsInFiles  = 0;
  filename            = "SetMe";
  totalEventsWeighted_LHE3.clear();
  names_LHE3.clear();
  totalEventsPdfWeighted.clear();
  totalEventsWeighted_LHE3_temp.clear();

  return;
}

void MetadataTree::Print(){
  std::cout << "dsid                 " << dsid                << std::endl;
  std::cout << "dsidPdf              " << dsidPdf             << std::endl;
  std::cout << "totalEvents          " << totalEvents         << std::endl;
  std::cout << "totalEventsWeighted  " << totalEventsWeighted << std::endl;
  std::cout << "totalEventsInFiles   " << totalEventsInFiles  << std::endl;

}

void MetadataTree::AssignVariables(std::shared_ptr<top::TopConfig> topConfig){
  // Assign sum weights
  m_sumWeights->Branch("dsid", &dsid);
  m_sumWeights->Branch("totalEventsWeighted", &totalEventsWeighted);
  m_sumWeights->Branch("inputFileName", &filename);
  m_sumWeights->Branch("totalEvents", &totalEvents, "totalEvents/l");

  if (topConfig->doMCGeneratorWeights()) {
    savePDF = true;
    m_sumWeights->Branch("totalEventsWeighted_mc_generator_weights", &totalEventsWeighted_LHE3);
    m_sumWeights->Branch("names_mc_generator_weights", &names_LHE3);
  }

  // Assign PDF
  if (topConfig->doLHAPDF()) {
    m_sumPdfWeights = new TTree("PDFsumWeights", "");
    m_sumPdfWeights->Branch("dsid", &dsidPdf);
    for (const auto &pdf_set : topConfig->LHAPDFSets()) {
      totalEventsPdfWeighted[pdf_set] = new std::vector<float>();
      m_sumPdfWeights->Branch(pdf_set.c_str(), totalEventsPdfWeighted[pdf_set]);
    }
  }

  return;
}
void MetadataTree::Update(){
  // If we were recalculating the LHE3 weights OTF, we need to copy the entries from _temp
  // This is handled in the event loop rather than here though, just do the copy now
  if(recalc_LHE3){
    for (double d: totalEventsWeighted_LHE3_temp)
      totalEventsWeighted_LHE3.push_back(d);
  }
  return;
}

void MetadataTree::Fill(){
  // We will write and update the tree each time we fill
  m_outputFile->cd();
  m_sumWeights->Fill();
  if(savePDF){
    m_sumPdfWeights->Fill();
  }
  return;
}
