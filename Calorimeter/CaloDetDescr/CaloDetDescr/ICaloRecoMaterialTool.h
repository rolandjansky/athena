/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ICaloMaterialTool.h 
 *
 * @brief declaration of ICaloMaterialTool interface
 * 04.8.2005 Creation of the interface CaloDetDescr/ICaloMaterialTool : the
 *           LAr/Tile implementation are in the LArDetDescr/TileDetDescr packages
 */

#ifndef CALODETDESCR_ICALOMATERIALTOOL_H
#define CALODETDESCR_ICALOMATERIALTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"
#include "CaloDetDescr/CaloSubdetNames.h"

/** @class ICaloRecoMaterialTool

    @brief This (clean) interface is driven by 2 constraints :

      - LAr and Tile information reside in two LAr/TileDetDescrManager,
        and Calorimeter MUST not depend on any LAr/Tile package.
	The client will thus have to retrieve the 2 tools implementing this 
	interface for LAr and for Tile.

      - no LAr/Tile package is allowed to depend on Tracking base clases:
        this tool can thus only return numbers, which will be used in the
	CaloTrackingGeometry package to build the Trk::Volumes

 **/

class ICaloRecoMaterialTool : virtual public IAlgTool
{
public:

  /** @brief the enum ALIGNVOL covers all LAr pieces, including cryostats 
   */
  virtual bool get_material (CaloSubdetNames::ALIGNVOL alvol, 
			     double& mass, double& volume,
			     double& x0, double& dEdx,
			     double& aveA, double& aveZ
			     ) = 0;
  
  /** @brief default material used to fill the envelope 
   */
  virtual void get_default_material (double& density,
				     double& x0, double& dEdx,
				     double& aveA, double& aveZ
				     ) = 0;

  /** @brief printout method
   */
  virtual void print() = 0;

};

 
#endif // CALODETDESCR_ICALOMATERIALTOOL






