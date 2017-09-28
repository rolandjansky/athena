/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArTBH6TriggerTimeTool_H
#define LArTBH6TriggerTimeTool_H

#include "AthenaKernel/ITriggerTime.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "StoreGate/ReadHandle.h"
#include "LArSimEvent/LArHitContainer.h"

class LArTBH6TriggerTimeTool : public extends<AthAlgTool, ITriggerTime, IIncidentListener>
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
   bool m_newEvent ;
   bool m_fixed;

   std::vector< SG::ReadHandle< LArHitContainer > > m_hitcoll;
};


#endif
