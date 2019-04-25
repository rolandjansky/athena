/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_LocalOccupancy.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRT_LOCALOCCUPANCY_H
#define INDETTRT_LOCALOCCUPANCY_H

#include "TRT_ElectronPidTools/ITRT_LocalOccupancy.h"
#include "AthenaBaseComps/AthAlgTool.h"       // Exchange IN
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkTrack/Track.h"
#include "TRT_ConditionsServices/ITRT_CalDbTool.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummaryTool.h"

#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "SGTools/CLASS_DEF.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "TRT_ConditionsData/AliveStraws.h"
#include <vector>

class AtlasDetectorID;
class TRT_ID;

namespace Trk{
	class Track;
}

namespace InDet
{

  /** @class TRT_LocalOccupancy 

      TRT_LocalOccupancy is a tool to compute the TRT occupancy for the elements crossed by a track.

      This tool has to be called with: StartEvent() before checking occupancy for a given track, so the different
      volumes are computed.

      Then, for each track call LocalOccupancy( Track ) and a  will be returned. 

      @author  Alejandro Alonso: <Alejandro.Alonso@cern.ch>
  */

  class TRT_LocalOccupancy : public extends <AthAlgTool, ITRT_LocalOccupancy> {
  public:
   TRT_LocalOccupancy(const std::string&,const std::string&,const IInterface*);

   /** default destructor */
   virtual ~TRT_LocalOccupancy ();
      
   /** standard Athena-Algorithm method */
   virtual StatusCode initialize() override;

   /** standard Athena-Algorithm method */
   virtual StatusCode finalize  () override; 

   /** Return the local occupancy for the sectors crossed by a given track */ 
   virtual float LocalOccupancy( const Trk::Track& track) const override;
   virtual float LocalOccupancy(const double eta, const double phi) const override;

   /** Return a map of the occupancy in the barrel (-1,+1) and endcaps (-2,+2) */
   virtual std::map<int, double> getDetectorOccupancy( const TRT_RDO_Container* p_trtRDOContainer ) const override;

   /** Return the global occupancy of the event*/ 
   /** 7 Floats: TRT, Barrel A / C, endcapA/B A/C */ 
   virtual std::vector<float> GlobalOccupancy( ) const override;


   //Total TRT, (B, ECA, ECB)side C, (B, ECA, ECB)side A [7]
   static const int NTOTAL = 7;
   //(barrel, ECA, ECB)side C, (barrel, ECA, ECB)side A [6][32]
   static const int NLOCAL = 6;
   static const int NLOCALPHI = 32;
   struct OccupancyData {
     int m_occ_total[NTOTAL] = {0};
     int m_hit_total[NTOTAL] = {0};
     int m_occ_local[NLOCAL][NLOCALPHI] = {{0}};
     int m_hit_local[NLOCAL][NLOCALPHI] = {{0}};
     int* m_stw_total = nullptr;
     int** m_stw_local = nullptr;
     int** m_stw_wheel = nullptr;
     float m_stws_ratio[2][NLOCALPHI] = {{0}};
   };


  private:
      const OccupancyData* getData() const;
      std::unique_ptr<OccupancyData> makeData() const;
      std::unique_ptr<OccupancyData> makeDataTrigger() const;

      bool isMiddleBXOn(unsigned int word) const;
      bool passValidityGate(unsigned int word, float t0) const;

      void  countHitsNearTrack (OccupancyData& data,
                                int track_local[NLOCAL][NLOCALPHI]) const;

      //   void  countHitsNearTrack(std::vector<IdentifierHash>* hash_vec);
  //   void  countHitsNearTrack(IdentifierHash hash);

  

   /** To convert from array index to det id and viceversa */
   int findArrayTotalIndex(const int det, const int lay) const;
   int mapPhiToPhisector(const double phi) const;
   int mapEtaToPartition(const double eta) const;

   /** External tools:  */
   const TRT_ID *m_TRTHelper;
   ToolHandle< ITRT_CalDbTool > m_CalDbTool; //!< CalDbTool
   ToolHandle< ITRT_StrawStatusSummaryTool > m_StrawStatusSummaryTool; //!< StrawStatusSummaryTool   
   SG::ReadHandleKey<TRT_RDO_Container> m_trt_rdo_location{ this, "TRT_RDOContainerName", "TRT_RDOs", "m_trt_rdo_location" };
   SG::ReadHandleKey<TRT_DriftCircleContainer> m_trt_driftcircles{ this, "TRT_DriftCircleCollection", "TRT_DriftCircles", "m_trt_driftcircles" };
   SG::ReadCondHandleKey<TRTCond::AliveStraws> m_strawReadKey{this,"AliveStraws","AliveStraws","AliveStraws in-key"};

   bool m_isTrigger;
   bool m_T0Shift; // choice to use T0shift or not
   float m_lowGate;
   float m_highGate;
   // use a wider validity gate if you're not using T0 shift:
   float m_lowWideGate; 
   float m_highWideGate;
   }; 
}


CLASS_DEF (InDet::TRT_LocalOccupancy::OccupancyData, 143585992, 0)
#endif 
