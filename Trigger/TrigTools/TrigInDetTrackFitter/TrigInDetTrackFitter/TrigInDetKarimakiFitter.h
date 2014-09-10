/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGINDETKARIMAKIFITTER_H__
#define __TRIGINDETKARIMAKIFITTER_H__

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 


class PixelID;
class SCT_ID;


class TrigInDetKarimakiFitter: public AthAlgTool, virtual public ITrigInDetTrackFitter {

  // Ancillary track structure
  struct AncillaryTrack {
    
    // Prefit info
    double xm1;
    double ym1;
    double cx1;
    double cy1;
    double eta;
    double phi;
    
    // Fit RZ info
    double cotantheta;
    double ang;
    double z0;
    
    // Fit RPhi info
    double q;
    double xc;
    double yc;
    double rc;
    double phic;
    double k;
    double delta;
    double pt;
    
    // Fit chi2 info
    double chi2;
    double chi2rz;
    double chi2rp;
    int    nSP;
  };
  
  
 public:
  
  TrigInDetKarimakiFitter(const std::string&, const std::string&, const IInterface*);
  virtual ~TrigInDetKarimakiFitter();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  void fit(TrigInDetTrackCollection*);
  TrackCollection* fit(const TrackCollection&, const Trk::ParticleHypothesis&) {return nullptr;}

 private:
  
  void circlefitRPhi(TrigInDetTrack*, TrigInDetKarimakiFitter::AncillaryTrack&);
  void fitRZ(TrigInDetTrack*, TrigInDetKarimakiFitter::AncillaryTrack&);
  void fitRPhi(TrigInDetTrack*, TrigInDetKarimakiFitter::AncillaryTrack&);
  
 private:
  
  MsgStream* m_log;
  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;

};

#endif
