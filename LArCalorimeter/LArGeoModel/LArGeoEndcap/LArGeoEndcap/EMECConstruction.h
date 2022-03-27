/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EMECConstruction.h
 *
 * @brief Declaration of EMECConstruction class
 *
 * $Id: EMECConstruction.h,v 1.9 2009-02-10 16:50:53 tsulaia Exp $
 */

#ifndef LARGEOENDCAP_EMECCONSTRUCTION_H
#define LARGEOENDCAP_EMECCONSTRUCTION_H

#include<vector>
#include <string>
class GeoFullPhysVol;
class GeoMaterial;

namespace LArGeo {

  /** @class LArGeo::EMECConstruction
      @brief GeoModel description of the LAr EMEC envelope and the active part (custom shapes)
   */
  class EMECConstruction
  {
  public:

    EMECConstruction(bool is_tb = false, bool has_inner = true, bool has_outer = true);
    virtual ~EMECConstruction();

    // Get the envelope containing this detector.
    virtual GeoFullPhysVol* GetEnvelope(bool bPos = true);

    // Set fullGeo flag
    void setFullGeo(bool flag);

    void setInnerVariant(const std::string &v){ m_innerWheelVariant = v; }
    void setOuterVariant(const std::string &v){ m_outerWheelVariant = v; }

  private:
    bool        m_fullGeo;  // true->FULL, false->RECO

    bool	    m_isTB;
    bool	    m_hasInnerWheel;
    bool	    m_hasOuterWheel;

    std::string m_innerWheelVariant;
    std::string m_outerWheelVariant;

    void place_custom_solids(
        GeoFullPhysVol *fullPV,
        std::vector<std::string> &absorbers,
        std::vector<std::string> &electrodes,
        int multilayered_absorbers,
        const GeoMaterial *Absorber, const GeoMaterial *Electrode,
        const GeoMaterial *Glue, const GeoMaterial *Lead
    );

  };

} // namespace LArGeo

#endif // __EMECConstruction_H__
