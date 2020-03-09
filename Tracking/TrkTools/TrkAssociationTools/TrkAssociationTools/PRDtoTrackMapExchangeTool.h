/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _PRDtoTrackMapExchangeTool_H_
#define _PRDtoTrackMapExchangeTool_H_

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/IPRDtoTrackMapExchangeTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "GaudiKernel/IIncidentListener.h"

namespace Trk {
   /** Tool emulate PRD assocation behaviour for trigger in run2
    * Not to be used!
    */
   class PRDtoTrackMapExchangeTool : public extends<AthAlgTool, Trk::IPRDtoTrackMapExchangeTool>,virtual public IIncidentListener
   {
   public:
      PRDtoTrackMapExchangeTool(const std::string&,const std::string&,const IInterface*);

      virtual StatusCode initialize() override;
      virtual void handle(const Incident& incident) override;

      virtual void setPRDtoTrackMap ATLAS_NOT_THREAD_SAFE (Trk::PRDtoTrackMap *a_map) override { m_prdToTrackMap.reset(a_map); }
      virtual const Trk::PRDtoTrackMap *getPRDtoTrackMap() const override { return m_prdToTrackMap.get(); }

   private:
      std::unique_ptr<Trk::PRDtoTrackMap> m_prdToTrackMap;
   };
}
#endif
