/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//implementation
////////////////////////////////////////////////////////////////////////////////
// FTK_VertexFinderTool tool
// -------------------------------
// ATLAS Collaboration
//
// Return fitted vertices
//
// 10.08.2015 Package created
//
// Author: Rui Wang, ANL
// e-mail: Rui.Wang@cern.ch
//
////////////////////////////////////////////////////////////////////////////////


#ifndef __TRIG_FTK_VERTEXFINDER_TOOL_H__
#define __TRIG_FTK_VERTEXFINDER_TOOL_H__

#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "FTK_RecToolInterfaces/IFTK_VertexFinderTool.h"
#include "FTK_DataProviderInterfaces/IFTK_UncertaintyTool.h"

#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "VxVertex/VxContainer.h"
class VxContainer;
using std::vector;
class FTK_VertexFinderTool : public AthAlgTool, virtual public IFTK_VertexFinderTool
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

  FTK_VertexFinderTool( const std::string&, const std::string&, const IInterface* );
  virtual ~FTK_VertexFinderTool(){};

  virtual StatusCode initialize();
  virtual  StatusCode finalize  ();


  //
  //  Get the element of the covariance matrix for id0th and id1th track parameter
  //
   VxContainer* findVertex(const FTK_RawTrackContainer* trks);
   VxContainer* findVertex(const TrackCollection* trks);

 private:

    bool m_barrelOnly;
    bool m_hasIBL;
    double  m_cluster_size;
    double m_chi2cut;
    double m_constTrkPt;
    double m_constTrkEta;
  //
  //  Helper functions with the uncerianties
  //
  VxContainer* findVertex(vector<MyTrack> trks);
  double ctheta2eta(double cot);
  vector<MyTrack> getTracks(const FTK_RawTrackContainer* trks);
  vector<MyTrack> getTracks(const TrackCollection* trks);
  //tool handel
  ToolHandle<IFTK_UncertaintyTool> m_uncertaintyTool;
};


#endif

