/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CosmicEventFilterTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class include
#include "CosmicEventFilterTool.h"

//Athena includes
#include "TrackRecord/TrackRecordCollection.h"
#include "StoreGate/ReadHandle.h"

// stl includes
#include <vector>
#include <string>

/** Constructor **/
ISF::CosmicEventFilterTool::CosmicEventFilterTool( const std::string& t,
                                                   const std::string& n,
                                                   const IInterface* p )
  : base_class(t,n,p)
  , m_ntot(0)
  , m_npass(0)
  , m_VolumeNames{""}
  , m_useANDFilter(true)
  , m_magicID(0)
  , m_ptMin(-1)
  , m_ptMax(-1)
{
  declareProperty("UseAndFilter", m_useANDFilter, "Use an AND Filter over all the Volumes, if false use an OR filter");
  declareProperty("VolumeNames", m_VolumeNames, "List of Volumes to use for filtering");
  declareProperty("PDG_ID", m_magicID, "If this is non-zero then more detailed checks will be made, requiring this particle to be present.");
  declareProperty("ptMin", m_ptMin, "Minimum pT for detailed cuts.");
  declareProperty("ptMax", m_ptMax, "Minimum pT for detailed cuts.");
}

/** Destructor */
ISF::CosmicEventFilterTool::~CosmicEventFilterTool() {
}


/** Athena algtool Hooks */
StatusCode  ISF::CosmicEventFilterTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");
  ATH_CHECK(m_VolumeNames.initialize());
  ATH_MSG_VERBOSE("initialize() successful");
  return StatusCode::SUCCESS;
}


/** Athena algtool Hook */
StatusCode  ISF::CosmicEventFilterTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter ");
  ATH_MSG_VERBOSE("finalize() successful");
  return StatusCode::SUCCESS;
}


/** Check that the current event passes this filter */
bool ISF::CosmicEventFilterTool::eventPassesFilter() const
{

  ++m_ntot;
  if (m_VolumeNames.empty()) { ++m_npass; return true;}
  bool evtPassesFilter(m_useANDFilter);
  auto trackRecordCollectionHandles = m_VolumeNames.makeHandles();
  for(auto& coll : trackRecordCollectionHandles) {
    if (!coll.isValid())
        {
          ATH_MSG_DEBUG("Cannot retrieve TrackRecordCollection " << coll.key());
          return false;
        }
      unsigned int counter(0);
      if (m_magicID!=0 || m_ptMin>0 || m_ptMax>0)
        {
          const unsigned int ntracks(coll->size());
          for (unsigned int track=0;track<ntracks;++track)
            {
              if (m_magicID!=0 && m_magicID != fabs(coll->operator[](track).GetPDGCode())) { continue; }
              if (m_ptMin>0 && m_ptMin > coll->operator[](track).GetMomentum().perp() ) { continue; }
              if (m_ptMax>0 && m_ptMax < coll->operator[](track).GetMomentum().perp() ) { continue; }
              ++counter;
            }// end of loop over tracks
        }
      else
        {
          counter = coll->size();
        }

      if(m_useANDFilter)
        { //AND Filter
          if(0==counter)
            { //One Failure is too many
              return false;
            }
        }
      else
        { //OR Filter
          if(0!=counter)
            { //One Success is enough
              ++m_npass;
              return true;
            }
        }
    }// end of loop over volumes

  if (evtPassesFilter)
    {
      ++m_npass;
    }
  return evtPassesFilter;
}
