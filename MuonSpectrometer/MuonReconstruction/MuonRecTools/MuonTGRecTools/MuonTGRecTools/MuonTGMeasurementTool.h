/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTGMeasurementTool.h, (c) ATLAS Detector Softwareop 
///////////////////////////////////////////////////////////////////

#ifndef MUONTGRECTOOLS_MUONTGMEASUREMENTTOOL_H
#define MUONTGRECTOOLS_MUONTGMEASUREMENTTOOL_H

// Base class
#include "MuonTGRecTools/IMuonTGMeasTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkTrack/Track.h"
#include "TrkExInterfaces/INavigator.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkTrack/TrackCollection.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkPrepRawData/PrepRawData.h"
#include <fstream>

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonTGHits.h"
#include "MuonTGSegments.h"

#include "GaudiKernel/ConcurrencyFlags.h"

//Amg
#include "EventPrimitives/EventPrimitives.h"

namespace MuonGM{
  class MuonDetectorManager;
}

namespace Muon{

 /** @class MuonTGMeasurementTool

     contain application tools for (muon) tracking geometry
          
     @author Sarka.Todorova@cern.ch
     */
 
class MuonTGMeasurementTool : public AthAlgTool,
                              virtual public IMuonTGMeasTool  {

public:

  typedef Trk::Track Track;
  typedef std::pair<const Trk::Layer*,std::vector<Identifier>*> PairOfLayerID;
  typedef std::pair<const Trk::Layer*,std::vector<const Trk::PrepRawData*>*> PairOfLayerPrd;

  /** Constructor with AlgTool parameters */
  MuonTGMeasurementTool(const std::string &name, const std::string& ,const IInterface*);

  /** Virtual destructor */
  virtual ~MuonTGMeasurementTool() {};

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  void updateAssocMeas( MuonTGHits*, MuonTGSegments* ) const;
  const std::vector<const Trk::PrepRawData*>* getMeasurementOnLayer(const Trk::Layer* lay) const;
  const std::vector<const Trk::PrepRawData*>* getEtaPhiMeasurementOnLayer(const Trk::Layer* lay, bool phi) const;
  const std::vector<const Trk::Segment*>* getSegments(const Trk::DetachedTrackingVolume* station) const;
  const MuonTGSegments* getAllSegments() const;
  const MuonTGHits* getAllHits() const;
  const Trk::TrackParameters* layerToDetEl(const Trk::Layer*, const Trk::TrackParameters*, Identifier ) const;
  const Trk::TrackParameters* detElToLayer(const Trk::Layer*, const Trk::TrackParameters*, Identifier ) const;
  const Trk::RIO_OnTrack* measToLayer(const Trk::Layer*, const Trk::TrackParameters*, const Trk::RIO_OnTrack* ) const;
  double residual(const Trk::Layer* , const Trk::TrackParameters*, const Trk::RIO_OnTrack*) const;
  double residual(const Trk::Layer* , const Trk::TrackParameters*, Identifier) const;
  double residual(const Trk::TrackParameters*, const Trk::RIO_OnTrack*) const;
  double residual(const Trk::TrackParameters*, Identifier&) const;
  const Identifier nearestDetEl(const Trk::Layer*, const Trk::TrackParameters*, bool measPhi, double& pitch) const; 
  const Trk::Layer* associatedLayer(Identifier id, Amg::Vector3D& gp) const; 
  const Trk::Layer* associatedLayer(Identifier id, const Trk::TrackingVolume* vol) const; 
  const Trk::Layer* match(Identifier id, const Trk::Layer* lay) const; 
 
private:

  // --- job options
  Gaudi::Property<std::string>  m_trackingGeometryName{this,"TrackingGeometryName","AtlasTrackingGeometry"};
  Gaudi::Property<std::string> m_ExtrapolatorName{this,"ExtrapolatorName"," "};      //!< Name of the Extrapolator Instance 
    
  ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
    "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};    
  const MuonGM::MuonDetectorManager* m_muonDetMgr;
 
  // -- algorithm members
  mutable MuonTGHits*              m_hits           ATLAS_THREAD_SAFE; //Marked as thread-safe because it's disabled when running multi-threaded
  mutable MuonTGSegments*     m_segments   ATLAS_THREAD_SAFE; //Marked as thread-safe because it's disabled when running multi-threaded

  // projection matrices
  AmgMatrix(5,5)                  *m_tgcProjEta;
  AmgMatrix(5,5)                  *m_tgcProjPhi;
  AmgMatrix(5,5)                  *m_rpcProjEta;
  AmgMatrix(5,5)                  *m_rpcProjPhi;

  // steering
  Gaudi::Property<bool>  m_alignedMode{this,"AlignedMode",true};
  Gaudi::Property<bool>  m_useDSManager{this,"UseDSManager",false};

};

} 

inline void Muon::MuonTGMeasurementTool::updateAssocMeas( Muon::MuonTGHits* assocHits,
                                                          Muon::MuonTGSegments* assocSegments ) const
{ 
  if (Gaudi::Concurrency::ConcurrencyFlags::concurrent()) {
    ATH_MSG_WARNING("Access to Muon::MuonTGMeasurementTool::updateAssocMeas() blocked due to thread safety concerns");
  } else {
    m_hits = assocHits;
    m_segments = assocSegments; 
  }
} 

#endif //MUONTGRECTOOLS_MUONTGMEASUREMENTTOOL_H

