/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#define private public
#define protected public
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#undef private
#undef protected

#include "TrkEventTPCnv/TrkTrackSummary/TrackSummaryCnv_p2.h"

void TrackSummaryCnv_p2::dbgPrint( const Trk::TrackSummary *t){
    
    std::cout << "-------------------------------------------" <<std::endl;
    std::cout << "m_idHitPattern:\t" << t->m_idHitPattern << std::endl;
    std::cout << "m_dedx        :\t" << t->m_dedx << std::endl;
    std::cout << "m_nhitsdedx   :\t" << t->m_nhitsdedx << std::endl;
    
    std::cout << " std::vector m_information size: "<< t->m_information.size() <<std::endl;
    for (std::vector<int>::const_iterator i=t->m_information.begin();i!=t->m_information.end();i++) std::cout<<"\t "<<(*i);std::cout<<std::endl;
    
    std::cout << " std::vector m_eProbability size: "<< t->m_eProbability.size() <<std::endl;
    for (std::vector<float>::const_iterator i=t->m_eProbability.begin();i!=t->m_eProbability.end();i++) std::cout<<"\t "<<(*i);std::cout<<std::endl;
    
    if(t->m_indetTrackSummary){
        std::cout << " m_indetTrackSummary->m_massdedx: "<< t->m_indetTrackSummary->m_massdedx;
        std::cout << " std::vector m_indetTrackSummary->m_likelihoodspixeldedx size: "<< t->m_indetTrackSummary->m_likelihoodspixeldedx.size() <<std::endl;
        for (std::vector<float>::const_iterator i=t->m_indetTrackSummary->m_likelihoodspixeldedx.begin();i!=t->m_indetTrackSummary->m_likelihoodspixeldedx.end();i++) std::cout<<"\t "<<(*i);std::cout<<std::endl;
    }
    
    if(t->m_muonTrackSummary){ 
        std::cout << " m_muonTrackSummary->m_nscatterers: "<< t->m_muonTrackSummary->m_nscatterers <<std::endl;
        std::cout << " m_muonTrackSummary->m_npseudoMeasurements: "<< t->m_muonTrackSummary->m_npseudoMeasurements <<std::endl;
        std::cout << " std::vector m_muonTrackSummary->m_chamberHitSummary size: "<< t->m_muonTrackSummary->m_chamberHitSummary.size() <<std::endl;
        for (size_t i=0;i<t->m_muonTrackSummary->m_chamberHitSummary.size();i++) {
            std::cout<<"\t m_chId           "<<t->m_muonTrackSummary->m_chamberHitSummary[i].m_chId           <<std::endl;
            std::cout<<"\t m_isMdt          "<<t->m_muonTrackSummary->m_chamberHitSummary[i].m_isMdt          <<std::endl;
            std::cout<<"\t first.nhits      "<<t->m_muonTrackSummary->m_chamberHitSummary[i].first.nhits      <<std::endl;
            std::cout<<"\t first.nholes     "<<t->m_muonTrackSummary->m_chamberHitSummary[i].first.nholes     <<std::endl;
            std::cout<<"\t first.noutliers  "<<t->m_muonTrackSummary->m_chamberHitSummary[i].first.noutliers  <<std::endl;
            std::cout<<"\t first.ndeltas    "<<t->m_muonTrackSummary->m_chamberHitSummary[i].first.ndeltas    <<std::endl;
            std::cout<<"\t first.ncloseHits "<<t->m_muonTrackSummary->m_chamberHitSummary[i].first.ncloseHits <<std::endl;
            std::cout<<"\t second.nhits     "<<t->m_muonTrackSummary->m_chamberHitSummary[i].second.nhits     <<std::endl;
            std::cout<<"\t second.nholes    "<<t->m_muonTrackSummary->m_chamberHitSummary[i].second.nholes    <<std::endl;
            std::cout<<"\t second.noutliers "<<t->m_muonTrackSummary->m_chamberHitSummary[i].second.noutliers <<std::endl;
            std::cout<<"\t second.ndeltas   "<<t->m_muonTrackSummary->m_chamberHitSummary[i].second.ndeltas   <<std::endl;
            std::cout<<"\t second.ncloseHits"<<t->m_muonTrackSummary->m_chamberHitSummary[i].second.ncloseHits <<std::endl;
        }
    }
    
}


void TrackSummaryCnv_p2::persToTrans( const Trk::TrackSummary_p2 *persObj, Trk::TrackSummary *transObj, MsgStream &/*log*/ ){
    transObj->m_information       = persObj->m_information;
    if (transObj->m_information.size() < Trk::numberOfTrackSummaryTypes)
      transObj->m_information.resize(Trk::numberOfTrackSummaryTypes,
                                     Trk::TrackSummary::SummaryTypeNotSet);

    transObj->m_idHitPattern      = persObj->m_idHitPattern;
    transObj->m_eProbability      = persObj->m_eProbability;
    transObj->m_dedx              = persObj->m_pixeldEdx;
    transObj->m_nhitsdedx         = persObj->m_nhitsfordEdx;

    size_t s = persObj->m_indetTrackSummary.size();
    if (s){ // INDET TRACK SUMMARIES
        Trk::InDetTrackSummary *ts=new Trk::InDetTrackSummary();
        ts->m_massdedx = persObj->m_indetTrackSummary.back();
        ts->m_likelihoodspixeldedx.resize(s-1);
        for(size_t i = 0; i < (s-1); ++i)
            ts->m_likelihoodspixeldedx[i]=persObj->m_indetTrackSummary[i];

        transObj->m_indetTrackSummary=ts;
    }
    
    
    s = persObj->m_muonTrackSummary.size();
    if(s){  // MUON TRACK SUMMARY
        Trk::MuonTrackSummary *ts= new Trk::MuonTrackSummary();
        std::vector<unsigned int>::const_iterator i = persObj->m_muonTrackSummary.begin();
        
        ts->m_nscatterers = *i; i++;
        ts->m_npseudoMeasurements = *i; i++;
        
        size_t size=(s-2)/12;
        ts->m_chamberHitSummary.resize(size);

        for (size_t sc=0;  sc<size ; ++sc ){
            ts->m_chamberHitSummary[sc].m_chId = Identifier(*i); i++;
            ts->m_chamberHitSummary[sc].m_isMdt          =(*i);i++;
            ts->m_chamberHitSummary[sc].first.nhits      =(*i);i++;
            ts->m_chamberHitSummary[sc].first.nholes     =(*i);i++;
            ts->m_chamberHitSummary[sc].first.noutliers  =(*i);i++;
            ts->m_chamberHitSummary[sc].first.ndeltas    =(*i);i++;
            ts->m_chamberHitSummary[sc].first.ncloseHits =(*i);i++;
            ts->m_chamberHitSummary[sc].second.nhits     =(*i);i++;
            ts->m_chamberHitSummary[sc].second.nholes    =(*i);i++;
            ts->m_chamberHitSummary[sc].second.noutliers =(*i);i++;
            ts->m_chamberHitSummary[sc].second.ndeltas   =(*i);i++;
            ts->m_chamberHitSummary[sc].second.ncloseHits=(*i);i++;
        }
        
        transObj->m_muonTrackSummary=ts;
    }
    
    // dbgPrint(transObj);
}


void TrackSummaryCnv_p2::transToPers( const Trk::TrackSummary    *transObj, Trk::TrackSummary_p2 *persObj, MsgStream & /*log*/ ){
    
    // dbgPrint(transObj);
    
    persObj->m_information       = transObj->m_information;
    persObj->m_idHitPattern      = transObj->m_idHitPattern;
    persObj->m_eProbability      = transObj->m_eProbability;
    persObj->m_pixeldEdx         = transObj->m_dedx;
    persObj->m_nhitsfordEdx      = transObj->m_nhitsdedx;

    if(transObj->m_indetTrackSummary){       // INDET TRACK SUMMARY
        size_t s = (transObj->m_indetTrackSummary->m_likelihoodspixeldedx).size(); 
        persObj->m_indetTrackSummary.reserve(s+1);
        for(size_t i = 0; i < s; ++i)
            persObj->m_indetTrackSummary.push_back(transObj->m_indetTrackSummary->m_likelihoodspixeldedx[i]);

        persObj->m_indetTrackSummary.push_back(transObj->m_indetTrackSummary->m_massdedx);
    }


    if(transObj->m_muonTrackSummary){  // MUON TRACK SUMMARY
        
        size_t size = transObj->m_muonTrackSummary->m_chamberHitSummary.size();
        
        persObj->m_muonTrackSummary.reserve(2 + 12 * size);
        persObj->m_muonTrackSummary.push_back(transObj->m_muonTrackSummary->m_nscatterers);
        persObj->m_muonTrackSummary.push_back(transObj->m_muonTrackSummary->m_npseudoMeasurements);

        for (size_t i=0; i<size ; ++i ){
            persObj->m_muonTrackSummary.push_back(transObj->m_muonTrackSummary->m_chamberHitSummary[i].m_chId.get_identifier32().get_compact() );
            persObj->m_muonTrackSummary.push_back(transObj->m_muonTrackSummary->m_chamberHitSummary[i].m_isMdt  ); // these are just bits and should be compressed by us
            persObj->m_muonTrackSummary.push_back(transObj->m_muonTrackSummary->m_chamberHitSummary[i].first.nhits      );
            persObj->m_muonTrackSummary.push_back(transObj->m_muonTrackSummary->m_chamberHitSummary[i].first.nholes     );
            persObj->m_muonTrackSummary.push_back(transObj->m_muonTrackSummary->m_chamberHitSummary[i].first.noutliers  );
            persObj->m_muonTrackSummary.push_back(transObj->m_muonTrackSummary->m_chamberHitSummary[i].first.ndeltas    );
            persObj->m_muonTrackSummary.push_back(transObj->m_muonTrackSummary->m_chamberHitSummary[i].first.ncloseHits );
            persObj->m_muonTrackSummary.push_back(transObj->m_muonTrackSummary->m_chamberHitSummary[i].second.nhits      );
            persObj->m_muonTrackSummary.push_back(transObj->m_muonTrackSummary->m_chamberHitSummary[i].second.nholes     );
            persObj->m_muonTrackSummary.push_back(transObj->m_muonTrackSummary->m_chamberHitSummary[i].second.noutliers  );
            persObj->m_muonTrackSummary.push_back(transObj->m_muonTrackSummary->m_chamberHitSummary[i].second.ndeltas    );
            persObj->m_muonTrackSummary.push_back(transObj->m_muonTrackSummary->m_chamberHitSummary[i].second.ncloseHits );
        }

    }
}
