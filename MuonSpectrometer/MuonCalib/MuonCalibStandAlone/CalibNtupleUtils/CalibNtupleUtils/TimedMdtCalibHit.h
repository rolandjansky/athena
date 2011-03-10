/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class for storing MdtCalibHitBase info and timing!      //
// Author: Zdenko van Kesteren                             //
// Date  : 24 january 2006                                 //
//*********************************************************//

#ifndef TimedMdtCalibHit_h
#define TimedMdtCalibHit_h

#include "MuonCalibEventBase/MdtCalibHitBase.h"

namespace MuonCalib{

  /**
     @class TimedMdtCalibHit
     Daughter of MdtCalibHitBase, contains additional timing information used for the (variable) t0-fit.

     @author Zdenko.Van.Kesteren@cern.ch
  */
  class TimedMdtCalibHit : public MdtCalibHitBase {

    public:
      TimedMdtCalibHit() {} ;  //!< default constructor
      ~TimedMdtCalibHit() {} ; //!< destructor
      TimedMdtCalibHit(const MdtCalibHitBase& hit ) : MdtCalibHitBase(hit), m_time(0.), m_sigma_time(0.) {}; //!< constructor initializing with MdtCalibHitBase
      TimedMdtCalibHit(const MdtCalibHitBase& hit, double time, double sigma_time ) : MdtCalibHitBase(hit), m_time(time), m_sigma_time(sigma_time) {} //!< constructor initializing with MdtCalibHitBase, time and error on the time
	  
      double time() ;                        //!< retrieve the timeshift of the TimedMdtCalibHit
      double sigmaTime() ;                   //!< retrieve the error on thetimeshift of the TimedMdtCalibHit
      void setTime( double time ) ;          //!< sets the timeshift of the TimedMdtCalibHit
      void setSigmaTime( double sigmaTime ); //!< sets the error on thetimeshift of the TimedMdtCalibHit

    private:
      double m_time;        //!< member storing the time shift
      double m_sigma_time;  //!< member storing the error on the time shift
    
    };

  inline double TimedMdtCalibHit::time() {return m_time ;}
  inline double TimedMdtCalibHit::sigmaTime() {return m_sigma_time ;}
  inline void   TimedMdtCalibHit::setTime( double time ) { m_time = time; }
  inline void   TimedMdtCalibHit::setSigmaTime( double sigmaTime ) { m_sigma_time = sigmaTime ;}
  


}//MuonCalib


#endif //TimedMdtCalibHit_h
