/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimHitCreatorMS.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLSMS_SIMHITCREATORMS_H
#define ISF_FATRASTOOLSMS_SIMHITCREATORMS_H

// Athena & Gaudi includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/IIncidentListener.h"
// ISF includes
#include "ISF_FatrasInterfaces/ISimHitCreator.h"
//#include "ISF_FatrasInterfaces/IHitCreator.h"
// Tracking includes
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/ITimedExtrapolator.h"
// Muon includes
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"

// Identifier
#include "Identifier/Identifier.h"
// Muon
class MdtHitIdHelper;
class RpcHitIdHelper;
class CscHitIdHelper;
class TgcHitIdHelper;
class sTgcHitIdHelper;
class MicromegasHitIdHelper;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {
  class IMuonTGMeasTool;
  class MuonIdHelperTool;
}

namespace Trk {
  class Layer;
  class Track;
  class TrackingGeometry;
}

namespace ISF {
    class ISFParticle;
}

struct MM_SimIdToOfflineId;
struct sTgcSimIdToOfflineId;

namespace iFatras 
{

   class IHitCreator;
    
  /** @class SimHitCreatorMS 

      A dedicated instance for Fatras sim hit creation
      in the Muon System.
      
      @author  Sharka Todorova <Sarka.Todorova@cern.ch>, 
               Andreas Salzburger <Andreas.Salzburger@cern.ch>
  */  

  class SimHitCreatorMS : virtual public ISimHitCreator, 
                            public AthAlgTool, virtual public IIncidentListener
    {
    public:
      SimHitCreatorMS(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~SimHitCreatorMS ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();

      /** handle for incident service */
      void handle(const Incident& inc);    

       /** Loop over the hits and call the hit creator - also provide the ISF particle to register the barcode */
      void createHits(const ISF::ISFParticle& isp, 
                      const std::vector<Trk::HitInfo>& hits) const;

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
      GenericMuonSimHitCollection          *m_mmSimHitCollection;
      GenericMuonSimHitCollection          *m_stgcSimHitCollection;
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
      MdtHitIdHelper*                      m_mdtHitIdHelper;
      RpcHitIdHelper*                      m_rpcHitIdHelper;
      CscHitIdHelper*                      m_cscHitIdHelper;
      TgcHitIdHelper*                      m_tgcHitIdHelper;
      //sTgcHitIdHelper*                     m_sTgcHitIdHelper;
      //MicromegasHitIdHelper*               m_mmHitIdHelper;
      MM_SimIdToOfflineId*                 m_mmOffToSimId;
      sTgcSimIdToOfflineId*                m_stgcOffToSimId;

      ToolHandle<Muon::MuonIdHelperTool>   m_idHelperTool; //!< Muon ID helper tool
 
      const MuonGM::MuonDetectorManager*   m_muonMgr;
      
      double                               m_mdtSigmaDriftRadius;
      
      mutable std::string                  m_stationName; 

      bool                                 m_createAllMdtHits;      
    }; 

    
} // end of namespace

#endif 
