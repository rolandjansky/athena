/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JpsiUpsilonTools/JpsiUpsilonCommon.h"
#include "TLorentzVector.h"
#include "xAODBPhys/BPhysHelper.h"
#include "xAODTracking/VertexContainer.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"

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
            auto& link = ( *muItr )->inDetTrackParticleLink();
            if ( link.isValid() && ( *link == theTrack ) ) {isContained=true; break;}
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

    Analysis::CleanUpVertex JpsiUpsilonCommon::ClosestRefPV(xAOD::BPhysHelper& bHelper,
							const xAOD::VertexContainer* importedPVerticesCollection,
							Analysis::PrimaryVertexRefitter *pvRefitter){
       const xAOD::Vertex* vtx_closest = nullptr; // vertex closest to bVertex track
       if(importedPVerticesCollection->empty()) return Analysis::CleanUpVertex(nullptr, false);
       double dc = 1e10;
       std::vector<const xAOD::Vertex*> tocleanup;
       if(pvRefitter) tocleanup.reserve(importedPVerticesCollection->size());
       bool vertexrefitted = false;
       for (const xAOD::Vertex* PV : *importedPVerticesCollection) {
          const xAOD::Vertex* refPV = pvRefitter ? pvRefitter->refitVertex(PV, bHelper.vtx(), false) : nullptr;
          const xAOD::Vertex* vtx = refPV ? refPV : PV;
          if(refPV) tocleanup.push_back(refPV);
          TVector3 posPV(vtx->position().x(),vtx->position().y(),vtx->position().z());
          auto &helperpos = bHelper.vtx()->position();
          TVector3 posV(helperpos.x(), helperpos.y(), helperpos.z());
          TVector3 nV = bHelper.totalP().Unit();
          TVector3 dposV = posPV-posV;
          double dposVnV = dposV*nV;
          double d = std::sqrt(std::abs(dposV.Mag2()-dposVnV*dposVnV));
          if (d<dc) {
             dc = d;
             vtx_closest = vtx;
             vertexrefitted = (vtx_closest == refPV);
          }
       }
       for(auto ptr : tocleanup){
           if(ptr != vtx_closest) delete ptr;
       }
       return Analysis::CleanUpVertex(vtx_closest, vertexrefitted);
    }
}

