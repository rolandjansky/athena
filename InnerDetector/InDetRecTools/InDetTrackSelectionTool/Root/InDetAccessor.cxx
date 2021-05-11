/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "InDetAccessor.h"
#ifndef XAOD_ANALYSIS
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"

namespace InDetAccessor {
   std::tuple<uint8_t,uint8_t> getSiHitsTopBottom( const Trk::Track& track, asg::AsgMessaging &msgHelper) {
      std::tuple<uint8_t, uint8_t> top_bottom {0,0};

      const DataVector<const Trk::MeasurementBase>* trkMeasurements = track.measurementsOnTrack();
      if (!trkMeasurements) {
         IDTRKSEL_MSG_WARNING( "Null pointer to Trk::MeasurementBase vector." );
      }
      else {
         // code in here is mostly adapted from InDetCosmicTrackSelectorTool
         for (const Trk::MeasurementBase* measurement : *trkMeasurements) {
            const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(measurement);
            if (!rot) {
               continue;
            }
            const InDet::SiClusterOnTrack* siclus
               = dynamic_cast<const InDet::SiClusterOnTrack*>(rot);
            if (!siclus) {
               continue;
            }
            float ypos = siclus->globalPosition().y();
            if (ypos == 0) {
               continue; // neither top nor bottom
            }

            const InDet::SCT_ClusterOnTrack* sctclus
               = dynamic_cast<const InDet::SCT_ClusterOnTrack*>(siclus);
            if (!sctclus) {
               // Pixel hit
               if (ypos > 0) {
                  std::get<0>(top_bottom) +=2;
               }
               else {
                  std::get<1>(top_bottom)+=2;
               }
            } else {
               // SCT hit
               if (ypos > 0) {
                  std::get<0>(top_bottom)++;
               }
               else {
                  std::get<1>(top_bottom)++;
               }
            }
         }
      }
      return top_bottom;
   }
}
#endif
