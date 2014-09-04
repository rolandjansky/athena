/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LArCellMerger_H
#define LARCELLREC_LArCellMerger_H

/** 
@class LArCellMerger
@brief Replace cells in calo cell container with cells from alternate raw channel container
@brief  Use case: when full calo cell container is made from DSP computed cells, replace some of then with better energies from offline reconstructed digits

 AlgTool properties (name defined in cxx file): 
     Name of the alternate raw channel container (i.e made from the digits offline)

 Created July 09, 2009  G.Unal
*/



#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthAlgTool.h"

class LArCablingService;
class StoreGateSvc;
class CaloCell_ID;

class LArCellMerger: public AthAlgTool,
	             virtual public ICaloCellMakerTool 

{
 
public:    
  
  LArCellMerger(const std::string& type, 
		const std::string& name, 
				 const IInterface* parent) ;


  /** initialize the tool
  */
  virtual StatusCode initialize() ; 

  /** finalize   the tool
  */
  virtual StatusCode finalize() ; 

  /** update theCellContainer
  */
  virtual StatusCode process( CaloCellContainer * theCellContainer) ;

  /** Callback added to handle Data-driven GeoModel initialisation
   */
  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);



 private:

  /** handle to LAr cabling service
  */
  ToolHandle<LArCablingService> m_cablingService;
  /** flags to select which errors to mask
  */

  /** key for raw channel container to merge to cells
  */
  std::string m_rawChannelContainerName;

  const CaloCell_ID* m_calo_id;

  /** Number of events processed
  */
  int m_evt;

  /** Number of cells read from new container
  */
  int m_ncell;

};

#endif

