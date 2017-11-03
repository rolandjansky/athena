/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////////////////
//
//    TRT_TrackSegmentsMaker_BarrelCosmics.h, (c) ATLAS Detector software 
//
//    TRT standalone segment finder for TRT barrel
//    specialized for cosmics and L2 trigger, plan to make it working in a magnetic field
//
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_TrackSegmentsMaker_BarrelCosmics_H
#define TRT_TrackSegmentsMaker_BarrelCosmics_H

#include <list>
#include <map>
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetRecToolInterfaces/ITRT_TrackSegmentsMaker.h"
#include "TrkParameters/TrackParameters.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"

#include "StoreGate/ReadHandleKey.h"

class MsgStream;


namespace InDet{

  /** @class TRT_TrackSegmentsMaker_BarrelCosmics

      AlgTool that creates TrackSegments out of TRT Driftcircles 
      for barrel TRT only, specializing for cosmic runs (to be used for LVL2 trigger)
	  
      @author  Sasa.Fratina@cern.ch 
   */
 
  class TRT_TrackSegmentsMaker_BarrelCosmics : virtual public ITRT_TrackSegmentsMaker, public AthAlgTool {
  
    public:
	
      TRT_TrackSegmentsMaker_BarrelCosmics(const std::string &, const std::string &, const IInterface*);
      virtual ~TRT_TrackSegmentsMaker_BarrelCosmics() {};

      ///////////////////////////////////////////////////////////////////////////////
      //  methods inherited from ITRT_TrackSegmentsMaker
      //  in each event, methods are called in this order:
      //  -  newEvent() in the offline, newRegion(...) for L2 (directed search)  	  
      //  -  find() finds all segments
      //  -  next() many times (returns next segment), untill next() returns 0 pointer
      ///////////////////////////////////////////////////////////////////////////////
		  
      virtual StatusCode               initialize();
//      StatusCode magneticFieldInit(IOVSVC_CALLBACK_ARGS);
      virtual StatusCode               finalize  ();

      void newEvent();
      void newRegion(const std::vector<IdentifierHash>&);
      void endEvent();
      
      virtual void find();
      virtual Trk::TrackSegment *next();

      MsgStream&    dump          (MsgStream   & out) const { return out; };
      std::ostream& dump          (std::ostream& out) const { return out; };
	  
    private:

      // other methods specific for this algorithm	
      void clear(); // to be called by both newEvent and newRegion (clear data members) - use endEvent instead?
	
      int findSeed(double xmin, double xmax, double phimin, double phimax, double *bestParameters);
      void findSeedInverseR(double *par);

      // follow 
      // convert lists of TRT circles (std::vector<const InDet::TRT_DriftCircle *>) to track segments (Trk::TrackSegment * -> fill m_segments):
      void linearRegressionParabolaFit(double *mean, double *a); // input: elsewhere calculated mean, output: result a
      /** sort hits on segment such that they are ordered from larger y to smaller (from top down) */
      static bool sortHits( const InDet::TRT_DriftCircle *, const InDet::TRT_DriftCircle * ); 
      void convert(std::vector<const InDet::TRT_DriftCircle *> &hits, double *trackpar);
	  
      void segFit(double *measx, double *measy, int nhits, double *residuals = 0, double *result = 0); 
	  void findOld();  // old function - need to debug an increased cpu consumption
	  
      MSG::Level m_debugLevel;

      int m_maxTotalHits; // protect against high occupancy events
	  	  
      int m_minHitsForSeed; // 10-12 is good
      int m_minHitsForSegment; // 20 	  
      int m_minHitsAboveTOT; // 15
      int m_nBinsInX; // 100
      int m_nBinsInPhi; // 10
	  
      double m_minSeedTOT; // minimal time over threshold for seed search - default ~ 10 ns
      bool m_magneticField; // search for lines (if False) or circles (if True)
      bool m_mergeSegments; // default: False, should not be turned on yet

      bool m_useAthenaFieldService;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected data and methods
      ///////////////////////////////////////////////////////////////////
      
      std::vector< const InDet::TRT_DriftCircle * > m_listHits ;
      
      std::vector< Amg::Vector3D >             m_listHitCenter ;
	  

      SG::ReadHandleKey<InDet::TRT_DriftCircleContainer>     m_driftCirclesName{this,"TRT_ClustersContainer","TRT_DriftCircles","RHK to retrieve Drift Circles"} ;  //!< Container with TRT clusters
      std::string                              m_TRTManagerName ; //!< Name of TRT det. manager 	  
	  
      const TRT_ID*                            m_trtid       ; 

      SG::ReadHandleKey<InDet::TRT_DriftCircleContainer> m_trtname{this,"TRTDriftCircleCollection","TRT_DriftCircles","RHK to retrieve TRT_DriftCircles"}; //!< TRTs   container
      ToolHandle<Trk::IPRD_AssociationTool>    m_assoTool    ; // Track-PRD association tool
      bool                                     m_useAssoTool ;

      unsigned int                             m_segmentDriftCirclesCount;
      std::vector< std::vector<const InDet::TRT_DriftCircle *> > m_segmentDriftCircles; // vector of DriftCircles associated to the segments
      std::vector< Trk::TrackSegment * >       m_segments    ; //!< List of found segments

  };

} // end namespace InDet 

#endif // TRT_TrackSegmentsMaker_BarrelCosmics_H

