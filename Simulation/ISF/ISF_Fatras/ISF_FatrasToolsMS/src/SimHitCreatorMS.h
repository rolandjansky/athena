/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FATRASTOOLSMS_SIMHITCREATORMS_H
#define ISF_FATRASTOOLSMS_SIMHITCREATORMS_H

// Athena & Gaudi includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/IIncidentListener.h"
// ISF includes
#include "ISF_FatrasInterfaces/ISimHitCreator.h"
// Tracking includes
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/ITimedExtrapolator.h"
// Muon includes
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/MMSimHitCollection.h"
#include "MuonSimEvent/sTGCSimHitCollection.h"
//Muon ReadoutGeometry includes
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonTGRecTools/IMuonTGMeasTool.h"

// Muon
class MdtHitIdHelper;
class RpcHitIdHelper;
class CscHitIdHelper;
class TgcHitIdHelper;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Trk {
  class Layer;
}

namespace ISF {
    class ISFParticle;
}

struct MM_SimIdToOfflineId;
struct sTgcSimIdToOfflineId;

namespace iFatras 
{

  /** @class SimHitCreatorMS 

      A dedicated instance for Fatras sim hit creation
      in the Muon System.
      
      @author  Sharka Todorova <Sarka.Todorova@cern.ch>, 
               Andreas Salzburger <Andreas.Salzburger@cern.ch>
  */  

  class SimHitCreatorMS : public extends<AthAlgTool, ISimHitCreator, IIncidentListener>
    {
    public:
      SimHitCreatorMS(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~SimHitCreatorMS ()=default;
      
      virtual StatusCode initialize();

      /** handle for incident service */
      void handle(const Incident& inc);    

       /** Loop over the hits and call the hit creator - also provide the ISF particle to register the barcode */
      void createHits(const ISF::ISFParticle& isp, 
                      const std::vector<Trk::HitInfo>& hits) const;

      void initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl);
         
    private:

      /** Private HitCreate method - returns bool for a successful hit creation */       
      bool createHit(const ISF::ISFParticle& isp, const Trk::Layer* , const Trk::TrackParameters*, Identifier, double, double, bool) const;

      int  offIdToSimId(Identifier id) const;

      /** Incident Service */  
      ServiceHandle<IIncidentSvc>          m_incidentSvc;

      /** Tool using the track creator per event */
      ToolHandle<Trk::ITimedExtrapolator>       m_extrapolator;
      
      /** Muon TrackingGeometry Measurement Tool */
      ToolHandle<Muon::IMuonTGMeasTool>    m_measTool;

      /* Hit collections and collection helpers */
      MDTSimHitCollection                  *m_mdtSimHitCollection;
      RPCSimHitCollection                  *m_rpcSimHitCollection;
      TGCSimHitCollection                  *m_tgcSimHitCollection;
      CSCSimHitCollection                  *m_cscSimHitCollection;
      MMSimHitCollection          	   *m_mmSimHitCollection;
      sTGCSimHitCollection          	   *m_stgcSimHitCollection;
      std::string                          m_mdtCollectionName;
      std::string                          m_rpcCollectionName;
      std::string                          m_tgcCollectionName;
      std::string                          m_cscCollectionName;
      std::string                          m_mmCollectionName;
      std::string                          m_stgcCollectionName;
      
      /** Pointer to the random number generator service */
      ServiceHandle<IAtRndmGenSvc>         m_randomSvc;                //!< Random Svc  
      std::string                          m_randomEngineName;         //!< Name of the random number stream
      CLHEP::HepRandomEngine*              m_randomEngine;             //!< Random Engine 
      const MdtHitIdHelper*                m_mdtHitIdHelper;
      const RpcHitIdHelper*                m_rpcHitIdHelper;
      const CscHitIdHelper*                m_cscHitIdHelper;
      const TgcHitIdHelper*                m_tgcHitIdHelper;
      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
      MM_SimIdToOfflineId*                 m_mmOffToSimId;
      sTgcSimIdToOfflineId*                m_stgcOffToSimId;
 
      const MuonGM::MuonDetectorManager*   m_muonMgr;
      
      double                               m_mdtSigmaDriftRadius;
      
      mutable std::string                  m_stationName; 

      int   				   m_BMGid; //added to protect against dead sensors
      bool                                 m_createAllMdtHits;      
      bool   				   m_BMGpresent = false;//added to protect against dead sensors
      std::map<Identifier, std::vector<Identifier> > m_DeadChannels;
    }; 

    
} // end of namespace

#endif 
