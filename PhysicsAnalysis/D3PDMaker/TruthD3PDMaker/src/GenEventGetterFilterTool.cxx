/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenEventGetterFilterTool.cxx 348274 2011-02-28 16:25:06Z krasznaa $
/**
 * @file EventCommonD3PDMaker/src/GenEventGetterFilterTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2009
 * @brief Collection getter filter to remove empty GenEvent instances.
 */


#include "GenEventGetterFilterTool.h"
#include "AtlasHepMC/GenEvent.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
GenEventGetterFilterTool::GenEventGetterFilterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : CollectionGetterFilterTool<HepMC::GenEvent> (type, name, parent)
{
}


/**
 * @brief Test to see if this event is non-empty.
 */
bool GenEventGetterFilterTool::filter (const HepMC::GenEvent* p) const
{
  return ! p->particles_empty();
}


} // namespace D3PD
