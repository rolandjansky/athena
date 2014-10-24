/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_TRACKMATCHSORTER_H
#define EGAMMATOOLS_TRACKMATCHSORTER_H
class egammaRec;

struct TrackMatch
{
 public:
  int trackNumber;
  double dR;
  double seconddR;
  bool isTRT;
  int score;
  int hitsScore;
  double deltaPhiLast;
  double deltaEta[4];
  double deltaPhi[4];
  double deltaPhiRescaled[4];

};

class TrackMatchSorter
: public std::binary_function<TrackMatch, TrackMatch, bool> {
 public:
  bool operator()(const TrackMatch& match1, const TrackMatch& match2) const
  {

    if(match1.score!= match2.score) {//Higher score 
      return match1.score>match2.score;
    }
    //sqrt(0.025**2)*sqrt(2)/sqrt(12) ~ 0.01
    if(fabs(match1.dR-match2.dR)<1e-02) {
      
      if(fabs(match1.seconddR-match2.seconddR)>1e-02 ){ //Can the second distance separate them?
	return match1.seconddR < match2.seconddR	;
      }

      if((match1.hitsScore!= match2.hitsScore)){ //use the one with more pixel
	return match1.hitsScore>match2.hitsScore;
      }

    }

    //closest DR
    return match1.dR < match2.dR	;
  }
};

#endif
