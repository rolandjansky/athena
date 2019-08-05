/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRKVERTEXSEEDFINDERTOOLS_INDEXCROSSDISTANCESSEEDFINDER_H
#define TRKVERTEXSEEDFINDERTOOLS_INDEXCROSSDISTANCESSEEDFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "TrkVertexSeedFinderUtils/IMode3dFinder.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk
{
  class Track;
  class IMode3dFinder;
  class ITrkDistanceFinder;

  // @author N. Giacinto Piacquadio (Albert-Ludwigs-Universitaet Freiburg - Germany)
  //
  // @ATLAS software
  //
  // This class implements a seed finder for the vertexing algorithm 
  // which is based on the mean points of the distances between all tracks 
  // (taken two by two). There's actually no selection on the tracks.
  // 
  // 
  // A very similar point finder was implemented for the first time by Waltenberger 
  // in CMS. I suggest his PhD thesis (2005) as a reference for the different concepts 
  // of point finders.
  //
  // ------------------------------------

  class IndexedCrossDistancesSeedFinder : public AthAlgTool, virtual public IVertexSeedFinder
  {
  public:
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    //default constructor due to Athena interface
    IndexedCrossDistancesSeedFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~IndexedCrossDistancesSeedFinder();


    // Interface for Tracks with starting seed/linearization point
    virtual Amg::Vector3D findSeed(const std::vector<const Trk::Track*> & vectorTrk,const xAOD::Vertex * constraint=0) override;
    
    /** Interface for MeasuredPerigee with starting point */
    virtual Amg::Vector3D findSeed(const std::vector<const Trk::TrackParameters*> & perigeeList,const xAOD::Vertex * constraint=0) override;
        
    virtual Amg::Vector3D
    findSeed(const double vx,
             const double vy,
             const std::vector<const Trk::TrackParameters*>& perigeeList,
             const xAOD::Vertex * constraint=0) override;

    
    virtual Amg::Vector3D
    findSeed(const double vx,
             const double vy,
             std::unique_ptr<Trk::IMode3dInfo>& info,
             const std::vector<const Trk::TrackParameters*>& perigeeList,
             const xAOD::Vertex * constraint=0) override;


    std::vector<Amg::Vector3D> findMultiSeeds(const std::vector<const Trk::Track*>& vectorTrk,
                          const xAOD::Vertex * constraint=0) override;

    std::vector<Amg::Vector3D> findMultiSeeds(
        const std::vector<const Trk::TrackParameters*>& perigeeList,const xAOD::Vertex * constraint=0)  override;

 
  private:

    bool m_useweights;
    float m_trackdistcutoff;
    int m_trackdistexppower;
    float m_constraintcutoff;
    float m_constrainttemp;
    ToolHandle<IMode3dFinder> m_mode3dfinder;
    ToolHandle<ITrkDistanceFinder> m_distancefinder;
    unsigned int m_maximumTracksNoCut;
    double m_maximumDistanceCut;
  };
}
#endif
