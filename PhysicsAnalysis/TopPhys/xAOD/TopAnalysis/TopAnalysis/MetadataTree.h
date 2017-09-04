/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef METADATATREE_H
#define METADATATREE_H

#include "TopConfiguration/TopConfig.h"
#include "TFile.h"
#include "TTree.h"
#include <string>
#include <vector>
#include <memory>

// This class is designed to hold any information required for the metatdata
// trees which we create in AnalysisTop, rather than defining these objects
// directly in the exectutable

class MetadataTree{
public:
  /// Constructor
  MetadataTree(std::shared_ptr<TFile> );
  /// Destructor
  ~MetadataTree();
  /// Assign variables
  void AssignVariables(std::shared_ptr<top::TopConfig>);
  /// Clear any variables
  void Clear();
  /// Print info at any point
  void Print();
  /// Update any counters before filling
  void Update();
  /// Fill the tree
  void Fill();


  /// Variables of tree - public as we need access
  float totalEventsWeighted;
  std::vector<float> totalEventsWeighted_LHE3;
  std::vector<double> totalEventsWeighted_LHE3_temp;
  std::vector<std::string> names_LHE3;
  bool recalc_LHE3;
  int dsid;
  ULong64_t totalEvents;        // Metadata, for one file
  ULong64_t totalEventsInFiles; // Total events in actual file
  std::unordered_map<std::string, std::vector<float> *> totalEventsPdfWeighted;
  int dsidPdf;
  bool pdfMetadataExists;
  std::string filename;
private:
  // Trees
  TTree* m_sumWeights;
  TTree* m_sumPdfWeights;
  bool savePDF;
  TFile* m_outputFile;


};
#endif
