/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PathFinder_h
#define PathFinder_h

#include <string>
#include <vector>

class TFile;
class TDirectory;


using std::string;
using std::vector;

/**
 *  PathFinder
 *
 *  class to walk a ROOT file's directory hierarchy and extract all TObject paths and types matching
 *  inclusion and exclusion regular expressions.
 *
 *  should be fed with a TFile, and regular expressions
 *
 *  can dump the list to file in a number of formats (plain, xml, ...)
 *  internal list can be iterated over
 *
 */

class PathFinder {

  public:
    PathFinder(string inputFilePath, string runNumber, string inclExpr, string exclExpr);
    ~PathFinder(void);


    void CopyHistograms(string outputfile);
    void WalkDirectory(TDirectory* dir = NULL);

    //void DumpHistoList(void);

    void FilterList(void);

  private:
    TFile* in_file;
    string runNo;
    string expr_incl;
    string expr_excl;
    vector<string> histoPaths1D;
    vector<string> histoPaths2D;
};



#endif
