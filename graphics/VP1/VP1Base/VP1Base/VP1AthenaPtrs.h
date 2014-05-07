/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1AthenaPtrs                       //
//                                                            //
//  Description: Static access to athena pointers             //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1ATHENAPTRS_H
#define VP1ATHENAPTRS_H

class StoreGateSvc;
class ISvcLocator;
class IToolSvc;

class VP1AthenaPtrs {
public:

  static StoreGateSvc * eventStore() { return m_eventStore; }
  static StoreGateSvc * detectorStore() { return m_detectorStore; }
  static ISvcLocator * serviceLocator() { return m_serviceLocator; }
  static IToolSvc * toolSvc() { return m_toolSvc; }

private:
  VP1AthenaPtrs();
  ~VP1AthenaPtrs();
  static StoreGateSvc * m_eventStore;
  static StoreGateSvc * m_detectorStore;
  static ISvcLocator * m_serviceLocator;
  static IToolSvc * m_toolSvc;
  friend class VP1ExecutionScheduler;
  static void setPointers( StoreGateSvc * eventStore,
			   StoreGateSvc * detectorStore,
			   ISvcLocator * , IToolSvc *);


};

#endif
