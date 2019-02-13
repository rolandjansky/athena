/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTools/CaloMBAverageTool.h" 
#include "StoreGate/StoreGateSvc.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArCabling/LArCablingLegacyService.h"

CaloMBAverageTool::CaloMBAverageTool (const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent) :
    base_class(type, name, parent),
    m_OFCTool("LArOFCTool"),
    m_calo_id(nullptr),
    m_Nminbias(-1),m_deltaBunch(1),m_keyShape("LArShape"), m_keyfSampl("LArfSampl"), m_keyMinBiasAverage("LArMinBiasAverage"),
    m_cabling("LArCablingLegacyService"),
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

  ATH_MSG_INFO( " initialize "  );

  m_ncell = 0;

  ATH_CHECK(  detStore()->retrieve( m_caloIdMgr ) );
  m_calo_id      = m_caloIdMgr->getCaloCell_ID();

// callback for Shape

  ATH_CHECK( detStore()->regFcn(&ICaloMBAverageTool::LoadCalibration,
                                dynamic_cast<ICaloMBAverageTool*>(this),
                                m_dd_shape,m_keyShape,true) );
  ATH_MSG_INFO( "Registered callback for key: " << m_keyShape  );

// callback for fSampl

  ATH_CHECK( detStore()->regFcn(&ICaloMBAverageTool::LoadCalibration,
                                dynamic_cast<ICaloMBAverageTool*>(this),
                                m_dd_fsampl,m_keyfSampl,true) );
  ATH_MSG_INFO( "Registered callback for key: " << m_keyfSampl  );

// callback for MinBiasAverage

  ATH_CHECK( detStore()->regFcn(&ICaloMBAverageTool::LoadCalibration,
                                dynamic_cast<ICaloMBAverageTool*>(this),
                                m_dd_minbiasAverage,m_keyMinBiasAverage,true) );
  ATH_MSG_INFO( "Registered callback for key: " << m_keyMinBiasAverage  );


// get OFC tool and register callback

  ATH_CHECK( m_OFCTool.retrieve() );
  ATH_MSG_DEBUG( " -- LArOFCTool retrieved"  );


  ATH_CHECK( detStore()->regFcn(&ILArOFCTool::LoadCalibration,&(*m_OFCTool),
                                &ICaloMBAverageTool::LoadCalibration,dynamic_cast<ICaloMBAverageTool*>(this),true) );
  ATH_MSG_INFO( "Registered callbacks for LArOFCTool -> CaloMBAverageTool" );


  ATH_CHECK(m_cabling.retrieve());

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------------------

StatusCode CaloMBAverageTool::LoadCalibration(IOVSVC_CALLBACK_ARGS_K(keys))
{
  ATH_MSG_INFO( "Callback invoked for " << keys.size() << " keys "  );

  m_ncell =  m_calo_id->calo_cell_hash_max();
  m_shift.resize(3*m_ncell,0.);

  for (unsigned int icell=0;icell<m_ncell;icell++) {
    IdentifierHash cellHash = icell;
    Identifier id = m_calo_id->cell_id(cellHash);
    HWIdentifier hwid=m_cabling->createSignalChannelID(id);
    if (m_calo_id->is_em(cellHash) || m_calo_id->is_hec(cellHash) || m_calo_id->is_fcal(cellHash)) {

      //  get MinBiasAverage
      float MinBiasAverage = m_dd_minbiasAverage->minBiasAverage(hwid);

      if (MinBiasAverage<0.) MinBiasAverage=0.;

      //  get fSampl
      const float fSampl = m_dd_fsampl->FSAMPL(hwid);
      const float inv_fSampl = 1. / fSampl;

      for (int igain=0;igain<3;igain++) {
 
         //  get Shape
         ILArShape::ShapeRef_t Shape = m_dd_shape->Shape(hwid,igain);

         //  get OFC
         ILArOFCTool::OFCRef_t OFC = m_OFCTool->OFC_a(id, igain, m_Nminbias) ;

         int nsamples = OFC.size();

         int nshapes = Shape.size();
         if (nshapes < nsamples) {
           ATH_MSG_ERROR( " Not enough samples in Shape " << nshapes << "   less than in OFC " << nsamples  );
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

float CaloMBAverageTool::average(const CaloCell* caloCell) const
{
  CaloGain::CaloGain igain = caloCell->gain();
  return this->average(caloCell->caloDDE(),igain);

}

//-------------------------------------------------

float CaloMBAverageTool::average(const CaloDetDescrElement* caloDDE,CaloGain::CaloGain gain) const
{
  const IdentifierHash idCaloHash = caloDDE->calo_hash();
  int igain=static_cast<int>(gain);
  unsigned int index = igain*m_ncell + ((int)(idCaloHash));
  if (index < m_shift.size())
    return m_shift[index];
  else
    return 0.;
}
