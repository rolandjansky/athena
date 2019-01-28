/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDUTILS_LARFECLVTEMPDCSTOOL_H
#define LARCONDUTILS_LARFECLVTEMPDCSTOOL_H

/**
 @class LArFecLvTempDcsTool
 @brief concrete tool to provide FEC LV TEMP DCS information.

 Environment:
      Software developed for the ATLAS Detector at the CERN LHC

 @author 
      Denis Oliveira Damazio <damazio@bnl.gov>
*/

// Includes for Gaudi
#include "AthenaBaseComps/AthAlgTool.h"

#include "LArElecCalib/ILArFecLvTempDcsTool.h" 
#include "StoreGate/DataHandle.h" 
#include "Identifier/Identifier.h" 
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "StoreGate/DataHandle.h" 

#include <string>


// forward declaration
//using namespace coral;
class StoreGateSvc; 
class LArFecLvTempDcs;
class AttributeList;
class HWIdentifier;
class LArOnlineID;
class CondAttrListCollection; 

class LArFecLvTempDcsTool: public AthAlgTool, virtual public ILArFecLvTempDcsTool
{
 
  public: 
  LArFecLvTempDcsTool(const std::string& type, const std::string& name,
                      const IInterface* parent);

  virtual ~LArFecLvTempDcsTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize(){return StatusCode::SUCCESS;}


  // Given a HV ID, return its voltage value 
  virtual StatusCode getV1( const std::string& cratename, float&  volt  )  ; 
  virtual StatusCode getFec( const std::string& cratename
			, LArFecLvTempDcs&  volt  )  ; 
  virtual StatusCode getFec( const HWIdentifier& crateID
			, LArFecLvTempDcs&  volt  )  ; 

  private:

  void translateFec( const coral::AttributeList, LArFecLvTempDcs& ) const;

  const LArOnlineID* m_larOnlineId;
  std::string m_foldername;

  const DataHandle<CondAttrListCollection> m_atrlistcol;

};

#endif
