/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCTrackSelector.h"
#include "TrigT1TGC/TGCSectorLogic.h"
#include "TrigT1TGC/TGCTrackSelectorOut.h"
#include "TrigT1TGC/TGCRPhiCoincidenceOut.h"


namespace LVL1TGCTrigger {



  TGCTrackSelector::TGCTrackSelector(const TGCSectorLogic* sL):
    m_numberOfCandidate_In(0),
    m_numberOfCandidate_Out(0),
    m_sectorLogic(sL) 
  {
  }

  TGCTrackSelector::~TGCTrackSelector()
  {
  }


  void TGCTrackSelector::input(TGCRPhiCoincidenceOut* rPhiOut){
    if(rPhiOut!=0){
      if(rPhiOut->getpT()!=0){
        m_coincidenceIn[m_numberOfCandidate_In].reset(rPhiOut);
        m_numberOfCandidate_In++;
      }
    }
  }


  bool TGCTrackSelector::select(std::shared_ptr<TGCTrackSelectorOut> TrackcandidateOut){
 
    if(m_numberOfCandidate_In==0){return false;}


    /////////////////////////////////////
    ///  compare between all combination
    ////////////////////////////////////
    for(int track1=0;track1!=m_numberOfCandidate_In-1 && m_numberOfCandidate_In!=0;track1++){
      for(int track2=track1+1;track2!=m_numberOfCandidate_In;track2++){
        bool compare_result=compare(m_coincidenceIn[track1].get(),m_coincidenceIn[track2].get()); //1>2 true:1 1<2 false:0

       //TrackPriorityRank : Smaller value is high priority.
        m_trackPriorityRank[track1]+=(int)!compare_result;
        m_trackPriorityRank[track2]+=(int)compare_result;
      }
    }
 
    /////////////////////////////////////
    ///// Select 4 high priority tracks
    ////////////////////////////////////
    for(int track=0;track!=m_numberOfCandidate_In;track++){
      if(m_trackPriorityRank[track]<=3){

        int R= 2*m_coincidenceIn[track]->getIdSSC()+m_coincidenceIn[track]->getR() - (m_sectorLogic->getRegion()==Endcap ? 1 : 0);
        TrackcandidateOut->setR(m_numberOfCandidate_Out,R);
        TrackcandidateOut->setPhi(m_numberOfCandidate_Out,m_coincidenceIn[track]->getPhi());
        TrackcandidateOut->setPtLevel(m_numberOfCandidate_Out,m_coincidenceIn[track]->getpT());
        TrackcandidateOut->setDR(m_numberOfCandidate_Out,2*m_coincidenceIn[track]->getDR());
        TrackcandidateOut->setDPhi(m_numberOfCandidate_Out,m_coincidenceIn[track]->getDPhi());
        TrackcandidateOut->setInnerVeto(m_numberOfCandidate_Out,m_coincidenceIn[track]->getInnerVeto());
        TrackcandidateOut->setCharge(m_numberOfCandidate_Out,m_coincidenceIn[track]->getCharge());
        TrackcandidateOut->setCoincidenceType(m_numberOfCandidate_Out,m_coincidenceIn[track]->getCoincidenceType());
        TrackcandidateOut->setGoodMFFlag(m_numberOfCandidate_Out,m_coincidenceIn[track]->getGoodMFFlag());
        TrackcandidateOut->setInnerCoincidenceFlag(m_numberOfCandidate_Out,m_coincidenceIn[track]->getInnerCoincidenceFlag());
        m_numberOfCandidate_Out++;
      }
    }

    m_numberOfCandidate_In=0;
    m_numberOfCandidate_Out=0;
 
    return true;

  }


  bool TGCTrackSelector::compare(TGCRPhiCoincidenceOut* track1, TGCRPhiCoincidenceOut* track2){


    // The definition of priority is not fixed. This function wiil be updated.
    if(track1->getpT() > track2->getpT()){return true;}
    if(track1->getpT() < track2->getpT()){return false;}
    else{
      // when the pt level of 2 tracks are same, track with large R is selected.
      if(track1->getIdSSC()<track2->getIdSSC()){return true;}
      if(track1->getIdSSC()>track2->getIdSSC()){return false;}
    }
    return true; 

  }





}
