/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONINSIDEOUTVALIDATIONNTUPLE_H
#define MUON_MUONINSIDEOUTVALIDATIONNTUPLE_H

#include "MuonLayerEvent/MuonSystemExtension.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include <vector>
#include <string>
#include "TTree.h"

namespace Muon {

  /** base class for ntuple block
      The class can handle float and integer block and supports sub blocks.
      
      Internally it caches the blocks in vectors which it uses to operate on them. 

      Users should add all content they want in the ntuple in the constructor of they
      concrete classes.
   */
  struct MuonValidationBlockBase {
    // type defs of the types
    typedef std::pair<std::reference_wrapper<std::vector<int>*>,std::string>           IntBlock;
    typedef std::pair<std::reference_wrapper<std::vector<unsigned int>*>,std::string>  UnsignedIntBlock;
    typedef std::pair<std::reference_wrapper<std::vector<float>*>,std::string>         FloatBlock;
    typedef std::pair< MuonValidationBlockBase*, std::string >                         SubBlock;

    // destructor, cleans up memory
    virtual ~MuonValidationBlockBase();

    // add a block to the internal caches, the name should correspond to the leave name of the block
    void addBlock( std::vector<int>*& block, std::string name ) { block=nullptr; intBlocks.push_back( IntBlock(block,name) ); }
    void addBlock( std::vector<unsigned int>*& block, std::string name ) { block=nullptr; unsignedIntBlocks.push_back( UnsignedIntBlock(block,name) ); }
    void addBlock( std::vector<float>*& block, std::string name ) { block=nullptr; floatBlocks.push_back( FloatBlock(block,name) ); }
    void addBlock( MuonValidationBlockBase* block, std::string name ) { subBlocks.push_back( SubBlock(block,name) ); }

    // caches for the pointers
    std::vector< IntBlock >           intBlocks;
    std::vector< UnsignedIntBlock >   unsignedIntBlocks;
    std::vector< FloatBlock >         floatBlocks;
    std::vector< SubBlock >           subBlocks;

    // initialize the ntuple for writing or reading
    void init( const std::string& prefix, TTree*, bool write = true);
      
    // clear, to be called with filling the ntuple at the end of the event
    void clear();

  };

  /**
     Block with identifier information
   */
  struct MuonValidationIdBlock : public MuonValidationBlockBase {
    MuonValidationIdBlock();

    std::vector<int>*   sector = nullptr;  // sector
    std::vector<int>*   chIndex = nullptr; // chamber index

    void fill( int sector_, int chIndex_ ) { sector->push_back(sector_); chIndex->push_back(chIndex_); }
      
  };

  /**
     Block with truth information
   */
  struct MuonValidationTruthBlock : public MuonValidationBlockBase {
    MuonValidationTruthBlock();

    std::vector<int>*   pdg = nullptr;     // pdg id
    std::vector<int>*   barcode = nullptr; // barcode
    std::vector<float>* beta = nullptr;    // beta

    void fill( int pdg_, int barcode_, float beta_=1. ) { pdg->push_back(pdg_); barcode->push_back(barcode_);beta->push_back(beta_); }
      
  };

  /**
     Block with the track index
   */
  struct MuonValidationTrackBlock : public MuonValidationBlockBase {
    MuonValidationTrackBlock();

    std::vector<int>*   trkid = nullptr;  // track index
      
    void fill( int trkid_ ) { trkid->push_back(trkid_); }

  };


  /**
     Block with timing information
   */
  struct MuonValidationTimeBlock : public MuonValidationBlockBase {
    MuonValidationTimeBlock();

    MuonValidationIdBlock             id;    // identifier information for the segment
    MuonValidationTrackBlock          track; // index of corresponding track particle
    MuonValidationTruthBlock          truth; // truth matching based on the hit content of the segment

    std::vector<int>*     type = nullptr;      // type: 0 = rpc, 1 = segment, 2 = rpc, 10 MDTT, 12 RPC track
    std::vector<int>*     stage = nullptr;     // stage of reco
    std::vector<unsigned int>* gasgapId = nullptr;  // gasgapId (set to the chamber id for segments)
    std::vector<float>*   r = nullptr;         // r-position
    std::vector<float>*   z = nullptr;         // z-position
    std::vector<float>*   d = nullptr;         // total distance from ip
    std::vector<float>*   time = nullptr;      // time 
    std::vector<float>*   err = nullptr;       // error
    std::vector<float>*   timeProp = nullptr;  // propagation time 
    std::vector<float>*   avTimeProp = nullptr;// average propagation time 
    std::vector<float>*   tof = nullptr;       // time of flight
    std::vector<float>*   avTof = nullptr;     // average time of flight
    std::vector<float>*   timeCor = nullptr;   // time correction

    void fill( int type_, unsigned int gasgapId_, float r_, float z_, float time_, float err_,
               float timeProp_ = 0., float avTimeProp_ = 0., float tof_ = 0., float avTof_ = 0., float timeCor_ = 0., int stage_ = 0 ) { 
      type->push_back(type_); gasgapId->push_back(gasgapId_); r->push_back(r_); z->push_back(z_); d->push_back(sqrt(r_*r_+z_*z_)); time->push_back(time_); 
      err->push_back(err_); timeProp->push_back(timeProp_); avTimeProp->push_back(avTimeProp_); tof->push_back(tof_); 
      avTof->push_back(avTof_); timeCor->push_back(timeCor_); stage->push_back(stage_);
    }
      
  };
  

  /**
     Block with residual and pull information
   */
  struct MuonValidationResidualBlock : public MuonValidationBlockBase {
    MuonValidationResidualBlock();

    void fill( const Trk::MeasurementBase& hit, const MuonSystemExtension::Intersection& intersection, Trk::ParamDefs par ){
      float pos_ = hit.localParameters()[par];
      float err_ = Amg::error(hit.localCovariance(),par);
      fill(pos_,err_,intersection,par);
    } 
      
    void fill( float pos_, float err_, const MuonSystemExtension::Intersection& intersection, Trk::ParamDefs par ){
      float expos_ = intersection.trackParameters->parameters()[Trk::loc1];
      auto cov = intersection.trackParameters->covariance();
      float expos_err_ = cov ? Amg::error(*cov,par) : 0;
      fill(pos_,err_,expos_,expos_err_, cov!=nullptr );
    }

    void fill( float pos_, float err_, float expos_, float expos_err_, int status=1 ){

      float res = pos_ - expos_;
      float err2 = err_*err_ + expos_err_*expos_err_;
      float pull_ = res/sqrt(err2);

      residual->push_back(res); 
      pull->push_back(pull_);
      pos->push_back(pos_);
      err->push_back(err_);
      expos->push_back(expos_);
      expos_err->push_back(expos_err_);
      expos_errstatus->push_back(status);
    }

    void fillResPull( float res_, float pull_, int status=1 ){
      fillResPull(res_,pull_,1.,1.,status);
    }

    void fillResPull( float res_, float pull_, float err_, float expos_err_, int status=1 ){

      residual->push_back(res_); 
      pull->push_back(pull_);
      pos->push_back(0.);
      err->push_back(err_);
      expos->push_back(0.);
      expos_err->push_back(expos_err_);
      expos_errstatus->push_back(status);
    }
 
    std::vector<float>* residual = nullptr;         // residual 
    std::vector<float>* pull = nullptr;             // pull
    std::vector<float>* pos = nullptr;              // measured position 
    std::vector<float>* err = nullptr;              // error on the measured position
    std::vector<float>* expos = nullptr;            // extrapolated position (prediction)
    std::vector<float>* expos_err = nullptr;        // error on extrapolated position
    std::vector<int>*   expos_errstatus = nullptr;  // status, 0 if no error estimate was provided on the extrapolated position
      
  };

  /**
     Block with information on the incoming track particle
   */
  struct MuonValidationTrackParticleBlock : public MuonValidationBlockBase {
    MuonValidationTrackParticleBlock();

    std::vector<float>* pt = nullptr;         // pt (at the vertex)
    std::vector<float>* p = nullptr;          // p (at the vertex)
    std::vector<float>* eta = nullptr;        // eta (at the vertex) 
    std::vector<float>* phi = nullptr;        // phi (at the vertex) 
    MuonValidationTruthBlock truth; // truth information

  };
  
  /**
     Block with segment information
   */
  struct MuonValidationSegmentBlock : public MuonValidationBlockBase {
    MuonValidationSegmentBlock();

    std::vector<int>*   stage = nullptr;               // reco stage (0 : segment finding, 1: after matching with ID track)
    std::vector<int>*   quality = nullptr;             // segment quality
    std::vector<int>*   nmdtHits = nullptr;            // number of MDT hits
    std::vector<int>*   ntrigEtaHits = nullptr;        // number of trigger eta hits
    std::vector<int>*   ntrigPhiHits = nullptr;        // number of trigger phi hits
    std::vector<float>* r = nullptr;                   // r-position
    std::vector<float>* z = nullptr;                   // z-position
    std::vector<float>* t0 = nullptr;                  // t0 from t0 fit
    std::vector<float>* t0Error = nullptr;             // error on t0 from t0 fit
    std::vector<float>* t0Trig = nullptr;              // t0 from trigger hits
    std::vector<float>* t0TrigError = nullptr;         // error on t0 from trigger hits
    MuonValidationIdBlock             id;    // identifier information for the segment
    MuonValidationTrackBlock          track; // index of corresponding track particle
    MuonValidationTruthBlock          truth; // truth matching based on the hit content of the segment
    MuonValidationResidualBlock       xresiduals; // residuals in the position in the non-bending plane
    MuonValidationResidualBlock       yresiduals; // residuals in the position in the bending plane
    MuonValidationResidualBlock       angleXZ;    // residuals in the angle in the non-bending plane
    MuonValidationResidualBlock       angleYZ;    // residuals in the angle in the bending plane
    MuonValidationResidualBlock       combinedYZ; // combined residuals in the position/angle in the bending plane 

  };

  /** 
      Block with hough maxima information
  */
  struct MuonValidationHoughBlock : public MuonValidationBlockBase {
    MuonValidationHoughBlock();

    std::vector<float>*               maximum = nullptr;   // height of the maximum
    MuonValidationIdBlock             id;        // identifier information for the maximum
    MuonValidationTrackBlock          track;     // index of corresponding track particle
    MuonValidationTruthBlock          truth;     // truth matching based on the hit content of the maximum
    MuonValidationResidualBlock       residuals; // residuals in the position in the bending plane

  };

  struct MuonValidationHitBlock : public MuonValidationBlockBase {
    MuonValidationHitBlock();

    MuonValidationIdBlock             id;        // identifier information for the hit
    MuonValidationTrackBlock          track;     // index of corresponding track particle
    MuonValidationTruthBlock          truth;     // truth matching based on the hit identifier
    MuonValidationResidualBlock       residuals; // residuals in the position in the measurement plane of the hit
  };

  struct MuonValidationCandidateBlock : public MuonValidationBlockBase {
    MuonValidationCandidateBlock();

    std::vector<int>*   ntimes = nullptr;        // number of time measurements
    std::vector<float>* beta = nullptr;          // beta
    std::vector<float>* chi2ndof = nullptr;      // chi2/ndof beta fit
    std::vector<int>*   nseg = nullptr;          // number of segments
    std::vector<int>*   nprec = nullptr;         // precision layers
    std::vector<int>*   ntrigPhi = nullptr;      // trigger phi layers
    std::vector<int>*   ntrigEta = nullptr;      // trigger eta layers
    std::vector<int>*   stage = nullptr;         // reco stage

    MuonValidationTrackBlock          track;     // index of corresponding track particle

    void fill( int ntimes_, float beta_, float chi2ndof_, int nseg_, int nprec_, int ntrigPhi_, int ntrigEta_, int stage_ ){
      ntimes->push_back(ntimes_); 
      beta->push_back(beta_); 
      chi2ndof->push_back(chi2ndof_); 
      nseg->push_back(nseg_); 
      nprec->push_back(nprec_); 
      ntrigPhi->push_back(ntrigPhi_); 
      ntrigEta->push_back(ntrigEta_); 
      stage->push_back(stage_); 
    }

  };


  class MuonInsideOutValidationNtuple : public MuonValidationBlockBase {
  public:
    MuonInsideOutValidationNtuple();

    MuonValidationTrackParticleBlock trackParticleBlock; // tracks
    MuonValidationSegmentBlock       segmentBlock;       // segments
    MuonValidationHoughBlock         houghBlock;         // hough maxima
    MuonValidationHitBlock           hitBlock;           // hits
    MuonValidationTimeBlock          timeBlock;          // times
    MuonValidationCandidateBlock     candidateBlock;     // block for reconstructed candidates
  };
  
}

#endif
