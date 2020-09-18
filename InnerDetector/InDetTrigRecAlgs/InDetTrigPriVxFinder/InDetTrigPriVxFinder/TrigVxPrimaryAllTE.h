/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// a temporary copy of TrigVxPrimary to run in AllTE 
// by Viviana Cavaliere 


#ifndef INDETTRIGPRIVXFINDER_VXPRIMARYALLTE_H
#define INDETTRIGPRIVXFINDER_VXPRIMARYALLTE_H

#include "GaudiKernel/ToolHandle.h"

//!< Trigger specific stuff
#include "TrigInterfaces/AllTEAlgo.h"
#include <vector>

#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

/** Primary Vertex Finder.  InDetTrigPriVxFinder uses the
  InDetPrimaryVertexFinderTool in the package
  InnerDetector/InDetRecTools/InDetPriVxFinderTool. It only gives the
  trackcollection from storegate to it and records the returned
  VxContainer.
 */

class IBeamCondSvc;


namespace InDet
{
  class IVertexFinder;

  class TrigVxPrimaryAllTE : public HLT::AllTEAlgo
  {
  public:
    TrigVxPrimaryAllTE(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TrigVxPrimaryAllTE();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&, unsigned int output);    
    HLT::ErrorCode hltFinalize();
    
  private:
    int m_nTracks;
    int m_nVertices;
    std::vector<float>   m_zOfPriVtx;
    std::vector<float>   m_zOfPileUp;
    std::vector<float>   m_zOfNoVtx;
    bool                 m_runWithoutField;
    
    ToolHandle< IVertexFinder > m_VertexFinderTool;
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
    ServiceHandle<IBeamCondSvc> m_BeamCondSvc;

    bool                 m_retrieve_tracks_from_SG;
    std::string          m_track_collection_from_SG;
  };
}
#endif
