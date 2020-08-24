/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class for storing TimedMdtCalibHits like segments       //
// Author: Zdenko van Kesteren                             //
// Date  : 24 january 2006                                 //
//*********************************************************//

#ifndef TimedSegment_h
#define TimedSegment_h

#include <iostream>
#include <vector>
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

namespace MuonCalib{
  class TimedMdtCalibHit;
  /**
     @class TimedSegment
     Similar like the MuonCalibSegment, although the TimedSegment only contains a set of TimedMdtCalibHits. It holds a copy of the original MuonCalibSegment which was used as input to the (variable) t0-fit. Timing-correction (and its error) on segment level (to the t0) are stored. 
     @author Zdenko.Van.Kesteren@cern.ch
  */
  class TimedSegment{
    public:
    typedef std::vector<TimedMdtCalibHit*>    TimedHitVec;  //!< typedef of a set op TimedMdtCalibHit s
    typedef TimedHitVec::iterator             THitVecIt;    //!< typedef of the iterator to the set of TimedMdtCalibHit s
    typedef TimedHitVec::const_iterator       THitVecCit;   //!< typedef of the const_iterator to the set of TimedMdtCalibHit s

    TimedSegment( const MuonCalibSegment* segment) ;        //!< constructor from original MuonCalibSegment
    TimedSegment(const TimedSegment& tseg);                 //!< copyconstructor
    TimedSegment& operator=( const TimedSegment& rhs );     //!< assignement operator
    ~TimedSegment() ;                                       //!< destructor

    unsigned int numberOfHits() const ;      //!< retrieve the number of TimedMdtCalibHit s on the TimedSegment
    THitVecIt    hitsBegin();                //!< retrieve the iterator pointing to the first TimedMdtCalibHit on the segment
    THitVecIt    hitsEnd();                  //!< retrieve the iterator pointing to the last TimedMdtCalibHit on the segment
    THitVecCit   hitsBegin() const ;         //!< retrieve the const_iterator pointing to the first TimedMdtCalibHit on the segment
    THitVecCit   hitsEnd() const ;           //!< retrieve the const_iterator pointing to the last TimedMdtCalibHit on the segment
    void         addHit( TimedMdtCalibHit* hit ); //!< add a TimedMdtCalibHit to the TimedSegment
    void         update();                   //!< overwrite the stored MuonCalibSegment with MuonCalibHits with the recalculated driftcircles (adapted to the variable t0)

    MuonCalibSegment* originalSegment() const; //!< retrieve the original MuonCalibSegment, used to initialize the TimedSegment with
    MuonCalibSegment* segment() const;         //!< retrieve the MuonCalibSegment with the updated MuonCalibHitBase s.

    void              setSegment( MuonCalibSegment* segment ) ; //!< sets MuonCalibSegment to 'segment'.
    
    double time() const;                                      //!< retrieve the timing correction on the t0 on segment level
    double sigmaTime() const;                                 //!< retrieve the error on the timing correction on the t0 on segment level
    double radiusHit(unsigned int i) const;                                   //!< retrieve the driftradius correction of hit i
    double timeHit(unsigned int i) const;                                     //!< retrieve the timing correction of hit i
    double sigmaTimeHit(unsigned int i) const;                                //!< retrieve the error on the timing correction of hit i
    unsigned int mlHit(unsigned int i) const { return m_mlHit.at(i); } ;      //!< retrieve the multilayer (1 or 2) of hit i

    void setTime( double time ) ;                             //!< sets the timing correction on the t0 on segment level
    void setSigmaTime( double sigmaTime );                    //!< sets the error on the timing correction on the t0 on segment level
    void setRadiusHit(unsigned int i,  double radius ) ;                      //!< retrieve the driftradius correction of hit i
    void setTimeHit(unsigned int i,  double time ) ;                          //!< retrieve the timing correction of hit i
    void setSigmaTimeHit(unsigned int i, double sigmaTime );                  //!< retrieve the error on the timing correction of hit i
    void setMlHit( unsigned int i, unsigned int ml) { m_mlHit.at(i) = ml ;} ; //!< sets the multilayer (1 or 2) of hit i

    private:
    std::vector<unsigned int> m_mlHit ;   //!< vector keeping track of multilayer (1 or 2) of the hits
    std::vector<double> m_radiiHit;       //!< vector keeping track of the driftradii of the hits 
    std::vector<double> m_timesHit;       //!< vector keeping track of the timing corrections of the hits
    std::vector<double> m_sigmaTimesHit;  //!< vector keeping track of the error on thetiming corrections of the hits
    TimedHitVec       m_tHitVec;          //!< vector of TimedMdtCalibHit s on the TimedSegment
    MuonCalibSegment* m_segment;          //!< instance of MuonCalibSegment, keeping track of updated timing
    MuonCalibSegment* m_segment_original; //!< instance of MuonCalibSegment, keeping the original MuonCalibSegment safe from harm
    double            m_time;             //!< timing correction on segment level
    double            m_sigma_time;       //!< error on the timing correction on segment level

  };

  inline unsigned int TimedSegment::numberOfHits() const { return m_tHitVec.size() ; }
  inline TimedSegment::THitVecIt TimedSegment::hitsBegin() { return m_tHitVec.begin() ;}
  inline TimedSegment::THitVecIt TimedSegment::hitsEnd() { return m_tHitVec.end() ;}
  inline TimedSegment::THitVecCit TimedSegment::hitsBegin() const { return m_tHitVec.begin() ;}
  inline TimedSegment::THitVecCit TimedSegment::hitsEnd() const { return m_tHitVec.end() ;}
  inline void TimedSegment::addHit( TimedMdtCalibHit* hit ) { m_tHitVec.push_back(hit) ; }

  inline double TimedSegment::time() const {return m_time ;}
  inline double TimedSegment::sigmaTime() const {return m_sigma_time ;}
  inline void   TimedSegment::setTime( double time ) { m_time = time; }
  inline void   TimedSegment::setSigmaTime( double sigmaTime ) { m_sigma_time = sigmaTime ;}
  
  inline void   TimedSegment::setSegment( MuonCalibSegment* segment ) { 
    m_segment = segment ; 
  }
  inline MuonCalibSegment* TimedSegment::segment() const { return m_segment; }
  inline MuonCalibSegment* TimedSegment::originalSegment() const { return m_segment_original; }

  inline double TimedSegment::radiusHit(unsigned int i) const{
    if(i>m_radiiHit.size() ) return 0.;
    else {
      MsgStream log(Athena::getMessageSvc(),"TimedSegment");
      log<<MSG::INFO<<"radiusHit(" << i << ") = " << m_radiiHit[i]<<endmsg;
      return m_radiiHit.at(i);
    }
  }
  inline double TimedSegment::timeHit(unsigned int i) const{
    if(i>m_timesHit.size() ) return 0.;
    else {
      return m_timesHit.at(i);
    }
  }
  inline double TimedSegment::sigmaTimeHit(unsigned int i) const{
    if(i>m_sigmaTimesHit.size() ) return 0.;
    else {
      return m_sigmaTimesHit.at(i);
    }
  }
  inline void TimedSegment::setRadiusHit(unsigned int i,  double dr ){
    MsgStream log(Athena::getMessageSvc(),"TimedSegment");
    if(i>m_radiiHit.size()){
      log<<MSG::WARNING<<"m_radiiHit has a too small size"<<endmsg;
      m_radiiHit.insert(m_radiiHit.begin()+i, dr);
    }
    else m_radiiHit.at(i) = dr;
    log<<MSG::INFO<<i<<" set " << dr << " in the dr_vector, " << m_radiiHit.at(i)<<endmsg;
  }
  inline void   TimedSegment::setTimeHit(unsigned int i,  double time ){
    if(i>m_timesHit.size()){
      MsgStream log(Athena::getMessageSvc(),"TimedSegment");
      log<<MSG::WARNING<<"m_timesHit has a too small size"<<endmsg;
      m_timesHit.insert(m_timesHit.begin()+i, time); 
       }
    else m_timesHit.at(i) = time;
  }
  inline void   TimedSegment::setSigmaTimeHit(unsigned int i, double sigmaTime ){
    if(i>m_sigmaTimesHit.size()){
      MsgStream log(Athena::getMessageSvc(),"TimedSegment");
      log<<MSG::WARNING<<"m_sTimesHit has a too small size"<<endmsg;
      m_sigmaTimesHit.insert(m_timesHit.begin()+i, sigmaTime); 
    }
    else m_sigmaTimesHit.at(i) = sigmaTime;
  }

}//MuonCalib


#endif //TimedSegment_h
