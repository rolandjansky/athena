/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Class  : ScoutingStreamWriter
 *
 *
 * @author Noemi Calace <noemi.calace@cern.ch>  - CERN
 *
 **********************************************************************************/

#ifndef TrigEgammaHypo_ScoutingStreamWriter_H
#define TrigEgammaHypo_ScoutingStreamWriter_H

//#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigInterfaces/HypoAlgo.h"
#include "GaudiKernel/ServiceHandle.h"
#include <vector>
#include <string>

class ScoutingInfo;
    
/*
     @class ScoutingStreamWriter
*/

class ScoutingStreamWriter : public HLT::HypoAlgo //public HLT::AllTEAlgo
{
  public:

  ScoutingStreamWriter(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor
  ~ScoutingStreamWriter();
  
  HLT::ErrorCode hltInitialize();
  
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
			    //std::vector<std::vector<HLT::TriggerElement*> >& input, 
			    //unsigned int output);
  
  HLT::ErrorCode hltFinalize();
  
 private:
  
  ServiceHandle<IClassIDSvc> m_clidSvc;
  StringArrayProperty  m_collectionTypeName; 
  
 
};

#endif
