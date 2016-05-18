/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTools/CaloMBAverageTool.h" 
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"


CaloMBAverageTool::CaloMBAverageTool (const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent) :
    AthAlgTool(type, name, parent),
    m_OFCTool("LArOFCTool"),
    m_calo_id(nullptr),
    m_Nminbias(-1),m_deltaBunch(1),m_keyShape("LArShape"), m_keyfSampl("LArfSampl"), m_keyMinBiasAverage("LArMinBiasAverage"),
    m_ncell(0)
{ 
  declareInterface<ICaloMBAverageTool>(this);
  declareProperty("NMinBias",m_Nminbias);
  declareProperty("deltaBunch",m_deltaBunch);
  declareProperty("keyShape",m_keyShape);
  declareProperty("keyfSampl",m_keyfSampl);
  declareProperty("keyMinBiasAverge",m_keyMinBiasAverage);
  declareProperty("LArOFCTool",m_OFCTool,"Tool handle for OFC");

}
                                                                                
//-----------------------------------------------------------------

CaloMBAverageTool::~CaloMBAverageTool() {}


//-------------------------------------------------------------------

StatusCode CaloMBAverageTool::initialize() {

  MsgStream log( msgSvc(), name() );

  log << MSG::INFO << " initialize " << endreq;

  m_ncell = 0;

  const IGeoModelSvc *geoModel=0;
  StatusCode sc = service("GeoModelSvc", geoModel);
  if(sc.isFailure())
  {
    log << MSG::ERROR << "Could not locate GeoModelSvc" << endreq;
    return sc;
  }

  // dummy parameters for the callback:
  int dummyInt=0;
  std::list<std::string> dummyList;

  if (geoModel->geoInitialized())
  {
    return geoInit(dummyInt,dummyList);
  }
  else
  {
    sc = detStore()->regFcn(&IGeoModelSvc::geoInit,
                          geoModel,
                          &CaloMBAverageTool::geoInit,this);
    if(sc.isFailure())
    {
      log << MSG::ERROR << "Could not register geoInit callback" << endreq;
      return sc;
    }
  }
  return sc;

}

// ------------------------------------------------------------------------------

StatusCode CaloMBAverageTool::geoInit(IOVSVC_CALLBACK_ARGS) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " geoInit " << endreq;

  StatusCode sc = detStore()->retrieve( m_caloIdMgr );
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve CaloIdMgr in  " << endreq;
   return StatusCode::FAILURE;
  }
  m_calo_id      = m_caloIdMgr->getCaloCell_ID();




// callback for Shape

  sc=detStore()->regFcn(&ICaloMBAverageTool::LoadCalibration,
                         dynamic_cast<ICaloMBAverageTool*>(this),
                         m_dd_shape,m_keyShape,true);
  if(sc.isSuccess()){
       log << MSG::INFO << "Registered callback for key: "
           << m_keyShape << endreq;
  } else {
       log << MSG::ERROR << "Cannot register callback function for key "
           << m_keyShape << endreq;
        return sc;
  }

// callback for fSampl

  sc=detStore()->regFcn(&ICaloMBAverageTool::LoadCalibration,
                         dynamic_cast<ICaloMBAverageTool*>(this),
                         m_dd_fsampl,m_keyfSampl,true);
  if(sc.isSuccess()){
       log << MSG::INFO << "Registered callback for key: "
           << m_keyfSampl << endreq;
  } else {
       log << MSG::ERROR << "Cannot register callback function for key "
           << m_keyfSampl << endreq;
       return sc;
  }

// callback for MinBiasAverage

  sc=detStore()->regFcn(&ICaloMBAverageTool::LoadCalibration,
                         dynamic_cast<ICaloMBAverageTool*>(this),
                         m_dd_minbiasAverage,m_keyMinBiasAverage,true);
  if(sc.isSuccess()){
       log << MSG::INFO << "Registered callback for key: "
           << m_keyMinBiasAverage << endreq;
  } else { 
       log << MSG::ERROR << "Cannot register callback function for key "
           << m_keyMinBiasAverage << endreq;
       return sc;
  }



// get OFC tool and register callback

  sc=m_OFCTool.retrieve();
  if (sc.isFailure())   {
        log << MSG::ERROR << "Unable to retrieve LArOFCTool" << endreq; 
        return sc;
  } else {
        log << MSG::DEBUG << " -- LArOFCTool retrieved" << endreq;
  }


  sc=detStore()->regFcn(&ILArOFCTool::LoadCalibration,&(*m_OFCTool),
                        &ICaloMBAverageTool::LoadCalibration,dynamic_cast<ICaloMBAverageTool*>(this),true);
  if (sc.isSuccess()) {
      log << MSG::INFO 
           << "Registered callbacks for LArOFCTool -> CaloMBAverageTool"
           << endreq;
  } else { 
       log << MSG::ERROR 
           << "Cannot register callbacks for LArOFCTool -> CaloMBAverageTool"
           << endreq;
       return sc;
  }


  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------------------

StatusCode CaloMBAverageTool::LoadCalibration(IOVSVC_CALLBACK_ARGS_K(keys))
{

  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "Callback invoked for " << keys.size() << " keys " << endreq;

  m_ncell =  m_calo_id->calo_cell_hash_max();
  m_shift.resize(3*m_ncell,0.);

  for (unsigned int icell=0;icell<m_ncell;icell++) {
    IdentifierHash cellHash = icell;
    Identifier id = m_calo_id->cell_id(cellHash);

    if (m_calo_id->is_em(cellHash) || m_calo_id->is_hec(cellHash) || m_calo_id->is_fcal(cellHash)) {

      //  get MinBiasAverage
      float MinBiasAverage = m_dd_minbiasAverage->minBiasAverage(id);

      if (MinBiasAverage<0.) MinBiasAverage=0.;

      //  get fSampl
      const float fSampl = m_dd_fsampl->FSAMPL(id);
      const float inv_fSampl = 1. / fSampl;

      for (int igain=0;igain<3;igain++) {
 
         //  get Shape
         ILArShape::ShapeRef_t Shape = m_dd_shape->Shape(id,igain);

         //  get OFC
         ILArOFCTool::OFCRef_t OFC = m_OFCTool->OFC_a(id, igain, m_Nminbias) ;

         int nsamples = OFC.size();

         int nshapes = Shape.size();
         if (nshapes < nsamples) {
             log << MSG::ERROR << " Not enough samples in Shape " << nshapes << "   less than in OFC " << nsamples << endreq;
             return StatusCode::FAILURE;
         }

         float e = MinBiasAverage*inv_fSampl*m_Nminbias;

         float eOFC=0.;
  
         for (int i=0;i<nsamples;i++) {
           float sumShape = 0.;
           for (int j=0;j<nshapes;j++) {
             int k=i-j;
             if (k%m_deltaBunch == 0 ) sumShape += Shape[j];
           }
           float energySample = sumShape*e;
           eOFC += energySample * ( OFC[i] );
         }


         unsigned int index = igain*m_ncell + icell;
         m_shift[index] = eOFC;

      }


    }

  }


  return StatusCode::SUCCESS;
}

//-------------------------------------------------

float CaloMBAverageTool::average(const CaloCell* caloCell)
{
  CaloGain::CaloGain igain = caloCell->gain();
  return this->average(caloCell->caloDDE(),igain);

}

//-------------------------------------------------

float CaloMBAverageTool::average(const CaloDetDescrElement* caloDDE,CaloGain::CaloGain gain)
{
  const IdentifierHash idCaloHash = caloDDE->calo_hash();
  int igain=static_cast<int>(gain);
  unsigned int index = igain*m_ncell + ((int)(idCaloHash));
  if (index < m_shift.size())
    return m_shift[index];
  else
    return 0.;
}
