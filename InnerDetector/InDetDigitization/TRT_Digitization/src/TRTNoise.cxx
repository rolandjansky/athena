/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTNoise.h"

#include "TRTDigCondBase.h"
#include "TRTElectronicsProcessing.h"
#include "TRTElectronicsNoise.h"

#include "TRTDigSettings.h"

#include "GaudiKernel/ServiceHandle.h"

#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <cstdlib> //Always include this when including cmath!

#include "InDetSimEvent/TRTHitIdHelper.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include <algorithm>
#include <exception>

struct TRTDigitSorter {
  bool operator() (TRTDigit digit1, TRTDigit digit2) { return (digit1.GetStrawID()<digit2.GetStrawID());}
} TRTDigitSorterObject;

//_____________________________________________________________________________
TRTNoise::TRTNoise( const TRTDigSettings* digset,
		    const InDetDD::TRT_DetectorManager* detmgr,
                    CLHEP::HepRandomEngine* noiseRndmEngine,
                    CLHEP::HepRandomEngine* elecNoiseRndmEngine,
                    CLHEP::HepRandomEngine* elecProcRndmEngine,
		    TRTDigCondBase* digcond,
		    TRTElectronicsProcessing * ep,
		    TRTElectronicsNoise * electronicsnoise,
		    const TRT_ID* trt_id,
		    int UseGasMix,
		    ServiceHandle<ITRT_StrawStatusSummarySvc> sumSvc
		  )
  : m_settings(digset),
    m_detmgr(detmgr),
    m_pDigConditions(digcond),
    m_pElectronicsProcessing(ep),
    m_pElectronicsNoise(electronicsnoise),
    m_id_helper(trt_id),
    m_digitPoolLength(5000),
    m_digitPoolLength_nextaccessindex(0),
    m_msg("TRTNoise"),
    m_UseGasMix(UseGasMix),
    m_sumSvc(sumSvc)
{
  if (msgLevel(MSG::VERBOSE)) { msg(MSG::VERBOSE) << "TRTNoise::Constructor begin" << endmsg; }
  InitThresholdsAndNoiseAmplitudes_and_ProduceNoiseDigitPool(noiseRndmEngine,elecNoiseRndmEngine,elecProcRndmEngine);
  if ( m_settings->noiseInSimhits() ) m_pElectronicsNoise->reinitElectronicsNoise( 1000, elecNoiseRndmEngine );
  if (msgLevel(MSG::VERBOSE)) { msg(MSG::VERBOSE) << "Constructor done" << endmsg; }
}

//_____________________________________________________________________________
TRTNoise::~TRTNoise() {

  if (msgLevel(MSG::VERBOSE)) {msg(MSG::VERBOSE) << "TRTNoise::Destructor" << endmsg; }
}

//_____________________________________________________________________________
void TRTNoise::InitThresholdsAndNoiseAmplitudes_and_ProduceNoiseDigitPool(CLHEP::HepRandomEngine* noiseRndmEngine,
                                                                          CLHEP::HepRandomEngine* elecNoiseRndmEngine,
                                                                          CLHEP::HepRandomEngine* elecProcRndmEngine) {

  /////////////////////////////////////////////////////////////////////
  //Strategy:                                                        //
  //                                                                 //
  //First we create a lookup table so that we can, based on the      //
  //noiselevel_i (i=straw_id), calculate for each straw: (LT/NA)_i   //
  //(NA=Noise Amplitude).                                            //
  //                                                                 //
  //We also know for each straw their relative noise amplitude, r_i. //
  //                                                                 //
  //C.f. the noise note, one finds that the lowthreshold and noise   //
  //amplitude for each straw is given by the formulas:               //
  //                                                                 //
  // LT_i = (LT/NA)_i * r_i * k   and   NA_i = r_i * k               //
  //                                                                 //
  // with k = <LT_i>/<(LT/NA)_i * r_i>                               //
  //                                                                 //
  // So now we first figure out the value of k, and afterwards we go //
  // through the straws and set LT_i and NA_i.                       //
  /////////////////////////////////////////////////////////////////////

  if (msgLevel(MSG::VERBOSE)) { msg(MSG::VERBOSE)
    << "TRTNoise::InitThresholdsAndNoiseAmplitudes_and_ProduceNoiseDigitPool Begin" << endmsg;
  }

  ///////////////////////////////////////////////////////////////////
  // Step 1 - create lookup table for mapping: noiselevel -> LT/NA //
  ///////////////////////////////////////////////////////////////////
  // According to Anatoli, the noise shaping function is not very different for Argon and Xenon(and Krypton).
  std::vector<float> maxLTOverNoiseAmp;
  m_pElectronicsNoise->getSamplesOfMaxLTOverNoiseAmp(maxLTOverNoiseAmp,10000,elecNoiseRndmEngine);

  std::stable_sort( maxLTOverNoiseAmp.begin(), maxLTOverNoiseAmp.end() );
  reverse(          maxLTOverNoiseAmp.begin(), maxLTOverNoiseAmp.end() );

  // If we have LT event-event fluctuations, we need to include that effect in the curve

  double relfluct = m_settings->relativeLowThresholdFluctuation();

  if ( relfluct > 0. ) {
    std::vector<float> nl_given_LT2Amp;
    float min_lt2amp, max_lt2amp;
    makeInvertedLookupTable( maxLTOverNoiseAmp, 0., 1.,
			     nl_given_LT2Amp, min_lt2amp, max_lt2amp);
    float new_min_lt2amp, new_max_lt2amp;
    evolve_LT2AmpVsNL_to_include_LTfluct( nl_given_LT2Amp,
					  min_lt2amp, max_lt2amp,
					  relfluct,
					  new_min_lt2amp, new_max_lt2amp,
					  static_cast<unsigned int>(0.1*nl_given_LT2Amp.size()));
    float min_nl,max_nl;  //Sanity check could be that ~0 and ~1 are returned.
    makeInvertedLookupTable( nl_given_LT2Amp,new_min_lt2amp, new_max_lt2amp, maxLTOverNoiseAmp,min_nl,max_nl);
  }

  ///////////////////////////////////////////////////////////////////
  // Step 2 - figure out constant, k = <LT_i> / <(LT/NA)_i * r_i>  //
  ///////////////////////////////////////////////////////////////////

  //Figure out <(LT/NA)_i * r_i>:

  unsigned long nstrawsBa_Xe(0), nstrawsEC_Xe(0);
  unsigned long nstrawsBa_Kr(0), nstrawsEC_Kr(0);
  unsigned long nstrawsBa_Ar(0), nstrawsEC_Ar(0);
  double sumBa_Xe(0.), sumEC_Xe(0.);
  double sumBa_Kr(0.), sumEC_Kr(0.);
  double sumBa_Ar(0.), sumEC_Ar(0.);
  int hitid(0);
  float noiselevel(0.), noiseamp(0.);

  m_pDigConditions->resetGetNextStraw();

  /// Loop through all non-dead straws (there are no dead straws since 2009!):
  while ( m_pDigConditions->getNextStraw(hitid, noiselevel, noiseamp) ) {

    const bool isBarrel(!(hitid & 0x00200000));
    int strawGasType = StrawGasType(getStrawIdentifier(hitid));
    float lt = useLookupTable(noiselevel, maxLTOverNoiseAmp, 0., 1.) * noiseamp;

    if (strawGasType==0) {
      if ( isBarrel ) { ++nstrawsBa_Xe; sumBa_Xe += lt; }
      else            { ++nstrawsEC_Xe; sumEC_Xe += lt; }
    }
    else if (strawGasType==1) {
      if ( isBarrel ) { ++nstrawsBa_Kr; sumBa_Kr += lt; }
      else            {	++nstrawsEC_Kr; sumEC_Kr += lt; }
    }
    else if (strawGasType==2) {
      if ( isBarrel ) { ++nstrawsBa_Ar; sumBa_Ar += lt; }
      else            {	++nstrawsEC_Ar; sumEC_Ar += lt; }
    }

  }; // end "Loop through all non-dead straws"

  //This gives us k right away:
  double kBa_Xe(0.), kEC_Xe(0.);
  double kBa_Kr(0.), kEC_Kr(0.);
  double kBa_Ar(0.), kEC_Ar(0.);
  if (sumBa_Xe !=0.) kBa_Xe = m_settings->lowThresholdBar(0) * (nstrawsBa_Xe/sumBa_Xe);
  if (sumBa_Kr !=0.) kBa_Kr = m_settings->lowThresholdBar(1) * (nstrawsBa_Kr/sumBa_Kr);
  if (sumBa_Ar !=0.) kBa_Ar = m_settings->lowThresholdBar(2) * (nstrawsBa_Ar/sumBa_Ar);
  if (sumEC_Xe !=0.) kEC_Xe = m_settings->lowThresholdEC(0)  * (nstrawsEC_Xe/sumEC_Xe);
  if (sumEC_Kr !=0.) kEC_Kr = m_settings->lowThresholdEC(1)  * (nstrawsEC_Kr/sumEC_Kr);
  if (sumEC_Ar !=0.) kEC_Ar = m_settings->lowThresholdEC(2)  * (nstrawsEC_Ar/sumEC_Ar);

  ///////////////////////////////////////////////////////////////////
  // Step 3 - Calculate and set actual LT_i and NA_i               //
  ///////////////////////////////////////////////////////////////////

  std::vector<float> actual_LTs, actual_noiseamps;
  std::vector<int> strawTypes;
  if ( m_settings->noiseInUnhitStraws() ) {
    int nstraws = m_pDigConditions->totalNumberOfActiveStraws();
    actual_LTs.reserve(nstraws);
    actual_noiseamps.reserve(nstraws);
  };

  float actualLT, actualNA;

  m_pDigConditions->resetGetNextStraw();

  double sumLT_Ar(0.), sumLTsq_Ar(0.), sumNA_Ar(0.), sumNAsq_Ar(0.);
  double sumLT_Xe(0.), sumLTsq_Xe(0.), sumNA_Xe(0.), sumNAsq_Xe(0.);
  double sumLT_Kr(0.), sumLTsq_Kr(0.), sumNA_Kr(0.), sumNAsq_Kr(0.);

  while ( m_pDigConditions->getNextStraw( hitid, noiselevel, noiseamp) ) {

    int strawGasType = StrawGasType(getStrawIdentifier(hitid));

    const bool isBarrel(!(hitid & 0x00200000));
    if      (strawGasType==0) { actualNA = noiseamp*( isBarrel ? kBa_Xe : kEC_Xe ); }
    else if (strawGasType==1) { actualNA = noiseamp*( isBarrel ? kBa_Kr : kEC_Kr ); }
    else if (strawGasType==2) { actualNA = noiseamp*( isBarrel ? kBa_Ar : kEC_Ar ); }
    else                      { actualNA = 0.0; } // should never happen

    actualLT = useLookupTable(noiselevel, maxLTOverNoiseAmp, 0., 1.)*actualNA;

    if      (strawGasType==0) {
      sumNA_Xe += actualNA; sumNAsq_Xe += actualNA*actualNA;
      sumLT_Xe += actualLT; sumLTsq_Xe += actualLT*actualLT;
    }
    else if (strawGasType==1) {
      sumNA_Kr += actualNA; sumNAsq_Kr += actualNA*actualNA;
      sumLT_Kr += actualLT; sumLTsq_Kr += actualLT*actualLT;
    }
    else if (strawGasType==2) {
      sumNA_Ar += actualNA; sumNAsq_Ar += actualNA*actualNA;
      sumLT_Ar += actualLT; sumLTsq_Ar += actualLT*actualLT;
    }

    m_pDigConditions->setRefinedStrawParameters( hitid, actualLT, actualNA );

    if ( m_settings->noiseInUnhitStraws() ) {
      actual_LTs.push_back(actualLT);
      actual_noiseamps.push_back(actualNA);
      strawTypes.push_back(strawGasType);
    }

  };


  if (msgLevel(MSG::INFO)) {

    unsigned long nstraws_Kr = nstrawsBa_Kr + nstrawsEC_Kr;
    unsigned long nstraws_Xe = nstrawsBa_Xe + nstrawsEC_Xe;
    unsigned long nstraws_Ar = nstrawsBa_Ar + nstrawsEC_Ar;

    if (nstraws_Xe) {
      msg(MSG::INFO) << "Xe Average LT is " << sumLT_Xe/nstraws_Xe/CLHEP::eV << " eV, with an RMS of "
      << sqrt((sumLTsq_Xe/nstraws_Xe)-(sumLT_Xe/nstraws_Xe)*(sumLT_Xe/nstraws_Xe))/CLHEP::eV << " eV" << endmsg;
      msg(MSG::INFO) << "Xe Average NA is " << sumNA_Xe/nstraws_Xe/CLHEP::eV << " eV, with an RMS of "
      << sqrt((sumNAsq_Xe/nstraws_Xe)-(sumNA_Xe/nstraws_Xe)*(sumNA_Xe/nstraws_Xe))/CLHEP::eV << " eV" << endmsg;
    }
    if (nstraws_Kr) {
      msg(MSG::INFO) << "Kr Average LT is " << sumLT_Kr/nstraws_Kr/CLHEP::eV << " eV, with an RMS of "
      << sqrt((sumLTsq_Kr/nstraws_Kr)-(sumLT_Kr/nstraws_Kr)*(sumLT_Kr/nstraws_Kr))/CLHEP::eV << " eV" << endmsg;
      msg(MSG::INFO) << "Kr Average NA is " << sumNA_Kr/nstraws_Kr/CLHEP::eV << " eV, with an RMS of "
      << sqrt((sumNAsq_Kr/nstraws_Kr)-(sumNA_Kr/nstraws_Kr)*(sumNA_Kr/nstraws_Kr))/CLHEP::eV << " eV" << endmsg;
    }
    if (nstraws_Ar) {
      msg(MSG::INFO) << "Ar Average LT is " << sumLT_Ar/nstraws_Ar/CLHEP::eV << " eV, with an RMS of "
      << sqrt((sumLTsq_Ar/nstraws_Ar)-(sumLT_Ar/nstraws_Ar)*(sumLT_Ar/nstraws_Ar))/CLHEP::eV << " eV" << endmsg;
      msg(MSG::INFO) << "Ar Average NA is " << sumNA_Ar/nstraws_Ar/CLHEP::eV << " eV, with an RMS of "
      << sqrt((sumNAsq_Ar/nstraws_Ar)-(sumNA_Ar/nstraws_Ar)*(sumNA_Ar/nstraws_Ar))/CLHEP::eV << " eV" << endmsg;
    }

  }

  ///////////////////////////////////////////////////////////////////
  // Step 4 - Produce pool of pure noise digits                    //
  ///////////////////////////////////////////////////////////////////
  if ( m_settings->noiseInUnhitStraws() ) {
    ProduceNoiseDigitPool( actual_LTs, actual_noiseamps, strawTypes, noiseRndmEngine, elecNoiseRndmEngine, elecProcRndmEngine );
  }
  if (msgLevel(MSG::VERBOSE)) { msg(MSG::VERBOSE)
      << "TRTNoise::InitThresholdsAndNoiseAmplitudes_and_ProduceNoiseDigitPool Done" << endmsg;
  }
  return;
}

//_____________________________________________________________________________
void TRTNoise::ProduceNoiseDigitPool( const std::vector<float>& lowthresholds,
				      const std::vector<float>& noiseamps,
				      const std::vector<int>& strawType,
                                      CLHEP::HepRandomEngine* noiseRndmEngine,
                                      CLHEP::HepRandomEngine* elecNoiseRndmEngine,
                                      CLHEP::HepRandomEngine* elecProcRndmEngine) {

  unsigned int nstraw = lowthresholds.size();
  unsigned int istraw;

  if (msgLevel(MSG::VERBOSE)) { msg(MSG::VERBOSE) << "TRTNoise::Producing noise digit pool" << endmsg; }

  m_digitPool.resize( m_digitPoolLength );

  std::vector<TRTElectronicsProcessing::Deposit> deposits;
  int dummyhitid(0);
  TRTDigit digit;
  unsigned int nokdigits(0);
  unsigned int ntries(0);

  while ( nokdigits < m_digitPoolLength ) {

    // Once and a while, create new vectors of shaped electronics noise.
    // These are used as inputs to TRTElectronicsProcessing::ProcessDeposits
    // to create noise digits
    if ( ntries%400==0 ) {
      m_pElectronicsNoise->reinitElectronicsNoise(200, elecNoiseRndmEngine);
    }
    // Initialize stuff (is that necessary)?
    digit = TRTDigit();
    deposits.clear();

    // Choose straw to simulate
    istraw = CLHEP::RandFlat::shootInt(noiseRndmEngine, nstraw );

    // Process deposits this straw. Since there are no deposits, only noise will contrinute
    m_pElectronicsProcessing->ProcessDeposits( deposits,
					       dummyhitid,
					       digit,
					       lowthresholds.at(istraw),
					       noiseamps.at(istraw),
					       strawType.at(istraw),
                                               elecProcRndmEngine,
                                               elecNoiseRndmEngine
 					    );

    // If this process produced a digit, store in pool
    if ( digit.GetDigit() ) {
      m_digitPool[nokdigits++] = digit.GetDigit();
    }
    ntries++;
  };

  if (msgLevel(MSG::VERBOSE)) {
    if(0==ntries) {
      if (msgLevel(MSG::FATAL)) { msg(MSG::FATAL) << "ntries==0 this should not be possible!" << endmsg; }
      throw std::exception();
    }
    msg(MSG::VERBOSE)
    << "Produced noise digit pool of size " << m_digitPool.size()
    << " (efficiency was " << static_cast<double>(m_digitPool.size())/ntries << ")" << endmsg;
  }

  m_digitPoolLength_nextaccessindex = 0;

}

//_____________________________________________________________________________
void TRTNoise::appendPureNoiseToProperDigits( std::vector<TRTDigit>& digitVect, const std::set<int>& sim_hitids,
                                              CLHEP::HepRandomEngine* noiseRndmEngine )
{

  const std::set<int>::const_iterator sim_hitids_end(sim_hitids.end());

  m_pDigConditions->resetGetNextNoisyStraw();
  int hitid;
  float noiselevel;

  while (m_pDigConditions->getNextNoisyStraw(noiseRndmEngine,hitid,noiselevel) ) {
    //returned noiselevel not used for anything right now (fixme?).
    // If this strawID does not have a sim_hit, add a pure noise digit
    if ( sim_hitids.find(hitid) == sim_hitids_end ) {
      const int ndigit(m_digitPool[CLHEP::RandFlat::shootInt(noiseRndmEngine,m_digitPoolLength)]);
      digitVect.push_back(TRTDigit(hitid,ndigit));
    }
  };

  digitVect.pop_back(); //Required since last hitID is occasionally corrupted.

  return;
}

//_____________________________________________________________________________

void TRTNoise::appendCrossTalkNoiseToProperDigits(std::vector<TRTDigit>& digitVect,
						  const std::set<Identifier>& simhitsIdentifiers,
						  ServiceHandle<ITRT_StrawNeighbourSvc> TRTStrawNeighbourSvc,
                                                  CLHEP::HepRandomEngine* noiseRndmEngine) {

  //id helper:
  TRTHitIdHelper* hitid_helper = TRTHitIdHelper::GetHelper();

  std::vector<Identifier> IdsFromChip;
  std::vector<Identifier> CrossTalkIds;
  std::vector<Identifier> CrossTalkIdsOtherEnd;
  std::set<Identifier>::const_iterator simhitsIdentifiers_end = simhitsIdentifiers.end();
  std::set<Identifier>::const_iterator simhitsIdentifiers_begin = simhitsIdentifiers.begin();
  std::set<Identifier>::iterator simhitsIdentifiersIter;

  for (simhitsIdentifiersIter=simhitsIdentifiers_begin; simhitsIdentifiersIter!=simhitsIdentifiers_end; ++simhitsIdentifiersIter) {

    TRTStrawNeighbourSvc->getStrawsFromChip(*simhitsIdentifiersIter,IdsFromChip);
    CrossTalkIds.assign(IdsFromChip.begin(),IdsFromChip.end());

    //for barrel only - treated exactly equally as id's on the right end
    if (!(abs(m_id_helper->barrel_ec(IdsFromChip[0]))==1)) { continue; }

    Identifier otherEndID = m_id_helper->straw_id((-1)*m_id_helper->barrel_ec(*simhitsIdentifiersIter),
						  m_id_helper->phi_module(*simhitsIdentifiersIter),
						  m_id_helper->layer_or_wheel(*simhitsIdentifiersIter),
						  m_id_helper->straw_layer(*simhitsIdentifiersIter),
						  m_id_helper->straw(*simhitsIdentifiersIter));
    if (otherEndID.get_compact()) { CrossTalkIdsOtherEnd.push_back(otherEndID); }

    for (unsigned int i=0;i<CrossTalkIds.size();++i) {

      if ( simhitsIdentifiers.find(CrossTalkIds[i]) == simhitsIdentifiers_end )  {
	if (m_pDigConditions->crossTalkNoise(noiseRndmEngine)==1 ) {
	  const int ndigit(m_digitPool[CLHEP::RandFlat::shootInt(noiseRndmEngine,
							  m_digitPoolLength)]);
	  int barrel_endcap, isneg;
	  switch ( m_id_helper->barrel_ec(CrossTalkIds[i]) ) {
	  case -1:  barrel_endcap = 0; isneg = 0; break;
	  case  1:  barrel_endcap = 0; isneg = 1; break;
	  default:
	    if (msgLevel(MSG::WARNING)) {msg(MSG::WARNING) << "TRTDigitization::TRTNoise - identifier problems - skipping detector element!!" <<  endmsg; }
	    continue;
	  }
	  const int ringwheel(m_id_helper->layer_or_wheel(CrossTalkIds[i]));
	  const int phisector(m_id_helper->phi_module(CrossTalkIds[i]));
	  const int layer    (m_id_helper->straw_layer(CrossTalkIds[i]));
	  const int straw    (m_id_helper->straw(CrossTalkIds[i]));

	  //built hit id
	  int hitid = hitid_helper->buildHitId( barrel_endcap, isneg, ringwheel, phisector,layer, straw);
	  //add to digit vector
	  digitVect.push_back(TRTDigit(hitid,ndigit));
	}
      }
    }

    for (unsigned int i=0;i<CrossTalkIdsOtherEnd.size();++i) {
      if ( simhitsIdentifiers.find(CrossTalkIdsOtherEnd[i]) == simhitsIdentifiers_end )  {
	if (m_pDigConditions->crossTalkNoiseOtherEnd(noiseRndmEngine)==1 ) {

	  const int ndigit(m_digitPool[CLHEP::RandFlat::shootInt(noiseRndmEngine,m_digitPoolLength)]);

	  int barrel_endcap, isneg;
	  switch ( m_id_helper->barrel_ec(CrossTalkIdsOtherEnd[i]) ) {
	  case -1:  barrel_endcap = 0; isneg = 0; break;
	  case  1:  barrel_endcap = 0; isneg = 1; break;
	  default:
	    if (msgLevel(MSG::WARNING)) { msg(MSG::WARNING) << "TRTDigitization::TRTNoise - identifier problems - skipping detector element!!" <<  endmsg; }
	    continue;
	    }

	  const int ringwheel(m_id_helper->layer_or_wheel(CrossTalkIdsOtherEnd[i]));
	  const int phisector(m_id_helper->phi_module(CrossTalkIdsOtherEnd[i]));
	  const int layer    (m_id_helper->straw_layer(CrossTalkIdsOtherEnd[i]));
	  const int straw    (m_id_helper->straw(CrossTalkIdsOtherEnd[i]));

	    //built hit id
	    int hitid = hitid_helper->buildHitId( barrel_endcap, isneg, ringwheel, phisector,layer, straw);
	    //add to digit vector
	    digitVect.push_back(TRTDigit(hitid,ndigit));
	}
      }
    }

    IdsFromChip.resize(0);
    CrossTalkIdsOtherEnd.resize(0);
    CrossTalkIds.resize(0);
  }
}

void TRTNoise::sortDigits(std::vector<TRTDigit>& digitVect)
{
  std::stable_sort(digitVect.begin(), digitVect.end(), TRTDigitSorterObject);
  return;
}

//_____________________________________________________________________________
float TRTNoise::useLookupTable(const float& x, // noise_level
			       const std::vector<float>& y_given_x,
			       const float& min_x,
			       const float& max_x ) {

  double       bin_withfrac;
  unsigned int lower_index;
  double       fraction_into_bin;

  // Assumes that y_given_x is homogeneous (and not entirely flat)

  // Low x-value, return first y-value
  if ( x < min_x ) {
    return y_given_x.front();
  }

  // Which bin?
  bin_withfrac = (x-min_x)*(y_given_x.size()-1)/(max_x-min_x);
  lower_index = static_cast<unsigned int>(bin_withfrac);

  // High x-value, return last y-value
  if ( lower_index >= y_given_x.size()-1 ) {
    return y_given_x.back();
  }

  // Normal case: return weighted sum of two neighbouring bin values
  fraction_into_bin = bin_withfrac - lower_index;
  return (1.-fraction_into_bin) * y_given_x[lower_index] + fraction_into_bin * y_given_x[lower_index+1];

}

//_____________________________________________________________________________
void TRTNoise::makeInvertedLookupTable( const std::vector<float>& y_given_x,
					const float & min_x,
					const float & max_x,
					std::vector<float>& x_given_y,
					float & min_y,
					float & max_y ) {

  //Only works well when y_given_x.size() is large.
  //
  //Also assumes that y_given_x is homogeneous.

  //Figure out if rising or falling, and y limits:
  bool rising;

  if ( y_given_x.front() < y_given_x.back() ) {
    //NB: All use-cases have so far been on rising=false lookuptables
    rising = true;
    min_y = y_given_x.front();
    max_y = y_given_x.back();
  } else {
    rising = false;
    min_y = y_given_x.back();
    max_y = y_given_x.front();
  };

  const unsigned int n(y_given_x.size());
  if ( x_given_y.size() != n ) {
    x_given_y.resize(n);
  }

  //Fill new array:
  const float step_y((max_y-min_y)/(n-1));
  const float step_x((max_x-min_x)/(n-1));

  unsigned int searchindex = rising ? 0 : n-1;
  float yval;
  x_given_y.front() = rising ? min_x : max_x;
  for (unsigned int i = 1; i < n-1; ++i) {
    yval = min_y + i*step_y;
    if (rising) {
      // increase index in old array until we come to the index
      //   with a appropriate yval
      while ( y_given_x[searchindex] < yval ) { ++searchindex; };
      x_given_y[i] = min_x + searchindex*step_x;
    } else {
      while ( y_given_x[searchindex]<yval ) { --searchindex; };
      x_given_y[i] = min_x + searchindex*step_x;
    }
  };
  x_given_y.back() = rising ? max_x : min_x;

}

//_____________________________________________________________________________
void TRTNoise::evolve_LT2AmpVsNL_to_include_LTfluct( std::vector<float>& nl_given_lt2na,
						     const float & min_lt2na,
						     const float & max_lt2na,
						     const float relativeLTFluct,
						     float & new_min_lt2na,
						     float & new_max_lt2na,
						     const unsigned int& number_new_bins )
{
  //RelativeLTfluct should be less than 0.2.

  //  unsigned int n = nl_given_lt2na.size();
  std::vector<float> new_nl_given_lt2na(number_new_bins);

  new_min_lt2na = min_lt2na;
  new_max_lt2na = relativeLTFluct < 0.4 ? max_lt2na/(1.0-2.0*relativeLTFluct) : 5*max_lt2na;

  for (unsigned int i = 0; i<number_new_bins;++i) {
    const float new_lt2naval(new_min_lt2na + (new_max_lt2na-new_min_lt2na)*static_cast<float>(i)/(number_new_bins-1));
    float sigma = new_lt2naval*relativeLTFluct;
    if ( sigma <= 0 ) {
      if (sigma<0) { sigma *= -1.0; }
      else { sigma = 1.0e-8; }
    }
    const float lowerintrange(new_lt2naval - 5.0*sigma);
    float upperintrange(new_lt2naval + 5.0*sigma);
    if (upperintrange>max_lt2na) {
      upperintrange = max_lt2na; //Since the NL is zero above anyway
    }
    const double du((upperintrange-lowerintrange)/100.0);
    double sum(0.);
    const double minusoneover2sigmasq(- 1.0 / (2.0*sigma*sigma));

    for (double u(lowerintrange); u < upperintrange; u += du) {
      sum += useLookupTable(u,nl_given_lt2na,min_lt2na,max_lt2na) *
	exp(minusoneover2sigmasq * (u-new_lt2naval) * (u-new_lt2naval));
    }
    sum *= du / ( sqrt (2.0 * 3.14159265358979 ) * sigma);
    new_nl_given_lt2na[i] = sum;
  };

  nl_given_lt2na.resize(number_new_bins);
  copy(new_nl_given_lt2na.begin(),
       new_nl_given_lt2na.end(),
       nl_given_lt2na.begin());
}

Identifier TRTNoise::getStrawIdentifier ( int hitID )
{
  Identifier IdStraw;
  Identifier IdLayer;

  const int mask(0x0000001F);
  const int word_shift(5);
  int trtID, ringID, moduleID, layerID, strawID;
  int wheelID, planeID, sectorID;

  const InDetDD::TRT_BarrelElement *barrelElement;
  const InDetDD::TRT_EndcapElement *endcapElement;

  if ( !(hitID & 0x00200000) ) {      // barrel
    strawID   = hitID & mask;
    hitID   >>= word_shift;
    layerID   = hitID & mask;
    hitID   >>= word_shift;
    moduleID  = hitID & mask;
    hitID   >>= word_shift;
    ringID    = hitID & mask;
    trtID     = hitID >> word_shift;

    barrelElement = m_detmgr->getBarrelElement(trtID, ringID, moduleID, layerID);
    if ( barrelElement ) {
      IdLayer = barrelElement->identify();
      IdStraw = m_id_helper->straw_id(IdLayer, strawID);
    } else {
      msg(MSG::ERROR) << "Could not find detector element for barrel identifier with "
		      << "(ipos,iring,imod,ilayer,istraw) = ("
		      << trtID << ", " << ringID << ", " << moduleID << ", "
		      << layerID << ", " << strawID << ")" << endmsg;
    }
  } else {                           // endcap
    strawID   = hitID & mask;
    hitID   >>= word_shift;
    planeID   = hitID & mask;
    hitID   >>= word_shift;
    sectorID  = hitID & mask;
    hitID   >>= word_shift;
    wheelID   = hitID & mask;
    trtID     = hitID >> word_shift;

    // change trtID (which is 2/3 for endcaps) to use 0/1 in getEndcapElement
    if (trtID == 3) { trtID = 0; }
    else            { trtID = 1; }

    endcapElement = m_detmgr->getEndcapElement(trtID, wheelID, planeID, sectorID);

    if ( endcapElement ) {
      IdLayer = endcapElement->identify();
      IdStraw = m_id_helper->straw_id(IdLayer, strawID);
    } else {
      msg(MSG::ERROR) << "Could not find detector element for endcap identifier with "
		      << "(ipos,iwheel,isector,iplane,istraw) = ("
		      << trtID << ", " << wheelID << ", " << sectorID << ", "
		      << planeID << ", " << strawID << ")" << endmsg;
      msg(MSG::ERROR) << "If this happens very rarely, don't be alarmed "
		      << "(it is a Geant4 'feature')" << endmsg;
      msg(MSG::ERROR) << "If it happens a lot, you probably have misconfigured geometry "
		      << "in the sim. job." << endmsg;
    }

  }

  return IdStraw;
}


//_____________________________________________________________________________
int TRTNoise::StrawGasType(Identifier TRT_Identifier) {

  // TRT/Cond/StatusHT provides: enum { Undefined, Dead(Ar), Good(Xe), Xenon(Xe), Argon(Ar), Krypton(Kr) }
  // The m_UseGasMix default behaviour (0) is to use TRT/Cond/StatusHT, other values can be set to force
  // the whole detector to (1)Xenon, (2)Krypton, (3)Argon:

  int strawGasType=99;

  if (m_UseGasMix==0) { // use StatusHT
    int stat =  m_sumSvc->getStatusHT(TRT_Identifier);
    if       ( stat==2 || stat==3 ) { strawGasType = 0; } // Xe
    else if  ( stat==5 )            { strawGasType = 1; } // Kr
    else if  ( stat==1 || stat==4 ) { strawGasType = 2; } // Ar
    else if  ( stat==6 )            { strawGasType = 0; } // Xe (em Ar)
    else if  ( stat==7 )            { strawGasType = 0; } // Xe (em Kr)
    // stat==6 is emulate argon, 7 is emular krypton --  make it xenon here,
    // and emulate argon later with reduced TR eff.
  }
  else if (m_UseGasMix==1) { strawGasType = 0; } // force whole detector to Xe
  else if (m_UseGasMix==2) { strawGasType = 1; } // force whole detector to Kr
  else if (m_UseGasMix==3) { strawGasType = 2; } // force whole detector to Ar

  return strawGasType;

}
