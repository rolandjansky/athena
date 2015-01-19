/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonHolesOnTrackTool.h, (c) ATLAS Detector Softwareop 
///////////////////////////////////////////////////////////////////

#ifndef MUONTGRECTOOLS_MUONHOLESONTRACKTOOL_H
#define MUONTGRECTOOLS_MUONHOLESONTRACKTOOL_H

// Base class
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrkTrack/Track.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkTrack/TrackCollection.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkPrepRawData/PrepRawData.h"

#include "MuonTrackMakerUtils/TrackStateOnSurfaceComparisonFunction.h"
#include <fstream>

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonTGRecTools/IMuonTGMeasTool.h"

namespace Muon{

 /** @class MuonHolesOnTrackTool

     contain application tools for (muon) tracking geometry
          
     @author Sarka.Todorova@cern.ch
     */
 
class MuonHolesOnTrackTool : public AthAlgTool,
                             virtual public Trk::ITrackHoleSearchTool {  

public:

  typedef Trk::Track Track;

  /** Constructor with AlgTool parameters */
  MuonHolesOnTrackTool(const std::string &name, const std::string& ,const IInterface*);

  /** Virtual destructor */
  virtual ~MuonHolesOnTrackTool() {};

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  void retrieveMeasurements();
  DataVector<const Trk::TrackStateOnSurface>* getHolesOnTrack(const Track& , const Trk::ParticleHypothesis) const;
  const Track*                                getTrackWithHoles(const Track& , const Trk::ParticleHypothesis ) const;
  const Track*                                getTrackWithHolesAndOutliers(const Track& , const Trk::ParticleHypothesis ) const;
  //const TrackCollection                     getTrackCollectionWithHoles(const TrackCollection );
  void                                        countHoles( const Trk::Track& track,  std::vector<int>& information , const Trk::ParticleHypothesis) const;

private:

  const Trk::TrackParameters* getMSEntry(const Track*, DataVector<const Trk::TrackStateOnSurface>*& ) const;  
  const Trk::TrackParameters* findAssociatedMeasurement(const Trk::TrackParameters* ,const Trk::Layer* layer,
                                                        const DataVector<const Trk::TrackStateOnSurface>*,int&, double& ) const;
  const Trk::TrackStateOnSurface* createHole(const Trk::TrackParameters*,const Trk::Layer*, bool) const;
  const Trk::TrackStateOnSurface* createOutlier(const Trk::TrackParameters*,const Trk::Layer*, bool) const;
  const Trk::Layer* nextExpectedLayer(const Trk::TrackParameters* currPar,
				      const DataVector<const Trk::TrackStateOnSurface>* tSoS) const;
   
  // --- job options
  std::string  m_trackingGeometryName;           // dummy, to be removed
  double       m_outlierLim;
  bool         m_parmsUpdate;
  bool         m_identifyHoles;
  double       m_softLowerCut;
  unsigned int m_stopSearch;         

  ToolHandle<Muon::IMuonTGMeasTool>   m_measTool;
  ToolHandle<Trk::IExtrapolator>      m_extrapolator;
  ToolHandle<Trk::IRIO_OnTrackCreator> m_rotCreator;
  
  // --- steering
  std::vector<double>               m_sortingRefPoint;    //!< start point to detect min TP, only if ROTfit
  mutable Muon::TrackStateOnSurfaceComparisonFunction*   m_tSoSOrder;
  
  const MdtIdHelper* m_mdtIdHelper;
  const RpcIdHelper* m_rpcIdHelper;
  const CscIdHelper* m_cscIdHelper;
  const TgcIdHelper* m_tgcIdHelper;
  const MuonGM::MuonDetectorManager* m_muonMgr;
  
  // -- algorithm members
  StoreGateSvc                    *m_StoreGate;         //!< transient event store providing the tracks
  StoreGateSvc                    *m_detStore;       

  mutable const  Trk::TrackingVolume*       m_msEntrance;
  
  bool    m_parUpdate;                                  // steering - extrapolation parameters update when looping over layers

  mutable std::vector<const Trk::TrackParameters*> m_garbage;
};

}  // end namespace

#endif //MUONTGRECTOOLS_MUONHOLESONTRACKTOOL_H

