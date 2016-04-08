/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EMECSupportConstruction.h
 *
 * @brief Declaration of EMECSupportConstruction class
 *
 * $Id: EMECSupportConstruction.h,v 1.5 2007-07-08 18:51:26 tsulaia Exp $
 */

// EMECSupportConstruction
// construct EMEC support structures
// 20-May-2003 AMS
// 06-Oct-2003 JT, modified for using GeoModel
#ifndef LARGEOENDCAP_EMECSUPPORTCONSTRUCTION_H
#define LARGEOENDCAP_EMECSUPPORTCONSTRUCTION_H

#include "RDBAccessSvc/IRDBAccessSvc.h"

class GeoPhysVol;
class GeoElement;
class GeoMaterial;
class GeoPcon;

/*
types:
	0 - front
	1 - back
	2 - outer
	3 - inner
	4 - middle

	10 - front only in region of inner wheel
	11 - back only in region of inner wheel
	12 - front only in region of outer wheel
	13 - back only in region of outer wheel
*/

namespace LArGeo {

  /** @class LArGeo::EMECSupportConstruction
      @brief GeoModel description of the EMEC Support Structures
   */
class EMECSupportConstruction
  {
  public:
    EMECSupportConstruction(
      int type,
      bool module = false,
      std::string basename = "LAr::EMEC::",
      double position = 0.
    );
    virtual ~EMECSupportConstruction();

    // Get the envelope containing this detector.
    virtual GeoPhysVol* GetEnvelope(void) const;

  private:
    GeoPhysVol *front_envelope(void) const;
    GeoPhysVol *back_envelope(void) const;
    GeoPhysVol *outer_envelope(void) const;
    GeoPhysVol *inner_envelope(void) const;
    GeoPhysVol *middle_envelope(void) const;
    GeoPhysVol *front_inner_envelope(void) const;
    GeoPhysVol *back_inner_envelope(void) const;
    GeoPhysVol *front_outer_envelope(void) const;
    GeoPhysVol *back_outer_envelope(void) const;

    void put_front_outer_extracyl(GeoPhysVol *) const;

    void put_front_inner_barettes(GeoPhysVol *) const;
    void put_front_inner_longbar(GeoPhysVol *) const;
    void put_front_inner_ring(GeoPhysVol *) const;
    void put_front_middle_ring(GeoPhysVol *) const;
    void put_back_middle_ring(GeoPhysVol *) const;
    void put_back_inner_ring(GeoPhysVol *) const;
    void put_back_inner_longbar(GeoPhysVol *) const;
    void put_back_inner_barettes(GeoPhysVol *) const;
    void put_front_outer_ring(GeoPhysVol *) const;
    void put_front_indexing_rings(GeoPhysVol *) const;
    void put_front_outer_longbar(GeoPhysVol *) const;
    void put_front_outer_barettes(GeoPhysVol *) const;
    void put_back_outer_ring(GeoPhysVol *) const;
    void put_back_indexing_rings(GeoPhysVol *) const;
    void put_back_outer_longbar(GeoPhysVol *) const;
    void put_back_outer_barettes(GeoPhysVol *) const;
    void put_front_outer_electronics(GeoPhysVol *) const;

	GeoPcon* getPcon(std::string) const;

    int m_Type;
    bool m_isModule;
    std::string m_BaseName;
    double m_Position, m_PhiStart, m_PhiSize;

	GeoMaterial *m_LAr, *m_Alu, *m_Gten, *m_PermaliE730;
	GeoMaterial *m_G10FeInner, *m_G10FeOuter, *m_Kapton_Cu;
	GeoMaterial *m_Cable, *m_Copper;
        GeoMaterial *m_Lead;

	IRDBRecordset_ptr m_DB_EmecGeometry, m_DB_EmecWheelParameters, m_DB_mn;
	IRDBRecordset_ptr m_DB_boxes, m_DB_numbers, m_DB_tubes, m_DB_pcons;
        IRDBRecordset_ptr m_DB_emecExtraCyl;
};

} // namespace LArGeo

#endif // LARGEOENDCAP_EMECSUPPORTCONSTRUCTION_H
