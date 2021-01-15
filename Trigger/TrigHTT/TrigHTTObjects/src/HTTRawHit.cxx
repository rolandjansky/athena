/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTRawHit.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTTrack.h"

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"


#include <fstream>
#include <cmath> // needed for floor()

std::ostream &operator<<(std::ostream& out,const HTTRawHit& hit)
{
  static const char* detTechNames[] = { "Strip", "Pixel", "NTechs", "Undefined" };
  static const char* detZoneNames[] = { "Barrel", "Endcap", "NZones", "Undefined" };
  out << detTechNames[static_cast<int>(hit.m_detType)] << "\t" << detZoneNames[static_cast<int>(hit.m_detectorZone)] << "\t" << hit.m_layer_disk << "\t";
  out << hit.m_phi_module << "\t" <<  hit.m_eta_module << "\t";
  out << hit.m_phi_index << "\t" << hit.m_eta_index;
  return out;
}

/** This function change the layer id, how it comes from the atlas numbering
    method, from the id used in HTT pmap. This method update the
    m_layer_disk field for the strip, doesn't affect the pixels
 */
void HTTRawHit::normalizeLayerID()
{
  if (m_detType != SiliconTech::strip ) {
    return;
  }
  else m_layer_disk = (2*m_layer_disk) + m_side;
}


// TODO TODO TODO this is a temporary fix for release 21.9. Here, each strip chip
// (the entire physical 10cm x 10cm chip) is considered a single module, and has a
// single hashID. In rel22 a unique hashID will be assigned to each row of strips
// (4 or 2 per chip). This change fixes the 21.9 version to match the 22 version,
// by adding the row index (eta coord) to the hashID.
unsigned HTTRawHit::getIdentifierHash() const
{
    if (m_detType == SiliconTech::strip)
        return (m_IdentifierHash << 2) + getEtaIndex();
    else return m_IdentifierHash;
}
