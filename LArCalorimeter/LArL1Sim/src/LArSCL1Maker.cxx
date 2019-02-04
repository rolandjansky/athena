/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// +======================================================================+
// +                                                                      +
// + Author ........: Denis O. Damazio , Will Buttinger (Cambridge)       +
// + Institut ......: BNL                                                 +
// + Creation date .: 18/11/2013                                          +
// +                                                                      +
// +======================================================================+
// 
// ........ includes
//
#include "LArL1Sim/LArSCL1Maker.h"
// .......... utilities
//
#include <math.h>
#include <fstream>
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArDigitization/LArHitList.h"

#include "CaloDetDescr/CaloDetDescrManager.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArID.h"
#include "CaloIdentifier/CaloID_Exception.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "LArIdentifier/LArIdManager.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArCabling/LArCablingService.h"
#include "LArCabling/LArSuperCellCablingTool.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloEvent/CaloCellContainer.h"
//
// ........ Gaudi needed includes
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IService.h"

#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "PathResolver/PathResolver.h"

#include "LArElecCalib/ILArfSampl.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArNoise.h"
#include "LArElecCalib/ILArAutoCorrNoiseTool.h"
#include "LArElecCalib/ILArADC2MeVTool.h"

// Pile up
#include "PileUpTools/PileUpMergeSvc.h"
// trigger time
#include "AthenaKernel/ITriggerTime.h"

//


using CLHEP::RandGaussZiggurat;


LArSCL1Maker::LArSCL1Maker(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
  , m_atRndmGenSvc("AtRndmGenSvc",name)
  , m_rndmEngineName("LArSCL1Maker")
  , m_rndmEngine(0)
  , p_triggerTimeTool()
  , m_cablingSCSvc("LArSuperCellCablingTool")
  , m_scidtool("CaloSuperCellIDTool")
  , m_scHelper(0)
  , m_OnlSCHelper(0)
  , m_incSvc("IncidentSvc",name)
  , m_hitmap(0)
  , m_shapes(0), m_fracS(0), m_PedestalSC(0), m_NoiseSC(0)
  , m_autoCorrNoiseTool("LArAutoCorrNoiseTool")
  , m_adc2mevTool("LArADC2MeVTool")
  , m_fSamplKey("LARfSamplSC")
  , m_shapesKey("LArShapeSC")
  , m_noiseKey("LArNoiseSC")
  , m_pedestalKey("LArPedestalSC")
  , m_sem_mgr(nullptr)
// + -------------------------------------------------------------------- +
// + Author ........: Denis O. Damazio                                    +
// + Creation date .: 18/11/2013                                          +
// + Subject: SCL1 Maker constructor                                     +
// + -------------------------------------------------------------------- +
{
  m_first=true;
//
// ........ default values of private data
//

  m_chronSvc              = 0;
  m_mergeSvc              = 0;
  m_useTriggerTime        = false;
  //m_triggerTimeToolName   = "CosmicTriggerTimeTool";


  m_BeginRunPriority      = 100;

  //m_ttSvc                 = 0;
  m_scHelper            = 0;

  m_SubDetectors             = "LAr_All";
  m_SCL1ContainerName        = "LArDigitSCL1";
  

  m_NoiseOnOff               = true;
  m_PileUp                   = false;
  m_noEmCalibMode            = false;
  m_noHadCalibMode           = false;
  m_chronoTest               = false;

  //
  // ........ declare the private data as properties
  //

  declareProperty("SubDetectors",m_SubDetectors);
  declareProperty("RndmSvc", m_atRndmGenSvc);
  declareProperty("AutoCorrNoiseTool",m_autoCorrNoiseTool,"Tool handle for electronic noise covariance");
  declareProperty("ADC2MeVTool",m_adc2mevTool,"Tool handle for ADC2MeV Conversion");


  declareProperty("SCL1ContainerName",m_SCL1ContainerName);

  declareProperty("NoiseOnOff",m_NoiseOnOff);

  declareProperty("PileUp",m_PileUp);
  declareProperty("UseTriggerTime",m_useTriggerTime);
  declareProperty("TriggerTimeTool",p_triggerTimeTool);

  declareProperty("FirstSample",m_firstSample=-1);
  declareProperty("NSamples",m_nSamples=7);

  declareProperty("ChronoTest",m_chronoTest);

   declareProperty("TruthHitsContainer",m_saveHitsContainer="","Specify to save hits");
//
return;
}


LArSCL1Maker::~LArSCL1Maker()
{  
  /**  SCL1 Maker destructor                                      */

return;
}


StatusCode LArSCL1Maker::initialize()
{
// +======================================================================+
// +                                                                      +
// + Author ........: Denis O. Damazio                                    +
// + Creation date .: 18/11/2013                                          +
// +                                                                      +
// +======================================================================+
//
// ......... declaration
//
  m_chronSvc = chronoSvc();

  ATH_MSG_INFO("***********************************************");
  ATH_MSG_INFO("* Steering options for LArSCL1Maker algorithm *");
  ATH_MSG_INFO("***********************************************");

  //
  // ......... print the noise flag
  //
  if ( m_NoiseOnOff ) { ATH_MSG_INFO("Electronic noise will be added in each SC for selected sub-detectors."); }
  else { ATH_MSG_INFO("No electronic noise added."); }

//
// ......... print the pile-up flag
//
  if (m_PileUp){ ATH_MSG_INFO("take events from PileUp service"); }
  else { ATH_MSG_INFO("no pile up"); }

//
// ......... print the trigger time flag
//
  if (m_useTriggerTime) { ATH_MSG_INFO("use Trigger Time service " <<  p_triggerTimeTool.name()); }
  else { ATH_MSG_INFO("no Trigger Time used"); }



//
// locate the PileUpMergeSvc and initialize our local ptr
//
  if (m_PileUp) {
    if (!(service("PileUpMergeSvc", m_mergeSvc)).isSuccess() ||	0 == m_mergeSvc) {
      ATH_MSG_ERROR( "Could not find PileUpMergeSvc" );
      return StatusCode::FAILURE;
    } else {
	ATH_MSG_DEBUG("PileUpMergeSvc successfully initialized");
    }
  }     
  
//
// .........retrieve tool computing trigger time if requested
//
  if (m_useTriggerTime && m_PileUp) {
       ATH_MSG_INFO(" In case of pileup, the trigger time subtraction is done in PileUpSvc ");
       ATH_MSG_INFO("  => LArSCL1Maker will not apply Trigger Time ");
       m_useTriggerTime = false;
  }

  if (m_useTriggerTime) {
   if( p_triggerTimeTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Unable to retrieve trigger time tool. Disabling Trigger time");
      m_useTriggerTime=false;
   }
  }

  //
  // ..... need LAr and CaloIdManager to retrieve all needed helpers
  //

  const CaloIdManager* caloMgr;
  const LArIdManager*	 larMgr;

   CHECK( detStore()->retrieve(caloMgr) );
   CHECK( detStore()->retrieve(larMgr) );
   CHECK( detStore()->retrieve(m_OnlSCHelper) );
   

  //
  //..... need of course the LVL1 helper
  //
  m_scHelper = caloMgr->getCaloCell_SuperCell_ID();
  if (!m_scHelper) {
    ATH_MSG_ERROR( "Could not access CaloCell_SuperCell_ID helper");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Successfully accessed CaloCell_SuperCell_ID helper");
  }
  
  // ..... need cabling services, to get channels associated to each SC
  //

  CHECK( m_cablingSCSvc.retrieve() );
  CHECK( m_scidtool.retrieve() );

  
  // Incident Service: 
  CHECK( m_incSvc.retrieve() );
  //start listening to "BeginRun"
  m_incSvc->addListener(this, "BeginRun",  m_BeginRunPriority);

  ATH_MSG_DEBUG( "Initialization completed successfully" );

  CHECK( m_atRndmGenSvc.retrieve() );
  
  m_rndmEngine = m_atRndmGenSvc->GetEngine(m_rndmEngineName);
  if(!m_rndmEngine) {
    ATH_MSG_ERROR( "Could not find RndmEngine : " << m_rndmEngineName );
    return StatusCode::FAILURE ;
  }

  CHECK( m_autoCorrNoiseTool.retrieve() );
  CHECK( m_adc2mevTool.retrieve() );

  CHECK( detStore()->retrieve (m_sem_mgr, "CaloSuperCellMgr") );

  return StatusCode::SUCCESS;

}



void LArSCL1Maker::handle(const Incident& /* inc*/ )
{
  ATH_MSG_DEBUG("LArSCL1Maker handle()");
  if( this->updateConditions().isFailure()) ATH_MSG_ERROR("Failure in updating Conditions");
  return;
}

StatusCode LArSCL1Maker::execute()
{


  ATH_MSG_DEBUG("Begining of execution" );

  //
  // ....... fill the LArHitEMap
  //
  if(m_chronoTest) {
    m_chronSvc->chronoStart( "fill LArHitEMap " );
  }

  int totHit=0;
  CHECK( detStore()->retrieve(m_hitmap,"LArHitEMap") );
  totHit = m_hitmap->GetNbCells();
  ATH_MSG_DEBUG("total number of hits with |E|> 1.e-06 found = " << totHit );


  if(m_chronoTest) {
    m_chronSvc->chronoStop ( "fill LArHitEMap " );
    m_chronSvc->chronoPrint( "fill LArHitEMap " );
  }

  if ( totHit==0) {
    ATH_MSG_WARNING( " No LAr hit in the event ");
    ATH_MSG_WARNING( "cannot process this event");
    return StatusCode::SUCCESS;
  }

  //
  // .....get the trigger time if requested
  //
  double trigtime=0;
  if (m_useTriggerTime && !p_triggerTimeTool.empty()) {
     trigtime = p_triggerTimeTool->time();
  }
  ATH_MSG_DEBUG("Trigger time used : " << trigtime);


  LArDigitContainer *scContainer = new LArDigitContainer();
  if ( scContainer == 0 ) {
    ATH_MSG_ERROR("Could not allocate a new LArSCDigitContainer" );
    return StatusCode::FAILURE;	  
  }


  // ...... register the TTL1 containers into the TES 
  //
  CHECK( evtStore()->record( scContainer ,  m_SCL1ContainerName) );


   CaloCellContainer *scHitContainer = 0;
   if(m_saveHitsContainer.size()>0) {
      scHitContainer = new CaloCellContainer;
      ATH_CHECK( evtStore()->record(scHitContainer, m_saveHitsContainer) );
   }


  //
  // ... initialise vectors for sums of energy in each TT
  //
  unsigned int nbSC = (unsigned int)m_scHelper->calo_cell_hash_max() ;
  ATH_MSG_DEBUG("Max number of LAr Super-Cell= " << nbSC);

  std::vector<std::vector<float> > sumEnergy ;   // inner index = time slot (from 0 to visEvecSize-1)
  std::vector<std::vector<float> > sumTime ;   // inner index = time slot (from 0 to visEvecSize-1)
  sumEnergy.resize(nbSC);
  sumTime.resize(nbSC);
  std::vector<float> scSumE;
  int scSumEvecSize=5;

  scSumE.reserve(scSumEvecSize);
  std::vector< std::vector < float> > scFloatContainerTmp;

  int it = 0;
  int it_end = m_hitmap->GetNbCells();
  scContainer->reserve( nbSC ); //container ordered by hash
  scFloatContainerTmp.assign( nbSC, std::vector<float>(0) ); //container ordered by hash
  std::vector<bool> alreadyThere;
  alreadyThere.resize( nbSC );
  alreadyThere.assign( nbSC, false );

   std::vector<float> truthE;
   truthE.resize( nbSC );
   truthE.assign( nbSC, 0 );

  std::vector<HWIdentifier> hwid;
  hwid.resize( nbSC );
  hwid.assign( nbSC, HWIdentifier(0) );

  for(unsigned int i=0;i<m_OnlSCHelper->channelHashMax();++i) hwid[i]=m_OnlSCHelper->channel_Id(IdentifierHash(i)); 

  //m_nSamples=5;
  std::vector<float> samples;
  samples.resize( m_nSamples );
  std::vector<short> samplesInt;
  samplesInt.resize( m_nSamples );
  CaloGain::CaloGain scGain = CaloGain::LARHIGHGAIN;

  for( ; it!=it_end;++it) {
    LArHitList * hitlist = m_hitmap->GetCell(it);
    if ( hitlist != (LArHitList*) NULL ) {

      const std::vector<std::pair<float,float> >* timeE = hitlist->getData();
      if (timeE->size() > 0 ) {
        Identifier  cellId = hitlist->getIdentifier();

         

          Identifier scId = m_scidtool->offlineToSuperCellID(cellId);
          IdentifierHash scHash = m_scHelper->calo_cell_hash(scId) ;
	  if ( scHash.value() == 999999 ) continue;
	  HWIdentifier hwSC = m_cablingSCSvc->createSignalChannelID(scId);
	  IdentifierHash scHWHash = m_OnlSCHelper->channel_Hash(hwSC);

         if(m_saveHitsContainer.size()>0) {
            for(auto itr = timeE->begin(); itr!= timeE->end(); ++itr) {
               if(fabs(itr->second) > 12.5) continue; //out of time energy deposit
               truthE[scHWHash] += itr->first/m_fracS->FSAMPL(hwSC);
            }
         }

          hwid[scHWHash] = hwSC;

	  float factor = 1.0;
          if ( m_first ) {
		printConditions(hwSC);
	  }
	  factor = (m_adc2mevTool->ADC2MEV(hwSC,(CaloGain::CaloGain)1))[1] ;
	  factor = 1.0/m_fracS->FSAMPL(hwSC)/factor;

          ConvertHits2Samples(hwSC, scGain, timeE, samples);
	  std::vector< float >& vec = scFloatContainerTmp.at(scHWHash);
	  if ( !alreadyThere[scHWHash] ){
		alreadyThere[scHWHash]=true;
	  	for(unsigned int i=0; i< samples.size(); i++)
		   vec.push_back( factor * samples[i] );
	  } else {
	  	for(unsigned int i=0; i< samples.size(); i++)
		   vec[i]+= ( factor * samples[i] );
	  }
	}


    } // if hit list is not empty
  } // it end

  it=0;
  it_end=nbSC;
  int cc=0;int dd=0;
  short MAXADC=4096; // 12 bits ADC?
  std::vector<float> noise(m_nSamples);
  double Rndm[32];
  std::vector<float> zeroSamp; 
  zeroSamp.assign(m_nSamples,0); // for empty channels 
  for( ; it != it_end; ++it){
      std::vector< float > *vecPtr = &zeroSamp; 
      if ( alreadyThere[it] ) vecPtr= &(scFloatContainerTmp.at(it)); 
      else {  cc++;  }
      std::vector<float>& vec = *vecPtr;

      HWIdentifier id = hwid[it];
       if ( id == 0 ) { dd++; continue; } 
		

      //record the truth hit information
      if(m_saveHitsContainer.size()>0) {
            Identifier soft_id = m_cablingSCSvc->cnvToIdentifier(id);
            IdentifierHash idhash = m_sem_mgr->getCaloCell_ID()->calo_cell_hash(soft_id);
            const CaloDetDescrElement* dde = m_sem_mgr->get_element(idhash);
            CaloGain::CaloGain gain = (CaloGain::CaloGain)1;
            
            CaloCell* cell = new CaloCell( dde, (float)truthE[it], 0, (uint16_t)0, (uint16_t)0, gain );
            scHitContainer->push_back(cell);
      }

         // reset noise
         noise.assign(m_nSamples,0);

         // noise definition
         if ( m_NoiseOnOff ) {
         float SigmaNoise = (m_NoiseSC->noise(id,0));
         int index;
         const std::vector<float>* CorrGen = &(m_autoCorrNoiseTool->autoCorrSqrt(id,0,m_nSamples));

         RandGaussZiggurat::shootArray(m_rndmEngine,m_nSamples,Rndm,0.,1.);

         for(int i=0;i<(int)m_nSamples;i++){
         noise[i]=0.;
         for(int j=0;j<=i;j++){
               index = i* m_nSamples + j;
               noise[i] += Rndm[j] * (*CorrGen)[index];
         }
         noise[i]=noise[i]*SigmaNoise;
         }
         }

         int ped = m_PedestalSC->pedestal(id,0);
         samplesInt.assign( m_nSamples, 0 );
         for(unsigned int i=0; i< vec.size(); i++) {
               samplesInt[i]=rint(vec[i]+ped+noise[i]);
               if ( samplesInt[i] >= MAXADC ) samplesInt[i]=MAXADC-1;
               if ( samplesInt[i] < 0 ) samplesInt[i]=0;
         }
         LArDigit* dig = new LArDigit(id, scGain, samplesInt );
         scContainer->push_back(dig);

  }

  if(m_chronoTest) {
    m_chronSvc->chronoStop( "LArSCL1Mk hit loop " );
    m_chronSvc->chronoPrint( "LArSCL1Mk hit loop " );
    m_chronSvc->chronoStart( "LArSCL1Mk SC loop " );
  }

  m_first = false;
  return StatusCode::SUCCESS;
}


StatusCode LArSCL1Maker::finalize()
{

 ATH_MSG_INFO(" LArSCL1Maker finalize completed successfully");
  m_chronSvc->chronoPrint( "LArSCL1Mk hit loop " );
  m_chronSvc->chronoPrint( "LArSCL1Mk TT loop " );
  
  return StatusCode::SUCCESS;

}



StatusCode LArSCL1Maker::updateConditions(){

  ATH_MSG_DEBUG( "Updating conditions" );

  CHECK( detStore()->retrieve(m_fracS,m_fSamplKey) );
  CHECK( detStore()->retrieve(m_shapes,m_shapesKey) );
  CHECK( detStore()->retrieve(m_NoiseSC,m_noiseKey) );
  CHECK( detStore()->retrieve(m_PedestalSC,m_pedestalKey) );

  return StatusCode::SUCCESS;

}

void LArSCL1Maker::printConditions(const HWIdentifier& hwSC){

	  ATH_MSG_VERBOSE("HW Identifier : " << hwSC.get_identifier32().get_compact() );
          if ( m_shapes ) {
                ILArShape::ShapeRef_t shape = m_shapes->Shape(hwSC,0);
                ILArShape::ShapeRef_t shapeder = m_shapes->ShapeDer(hwSC,0);
                ATH_MSG_VERBOSE( "shape0.size() : " << shape.size() );
                for(unsigned int i=0;i<shape.size();i++)
                   ATH_MSG_VERBOSE("shape[" << i << "]=" << shape[i] << " - " << shapeder[i] << "; ");
          }
          if ( m_fracS  ) {
                ATH_MSG_VERBOSE("fSample : " << m_fracS->FSAMPL(hwSC) );
          }
          if ( m_adc2mevTool ) {
                ATH_MSG_VERBOSE("Ramp (gain0) : " << (m_adc2mevTool->ADC2MEV(hwSC,(CaloGain::CaloGain)0))[1] );
                ATH_MSG_VERBOSE("Ramp (gain1) : " << (m_adc2mevTool->ADC2MEV(hwSC,(CaloGain::CaloGain)1))[1] );
                ATH_MSG_VERBOSE("Ramp (gain2) : " << (m_adc2mevTool->ADC2MEV(hwSC,(CaloGain::CaloGain)2))[1] );
          }
          if ( m_PedestalSC ) {
                ATH_MSG_VERBOSE("Pedestal : " << m_PedestalSC->pedestal(hwSC,0) );
          }
          if ( m_NoiseSC ) {
                ATH_MSG_VERBOSE("Noise : " << m_NoiseSC->noise(hwSC,0) );
          }
          if ( m_autoCorrNoiseTool ) {
		const std::vector<float>* CorrGen = &(m_autoCorrNoiseTool->autoCorrSqrt(hwSC,0,m_nSamples));
                std::stringstream ss; ss << "Auto : ";
		for(size_t ii=0;ii<m_nSamples;++ii) ss << CorrGen->at(ii) << " ";
                ATH_MSG_VERBOSE(ss.str()) ;

	  }
}

void LArSCL1Maker::ConvertHits2Samples(const HWIdentifier & hwSC, CaloGain::CaloGain igain,
                   const std::vector<std::pair<float,float> >  *TimeE, std::vector<float>& samples)
{
// Converts  hits of a particular LAr cell into energy samples
// declarations

   int nsamples ;
   int nsamples_der ;
   int i ;
   int j ;
   float energy ;
   float time ;

// ........ retrieve data (1/2) ................................
//
   ILArShape::ShapeRef_t Shape = m_shapes->Shape(hwSC,igain);
   ILArShape::ShapeRef_t ShapeDer = m_shapes->ShapeDer(hwSC,igain);

  nsamples = Shape.size();
  nsamples_der = ShapeDer.size();

  if (nsamples==0) {
    std::cout << " No samples for cell = " << hwSC << std::endl;
    return;
  }

  std::vector<std::pair<float,float> >::const_iterator first = TimeE->begin();
  std::vector<std::pair<float,float> >::const_iterator last  = TimeE->end();
  samples.clear();
  samples.assign(m_nSamples,0);
  //m_firstSample=0;

  while (first != last)
  {
   energy = (*first).first;
   time   = (*first).second;

 // Atlas like mode where we use 25ns binned pulse shape and derivative to deal with time offsets
// shift between reference shape and this time
      int ishift=(int)(rint(time*(1./25)));
      double dtime=time-25.*((double)(ishift));

      for (i=0;i<(int)m_nSamples;i++)
      {
       j = i - ishift + m_firstSample;
       if (j >=0 && j < nsamples ) {
         if (j<nsamples_der && std::fabs(ShapeDer[j])<10. )
              samples[i] += (Shape[j]- ShapeDer[j]*dtime)*energy ;
         else samples[i] += Shape[j]*energy ;
       }
      }

   ++first;
  }         // loop over hits

   return;

}
