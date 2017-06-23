/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// FTK_DuplicateTrackRemovalTool tool
// -------------------------------
// ATLAS Collaboration
//
// Remove duplicate (overlapping) tracks
//
// June 2017: Tool created
//
// Author: Andy Haas, NYU
// e-mail: ahaas@cern.ch
//
////////////////////////////////////////////////////////////////////////////////


#ifndef __TRIG_FTK_DUPLICATETRACKREMOVAL_TOOL_H__
#define __TRIG_FTK_DUPLICATETRACKREMOVAL_TOOL_H__

#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "FTK_RecToolInterfaces/IFTK_DuplicateTrackRemovalTool.h"
#include "FTK_DataProviderInterfaces/IFTK_UncertaintyTool.h"
#include "TrkVxEdmCnv/IVxCandidateXAODVertex.h"

#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "VxVertex/VxContainer.h"	
//#include "xAODTracking/Vertex.h"
//#include "xAODTracking/TrackParticle.h"
//#include "xAODTracking/VertexContainer.h"
//#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexFwd.h"
//#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
//#include "xAODTracking/TrackParticleContainerFwd.h"
//#include "Tracking/TrkVertexFitter/TrkVxEdmCnv/TrkVxEdmCnv/IVxCandidateXAODVertex.h"
class VxContainer;
using std::vector;
namespace Trk {
  class Track;
  //class Trk::IVxCandidateXAODVertex;
}
class FTK_DuplicateTrackRemovalTool : public AthAlgTool, virtual public IFTK_DuplicateTrackRemovalTool
{
  //struct to hold track parameters
  struct MyTrack{
    MyTrack(int idx,float pt,float theta,float phi,float d0,float z0,float pterr,float thetaerr,float phierr,float d0err,float z0err){
    m_idx=idx;
    m_pt=pt;
    m_theta=theta;
    m_phi=phi;
    m_d0=d0;
    m_z0=z0;

    m_pterr=pterr;
    //    m_qoverperr=qoverperr;
    m_thetaerr=thetaerr;
    m_phierr=phierr;
    m_d0err=d0err;
    m_z0err=z0err;
    }
    int m_idx;
    double m_pt;
    double m_theta;
    double m_phi;
    double m_d0;
    double m_z0;

    double m_pterr;
    //double m_qoverperr=qoverperr;
    double m_thetaerr;
    double m_phierr;
    double m_d0err;
    double m_z0err;

    //Track pT sort
    bool operator<(const MyTrack &a)const{
      return fabs(m_pt) > fabs(a.m_pt);
    }
  };

 public:

  FTK_DuplicateTrackRemovalTool( const std::string&, const std::string&, const IInterface* );
  virtual ~FTK_DuplicateTrackRemovalTool(){};

  virtual StatusCode initialize();
  virtual  StatusCode finalize  ();

  FTK_RawTrackContainer* removeDuplicates(const FTK_RawTrackContainer* trks);
  
 private:

    bool m_barrelOnly;
    bool m_hasIBL;
    double  m_cluster_size;
    double m_chi2cut;
    double m_constTrkPt;
    double m_constTrkEta;
    double m_z0errfactor;

    FTK_RawTrackContainer* m_trks_nodups;
 
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(vector<MyTrack> trks);
    double ctheta2eta(double cot);
    vector<MyTrack> getTracks(const FTK_RawTrackContainer* trks);
    vector<MyTrack> getTracks(const TrackCollection* trks);

    ToolHandle<IFTK_UncertaintyTool> m_uncertaintyTool;
};


#endif

