/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
///
/// This class aims to do track matching given two vectors of 
/// FTK_RawTrack: one is the reference and one is the test
/// this class gets also maps of histograms to be filled with
/// the track parameters.
/// The goal is to have a summary of the matching into four types of histos:
/// HWSW*: if HW and SW tracks are completely matched, i.e. have the same track parameters within 0.1% of relative error
///	      the distrubutions of the track paramters (like pt, eta, ...) are displayed in the histograms
/// HWonly*: if HW tracks do not have SW tracks within m_dmax=0.2, 
///		the distrubutions of these HW track paramters (like pt, eta, ...) are displayed in the histograms
/// SWonly*: if SW tracks do not have HW tracks within m_dmax=0.2, 
///		the distrubutions of these SW track paramters (like pt, eta, ...) are displayed in the histograms
/// HWvsSW: if HW and SW tracks are not completely matched, 
///		i.e. they are geometrically closed but do not have the same track parameters
///		these histograms in turn are divided into :
///		HWvsSWsw*: the distrubutions of the SW track paramters (like pt, eta, ...) are displayed in the histograms
///		HWvsSWhw*: the distrubutions of the HW track paramters (like pt, eta, ...) are displayed in the histograms
///		HWvsSWdiff*: the distrubutions of the difference btw the HW and SW track paramters (like pt, eta, ...) 
///			     are displayed in the histograms
/// 
///////////////////////////////////////////////////////////////////


#ifndef __CompareFTKTracks__
#define __CompareFTKTracks__


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
    
    /// initialization of the object with the vectors of reference and test FTK_RawTrack, and the maps of histos to be filled
    CompareFTKTracks(std::vector<const FTK_RawTrack *> &ref, std::vector<const FTK_RawTrack *> &test, std::map<std::string , TH1D * > &map_histo, std::map<std::string , TH2D * > &map_histo_2D);
    
    /// here the matching is done: the criterium is angular distance specified by m_dmax
    void AssociateTracks();
    
    /// change the maximum distance between two tracks for angular matching: default 0.2
    void SetDRmax(double dmax){m_dmax=dmax;}
    
    /// here a rough printout equivalence between the two vectors of tracks is performed
    void TestEquivalence();

    /// here histograms are filled wrt to the 4 types of histograms:
    void FillHistos();

    /// functions which from a string concerning the track parameter returns the value of the track parameter, 
    /// through the FTK_RawTrack method 
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
#endif //__CompareFTKTracks__
