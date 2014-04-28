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
#ifndef __EMECSupportConstruction_H__
#define __EMECSupportConstruction_H__

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

    int Type;
    bool isModule;
    std::string BaseName;
    double Position, PhiStart, PhiSize;

	GeoMaterial *LAr, *Alu, *Gten, *PermaliE730;
	GeoMaterial *G10FeInner, *G10FeOuter, *Kapton_Cu;
	GeoMaterial *Cable, *Copper;
        GeoMaterial *Lead;

	IRDBRecordset_ptr DB_EmecGeometry, DB_EmecWheelParameters, DB_mn;
	IRDBRecordset_ptr DB_boxes, DB_numbers, DB_tubes, DB_pcons;
        IRDBRecordset_ptr DB_emecExtraCyl;
};

} // namespace LArGeo

#endif // __EMECSupportConstruction_H__
