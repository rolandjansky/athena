/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloAlignTool.h
 *
 * @brief ICaloSimpleGeomTool interface declaration 
 * 19.9.2005 Creation of the interface CaloDetDescr/ICaloSimpleGeomTool : the 
             LAr/Tile implementation are in the LArDetDescr/TileDetDescr packages
 */

#ifndef CALODETDESCR_ICALOSIMPLEGEOMTOOL_H
#define CALODETDESCR_ICALOSIMPLEGEOMTOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloSubdetNames.h"

/** @class ICaloSimpleGeomTool
    @brief T his (clean) interface is driven by 2 constraints :

      - LAr and Tile information reside in two LAr/TileDetDescrManager,
        and Calorimeter MUST not depend on any LAr/Tile package.
	The client will thus have to retrieve the 2 tools implementing this 
	interface for LAr and for Tile.

      - no LAr/Tile package is allowed to depend on Tracking base clases:
        this tool can thus only return numbers, which will be used in the
	CaloTrackingGeometry package to build the Trk::Volumes

 **/

class ICaloRecoSimpleGeomTool : virtual public IAlgTool
{
public:

  /** @brief the enum ALIGNVOL covers all LAr pieces, including cryostats and coil 
   */
  virtual bool get_cylinder_surface (CaloSubdetNames::ALIGNVOL alvol,
                                     Amg::Transform3D& htrans,double& hphi,
                                     std::vector<double>& radius,  
				     std::vector<double>& depth, 
                                     std::vector<double>& hlength )=0;
  
  /** @brief the enum ALIGNVOL covers all LAr pieces, including cryostats and coil 
   */
  virtual bool get_disk_surface (CaloSubdetNames::ALIGNVOL alvol,
                                 Amg::Transform3D& htrans,double& hphi,
				 std::vector<double>& z, 
				 std::vector<double>& depth,
                                 std::vector<double>& rmin, std::vector<double>& rmax )=0;  
  /** @brief method for printout
   */ 
  virtual void print() = 0;

};

 
#endif // CALODETDESCR_ICALOSIMPLEGEOMTOOL






