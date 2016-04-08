/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_ITGCCOINHIERARCHYSVC_H
#define TRIGGER_ITGCCOINHIERARCHYSVC_H

#include "GaudiKernel/IInterface.h"

#include <string>

#include "TrigMuonCoinHierarchy/TgcCoinHierarchy.h"

namespace Trk {
  class PrepRawData;
  class RIO_OnTrack;
  class Track;
}

namespace Muon {
  class TgcPrepData;
  class MuonCluster;
}

namespace Trigger {
  class ITgcCoinHierarchySvc : virtual public IInterface
    {
    public:
      /** Destructor */
      virtual ~ITgcCoinHierarchySvc() {}
      
      /** Provide InterfaceID */
      static const InterfaceID & interfaceID();

      /** Get TgcCoinHierarchy's from an Identifier */
      virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
				      const Identifier identify)=0;
      /** Get TgcCoinHierarchy's from a TgcPrepData (hit) */
      virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
				      const Muon::TgcPrepData* hit)=0;
      /** Get TgcCoinHierarchy's from a MuonCluster (hit) */
      virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
				      const Muon::MuonCluster* hit)=0;
      /** Get TgcCoinHierarchy's from a PrepRawData (hit) */
      virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
				      const Trk::PrepRawData* hit)=0;
      /** Get TgcCoinHierarchy's from an RIO_OnTrack (hit associated to a track) */
      virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
				      const Trk::RIO_OnTrack* rio)=0; 

      /** Get TgcCoinHierarchy's from a Track */
      virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
				      const Trk::Track* track)=0; 

      /** Get TgcCoinHierarchy's from a TgcCoinData (coincidence) */
      virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
				      const Muon::TgcCoinData* coin)=0; 

      /** Get TgcCoinHierarchy's in a trigger sector */
      virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
				      const bool isAside, const bool isForward, const int phi)=0; 
      
      /** Get TgcCoinHierarchy's from a ROI number. 
	  This method returs only TgcCoinHierarchies which have Sector Logic. */
      virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
				      const bool isAside, const bool isForward, const int phi, const int roi)=0; 

      /** Get all TgcCoinHierarchy's in an event */
      virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies)=0;
    protected:
      /** Make TgcCoinHierarchy's using ITgcCoinHierarchyClassifyTool and ITgcCoinHierarchyFindTool */
      virtual StatusCode execute()=0; 
    };
  
  inline const InterfaceID & ITgcCoinHierarchySvc::interfaceID() {
    static const InterfaceID IID_ITgcCoinHierarchySvc("Trigger::ITgcCoinHierarchySvc", 1, 0);
    return IID_ITgcCoinHierarchySvc;
  }
  
} // end of namespace Trigger

#endif // TRIGGER_ITGCCOINHIERARCHYSVC_H
