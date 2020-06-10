/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************
   InDetVKalPriVxFinderTool.h - description

  A tool to find primary vertex with or without pileup, with or without beam
  constraint. It works with TrackCollection of TrackParticleBaseCollection. Beam
  position if needed is provided via BeamCond service or jobOptions.

  Tool used VKalVrt vertex fitter (TrkVKalVrtFitter tool).

  By default primary vertex finder user robust functional Rob=5 (see VKalVrt
  descrption for details) which provides optimal pulls.

  Author: Vadim Kostyukhin
  e-mail: vadim.kostyukhin@cern.ch
********************************************************/

#ifndef _VKalVrt_InDetVKalPriVxFinderTool_H
#define _VKalVrt_InDetVKalPriVxFinderTool_H
// Normal STL and physical vectors
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

// Gaudi includes
#include "TrkParameters/TrackParameters.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkTrack/TrackCollection.h"

// Gaudi includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
//
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/VertexContainerFwd.h"
//
#include <utility> //for std::pair
#include <vector>

#include "BeamSpotConditionsData/BeamSpotData.h"

#include "CxxUtils/checker_macros.h"

namespace Trk {
class TrackParticle;
}

namespace InDet {

class InDetVKalPriVxFinderTool
  : public AthAlgTool
  , virtual public IVertexFinder
{
public:
  InDetVKalPriVxFinderTool(const std::string& t,
                           const std::string& n,
                           const IInterface* p);
  virtual ~InDetVKalPriVxFinderTool();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  //
  //    Tool interface
  //
  using IVertexFinder::findVertex;

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  findVertex(const TrackCollection* trackTES) const override;

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  findVertex(const Trk::TrackParticleBaseCollection* trackTES) const override;

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  findVertex(const xAOD::TrackParticleContainer* trackTES) const override;
  //

private:
  /*  JobOption tunable parameters */

  // Beam position
  double m_BeamPositionX;
  double m_BeamPositionY;
  long int m_BeamConstraint;
  long int m_TypeRobust;
  double m_RobustScale;

  // Track selection
  long int m_CutSctHits;
  long int m_CutPixelHits;
  long int m_CutSiHits;
  long int m_CutBLayHits;
  long int m_CutSharedHits;
  double m_CutPt;
  double m_CutZVrt;
  double m_CutA0;
  double m_CutChi2;
  double m_A0TrkErrorCut;
  double m_ZTrkErrorCut;
  // Cuts for track - initial vertex association
  double m_RImpSelCut;
  double m_ZImpSelCut;
  double m_RDistSelCut;
  double m_ZDistSelCut;
  double m_SignifSelCut;
  // Stop point for the common fit
  double m_SecTrkChi2Cut;
  double m_WeightCut;
  // Maximal number of vertices
  long int m_NPVertexMax;

  PublicToolHandle<Trk::ITrkVKalVrtFitter> m_fitSvc{ this,
                                                     "TrkVKalVrtFitter",
                                                     "Trk::TrkVKalVrtFitter",
                                                     "" };
  PublicToolHandle<Trk::ITrackSummaryTool> m_sumSvc{
    this,
    "TrackSummaryTool",
    "Trk::TrackSummaryTool/AtlasTrackSummaryTool",
    ""
  }; //!< Pointer to the track summary tool
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{
    this,
    "BeamSpotKey",
    "BeamSpotData",
    "SG key for beam spot"
  };
  PublicToolHandle<Trk::ITrackSelectorTool>
    m_trkSelector{ this, "DefaultTrackSelection", "DefaultTrackSelection", "" };

 //
  int m_SummaryToolExist;
  int m_trkSelectorExist;
  
  /*
   * C.A
   * Cache for passing arguements between methods
   * Added for the MT migration to avoid state
   */
  struct Cache {
  Amg::Vector3D m_BeamCnst;
  std::vector<double> m_BeamCnstWid;
  StatusCode m_sc;
  TLorentzVector m_Momentum;
  long int m_Charge;
  std::vector<double**> m_savedTrkFittedPerigees;
  std::vector<std::vector<AmgSymMatrix(5)>> m_fittedTrkCov;
  };
  //----------------------------------------------------------------------------------------------
  //  Private functions
  //
  void CleanTrkSet(Cache& cache,
                   std::vector<const Trk::TrackParticleBase*>&,
                   std::vector<const Trk::Track*>&,
                   Amg::Vector3D& FitVertex,
                   std::vector<double>& Chi2PerTrk,
                   std::vector<const Trk::TrackParticleBase*>&,
                   std::vector<const Trk::Track*>&) const;

  int FindMax(std::vector<double>&) const;

  int FindMin(std::vector<double>&) const;

  int FindMaxSecond(std::vector<double>&) const;

  const Trk::Perigee* GetPerigee(const Trk::TrackParticleBase* List) const;

  const Trk::Perigee* GetPerigee(const Trk::Track* List) const;

  void RemoveEntryInList(std::vector<const Trk::Track*>& ListTracks, int) const;

  void RemoveEntryInList(std::vector<const Trk::TrackParticleBase*>& ListTracks,
                         int) const;

  void RemoveEntryInList(std::vector<double>& List, int Outlier) const;

  double GetLimitAngle(double) const;

  void UniqList(std::vector<const Trk::Track*>& List) const;

  void UniqList(std::vector<const Trk::TrackParticleBase*>& List) const;

  void RemoveUsedEntry(std::vector<const Trk::Track*>&,
                       std::vector<const Trk::Track*>&,
                       std::vector<double>&) const;

  void RemoveUsedEntry(std::vector<const Trk::TrackParticleBase*>&,
                       std::vector<const Trk::TrackParticleBase*>&,
                       std::vector<double>&) const;

  void inpSelector(
    std::vector<const Trk::TrackParticleBase*>& ListParticles,
    std::vector<const Trk::Track*>& ListTracks,
    Amg::Vector3D& IniVertex,
    std::vector<const Trk::TrackParticleBase*>& SelectedParticles,
    std::vector<const Trk::Track*>& SelectedTracks) const;

  double FitCommonVrt(Cache& cache,
                      std::vector<const Trk::TrackParticleBase*>& ListP,
                      std::vector<const Trk::Track*>& ListT,
                      double ZEstimation,
                      Amg::Vector3D& FitVertex,
                      std::vector<double>& ErrorMatrix,
                      std::vector<double>& TrkWeights) const;

  double FindZPosTrk(Cache& cache,
    std::vector<const Trk::Track*>& ListTracks,
                     double& ControlVariable) const;

  double FindZPosTrk(Cache& cache,
                     std::vector<const Trk::TrackParticleBase*>& ListTracks,
                     double& ControlVariable) const;

  double FindZPos(std::vector<double>& ZTrk,
                  std::vector<double>& PtTrk,
                  std::vector<double>& PxTrk,
                  std::vector<double>& PyTrk,
                  std::vector<double>& PhiTrk,
                  double& ControlVar) const;

  void SetTrkParamVectors(
    Cache& cache,
    std::vector<const Trk::TrackParticleBase*>& ListTracks,
    std::vector<double>& ZTrk,
    std::vector<double>& PtTrk,
    std::vector<double>& PxTrk,
    std::vector<double>& PyTrk,
    std::vector<double>& PhiTrk) const;

  void SetTrkParamVectors(Cache& cache,
                          std::vector<const Trk::Track*>& ListTracks,
                          std::vector<double>& ZTrk,
                          std::vector<double>& PtTrk,
                          std::vector<double>& PxTrk,
                          std::vector<double>& PyTrk,
                          std::vector<double>& PhiTrk) const;

  int PVrtListFind(
    Cache& cache,
    std::vector<const Trk::TrackParticleBase*>& ListParticles,
    std::vector<const Trk::Track*>& ListTracks,
    std::vector<Amg::Vector3D>& PVrtList,
    std::vector<AmgSymMatrix(3)>& ErrorMatrixPerVrt,
    std::vector<double>& Chi2PerVrt,
    std::vector<double>& ControlVariablePerVrt,
    std::vector<std::vector<const Trk::TrackParticleBase*>>& PrtPerVrt,
    std::vector<std::vector<const Trk::Track*>>& TrkPerVrt,
    std::vector<std::vector<double>>& TrkWgtPerVrt) const;

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> SaveResults(
    Cache& cache,
    int NVrt,
    std::vector<Amg::Vector3D>& PVrtList,
    std::vector<AmgSymMatrix(3)>& ErrorMatrixPerVrt,
    std::vector<double>& Chi2PerVrt,
    std::vector<int>& NTrkPerVrt,
    std::vector<std::vector<const Trk::TrackParticleBase*>>& PrtPerVrt,
    std::vector<std::vector<const Trk::Track*>>& TrkPerVrt,
    std::vector<std::vector<double>>& TrkWgtPerVrt,
    const TrackCollection* trackTES = 0,
    const Trk::TrackParticleBaseCollection* partTES = 0) const;

  StatusCode CutTrk(double PInvVert,
                    double ThetaVert,
                    double A0Vert,
                    double Chi2,
                    long int PixelHits,
                    long int SctHits,
                    long int SharedHits,
                    long int BLayHits) const;

  double** getWorkArr2(long int dim1, long int dim2) const;
  void removeWorkArr2(double** Arr, long int dim1, long int dim2) const;
  double*** getWorkArr3(long int dim1, long int dim2, long int dim3) const;
  void removeWorkArr3(double*** Arr,
                      long int dim1,
                      long int dim2,
                      long int dim3) const;
  AmgSymMatrix(5) FillCovMatrix(int iTrk, std::vector<double>& Matrix) const;
  Amg::Vector3D findIniXY(
    const Trk::TrackParticleBaseCollection* newPrtCol) const;
  Amg::Vector3D findIniXY(const TrackCollection* newPrtCol) const;
  //---------------------------------------------------------------------------
  //  Arrays for propagation of fit results to steering procedure
  //
  /*
   * Consider having an internal stuct Cache {.... };
   * and then pass it to methods to keep track
   */
};

} // end of namespace bracket

#endif
