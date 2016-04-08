/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigTRTSegFinder_H
#define TrigTRTSegFinder_H

#include <string>
//#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetRecToolInterfaces/ITRT_TrackSegmentsMaker.h"
#include "InDetRecToolInterfaces/ITRT_TrackExtensionTool.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrkSegment/SegmentCollection.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "TrigInDetToolInterfaces/ITrigTRT_DriftCircleProvider.h"
#include "RegionSelector/IRegSelSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 

//class IRegSelSvc;
//class IROBDataProviderSvc;
//

namespace MagField {	
	class IMagFieldSvc;
}
namespace Trk 
{
  class MagneticFieldProperties;
}

//namespace InDet {

class TrigTRTSegFinder :  public HLT::FexAlgo {
  
 public:
  
  TrigTRTSegFinder(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~TrigTRTSegFinder() {};

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, 
			    HLT::TriggerElement* outputTE);
 
  MsgStream&    dump     (MsgStream&    out) const;
  std::ostream& dump     (std::ostream& out) const;
  
 protected:
  
  //  IROBDataProviderSvc*                 m_robDataProvider   ;
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider ;
  
  const InDet::TRT_DriftCircleContainer* m_trtcontainer; // TRTs   container  
  ToolHandle<ITrigTRT_DriftCircleProviderTool>             m_trtDCtool       ; //

  //IRegSelSvc* m_pRegionSelector;
  ServiceHandle<IRegSelSvc> m_pRegionSelector;
  int                         m_nprint          ; // Kind of  print
  int                         m_nsegments       ; // Number segments
  ToolHandle<InDet::ITRT_TrackSegmentsMaker>    m_segmentsmaker   ; // TRT segments maker
  ToolHandle<InDet::ITRT_TrackExtensionTool>    m_trackextension   ; // TRT track extension tool
  ToolHandle<Trk::IPropagator>                  m_proptool       ; // Propagator     tool 
  Trk::MagneticFieldProperties* m_magFieldProperties; // Magnetic field properties
  double               m_roiDeltaPhi               ;
  double               m_roiDeltaEta               ;
  double               m_pTmin;  

  TrigTimer* m_totalTimer;
  TrigTimer* m_relSelTimer;
  TrigTimer* m_segFindTimer;
  TrigTimer* m_trkConvTimer;
  TrigTimer* m_DCTimer;

  Trk::MagneticFieldProperties*  getMagneticFieldProperties();
  TrigInDetTrackCollection* m_recoTracks;

  void ConvertTRT( const Trk::SegmentCollection&);

 //tracking quantities to be monitored
  float nStraw_TRT; 
  float nTR_TRT;
  float Chi2;  
  int m_ntracks;
  std::vector<int> m_dataErrors;
  std::vector<int> m_trtDataErrors;

  //functions to retrieve monitored quantities
  static inline double monPt(const TrigInDetTrack *t){return t->param()->pT(); }
  static inline double mona0(const TrigInDetTrack *t){return t->param()->a0(); }
  static inline double monz0(const TrigInDetTrack *t){return t->param()->z0(); }
  static inline double monPhi(const TrigInDetTrack *t){return t->param()->phi0();}
  static inline double monEta(const TrigInDetTrack *t){return t->param()->eta(); }


  MsgStream&    dumptools(MsgStream&    out) const;
  MsgStream&    dumpevent(MsgStream&    out) const;
 private:
	ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;       //!< helper tool to get the magnetic field
};

MsgStream&    operator << (MsgStream&   ,const TrigTRTSegFinder&);
std::ostream& operator << (std::ostream&,const TrigTRTSegFinder&); 
//}
#endif // TrigTRTSegFinder_H
