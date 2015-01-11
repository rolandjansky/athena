/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBUTILS_LArOFPhaseFill_H
#define LARCALIBUTILS_LArOFPhaseFill_H

/********************************************************************

NAME:     LArOFPhaseFill.h 
AUTHORS:  P. Strizenec
used LArFill  by S. Laplace
CREATED:  Mar. 2008

Used to fill a list of picked OFC phases from text file - default is 0 
Use a Tshaper raw conditions - reusing existing class instead of writing new
********************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h" 

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "LArRawConditions/LArOFCBinComplete.h"

#include "StoreGate/DataHandle.h"
#include "AthenaKernel/IOVSvcDefs.h" 


class LArOFPhaseFill : public AthAlgorithm
{

 public:

  LArOFPhaseFill(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArOFPhaseFill();

  StatusCode initialize();
  StatusCode execute();
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

 private:

  // Services 
  const LArOnlineID*        m_lar_on_id;

  LArOFCBinComplete*   m_OFCbin;

  std::string m_InputFile; 
  std::string m_keyOFCbin;
  std::string m_groupingName;
  int m_groupingType;
  unsigned m_defaultPhase;

};
#endif
