/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_SIMPLEPOSIXTIMEKEEPERSVC_H
#define ATHENASERVICES_SIMPLEPOSIXTIMEKEEPERSVC_H

//base classes
#ifndef ATHENAKERNEL_POSIXTIMEKEEPER_H
 #include "AthenaKernel/POSIXTimeKeeper.h"
#endif
#ifndef GAUDIKERNEL_PROPERTY_H
 #include "GaudiKernel/Property.h"
#endif
#ifndef ATHENABASECOMPS_ATHSERVICE_H
 #include "AthenaBaseComps/AthService.h"
#endif


/** @class SimplePOSIXTimeKeeperSvc
  * @brief service that implements ITimeKeeper using basic POSIX time api.
  *        Requires user to set the initial time budget.
  *        N.B. times are in 1/100 CPU sec units
  * 
  * @param AllocTime (Property): job time allocation in CPU sec
  * @author pcalafiura@lbl.gov
  * $Id: SimplePOSIXTimeKeeperSvc.h,v 1.2 2008-09-23 22:00:47 binet Exp $
  */
class SimplePOSIXTimeKeeperSvc   : virtual public POSIXTimeKeeper,
                 	           virtual public AthService
{
public:
  /// \name ITimeKeeper Implementation
  //@{
  virtual bool nextIter();         ///< main user entry point
  //INH  virtual time_t timeL() const;    ///< time remaining before limit
  //INH  virtual time_t timeX() const;    ///< time used so far
  //INH  virtual bool timeOver() const;   ///< is there time for another iter?
  //@}

  /// \name TimeKeeper Implementation
  //@{
  virtual time_t allocTime() const;       ///< allocated CPU time for job
  //INH void updateTime();  
  //@}


  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
				     void** ppvInterface );
  //@}
  
  // Standard Constructor
  SimplePOSIXTimeKeeperSvc(const std::string& name, ISvcLocator* svc);
  
  // Standard Destructor
  virtual ~SimplePOSIXTimeKeeperSvc();
private:
  void decodeAllocTime(Property&); 
  time_t m_allocTime;             ///< allocated job cpu time (in 1/100 sec)
  IntegerProperty m_allocTimeProp;///< property: alloc job cpu time (in sec)
};
#endif


