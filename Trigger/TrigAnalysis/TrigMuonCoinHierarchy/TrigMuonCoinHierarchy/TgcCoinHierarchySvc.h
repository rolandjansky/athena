/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_TGCCOINHIERARCHYSVC_H
#define TRIGGER_TGCCOINHIERARCHYSVC_H

#include "TrigMuonCoinHierarchy/ITgcCoinHierarchySvc.h"

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrigMuonCoinHierarchy/ITgcCoinHierarchyClassifyTool.h"
#include "TrigMuonCoinHierarchy/ITgcCoinHierarchyFindTool.h"

#include <map>

namespace Trk {
  class RIO_OnTrack;
  class Track;
}

namespace Trigger {
  class TgcCoinHierarchySvc : public AthService, virtual public ITgcCoinHierarchySvc, virtual public IIncidentListener {
  public: 
    /** Constructor */
    TgcCoinHierarchySvc(const std::string& name, ISvcLocator* svc);
    /** Destructor */
    virtual ~TgcCoinHierarchySvc();

    /** Provide InterfaceID */
    static const InterfaceID & interfaceID() { return ITgcCoinHierarchySvc::interfaceID(); };
    /** Query interface */
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
    
    /** Intialize */
    virtual StatusCode initialize(void);
    /** Finalize */
    virtual StatusCode finalize(void);

    /** Get TgcCoinHierarchy's from an Identifier */
    virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, const Identifier identify);
    /** Get TgcCoinHierarchy's from a TgcPrepData (hit) */
    virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, const Muon::TgcPrepData* hit);
    /** Get TgcCoinHierarchy's from a MuonCluster (hit) */
    virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, const Muon::MuonCluster* hit);
    /** Get TgcCoinHierarchy's from a PrepRawData (hit) */
    virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, const Trk::PrepRawData* hit);
    /** Get TgcCoinHierarchy's from an RIO_OnTrack (hit associated to a track) */
    virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, const Trk::RIO_OnTrack* rio);
    /** Get TgcCoinHierarchy's from a Track 
        (This method needs improvement. One RIO_OnTrack represents one chamber now) */
    virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, const Trk::Track* track);

    /** Get TgcCoinHierarchy's from a TgcCoinData (coincidence) */
    virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, const Muon::TgcCoinData* coin);

    /** Get TgcCoinHierarchy's in a trigger sector */
    virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
                                    const bool isAside, const bool isForward, const int phi);
      
    /** Get TgcCoinHierarchy's from a ROI number. This method returs only TgcCoinHierarchies which have Sector Logic. */
    virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
                                    const bool isAside, const bool isForward, const int phi, const int roi);

    /** Get all TgcCoinHierarchy's in an event */
    virtual StatusCode getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies);
 
    /** To know a new event by IIncidentListener */
    virtual void handle(const Incident& inc);

  private:
    /** Make TgcCoinHierarchy's using ITgcCoinHierarchyClassifyTool and ITgcCoinHierarchyFindTool */
    virtual StatusCode execute(void);

    /** Make maps from Identifier to TgcCoinHierarchy's numbers */
    StatusCode makeIndexIdentifierToHierarchy(void);
    /** Get TgcCoinHierarchy's numbers from Identifier */
    std::vector<unsigned int>* getIndexIdentifierToHierarchy(const Identifier identify) const;

    /** Show details of a vector of TgcCoinHierarchy's */
    void showHierarchyVector(const std::vector<TgcCoinHierarchy*>* hierarchyVector) const;
    /** Initializing and deleting variables */
    void prepareForNewEvent(void);

    /** Make maps from trigger sector to TgcCoinHierarchy's numbers and existence of TgcCoinHierarchy's */ 
    StatusCode makeFoundHierarchyTables(void);

    /** ITgcCoinHierarchyClassifyTool */
    ToolHandle<ITgcCoinHierarchyClassifyTool> m_classifyTool;
    /** ITgcCoinHierarchyFindTool */
    ToolHandle<ITgcCoinHierarchyFindTool> m_findTool;
    /** Data member of TgcCoinHierarchy's to hold inside TgcCoinHierarchySvc */
    std::vector<TgcCoinHierarchy*> m_hierarchyVector;
    /** Flag of DEBUG level */ 
    bool m_debug;
    /** Flag of a new event */
    bool m_newEvent;

    /** Maps from Identifier to TgcCoinHierarchy's numbers */
    std::map<const Identifier, std::vector<unsigned int>*> 
      m_indexIdentifierToHierarchy[TgcCoinHierarchy::NTYPES][TgcCoinHierarchy::NSTATIONS];
    /** Maps from trigger sector to existence of TgcCoinHierarchy's */
    bool m_foundHierarchy[TgcCoinHierarchy::NSIDES][TgcCoinHierarchy::NREGIONS][TgcCoinHierarchy::NPHIS];
    /** Maps from trigger sector to TgcCoinHierarchy's numbers */
    std::vector<unsigned int> 
      m_indexTriggerSectorToHierarchy[TgcCoinHierarchy::NSIDES][TgcCoinHierarchy::NREGIONS][TgcCoinHierarchy::NPHIS];

    /** Flag of ContainedROTS treatment Trk::Track with CompetingRIOsOnTrack */
    bool m_loopOverContainedROTS; 
  };
  
} // end of namespace Trigger

#endif // TRIGGER_TGCCOINHIERARCHYSVC_H 
