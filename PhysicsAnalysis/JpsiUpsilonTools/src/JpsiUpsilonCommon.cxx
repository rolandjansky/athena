/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JpsiUpsilonTools/JpsiUpsilonCommon.h"
#include "TLorentzVector.h"


namespace Analysis {   
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // getPt: returns the pT of a track pair
    // ---------------------------------------------------------------------------------
    double JpsiUpsilonCommon::getPt(const xAOD::TrackParticle* trk1, const xAOD::TrackParticle* trk2) {
        
        TLorentzVector momentum(trk1->p4() + trk2->p4() ); 
        return momentum.Perp();
        
    }
    


    double JpsiUpsilonCommon::getPt(const xAOD::TrackParticle* trk1,
                                  const xAOD::TrackParticle* trk2,
                                  const xAOD::TrackParticle* trk3)
    {
        TLorentzVector momentum( trk1->p4() );
        momentum+= trk2->p4();
        momentum+= trk3->p4();
        return momentum.Perp();
    }


    // ---------------------------------------------------------------------------------
    // getPt: returns the pT of a track quadruplet
    // ---------------------------------------------------------------------------------
    double JpsiUpsilonCommon::getPt(const xAOD::TrackParticle* trk1,
                                  const xAOD::TrackParticle* trk2,
                                  const xAOD::TrackParticle* trk3,
                                  const xAOD::TrackParticle* trk4)
    {
        TLorentzVector momentum( trk1->p4() );
        momentum += trk2->p4();
        momentum += trk3->p4();
        momentum += trk4->p4();
        return momentum.Perp();
    }

    
    // -------------------------------------------------------------------------------------------------
    // isContainedIn: boolean function which checks if a track (1st argument) is also contained in a
    // vector (second argument)
    // -------------------------------------------------------------------------------------------------
    
    bool JpsiUpsilonCommon::isContainedIn(const xAOD::TrackParticle* theTrack, const std::vector<const xAOD::TrackParticle*> &theColl) {
        return std::find(theColl.cbegin(), theColl.cend(), theTrack) != theColl.cend();
    }
    
    bool JpsiUpsilonCommon::isContainedIn(const xAOD::TrackParticle* theTrack, const xAOD::MuonContainer* theColl) {
        bool isContained(false);
        xAOD::MuonContainer::const_iterator muItr;
        for (muItr=theColl->begin(); muItr!=theColl->end(); ++muItr) {
            if ( (*muItr)->inDetTrackParticleLink().cachedElement() == theTrack ) {isContained=true; break;}
        }
        return isContained;
    }

    bool JpsiUpsilonCommon::cutRange(double value, double min, double max){
        return (min<=0.0 || value >= min) && (max <= 0.0 || value <= max);
    }

    bool JpsiUpsilonCommon::cutRangeOR(const std::vector<double> &values, double min, double max){
        for(double m : values) {
           if( (min<=0.0 || m >= min) && (max <= 0.0 || m <= max)) return true;
        }
        return false;
    }

    bool JpsiUpsilonCommon::cutAcceptGreater(double value, double min ){
        return (min <=0.0 || value >= min);
    }

    bool JpsiUpsilonCommon::cutAcceptGreaterOR(const std::vector<double> &values, double min){
        for(double m : values) {
           if(min <=0.0 || m >= min) return true;
        }
        return false;
    }
}

