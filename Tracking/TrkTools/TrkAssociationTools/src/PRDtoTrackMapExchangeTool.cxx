
#include "TrkAssociationTools/PRDtoTrackMapExchangeTool.h"
#include "ext/functional"

Trk::PRDtoTrackMapExchangeTool::PRDtoTrackMapExchangeTool(const std::string&t,const std::string&n,const IInterface*p)
  : base_class(t,n,p)
{
}

StatusCode Trk::PRDtoTrackMapExchangeTool::initialize() {
   IIncidentSvc* incsvc;
   CHECK( service("IncidentSvc",incsvc) );
   incsvc->addListener(this,IncidentType::EndEvent);
   return StatusCode::SUCCESS;
}

void Trk::PRDtoTrackMapExchangeTool::handle(const Incident& incident) {
   if (incident.type()==IncidentType::EndEvent) {
      m_prdToTrackMap.reset();
   }
}


