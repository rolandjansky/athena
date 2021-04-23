/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_VERTEX
#define INDETPHYSVALMONITORING_INDETPERFPLOT_VERTEX
/**
 * @file InDetPerfPlot_Vertex.h
 * @author Simone Pagan Griso
 **/




// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/VertexFwd.h"
// std includes
#include <string>

class TH1;

///class holding general plots for xAOD::Vertex
class InDetPerfPlot_Vertex: public InDetPlotBase {
public:
  InDetPerfPlot_Vertex(InDetPlotBase* pParent, const std::string& dirName);
  void fill(const xAOD::Vertex& vertex, float weight);
private:
  ///@name Position of vertex
  ///@{
  ///Position x
  TH1* m_vx_x;
  ///Position y
  TH1* m_vx_y;
  ///Position z
  TH1* m_vx_z;
  ///@}

  ///@name Errors of vertex
  ///Error x
  TH1* m_vx_err_x;
  ///Error y
  TH1* m_vx_err_y;
  ///Error z
  TH1* m_vx_err_z;
  ///@}

  ///@name Vertex quality and type
  ///@{
  /// chi^2/ndf
  TH1* m_vx_chi2_over_ndf;
  /// type
  TH1* m_vx_type;
  ///@}

  ///@name Track properties
  ///@{
  ///Number of tracks at vertex
  TH1* m_vx_nTracks;
  ///Distribution of tracks' weights
  TH1* m_vx_track_weights;
  ///Tracks pT (iDetailLevel >= 100)
  TH1* m_vx_track_pt;
  ///Tracks eta (iDetailLevel >= 100)
  TH1* m_vx_track_eta;
  ///Tracks silicon hits (iDetailLevel >= 100)
  TH1* m_vx_track_nSiHits;
  ///Tracks silicon holes (iDetailLevel >= 100)
  TH1* m_vx_track_nSiHoles;
  ///Tracks d0 (iDetailLevel >= 100)
  TH1* m_vx_track_d0;
  ///Tracks d0 error (iDetailLevel >= 100)
  TH1* m_vx_track_err_d0;
  ///Tracks z0 (iDetailLevel >= 100)
  TH1* m_vx_track_z0;
  ///Tracks z0 error (iDetailLevel >= 100)
  TH1* m_vx_track_err_z0;
  ///@}


  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
};




#endif
