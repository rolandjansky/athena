/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StauTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUGIRLNSSTAUTOOL_H
#define MUGIRLNSSTAUTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "MuGirlInterfaces/IStauTool.h"
#include "MuGirlInterfaces/MdtSegmentMakerInfo.h"
#include "MuGirlInterfaces/RpcSegmentInfo.h"
#include "MuGirlStau/StauToolDefinitions.h"
#include "MuGirlStau/StauCalibration.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuGirlStau/StauCollection.h"
#include "TrkTrack/Track.h"
#include "MuGirlInterfaces/IGlobalFitTool.h"
#include "AthenaKernel/IOVSvcDefs.h" 
#include "ITrackInCaloTools/ITrackInCaloTools.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <map>

namespace Muon
{
class IMdtDriftCircleOnTrackCreator;
class IMuonSegmentMaker;
class MuonSegment;
}
class MdtCalibrationDbSvc;

namespace MuGirlNS
{

class StauNTuple;
class IStauBetaTofTool;
class StauRPC;
class StauMDT;
class StauGF;
class StauTRT;
class StauTileCal;
class StauMDTT;

/** @class StauTool

      This is for the Doxygen-Documentation.
      Please delete these lines and fill in information about
      the Algorithm!
      Please precede every member function declaration with a
      short Doxygen comment stating the purpose of this function.

      @author  shikma bressler <sshikma@tx.technion.ac.il>
  */

  class StauTool : virtual public IStauTool, public AlgTool
    {
    public:
      StauTool(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~StauTool ();

       /** standard Athena-Algorithm method */
      StatusCode initialize();
       /** standard Athena-Algorithm method */
      StatusCode finalize  ();

      /** interface to init necessary variable */
      void initTool(bool doGlobalFit)
      {
          m_doGlobalFit = doGlobalFit;
      }
      
      /** Main interace to process stau */
      StatusCode processStau(const xAOD::TrackParticle* trkParticle, 
                             const MuonSegmentList& muonSegments, 
                             const MuGirlNS::MdtSegmentMakerInfoList& mdtSegmentMakerInfoList, 
                             const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments,
                             const MuGirlNS::RIO_OnTrackLists& tgcHitsInSegments,
                             const Trk::Track* pMuonRefittedTrack = NULL,
                             const xAOD::TrackParticle* pMuonTrackParticle = NULL,
                             Amg::Vector3D trigMomentum = Amg::Vector3D(0., 0., 0.), const Trk::Perigee* trk0=NULL,bool isMuonBetaRefit=false);
      
      StatusCode processStauCosmic(const xAOD::TrackParticle* trkParticle, 
              const MuonSegmentList& muonSegments, 
              const MuGirlNS::MdtSegmentMakerInfoList& mdtSegmentMakerInfoList, 
              const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments,
              const MuGirlNS::RIO_OnTrackLists& tgcHitsInSegments,
              Amg::Vector3D trigMomentum );
      
      StatusCode readTagInfo(IOVSVC_CALLBACK_ARGS); 
                                      
      void clearCandidate();
      bool recalculateFirstSteps();
      bool selectRange(double& min, double& max);
      bool selectRangeTTrack(double& min, double& max);
      void clearNewMdtSegments();
      StatusCode fillStauSummary(CandidateSummary* summary,CandidateSummary* stauSummary);
      void consistencyCheck();
      void recordCollection();
      void setCandidateAnn(double ann);      

      /**ntuple functions*/
      StatusCode bookNTuple(NTuple::Tuple* pNTuple);
      StatusCode fillNTuple();
      
      //void setCandidateAnn(double);//{}
      bool doStauNtuple()
      {
         return (m_doMuGirlNtuple && m_doStauNtuple);
      }

      double beta()
      {
          return m_beta;
      }
      double mass()
      {
          return m_mass;
      }
      double tzero()
      {
          return m_tzero;
      }

      void setSegmentManager(SegmentManager* pSegmentManager)
      {
          m_pSegmentManager = pSegmentManager;
      }
      SegmentManager* getSegmentManager()
      {
          return m_pSegmentManager;
      }
      
      FcnStepsData* fcnStepsData()
      {
          return m_pFcnSteps;
      }
      bool addMuToF() const
      {
          return m_addMuToF;
      }
      bool rpcBugFix() const
      {
          return m_rpcBugFix;
      }
      const MuGirlNS::RIO_OnTrackLists* rpcHitsInSegments() const
      {
          return m_pRpcHitsInSegments;
      }
      const MuGirlNS::RIO_OnTrackLists* tgcHitsInSegments() const
      {
          return m_pTgcHitsInSegments;
      }
      const MuGirlNS::MdtSegmentMakerInfoList* mdtSegmentMakerInfoList() const
      {
          return m_pMdtSegmentMakerInfoList;
      }
      const ToolHandle<Muon::IMuonSegmentMaker>& mdtSegmentMaker() const
      {
          return m_pMdtSegmentMaker;
      }
      const ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>& driftCircleCreator() const
      {
          return  m_pMdtDriftCircleCreator;
      }
      const ToolHandle<IStauBetaTofTool>& tofTool() const
      {
          return m_pTofTool;
      }
      const ToolHandle<MuGirlNS::IGlobalFitTool>& globalFitTool() const
      {
          return m_pGlobalFitTool;
      }
      const ToolHandle<ITrackInCaloTools>& toCaloTool() const
      {
          return m_pToCalo;
      }
      const ToolHandle<Trk::ITrackParticleCreatorTool>& particleCreator() const
      {
          return  m_particleCreatorTool;
      }

      const ServiceHandle<MdtCalibrationDbSvc>& mdtCalibrationDbSvc()
      {
          return m_pMdtCalibDbSvc;
      }
      double idP() const
      {
          return m_idP;
      }
      virtual double stauP()
      {
          return m_p;
      }
      const Trk::Track* idTrack() const
      {
          return m_pIdTrack;
      }
      virtual MdtSegments* newMdtSegments()
      {
          return m_pNewMdtSegments;
      }
      const MuonGM::MuonDetectorManager* muonMgr()
      {
          return m_pMuonMgr;
      }
      void setRefittedTrack(const Trk::Track* pTrack);
      const Trk::Track* refittedTrack()
      {
          return m_pRefittedTrack;
      }
      StauRPC* pStauRPC()
      {
          return m_pStauRPC;
      }
      StauMDT* pStauMDT()
      {
          return m_pStauMDT;
      }
      StauTRT* pStauTRT()
      {
         return m_pStauTRT;
      }
      StauGF* pStauGF()
      {
          return m_pStauGF;
      }
      StauTileCal* pStauTileCal()
      {
          return m_pStauTileCal;
      }
      StauMDTT* pStauMDTT()
      {
          return m_pStauMDTT;
      }
      bool noIDTrack()
      {
          if(NULL==m_pIdTrack) return true;
          return false;
      }
      double rpcTimeShift()
      {
          return m_rpcTimeShift;
      }
      double tileEnergyCut()
      {
          return m_tileEnergyCut;
      }
      bool doGlobalFit() const
      {
          return m_doGlobalFit;
      }
      bool doMdt() const
      {
          return m_doMdt;
      }
      bool doTrt() const
      {
          return m_doTrt;
      }
      bool doRpc() const
      {
          return m_doRpc;
      }
      bool isData() const
      {
          return m_isData;
      }
      bool mdtSmearFactor() const
      {
          return m_mdtSmearFactor;
      }
      bool rpcSmearFactor() const
      {
          return m_rpcSmearFactor;
      }
      bool tileSmearFactor() const
      {
          return m_tileSmearFactor;
      }
      bool doTileCal() const
      {
          return m_doTileCal;
      }
      void setRpcHitsLists(StauRpcHitsLists* rpcHitsLists)
      {
          m_rpcHitsByStations = rpcHitsLists;
      }
      bool doConsistency()
      {
           return m_doConsistency;
      }
      std::map<StauTechnology,BetaChi2> techContribution2Chi2()
      {
           return m_techContribution2Chi2;
      }
      std::map<StauTechnology,BetaAvg> techBetaAvg()
      {
           return  m_techBetaAvg;
      }
      double avgBeta(StauTechnology eTech)
      {
           return m_techBetaAvg[eTech].beta;
      }
      double rmsBeta(StauTechnology eTech)
      {
           return m_techBetaAvg[eTech].rms;
      }
      std::map<StauTechnology,BetaChi2> techBetaChi2()
      {
           return  m_techBetaChi2;
      }
      double beta(StauTechnology eTech)
      {
           return m_techBetaChi2[eTech].beta;
      }
      double chi2(StauTechnology eTech)
      {
           return m_techBetaChi2[eTech].chi2;
      }
      double dof(StauTechnology eTech)
      {
           return m_techBetaChi2[eTech].dof;
      }
      double lowerBetaLimit()
      {
	  return m_lowerBetaLimit;
      }
      double upperBetaLimit()
      {
	  return m_upperBetaLimit;
      }
      
      double lowerTTrackLimit()
      {
       return m_lowerTTrackLimit;
      }
      double upperTTrackLimit()
      {
    	  return m_upperTTrackLimit;
      }
      inline bool hasSummary() 
      {
          return m_hasSummary;
      }
      bool doCalibration() const
      {
          return m_doCalibration;
      }
      inline StauCalibration& calibration()
      {
          return m_calibration;
      }
      inline double phaseShift() const
      {
          return m_phaseShift;
      }
    private:
      template <class T>
        StatusCode retrieve(ToolHandle<T>& pHandle, bool bError = true)
        {
            StatusCode sc = pHandle.retrieve();
            if (sc.isFailure())
            {
                if (bError)
                    m_log << MSG::ERROR << "Cannot retrieve tool " << pHandle << endreq;
                else
                    m_log << MSG::WARNING << "Cannot retrieve tool " << pHandle << endreq;
            }
            else
                m_log << MSG::INFO << "Retrieved tool " << pHandle << endreq;
            return sc;
        }
      template <class T>
        StatusCode retrieve(ServiceHandle<T>& pHandle, bool bError = true)
        {
            StatusCode sc = pHandle.retrieve();
            if (sc.isFailure())
            {
                if (bError)
                    m_log << MSG::ERROR << "Cannot retrieve service " << pHandle << endreq;
                else
                    m_log << MSG::WARNING << "Cannot retrieve service " << pHandle << endreq;
            }
            else
                m_log << MSG::INFO << "Retrieved service " << pHandle << endreq;
            return sc;
        }

      /** class member version of retrieving MsgStream */
      mutable MsgStream          m_log;
      int                        m_outputlevel;

      /** add muon tof flag  */
      bool m_addMuToF;
      
      /** rpc bug fix flag - to use on files digitized in releases 12.0.6 - 15.0.0 */
      bool m_rpcBugFix;

      /** rpc time shift - to correct jitter calibration */
      double m_rpcTimeShift;
     
      /** Use only tile cells with energy larger than this */

      double m_tileEnergyCut; 
      /** */
      bool m_doGlobalFit;
      bool m_doMdt;
      bool m_doTrt;
      bool m_doRpc;
      bool m_doTileCal;
      bool m_doConsistency;
      bool m_isData;
      double m_mdtSmearFactor;
      double m_rpcSmearFactor;
      double m_tileSmearFactor;
      bool m_doCalibration;
     
      /* calibration */
      std::string m_mdtCalibFile;
      std::string m_rpcCalibFile;
      std::string m_rpcNewCalibFile;
      std::string m_tileCalibFile;
      std::string m_phaseShiftFile;
      int m_runNumber; 
      double m_phaseShift; 

      /** Stau collection - save info in store gate */
      StauCollection* m_pCollection;

      /** tool handles */
      ToolHandle<Muon::IMuonSegmentMaker>                m_pMdtSegmentMaker;
      ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>    m_pMdtDriftCircleCreator;
      ToolHandle<IStauBetaTofTool>                       m_pTofTool;
      ToolHandle<MuGirlNS::IGlobalFitTool>               m_pGlobalFitTool;
      ToolHandle<ITrackInCaloTools>                      m_pToCalo;
      ToolHandle< Trk::ITrackParticleCreatorTool >       m_particleCreatorTool;
     
      ServiceHandle<MdtCalibrationDbSvc>                 m_pMdtCalibDbSvc;
      
      /** a handle on Store Gate for access to the Event Store */
      StoreGateSvc* m_storeGate;
      const EventInfo* m_pEventInfo;

      const MuonGM::MuonDetectorManager*                 m_pMuonMgr; /* the muon detector manager*/

      /** member variables for algorithm properties: */
      double    m_lowerBetaLimit;       /** the lower beta limit */
      double    m_upperBetaLimit;       /** the upper beta limit */
      double    m_betaTolerance;
      double    m_gfBetaTolerance;      /** the global fit beta tolerance */

      double    m_minBeta;       /** the lower beta limit - after fixing the range */
      double    m_maxBeta;       /** the upper beta limit - after fixing the range*/

      
      double    m_lowerTTrackLimit;       /** the lower ttrack limit */
      double    m_upperTTrackLimit;       /** the upper ttrack limit */

      double    m_minTTrack;       /** the lower ttrack limit - after fixing the range */
      double    m_maxTTrack;       /** the upper ttrack limit - after fixing the range*/


      bool m_mdtOnTheEdge;
      double m_lowerLimitNoMdt;
      double m_upperLimitNoMdt;
      
      /** NTuple related parameters */
      StauNTuple*                                        m_pStauNtuple; //the ntuple
      bool m_doMuGirlNtuple;
      bool m_doStauNtuple;
      
      /** The 'technologies' */
      StauRPC* m_pStauRPC;
      StauMDT* m_pStauMDT;
      StauTRT* m_pStauTRT;
      StauGF*  m_pStauGF;
      StauTileCal* m_pStauTileCal;
      StauMDTT* m_pStauMDTT;
      
      /** segment manager related parameters */
      std::vector<const Muon::MuonSegment*> m_mdtSegments;
      SegmentManager* m_pSegmentManager;

      /** the tool input */
      unsigned int m_skipStation;
      const MuGirlNS::MdtSegmentMakerInfoList*           m_pMdtSegmentMakerInfoList;
      const MuGirlNS::RIO_OnTrackLists*                  m_pRpcHitsInSegments;
      const MuGirlNS::RIO_OnTrackLists*                  m_pTgcHitsInSegments;
      double m_idP;
      const xAOD::TrackParticle*                         m_pTrkParticle;
      const Trk::Track*                                  m_pIdTrack;
      Amg::Vector3D m_trigP;

      /** the tool output */
      std::map<StauTechnology,BetaChi2> m_techBetaChi2;
      std::map<StauTechnology,BetaAvg> m_techBetaAvg;
      std::map<StauTechnology,BetaChi2> m_techContribution2Chi2;
      double m_beta;   //the beta to the AOD - for now m_beta = m_betaMS
      double m_mass;
      double m_tzero;
      double m_p;
      
      double m_tTrack;
      MdtSegments* m_pNewMdtSegments; // list of segments with the new beta
      const Trk::Track* m_pRefittedTrack;
      const Trk::Track* m_pMuonRefittedTrack;
      StauRpcHitsLists*    m_rpcHitsByStations; //list of RPC hits
      bool m_hasSummary;  //true if a stau summary was created to this candidate. Default is false;
  
      /** single step parameters
          clean this parameters when call to the FCN */
      FcnStepsData* m_pFcnSteps;

      /** candidate summary parameters */
      int m_numMdtSegs;
      int m_numMdtHitsInSeg;

       /** Calibration */
      StauCalibration m_calibration;

      /** */
      void printFcnSteps();
      void clearFcnSteps();
      void initializeCandidate(const xAOD::TrackParticle* trkParticle,
                               const MuGirlNS::MuonSegmentList& muonSegments,
                               const MuGirlNS::MdtSegmentMakerInfoList& mdtSegmentMakerInfoList,
                               const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments,
                               const MuGirlNS::RIO_OnTrackLists& tgcHitsInSegments,
                               const Trk::Track* pMuonRefittedTrackParticle,
                               const xAOD::TrackParticle* pMuonTrackParticle,
                               Amg::Vector3D trigMomentum);
      double stauMomentum();
      double stauMass();
      double computeTechnologyBeta(StauTechnology eTech, double minBeta, double maxBeta);
      void updateTechBetaChi2(StauTechnology eTech);
      void printTechBetaChi2();
      void updateTechContribution2Chi2();
      void printTechContribution2Chi2();
   };

} // end of namespace

#endif
