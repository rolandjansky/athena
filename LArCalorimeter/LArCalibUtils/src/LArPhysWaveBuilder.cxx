// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LArPhysWaveBuilder.cxx,v 1.13 2009-04-27 15:46:36 gunal Exp $
/**
 * @file  LArCalibUtils/LArPhysWaveBuilder.h
 * @brief Build LAr wave shapes from real data.
 */

#include "LArCalibUtils/LArPhysWaveBuilder.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArRawConditions/LArPhysWaveContainer.h"
#include "LArRecUtils/LArParabolaPeakRecoTool.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArElecCalib/ILArPhaseTool.h"
#include "LArElecCalib/ILArPedestal.h"
#include "StoreGate/ReadHandle.h"
#include "Identifier/HWIdentifier.h"
#include "AthenaKernel/errorcheck.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

#include <algorithm>
#include <math.h>


using CLHEP::MeV;


namespace {


/// Maps between gain indices and gain enums/names.
CaloGain::CaloGain gain_enums[LArPhysWaveBuilder::N_GAINS] =
  { CaloGain::LARHIGHGAIN, CaloGain::LARMEDIUMGAIN, CaloGain::LARLOWGAIN };
const char* gain_names[LArPhysWaveBuilder::N_GAINS] =
  { "HG", "MG", "LG" };

/// Used to mark an invalid cell entry in the table.
LArPhysWave* const wave_invalid = reinterpret_cast<LArPhysWave*> (-1);


/**
 * @brief Find the peak of a pulse by doing cubic interpolation.
 * @param samples The (pedestal-subtracted) sample values.
 * @param max_bin The index of the bin with the maximum value.
 * @param[out] peak_time The position of the peak (in samples).
 * @param[out] peak_value The value of the cubic at the maximum.
 * @returns True if the cubic fit succeeded, false if not.
 */
bool find_peak (const std::vector<double>& samples,
                int max_bin,
                float& peak_time,
                float& peak_value)
{
  // We want to find the coefficents of a cubic that goes through
  // four specified points.  The abcissas of the ordinates of the
  // four points can be taken to be fixed at 0,1,2,3.  So, let
  // f(x;A,B,C,D) be a cubic in x with coefficients A,B,C,D.
  // We then write down the four equations:
  //
  //    f(0) = y0
  //    f(1) = y1
  //    f(2) = y2
  //    f(3) = y3
  //
  // This is then a linear system in A,B,C,D, which we can solve by inverting
  // the matrix formed by the LHS.  This matrix is in fact a constant
  // (because the abcissas are constant), so we can write its inverse
  // here directly as a constant:
  const double invT[4][4] 
    = { {        1,       0,       0,        0},
        {   -11./6,       3,    -1.5,     2./6},
        {        1,    -2.5,       2,     -0.5},
        {    -1./6,     0.5,    -0.5,     1./6}  };

  // Choose four samples around the maximum bin to define the cubic.
  // Four is an even number, so we can't center them on the maximum bin.
  // Choose the alternative that gives us the largest bin on the edge.
  // But be careful not to go outside the array bounds.
  int it0;
  if (max_bin < 2)
    it0 = 0;
  else if (max_bin+2 >= (int)samples.size())
    it0 = samples.size() - 4;
  else if (samples[max_bin-2] >  samples[max_bin+2])
    it0 = max_bin - 2;
  else
    it0 = max_bin - 1;
	  
  // 4 samples to be used start at 0 <= t0 <= nsamples-4
  // Do the matrix multiplication to solve the linear system
  // for the coefficients.
  double A[4] = {0, 0, 0, 0};
  for (int ia = 0; ia < 4; ia++) 
    for (int it = 0; it < 4; it++) 
      A[ia] += invT[ia][it] * samples[it0+it];

  // Now, find the maximum of the cubic.  The position of the maximum
  // will be given by a quadratic.  But if this quadratic's discriminant
  // is negative, then we don't have a maximum.
  double disc = A[2]*A[2] - 3*A[1]*A[3];
  bool cubic_failed = disc < 0;
  if ( ! cubic_failed) {
    double dtmax;
    // Protect against the cubic coefficent being zero --- in that case,
    // we just have a quadratic and the solution's linear.
    if (A[3] == 0) {
      if (A[2] == 0)
        dtmax = -1;
      else
        dtmax = -A[1] / 2 / A[2];
    }
    else {
      // Solve the quadratic to find the position of the maximum.
      dtmax = (-A[2]-sqrt(disc))/3/A[3];
    }

    // The maximum must be within the range fit.
    // Give up if it isn't.
    cubic_failed = dtmax < 0 || dtmax > 3;
    if ( ! cubic_failed ) {
      // Add in the offset of the first fit sample.
      peak_time = it0 + dtmax; // samples

      // Evaluate the polynomial to find the peak value.
      double d_peak_value = A[3];
      for (int ia=2; ia >= 0; --ia)
        d_peak_value = d_peak_value * dtmax + A[ia];

      // Check for a pathological fit.
      for (int ia=0; ia < 4; ia++) {
        if (samples[it0+ia] > d_peak_value * 1.000001) {
          cubic_failed = true;
          break;
        }
      }
      peak_value = d_peak_value;
    }
  }

  return cubic_failed;
}


} // anonymous namespace


/**
 * @brief Constructor for wave holder.
 *
 * This ensures that the pointers get initialized to null.
 */
LArPhysWaveBuilder::Cellwave::Cellwave ()
{
  for (int i=0; i<N_GAINS; i++)
    gains[i]=0; 
}


/**
 * @brief Standard Algorithm constructor.
 * @brief name The name of the algorithm.
 * @brief svcloc The service locator object.
 */
LArPhysWaveBuilder::LArPhysWaveBuilder (const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_peakParabolaTool (0),
    m_adc2mevTool (0),
    m_phase_tool (0),
    m_hmaxADCpeak(0),
    m_hADCpeak(0),
    m_hDigiGainSampling(0),
    m_groupingType("FeedThrough") // SubDetector, Single, FeedThrough
{


  for (int i=0;i<N_SAMPLINGS;++i) {
    m_henergy[i]=0;
    m_hsigma[i]=0;
    m_hlaymax[i]=0;
  }

  /// Algorithm properties.

  // Number of bins per sample to record.
  declareProperty("BinsPerSample", m_bins_per_sample = 24);

  // Time in ns between samples (normally 25).
  declareProperty("SampleClock", m_sample_clock = 25*CLHEP::ns);

  // minimal number of entries for a profile to be saved
  declareProperty("NEntriesCut", m_nentriescut = 128); 

  // Energy threshold for each layer per cell per event
  // required to accumulate the cell.
  declareProperty("PSCut",m_PSCut = 0.*MeV);
  declareProperty("StripsCut",m_StripsCut = 0.*MeV);
  declareProperty("MiddleCut",m_MiddleCut = 0.*MeV);
  declareProperty("BackCut",m_BackCut = 0.*MeV);

  // In order to accumulate a cell, its energy must be at least
  // this many sigma over the pedestal.
  declareProperty("SigmaCut",m_sigma_cut = 0);
  
  // If true, normalize each shape to the peak value before accumulating.
  declareProperty("Normalize",m_normalize = true);

  // If true, use the parabola fitting tool to find the peak.
  // If false, use a simple cubic fit.
  declareProperty("UseParabola", m_use_parabola = true);

  // The name of the tool to get the cell-by-cell phase information.
  // If this is the string `peak', then we instead simply line up the
  // waveforms using the fitted peak positions.
  declareProperty("PhaseTool", m_phase_tool_name = "LArPhaseToolTB");

  // If true, dump the accumulated data to a ROOT file.
  declareProperty("RootRawDump",m_rootrawdump=false);

  // Name of the ROOT file to create.
  declareProperty("RootOutputFile", m_rootoutputfile="profile.root");

  // Folder to fill in the database.
  declareProperty("FolderName_LArPhysWave",
                  m_fullFolderNameLArPhysWave =
                    "/lar/LArElecCalibTB04/LArPhysWaveContainer/LArPhysWave");

  // If true, merge the data into an existing energy
  // in the database.
  // (XXX Obsolete: this needs to change to COOL.)
  declareProperty("UpdateDB",m_updateDB=false);
  
  // Grouping type
  declareProperty("GroupingType",   m_groupingType); 
}


/**
 * @brief Destructor.
 */
LArPhysWaveBuilder::~LArPhysWaveBuilder() 
{
}


/**
 * @brief Standard Algorithm initialize method.
 */
StatusCode LArPhysWaveBuilder::initialize()
{
   ATH_CHECK(m_cablingKey.initialize());

  // Fetch tools and services.
  CHECK( toolSvc()->retrieveTool("LArADC2MeVTool", m_adc2mevTool) );
  if (m_phase_tool_name != "peak")
    CHECK( toolSvc()->retrieveTool(m_phase_tool_name, m_phase_tool) );
  if (m_use_parabola)
    CHECK( toolSvc()->retrieveTool("LArParabolaPeakRecoTool",
                                   m_peakParabolaTool) );
  // declaration of histograms for debuging
  gROOT->cd();
  m_hmaxADCpeak = new TH1F("hmaxADCpeak","hmaxADCpeak",500,0,4096);
  m_hADCpeak = new TH1F("hADCpeak","hADCpeak",500,0,4096);
  m_hDigiGainSampling = new TH2F("hDigiGainSampling","DigiGainSampling",
                                 5,0,5,5,0,5);
  for (int i=0; i<N_SAMPLINGS; i++) {
    char buf[50];
    sprintf (buf, "energy%d", i);
    m_henergy[i] = new TH1F (buf, buf, 500, 0, 10*1000);
    sprintf (buf, "sigma%d", i);
    m_hsigma[i] = new TH1F (buf, buf, 500, 0, 100);
    sprintf (buf, "laymax%d", i);
    m_hlaymax[i] = new TH1F (buf, buf, 500, 0, 500);
  }

  ATH_CHECK( m_gainCont.initialize() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Algorithm execute method.
 */
StatusCode LArPhysWaveBuilder::execute()
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  ATH_MSG_DEBUG ( "LArPhysWaveBuilder in execute()" );

  // Retrieve cabling
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling=*cablingHdl;
  if (!cabling) {
    ATH_MSG_ERROR( " Can't get cabling with key: " << m_cablingKey.key() );
    return StatusCode::FAILURE;
  }

  // Get the identifier helper.
  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  const LArEM_ID* emId = idHelper->em_idHelper();

  // Our input data.
  SG::ReadHandle<LArDigitContainer> digcontainer (m_gainCont, ctx);

  // Get the pedestal information.
  const ILArPedestal* larPedestal =
    detStore()->retrieve<const ILArPedestal> ();

  // Get the ID hash helper.
  const LArOnlineID* online_helper =
    detStore()->retrieve<const LArOnlineID> ("LArOnlineID");

  if (!online_helper) {
    REPORT_ERROR (StatusCode::FAILURE) << "Cannot find storegate inputs";
    return StatusCode::SUCCESS;
  }

  // Loop on LArDigits
  for (const LArDigit* p_lardigit : *digcontainer) {

    // Get data from LArDigit
    HWIdentifier chid = p_lardigit->hardwareID();  

    // Convert the gain from an enum to an index in our tables.
    CaloGain::CaloGain digiGain = p_lardigit->gain();
    int gain_index = std::find (gain_enums, gain_enums + N_GAINS, digiGain)
                       - gain_enums;
    if (gain_index >= N_GAINS) {
      REPORT_ERROR (StatusCode::FAILURE) << "Bad gain value " << digiGain
                                         << " for channel " << chid << endmsg;
      continue;
    }

    // For each channel, we need to check that it's actually connected.
    // If the channel passes the adc cut, we'll also need a wave
    // object for it.  The logic below is to avoid doing the connected
    // check for any channel more than once, and also to be lazy,
    // so that we don't check a channel or make a wave object
    // before we absolutely have to.
    //
    // Before we check a channel, cellwave.id will be invalid, and
    // all wave pointers will be null.
    //
    // After we check a channel, if it is not in fact valid, the wave
    // pointers will all be set to wave_invalid.  If it is valid,
    // then the identifier will be made valid, and the pointers
    // will either be null or pointing at wave structures.
    IdentifierHash hash = online_helper->channel_Hash (chid);
    if (m_waves.size() <= hash)
      m_waves.resize (hash+1);
    Cellwave& cellwave = m_waves[hash];
    LArPhysWave* wave = cellwave.gains[gain_index];
    if (wave == wave_invalid) continue;
    if (! cellwave.id.is_valid()) {
      // Save online id in cell wave
      cellwave.onlineId = chid;

      if (cabling->isOnlineConnected(chid)) {
        try {
          cellwave.id = cabling->cnvToIdentifier( chid );
        }
        catch( LArID_Exception & except ) {
          ATH_MSG_ERROR ( "Channel not registered: " << chid );
        }  
      }
      if (!cellwave.id.is_valid() || !emId->is_lar_em(cellwave.id)) {
        for (int i = 0; i < N_GAINS; i++)
          cellwave.gains[i] = wave_invalid;
        continue;
      }
    }
                   
    // Find the sampling for this cell.
    int sampling = emId->sampling (cellwave.id);
    if (sampling < 0 || sampling >= N_SAMPLINGS) {
      REPORT_ERROR (StatusCode::FAILURE) << "Bad sampling " << sampling
                                         << "for channel " << chid;
      continue;
    }

    // Fill histograms for debugging
    m_hDigiGainSampling->Fill(gain_index, sampling);

    ATH_MSG_DEBUG ( "gain = " << digiGain << " of channel id = " << chid );

    // Get the vector of sample values.
    const std::vector<short>& samples = p_lardigit->samples();
    int nSamples  = samples.size();

    // Find the pedestal and sigma for this channel.
    double pedestal=0.;
    double sigma = 0;
    if(larPedestal) {
      float  db_pedestal=larPedestal->pedestal(chid,digiGain);
      // only db_pedestal[0] is filled when retrieve from DB 
      if (db_pedestal>= (1.0+LArElecCalib::ERRORCODE))
        pedestal=db_pedestal; 
      else
        pedestal = samples[0];   
      ATH_MSG_DEBUG("pedestal: "<<pedestal);

      float  db_sigma=larPedestal->pedestalRMS(chid,digiGain);
      if (db_sigma >= (1.0+LArElecCalib::ERRORCODE))
        sigma = db_sigma;
    } else {
      ATH_MSG_DEBUG("Pedestal not loaded - subtract sample[0]");
      pedestal=samples[0];	
    }

    // Subtract pedestal and check for saturation.
    bool saturation = false;
    std::vector<double> dsamples (samples.size());
    for (int i=0; i<nSamples; i++) {
      if (samples[i] > 4000) saturation = true;
      dsamples[i] = (samples[i] - pedestal);
    }
    if (saturation) continue; // Skip cell if ADC saturated.

    // Find the maximum sample.
    std::vector<double>::const_iterator max_it =
      std::max_element (dsamples.begin(), dsamples.end());
    unsigned int iPeakSamp = max_it - dsamples.begin();
    float maxADCPeak = *max_it;

    // calculation of AdcCut depending of the gain and of the layer 
    float energy = 0.;
    float energyCut=10000.;

    // Find the energy cut, based on the sampling.
    switch (sampling) {
    case 0: energyCut = m_PSCut; break;
    case 1: energyCut = m_StripsCut; break;
    case 2: energyCut = m_MiddleCut; break;
    case 3: energyCut = m_BackCut; break;
    default:
      abort();
    }

    // Find the energy in this cell.
    if (m_adc2mevTool) {
      //ADC2MeV (a.k.a. Ramp)   
      const std::vector<float>& ramp=m_adc2mevTool->ADC2MEV(chid,digiGain);

      //Check ramp coefficents
      if (ramp.size()>1 && ramp[1]<500 && ramp[1]>0) {
        energy=0;

        //for (unsigned i=1;i<ramp.size();i++){
        //  energy += ramp[i]*pow(maxADCPeak,i);
        //}
        energy = ramp.back();
        for (unsigned i = ramp.size()-2; i >= 1; i--)
          energy = energy * maxADCPeak + ramp[i];
        energy *= maxADCPeak;

        ATH_MSG_DEBUG(" layer = " << sampling
                      << " gain = " << digiGain 
                      << " energy = "<< energy << " chid = " << chid  );
      }
    }

    // Fill per-sampling histograms.
    m_henergy[sampling]->Fill (energy);
    m_hsigma[sampling]->Fill (maxADCPeak / (sigma ? sigma : 1));
    m_hlaymax[sampling]->Fill (maxADCPeak);

    // To keep the cell, we must pass cuts on energy and sigma.
    if (energy > energyCut && maxADCPeak > m_sigma_cut * sigma) {

      // fill histo with the maximum of the peak (ADC value)
      m_hmaxADCpeak->Fill(maxADCPeak);

      float ADCtime = -999;
      float ADCpeak = 0;
      bool fit_failed = false;

      // Try to find the peak of the wave.
      // Use either peakParabolaTool or a cubic fit.
      if( m_peakParabolaTool) {
        if (!emId->is_em_barrel(cellwave.id)) continue;
        std::vector<float> peak =
          m_peakParabolaTool->peak(samples,sampling,pedestal); 

        if(peak.size() >2){
          ADCpeak = peak[0]-pedestal;
          ADCtime = peak[2];
          ATH_MSG_DEBUG ( "Computed from Parabola" );
        }else{
          ATH_MSG_DEBUG( "No pic is computed from Parabola. Use Max Sample." );
          ADCpeak = -9999;
          ADCtime = -9999;
          fit_failed = true;
        }
      }
      else {
        // Use old cubic fit.
        fit_failed = find_peak (dsamples, iPeakSamp, ADCtime, ADCpeak);
        ADCtime *= m_sample_clock; // Convert from samples to ns.
      }

      // We'll divide all the samples by ADCpeak.
      // If we don't want to normalize, set to 1.
      if (!m_normalize) ADCpeak=1;

      // Avoid division by 0.
      if (ADCpeak == 0) fit_failed = true;
	  
      // Get the phase for this channel.
      float time = 0;
      if (!m_phase_tool) {
        // Get the phase from the position of the peak.
        time = ADCtime - 2*m_sample_clock;
        if (time < 0) time = 0;
      }
      else {
        // Call the provided phase tool.
        CHECK( m_phase_tool->phase (cellwave.id, time) );
      }

      // Convert phase to a sample index.
      int itime = static_cast<int> (time / m_sample_clock * m_bins_per_sample);
	  
      // See if we have a good fit and phase.
      if (!fit_failed && ADCtime >= 0 && ADCtime < nSamples * m_sample_clock) {
        // Ok, histogram the phase.
        m_hADCpeak->Fill(ADCpeak);

        // Normalize the wave.
        for (int i=0; i<nSamples; i++)
          dsamples[i] /= ADCpeak;

        // If we don't have a wave structure for this channel/gain yet,
        // create it.
        if (! wave) {
          cellwave.gains[gain_index] = 
            wave = new LArPhysWave (nSamples * m_bins_per_sample +
                                    m_bins_per_sample/2,
                                    m_sample_clock / m_bins_per_sample,
                                    0,
                                    (unsigned)LArWave::meas);
        }

        // Accumulate this wave.
        wave->addEvent (itime, m_bins_per_sample, dsamples);

      } // fit cuts
    } // energy/sigma cut
  } // end loop on LArDigit in LArDigitContainer

  ATH_MSG_DEBUG(" end of execute ");


  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Algorithm finalize method.
 */
StatusCode LArPhysWaveBuilder::stop()
{
  std::unique_ptr<LArPhysWaveContainer>  larPhysWaveContainer;
  CHECK( make_container(larPhysWaveContainer) );

  // Root raw output.
  if (m_rootrawdump)
    CHECK( write_root (larPhysWaveContainer.get()) );

  // Record LArPhysWaveContainer to detector store
  CHECK( detStore()->record(std::move(larPhysWaveContainer), "") );
  ATH_MSG_DEBUG ( "LArPhysWaveContainer has been recorded to StoreGate"
                  << " with key= size="<<larPhysWaveContainer->size());
 
  ATH_MSG_DEBUG ( "End of stop" );
  return StatusCode::SUCCESS;
}


/**
 * @brief Make the wave container from the accumulated waves.
 * @param[out] larPhysWaveContainer The created container.
 */
StatusCode LArPhysWaveBuilder::make_container (std::unique_ptr<LArPhysWaveContainer>&
                                                larPhysWaveContainer)
{
  MsgStream log(msgSvc(), name());

  // Create LArPhysWaveContainer
  larPhysWaveContainer = std::make_unique<LArPhysWaveContainer>();

  ATH_CHECK( larPhysWaveContainer->setGroupingType(m_groupingType,msg()) );
  ATH_CHECK( larPhysWaveContainer->initialize() );
        
  // Create LArPhysWave and put it to LArPhysWaveContainer
  int nentries[N_GAINS] = {0};
  int nentries_cut[N_GAINS] = {0};

  int nchannels = m_waves.size();
  ATH_MSG_DEBUG ("nchannels: "<<nchannels);
  for (int i=0; i<nchannels; i++) { //loop on channels
    Cellwave& cellwave = m_waves[i];
    for (int gain=0; gain < N_GAINS; ++gain) {
      LArPhysWave* wave = cellwave.gains[gain];
      if (wave && wave != wave_invalid) {
        const std::vector<int>& triggers = wave->getTriggers();
        int tot_ent = 0;
        for (unsigned i=0; i < triggers.size(); i++)
          tot_ent += triggers[i];
        if (tot_ent>m_nentriescut) {
          larPhysWaveContainer->setPdata (cellwave.onlineId,
                                          *wave,
                                          gain);
          ++nentries[gain];
        }
        else {
          delete wave;
          ++nentries_cut[gain];
        }
        wave = 0;
      }
    }
  }
    
  ATH_MSG_INFO(" | Summary of processed channels");
  for (int gain=0; gain < N_GAINS; ++gain) {
    ATH_MSG_INFO ( " | " << gain_names[gain] << " : "
                   << nentries[gain] << " channels accepted. "
                   << nentries_cut[gain] << " channels rejected (nentries cut)" );
  }
  ATH_MSG_DEBUG("LArPhysWave created");

  return StatusCode::SUCCESS;
}


/**
 * @brief Dump the accumulated waves to a root file.
 * @param larPhysWaveContainer The container of waves to dump.
 */
StatusCode
LArPhysWaveBuilder::write_root (LArPhysWaveContainer* larPhysWaveContainer)
{
  // Get ID translators.
  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  const LArEM_ID* emId = idHelper->em_idHelper();

  // Retrieve cabling
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling=*cablingHdl;
  if (!cabling) {
    ATH_MSG_ERROR( " Can't get cabling with key: " << m_cablingKey.key() );
    return StatusCode::FAILURE;
  }

  // Open the root file.
  TFile rootoutputfile(m_rootoutputfile.c_str(),"RECREATE");  

  // Loop over all waves.
  int nwave = 0;
  for (int igain = 0; igain < N_GAINS; ++igain) {
    typedef LArPhysWaveContainer::ConditionsMapIterator It;
    CaloGain::CaloGain gain = gain_enums[igain];
    It end = larPhysWaveContainer->end (gain);
    for (It i = larPhysWaveContainer->begin (gain);
         i != end;
         ++i)
    {
      // Fetch the wave to wave.
      const LArPhysWave& wave = *i;
      int nbins = wave.getSize();
      if (nbins == 0) continue;
      char name[50];

      // Construct the name to use for the histogram
      // in the root file.
      HWIdentifier chid = i.channelId();
      Identifier id = cabling->cnvToIdentifier (chid);
      sprintf(name, "phys%.9d_%.1d%.1d%.1d%.1d%.3d%.3d",
              (int)chid.get_identifier32().get_compact(),
              gain,
              emId->barrel_ec (id)+3,
              emId->sampling(id),
              emId->region(id),
              emId->phi(id),
              emId->eta(id));

      // Create the histogram.
      TProfile* prof = new TProfile (name, name, nbins,
                                     -(m_bins_per_sample-1),
                                     nbins-(m_bins_per_sample-1));

      // Fill the histogram.
      for (int j=0; j<nbins; j++) {
        int nent = wave.getTrigger(j);
        double mean = wave.getSample(j);
        double err = wave.getError(j);
        double sumsq = (err*err + mean*mean) * nent;
        prof->SetBinContent (j+1, mean * nent);
        prof->SetBinError (j+1, sqrt(sumsq));
        prof->SetBinEntries (j+1, nent);
      }

      // Write it to the root file.
      // Remember to delete it after writing.
      prof->Write();
      delete prof;
      ++nwave;
    }
  }

  // Write debugging histograms.
  m_hmaxADCpeak->Write();
  m_hADCpeak->Write();
  m_hDigiGainSampling->Write();
  for (int i=0; i<N_SAMPLINGS; i++) {
    m_henergy[i]->Write();
    m_hsigma[i]->Write();
    m_hlaymax[i]->Write();
  }
    
  // Close the root file.
  rootoutputfile.Close();
  ATH_MSG_INFO ( nwave << " profile histos dumped to root file"
                 << m_rootoutputfile );

  return StatusCode::SUCCESS;
}
