/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONINSIDEOUTANALYSISPLOTS_H
#define MUON_MUONINSIDEOUTANALYSISPLOTS_H

#include "MuonRecValidationNtuples/MuonInsideOutValidationNtuple.h"
#include "TString.h"

class TH1;
class TH2;
class TDirectory;

namespace Muon {
  
  /** residual plots */
  struct ResPlots {
    TH1* res;
    TH1* pull;
    TH1* exerr;
    
    void book( TString prefix );
    void fill( float res_, float pull_, float exerr_ );
  };

  /** residual plots vs pt */
  struct ResPlotsPt {
    TH2* res_p;
    TH2* pull_p;
    TH2* exerr_p;
    
    void book( TString prefix );
    void fill( float res_, float pull_, float exerr_, float pt );
  };

  /** residual plots per chamber index */
  struct ChamberResPlots {

    std::vector<ResPlots>   plots;
    std::vector<ResPlotsPt> plots_p;

    void book( TDirectory* dir, TString prefix );
    void fill( int chIndex_, float res_, float pull_, float exerr_, float p_ );

  };

  /** hit plots */
  struct HitPlots {
    ResPlots         all;
    ResPlotsPt       all_p;
    ChamberResPlots  chamber;

    void book( TDirectory* dir, TString prefix );

    void fill( const MuonValidationHitBlock& hits, int index, float p_ );

    void fill( int chIndex_, float res_, float pull_, float exerr_, float p_ );
  };

  /** time plots */
  struct TimePlots {
    TH1* time;
    TH1* error;
    TH1* timeRes;
    TH1* timePull;
    TH1* beta;
    TH1* betaRes;

    void book( TDirectory* dir, TString prefix );

    void fill( const MuonValidationTimeBlock& times, int index, float p_, int pdg );
  };

  /** hough plots */
  struct HoughPlots {
    TH1* max;
    TH2* max_pull;

    ResPlots         all;
    ChamberResPlots  chamber;

    void book( TDirectory* dir, TString prefix );
    void fill( const MuonValidationHoughBlock& hough, int index, float p_ );
    void fill( int chIndex_, float res_, float pull_, float exerr_, float p_, float max_ );

  };

  /** segment plots */
  struct SegmentPlots {
    TH1* t0;
    TH1* t0Trig;
    TH1* beta;
    TH2* beta2;
    TH1* betaTrig;
    TH1* t0Res;
    TH1* t0ResTrig;
    TH1* betaRes;
    TH1* betaResTrig;
    TH1* quality;
    TH2* quality_chIndex;
    TH2* quality_sector;
    
    ResPlots        allx;
    ChamberResPlots chamberx;

    ResPlots        ally;
    ChamberResPlots chambery;

    ResPlots        allxz;
    ChamberResPlots chamberxz;

    ResPlots        allyz;
    ChamberResPlots chamberyz;

    ResPlots        allcy;
    ChamberResPlots chambercy;

    void book( TDirectory* dir, TString prefix );
    void fill( int chIndex_, int sector_, int quality_ );
    void fill( const MuonValidationSegmentBlock& segments, int index, float p_ );

  };


  /** beta fit result */
  struct BetaFitPlots {
    TH1* beta;
    TH1* chi2Ndof;
    TH1* ndof;
    TH1* res;

    void book( TDirectory* dir, TString prefix );
    void fill( float beta_, float betaTruth_, float chi2_, int ndof_ );
  };
  
  /** track based plots */
  struct TrackPlots {
    TH1* pt;
    TH1* eta;
    TH1* phi;
    TH1* beta;
    TH1* pdg;
    TH1* ntruth;
    TH1* nseg;
    TH1* nseg1;
    TH1* nhough;
    TH2* ntruth_seg;
    TH2* ntruth_seg1;
    TH2* ntruth_hough;
    
    HitPlots     hits;
    HoughPlots   hough;
    SegmentPlots segments;
    SegmentPlots segments1;
    TimePlots    rpcTimes;
    TimePlots    segmentTimes;
    BetaFitPlots betaAll;
    BetaFitPlots betaType0;
    BetaFitPlots betaType1;
    
    void book( TDirectory* dir, TString prefix );
    void fill( int ntruth_, int nseg_, int nseg1_, int nhough_ );
    void fill( const MuonValidationTrackParticleBlock& tracks, int index );
  };

  struct MuonInsideOutAnalysisPlots {
    TrackPlots muon;
    TrackPlots rest;
    void book( TDirectory* dir, TString prefix );
  };

}

#endif
