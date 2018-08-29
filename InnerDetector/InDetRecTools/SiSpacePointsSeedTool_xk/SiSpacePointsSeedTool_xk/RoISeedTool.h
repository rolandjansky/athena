/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TrkSeedTools
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for track candidates generation using space points information
// for standard Atlas geometry
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedTool_xk_RoISeedTool_h
#define SiSpacePointsSeedTool_xk_RoISeedTool_h

#include "InDetRecToolInterfaces/IRoISeedTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

class MsgStream;

namespace InDet {

  class RoISeedTool : 
   virtual public IRoISeedTool ,public AthAlgTool
  {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    RoISeedTool(const std::string&,const std::string&,const IInterface*);
    virtual ~RoISeedTool();
    virtual StatusCode               initialize();
    virtual StatusCode               finalize  ();

    // Interface method to the user
    // Algoirthm will grab all measurements 
    // from tracks and/or vertices
    std::vector<Amg::Vector3D> getRoIs();

  protected:

    /**    @name Disallow default instantiation, copy, assignment **/
    RoISeedTool() = delete;
    RoISeedTool(const RoISeedTool&) = delete;
    RoISeedTool &operator=(const RoISeedTool&) = delete;

    void getTrkMeasSeeds(std::vector<Amg::Vector3D>& trkseeds);
    void getVtxSeeds(std::vector<Amg::Vector3D>& vtxseeds);

    // Misc methods
    float getTrackPtCone(const TrackCollection* std_tracks,const Trk::Perigee* perigee);


    ///////////////////////////////////////////////////////////////////
    // Protected data and methods
    ///////////////////////////////////////////////////////////////////
  
    bool                              m_doTrkMethod;
    bool                              m_doVtxMethod;

    std::string                       m_RoiSeedTracks;
    std::string                       m_tracksForIsolation;

    float                             m_RoISeedTrackD0;
    float                             m_RoISeedTrackPt;
    int                               m_RoISeedTrackSCTHits;
    int                               m_RoISeedTrackPixHits;
    float                             m_RoISeedTrackIso;
    float                             m_IsoTrackPtThr;
    float                             m_IsoTrackConeSize;
    bool                              m_RoISeedTrackD0Sort;
 

  };
}

class rankD0  {
 public:
  bool operator ()
    (const std::pair<float,Amg::Vector3D>& i1,
     const std::pair<float,Amg::Vector3D>& i2)
  {
    return fabs(i1.first) < fabs(i2.first);
  }
};

#endif // SiSpacePointsSeedMaker_TrkSeeded_H

