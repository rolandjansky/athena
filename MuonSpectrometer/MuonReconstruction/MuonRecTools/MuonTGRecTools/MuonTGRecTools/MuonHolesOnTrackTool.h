/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGRECTOOLS_MUONHOLESONTRACKTOOL_H
#define MUONTGRECTOOLS_MUONHOLESONTRACKTOOL_H

// Base class
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
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

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
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
  Gaudi::Property<double>         m_outlierLim{this,"OutlierResidualLimit",50.};
  Gaudi::Property<bool>            m_identifyHoles{this,"DoHolesIdentification",true};
  Gaudi::Property<double>         m_softLowerCut{this,"LowerTrackMomentumCut",0.};
  Gaudi::Property<unsigned int> m_stopSearch{this,"StopHolesSearchMode",1};         

  ToolHandle<Muon::IMuonTGMeasTool>   m_measTool{this,"MuonTGMeasurementTool","Muon::MuonTGMeasurementTool/MuonTGMeasurementTool"};
  ToolHandle<Trk::IExtrapolator>             m_extrapolator{this,"ExtrapolatorName","Trk::Extrapolator/Extrapolator"};
  ToolHandle<Trk::IRIO_OnTrackCreator> m_rotCreator{this,"RIO_OnTrackCreator","Trk::RIO_OnTrackCreator/RIO_OnTrackCreator"};
  
  // --- steering
  std::vector<double>               m_sortingRefPoint;    //!< start point to detect min TP, only if ROTfit
  
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  
  const  Trk::TrackingVolume*       m_msEntrance;
  
  Gaudi::Property<bool>    m_parUpdate{this,"DoParameterUpdate",false};  // steering - extrapolation parameters update when looping over layers

};

}  // end namespace

#endif //MUONTGRECTOOLS_MUONHOLESONTRACKTOOL_H

