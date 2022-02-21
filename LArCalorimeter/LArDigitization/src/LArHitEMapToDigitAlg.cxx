
#include "LArHitEMapToDigitAlg.h"
#include "AthenaKernel/ITriggerTime.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArID.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "CaloIdentifier/LArNeighbours.h"
#include "Identifier/IdentifierHash.h"
#include "LArIdentifier/LArOnlineID.h"

#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"
#include <CLHEP/Random/Randomize.h>

using CLHEP::RandFlat;
using CLHEP::RandGaussZiggurat;

LArHitEMapToDigitAlg::LArHitEMapToDigitAlg(const std::string& name, ISvcLocator* pSvcLocator) :
   AthReentrantAlgorithm(name,pSvcLocator)
{
  // default properties
  m_LowGainThresh[EM]    = 3900;//ADC counts in MediumGain
  m_HighGainThresh[EM]   = 1300;//ADC counts in MediumGain
  m_LowGainThresh[HEC]   = 2500;//ADC counts in MediumGain
  m_HighGainThresh[HEC]  = 0;//-> high-gain never used for HEC
  m_LowGainThresh[FCAL]  = 2000.;//ADC counts in Medium Gain
  m_HighGainThresh[FCAL] = 1100.;//ADCcounts in MediumGain
  m_LowGainThresh[EMIW]    = 3900;//ADC counts in MediumGain
  m_HighGainThresh[EMIW]   = 1300;//ADC counts in MediumGain
  // given the enum, it seems complicated to do it with modern configuration
  declareProperty("LowGainThreshEM",m_LowGainThresh[EM],"Medium/Low gain transition in EM");
  declareProperty("HighGainThreshEM",m_HighGainThresh[EM],"Medium/High gain transition in EM");
  declareProperty("LowGainThreshHEC",m_LowGainThresh[HEC],"Medium/Low gain transition in HEC");
  declareProperty("HighGainThreshHEC",m_HighGainThresh[HEC],"Medium/High gain transition in HEC");
  declareProperty("LowGainThreshFCAL",m_LowGainThresh[FCAL],"Medium/Low gain transition in FCAL");
  declareProperty("HighGainThreshFCAL",m_HighGainThresh[FCAL],"Medium/High gain transition in FCAL");
  declareProperty("LowGainThreshEMECIW",m_LowGainThresh[EMIW],"Medium/Low gain transition in EMEC IW");
  declareProperty("HighGainThreshEMECIW",m_HighGainThresh[EMIW],"Medium/High gain transition in EMEC IW");
}

StatusCode LArHitEMapToDigitAlg::initialize()
{

  ATH_CHECK(m_noiseKey.initialize((!m_RndmEvtOverlay || m_isMcOverlay) && !m_pedestalNoise && m_NoiseOnOff));
 
  ATH_CHECK(m_shapeKey.initialize());
  ATH_CHECK(m_fSamplKey.initialize());
  ATH_CHECK(m_OFCKey.initialize());
  ATH_CHECK(m_pedestalKey.initialize());
  ATH_CHECK(m_autoCorrNoiseKey.initialize());
  ATH_CHECK(m_bcContKey.initialize());
  ATH_CHECK(m_badFebKey.initialize());
  ATH_CHECK(m_adc2mevKey.initialize());
  ATH_CHECK(m_caloMgrKey.initialize());
 
  ATH_CHECK(m_cablingKey.initialize());

  // helpers
  //retrieve ID helpers
  ATH_CHECK(detStore()->retrieve(m_calocell_id,"CaloCell_ID"));


  const CaloIdManager* caloIdMgr = nullptr;
  StatusCode sc = detStore()->retrieve(caloIdMgr);
  if (sc.isFailure()) {
    ATH_MSG_ERROR(" Unable to retrieve CaloIdManager from DetectoreStore");
    return StatusCode::FAILURE;
  }
  m_larem_id   = caloIdMgr->getEM_ID();
  m_larhec_id  = caloIdMgr->getHEC_ID();
  m_larfcal_id = caloIdMgr->getFCAL_ID();

  sc = detStore()->retrieve(m_laronline_id);
  if (sc.isFailure()) {
    ATH_MSG_ERROR(" Unable to retrieve LArOnlineId from DetectoreStore");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_bcMask.buildBitMask(m_problemsToMask,msg()));

  // Services
  ATH_CHECK(m_rndmGenSvc.retrieve());
 
  ATH_CHECK(m_hitMapKey.initialize());
  ATH_CHECK(m_hitMapKey_DigiHSTruth.initialize(m_doDigiTruth));
 
  ATH_CHECK(m_DigitContainerName.initialize());
  ATH_CHECK(m_DigitContainerName_DigiHSTruth.initialize(m_doDigiTruth));
  return StatusCode::SUCCESS;
 
}

StatusCode LArHitEMapToDigitAlg::execute(const EventContext& context) const {

   // load many conditions
   const LArBadFebCont* badFebs = pointerFromKey<LArBadFebCont>(context,m_badFebKey);
   SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey, context};
   const LArOnOffIdMapping* cabling=*cablingHdl;
   if(!cabling) {
     ATH_MSG_ERROR("Failed to retrieve LAr Cabling map with key " << m_cablingKey.key() );
     return StatusCode::FAILURE;
   }

   // Inputs
   SG::ReadHandle<LArHitEMap> hitmap(m_hitMapKey,context);
   const LArHitEMap* hitmapPtr = hitmap.cptr();
   const LArHitEMap* hitmapPtr_DigiHSTruth = nullptr;
   if ( m_doDigiTruth ) {
     SG::ReadHandle<LArHitEMap> hitmap_DigitHSTruth(m_hitMapKey_DigiHSTruth,context);
     hitmapPtr_DigiHSTruth = hitmap_DigitHSTruth.cptr();
   }

   // Prepare Output
   auto DigitContainer = std::make_unique<LArDigitContainer>();
   auto DigitContainer_DigiHSTruth = std::make_unique<LArDigitContainer>();
   int it,it_end;
   it =  0;
   it_end = hitmapPtr->GetNbCells();
   DigitContainer->reserve(it_end);
   DigitContainer_DigiHSTruth->reserve(it_end);

   const std::vector<std::pair<float,float> >* TimeE;
   const std::vector<std::pair<float,float> >* TimeE_DigiHSTruth = nullptr;

   ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this, m_randomStreamName);
   CLHEP::HepRandomEngine * engine = rngWrapper->getEngine(context);
   ATHRNG::RNGWrapper::SeedingOptionType seedingmode=m_useLegacyRandomSeeds ? ATHRNG::RNGWrapper::MC16Seeding : ATHRNG::RNGWrapper::SeedingDefault;
  rngWrapper->setSeedLegacy( m_randomStreamName, context, m_randomSeedOffset, seedingmode );

   for( ; it!=it_end;++it) // now loop on cells
   {
      const LArHitList& hitlist = hitmapPtr->GetCell(it);
      
      if (!m_Windows || hitlist.inWindows()) {
    TimeE = &(hitlist.getData());
    if(m_doDigiTruth) {
      auto& hitlist_DigiHSTruth=hitmapPtr_DigiHSTruth->GetCell(it);
      TimeE_DigiHSTruth = &(hitlist_DigiHSTruth.getData());
    }

    if (TimeE->size() > 0 || m_NoiseOnOff || m_RndmEvtOverlay) {
      const Identifier cellID=m_calocell_id->cell_id(IdentifierHash(it));
      HWIdentifier ch_id = cabling->createSignalChannelIDFromHash(IdentifierHash(it));
      HWIdentifier febId = m_laronline_id->feb_Id(ch_id);
      bool missing=!(badFebs->status(febId).good());
      if (!missing) {
        const LArDigit * digit = 0 ;
        if(m_RndmEvtOverlay) digit = hitmapPtr->GetDigit(it);
        // MakeDigit called if in no overlay mode or
        // if in overlay mode and random digit exists
        if( (!m_RndmEvtOverlay) || (m_RndmEvtOverlay && digit) ) {
	      LArDigit* Digit = nullptr;
	      LArDigit* Digit_DigiHSTruth = nullptr;
              ATH_CHECK( MakeDigit(context,cellID, ch_id, Digit, Digit_DigiHSTruth, TimeE, digit, engine, TimeE_DigiHSTruth));
	   DigitContainer->push_back(Digit);
	   if ( m_doDigiTruth && Digit_DigiHSTruth ) DigitContainer_DigiHSTruth->push_back(Digit_DigiHSTruth);
        }
      }
    }
      }     // check window
   }        // end of loop over the cells


//  ATH_MSG_DEBUG(" total number of hits found= " << m_nhit_tot);
  ATH_MSG_DEBUG(" number of created digits  = " << DigitContainer->size());

  SG::WriteHandle<LArDigitContainer> DigitContainerHandle( m_DigitContainerName, context);
  ATH_CHECK(DigitContainerHandle.record( std::move(DigitContainer) ) );
  if ( m_doDigiTruth ){
  SG::WriteHandle<LArDigitContainer> DigitContainer_DigiHSTruthHandle( m_DigitContainerName_DigiHSTruth, context);
  ATH_CHECK(DigitContainer_DigiHSTruthHandle.record( std::move(DigitContainer_DigiHSTruth) ) );
  }


  return StatusCode::SUCCESS;
}

StatusCode LArHitEMapToDigitAlg::MakeDigit(const EventContext& ctx, const Identifier & cellId,
                                    const HWIdentifier & ch_id,
				    LArDigit*& Digit, LArDigit*& Digit_DigiHSTruth,
                                    const std::vector<std::pair<float,float> >* TimeE,
                                    const LArDigit * rndmEvtDigit, CLHEP::HepRandomEngine * engine,
                    const std::vector<std::pair<float,float> >* TimeE_DigiHSTruth) const

{
  bool createDigit_DigiHSTruth = true;

  int sampleGainChoice{2};

  int i;
  short Adc;
  short Adc_DigiHSTruth;

  CaloGain::CaloGain igain;
  std::vector<short> AdcSample(m_NSamples);
  std::vector<short> AdcSample_DigiHSTruth(m_NSamples);

  float MeV2GeV=0.001;   // to convert hit from MeV to GeV before apply GeV->ADC

  float SF=1.;
  float SigmaNoise;
  std::vector<float> rndm_energy_samples(m_NSamples) ;


  SG::ReadCondHandle<LArADC2MeV> adc2mevHdl(m_adc2mevKey, ctx);
  const LArADC2MeV* adc2MeVs=*adc2mevHdl;

  SG::ReadCondHandle<ILArfSampl> fSamplHdl(m_fSamplKey, ctx);
  const ILArfSampl* fSampl=*fSamplHdl;

  SG::ReadCondHandle<ILArPedestal> pedHdl(m_pedestalKey, ctx);
  const ILArPedestal* pedestal=*pedHdl;

  const ILArNoise* noise=nullptr;  
  if ( (!m_RndmEvtOverlay || m_isMcOverlay)  && !m_pedestalNoise && m_NoiseOnOff ){
    SG::ReadCondHandle<ILArNoise> noiseHdl(m_noiseKey, ctx);
    noise=*noiseHdl;
  }

  const LArAutoCorrNoise* autoCorrNoise=nullptr;
  if ( !m_RndmEvtOverlay  &&  m_NoiseOnOff) {
    SG::ReadCondHandle<LArAutoCorrNoise>  autoCorrNoiseHdl(m_autoCorrNoiseKey, ctx);
    autoCorrNoise=*autoCorrNoiseHdl;
  }

  /** Retrieve BadChannels */
  SG::ReadCondHandle<LArBadChannelCont> bch{m_bcContKey,ctx};
  const LArBadChannelCont* bcCont{*bch}; 

  int iCalo=0;
  if(m_larem_id->is_lar_em(cellId)) {
     if (m_larem_id->is_em_endcap_inner(cellId)) iCalo=EMIW;
     else iCalo=EM;
  }
  if(m_larem_id->is_lar_hec(cellId))  iCalo=HEC;
  if(m_larem_id->is_lar_fcal(cellId)) iCalo=FCAL;

  CaloGain::CaloGain  initialGain = CaloGain::LARHIGHGAIN;
  if (iCalo==HEC) initialGain = CaloGain::LARMEDIUMGAIN;

  CaloGain::CaloGain rndmGain = CaloGain::LARHIGHGAIN;

// ........ retrieve data (1/2) ................................
//
  SF=fSampl->FSAMPL(ch_id);

//
// ....... dump info ................................
//


#ifndef NDEBUG
  ATH_MSG_DEBUG("    Cellid " << m_larem_id->show_to_string(cellId));
  ATH_MSG_DEBUG("    SF: " << SF);
#endif

  std::vector<double> Samples;
  std::vector<double> Samples_DigiHSTruth;
  std::vector<double> Noise;
  Samples.resize(m_NSamples,0);
  if(m_doDigiTruth) Samples_DigiHSTruth.resize(m_NSamples,0);
  Noise.resize(m_NSamples,0);

//
// ....... make the five samples
//

#ifndef NDEBUG
  ATH_MSG_DEBUG(" number of hit for this cell " << TimeE->size());
#endif

//
// convert Hits into energy samples and add result to Samples assuming LARHIGHGAIN for pulse shape
//
  bool isDead = m_bcMask.cellShouldBeMasked(bcCont,ch_id);


  if (!isDead) {
    if( this->ConvertHits2Samples(ctx, cellId,ch_id,initialGain,TimeE, Samples).isFailure() ) return StatusCode::SUCCESS;
    if(m_doDigiTruth){
      if( this->ConvertHits2Samples(ctx, cellId,ch_id,initialGain,TimeE_DigiHSTruth, Samples_DigiHSTruth).isFailure() ) return StatusCode::SUCCESS;
    }
  }

//
// .... add random event digit if needed
//
 float energy2adc ;
 float rAdc ;
 if(m_RndmEvtOverlay && rndmEvtDigit ) // no overlay if missing random digit
 {
  rndmGain= rndmEvtDigit->gain();
  auto polynom_adc2mev =adc2MeVs->ADC2MEV(cellId,rndmEvtDigit->gain());
  if (polynom_adc2mev.size() > 1) {
     float adc2energy = SF * polynom_adc2mev[1];
     const std::vector<short> & rndm_digit_samples = rndmEvtDigit->samples() ;
     float Pedestal = pedestal->pedestal(ch_id,rndmEvtDigit->gain());
     if (Pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
       ATH_MSG_WARNING("  Pedestal not found in database for this channel offID " << cellId << " Use sample 0 for random");
       Pedestal = rndm_digit_samples[0];
     }
     ATH_MSG_DEBUG(" Params for inverting LAr Digitization: pedestal " << Pedestal << "  adc2energy " << adc2energy);

// in case Medium or low gain, take into account ramp intercept in ADC->"energy" computation
//   this requires to take into account the sum of the optimal filter coefficients, as they don't compute with ADC shift
     float adc0=0.;
     if (!m_isMcOverlay && rndmEvtDigit->gain()>0) {
        SG::ReadCondHandle<ILArOFC> larOFC(m_OFCKey, ctx);
        if (larOFC.cptr() != nullptr) {
          ILArOFC::OFCRef_t ofc_a = larOFC->OFC_a(ch_id,rndmEvtDigit->gain(),0);
          float sumOfc=0.;
          if (ofc_a.size()>0) {
            for (unsigned int j=0;j<ofc_a.size();j++) sumOfc += ofc_a.at(j);
          }
          if (sumOfc>0) adc0 =  polynom_adc2mev[0] * SF /sumOfc;
        }
     }

     int nmax=m_NSamples;
     if ((int)(rndm_digit_samples.size()) < m_NSamples) {
         ATH_MSG_WARNING("Less digit Samples than requested in digitization for cell " << ch_id.get_compact() << " Digit has " << rndm_digit_samples.size() << " samples.  Digitization request " << m_NSamples); 
         nmax=rndm_digit_samples.size();
     }
     for(i=0 ; i<nmax ; i++)
     {
       rAdc = (rndm_digit_samples[i] - Pedestal ) * adc2energy + adc0;
       rndm_energy_samples[i] = rAdc ;
       Samples[i] += rAdc ;
     }
  }
  else {
    ATH_MSG_WARNING(" No ramp found for this random cell " << m_larem_id->show_to_string(cellId) << " for gain " << rndmEvtDigit->gain());
  }
 }
//...................................................................

//
//........ choice of the gain
//
//
// fix the shift +1 if HEC  and nSamples 4 and firstSample 0
  int ihecshift=0;
  if(iCalo == HEC && m_NSamples.value() == 4 && m_firstSample.value() == 0) ihecshift=1;
  float samp2=Samples[sampleGainChoice-ihecshift]*MeV2GeV;
  if ( samp2 <= m_EnergyThresh ) return(StatusCode::SUCCESS);

  if(m_doDigiTruth){
    float Samp2_DigiHSTruth=Samples_DigiHSTruth[sampleGainChoice-ihecshift]*MeV2GeV;
    if ( Samp2_DigiHSTruth <= m_EnergyThresh ) createDigit_DigiHSTruth = false;
  }
    //We choose the gain in applying thresholds on the 3rd Sample (index "2")
    //converted in ADC counts in MediumGain (index "1" of (ADC2MEV)).
    //Indeed, thresholds in ADC counts are defined with respect to the MediumGain.
    //
    //              1300              3900
    // ---------------|----------------|--------------> ADC counts in MediumGain
    //    HighGain  <---  MediumGain  --->  LowGain

  float pseudoADC3;
  float Pedestal = pedestal->pedestal(ch_id,CaloGain::LARMEDIUMGAIN);
  if (Pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
   ATH_MSG_DEBUG(" Pedestal not found for medium gain ,cellID " << cellId <<  " assume 1000 ");
   Pedestal=1000.;
  }
  auto polynom_adc2mev = adc2MeVs->ADC2MEV(cellId,CaloGain::LARMEDIUMGAIN);
  if ( polynom_adc2mev.size() < 2) {
    ATH_MSG_WARNING(" No medium gain ramp found for cell " << m_larem_id->show_to_string(cellId) << " no digit produced...");
    return StatusCode::SUCCESS;
  }
  pseudoADC3 = Samples[sampleGainChoice-ihecshift]/(polynom_adc2mev[1])/SF + Pedestal ;

  //
  // ......... try a gain
  //
  if ( pseudoADC3 <= m_HighGainThresh[iCalo])
    igain = CaloGain::LARHIGHGAIN;
   else if ( pseudoADC3 <= m_LowGainThresh[iCalo])
    igain = CaloGain::LARMEDIUMGAIN;
  else
    igain = CaloGain::LARLOWGAIN;

 // check that select gain is never lower than random gain in case of overlay
  if (rndmGain==CaloGain::LARMEDIUMGAIN && igain==CaloGain::LARHIGHGAIN) igain=CaloGain::LARMEDIUMGAIN;
  if (rndmGain==CaloGain::LARLOWGAIN ) igain=CaloGain::LARLOWGAIN;

//
// recompute Samples if igain != HIGHGAIN
//
   if (igain != initialGain ){

     for (i=0;i<m_NSamples;i++) {
       if(m_doDigiTruth) Samples_DigiHSTruth[i] = 0.;
       if (m_RndmEvtOverlay) Samples[i]= rndm_energy_samples[i] ;
       else Samples[i] = 0.;
     }

     if (!isDead) {
       if( this->ConvertHits2Samples(ctx, cellId,ch_id,igain,TimeE, Samples) == StatusCode::FAILURE ) return StatusCode::SUCCESS;
       if(m_doDigiTruth){
         if( this->ConvertHits2Samples(ctx, cellId,ch_id,igain,TimeE_DigiHSTruth, Samples_DigiHSTruth) == StatusCode::FAILURE ) return StatusCode::SUCCESS;
       }
     }

   }

//
// ........ add the noise ................................
//

  double Rndm[32]{};
  int BvsEC=0;
  if(iCalo==EM || iCalo==EMIW) BvsEC=abs(m_larem_id->barrel_ec(cellId));

  bool addedNoise=false;
  if(    m_NoiseOnOff
      && (    (BvsEC==1 && m_NoiseInEMB)
           || (BvsEC>1  && m_NoiseInEMEC)
           || (iCalo==HEC && m_NoiseInHEC)
           || (iCalo==FCAL && m_NoiseInFCAL)
         )
    )
    //add the noise only in the wanted sub-detectors
  {
     if ( !m_RndmEvtOverlay ) {
        if (!m_pedestalNoise) {
          SigmaNoise =noise->noise(ch_id,igain );
        } else {
          float noise = pedestal->pedestalRMS(ch_id,igain);
          if (noise >= (1.0+LArElecCalib::ERRORCODE) ) SigmaNoise = noise;
          else SigmaNoise=0.;
        }
        // Sqrt of noise covariance matrix
    const std::vector<float>& CorGen=autoCorrNoise->autoCorrSqrt(cellId,igain);

        RandGaussZiggurat::shootArray(engine,m_NSamples,Rndm,0.,1.);

        int index;
        for (int i=0;i<m_NSamples;i++) {
           Noise[i]=0.;
           for(int j=0;j<=i;j++) {
             index = i*m_NSamples + j;
             Noise[i] += Rndm[j] * CorGen[index];
           }
           Noise[i]=Noise[i]*SigmaNoise;
        }
        addedNoise=true;
     } else  {
        // overlay case a priori don't add any noise
        for (int i=0;i<m_NSamples;i++) Noise[i]=0.;
        // if gain from zerobias events is < gain from mixed events => add extra noise to account for gain vs noise dependance
        //   done in a simple way without taking into account the time correlation of this extra noise properly
        if (rndmEvtDigit) {
            // if gain of cell is different from ZB event gain
            if (igain > rndmEvtDigit->gain() ) {
               double SigmaNoiseZB=0.;    // noise in ZB event for gain of ZB event
               double SigmaNoise=0.;      // noise expected for new gain value
               double SigmaExtraNoise=0.;  // quadratic difference of noise values
               if (!m_pedestalNoise) {
                  SigmaNoiseZB =  noise->noise(ch_id,rndmEvtDigit->gain());
                  SigmaNoise   =  noise->noise(ch_id,igain );
               }    
               else {
                  float noise = pedestal->pedestalRMS(ch_id,rndmEvtDigit->gain() );
                  if (noise >= (1.0+LArElecCalib::ERRORCODE) ) SigmaNoiseZB = noise;
                  else SigmaNoiseZB=0.;
                  noise = pedestal->pedestalRMS(ch_id,igain);
                  if (noise >= (1.0+LArElecCalib::ERRORCODE) ) SigmaNoise=noise;
                  else SigmaNoise=0.;
               }
               // Convert SigmaNoiseZB in noise in ADC counts for igain conversion
               auto polynom_adc2mevZB = adc2MeVs->ADC2MEV(cellId,rndmEvtDigit->gain() );
               auto polynom_adc2mev = adc2MeVs->ADC2MEV(cellId,igain);
               if ( polynom_adc2mevZB.size()>1 &&  polynom_adc2mev.size()>1) {
                  if (polynom_adc2mev[1] >0.) {
                   SigmaNoiseZB = SigmaNoiseZB * (polynom_adc2mevZB[1])/(polynom_adc2mev[1]);
                   if (SigmaNoise > SigmaNoiseZB) SigmaExtraNoise = sqrt(SigmaNoise*SigmaNoise - SigmaNoiseZB*SigmaNoiseZB);
                  }
               }    // check that AC2MeV factors are there
               RandGaussZiggurat::shootArray(engine,m_NSamples,Rndm,0.,1.);   // generate noise
               for (int i=0;i<m_NSamples;i++) Noise[i] = SigmaExtraNoise * Rndm[i];
               addedNoise=true;
            }  // different gains
        }      // rndm Digit is there
     }         // rndm Overlay test
  }            // add noise ?
//
// ......... convert into adc counts  ................................
//
  Pedestal = pedestal->pedestal(ch_id,igain);
  if (Pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
     ATH_MSG_WARNING(" pedestal not found for cellId " << cellId << " assume 1000" );
     Pedestal=1000.;
  }
  polynom_adc2mev = adc2MeVs->ADC2MEV(cellId,igain);
  if (polynom_adc2mev.size() < 2) {
    ATH_MSG_WARNING(" No ramp found for requested gain " << igain << " for cell " << m_larem_id->show_to_string(cellId) << " no digit made...");
    return StatusCode::SUCCESS;
  }

  energy2adc=1./(polynom_adc2mev[1])/SF;

// in case Medium or low gain, take into account ramp intercept in energy->ADC computation
//   this requires to take into account the sum of the optimal filter coefficients, as they don't compute with ADC shift
  if(!m_isMcOverlay && m_RndmEvtOverlay  && igain>0)
  {
    SG::ReadCondHandle<ILArOFC> larOFC(m_OFCKey, ctx);
    if (larOFC.cptr() != nullptr) {
      float sumOfc=0.;
      ILArOFC::OFCRef_t ofc_a = larOFC->OFC_a(ch_id,igain,0);
      if (ofc_a.size()>0) {
        for (unsigned int j=0;j<ofc_a.size();j++) sumOfc+= ofc_a.at(j);
      }
      if ((polynom_adc2mev[1])>0 && sumOfc>0) Pedestal = Pedestal - (polynom_adc2mev[0])/(polynom_adc2mev[1])/sumOfc;
      ATH_MSG_DEBUG("  Params for final LAr Digitization  gain: " << igain << "    pedestal: " << Pedestal <<  "   energy2adc: " << energy2adc);
    }
  }
  for(i=0;i<m_NSamples;i++)
  {
    double xAdc;
    double xAdc_DigiHSTruth = 0;

    if ( addedNoise ){
      xAdc =  Samples[i]*energy2adc + Noise[i] + Pedestal + 0.5;
      if(m_doDigiTruth) {
        xAdc_DigiHSTruth =  Samples_DigiHSTruth[i]*energy2adc + Noise[i] + Pedestal + 0.5;
      }
    }

    else {
      if (m_roundingNoNoise) {
        float flatRndm = RandFlat::shoot(engine);
        xAdc =  Samples[i]*energy2adc + Pedestal + flatRndm;
        if(m_doDigiTruth) {
          xAdc_DigiHSTruth =  Samples_DigiHSTruth[i]*energy2adc + Pedestal + flatRndm;
        }

      }
      else{
         xAdc =  Samples[i]*energy2adc + Pedestal + 0.5;
         if(m_doDigiTruth) {
           xAdc_DigiHSTruth =  Samples_DigiHSTruth[i]*energy2adc + Pedestal + 0.5;
         }
      }

    }

//
// ........ truncate at maximum value + 1
//          add possibility to saturate at 0 for negative signals
//
    if (xAdc <0)  Adc=0;
    else if (xAdc >= MAXADC) Adc=MAXADC;
    else Adc = (short) xAdc;

    AdcSample[i]=Adc;

    if(m_doDigiTruth){
      if (xAdc_DigiHSTruth <0)  Adc_DigiHSTruth=0;
      else if (xAdc_DigiHSTruth >= MAXADC) Adc_DigiHSTruth=MAXADC;
      else Adc_DigiHSTruth = (short) xAdc_DigiHSTruth;
      AdcSample_DigiHSTruth[i] = Adc_DigiHSTruth;
    }

#ifndef NDEBUG
    ATH_MSG_DEBUG(" Sample " << i << "  Energy= " << Samples[i] << "  Adc=" << Adc);
#endif

  }

//
// ...... create the LArDigit and push it into the Digit container ..................
//
  Digit = new LArDigit(ch_id,igain,AdcSample);


  if(m_doDigiTruth && createDigit_DigiHSTruth){
    createDigit_DigiHSTruth = false;
    Digit_DigiHSTruth = nullptr;

    for(int i=0; i<m_NSamples; i++) {
      if(Samples_DigiHSTruth[i] != 0) createDigit_DigiHSTruth = true;
    }

    Digit_DigiHSTruth = new LArDigit(ch_id,igain,AdcSample_DigiHSTruth);
  }


  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------------------------------------------------------------------

StatusCode LArHitEMapToDigitAlg::ConvertHits2Samples(const EventContext& ctx,
                                              const Identifier & cellId, const HWIdentifier ch_id, CaloGain::CaloGain igain,
                          const std::vector<std::pair<float,float> >  *TimeE, std::vector<double> &sampleList) const

{
// Converts  hits of a particular LAr cell into energy samples
// declarations
   int nsamples ;
   int nsamples_der ;
   int i ;
   int j ;
   float energy ;
   float time ;

   SG::ReadCondHandle<ILArShape> shapeHdl(m_shapeKey, ctx);
   const ILArShape* shape=*shapeHdl;


// ........ retrieve data (1/2) ................................
//
   ILArShape::ShapeRef_t Shape = shape->Shape(ch_id,igain);
   ILArShape::ShapeRef_t ShapeDer = shape->ShapeDer(ch_id,igain);

  nsamples = Shape.size();
  nsamples_der = ShapeDer.size();

  if (nsamples==0) {
    ATH_MSG_INFO(" No samples for cell = " << cellId );
    return StatusCode::FAILURE;
  }

#ifndef NDEBUG
  ATH_MSG_DEBUG(" Cellid " << m_larem_id->show_to_string(cellId));
  for (i=0;i<nsamples;i++)
  {
       ATH_MSG_DEBUG(Shape[i] << " ");
  }
#endif

  std::vector<std::pair<float,float> >::const_iterator first = TimeE->begin();
  std::vector<std::pair<float,float> >::const_iterator last  = TimeE->end();

  while (first != last)
  {
   energy = (*first).first;
   time   = (*first).second;

#ifndef NDEBUG
  ATH_MSG_DEBUG("m_NSamples, m_usePhase " << m_NSamples << " " << m_usePhase);
#endif

   // fix the shift +1 if HEC  and nSamples 4 and firstSample 0
   // in case of data overlay this should NOT  be done as the pulse shape read from the database is already shifted
   //   but this should still be done in case of MC overlay
   int ihecshift=0;
   if((!m_RndmEvtOverlay || m_isMcOverlay) && m_larem_id->is_lar_hec(cellId) && m_NSamples.value() == 4 && m_firstSample.value() == 0) ihecshift=1;


   if (!m_usePhase) {

 // Atlas like mode where we use 25ns binned pulse shape and derivative to deal with time offsets

// shift between reference shape and this time
      int ishift=(int)(rint(time*(1./25.)));
      double dtime=time-25.*((double)(ishift));

      for (i=0;i<m_NSamples.value();i++)
      {
       j = i - ishift + m_firstSample + ihecshift;
#ifndef NDEBUG
       ATH_MSG_DEBUG(" time/i/j " << time << " "<< i << " " << j);
#endif
       if (j >=0 && j < nsamples ) {
         if (j<nsamples_der && std::fabs(ShapeDer[j])<10. )
              sampleList[i] += (Shape[j]- ShapeDer[j]*dtime)*energy ;
         else sampleList[i] += Shape[j]*energy ;
       }
      }
   }

// Mode to use phase (tbin) to get pulse shape ( pulse shape with fine time binning should be available)

   else {

     // FIXME hardcode 8phases3ns configuration (cannot access parameters from ILArShape interface now)
     int nTimeBins = 8;
     float timeBinWidth = 25./24.*3.;

//    -50<t<-25 phase=-t-25, shift by one peak time (for s2 uses shape(3) with tbin)
// for -25<t<0  phase = -t, no shift of peak time
// for 0<t<25   phase=25-t, shift by one peak time (for s2 uses shape(1) with tbin)
//    25<t<50   phase=50-t, shift by two
//  etc...

      int ishift = (int)(time*(1./25.));
      int tbin;
      if (time>0) {
         tbin = (int)(fmod(time,25)/timeBinWidth);
          if (tbin>0) {
             tbin=nTimeBins-tbin;
             ishift +=1;
          }
      } else {
         tbin = (int)(fmod(-time,25)/timeBinWidth);
      }

      double dtime = time - ( 25.*((float)(ishift)) - timeBinWidth*tbin);

      Shape = shape->Shape(ch_id,igain,tbin);
      ShapeDer = shape->ShapeDer(ch_id,igain,tbin);

      nsamples = Shape.size();
      nsamples_der = ShapeDer.size();


      for (i=0;i<m_NSamples.value();i++)
      {
       j = i - ishift+m_firstSample + ihecshift;
#ifndef NDEBUG
       ATH_MSG_DEBUG(" time/i/j " << time << " "<< i << " " << j);
#endif
       if (j >=0 && j < nsamples ) {
         if (j<nsamples_der && std::fabs(ShapeDer[j])<10. )
              sampleList[i] += (Shape[j]- ShapeDer[j]*dtime)*energy ;
         else sampleList[i] += Shape[j]*energy ;
       }
      }

   }     // else if of m_usePhase

   ++first;
  }         // loop over hits

   return StatusCode::SUCCESS;

}
