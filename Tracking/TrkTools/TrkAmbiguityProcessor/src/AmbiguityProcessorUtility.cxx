/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AmbiguityProcessorUtility.h"
#include "AthContainers/DataVector.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkTrack/Track.h"


namespace AmbiguityProcessor{
  TrackFilterCategory
  categoriseTrack(const Trk::Track & track, const Trk::TrackScore & score, const bool dropDuplicates, const AssociationTool & associate, AssociationMap & map, DuplicationCheckSet & set){
    if (score == 0) return  TrackFilterCategory::ScoreIsZero;
    if (dropDuplicates){
      if(const auto & [p, uniqueTrack] = set.insert(associate->getPrdsOnTrack( map, track)); not uniqueTrack) return TrackFilterCategory::TrackIsDuplicate;
    }
    return TrackFilterCategory::TrackAccepted;
  }
  //
  float 
  calculateFitQuality(const Trk::Track & track){
    float result{0.0};
    if  (const auto quality=track.fitQuality(); quality and quality->numberDoF()>0 ){ 
      result = quality->chiSquared()/quality->numberDoF();
    }
    return result; 
  }
  //
  std::unique_ptr<Trk::Track>
  createNewFitQualityTrack(const Trk::Track & track){
    double reXi2 = 0.; 
    int nDF = 0;
    const DataVector<const Trk::TrackStateOnSurface>* tsos = track.trackStateOnSurfaces();
    DataVector<const Trk::TrackStateOnSurface>* vecTsos = new DataVector<const Trk::TrackStateOnSurface>();
    // loop over TSOS, copy TSOS and push into vector
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end(); 
    for ( ; iTsos != iTsosEnd ; ++iTsos) {
      const Trk::TrackStateOnSurface* newTsos = new Trk::TrackStateOnSurface(**iTsos);
      vecTsos->push_back(newTsos);
      if((*iTsos)->type(Trk::TrackStateOnSurface::Measurement)){  //Get the chi2 and number of hits
        if ((*iTsos)->fitQualityOnSurface()) {
          reXi2 += (*iTsos)->fitQualityOnSurface()->chiSquared();
          nDF   += (*iTsos)->fitQualityOnSurface()->numberDoF();
        }
      }
    }
    Trk::FitQuality* fq = new Trk::FitQuality(reXi2,nDF-5);
    Trk::TrackInfo info;
    info.addPatternRecoAndProperties(track.info());
    Trk::TrackInfo newInfo;
    newInfo.setPatternRecognitionInfo(Trk::TrackInfo::SimpleAmbiguityProcessorTool);
    info.addPatternReco(newInfo); 
    return std::make_unique<Trk::Track>(info, vecTsos, fq);
  }

}
