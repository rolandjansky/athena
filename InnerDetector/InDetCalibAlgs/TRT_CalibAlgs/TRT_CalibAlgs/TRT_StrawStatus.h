/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_StrawStatus.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRT_STRAWSTATUS_H
#define INDETTRT_STRAWSTATUS_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/IUpdator.h"

#include "xAODEventInfo/EventInfo.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "DataModel/DataVector.h"
#include "TrkTrack/Track.h"
#include "VxVertex/VxContainer.h"
#include "StoreGate/ReadHandleKey.h"

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

class AtlasDetectorID;
class Identifier;

class TRT_ID;
class ITRT_StrawNeighbourSvc;
class ITRT_StrawStatusSummarySvc ;
class ITRT_HWMappingSvc;
class ITRT_DCS_ConditionsSvc;

namespace Trk {
  class ITrackHoleSearchTool;
}


namespace InDet 
{

  /** @class TRT_StrawStatus

      This algorithm finds dead or hot TRT straws / chips / boards based on  
      occupancy and hits on track.
      To be used in calibration stream (to mask off dead regions before processing)
            
      @author  Sasa Fratina <sasa.fratina@cern.ch>
  */  

  class TRT_StrawStatus : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       TRT_StrawStatus(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~TRT_StrawStatus();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:

      void clear();	
   //   void detailedReportResults();
      void reportResults();
      void printDetailedInformation();
	
      /** function that returns straw index (in range 0-5481; 0-1641 for barrel, the rest for endcap) 
	      same convention as for TRT_monitoring (copied from there) */
      void myStrawIndex(Identifier id, int *index);
	  int barrelStrawNumber(int strawNumber, int strawlayerNumber, int LayerNumber);
	  int endcapStrawNumber( int strawNumber, int strawLayerNumber, int LayerNumber );
	  	  
      /** returns index of hardware units: board, chip, pad
		  private fix for now, will call TRTStrawNeighbourSvc when available 
		  number boards 0-9 barrel, 0-19 endcap (first 12 on A wheels, ordering from smaller to larger |z|)
		  number chips 0-103 barrel, 0-239 endcap
		  number pads: chips x 2 */
		  
   //   void myHardwareMapping(Identifier id, int &board, int &chip, int &pad);

      int m_nBarrelStraws; // 1642
      int m_nEndcapStraws; // 3840
      int m_nAllStraws; // 1642+3840=5484
	  
      int m_nBarrelBoards; // 9
      int m_nEndcapBoards; // 20
      int m_nAllBoards; // 29

      int m_nBarrelChips; // 104
      int m_nEndcapChips; // 240
      int m_nAllChips; // 344

      int m_nBarrelPads; // 2 x N of chips
      int m_nEndcapPads; 
      int m_nAllPads; 
	  
      int m_nEvents; // count N of processed events, needed for normalization
      int m_runNumber;
 
      /** accumulate hits, last index: 0 - all hits, 1 - hits on track, 2 - all HT (TR) hits, 3 - HT (TR) hits on track */	 
      int m_accumulateHits[2][32][5482][6];

      const TRT_ID *m_TRTHelper;

      ServiceHandle<ITRT_HWMappingSvc> m_mapSvc;
      ServiceHandle<ITRT_DCS_ConditionsSvc> m_DCSSvc;
      ServiceHandle<ITRT_StrawNeighbourSvc> m_TRTStrawNeighbourSvc;
      ServiceHandle<ITRT_StrawStatusSummarySvc> m_TRTStrawStatusSummarySvc;
      ToolHandle<Trk::ITrackHoleSearchTool>  m_trt_hole_finder;
      ToolHandle<Trk::IUpdator> m_updator;
      double m_locR_cut;

      SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,"EventInfoKey","EventInfo","RHK to retrieve xAOD::EventInfo"};
      SG::ReadHandleKey<TRT_RDO_Container> m_rdoContainerKey{this,"RDO_ContainerKey","TRT_RDOs","RHK to retrieve TRT RDO's"};
      SG::ReadHandleKey<DataVector<Trk::Track>> m_tracksName{this,"tracksCollection","CombinedInDetTracks","RHK to retrieve CombinedInDetTracks"};
      SG::ReadHandleKey<VxContainer> m_vxContainerKey{this,"VxContainerKey","VxPrimaryCandidate","RHK to retrieve VX Primary candidates"};
     
      std::string m_fileName;         
      int m_skipBusyEvents;

 //     int m_accumulateStrawWordFlag;
   //   int m_accumulateStrawWord[2][32][5482][28];

      /** member variables for algorithm properties: */
      // int/double/bool  m_propertyName;
      int m_printDetailedInformation;
    }; 
} // end of namespace

#endif 
