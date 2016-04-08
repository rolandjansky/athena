/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNGENALGS_SELECTEVENTNUMBER_H
#define TRKALIGNGENALGS_SELECTEVENTNUMBER_H

#include "AthenaBaseComps/AthAlgorithm.h"

/**
   @file SelectEventNumber.h
   @class SelectEventNumber

   @brief This class is an algorithm that can be run before track fitting to select events from an ascii file.

   @author roberth@bu.edu  
*/  

namespace Trk {
  
  class SelectEventNumber : public AthAlgorithm {
    
  public: 
    
    /** constructor */
    SelectEventNumber(const std::string& name, ISvcLocator* pSvcLocator);

    /** destructor */
    virtual ~SelectEventNumber();
    
    /** initialize method */
    virtual StatusCode  initialize();
    
    /** execute method */
    virtual StatusCode  execute();
    
    /** finalize method */
    virtual StatusCode  finalize(); 
    
  private:

    std::string m_eventListName;  //!> name of event list ASCII file    
    std::vector< std::pair<int,int> > m_goodEventList; //!> good events read in from ASCII file

    int m_lastGoodEvt;

  };
  
} // end namespace
#endif // TRKALIGNGENALGS_SELECTEVENTNUMBER_H
