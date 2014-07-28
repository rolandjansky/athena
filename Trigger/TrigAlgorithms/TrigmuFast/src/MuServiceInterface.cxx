/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LVL2_STANDALONE

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "RPCgeometry/RPCgeometrySvc.h"
#include "TGCgeometry/TGCgeometrySvc.h"
#include "CSCgeometry/CSCgeometrySvc.h"
#include "TrigmuFast/PtLUTSvc.h"

#endif

#include "TrigmuFast/MuServiceInterface.h"
#include "MuonCalibEvent/MdtCalibHit.h"

#include "GeoPrimitives/GeoPrimitives.h"

///////////////////////////////////////////////////////////////////////////////

#ifdef LVL2_STANDALONE
MuServiceInterface::MuServiceInterface() : m_algoName(""),m_beta(1.),
                                           m_rpccab(0),
					   m_pTgcLUTSvc(0),m_pCscLUTSvc(0),
					   m_pMdtCABSvc(0),m_pRpcLUTSvc(0),
					   m_roads(0),m_lut(0),
					   m_load(false),m_use_calib(false),
					   m_cosmic_run(false),
                                           m_b_field_on(false)
{}

#else

MuServiceInterface::MuServiceInterface() : m_algoName(""),m_beta(1.),
                                           m_rpccab(0),
                                           m_pTgcLUTSvc(0),m_pCscLUTSvc(0),
					   m_pMdtCABSvc(0),m_pRpcLUTSvc(0),
					   m_backExtrapolator(0),
					   m_roads(0),m_lut(0),
					   m_load(false),m_use_calib(false),
					   m_cosmic_run(false),
					   m_b_field_on(false)
{
    for(int i=0;i<NPROC;++i) m_timers[i] = 0;
}

MuServiceInterface::MuServiceInterface(ISvcLocator* locator,
    ServiceHandle<IRPCgeometrySvc>* RpcGeometry,
    ServiceHandle<TGCgeometrySvc>*  TgcGeometry,
    ServiceHandle<CSCgeometrySvc>*  CscGeometry,
    ServiceHandle<IMDTcablingSvc>*  MdtCabling,
    ToolHandle<ITrigMuonBackExtrapolator>* backExtrapolator, std::string name,
    bool use_calib, bool cosmic, bool bfield, 
    bool use_mcLUT, bool use_pol2ForEPt,
    double beta,
    const TGCRoadParameters* tgc_road_parameters,
    const TGCFitParameters* tgc_fit_parameters)
{
    m_algoName   = name;
    m_beta       = beta;
    m_load       = false;
    m_use_calib  = use_calib;
    m_cosmic_run = cosmic;
    m_b_field_on = bfield;
    
    if (tgc_road_parameters != NULL)
        memcpy(&m_tgc_road_parameters, tgc_road_parameters, sizeof(TGCRoadParameters));
    if (tgc_fit_parameters != NULL)
        memcpy(&m_tgc_fit_parameters, tgc_fit_parameters, sizeof(TGCFitParameters));
    
    
    //m_rpcgeo = (**RpcGeometry).rpcGeometry();
    m_pRpcLUTSvc = &(**RpcGeometry);
    m_pTgcLUTSvc = &(**TgcGeometry);
    m_pCscLUTSvc = &(**CscGeometry);
    m_pMdtCABSvc = &(**MdtCabling);
    
    m_backExtrapolator = backExtrapolator;

    for(int i=0;i<NPROC;++i) m_timers[i] = 0;
    
    StatusCode sc;
    
    IMessageSvc*  msgSvc;
    sc = locator->service( "MessageSvc", msgSvc );
    if ( sc.isFailure() ) return;
        
    MsgStream log(msgSvc, "MuServiceInterface");

    log << MSG::DEBUG << "Start getting Muon services"
        << endreq;




    // =================== GET THE RPC GEOMETRY SERVICE  =====================
    /*
    const RPCgeometrySvc* pRpcGeo = 0;
    sc = locator->service("RPCgeometrySvc", pRpcGeo);
    if(sc.isFailure())
    {
        log << MSG::FATAL
            << "Unable to retrieve the RPC geometry Service"
            << endreq;
        return;
    }
    m_rpcgeo = pRpcGeo->rpcGeometry();
    */

    // ==================== GET THE RPC CABLING SERVICE  =====================

    const IRPCcablingServerSvc* pRpcCabGet = 0;
    sc = locator->service("RPCcablingServerSvc", pRpcCabGet,true);
    if(sc.isFailure())
    {
        log << MSG::FATAL
            << "Unable to retrieve the RPC cabling Server Service"
            << endreq;
        return;
    }
    // const IRPCcablingSvc* pRpcCab = 0;
    // sc = pRpcCabGet->giveCabling(pRpcCab);
    m_iRpcCablingSvc = 0;
    sc = pRpcCabGet->giveCabling(m_iRpcCablingSvc);
    if(sc.isFailure())
    {
        log << MSG::FATAL
            << "Unable to retrieve the RPC cabling Service from Server"
            << endreq;
        return;
    }
    // m_rpccab = pRpcCab->getRPCCabling();
    m_rpccab = m_iRpcCablingSvc->getRPCCabling();

    // ==================== GET THE MDT CABLING SERVICE  =====================
    //const MDTcablingSvc* mdtCabling = 0;
    //sc = locator->service("MDTcablingSvc", mdtCabling);
    //if(sc.isFailure())
    //{
    //    log << MSG::FATAL
    //        << "Unable to retrieve the MDT cabling Service"
    //        << endreq;
    //    return;
    //}
    //m_mdtgeo = mdtCabling->mdtGeometry();
    m_pMdtIdHelper = m_pMdtCABSvc->idHelper();

    
    // ==================== GET THE CSC LUT SERVICE  =====================
    /*
    const CSCgeometrySvc* pCscLutSvc = 0;
    sc = locator->service("CSCgeometrySvc", pCscLutSvc);
    if(sc.isFailure())
    {
        log << MSG::FATAL
            << "Unable to retrieve the CSC LUT Service"
            << endreq;
        return;
    }
    m_pCscLUTSvc = pCscLutSvc;
    */
    // ==================== GET THE PT LUT SERVICE  =====================
    const PtLUTSvc* pPtLUTSvc = 0;
    if( use_pol2ForEPt ) {
       if( use_mcLUT ) sc = locator->service("PtLUTSvc_pol2_MC", pPtLUTSvc);
       else            sc = locator->service("PtLUTSvc_pol2",    pPtLUTSvc);
    }
    else {
       if( use_mcLUT ) sc = locator->service("PtLUTSvc_MC", pPtLUTSvc);
       else            sc = locator->service("PtLUTSvc",    pPtLUTSvc);
    }
    if(sc.isFailure())
    {
        log << MSG::FATAL
            << "Unable to retrieve the PT LUT Service"
            << endreq;
        return;
    }
    m_pPtLUTSvc = pPtLUTSvc;


    // ================== GET THE MDT CALIBRATION SERVICE  ====================
    //m_pMuonTBCalibration = 0;
    m_pMdtCalibration = 0;
    if(use_calib)
    {
        
//        if (rpcgeo()->isFull())
//	{   // Normal ATLAS setup: use the standard Calibration Service
            sc = locator->service("MdtCalibrationSvc", m_pMdtCalibration );
            if(sc.isFailure())
            {
                log << MSG::WARNING
                    << "Unable to retrieve the MDT calibration Service"
                    << endreq;
	        log << MSG::WARNING
                    << "Proceed using dummy calibration for MDT"
                    << endreq;
            }
/*	}
	else
	{   // Testbeam setup: use the H8 Calibration Service
            sc = locator->service("MuonTBCalibrationSvc", m_pMuonTBCalibration );
            if(sc.isFailure())
            {
                log << MSG::WARNING
                    << "Unable to retrieve the MDT H8 calibration Service"
                    << endreq;
	        log << MSG::WARNING
                    << "Proceed using dummy calibration for MDT"
                    << endreq;
            }
	}  */
    }


    // ================ GET THE POINTER TO THE MUON ROADS  ===================
    MuRoadsSvc* RoadsSvc = 0;
    
    if(m_algoName.find("_Muon") != std::string::npos) {
        sc = locator->service("MuRoadsSvc", RoadsSvc);
    } else {
        sc = locator->service("MuRoadsSvc_900GeV", RoadsSvc);
    }
    
    if(sc.isFailure())
    {
        log << MSG::FATAL
            << "Unable to retrieve the Muon Roads Service"
            << endreq;
        return;
    }
    m_roads = RoadsSvc->muRoads();



    // ============ GET THE POINTER TO THE MUON LOOK UP TABLES  ==============
    MuLUTSvc* LutSvc = 0;
    if( use_mcLUT ) sc = locator->service("MuLUTSvc_MC", LutSvc);
    else            sc = locator->service("MuLUTSvc",    LutSvc);
    if(sc.isFailure())
    {
        log << MSG::FATAL
            << "Unable to retrieve the Muon Look Up Tables Service"
            << endreq;
        return;
    }
    m_lut = LutSvc->muLUT();


    m_load = true;
}


bool
MuServiceInterface::load_timer(TrigTimer* timer, unsigned short int number)
{
    if(number >= NPROC) return false;
    m_timers[number] = timer;
    return true;
}


TrigTimer*
MuServiceInterface::give_timer(unsigned short int number) const
{
    if(number >= NPROC) return 0;
    return m_timers[number];
}



bool
MuServiceInterface::hasEcut(char num) const
{
    std::string::size_type pos = m_algoName.find("MuonEcut");
    if (pos != std::string::npos) {
        std::string cuts = m_algoName.substr(pos+8);
        if (cuts.find(num) != std::string::npos) return true;
    }
    
    return false;
}


MuServiceInterface::MuServiceInterface(const MuServiceInterface& svc)
{
    //m_rpcgeo   = svc.rpcgeo();
    m_rpccab   = svc.rpccab();
    //m_mdtgeo   = svc.mdtgeo();
    m_roads    = svc.roads();
    m_lut      = svc.lut();
    m_beta     = svc.beta();
    m_algoName = svc.name();

#ifndef LVL2_STANDALONE
    //m_pMuonTBCalibration = svc.calibration();
    m_pMdtCalibration    = svc.mdtCalibration();
    m_pMdtIdHelper       = svc.mdtIdHelper();
    m_pTgcLUTSvc         = svc.tgcLutSvc();
    m_pCscLUTSvc         = svc.cscLutSvc();
    m_pMdtCABSvc         = svc.mdtCABSvc();
    m_pRpcLUTSvc         = svc.rpcLUTSvc();
    m_pPtLUTSvc          = svc.ptLUTSvc();
#endif

    m_cosmic_run = svc.isCosmic();
    for(int i=0;i<NPROC;++i) m_timers[i] = svc.timers();

    if( svc.isLoaded()) m_load = true;
    else m_load = false;
}


MuServiceInterface
MuServiceInterface::operator=(const MuServiceInterface& svc)
{
    //m_rpcgeo = svc.rpcgeo();
    m_rpccab = svc.rpccab();
    //m_mdtgeo = svc.mdtgeo();
    m_roads  = svc.roads();
    m_lut    = svc.lut();
    m_beta   = svc.beta();

#ifndef LVL2_STANDALONE
    //m_pMuonTBCalibration = svc.calibration();
    m_pMdtCalibration    = svc.mdtCalibration();
    m_pMdtIdHelper       = svc.mdtIdHelper();
    m_pTgcLUTSvc         = svc.tgcLutSvc();
    m_pCscLUTSvc         = svc.cscLutSvc();
    m_pMdtCABSvc         = svc.mdtCABSvc();
    m_pRpcLUTSvc         = svc.rpcLUTSvc();
    m_pPtLUTSvc          = svc.ptLUTSvc();
#endif

    m_cosmic_run = svc.isCosmic();
    for(int i=0;i<NPROC;++i) m_timers[i] = svc.timers();

    if( svc.isLoaded()) m_load = true;
    else m_load = false;

    return *this;
}


#endif

MuServiceInterface::~MuServiceInterface() {}



#ifdef LVL2_STANDALONE
void
MuServiceInterface::mdtCalibration(int StationName, int StationEta,
                                   int StationPhi,int Multilayer, int layer, int Tube,
                                   int tdcCounts, int adcCounts,int triggerTime, float phi,
                                   float& driftTime, float& driftSpace, float& driftSigma)
                                   const
{
    driftTime  = 0.;
    driftSpace = 0.;
    driftSigma = 0.;
}

#else

void
MuServiceInterface::mdtCalibration(int StationName, int StationEta,
                                   int StationPhi,int Multilayer, int Layer, int Tube,
                                   int& tdcCounts, int adcCounts,int /* triggerTime */,
                                   float X,float Y,float Z,float /* phi */,
                                   float& driftTime, float& driftSpace, float& driftSigma)
                                   const
{
    if(m_beta != 1.)
    {
        float d  = sqrt(X*X + Y*Y + Z*Z)/10.;
	float Dt = d*((1-m_beta)/(m_beta*2.99979e+1));
	float newCounts = Dt*32./25.;
	tdcCounts += (int)newCounts;
    }

    // phi and triggerTime where used only at H8 test beam
    // CID 22990,991: PARAM_SET_BUT_NOT_USED 
    // triggerTime = 0;
    // phi = 0.;

/*    if (m_pMuonTBCalibration)
    {
        Identifier id = m_pMdtIdHelper->channelID(StationName,StationEta,
	                                     StationPhi,Multilayer,Layer,Tube);

	m_pMuonTBCalibration->triggerCounts(triggerTime);
        driftTime  = m_pMuonTBCalibration->getDriftTime(tdcCounts,phi,id);
	driftSpace = m_pMuonTBCalibration->getDistFromTime(driftTime,id);
	driftSigma = m_pMuonTBCalibration->getResolFromTime(driftTime,id);
    }
    else if (m_pMdtCalibration)*/
    if (m_pMdtCalibration)
    {
        TrigTimer* idHelper_time = this->give_timer(10);
	if(idHelper_time) idHelper_time->start();

        Identifier id = m_pMdtIdHelper->channelID(StationName,StationEta,
	                                     StationPhi,Multilayer,Layer,Tube);

	if(idHelper_time) idHelper_time->stop();

	TrigTimer* digit_time = this->give_timer(11);
	if(digit_time) digit_time->start();
	//const MdtDigit digi = MdtDigit(id,tdcCounts);
	//const HepGeom::Point3D<double> point(X,Y,Z);
	const Amg::Vector3D point(X,Y,Z);
	if(digit_time) digit_time->stop();

	driftTime = tdcCounts;

	TrigTimer* calRetrieve_time = this->give_timer(12);
	if(calRetrieve_time) calRetrieve_time->start();
	//MdtCalibHit calHit( &digi, point, 0 );
	MdtCalibHit calHit(id, tdcCounts,adcCounts, point, 0 );
        m_pMdtCalibration->driftRadiusFromTime( calHit, point.mag() );
	if(calRetrieve_time) calRetrieve_time->stop();

	driftSpace = calHit.driftRadius();
	driftSigma = calHit.sigmaDriftRadius();

/*
	ExpandedIdentifier exId;
	IdContext context = m_pMdtIdHelper->channel_context();
	m_pMdtIdHelper->get_expanded_id(id,exId,&context);
        std::string MdtId = exId;

	std::cout << MdtId << "   X=" << X << "  Y=" << Y << "  Z=" << Z
	          << "  phi=" << phi << "  STD=" << point.mag() << std::endl;
	std::cout << "MdtCalibHit::driftTime()=" << calHit.driftTime()
		  << "  MdtCalibHit::propagationTime()="
		  << calHit.propagationTime()
		  << "  MdtCalibHit::timeOfFlight()="
		  << calHit.timeOfFlight() << std::endl;
        std::cout << "MdtCalibHit::driftRadius()=" << calHit.driftRadius()
		  << "  MdtCalibHit::=sigmaDriftRadius()"
		  << calHit.sigmaDriftRadius()<< std::endl;
	std::cout << "driftSpace=" << driftSpace
		  << "  driftSigma" << driftSigma << std::endl << std::endl;
*/

    }
    else
    {
        driftTime  = 0.;
        driftSpace = 0.;
	driftSigma = 0.;
    }
}


bool
MuServiceInterface::etaInner(int address,double etaMiddle, double phiMiddle, 
                     double PT,double& etaInner,double& sigma_etaInner) const 
{
    MuonFeature* feature = 
        new MuonFeature(address,PT,0.,etaMiddle,phiMiddle,0.,0.,0.,1.0);

    double phi;
    double sigma_phi;

    StatusCode sc = (*m_backExtrapolator)->give_eta_phi_at_vertex(feature,
                                    etaInner,sigma_etaInner,phi,sigma_phi,0.);
    
    delete feature;
    
    if(sc.isFailure()) return false;
    else return true;
}

# endif
