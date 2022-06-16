/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBUTILS_LArOFPhaseFill_H
#define LARCALIBUTILS_LArOFPhaseFill_H

/********************************************************************

NAME:     LArOFPhaseFill.h 
AUTHORS:  P. Strizenec
CREATED:  Mar. 2008

Used to fill a list of picked OFC phases from text file - default is 0 
Use a Tshaper raw conditions - reusing existing class instead of writing new
********************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h" 
#include "LArIdentifier/LArOnlineID_Base.h"
#include "LArRawConditions/LArOFCBinComplete.h"
#include "AthenaKernel/IOVSvcDefs.h"


class LArOFPhaseFill : public AthAlgorithm
{

 public:

  LArOFPhaseFill(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArOFPhaseFill() = default;

  StatusCode initialize();
  StatusCode execute() {return StatusCode::SUCCESS;};
  StatusCode stop();

 private:

  // Services 
  const LArOnlineID_Base*        m_lar_on_id;

  StringProperty m_InputFile {this,"InputFile", "", "Which file to read"}; 
  StringProperty m_keyOFCbin {this,"keyOFCBin","LArOFCBin","Output key of LArOFCBinComlete object"};
  StringProperty m_groupingName {this, "GroupingType", "Unknown", "Which grouping type to use"};
  Gaudi::Property<unsigned int> m_defaultPhase {this, "DefaultPhase", 0, "Phase used for channels not in input file"};
  BooleanProperty m_isID {this, "isID", false, "contains the input file onlineID ?"};
  BooleanProperty m_isSC {this, "isSC", false, "Is this the SuperCell ?"};
  int m_groupingType;

};
#endif
