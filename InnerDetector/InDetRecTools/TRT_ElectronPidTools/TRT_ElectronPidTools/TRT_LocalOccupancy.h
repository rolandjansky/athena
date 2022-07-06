/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "TRT_ConditionsData/AliveStraws.h"
#include "TRT_ConditionsData/StrawStatusData.h"
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

   /** standard Athena-Algorithm method */
   virtual StatusCode initialize() override;

   /** Return the local occupancy for the sectors crossed by a given track */
   virtual float LocalOccupancy(const EventContext& ctx,
                                const Trk::Track& track) const override;
   virtual float LocalOccupancy(const EventContext& ctx,
                                const double eta,
                                const double phi) const override;

   /** Return a map of the occupancy in the barrel (-1,+1) and endcaps (-2,+2) */
   virtual std::map<int, double> getDetectorOccupancy(const EventContext& ctx,
                                                      const TRT_RDO_Container* p_trtRDOContainer) const override;

   /** Return the global occupancy of the event*/
   /** 7 Floats: TRT, Barrel A / C, endcapA/B A/C */
   virtual std::vector<float> GlobalOccupancy(const EventContext& ctx) const override;

   //Total TRT, (B, ECA, ECB)side C, (B, ECA, ECB)side A [7]
   static const int NTOTAL = 7;
   //(barrel, ECA, ECB)side C, (barrel, ECA, ECB)side A [6][32]
   static const int NLOCAL = 6;
   static const int NWHEEL = 34;
   static const int NLOCALPHI = 32;
   struct OccupancyData {
      OccupancyData(const std::array<std::array<int,NLOCALPHI>,NLOCAL> &local)
         : m_stw_local(local)
      {}
     int m_occ_total[NTOTAL] = {0};
     int m_hit_total[NTOTAL] = {0};
     int m_occ_local[NLOCAL][NLOCALPHI] = {{0}};
     int m_hit_local[NLOCAL][NLOCALPHI] = {{0}};
     const std::array<std::array<int,NLOCALPHI>,NLOCAL> &m_stw_local;
     float m_stws_ratio[2][NLOCALPHI] = {{0}};
   };


  private:
      const OccupancyData* getData(const EventContext& ctx) const;
      std::unique_ptr<OccupancyData> makeData(const EventContext& ctx) const;
      std::unique_ptr<OccupancyData> makeDataTrigger(const EventContext &ctx) const;

      static bool isMiddleBXOn(unsigned int word) ;
      bool passValidityGate(unsigned int word, float t0) const;

      void countHitsNearTrack (const EventContext &ctx,
                               OccupancyData& data,
                               int track_local[NLOCAL][NLOCALPHI]) const;

   /** To convert from array index to det id and viceversa */
   int findArrayTotalIndex(const int det, const int lay) const;
   static int mapPhiToPhisector(const double phi) ;
   int mapEtaToPartition(const double eta) const;

   /** External tools:  */
   const TRT_ID *m_TRTHelper{};
   ToolHandle< ITRT_CalDbTool > m_CalDbTool{this, "TRTCalDbTool", "TRT_CalDbTool", ""};
   SG::ReadHandleKey<TRT_DriftCircleContainer> m_trt_driftcircles{ this, "TRT_DriftCircleCollection", "TRT_DriftCircles", "m_trt_driftcircles" };

   SG::ReadCondHandleKey<TRTCond::AliveStraws> m_strawReadKey{this,"AliveStraws","AliveStraws","AliveStraws in-key"};
   SG::ReadCondHandleKey<TRTCond::StrawStatusData> m_strawStatusKey{this,"StrawStatus","StrawStatusData","StrawStatus key"};
   SG::ReadCondHandleKey<TRTCond::StrawStatusData> m_strawStatusPermKey{this,"StrawStatusPerm","StrawStatusPermanentData","StrawStatusPermanent key"};

   SG::ReadHandleKey<OccupancyData> m_occupancyCacheRead{"OccupancyData"};
   SG::WriteHandleKey<OccupancyData> m_occupancyCacheWrite{"OccupancyData"};

   Gaudi::Property<bool> m_isTrigger{this, "isTrigger", false, ""};
   Gaudi::Property<bool> m_T0Shift{this, "includeT0Shift", true, "choice to use T0shift or not"};
   Gaudi::Property<float> m_lowGate{this, "LowGate", 14.0625*Gaudi::Units::ns, ""};
   Gaudi::Property<float> m_highGate{this, "HighGate", 42.1875*Gaudi::Units::ns, ""};
   // use a wider validity gate if you're not using T0 shift:
   Gaudi::Property<float> m_lowWideGate{this, "LowWideGate", 20.3125*Gaudi::Units::ns, ""};
   Gaudi::Property<float> m_highWideGate{this, "HighWideGate", 54.6875*Gaudi::Units::ns, ""};

  };
}


CLASS_DEF (InDet::TRT_LocalOccupancy::OccupancyData, 143585992, 0)
#endif
