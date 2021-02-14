/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TGCTrackSelector_hh
#define TGCTrackSelector_hh

#include <memory>

namespace LVL1TGCTrigger {

  class TGCSectorLogic;
  class TGCTrackSelectorOut;
  class TGCRPhiCoincidenceOut;

  class TGCTrackSelector{

  public:
    TGCTrackSelector(const TGCSectorLogic* sL=0);
    ~TGCTrackSelector();

    void reset();
    void input(TGCRPhiCoincidenceOut* rPhiOut);
    bool select(std::shared_ptr<TGCTrackSelectorOut> TrackcandidateOut);
    TGCTrackSelectorOut getTrackSelectorOut(int index);

    bool compare(TGCRPhiCoincidenceOut* track1, TGCRPhiCoincidenceOut* track2);
    enum {MaxNumber_of_TrackCandidate_in = 7}; 



  private:

    std::unique_ptr<TGCRPhiCoincidenceOut> m_coincidenceIn[MaxNumber_of_TrackCandidate_in]; 
    int m_trackPriorityRank[MaxNumber_of_TrackCandidate_in];
    
  
    int m_numberOfCandidate_In;
    int m_numberOfCandidate_Out;

    const TGCSectorLogic* m_sectorLogic;
  };
 


}

#endif
