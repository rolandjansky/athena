/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GOODNESSMUONFILLER_H
#define GOODNESSMUONFILLER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ObjectVector.h"
//#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "MissingETGoodness/Goodies.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
//class IMuonTRTTimingTool;
class MuonGoodiesFiller : public AthAlgorithm
{
 public:

  MuonGoodiesFiller( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~MuonGoodiesFiller( );

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();


 private:

  static MET::Goodies& s_goodies;
  //ToolHandle< IMuonTRTTimingTool > p_ITRTTimingTool ; //!< Pointer on IMuonTRTTimingTool
  //storegate key names
  std::string m_muonSpShowerContainerKey;
  std::string m_muonContainerKey;
};

#endif  

