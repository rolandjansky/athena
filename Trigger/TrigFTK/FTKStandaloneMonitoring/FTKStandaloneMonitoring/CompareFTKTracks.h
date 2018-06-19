/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef FTKSTANDALONEMONITORING_COMPAREFTKTRACKS_H
#define FTKSTANDALONEMONITORING_COMPAREFTKTRACKS_H


#include <iostream>
#include <fstream>

#include <vector>
#include <cmath>
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "FTKStandaloneMonitoring/FTKTrkAssoc.h"
class CompareFTKTracks{
  public:
    CompareFTKTracks();
    CompareFTKTracks(std::vector<const FTK_RawTrack *> &ref, std::vector<const FTK_RawTrack *> &test, std::map<std::string , TH1D * > &map_histo, std::map<std::string , TH2D * > &map_histo_2D);
    void AssociateTracks();
    void SetDRmax(double dmax){m_dmax=dmax;}
    void TestEquivalence();
    void FillHistos();
    double GetValue(std::string & variable, const FTK_RawTrack * tmptrk);
  private:
    std::map<std::string , TH1D * > m_map_histo;
    std::map<std::string , TH2D * > m_map_histo_2D;
    std::vector<const FTK_RawTrack *> m_ref;
    std::vector<const FTK_RawTrack *> m_test;
    FTKTrkAssoc* m_associator;
    double m_dmax=0.2;
    bool m_allmatched=true;
    std::vector<std::string> m_variable_list={"pt","eta","phi","d0","z0","chi2","ETA_PHI"};
};
#endif //FTKSTANDALONEMONITORING_COMPAREFTKTRACKS_H
