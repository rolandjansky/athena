/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONROADCHAMBERASSOCTOOL_H
#define MUONROADCHAMBERASSOCTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "MuonPattern/MuonPatternChamberIntersect.h"
#include "MuonHoughPatternEvent/MuonHoughMathUtils.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MuonPrepDataCollection.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkRoad/TrackRoad.h"
#include "MuonPatternFinderToolInterfaces/IMuonRoadChamberAssocTool.h"

class StoreGate;
class MsgStream;

namespace Muon {

  class MuonRoadChamberAssocTool : virtual public IMuonRoadChamberAssocTool, public AthAlgTool
  {
    
  public :
    /** default constructor */
    MuonRoadChamberAssocTool (const std::string& type, const std::string& name, const IInterface* parent);
    /** destructor */
    virtual ~MuonRoadChamberAssocTool();

    //retrieve the hits and store the pattern
    MuonPatternCombination* execute(const Trk::TrackRoad road);

    static const InterfaceID& interfaceID();

    virtual StatusCode initialize(void);  
    virtual StatusCode finalize(void);  
    
  private:
    StoreGateSvc* m_storeGate;    
    std::string mdtkey;
    std::string rpckey;
    std::string tgckey;
    std::string csckey;
    const MdtIdHelper* m_mdtIdHelper;
    const RpcIdHelper* m_rpcIdHelper;
    const TgcIdHelper* m_tgcIdHelper;
    const CscIdHelper* m_cscIdHelper;        

    MuonHoughMathUtils m_muonHoughMathUtils;
    double m_PI;

    bool isInPhiBounds(double phi, double roadPhi, double phiWidth);

  };
  
  
}
#endif //MUONROADCHAMBERASSOCTOOL_H
