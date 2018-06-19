//Dear emacs, this is -*-c++-*-
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
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

class LArOnOffIdMapping;
class CaloCell_ID;
class LArRawChannelContainer;

class LArCellMerger: public AthAlgTool,
	             virtual public ICaloCellMakerTool 

{
 
public:    
  
  LArCellMerger(const std::string& type, 
		const std::string& name, 
				 const IInterface* parent) ;


  /** initialize the tool
  */
  virtual StatusCode initialize() override; 

  /** finalize the tool
  */
  virtual StatusCode finalize() override; 

  /** update theCellContainer
  */
  virtual StatusCode process( CaloCellContainer * theCellContainer) override; //could be const if the abstract base-class were const



 private:

  /** handle to LAr cabling
  */
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey;


  /** key for raw channel container to merge to cells
  */  
  SG::ReadHandleKey<LArRawChannelContainer>  m_rawChannelContainerName; 

  const CaloCell_ID* m_calo_id;
};

#endif

