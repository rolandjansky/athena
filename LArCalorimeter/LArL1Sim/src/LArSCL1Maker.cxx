/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "LArSCL1Maker.h"
// .......... utilities
//
#include <math.h>
#include <fstream>
#include "AthenaKernel/RNGWrapper.h"

#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandomEngine.h"
#include "GaudiKernel/ServiceHandle.h"

#include "LArSimEvent/LArHitContainer.h"
#include "LArDigitization/LArHitList.h"

#include "CaloDetDescr/CaloDetDescrManager.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArID.h"
#include "CaloIdentifier/CaloID_Exception.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloEvent/CaloCellContainer.h"
//
// ........ Gaudi needed includes
//
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IService.h"

#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "PathResolver/PathResolver.h"

// trigger time
#include "AthenaKernel/ITriggerTime.h"

//


using CLHEP::RandGaussZiggurat;


LArSCL1Maker::LArSCL1Maker(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
  //, p_triggerTimeTool()
  , m_scidtool("CaloSuperCellIDTool")
  , m_scHelper(0)
  , m_OnlSCHelper(0)
  , m_incSvc("IncidentSvc",name)
  , m_sem_mgr(nullptr)
{
//
// ........ default values of private data
//

  m_chronSvc              = 0;
  m_useTriggerTime        = false;
  //m_triggerTimeToolName   = "CosmicTriggerTimeTool";


  m_BeginRunPriority      = 100;

  //m_ttSvc                 = 0;
  m_scHelper            = 0;

  m_NoiseOnOff               = true;
  m_PileUp                   = false;
  m_noEmCalibMode            = false;
  m_noHadCalibMode           = false;
  m_chronoTest               = false;

  //
  // ........ declare the private data as properties
  //

  declareProperty("SubDetectors",m_SubDetectors = "LAr_All");

  declareProperty("NoiseOnOff",m_NoiseOnOff);

  declareProperty("PileUp",m_PileUp);
  declareProperty("UseTriggerTime",m_useTriggerTime);
  //declareProperty("TriggerTimeTool",p_triggerTimeTool);

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
//  if (m_useTriggerTime) { ATH_MSG_INFO("use Trigger Time service " <<  p_triggerTimeTool.name()); }
//  else { ATH_MSG_INFO("no Trigger Time used"); } 

//
// .........retrieve tool computing trigger time if requested
//
  if (m_useTriggerTime && m_PileUp) {
       ATH_MSG_INFO(" In case of pileup, the trigger time subtraction is done in PileUpSvc ");
       ATH_MSG_INFO("  => LArSCL1Maker will not apply Trigger Time ");
       m_useTriggerTime = false;
  }

/*
  if (m_useTriggerTime) {
   if( p_triggerTimeTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Unable to retrieve trigger time tool. Disabling Trigger time");
      m_useTriggerTime=false;
   }
  }
*/

  //
  // ..... need LAr and CaloIdManager to retrieve all needed helpers
  //

  const CaloIdManager* caloMgr;

  CHECK( detStore()->retrieve(caloMgr) );
  CHECK( detStore()->retrieve(m_OnlSCHelper) );
  CHECK( detStore()->retrieve(m_OflHelper,"CaloCell_ID") );

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

  CHECK( m_cablingKeySC.initialize() );
  CHECK( m_shapesKey.initialize() );
  CHECK( m_fracSKey.initialize() );
  CHECK( m_pedestalSCKey.initialize() );
  CHECK( m_noiseSCKey.initialize() );
  CHECK( m_autoCorrNoiseSCKey.initialize() );
  CHECK( m_adc2mevSCKey.initialize() );
  CHECK( m_scidtool.retrieve() );
  CHECK( m_sLArDigitsContainerKey.initialize() );
  CHECK( m_hitMapKey.initialize() );

  
  // Incident Service: 
  ATH_MSG_DEBUG( "Initialization completed successfully" );

  CHECK( m_atRndmGenSvc.retrieve() );
  
  CHECK( detStore()->retrieve (m_sem_mgr, "CaloSuperCellMgr") );

  return StatusCode::SUCCESS;

}


StatusCode LArSCL1Maker::execute(const EventContext& context) const
{


  ATH_MSG_DEBUG("Begining of execution" );

  //
  // ....... fill the LArHitEMap
  //
  SG::ReadHandle<LArHitEMap> hitmap(m_hitMapKey,context);
  const LArHitEMap* hitmapPtr = hitmap.cptr();

  //
  // .....get the trigger time if requested
  //
  //double trigtime=0;
/*
  if (m_useTriggerTime && !p_triggerTimeTool.empty()) {
     trigtime = p_triggerTimeTool->time();
  }
  ATH_MSG_DEBUG("Trigger time used : " << trigtime);
*/

  auto fracS = this->retrieve(context,m_fracSKey);
  auto pedestal = this->retrieve(context,m_pedestalSCKey);
  auto larnoise = this->retrieve(context,m_noiseSCKey);
  auto autoCorrNoise = this->retrieve(context,m_autoCorrNoiseSCKey);
  auto adc2mev = this->retrieve(context,m_adc2mevSCKey);


  SG::WriteHandle<LArDigitContainer> scContainerHandle( m_sLArDigitsContainerKey, context);
  auto scContainer = std::make_unique<LArDigitContainer> (SG::VIEW_ELEMENTS);

  unsigned int nbSC = (unsigned int)m_scHelper->calo_cell_hash_max() ;
  scContainer->reserve(nbSC);

  // .... get SC cabling map
  //
  const LArOnOffIdMapping* cabling = this->retrieve(context, m_cablingKeySC);
  if(!cabling) {
     ATH_MSG_ERROR("Do not have SC cabling map !!!");
     return StatusCode::FAILURE;	  
  }



/* Disable HIT recording for the moment
   CaloCellContainer *scHitContainer = 0;
   if(m_saveHitsContainer.size()>0) {
      scHitContainer = new CaloCellContainer;
      ATH_CHECK( evtStore()->record(scHitContainer, m_saveHitsContainer) );
   }
*/


  //
  // ... initialise vectors for sums of energy in each TT
  //
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
  int it_end = hitmapPtr->GetNbCells();
  scContainer->reserve( nbSC ); //container ordered by hash
  const std::vector<float> base_vec(0);
  scFloatContainerTmp.assign( nbSC, base_vec ); //container ordered by hash
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
    const LArHitList& hitlist = hitmapPtr->GetCell(it);
    const std::vector<std::pair<float,float> >& timeE = hitlist.getData();
    if (timeE.size() > 0 ) {
      Identifier cellId = m_OflHelper->cell_id(IdentifierHash(it));
      Identifier scId = m_scidtool->offlineToSuperCellID(cellId);
      IdentifierHash scHash = m_scHelper->calo_cell_hash(scId) ;
      if ( scHash.value() == 999999 ) continue;
      HWIdentifier hwSC = cabling->createSignalChannelID(scId);
      IdentifierHash scHWHash = m_OnlSCHelper->channel_Hash(hwSC);

      hwid[scHWHash] = hwSC;

      float factor = 1.0;
      if ( (adc2mev->ADC2MEV(hwSC,scGain)).size() < 2 ) {
	continue;
      }
      factor = (adc2mev->ADC2MEV(hwSC,scGain))[1] ;
      factor = 1.0/fracS->FSAMPL(hwSC)/factor;

      ConvertHits2Samples(context,hwSC, scGain, timeE, samples);

      std::vector< float >& vec = scFloatContainerTmp.at(scHWHash);
      if ( !alreadyThere[scHWHash] ){
	alreadyThere[scHWHash]=true;
	for(unsigned int i=0; i< samples.size(); i++){
	  vec.push_back( factor * samples[i] );
	}
      } else {
	for(unsigned int i=0; i< samples.size(); i++){
	  vec[i]+= ( factor * samples[i] );
	}
      }
    }
  } // it end

  it=0;
  it_end=nbSC;
  int cc=0;int dd=0;
  short MAXADC=4096; // 12 bits ADC?
  std::vector<float> noise(m_nSamples);
  double Rndm[32];
  std::vector<float> zeroSamp; 
  zeroSamp.assign(m_nSamples,0); // for empty channels 
  ATHRNG::RNGWrapper* rngWrapper = m_atRndmGenSvc->getEngine(this, m_randomStreamName);
  rngWrapper->setSeedLegacy( m_randomStreamName, context, m_randomSeedOffset, m_useLegacyRandomSeeds );
  CLHEP::HepRandomEngine *rndmEngine = rngWrapper->getEngine(context);
  for( ; it != it_end; ++it){
      std::vector< float > *vecPtr = &zeroSamp; 
      if ( alreadyThere[it] ) vecPtr= &(scFloatContainerTmp.at(it)); 
      else {  cc++;  }
      std::vector<float>& vec = *vecPtr;

      const HWIdentifier id = hwid[it];
       if ( id == 0 ) { dd++; continue; } 

         // reset noise
         noise.assign(m_nSamples,0);

         // noise definition
         if ( m_NoiseOnOff ) {
         float SigmaNoise = (larnoise->noise(id,0));
         int index;
         const std::vector<float>& CorrGen = (autoCorrNoise->autoCorrSqrt(id,0));

         RandGaussZiggurat::shootArray(rndmEngine,static_cast<int>(m_nSamples),Rndm,0.,1.);

         for(int i=0;i<(int)m_nSamples;i++){
         noise[i]=0.;
         for(int j=0;j<=i;j++){
               index = i* m_nSamples + j;
               noise[i] += Rndm[j] * (CorrGen)[index];
         }
         noise[i]=noise[i]*SigmaNoise;
         }
         }

         int ped = pedestal->pedestal(id,0);
         samplesInt.assign( m_nSamples, 0 );
         for(unsigned int i=0; i< vec.size(); i++) {
               samplesInt[i]=rint(vec[i]+ped+noise[i]);
               if ( samplesInt[i] >= MAXADC ) samplesInt[i]=MAXADC-1;
               if ( samplesInt[i] < 0 ) samplesInt[i]=0;
         }
         LArDigit* dig = new LArDigit(id, scGain, samplesInt );
         scContainer->push_back(dig);

  }
  // record final output container
  ATH_CHECK( scContainerHandle.record( std::move(scContainer) ) );

  if(m_chronoTest) {
    m_chronSvc->chronoStop( "LArSCL1Mk hit loop " );
    m_chronSvc->chronoPrint( "LArSCL1Mk hit loop " );
    m_chronSvc->chronoStart( "LArSCL1Mk SC loop " );
  }

  return StatusCode::SUCCESS;
}


StatusCode LArSCL1Maker::finalize()
{

 ATH_MSG_INFO(" LArSCL1Maker finalize completed successfully");
  m_chronSvc->chronoPrint( "LArSCL1Mk hit loop " );
  m_chronSvc->chronoPrint( "LArSCL1Mk TT loop " );
  
  return StatusCode::SUCCESS;

}

void LArSCL1Maker::printConditions(const HWIdentifier& /*hwSC*/){
	// will keep it for future implementation
}

void LArSCL1Maker::ConvertHits2Samples(const EventContext& context, const HWIdentifier & hwSC, CaloGain::CaloGain igain,
                   const std::vector<std::pair<float,float> >& TimeE, std::vector<float>& samples) const
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
   auto shapes = this->retrieve(context,m_shapesKey);
   ILArShape::ShapeRef_t Shape = shapes->Shape(hwSC,igain);
   ILArShape::ShapeRef_t ShapeDer = shapes->ShapeDer(hwSC,igain);

  nsamples = Shape.size();
  nsamples_der = ShapeDer.size();

  if (nsamples==0) {
    std::cout << " No samples for cell = " << hwSC << std::endl;
    return;
  }

  std::vector<std::pair<float,float> >::const_iterator first = TimeE.begin();
  std::vector<std::pair<float,float> >::const_iterator last  = TimeE.end();
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
