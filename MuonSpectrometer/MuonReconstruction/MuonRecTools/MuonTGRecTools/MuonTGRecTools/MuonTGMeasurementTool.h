/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTGMeasurementTool.h, (c) ATLAS Detector Softwareop 
///////////////////////////////////////////////////////////////////

#ifndef MUONTGRECTOOLS_MUONTGMEASUREMENTTOOL_H
#define MUONTGRECTOOLS_MUONTGMEASUREMENTTOOL_H

// Base class
#include "MuonTGRecTools/IMuonTGMeasTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"
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

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonTGHits.h"
#include "MuonTGSegments.h"

//Amg
#include "EventPrimitives/EventPrimitives.h"

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

  StatusCode getTrackingGeometry() const;
  
  // --- job options
  mutable const Trk::TrackingGeometry* m_trackingGeometry;
  std::string  m_trackingGeometryName;
  std::string m_ExtrapolatorName;         //!< Name of the Extrapolator Instance 
    
  const MdtIdHelper* m_mdtIdHelper;
  const RpcIdHelper* m_rpcIdHelper;
  const CscIdHelper* m_cscIdHelper;
  const TgcIdHelper* m_tgcIdHelper;
  const MuonGM::MuonDetectorManager* m_muonMgr;
  
  // -- algorithm members
  StoreGateSvc                    *m_StoreGate;         
  ActiveStoreSvc*                  m_activeStore;
  const Trk::IExtrapolator        *m_extrapolator;
  mutable MuonTGHits*              m_hits; 
  mutable MuonTGSegments*          m_segments;

  // projection matrices
  mutable AmgMatrix(5,5)                  *m_tgcProjEta;
  mutable AmgMatrix(5,5)                  *m_tgcProjPhi;
  mutable AmgMatrix(5,5)                  *m_rpcProjEta;
  mutable AmgMatrix(5,5)                  *m_rpcProjPhi;

  // steering
  bool  m_alignedMode;

};

} 

inline void Muon::MuonTGMeasurementTool::updateAssocMeas( Muon::MuonTGHits* assocHits,
                                                          Muon::MuonTGSegments* assocSegments ) const
{ m_hits = assocHits; m_segments = assocSegments; } 

#endif //MUONTGRECTOOLS_MUONTGMEASUREMENTTOOL_H

