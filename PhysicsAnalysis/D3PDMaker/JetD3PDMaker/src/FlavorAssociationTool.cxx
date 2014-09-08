/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMaker/JetD3PDMaker/src/FlavorAssociationTool.cxx
 * @author Erik Devetak <erik.devetak@cern.ch>
 * @date Mar, 2009
 * @brief setting up an association of jets to tracks
 *        represented by indices. 
 */


#include "FlavorAssociationTool.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagEvent/TrackAssociation.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
FlavorAssociationTool::FlavorAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : TrackToJetBase(type, name, parent)
{
}

/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode FlavorAssociationTool::initialize()
{
  return  TrackToJetBase::initialize();
}


/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode FlavorAssociationTool::reset (const  Jet& p)
{
  const Analysis::TrackAssociation* trackassoc = p.getAssociation<Analysis::TrackAssociation>("Tracks");
  if (trackassoc !=0)
    {
      std::vector<const Rec::TrackParticle*>* tracks = trackassoc->tracks();
      if(tracks->size() !=0)
	{
	  m_trkItr = tracks->begin();
	  m_trkEnd = tracks->end();
	}
      else
	{
	  m_trkItr = m_trkEnd;
	}
    }
  else
    {
      m_trkItr=m_trkEnd;
    }
  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Rec::TrackParticle* FlavorAssociationTool::next()
{

  // reached the end
  if(m_trkItr == m_trkEnd)
    {
      return 0;
    }
  else
    {
      const Rec::TrackParticle* obj= *m_trkItr;
      *m_trkItr++;
      return obj;
    }
  return 0;
}

} // namespace D3PD
 
