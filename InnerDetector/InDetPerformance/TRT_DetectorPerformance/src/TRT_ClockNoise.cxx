/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_DetectorPerformance/TRT_ClockNoise.h"

//#include "GaudiKernel/IJobOptionsSvc.h"
//#include "GaudiKernel/ITHistSvc.h"
//#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/DataHandle.h"

#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_Numerology.h"

#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

//#include "TrkParameters/MeasuredPerigee.h"
#include "TrkParameters/TrackParameters.h"
//#include "TrkTrackSummaryTool/ITrackSummaryTool.h"
#include "TrkTrackSummary/TrackSummary.h"
//#include "TrkParameters/MeasuredAtaStraightLine.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "TRT_DetectorPerformance/TRT_StrawMap.h"

#include "TTree.h"
//#include "TVirtualFFT.h"
#include "TComplex.h"


/////////////////////////////////////////////////////////////////////////////

InDet::TRT_ClockNoise::TRT_ClockNoise(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_trtStrawNeighbourTool("TRT_StrawNeighbourSvc", name),
  m_trtStrawMap("InDet::TRT_StrawMap"),
  m_TRTStrawStatusSummarySvc("TRT_StrawStatusSummarySvc", name),
  m_print_ntuple(false),
  m_textfile("clocknoise.dat"),
  m_min_noise_hits(100),
  m_min_asym(0.9),
  m_histSvc("THistSvc", name)
{
  declareProperty("printNtuple", m_print_ntuple);
  declareProperty("outputTextFileName", m_textfile);
  declareProperty("histogramService", m_histSvc);
  declareProperty("minNoiseHits",m_min_noise_hits);
  declareProperty("minAsymmetry",m_min_asym);

  m_ntuple = 0;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode InDet::TRT_ClockNoise::initialize(){

  if (msgLvl(MSG::INFO)) msg() << "TRT_ClockNoise::initialize()" << endreq;
  
  m_evtNum = 0;
  
  // initialize occupancy array
  for (int iphi=0; iphi<nPhi; ++iphi) {
    for (int iside=0; iside<nSide; ++iside) {
      for (int istraw=0; istraw<nStraw; ++istraw) {
        for (int ibin=0; ibin<nBin; ++ibin) {
          m_straw_bin_occ[iphi][iside][istraw][ibin] = 0;
        }
      }
    }
  }
  
  if (m_print_ntuple) {
    //////////////////////////////////////////////
    // Create TTree and register it to THistSvc
    //////////////////////////////////////////////
    // get StoreGate service

    if ( m_histSvc.retrieve().isFailure() ){
      msg(MSG::FATAL) << "histogram service not found" << endreq;
      return StatusCode::FAILURE;
    } else 
      if (msgLvl(MSG::INFO)) msg() << "retrieved histogramService" << endreq;
  
    if (m_TRTStrawStatusSummarySvc.retrieve().isFailure()) {
      msg(MSG::ERROR) << "could not retrieve m_TRTStrawStatusSummarySvc" << endreq;
    } else {
      msg(MSG::INFO) << "retrieved m_TRTStrawStatusSummarySvc" << endreq; 
    }
 
    m_ntuple = new TTree("ntuple" , "ntuple");
  
    std::string fullTreeName_ntuple =  "/noise/ntuple" ;
    if((m_histSvc->regTree(fullTreeName_ntuple, m_ntuple)).isFailure())
      {
        msg(MSG::ERROR) << "Unable to register TTree : " << fullTreeName_ntuple << endreq;
        return StatusCode::FAILURE;
      }

    //////////////////////////////////////////////////////
    /// Book variables to be added to the output ntuple
    //////////////////////////////////////////////////////
    m_ntuple->Branch("LEbin"           , &m_leadingEdgeBin      , "LEbin/I"); 
    m_ntuple->Branch("board"           , &m_board		, "board/I"); 
    m_ntuple->Branch("chip"	       , &m_chip                , "chip/I"); 
    m_ntuple->Branch("pad"	       , &m_pad                 , "pad/I"); 
    m_ntuple->Branch("side"	       , &m_side                , "side/I"); 
    m_ntuple->Branch("phi"             , &m_phi                 , "phi/I"); 
    m_ntuple->Branch("straw"           , &m_straw               , "straw/I"); 
    m_ntuple->Branch("strawLayer"      , &m_strawLayer          , "strawLayer/I"); 
    m_ntuple->Branch("layer"           , &m_layer               , "layer/I"); 
    m_ntuple->Branch("evtNum"          , &m_evtNum              , "evtNum/I"); 
  
  }

  if (msgLvl(MSG::INFO)) msg() << "end initialize()" << endreq;
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode InDet::TRT_ClockNoise::execute() {

  if (msgLvl(MSG::DEBUG)) msg() << "execute()" << endreq;

  // TRT helper
  const TRT_ID * m_trtHelper;
  if (detStore()->retrieve(m_trtHelper, "TRT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endreq;
    return StatusCode::FAILURE;
  }

  const DataVector<Trk::Track> *trkCollection = 0;
  StatusCode sc = evtStore()->retrieve( trkCollection, "Tracks" );
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Could not find Tracks Collection" << endreq;
    return sc;  
  }
  assert( trkCollection );
  if ( trkCollection == 0 ) msg(MSG::ERROR) << "trkCollection == 0 " << endreq;

  sc = m_trtStrawNeighbourTool.retrieve() ;
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not find TRTStrawNeighbourTool " << endreq;
    return sc;
  }


  // vector to keep track of the identifiers for hits on track, so we can exclude them later
  std::vector<Identifier> associatedRDO;


  int nTracks = trkCollection->size();
  int countTracks = 0;
  if (msgLvl(MSG::VERBOSE)) msg() << "starting DataVector<Trk::Track> loop over nTracks = " << nTracks << endreq;
  // loop over all tracks 
  for ( DataVector<Trk::Track>::const_iterator trackIt = trkCollection->begin(); trackIt != trkCollection->end(); ++trackIt ) {

    assert( countTracks < nTracks ); ++countTracks; 
    
        
    if (msgLvl(MSG::VERBOSE)) msg() << "start DataVector<const Trk::TrackStateOnSurface>* trackStates " << endreq;
    const DataVector<const Trk::TrackStateOnSurface>* trackStates = (**trackIt).trackStateOnSurfaces();   
    if ( trackStates == 0 ) {
      if (msgLvl(MSG::WARNING)) msg() << "Trk::TrackStateOnSurface empty" << endreq;
      continue;
    } 

    if (msgLvl(MSG::VERBOSE)) msg() << "start DataVector<const Trk::TrackStateOnSurface>* trackStatesIt " << trackStates->size() << endreq;
    int countTrackStates(0); 

    
    // loop over all "trackStates", in principle one state is associated to each measurement + one more trackState for track parameters at the Interaction Point
    for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStatesIt = trackStates->begin(); trackStatesIt != trackStates->end(); ++trackStatesIt ) {
      
      if (msgLvl(MSG::VERBOSE)) msg() << "countTrackStates " << countTrackStates << endreq;
      assert( countTrackStates < (int) trackStates->size() ); ++countTrackStates; 
      assert( (*trackStatesIt) != 0 );
      if ( *trackStatesIt == 0 ) { if (msgLvl(MSG::WARNING)) msg() << "*trackStatesIt == 0" << endreq; }

      //CLHEP::Hep3Vector pos(0., 0., 0.);
      //CLHEP::Hep3Vector mom(0., 0., 0.);
      Amg::Vector3D pos(0., 0., 0.);
      Amg::Vector3D mom(0., 0., 0.);

      const Trk::TrackParameters* trackParam = (*trackStatesIt)->trackParameters();
      if ( trackParam != 0 ) {
	  
        pos = trackParam->position();
        mom = trackParam->momentum();
		
      } else { 
	  
	if (msgLvl(MSG::WARNING)) msg() << "(*trackStatesIt)->trackParameters() == NULL" << endreq; 
      }



      if ( (*trackStatesIt)->type(Trk::TrackStateOnSurface::Measurement) ) {
      
        // try to dynamic cast as SCT space points etc to count the number of silicon hits
        if (dynamic_cast<const InDet::SCT_ClusterOnTrack*>((*trackStatesIt)->measurementOnTrack())!=0) {
          continue;
        }
                
        if (dynamic_cast<const InDet::PixelClusterOnTrack*>((*trackStatesIt)->measurementOnTrack())!=0) {
          continue;
        }
        
        if (msgLvl(MSG::VERBOSE)) msg() << "drift Circle " << endreq;
        const InDet::TRT_DriftCircleOnTrack *driftCircleOnTrack = dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>( (*trackStatesIt)->measurementOnTrack() );
        if ( driftCircleOnTrack == 0 ) { 
//          if (msgLvl(MSG::WARNING)) msg() << "driftCircleOnTrack == 0" << endreq; 
          continue; 
        }
	//HepGeom::Point3D<double> hit = driftCircleOnTrack->globalPosition();
	Amg::Vector3D hit = driftCircleOnTrack->globalPosition();
        if (msgLvl(MSG::VERBOSE)) msg() << "driftCircleOnTrack global position " << hit << endreq;
	
        const InDet::TRT_DriftCircle *driftCircle = dynamic_cast<const InDet::TRT_DriftCircle*> (driftCircleOnTrack->prepRawData());
        if ( driftCircle == 0 ) { 
//          if (msgLvl(MSG::WARNING)) msg() << "driftCircle == 0" << endreq; 
          continue; 
        }
        if (msgLvl(MSG::VERBOSE)) msg() << "track measurement trail. edge " << driftCircle->trailingEdge() << " " 
	       << ((int) driftCircle->driftTimeValid()) 
	       << " Base valid " << ( (driftCircleOnTrack->detectorElement() == 0) ? 0 : 1 ) << endreq;  
	    
// add this drift circle to me vector of Identifiers from hits on track
	const std::vector<Identifier> &listRDO = driftCircle->rdoList();
    //         log << MSG::INFO << "test RDO " << listRDO.size() << endreq;
	for (unsigned int i=0; i<listRDO.size(); ++i) associatedRDO.push_back(listRDO[i]);
    
      }  // end if(measurement)
    } // end loop over trackstates
  } // end loop over tracks

  
// --------------------------------------------------------
// loop over RDO hits, record hits NOT on track:


  const TRT_RDO_Container* rdoContainer;
  sc = evtStore()->retrieve(rdoContainer, "TRT_RDOs");
  if ( sc.isFailure() ) {

    msg(MSG::ERROR) << "no TRT_RDOs container available " << endreq;
    return sc;
  }

  for (TRT_RDO_Container::const_iterator rdoIt = rdoContainer->begin(); rdoIt != rdoContainer->end(); ++rdoIt) {

    const InDetRawDataCollection<TRT_RDORawData>* TRTCollection(*rdoIt);
    if (TRTCollection==0) continue;

    for (DataVector<TRT_RDORawData>::const_iterator trtIt = TRTCollection->begin(); trtIt != TRTCollection->end(); ++trtIt) {

      Identifier elID = (*trtIt)->identify();
      
      // check whether this is a dead straw -- will return 1 if dead
      if (m_TRTStrawStatusSummarySvc->get_status(elID) == 1) continue;
      
      // use std::find to see if this identifier is in our vector of hits on track
      //   if we find the identifier, skip this hit
      //   if the identifier is not found, std::find returns associatedRDO.end()
      if (std::find(associatedRDO.begin(),associatedRDO.end(),elID) != associatedRDO.end()) continue;

      m_trtStrawMap->setNewIdentifier(elID);
      
      // note that valid leading edge bins are 1-23
      // 0 is not a valid bin, and instead this method will return 24
      int leadingEdgeBin = (*trtIt)->driftTimeBin();      
      int side           = m_trtHelper->barrel_ec(elID);  
      int layer          = m_trtHelper->layer_or_wheel(elID);
      int phi            = m_trtHelper->phi_module(elID);
      int strawLayer     = m_trtHelper->straw_layer(elID);
      int myStrawNumber  = m_trtStrawMap->straw();
      int chip; m_trtStrawNeighbourTool->getChip(elID, chip);
      int pad; m_trtStrawNeighbourTool->getPad(elID, pad);
      int myBoard        = m_trtStrawMap->board();

      // need to correct chip numbers in barrel, not in endcap
      if(abs(side) == 1)    // barrel
        {
          // set chip number to correct scale 
          chip-=1; 
          if(layer == 1)      chip+=21;
          else if(layer == 2) chip+=54;
        } 
        
      if (myStrawNumber < 0 || myStrawNumber>=nStraw) { if (msgLvl(MSG::WARNING)) msg() << "invalid straw number !!! " << myStrawNumber << endreq; continue; }

      // ------ add hit information to array ------------
      int AorC = 0;
      if (side > 0) AorC = 1;
      
      // bin 24 is not valid, so don't consider those hits for calculating clock noise
      if (leadingEdgeBin < 24) {
        ++(m_straw_bin_occ[phi][AorC][myStrawNumber][leadingEdgeBin]);
      }
      
      if (m_print_ntuple) {
        // ------ store hit info in ntuple ----------------
      
        m_leadingEdgeBin     = leadingEdgeBin;
        m_board		     = myBoard;
        m_chip		     = chip;
        m_pad                = pad;
        m_side 		     = side;
        m_phi		     = phi;
        m_straw		     = myStrawNumber;
        m_strawLayer	     = strawLayer;
        m_layer		     = layer;

        m_ntuple->Fill();
      }
      
    }
  } // loop over RDO container
  
  // increment our internal event counter
  ++m_evtNum;

  if (msgLvl(MSG::DEBUG)) msg() << "end execute()" << endreq;
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode InDet::TRT_ClockNoise::finalize() {

  if (msgLvl(MSG::INFO)) msg() << "finalize()" << endreq;
  if (msgLvl(MSG::INFO)) msg() << "Calculating noise -- this takes a few seconds" << endreq;
  int numNoisyStraws = classify_noise();
  if (msgLvl(MSG::INFO)) msg() << numNoisyStraws << " straws showed clock noise, printed to file " << m_textfile << endreq;  
  if (m_print_ntuple) {
    if (msgLvl(MSG::INFO)) msg() << "occupancy information printed to ntuple" << endreq;  
  }
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// ----------------------------------------------------------------
//  clock noise definition is implemented here
int InDet::TRT_ClockNoise::clock_noise_cut(int totalocc, float asym) {

  if ((totalocc >= m_min_noise_hits) && (asym > m_min_asym))
    return 1;
  
  return -1;
}

// ----------------------------------------------------------------
int InDet::TRT_ClockNoise::classify_noise() {
  
  // open an output text file
  std::ofstream out;
  out.open(m_textfile.c_str());  
  
  int numNoisyStraws = 0;
  
  // loop over all straws and look at occupancy for clock noise
  for (int iphi=0; iphi < nPhi; ++iphi) {
    for (int iside=0; iside < nSide; ++iside) {
      for (int istraw = 0; istraw < nStraw; ++istraw) {
        
        // get the occupancy info for this straw
        int occ[24];
        int totalocc = 0;
        int occbins = 0;
        for (int ibin=0;ibin<24;++ibin) {
          occ[ibin] = m_straw_bin_occ[iphi][iside][istraw][ibin];
          totalocc += occ[ibin];
          if (occ[ibin] > 0) ++occbins;
        }
            
        // clock noise calculations:
        //   find the maximum asymmetry and corresponding phase for 40MHz and 80MHz
        //   then classify the type of noise being seen
        
        int phase40 = -1;
        float occ_above_thermal40 = 0;
        bool noisy40 = false;
        float asym40 = find_max_asymmetry(occ,PERIOD_40MHZ,&phase40,&occ_above_thermal40);
        // see if this passes our definition of clock noise
        if (clock_noise_cut(totalocc,asym40) > 0) noisy40 = true;

        
        int phase80 = -1;
        float occ_above_thermal80 = 0;
        bool noisy80 = false;
        float asym80 = find_max_asymmetry(occ,PERIOD_80MHZ,&phase80,&occ_above_thermal80);
        // see if this passes our definition of clock noise
        if (clock_noise_cut(totalocc,asym80) > 0) noisy80 = true;
        
        // use a discrete fourier transform to pick out frequency components of the noise
        float dft_thermal_comp = 0.;
        float dft_40mhz_comp = 0.;
        float dft_80mhz_comp = 0.;
        int dft_phase40 = -1;
        int dft_phase80 = -1;
        DFT(occ,&dft_thermal_comp,&dft_40mhz_comp,&dft_phase40,&dft_80mhz_comp,&dft_phase80);
        
        // the most interesting figures of merit from the DFT are:
        // - the ratio of the 40mhz component to the thermal component
        // - the ratio of the 80mhz component to the thermal component
        float dft_40mhz_ratio = 0.;
        float dft_80mhz_ratio = 0.;
        if (dft_thermal_comp > 0) {
          dft_40mhz_ratio = dft_40mhz_comp / dft_thermal_comp;
          dft_80mhz_ratio = dft_80mhz_comp / dft_thermal_comp;
        }
        
        if(noisy40 || noisy80) { 
        
          ++numNoisyStraws;
        
          // if this straw passed the cut, print it to a text file along with some relevant info                
          // text file format:
          //    phi | side | straw | frequency | phase | asymmetry | dft_phase | dft_ratio | occ_above_thermal | totalocc
          
          // make two entries if a straw shows both 40 and 80MHz noise
          
          if (noisy40) {
            out << std::setw(2) << iphi << std::setw(4) << recover_side(iside,istraw) 
                << std::setw(6) << istraw << "  40" << std::setw(4) << phase40
                << std::setw(12) << asym40
                << std::setw(4) << dft_phase40
                << std::setw(12) << dft_40mhz_ratio
                << std::setw(10) << int(occ_above_thermal40)
                << std::setw(10) << totalocc << std::endl;
          }
          
          if (noisy80) {
            out << std::setw(2) << iphi << std::setw(4) << recover_side(iside,istraw) 
                << std::setw(6) << istraw << "  80" << std::setw(4) << phase80
                << std::setw(12) << asym80
                << std::setw(4) << dft_phase80
                << std::setw(12) << dft_80mhz_ratio
                << std::setw(10) << int(occ_above_thermal80)
                << std::setw(10) << totalocc << std::endl;
          }
          
        } // noisy straw
                
      } // end straw loop
    
    } // end side loop
  
  } // end phi loop  
  
  out.close();
  
  return numNoisyStraws;
}

// ----------------------------------------------------------------
float InDet::TRT_ClockNoise::find_max_asymmetry(int occ[24], int period, int* phase, float* occ_above_thermal) {
  float max_asym = 0.;
  *phase = -1;
  *occ_above_thermal = 0;
  
  // loop over all possible phases and find the maximum asymmetry
  for (int p=0; p<period; ++p) {
    float temp_occ;
    float asym = find_asymmetry(occ,period,p,&temp_occ);
    if (asym > max_asym) {
      max_asym = asym;
      *phase = p;
      *occ_above_thermal = temp_occ;
    }
  }
    
  // if none of the asym values are > 0, the return values will be
  //  0. for asym and -1 for phase
  
  return max_asym;
}


// ----------------------------------------------------------------
float InDet::TRT_ClockNoise::find_asymmetry(int occ[24], int period, int phase, float* occ_above_thermal) {
  
  float sum_max = 0;
  float sum_min = 0;
  int numbins = 0;
  int numbins_to_sum = 24/period; // 6 for 80MHz, 3 for 40MHz
  float thermal_occ = 0;
  
  // sum up the occupancy from bins that should have the maximum number of hits
  //   these are assumed to be integer multiples of the period away from the bin
  //   with the max number of hits (which we know from the phase)
  //
  // note that bin 0 is not valid for leading edge, and there will never be any hits there
  //   so we weight the sum differently if bin 0 is one of the summed bins (below)
  for (int ibin=phase; ibin < 24; ibin += period) {
    sum_max += occ[ibin];
    ++numbins;
  }
  
  // check that we did the sum correctly
  if (numbins != numbins_to_sum) {
    if (msgLvl(MSG::WARNING)) msg() << "!! sum_max: only summed " << numbins << " bins for period " << period << endreq;
  }
  
  // do the same for the bins which should have the minimum number of hits
  //   these occur a half period away from the maximum
  //   this sum may need to wrap, depending on the phase
  int start_bin = -1;
  if ((phase - period/2) >= 0) {
    start_bin = phase - period/2;
  } else {
    start_bin = phase + period/2;
  }
  
  numbins = 0;
  for (int ibin = start_bin; ibin < 24; ibin += period) {
    sum_min += occ[ibin];
    ++numbins;
  }
  
  // check that we did the sum correctly
  if (numbins != numbins_to_sum) {
    if (msgLvl(MSG::WARNING)) msg() << "!! sum_min: only summed " << numbins << " bins for period " << period << endreq;
  }
  
  // find the thermal background:
  //  just use sum_min / (number of bins summed)
  thermal_occ = sum_min/float(numbins_to_sum);
  
  // weight the sums differently if one of them included bin 0
  if (phase == 0) { // sum_max includes bin 0
    sum_min *= float(numbins_to_sum - 1)/float(numbins_to_sum);
  } else if (start_bin == 0) { // sum_min includes bin 0
    sum_max *= float(numbins_to_sum - 1)/float(numbins_to_sum);
    thermal_occ = sum_min/float(numbins_to_sum - 1);
  }
  
  // now calculate the asymmetry
  float asym = (sum_max - sum_min) / (sum_max + sum_min);
    
  // calcuate the occupancy above the "thermal background"
  //  if a bin is below the "thermal background," ignore it
  *occ_above_thermal = 0;
  for (int ibin = 1; ibin<24; ++ibin) {
    float occ_extra = float(occ[ibin]) - thermal_occ;
    if (occ_extra > 0) {
      *occ_above_thermal += occ_extra;
    }
  }
    
  return asym;
}

// ----------------------------------------------------------------
int InDet::TRT_ClockNoise::DFT(int occ[24], float* comp0mhz, float* comp40mhz, int* phase40mhz, float* comp80mhz, int* phase80mhz) 
{
  Int_t N = 16;
  Double_t occf[16];
  Int_t offset = 4;
  for (int i=0; i<16; ++i) {
    occf[i] = float(occ[i+offset]);
  }

  // the transform returns 16 (complex valued) bins
  //  we're interested in the modulus of the following bins:
  //  bin 0: thermal component (no frequency)
  //  bin 2: 40Mhz component
  //  bin 4: 80Mhz component

//   // transform using ROOT class -- requires FFTW libraries to be installed with ROOT
//   //   which isn't always the case for the LCG installs
//   TVirtualFFT *dftr2c = TVirtualFFT::FFT(1, &N, "R2C");
//   dftr2c->SetPoints(occf);
//   dftr2c->Transform();
//   Double_t re, im;
//   dftr2c->GetPointComplex(0, re, im);
//   *comp0mhz = sqrt(re*re + im*im);
//   dftr2c->GetPointComplex(2, re, im);
//   *comp40mhz = sqrt(re*re + im*im);
//   *phase40mhz = find_dft_phase(re,im,PERIOD_40MHZ,offset);
//   dftr2c->GetPointComplex(4, re, im);
//   *comp80mhz = sqrt(re*re + im*im);
//   *phase80mhz = find_dft_phase(re,im,PERIOD_80MHZ,offset);
    
  // do the transform by hand -- had to resort to this because the ROOT install for LCG56a doesn't have
  //   the FFTW libraries ROOT needs installed ....
  TComplex* trans = new TComplex[N];
  for (int k = 0; k < N; ++k) {
    trans[k] = TComplex(0.,0.);
    for (int n = 0; n < N; ++n) {
      trans[k] += occf[n]*TComplex(1,-2*M_PI*k*n/N,true);
    }
  }
  *comp0mhz = TComplex::Abs(trans[0]);
  *comp40mhz = TComplex::Abs(trans[2]);
  *phase40mhz = find_dft_phase(trans[2].Re(),trans[2].Im(),PERIOD_40MHZ,offset);
  *comp80mhz = TComplex::Abs(trans[4]);
  *phase80mhz = find_dft_phase(trans[4].Re(),trans[4].Im(),PERIOD_80MHZ,offset);
 
  delete[] trans; 
 
  return 0;
}

// ----------------------------------------------------------------
int InDet::TRT_ClockNoise::find_dft_phase(float re, float im, int period, int offset)
{
  // determined this relationship by playing around with toy distributions
  int phase = int(floor((period*5/4 - ((period/4)*(((atan2(im,re) + M_PI) * 2/M_PI) - 1)))+0.5) + offset)%(period);
  
  return phase;
} 


// ----------------------------------------------------------------
int InDet::TRT_ClockNoise::recover_side(int AorC, int straw)
{
  if (AorC == 0) { // C side
    if (straw >= nStrawB) { // endcap C
      return -2;
    } else { // barrel C
      return -1;
    }
  } else { // A side
    if (straw < nStrawB) { // barrel A
      return 1;
    } else { // endcap A
      return 2;
    }
  }
  
  // should never get here  
  return 0;
}
