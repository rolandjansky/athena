/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOCELLNEIGHBORSAVERAGECORR_H
#define CALOREC_CALOCELLNEIGHBORSAVERAGECORR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include <string>

class CaloCellContainer;
class CaloDetDescrManager;
class CaloCell_ID;
class TileID;

//inspiration from http://alxr.usatlas.bnl.gov/lxr-stb3/source/atlas/Calorimeter/CaloRec/CaloRec/CaloCellCopyTool.h#032
class CaloCellNeighborsAverageCorr :  public AthAlgTool, 
                                      virtual public ICaloCellMakerTool

{

public:

  CaloCellNeighborsAverageCorr(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~CaloCellNeighborsAverageCorr() {};

  /** initialize method
  */
  virtual StatusCode initialize();

  /** process calo cell collection to apply corrections
  */
  StatusCode process( CaloCellContainer * theCellContainer);

  /** Callback added to handle Data-driven GeoModel initialisation
   */
  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);



private:

 const CaloDetDescrManager* m_calo_dd_man;
 const CaloCell_ID* m_calo_id;
 const TileID* m_tile_id;
 bool m_testMode;
 bool m_skipDeadFeb;
 bool m_skipDeadLAr;
 bool m_skipDeadDrawer;
 bool m_skipDeadTile;

};

#endif
