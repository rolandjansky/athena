/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///===================================================
/// SkimTrigNtuples class
///---------------------------------------------------
/// Skim the trigger ntuples with trigger or pass-through
///
/// Author: Daniel Scheirich <scheiric@cern.ch>
/// Part of TriggerJpsiAnalysis
///===================================================

#ifndef SkimTrigNtuples_H
#define SkimTrigNtuples_H

// c++ stuff
#include <Riostream.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>

// Root stuff
#include <TSystem.h>
#include <TString.h>
#include <TLorentzVector.h>
#include <TLegend.h>
#include <TF1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLine.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>

// BPhys stuff
#include "Muon.h"
#include "Track.h"

typedef std::vector<Track> TrackBag;
typedef std::vector<Muon> MuonBag;

// TrigJpsiAnalysis stuff
#include "Log.h"
#include "Utils.h"
#include "AnalysisBase.h"
#include "CutsSkim.h"
#include "BCompositeParticle.h"
#include "Chain.h"

///===================================================
class SkimTrigNtuples : public AnalysisBase {

  public:
    /// constructor
    SkimTrigNtuples(int olIn, std::vector<std::string> files) :  AnalysisBase(olIn)
    {
      Log log("SkimTrigNtuples::SkimTrigNtuples", ol());
      //load files
      loadFiles(files);
    }

    SkimTrigNtuples(int olIn, std::string dirPath) : AnalysisBase(olIn)
    {
      Log log("SkimTrigNtuples::SkimTrigNtuples", ol());
      //load files
      loadDirectory(dirPath);
    }

    /// analyzes

    void skim(string settings,
              vector<string> triggers);


  private:

};
///===================================================

#endif

///===================================================
///      . .
///====o==V==o========================================
