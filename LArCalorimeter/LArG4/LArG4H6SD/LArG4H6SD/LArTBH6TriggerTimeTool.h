/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArTBH6TriggerTimeTool_H
#define LArTBH6TriggerTimeTool_H

#include "AthenaKernel/ITriggerTime.h" 
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/AlgTool.h"

class StoreGateSvc ; 

class LArTBH6TriggerTimeTool :public ITriggerTime, public AlgTool,
virtual public IIncidentListener
{

public:
   LArTBH6TriggerTimeTool(const std::string& type, 
			 const std::string& name,
			 const IInterface* parent);


   virtual StatusCode initialize() ; 

   virtual ~LArTBH6TriggerTimeTool() {}

   /// returns the time offset of the current trigger
   virtual double time() ;

   virtual void handle(const Incident& incident);

    double larTime(); 
    double trackRecordTime()   ; 

private: 

   double m_time; 
   StoreGateSvc* m_sg; 
   bool m_newEvent ; 
   bool m_fixed;
};


#endif 
