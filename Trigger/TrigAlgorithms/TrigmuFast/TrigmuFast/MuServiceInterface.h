/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSICS_MU_MUSERVICEINTERFACE_H
#define PHYSICS_MU_MUSERVICEINTERFACE_H



#ifdef LVL2_STANDALONE

#include "RPCgeometry/RPCGeometry.h"
#include "RPCcablingInterface/CablingRPCBase.h"


#include "MDTcabling/MDTgeometry.h"

#include "TrigmuFast/MuRoads.h"
#include "TrigmuFast/MuLUT.h"

#else

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigMuonBackExtrapolator/ITrigMuonBackExtrapolator.h"

#include "RPCcablingInterface/CablingRPCBase.h"

#include "MdtCalibSvc/MdtCalibrationSvc.h"

#include "MDTcabling/IMDTcablingSvc.h"
#include "RPCgeometry/IRPCgeometrySvc.h"

#include "MuonRPC_Cabling/MuonRPC_CablingSvc.h"

#include "TrigmuFast/MuRoadsSvc.h"
#include "TrigmuFast/MuLUTSvc.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "MuonIdHelpers/TgcIdHelper.h"

#endif

///////////////////////////////////////////////////////////////////////////////

#define NPROC 13

//class RPCGeometry;
class MDTGeometry;
class PtLUTSvc;
//class RPCgeometrySvc;
class CSCgeometrySvc;
class TGCgeometrySvc;

struct TGCRoadParameters
{
    double ETA_LOW_MID_DELTA;
    double ETA_HIGH_MID_DELTA;
    double PHI_MID_DELTA;
    double ETA_LOW_INN_DELTA;
    double ETA_HIGH_INN_DELTA;
    double PHI_INN_DELTA;
    
    TGCRoadParameters()
    {
        ETA_LOW_MID_DELTA  = 0.10;
        ETA_HIGH_MID_DELTA = 0.05;
        PHI_MID_DELTA      = 0.05;
        ETA_LOW_INN_DELTA  = 0.15;
        ETA_HIGH_INN_DELTA = 0.10;
        PHI_INN_DELTA      = 0.10;
    }
};

struct TGCFitParameters
{
    double CHI2_TEST;
    unsigned MIN_WIRE_POINTS;
    unsigned MIN_STRIP_POINTS;

    TGCFitParameters()
    {
        CHI2_TEST = 10.0;
        MIN_WIRE_POINTS = 4;
        MIN_STRIP_POINTS = 3;
    }
};

class MuServiceInterface
{
    public:
#ifdef LVL2_STANDALONE
    MuServiceInterface();
#else
    MuServiceInterface();
    MuServiceInterface(ISvcLocator* locator,
                       ServiceHandle<IRPCgeometrySvc>* RpcGeometry,
                       ServiceHandle<TGCgeometrySvc>*  TgcGeometry,
		       ServiceHandle<CSCgeometrySvc>*  CscGeometry,
		       ServiceHandle<IMDTcablingSvc>*  MdtCabling,
                       ToolHandle<ITrigMuonBackExtrapolator>* backExtrapolator,
                       std::string name,bool use_calib,
                       bool cosmic, bool bfield, 
		       bool use_mcLUT, bool use_pol2ForEPt,
		       double beta,
                       const TGCRoadParameters* tgc_road_parameters = NULL,
                       const TGCFitParameters* tgc_fit_parameters = NULL);
    MuServiceInterface(const MuServiceInterface& svc);
    MuServiceInterface operator=(const MuServiceInterface& svc);
#endif

    ~MuServiceInterface();


    private:
    std::string m_algoName;
    double m_beta;
    //const RPCGeometry*    m_rpcgeo;
    const CablingRPCBase* m_rpccab;

    const IRPCcablingSvc* m_iRpcCablingSvc;

#ifndef LVL2_STANDALONE
    TrigTimer* m_timers[NPROC];
    MdtCalibrationSvc*     m_pMdtCalibration;
    const MdtIdHelper*     m_pMdtIdHelper;
    const TGCgeometrySvc*  m_pTgcLUTSvc;
    const CSCgeometrySvc*  m_pCscLUTSvc;
    const IMDTcablingSvc*  m_pMdtCABSvc;
    const IRPCgeometrySvc* m_pRpcLUTSvc;
    const PtLUTSvc*        m_pPtLUTSvc;
    //ServiceHandle<IMDTcablingSvc>* m_pMdtCABSvc;
    ToolHandle<ITrigMuonBackExtrapolator>* m_backExtrapolator;
    
#endif
    //const MDTGeometry* m_mdtgeo;
    const MuRoads*  m_roads;
    const MuLUT*    m_lut;

    bool m_load;
    bool m_use_calib;
    bool m_cosmic_run;
    bool m_b_field_on;
    TGCRoadParameters m_tgc_road_parameters;
    TGCFitParameters m_tgc_fit_parameters;

    public:
    const RPCGeometry*    rpcgeo(void) const {return m_pRpcLUTSvc->rpcGeometry();}
    const CablingRPCBase* rpccab(void) const {return m_rpccab;}
     
    const IRPCcablingSvc* iRpcCablingSvc(void) const {return m_iRpcCablingSvc;}

#ifndef LVL2_STANDALONE
    TrigTimer*            timers(void) const {return m_timers[0];}

    bool load_timer(TrigTimer* timer, unsigned short int number);
    TrigTimer* give_timer(unsigned short int number) const;
    const MdtIdHelper*    mdtIdHelper(void) const    {return m_pMdtIdHelper;}
    //MuonTBCalibrationSvc* calibration(void) const    {return m_pMuonTBCalibration;}
    MdtCalibrationSvc*     mdtCalibration(void) const {return m_pMdtCalibration;}
    const TGCgeometrySvc*  tgcLutSvc(void) const      {return m_pTgcLUTSvc;}
    const CSCgeometrySvc*  cscLutSvc(void) const      {return m_pCscLUTSvc;}
    const IMDTcablingSvc*  mdtCABSvc(void) const      {return m_pMdtCABSvc;}
    const IRPCgeometrySvc* rpcLUTSvc(void) const      {return m_pRpcLUTSvc;} 
    const PtLUTSvc*        ptLUTSvc(void) const       {return m_pPtLUTSvc;}
    
    const ToolHandle<ITrigMuonBackExtrapolator>* backExtrapolator(void) const
                                                    {return m_backExtrapolator;}
#endif
    const MDTGeometry* mdtgeo(void) const {return m_pMdtCABSvc->mdtGeometry();}
    const MuRoads*     roads(void)  const {return m_roads;}
    const MuLUT*       lut(void)    const {return m_lut;}
    double             beta(void)   const {return m_beta;}
    std::string        name(void)   const {return m_algoName;}

    bool isCosmic(void) const {return m_cosmic_run;}
    bool isLoaded(void) const {return m_load;}
    const TGCRoadParameters& tgcRoadParameters() const {return m_tgc_road_parameters;}
    const TGCFitParameters& tgcFitParameters() const {return m_tgc_fit_parameters;}

    void mdtCalibration(int StationName, int StationEta, int StationPhi,
                        int Multilayer, int layer, int Tube,
                        int& tdcCounts, int adcCounts, int triggerTime,
                        float X, float Y, float Z, float phi,
                        float& driftTime, float& driftSpace, float& driftSigma)
                        const;
			
    bool etaInner(int address, double etaMiddle, double phiMiddle, double PT,
                  double& etaInner,double& sigmaEtaInner) const; 


    bool hasEcut(char num) const;
};



#endif  // PHYSICS_MU_MUSERVICEINTERFACE_H
