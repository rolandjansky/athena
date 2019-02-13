/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawChannelMonTool.h"
#include "LArOnlineIDStrHelper.h"

// --- athena ---
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODEventInfo/EventInfo.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnlID_Exception.h"
#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArRawChannelContainer.h"
//#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArRecEvent/LArEventBitInfo.h"
#include "AthenaKernel/Units.h"

#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHistFactory.h"


// --- boost ---
#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH


// --- stl ---
#include <algorithm>
#include <cfloat>
#include <climits>
#include <cmath>
#include <deque>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <stdexcept>
#include <typeinfo>
#include <utility>


namespace Units = Athena::Units;
using Units::nanosecond;
using Units::picosecond;
using Units::MeV;


using namespace LArMonTools;

using boost::lexical_cast;
using boost::bad_lexical_cast;
using boost::shared_ptr;
using namespace boost::assign;

typedef std::map<RawChHisto,boost::shared_ptr<IHistoProxyBase> > map_str_th2;
typedef std::map<IdentifierHash,map_str_th2> map_idhash_str_th2;
typedef std::map<Detector,map_idhash_str_th2> map_det_idhash_str_th2;

typedef std::vector<HWIdentifier>::const_iterator citer_vect_hwid;

//typedef std::pair<Detector,TH2*> pair_det_th2ptr;
typedef std::map<Detector,boost::shared_ptr<IHistoProxyBase> > map_det_th2ptr;
typedef map_det_th2ptr::const_iterator citer_det_th2ptr;

//typedef std::map<Detector, std::map<Sampling, std::deque<Region> > > det_region_map_t;
//typedef std::map<Sampling, std::deque<Region> > sam_region_map_t;


//static const double coarse_pi = 3.2;


/*----------------------------------------------------------------------------*/
LArRawChannelMonTool::LArRawChannelMonTool( const std::string & type,
					    const std::string & name,
					    const IInterface* parent )
  : ManagedMonitorToolBase(type, name, parent)
  , m_occupancy_thresholds( 8, 500. * MeV )
  , m_signal_thresholds(8, 500. * MeV )
  , m_pos_noise_thresholds( 8, 3 )
  , m_neg_noise_thresholds( 8, 3 )
  , m_bcid_signal_threshold( 500. * MeV )
  , m_time_threshold ( 5 )
  , m_quality_threshold( 4000 )
  , m_noise_burst_percent_thresholds( 8, 1. )
  , m_noise_burst_nChannel_thresholds( 8, 10 )
  , m_n_lumi_blocks( 1500 )
  , m_lar_online_id_ptr ( 0 )
  , m_calo_description_mgr_ptr( 0 )
  , m_calo_id_mgr_ptr( 0 )
  , m_cabel_service_tool ( "LArCablingLegacyService" )
  , m_masking_tool ( "BadLArRawChannelMask" )
  , m_atlas_ready( false )
  , m_lar_online_id_str_helper_ptr ( 0 )
  , m_has_lar_raw_channels( false )
    //  , _global_bcid_occupancy(0)
					  //  , _global_lar_eta_phi_acc( 0 )
  , m_interval( lowStat )
  , m_is_noise_event( false )
  , m_timeResolution( 30, 1 )
  , m_event_counter( 0 )
  , m_noise_stream_event_counter( 0 )
{

  declareProperty( "dataNameBase", m_data_name_base = "LArRawChannel" );
  declareProperty( "LArRawChannelContainerKey",
		   m_LArRawChannel_container_key = "LArRawChannels" );
  declareProperty( "calo_noise_tool", m_calo_noise_tool );
  declareProperty( "masking_tool",    m_masking_tool );
  declareProperty( "ATLASReadyFilterTool",    m_filterAtlasReady_tools );

  declareProperty( "occupancy_thresholds",     m_occupancy_thresholds );
  declareProperty( "signal_thresholds",        m_signal_thresholds );
  declareProperty( "pos_noise_thresholds",     m_pos_noise_thresholds );
  declareProperty( "neg_noise_thresholds",     m_neg_noise_thresholds );

  declareProperty( "bcid_signal_threshold",    m_bcid_signal_threshold = 500. * MeV );
  declareProperty( "time_threshold",           m_time_threshold        = 5 );
  declareProperty( "quality_threshold",        m_quality_threshold     = 4000 );
  declareProperty( "noise_threshold",          m_noise_threshold       = 3 );
  declareProperty( "noise_burst_percent_threshold", m_noise_burst_percent_thresholds );
  declareProperty( "noise_burst_nChannel_threshold", m_noise_burst_nChannel_thresholds );

  declareProperty( "UseElecNoiseOnly",        m_useElecNoiseOnly      = false );

  declareProperty( "monitor_occupancy",        m_monitor_occupancy      = false );
  declareProperty( "monitor_signal",           m_monitor_signal         = true );
  declareProperty( "monitor_positive_noise",   m_monitor_positive_noise = true );
  declareProperty( "monitor_negative_noise",   m_monitor_negative_noise = true);
  declareProperty( "monitor_time",             m_monitor_time           = true );
  declareProperty( "monitor_quality",          m_monitor_quality        = false );
  declareProperty( "monitor_burst",            m_monitor_burst          = true );

  declareProperty( "monitor_febs",             m_monitor_febs         = false );
  declareProperty( "monitor_feedthroughs",     m_monitor_feedthroughs = false );
  declareProperty( "monitor_detectors",        m_monitor_detectors    = true );

  declareProperty( "db_and_ofc_only",          m_db_and_ofc_only = true );

  declareProperty( "interval",                 m_interval_str        = "lowStat" );

  declareProperty( "n_lumi_blocks",      m_n_lumi_blocks = 1500 );

  declareProperty( "noise_streams",            m_noise_streams);

}


/*----------------------------------------------------------------------------*/
LArRawChannelMonTool::~LArRawChannelMonTool()
{
  delete m_lar_online_id_str_helper_ptr;
}

/*----------------------------------------------------------------------------*/
StatusCode LArRawChannelMonTool::initialize()
{
  ATH_MSG_DEBUG( "===> start " << name() << "::initialize <=== " );

  m_event_counter = 0;
  m_noise_stream_event_counter = 0;

  m_interval        = intervalStringToEnum( m_interval_str );

  // --- non-trivial ManagedMonitorToolBase intialize routine ---
  if ( ManagedMonitorToolBase::initialize().isSuccess() ) {

    ATH_MSG_DEBUG( "initialized ManagedMonitorToolBase" );

  } else {

    ATH_MSG_FATAL( "Unable to initialize ManagedMonitorToolBase"
		    );
    return StatusCode::FAILURE;

  }

  // --- get LArOnlineID ---
  if ( detStore()->retrieve( m_lar_online_id_ptr, "LArOnlineID" ).isSuccess() ) {

    ATH_MSG_DEBUG( "connected non-tool: LArOnlineID" );

  } else {

    ATH_MSG_FATAL( "unable to connect non-tool: LArOnlineID" );
    return StatusCode::FAILURE;

  }



  // --- get CaloDetDescrManager ---
  if ( detStore()->retrieve( m_calo_description_mgr_ptr ).isSuccess() ) {

    ATH_MSG_DEBUG( "connected non-tool: CaloDetDescrManager " );

  } else {

    ATH_MSG_FATAL( "unable to connect non-tool: CaloDetDescrMgr "
	);
    return StatusCode::FAILURE;

  }

  // --- get CaloIDManager ---
  if ( detStore()->retrieve( m_calo_id_mgr_ptr ).isSuccess() ) {

    ATH_MSG_DEBUG( "connected non-tool: CaloIdManager" );

  } else {

    ATH_MSG_FATAL( "unable to connect non-tool: CaloIdManager "
	);
    return StatusCode::FAILURE;

  }


  // --- get LArCablingService ---
  if ( !RetrieveHandle( m_cabel_service_tool ) ) return StatusCode::FAILURE;

  // --- get CaloNoiseTool ---
  if ( !RetrieveHandle( m_calo_noise_tool ) ) return StatusCode::FAILURE;

  // --- get bad channel mask ---
  if ( !RetrieveHandle( m_masking_tool ) ) return StatusCode::FAILURE;

   StatusCode sc = m_filterAtlasReady_tools.retrieve();
   if( !sc ) {
     ATH_MSG_ERROR("Could Not Retrieve AtlasFilterTool " << m_filterAtlasReady_tools);
     return StatusCode::FAILURE;
   }
  // ---
  // Get Michel's LArOnlineIDStrHelper: All names are Expert view
  m_lar_online_id_str_helper_ptr = new
    LArOnlineIDStrHelper( m_lar_online_id_ptr,LArOnlineIDStrHelper::LARONLINEID);
  if ( msgLvl(MSG::DEBUG) ) m_lar_online_id_str_helper_ptr->test();


  // ---
  // Create FEB -> Detector  and FEB -> Feedthrough maps
  Identifier::size_type feb_hash_max = m_lar_online_id_ptr->febHashMax();
  m_feb_hash_to_detector.resize( feb_hash_max );
  m_feb_hash_to_ft_hash.resize( feb_hash_max );


  citer_vect_hwid end_feb = m_lar_online_id_ptr->feb_end();
  for ( citer_vect_hwid itr_feb = m_lar_online_id_ptr->feb_begin();
	itr_feb != end_feb ; ++itr_feb) {

    HWIdentifier feb_id    = *itr_feb;
    HWIdentifier feedthrough_id = m_lar_online_id_ptr->feedthrough_Id( feb_id );

    IdentifierHash feb_hash = m_lar_online_id_ptr->feb_Hash( feb_id );
    IdentifierHash ft_hash  = m_lar_online_id_ptr->feedthrough_Hash( feedthrough_id );

    std::string ft_name          = m_lar_online_id_ptr->feedthrough_name( feb_id );

    Detector det(UNDEF);

    // Determine if the FEB is on the Airport's or Charlie's side
    if ( m_lar_online_id_ptr->pos_neg( *itr_feb ) ) {

      // Determine if the FEB is in: the EMB, EMEC, HEC, FCAL
      if     ( m_lar_online_id_ptr->isEMBchannel (*itr_feb) ) det = EMBA;
      else if( m_lar_online_id_ptr->isEMECchannel(*itr_feb) ) det = EMECA;
      else if( m_lar_online_id_ptr->isHECchannel (*itr_feb) ) det = HECA;
      else if( m_lar_online_id_ptr->isFCALchannel(*itr_feb) ) det = FCALA;

    } else {

      // Determine if the FEB is in: the EMB, EMEC, HEC, FCAL
      if     ( m_lar_online_id_ptr->isEMBchannel (*itr_feb) ) det = EMBC;
      else if( m_lar_online_id_ptr->isEMECchannel(*itr_feb) ) det = EMECC;
      else if( m_lar_online_id_ptr->isHECchannel (*itr_feb) ) det = HECC;
      else if( m_lar_online_id_ptr->isFCALchannel(*itr_feb) ) det = FCALC;

    }

    try {

      m_feb_hash_to_detector.at( feb_hash ) = det;
      m_feb_hash_to_ft_hash.at( feb_hash )  = ft_hash;

    } catch ( std::out_of_range &err ) {

      ATH_MSG_WARNING( "FEB hash out of range" << err.what() );

    }

  }

  // --- loop over channels to determine number of channels in each detector ---
  // (looping over all channels so this can easlily be done for FEBs and
  //  feedthroughs later ... )
  //m_det_to_nchannels = boost::assign::map_list_of(EMBA,0)(EMECA,0)(HECA,0)(FCALA,0)(EMBC,0)(EMECC,0)(HECC,0)(FCALC,0);
  m_det_to_nchannels[EMBA] = 0;
  m_det_to_nchannels[EMECA] = 0;
  m_det_to_nchannels[HECA] = 0;
  m_det_to_nchannels[FCALA] = 0;
  m_det_to_nchannels[EMBC] = 0;
  m_det_to_nchannels[EMECC] = 0;
  m_det_to_nchannels[HECC] = 0;
  m_det_to_nchannels[FCALC] = 0;

  citer_vect_hwid channel_end = m_lar_online_id_ptr->channel_end();
  for ( citer_vect_hwid channel_itr = m_lar_online_id_ptr->channel_begin();
	channel_itr != channel_end ; ++channel_itr ) {

    // fixme: skip masked and disconnected channels

    Detector det(UNDEF);

    // Determine if the FEB is on the Airport's or Charlie's side
    if ( m_lar_online_id_ptr->pos_neg( *channel_itr ) ) {

      // Determine if the FEB is in: the EMB, EMEC, HEC, FCAL
      if     ( m_lar_online_id_ptr->isEMBchannel (*channel_itr) ) det = EMBA;
      else if( m_lar_online_id_ptr->isEMECchannel(*channel_itr) ) det = EMECA;
      else if( m_lar_online_id_ptr->isHECchannel (*channel_itr) ) det = HECA;
      else if( m_lar_online_id_ptr->isFCALchannel(*channel_itr) ) det = FCALA;

    } else {

      // Determine if the FEB is in: the EMB, EMEC, HEC, FCAL
      if     ( m_lar_online_id_ptr->isEMBchannel (*channel_itr) ) det = EMBC;
      else if( m_lar_online_id_ptr->isEMECchannel(*channel_itr) ) det = EMECC;
      else if( m_lar_online_id_ptr->isHECchannel (*channel_itr) ) det = HECC;
      else if( m_lar_online_id_ptr->isFCALchannel(*channel_itr) ) det = FCALC;

    }

    m_det_to_nchannels[det] += 1;

  }

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG( "Number of channels in detectors: " );
    typedef std::map<Detector,unsigned int> det_int_map_t;
    foreach( det_int_map_t::value_type i, m_det_to_nchannels){
      ATH_MSG_DEBUG( detector_str( i.first ) << " has " << i.second << "channels " );
    }
  }


  std::deque<Detector> detectors = list_of( EMBA )( EMBC )( EMECA )( EMECC )( HECA )( HECC )( FCALA )( FCALC );
  foreach( const Detector & det, detectors ){
    m_selectionContext[det].quality( m_quality_threshold );
  }


  m_noise_streams_set.insert(m_noise_streams.begin(),m_noise_streams.end());
  ATH_MSG_DEBUG( "===> end " << name() << "::initialize <=== " );

  return StatusCode::SUCCESS;

}


/*----------------------------------------------------------------------------*/
StatusCode LArRawChannelMonTool::bookHistograms()
{

  ATH_MSG_DEBUG( "===> start " << name() << "::bookHistograms <=== " );

  std::deque<Detector> detectors = list_of( EMBA )( EMBC )( EMECA )( EMECC )( HECA )( HECC )( FCALA )( FCALC );

  // --- determine whether new histograms need booking ---
  bool is_new_interval = true;
//  switch ( m_interval ) {
//
//  case eventsBlock:
//    is_new_interval = isNewEventsBlock;
//    break;
//  case lumiBlock:
//    is_new_interval = isNewLumiBlock;
//    break;
//  case lowStat:
//    is_new_interval = newLowStatInterval;
//    break;
//  case medStat:
//    is_new_interval = newMedStatInterval;
//    break;
//  case higStat:
//    is_new_interval = newHigStatInterval;
//    break;
//  case run:
//    is_new_interval = isNewRun;
//    break;
//  case fill:
//  case file:
//  case all:
//  default:
//    ATH_MSG_WARNING( "given bad interval job-option: " << m_interval_str
//     << " ... booking histograms per run ..." );
//      is_new_interval = isNewRun;
//  }
//
//
//  // --- book histograms ---
  if ( is_new_interval ) {

    // --- exit if no LArRawChannelContainer exists ---
    m_has_lar_raw_channels = evtStore()->contains<LArRawChannelContainer>(m_LArRawChannel_container_key);
    if ( !m_has_lar_raw_channels ) return StatusCode::SUCCESS;

    // --- To remeber which ft have been booked in which detector ---
    map_det_idhash_str_th2 det_ft_type_hists;

    Identifier::size_type feb_hash_max = m_lar_online_id_ptr->febHashMax();

    // --- Per FEB Histograms ---
    m_per_feb_hists.clear();
    m_per_feb_hists[occ_h]       = std::vector<shared_ptr<IHistoProxyBase> >( feb_hash_max );
    m_per_feb_hists[sig_h]       = std::vector<shared_ptr<IHistoProxyBase> >( feb_hash_max );
    m_per_feb_hists[gain_h]      = std::vector<shared_ptr<IHistoProxyBase> >( feb_hash_max );
    m_per_feb_hists[pos_noise_h] = std::vector<shared_ptr<IHistoProxyBase> >( feb_hash_max );
    m_per_feb_hists[neg_noise_h] = std::vector<shared_ptr<IHistoProxyBase> >( feb_hash_max );
    m_per_feb_hists[time_h]      = std::vector<shared_ptr<IHistoProxyBase> >( feb_hash_max );
    m_per_feb_hists[quality_h]   = std::vector<shared_ptr<IHistoProxyBase> >( feb_hash_max );

    std::vector<shared_ptr<IHistoProxyBase> >& per_feb_occu = m_per_feb_hists[occ_h];
    std::vector<shared_ptr<IHistoProxyBase> >& per_feb_sign = m_per_feb_hists[sig_h];
    //std::vector<shared_ptr<IHistoProxyBase> >& per_feb_gain = m_per_feb_hists[gain_h];
    std::vector<shared_ptr<IHistoProxyBase> >& per_feb_posn = m_per_feb_hists[pos_noise_h];
    std::vector<shared_ptr<IHistoProxyBase> >& per_feb_negn = m_per_feb_hists[neg_noise_h];
    std::vector<shared_ptr<IHistoProxyBase> >& per_feb_time = m_per_feb_hists[time_h];
    std::vector<shared_ptr<IHistoProxyBase> >& per_feb_qual = m_per_feb_hists[quality_h];

    // --- Per Feedthrough Histograms ---
    m_per_feedthrough_hists.clear();
    m_per_feedthrough_hists[occ_h]       = std::vector<shared_ptr<IHistoProxyBase> >( feb_hash_max );
    m_per_feedthrough_hists[sig_h]       = std::vector<shared_ptr<IHistoProxyBase> >( feb_hash_max );
    m_per_feedthrough_hists[pos_noise_h] = std::vector<shared_ptr<IHistoProxyBase> >( feb_hash_max );
    m_per_feedthrough_hists[neg_noise_h] = std::vector<shared_ptr<IHistoProxyBase> >( feb_hash_max );
    m_per_feedthrough_hists[quality_h]   = std::vector<shared_ptr<IHistoProxyBase> >( feb_hash_max );

    std::vector<shared_ptr<IHistoProxyBase> >& per_ft_occu = m_per_feedthrough_hists[occ_h];
    std::vector<shared_ptr<IHistoProxyBase> >& per_ft_sign = m_per_feedthrough_hists[sig_h];
    std::vector<shared_ptr<IHistoProxyBase> >& per_ft_posn = m_per_feedthrough_hists[pos_noise_h];
    std::vector<shared_ptr<IHistoProxyBase> >& per_ft_negn = m_per_feedthrough_hists[neg_noise_h];
    std::vector<shared_ptr<IHistoProxyBase> >& per_ft_qual = m_per_feedthrough_hists[quality_h];

    // --- clear per detector maps ---
    m_per_detector_hists.clear();
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_occu = m_per_detector_hists[occ_h];
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_sign = m_per_detector_hists[sig_h];
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_gain = m_per_detector_hists[gain_h];
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_posn = m_per_detector_hists[pos_noise_h];
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_negn = m_per_detector_hists[neg_noise_h];
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_burs = m_per_detector_hists[burst_h];
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_timeVetoBurs = m_per_detector_hists[timeVetoBurst_h];
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_nfrac= m_per_detector_hists[noise_fraction_h];
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_nfracNeg= m_per_detector_hists[noise_fraction_Neg_h];
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_nfrac_W= m_per_detector_hists[noise_fraction_W_h];
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_nfracNoLArNoisyRO= m_per_detector_hists[noise_fraction_NoLArNoisyRO_h];
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_nfracTimeVetoLArNoisyRO= m_per_detector_hists[noise_fraction_TimeVetoLArNoisyRO_h];

    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_pede = m_per_detector_hists[pedestal_evolution_h];
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_sumE_BCID = m_per_detector_hists[sumE_vs_BCID_h];

    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_mfebt= m_per_detector_hists[mean_feb_time_h];
    //    map<Detector,shared_ptr<IHistoProxyBase> >& per_det_dtdet= m_per_detector_hists["det_time_diff"];// VB
    //    map<Detector,shared_ptr<IHistoProxyBase> >& per_det_lbdt = m_per_detector_hists["det_lb_time_diff"];// VB
    //    map<Detector,shared_ptr<IHistoProxyBase> >& per_det_lbvdt= m_per_detector_hists["lb_v_time_diff"];// VB

    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_qual = m_per_detector_hists[quality_h];
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_qnChan= m_per_detector_hists[quality_nChannel_h];
    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_qburs= m_per_detector_hists[quality_burst_h];
    //    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_nChan_qburs= m_per_detector_hists[quality_nChannel_burst_h];
    //    std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_mevbc= m_per_detector_hists[E_v_bc_h]; 
    
    citer_vect_hwid end_feb = m_lar_online_id_ptr->feb_end();



    // --- histogram factories ---
    std::map<Detector,std::map<RawChHisto,shared_ptr<LWHistFactoryBase> > > feb_histogram_factories;
    std::map<Detector,std::map<RawChHisto,shared_ptr<LWHistFactoryBase> > > ft_histogram_factories;
    std::map<Detector,std::map<RawChHisto,shared_ptr<LWHistFactoryBase> > > det_histogram_factories;
    shared_ptr<LWHistFactoryBase> factory_ptr;
    // --- FEB histogram factories ---
    if ( m_monitor_febs ) {

      foreach ( const Detector& det, detectors ) {

	if ( m_monitor_occupancy ) {
          std::string title = "Number" +
	    threshold_histogram_title_snippet(m_occupancy_thresholds.at(det))
	    + m_data_name_base + " in ";
	  factory_ptr.reset(new LWHist1DFactory<TH1I_LW>(title.c_str(),128,-0.5,127.5));
	  factory_ptr->SetXTitle("Channel");
	  feb_histogram_factories[det][occ_h] = factory_ptr;
	}

	if ( m_monitor_signal ) {
          std::string title= "Average Energy (MeV)" +
	    threshold_histogram_title_snippet( m_signal_thresholds.at(det) ) +
	    m_data_name_base + " in ";
          factory_ptr.reset( new LWHistProfileFactory(title.c_str(), 128, -0.5, 127.5) );
	  factory_ptr->SetXTitle("Channel");
	  factory_ptr->SetYTitle("Average Signal (MeV)");
          feb_histogram_factories[det][sig_h] = factory_ptr;
	}

	if ( m_monitor_positive_noise ) {
            std::string title= "Percentage of events above positive " +
	      threshold_histogram_title_snippet(m_pos_noise_thresholds.at(det))+
	      m_data_name_base + " in ";
	    factory_ptr.reset(new LWHist1DFactory<TH1F_LW>(title.c_str(),128, -0.5, 127.5));
	    factory_ptr->SetXTitle("Channel");
	    factory_ptr->SetYTitle("Percentage Accepted");
	    feb_histogram_factories[det][pos_noise_h] = factory_ptr;
	}

	if ( m_monitor_negative_noise ) {
          std::string title= "Percentage of events below negative " +
	    threshold_histogram_title_snippet(m_neg_noise_thresholds.at(det)) +
	    m_data_name_base + " in ";
	  factory_ptr.reset(new LWHist1DFactory<TH1F_LW>(title.c_str(),128, -0.5, 127.5));
	  factory_ptr->SetXTitle("Channel");
	  factory_ptr->SetYTitle("Percentage Accepted");
	  feb_histogram_factories[det][neg_noise_h] = factory_ptr;
	}

	if ( m_monitor_time ) {
          std::string title= "Time (ns)" +
	    threshold_histogram_title_snippet( m_time_threshold ) +
	    m_data_name_base + " in ";
	  factory_ptr.reset(new LWHist1DFactory<TH1F_LW>(title.c_str(),101, -50.5, 50.5));
	  factory_ptr->SetXTitle( "Time (ns)" );
	  feb_histogram_factories[det][time_h] = factory_ptr;
	}

	if ( m_monitor_quality ) {
	  std::string title = "Perceantage of events above " +
	    lexical_cast<std::string>( m_quality_threshold ) + " q-factor as" +
	    " function of " + m_data_name_base + " in ";
	  factory_ptr.reset(new LWHist1DFactory<TH1I_LW>(title.c_str(),128,-0.5,127.5));
	  factory_ptr->SetXTitle( "Quality Factor" );
	  feb_histogram_factories[det][quality_h] = factory_ptr;
	}
      }
    }

    if ( m_monitor_feedthroughs ) {
      foreach( const Detector &det, detectors ) {

	int n_slots        = n_slots_in_feedthroughs( det );
	double x_slots_max = double( n_slots ) + 0.5;

	if ( m_monitor_occupancy ) {
	  std::string title = "Number" +
	    threshold_histogram_title_snippet( m_occupancy_thresholds.at(det) ) +
	    m_data_name_base + " in ";
	  factory_ptr.reset(new LWHist2DFactory<TH2I_LW>(title.c_str(),128, -0.5, 127.5, n_slots, 0.5, x_slots_max));
	  factory_ptr->SetXTitle( "Channel" );
	  factory_ptr->SetYTitle( "Front End Board" );
	  ft_histogram_factories[det][occ_h] = factory_ptr;
	}

	if ( m_monitor_signal ) {
	  std::string title    = "Mean Energy (MeV)" +
	    threshold_histogram_title_snippet( m_signal_thresholds.at(det) ) +
	    m_data_name_base + " in ";
          factory_ptr.reset(new LWHistProfile2DFactory(title.c_str(),128, -0.5, 127.5, n_slots, 0.5, x_slots_max ));
          factory_ptr->SetXTitle("Channel");
          factory_ptr->SetYTitle("Front End Board");
          factory_ptr->SetZTitle("Average Signal (MeV)");
          ft_histogram_factories[det][sig_h] = factory_ptr;
	}

	if ( m_monitor_positive_noise ) {
	  std::string title    = "Percentage of events above " +
	    threshold_histogram_title_snippet( m_pos_noise_thresholds.at(det) ) +
	    m_data_name_base + " in ";
          factory_ptr.reset(new LWHist2DFactory<TH2F_LW>(title.c_str(),128, -0.5, 127.5, n_slots, 0.5, x_slots_max ));
	  factory_ptr->SetXTitle( "Channel" );
	  factory_ptr->SetYTitle( "Front End Board" );
	  factory_ptr->SetZTitle( "Percentage Accepted" );
	  ft_histogram_factories[det][pos_noise_h] = factory_ptr;
	}

	if ( m_monitor_negative_noise ) {
	  std::string title    = "Percentage of events below " +
	    threshold_histogram_title_snippet( m_neg_noise_thresholds.at(det) ) +
	    m_data_name_base + " in ";
	  factory_ptr.reset(new LWHist2DFactory<TH2F_LW>(title.c_str(), 128, -0.5, 127.5, n_slots, 0.5, x_slots_max ));
	  factory_ptr->SetXTitle( "Channel" );
	  factory_ptr->SetYTitle( "Front End Board" );
	  factory_ptr->SetZTitle( "Percentage Accepted" );
	  ft_histogram_factories[det][neg_noise_h] = factory_ptr;
	}

	if ( m_monitor_quality ) {
	  std::string title    = "Percentage of events above " +
	   lexical_cast<std::string>( m_quality_threshold ) + " q-factor as a function of " +
	    m_data_name_base + " in ";
          factory_ptr.reset(new LWHist2DFactory<TH2F_LW>(title.c_str(), 128, -0.5, 127.5, n_slots, 0.5, x_slots_max ));
          factory_ptr->SetXTitle( "Channel" );
          factory_ptr->SetYTitle( "Front End Board" );
          factory_ptr->SetZTitle( "Percentage Accepted" );
	  ft_histogram_factories[det][quality_h] = factory_ptr;
	}
      }
    }


    if ( m_monitor_detectors ) {

      foreach( Detector det, detectors ) {

	std::vector<double> axis = detector_superslot_axis(  det, m_lar_online_id_ptr );
	int n_bins = axis.size() - 1;

	if ( m_monitor_occupancy ) {
	  std::string title = "Number" + threshold_histogram_title_snippet( m_occupancy_thresholds.at(det) ) +
	    "FEB and " + m_data_name_base + " in " + detector_str( det );
          factory_ptr.reset(new LWHist2DFactory<TH2I_LW>(title.c_str(), n_bins, &axis[0], 128, -0.5, 127.5 ));
	  factory_ptr->SetXTitle("Halfcrate (+ increasing slot)");
	  factory_ptr->SetYTitle("Channel");
	  factory_ptr->setXAxisBinLabelsAllFebs(m_lar_online_id_str_helper_ptr,detector_str(det));
          det_histogram_factories[det][occ_h] = factory_ptr;
	}

	if ( m_monitor_signal ) {
	  std::string title = "Average Energy (MeV)" + threshold_histogram_title_snippet( m_signal_thresholds.at(det) ) +
	    "FEB and " + m_data_name_base + " in " + detector_str( det ) + " (no LArEventInfo::ERROR)";
          factory_ptr.reset(new LWHistProfile2DFactory(title.c_str(), n_bins, &axis[0],128, -0.5, 127.5));
	  factory_ptr->SetXTitle("Halfcrate (+ increasing slot)");
	  factory_ptr->SetYTitle("Channel");
	  factory_ptr->SetZTitle("Average Energy (MeV)");
	  factory_ptr->setXAxisBinLabelsAllFebs(m_lar_online_id_str_helper_ptr,detector_str(det));
	  det_histogram_factories[det][sig_h] = factory_ptr;

          title = "Average gain FEB and " + m_data_name_base + " in " + detector_str( det ) + " (no LArEventInfo::ERROR)";
          factory_ptr.reset(new LWHistProfile2DFactory(title.c_str(), n_bins, &axis[0],128, -0.5, 127.5));
          factory_ptr->SetXTitle("Halfcrate (+ increasing slot)");
          factory_ptr->SetYTitle("Channel");
          factory_ptr->SetZTitle("Average gain");
          factory_ptr->setXAxisBinLabelsAllFebs(m_lar_online_id_str_helper_ptr,detector_str(det));
          det_histogram_factories[det][gain_h] = factory_ptr;
	}

	if ( m_monitor_positive_noise ) {
	  std::string his_title = "Percentage of events above positive " +
	    threshold_histogram_title_snippet( m_pos_noise_thresholds.at(det) ) +
	    "FEB and " + m_data_name_base + " in " + detector_str( det ) + " (no LArEventInfo::ERROR)";
          factory_ptr.reset(new LWHistProfile2DFactory(his_title.c_str(), n_bins, &axis[0], 128, -0.5, 127.5 ));
	  factory_ptr->SetXTitle("Halfcrate (+ increasing slot)");
	  factory_ptr->SetYTitle("Channel");
	  factory_ptr->SetZTitle("Percentage Accepted");
	  factory_ptr->setXAxisBinLabelsAllFebs(m_lar_online_id_str_helper_ptr,detector_str(det));
          det_histogram_factories[det][pos_noise_h] = factory_ptr;
	}

	if ( m_monitor_negative_noise ) {
	  std::string his_title    = "Percentage of events below negative " +
	    threshold_histogram_title_snippet( m_neg_noise_thresholds.at(det) ) +
	    "FEB and " + m_data_name_base + " in " + detector_str( det ) + " (no LArEventInfo::ERROR)";
          factory_ptr.reset(new LWHistProfile2DFactory(his_title.c_str(),n_bins,&axis[0],128,-0.5,127.5 ));
	  factory_ptr->SetXTitle("Halfcrate (+ increasing slot)");
	  factory_ptr->SetYTitle("Channel");
	  factory_ptr->SetZTitle("Percentage Accepted");
	  factory_ptr->setXAxisBinLabelsAllFebs(m_lar_online_id_str_helper_ptr,detector_str(det));
          det_histogram_factories[det][neg_noise_h] = factory_ptr;
	}


	if ( m_monitor_burst){
	  //	if ( m_monitor_positive_noise ) {
	  std::string his_title    = "Yield of channels with +E > " + lexical_cast<std::string>( m_noise_threshold ) +" #sigma -"+ detector_str( det );
	  factory_ptr.reset(new LWHist1DFactory<TH1F_LW>(his_title.c_str(),375, 0., 7.5 ));
	  factory_ptr->SetXTitle("Percent of Channels");
	  factory_ptr->SetYTitle("Number of Events per 0.02 %");
	  det_histogram_factories[det][noise_fraction_h] = factory_ptr;
	}


	if ( m_monitor_burst){
	  //	if ( m_monitor_negative_noise ) {
	  std::string his_title    = "Yield of channels with -E > " + lexical_cast<std::string>( m_noise_threshold ) +" #sigma - " + detector_str( det );
	  factory_ptr.reset(new LWHist1DFactory<TH1F_LW>(his_title.c_str(),375, 0., 7.5 ));
	  factory_ptr->SetXTitle("Percent of Channels");
	  factory_ptr->SetYTitle("Number of Events per 0.02 %");
	  det_histogram_factories[det][noise_fraction_Neg_h] = factory_ptr;
	}

	// Noise fraction (Positive only) histogram when not flagged by LArNoisyROAlg_W 
	if ( m_monitor_burst){
	  //	if ( m_monitor_negative_noise ) {
	  std::string his_title    = "Yield of channels with +E > " + lexical_cast<std::string>( m_noise_threshold ) +" #sigma (no LArNoisyRO_StdOpt) -"+ detector_str( det );
	  factory_ptr.reset(new LWHist1DFactory<TH1F_LW>(his_title.c_str(),375, 0., 7.5 ));
	  factory_ptr->SetXTitle("Percent of Channels");
	  factory_ptr->SetYTitle("Number of Events per 0.02 %");
	  det_histogram_factories[det][noise_fraction_W_h] = factory_ptr;
	}


	// Noise fraction (Positive only) histogram when not flagged by LArNoisyROAlg
	if ( m_monitor_burst){
	  //	if ( m_monitor_positive_noise ) {
	  std::string his_title    = "Yield of channels with +E > " + lexical_cast<std::string>( m_noise_threshold ) +" #sigma (no LArNoisyRO_Std) -"+ detector_str( det );
	  factory_ptr.reset(new LWHist1DFactory<TH1F_LW>(his_title.c_str(),375, 0., 7.5 ));
	  factory_ptr->SetXTitle("Percent of Channels");
	  factory_ptr->SetYTitle("Number of Events per 0.02 %");
          det_histogram_factories[det][noise_fraction_NoLArNoisyRO_h] = factory_ptr;
	}

	// Noise fraction (Positive only) histogram when flagged by LArNoisyROAlg but without time bit set (time veto)
	if ( m_monitor_burst){
	  //	if ( m_monitor_positive_noise ) {
	  std::string his_title    = "Yield of channels with +E > " + lexical_cast<std::string>( m_noise_threshold ) +" #sigma (time vetoed) -"+ detector_str( det );
	  factory_ptr.reset(new LWHist1DFactory<TH1F_LW>(his_title.c_str(),375, 0., 7.5 ));
	  factory_ptr->SetXTitle("Percent of Channels");
	  factory_ptr->SetYTitle("Number of Events per 0.02 %");
          det_histogram_factories[det][noise_fraction_TimeVetoLArNoisyRO_h] = factory_ptr;
	}

	if ( m_monitor_signal ) {
	  std::string his_title    = "Energy sum (time vetoed) " + detector_str( det );
	  factory_ptr.reset(new LWHistProfileFactory(his_title.c_str(),m_n_lumi_blocks, 0.5, double(m_n_lumi_blocks)+0.5 ));
	  factory_ptr->SetXTitle("Luminosity Block");
	  factory_ptr->SetYTitle("Mean total energy(MeV)");
          det_histogram_factories[det][pedestal_evolution_h] = factory_ptr;
	}

	if ( m_monitor_burst ) {
	  std::string his_title    = "# of Events With Y_{3#sigma}>" + std::string( Form( "%.2f", m_noise_burst_percent_thresholds[det]) ) + " % - " + detector_str(det);
	  //	  TH1F graft( ("det_burst_graft"+detector_str(det)).c_str(), his_title.c_str(), 1440, -0.5, 1439.5 );
          factory_ptr.reset(new LWHist1DFactory<TH1F_LW>(his_title.c_str(),m_n_lumi_blocks, 0.5, double(m_n_lumi_blocks)+0.5));
          factory_ptr->SetXTitle("Luminosity Block");
          factory_ptr->SetYTitle("Number of Events per lumi block");
	  det_histogram_factories[det][burst_h] = factory_ptr;
	}

	// With Time Veto
	if ( m_monitor_burst ) {
	  std::string his_title    = "# of Events With Y_{3#sigma}>" + std::string( Form( "%.2f", m_noise_burst_percent_thresholds[det]) ) + " % (time vetoed)- " + detector_str(det);
	  //	  TH1F graft( ("det_burst_graft"+detector_str(det)).c_str(), his_title.c_str(), 1440, -0.5, 1439.5 );
          factory_ptr.reset(new LWHist1DFactory<TH1F_LW>(his_title.c_str(),m_n_lumi_blocks, 0.5, double(m_n_lumi_blocks)+0.5));
          factory_ptr->SetXTitle("Luminosity Block");
          factory_ptr->SetYTitle("Number of Events per lumi block");
	  det_histogram_factories[det][timeVetoBurst_h] = factory_ptr;
	}


	if ( m_monitor_time ) {
	  std::string his_title    = "Average time of " + m_data_name_base +
	    " in each FEB of the " + detector_str(det) + " Reporting E > " +
	    lexical_cast<std::string>( m_time_threshold ) + " #sigma";
          factory_ptr.reset(new LWHist1DFactory<TH1F_LW>(his_title.c_str(),101, -50.5, 50.5));
          factory_ptr->SetXTitle("<t_{FEB}> - <t_{event}> (ns)");
          factory_ptr->SetYTitle("Number of FEBs per ns");
	  det_histogram_factories[det][mean_feb_time_h] = factory_ptr;
	}

	if ( m_monitor_quality ) {
	  std::string his_title    = "Percentage of events above " +
	    lexical_cast<std::string>( m_quality_threshold ) +" q-factor as function"+
	    " of FEB and " + m_data_name_base + " in " + detector_str( det ) + " (no LArEventInfo::ERROR)";
          factory_ptr.reset(new LWHistProfile2DFactory(his_title.c_str(),n_bins,&axis[0],128,-0.5,127.5 ));
	  factory_ptr->SetXTitle("Halfcrate (+ increasing slot)");
	  factory_ptr->SetYTitle("Channel");
	  factory_ptr->SetZTitle("Percentage Accepted");
	  factory_ptr->setXAxisBinLabelsAllFebs(m_lar_online_id_str_helper_ptr,detector_str(det));
          det_histogram_factories[det][quality_h] = factory_ptr;
	}

	if ( m_monitor_quality ) {
	  std::string his_title    = "Number of Channels in the " + detector_str( det ) +
	    " with q-factor > " + lexical_cast<std::string>( m_quality_threshold );
          factory_ptr.reset(new LWHist1DFactory<TH1F_LW>(his_title.c_str(),50, -0.5, 49.5 ));
          factory_ptr->SetXTitle("Number of Channels");
          factory_ptr->SetYTitle("Number of Events per channel");
	  det_histogram_factories[det][quality_nChannel_h] = factory_ptr;
	}


	if ( m_monitor_quality ) {
	  std::string his_title    = "Number Of Events With More Than " +
	    std::string( Form( "%.2f", m_noise_burst_percent_thresholds[det]) ) +
	    " % Of All Channels In The " + detector_str(det) + " Reporting q-factor > " +
	    lexical_cast<std::string>( m_quality_threshold );
          factory_ptr.reset(new LWHist1DFactory<TH1F_LW>(his_title.c_str(),m_n_lumi_blocks, 0.5, double(m_n_lumi_blocks)+0.5));
          factory_ptr->SetXTitle("Luminosity Block");
          factory_ptr->SetYTitle("Number of Events per lumi block");
          det_histogram_factories[det][quality_burst_h] = factory_ptr;
	}

	if ( m_monitor_signal ) {
	  std::string his_title    = "Energy sum per bunch crossing - " + detector_str( det );
	  factory_ptr.reset(new LWHistProfileFactory(his_title.c_str(),3564, 0.5, 3564.5 ));
	  factory_ptr->SetXTitle("Bunch Crossing Number");
	  factory_ptr->SetYTitle("Mean total energy (MeV)");
          det_histogram_factories[det][sumE_vs_BCID_h] = factory_ptr;
	}	

      }

    }


    for ( citer_vect_hwid itr_feb = m_lar_online_id_ptr->feb_begin();
	  itr_feb != end_feb ; ++itr_feb) {

      try {

	HWIdentifier feedthrough_online_id = m_lar_online_id_ptr->feedthrough_Id(*itr_feb);
	IdentifierHash feb_hash            = m_lar_online_id_ptr->feb_Hash( *itr_feb );
	IdentifierHash ft_hash             = m_feb_hash_to_ft_hash.at( feb_hash );
	Detector feb_pos                   = m_feb_hash_to_detector.at( feb_hash );
        std::map<RawChHisto,shared_ptr<LWHistFactoryBase> > &the_feb_histo_factories = feb_histogram_factories[feb_pos];
        std::map<RawChHisto,shared_ptr<LWHistFactoryBase> > &the_ft_histo_factories  = ft_histogram_factories[feb_pos];

	std::string dir_name = m_path + "/" + detector_str( feb_pos );

	// --- Book and Register per FEB histograms ---
	if ( m_monitor_febs ) {

	  std::string feb_name = m_lar_online_id_str_helper_ptr->feb_str( *itr_feb );
	  std::string his_name;

	  if ( m_monitor_occupancy ) {
	    std::string dir_name_occ = dir_name + "/per_feb_occupancy";
	    his_name = detector_str( feb_pos ) + "Occupancy"+feb_name;
	    LWHist* histo = the_feb_histo_factories[occ_h]->create(his_name,feb_name);
	    if ( registerHistogram( histo, dir_name_occ ) )  
	      per_feb_occu.at( feb_hash ) = shared_ptr<IHistoProxyBase>( createLWHistProxy( histo ) );  
	    else return StatusCode::FAILURE;
	  }

	  if ( m_monitor_signal ) {
	    std::string dir_name_sig = dir_name + "/per_feb_signal";
	    his_name  = detector_str( feb_pos ) + "Signal" + feb_name + "_AVG";
	    LWHist* histo = ( the_feb_histo_factories[sig_h]->create(his_name,feb_name) );
	    if ( registerHistogram( histo,dir_name_sig ) )  
	      per_feb_sign.at( feb_hash ) = shared_ptr<IHistoProxyBase>( createLWHistProxy( histo ) );  
	    else return StatusCode::FAILURE;
	  }

	  if ( m_monitor_positive_noise ) {
	    std::string dir_name_acc = dir_name + "/per_feb_acceptance";
	    his_name  = detector_str( feb_pos ) + "Acceptance" + feb_name + "_AVG";
	    LWHist* histo = ( the_feb_histo_factories[pos_noise_h]->create(his_name,feb_name) );
	    if ( registerHistogram( histo, dir_name_acc, "effAsPerCent" ) )  
	      per_feb_posn.at( feb_hash ) = shared_ptr<IHistoProxyBase>( createLWHistProxy( histo ) );  
	    else return StatusCode::FAILURE;
	  }

	  if ( m_monitor_negative_noise ) {
	    std::string dir_name_acc = dir_name + "/per_feb_noise_acceptance";
	    his_name  = detector_str( feb_pos ) + "NoiseAcceptance" + feb_name + "_AVG";
	    LWHist* histo = ( the_feb_histo_factories[neg_noise_h]->create(his_name,feb_name) );

	    if ( registerHistogram( histo, dir_name_acc, "effAsPerCent" ) )  
              per_feb_negn.at( feb_hash ) = shared_ptr<IHistoProxyBase>( createLWHistProxy( histo ) );  
	    else return StatusCode::FAILURE;
	  }

	  if ( m_monitor_time ) {
	    std::string this_dir_name = dir_name + "/per_feb_time";
	    his_name  = detector_str( feb_pos ) + "Time" + feb_name;
	    LWHist* histo = ( the_feb_histo_factories[time_h]->create(his_name,feb_name) );
	    if ( registerHistogram( histo, this_dir_name ) )  
	      per_feb_time.at( feb_hash ) = shared_ptr<IHistoProxyBase>( createLWHistProxy( histo ) );  
	    else return StatusCode::FAILURE;
	  }

	  if ( m_monitor_quality ) {
	    std::string this_dir_name = dir_name + "/per_feb_quality";
	    his_name   = detector_str( feb_pos ) + "_quality_" + feb_name;
	    LWHist *histo = ( the_feb_histo_factories[quality_h]->create(his_name,feb_name) );
	    if ( registerHistogram( histo, this_dir_name ) )  
	      per_feb_qual.at( feb_hash ) = shared_ptr<IHistoProxyBase>( createLWHistProxy( histo ) );  
	    else return StatusCode::FAILURE;
	  }

	}

	// --- Book and register per Feedthrough Histograms ---
	if ( m_monitor_feedthroughs ) {

	  ATH_MSG_DEBUG( " - Booking per feedthrough histos" );

	  // --- Look for existing feedthough histogram ---
	  bool is_new_ft = false;
	  map_det_idhash_str_th2::const_iterator itr_det_ft_type_hists;
	  map_idhash_str_th2::const_iterator     itr_idhash_str_th2;

	  // --- Have feedthrough histograms been booked for this
	  // --- detector?
	  itr_det_ft_type_hists = det_ft_type_hists.find( feb_pos );
	  if ( itr_det_ft_type_hists == det_ft_type_hists.end() )
	    is_new_ft = true;

	  // --- If histograms exist for this detector, do they exist
	  // --- for this feedthrough in this detector?
	  if ( !is_new_ft ) {
	    itr_idhash_str_th2 = (itr_det_ft_type_hists->second).find( ft_hash );
	    if ( itr_idhash_str_th2 == (itr_det_ft_type_hists->second).end() )
	      is_new_ft = true;
	  }

	  // --- If histos already booked use same feedthrough histo ---
	  if ( !is_new_ft ) {

	    if ( m_monitor_occupancy ) {
	      per_ft_occu.at( feb_hash ) = det_ft_type_hists[feb_pos][ft_hash][occ_h];
	    }

	    if ( m_monitor_signal ) {
	      per_ft_sign.at( feb_hash ) = det_ft_type_hists[feb_pos][ft_hash][sig_h];
	    }

	    if ( m_monitor_positive_noise ) {
	      per_ft_posn.at( feb_hash ) = det_ft_type_hists[feb_pos][ft_hash][pos_noise_h];
	    }

	    if ( m_monitor_negative_noise ) {
	      per_ft_negn.at( feb_hash ) = det_ft_type_hists[feb_pos][ft_hash][neg_noise_h];
	    }

	    if ( m_monitor_quality ) {
	      per_ft_qual.at( feb_hash ) = det_ft_type_hists[feb_pos][ft_hash][quality_h];
	    }

	  } else {

	    std::string ft_name = m_lar_online_id_str_helper_ptr->feedthrough_str( *itr_feb );

	    // --- new occupancy histogram ---
	    if ( m_monitor_occupancy ) {
	      std::string the_dir_name = dir_name + "/per_ft_occupancy";
	      std::string his_name     = detector_str( feb_pos ) + "Occupancy" + ft_name;
	      LWHist2D* histo = static_cast<LWHist2D*>( the_ft_histo_factories[occ_h]->create(his_name,ft_name) );
	      m_lar_online_id_str_helper_ptr->setAxisBinLabelsFebs( histo->GetYaxis(), feedthrough_online_id );
	      if ( registerHistogram( histo, the_dir_name ) ) {
		shared_ptr<IHistoProxyBase> b( createLWHistProxy(histo) );
		per_ft_occu.at( feb_hash ) = b;
		det_ft_type_hists[feb_pos][ft_hash][occ_h] = b;
	      } else return StatusCode::FAILURE;
	    }

	    // --- new signal histogram ---
	    if ( m_monitor_signal ) {
	      std::string the_dir_name = dir_name + "/per_ft_signal";
	      std::string his_name     = detector_str( feb_pos ) + "Signal" + ft_name + "_AVG";
	      LWHist2D* histo = static_cast<LWHist2D*>( the_ft_histo_factories[sig_h]->create(his_name,ft_name) );
	      m_lar_online_id_str_helper_ptr->setAxisBinLabelsFebs( histo->GetYaxis(), feedthrough_online_id );
	      if ( registerHistogram( histo, the_dir_name ) ) {
		shared_ptr<IHistoProxyBase> b( createLWHistProxy(histo) );
		per_ft_sign.at( feb_hash ) = b;
		det_ft_type_hists[feb_pos][ft_hash][sig_h] = b;
	      } else return StatusCode::FAILURE;
	    }

	    // --- new acceptance histogram ---
	    if ( m_monitor_positive_noise ) {
	      std::string the_dir_name = dir_name + "/per_ft_acceptance";
	      std::string his_name     = detector_str( feb_pos ) + "Acceptance" + ft_name + "_AVG";
	      LWHist2D* histo = static_cast<LWHist2D*>( the_ft_histo_factories[pos_noise_h]->create(his_name,ft_name) );
	      m_lar_online_id_str_helper_ptr->setAxisBinLabelsFebs( histo->GetYaxis(), feedthrough_online_id );
	      if ( registerHistogram( histo, the_dir_name, "effAsPerCent" ) ) {
		shared_ptr<IHistoProxyBase> b( createLWHistProxy(histo) );
		per_ft_posn.at( feb_hash ) = b;
		det_ft_type_hists[feb_pos][ft_hash][pos_noise_h] = b;
	      } else return StatusCode::FAILURE;
	    }

	    // --- new per feedthrough noise acceptance ---
	    if ( m_monitor_negative_noise ) {

	      std::string the_dir_name = dir_name + "/per_ft_noise_acceptance";
	      std::string his_name     = detector_str( feb_pos ) + "NoiseAcceptance" + ft_name + "_AVG";
	      LWHist2D* histo = static_cast<LWHist2D*>( the_ft_histo_factories[neg_noise_h]->create(his_name,ft_name) );
	      m_lar_online_id_str_helper_ptr->setAxisBinLabelsFebs( histo->GetYaxis(), feedthrough_online_id );
	      if ( registerHistogram( histo, the_dir_name, "effAsPerCent" ) ) {
		shared_ptr<IHistoProxyBase> b( createLWHistProxy(histo) );
		per_ft_negn.at( feb_hash ) = b;
		det_ft_type_hists[feb_pos][ft_hash][neg_noise_h] = b;
	      } else return StatusCode::FAILURE;
	    }

	    // --- new per feedthrough quality acceptance ---
	    if ( m_monitor_quality ) {

	      std::string the_dir_name = dir_name + "/per_ft_quality";
	      std::string his_name     = detector_str( feb_pos ) + "Quality" + ft_name + "_AVG";
	      LWHist2D* histo = static_cast<LWHist2D*>( the_ft_histo_factories[quality_h]->create(his_name,ft_name) );
	      m_lar_online_id_str_helper_ptr->setAxisBinLabelsFebs( histo->GetYaxis(), feedthrough_online_id );
	      if ( registerHistogram( histo, the_dir_name, "effAsPerCent" ) ) {
		shared_ptr<IHistoProxyBase> b( createLWHistProxy(histo) );
		per_ft_qual.at( feb_hash ) = b;
		det_ft_type_hists[feb_pos][ft_hash][quality_h] = b;
	      } else return StatusCode::FAILURE;

	    }

	  } // --- end new feedthrough booking ---

	} // --- end feedthrough booking ---

      } catch ( std::out_of_range &err ){

	ATH_MSG_WARNING( "FEB hash out of range: " << err.what() );

      }

    } // --- end loop over FEBs ---

    // --- per detector histograms ---
    if ( m_monitor_detectors ) {

      // --- occupancy histograms ---
      foreach( Detector det, detectors ) {

        std::map<RawChHisto,shared_ptr<LWHistFactoryBase> > &the_det_histo_factories = det_histogram_factories[det];

	if ( m_monitor_occupancy ) {
	  std::string his_name  = detector_str( det ) + "_occupancy";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  LWHist2D* histo = static_cast<LWHist2D*>( the_det_histo_factories[occ_h]->create( his_name ) );
	  if ( registerHistogram( histo, dir_name ) ) {
// 	    pair_det_th2ptr dummy = make_pair(det, histo );
// 	    per_det_occu.insert( dummy );
	    per_det_occu[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;
	}

	if ( m_monitor_signal ) {
	  std::string his_name  = detector_str( det ) + "_signal" + "_AVG";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  TProfile2D_LW* histo = static_cast<TProfile2D_LW*>( the_det_histo_factories[sig_h]->create( his_name ) );
	  if ( registerHistogram( histo, dir_name ) ) {
// 	    pair_det_th2ptr dummy = make_pair(det, histo );
// 	    per_det_sign.insert( dummy );
	    per_det_sign[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;

          his_name  = detector_str( det ) + "_gain";
          dir_name  = m_path + "/" + detector_str( det );
          TProfile2D_LW* histog = static_cast<TProfile2D_LW*>( the_det_histo_factories[gain_h]->create( his_name ) );
          if ( registerHistogram( histog, dir_name ) ) {
//          pair_det_th2ptr dummy = make_pair(det, histo );
//          per_det_sign.insert( dummy );
            per_det_gain[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histog) );
          } else return StatusCode::FAILURE;
	}

	if ( m_monitor_positive_noise ) {
	  std::string his_name  = detector_str( det ) + "_acceptance" + "_AVG";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  TProfile2D_LW* histo = static_cast<TProfile2D_LW*>( the_det_histo_factories[pos_noise_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  //	  if ( registerHistogram( histo, dir_name, "", "effAsPerCent" ) ) {
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_posn[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;

	}

	if ( m_monitor_negative_noise ) {
	  std::string his_name  = detector_str( det ) + "_noise_acceptance" + "_AVG";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  TProfile2D_LW* histo = static_cast<TProfile2D_LW*>( the_det_histo_factories[neg_noise_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_negn[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;
	}


	if ( m_monitor_time ) {
	  std::string his_name  = detector_str( det ) + "_mean_feb_time";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  LWHist* histo = ( the_det_histo_factories[mean_feb_time_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_mfebt[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;
	}

	if ( m_monitor_quality ) {
	  std::string his_name  = detector_str( det ) + "_quality" + "_AVG";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  LWHist2D* histo = static_cast<LWHist2D*>( the_det_histo_factories[quality_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_qual[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;

	}

	if ( m_monitor_quality ) {
	  std::string his_name  = detector_str( det ) + "_quality_nChannel";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  LWHist* histo = ( the_det_histo_factories[quality_nChannel_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_qnChan[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;
	}


	if ( m_monitor_quality ) {
	  std::string his_name  = detector_str( det ) + "_quality_burst";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  LWHist* histo = ( the_det_histo_factories[quality_burst_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_qburs[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;  
	}

	// per_det_nfra
	if ( m_monitor_burst){
	  //	if ( m_monitor_positive_noise ) {
	  std::string his_name  = detector_str( det ) + "_noise_fraction";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  LWHist* histo = ( the_det_histo_factories[noise_fraction_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_nfrac[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;
	}

	if ( m_monitor_burst){
	  //	if ( m_monitor_negative_noise ) {
	  std::string his_name  = detector_str( det ) + "_noise_fraction_Neg";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  LWHist* histo = ( the_det_histo_factories[noise_fraction_Neg_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_nfracNeg[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;
	}


	if ( m_monitor_burst){
	  //	if ( m_monitor_positive_noise ) {
	  std::string his_name  = detector_str( det ) + "_noise_fraction_W";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  LWHist* histo = ( the_det_histo_factories[noise_fraction_W_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_nfrac_W[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;
	}
	


	// per_det_nfra (for positive energy only ) when Not flagged by LArNoisyROAlg
	if ( m_monitor_burst){
	  //	if ( m_monitor_positive_noise ) {
	  std::string his_name  = detector_str( det ) + "_noise_fraction_NoLArNoisyRO";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  LWHist* histo = ( the_det_histo_factories[noise_fraction_NoLArNoisyRO_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_nfracNoLArNoisyRO[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;
	}

	// per_det_nfra (for positive energy only ) when Not flagged by LArNoisyROAlg within time window of 500ms ( no LArEventInfo::ERROR)
	if ( m_monitor_burst){
	  //	if ( m_monitor_positive_noise ) {
	  std::string his_name  = detector_str( det ) + "_noise_fraction_TimeVetoLArNoisyRO";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  LWHist* histo = ( the_det_histo_factories[noise_fraction_TimeVetoLArNoisyRO_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_nfracTimeVetoLArNoisyRO[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;
	}

	// per detector pedestal evolution
	if ( m_monitor_signal ) {
	  std::string his_name  = detector_str( det ) + "_pedestal_evolution";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  LWHist* histo = ( the_det_histo_factories[pedestal_evolution_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_pede[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;
	}

	// per detector sumE vs BCID
	if ( m_monitor_signal ) {
	  std::string his_name  = detector_str( det ) + "_sumE_vs_BCID";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  LWHist* histo = ( the_det_histo_factories[sumE_vs_BCID_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_sumE_BCID[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;
	}

	if ( m_monitor_burst ) {
	  std::string his_name  = detector_str( det ) + "_burst";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  LWHist* histo = ( the_det_histo_factories[burst_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_burs[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;
	}

	  // With LArNoisyRO Time Veto
	if ( m_monitor_burst ) {
	  std::string his_name  = detector_str( det ) + "_timeVetoBurst";
	  std::string dir_name  = m_path + "/" + detector_str( det );
	  LWHist* histo = ( the_det_histo_factories[timeVetoBurst_h]->create( his_name ) );
	  //TK: REMOVED//histo->Sumw2();
	  if ( registerHistogram( histo, dir_name ) ) {
	    per_det_timeVetoBurs[det] = shared_ptr<IHistoProxyBase>( createLWHistProxy(histo) );
	  } else return StatusCode::FAILURE;
	}

      }

    } // --- end detector histograms ---

  } // --- end booking for monitoring interval ---

  ATH_MSG_DEBUG( "===> end " << name() << "::bookHistograms <=== " );

  return StatusCode::SUCCESS;

}


/*----------------------------------------------------------------------------*/
StatusCode LArRawChannelMonTool::fillHistograms()
{

  ATH_MSG_DEBUG( "===> start " << name() << "::fillHistograms boulou <=== " );


  // -- Set ATLAS Ready Filter
  setIsATLASReady();
  //  ATH_MSG_DEBUG( "Vikas Says: ATLAS READY Flag is set to " << isATLASReady() );

  // --- check set in bookHistogram ---
  if ( !m_has_lar_raw_channels ) return StatusCode::SUCCESS;

  // --- retrieve raw channels ---
  const LArRawChannelContainer *raw_channels = 0;
  if ( !evtStore()->retrieve( raw_channels, m_LArRawChannel_container_key ).isSuccess() ) {

    ATH_MSG_WARNING( "Cannot retrieve LArRawChannelContainer with key: "
	<< m_LArRawChannel_container_key );
    return StatusCode::FAILURE;

  }

  // --- retrieve event information ---
  const xAOD::EventInfo* event_info;
  uint32_t bunch_crossing_id = 0;
  uint32_t lumi_block        = 0;
  bool isEventFlaggedByLArNoisyROAlg = false; // keep default as false
  bool isEventFlaggedByLArNoisyROAlgInTimeW = false; // keep deault as false
  bool isEventFlaggedByLArNoisyROAlg_W =false; // keep deault as false
  //  double event_time_minutes = -1;
  if ( evtStore()->retrieve( event_info ).isSuccess()) {
   
    //ATH_MSG_DEBUG( "event_info->isEventFlagBitSet(xAOD::EventInfo::LAr,0"<<event_info->isEventFlagBitSet(xAOD::EventInfo::LAr,0) );
    // Check for LArNoisyROAlg event info
    //    if (event_info->errorState(EventInfo::LAr) == EventInfo::Warning) {
    if ( event_info->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::BADFEBS) ) {
      isEventFlaggedByLArNoisyROAlg = true;
      ATH_MSG_DEBUG( " !!! Noisy event found from LArNoisyROAlg !!!" );
    }

   
    if ( event_info->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::BADFEBS_W) ) {
      isEventFlaggedByLArNoisyROAlg_W = true;
      ATH_MSG_DEBUG( " !!! Noisy event found from LArNoisyROAlg_W !!!" );
    }
   

    if ( event_info->isEventFlagBitSet(xAOD::EventInfo::LAr,3) ) {
      isEventFlaggedByLArNoisyROAlgInTimeW = true;
      ATH_MSG_DEBUG( " !!! Noisy event found by LArNoisyROAlg in Time window of 500ms!!!" );
    }


    // bunch crossing ID:
    bunch_crossing_id = event_info->bcid();

    // luminosity block number
    lumi_block = event_info->lumiBlock();

    // event time (GMT). note:
    //  - time_stamp() returns Unix time (No of seconds since 1/1/1970)
    //  - 86400s in a day
    //    event_time_minutes = (event_info->event_ID()->time_stamp()%86400) / 60.;



    if (m_noise_streams_set.size() == 0) m_is_noise_event=true;
    const std::vector<xAOD::EventInfo::StreamTag>& event_stream_tags=event_info->streamTags();
    for (const xAOD::EventInfo::StreamTag& stream_tag: event_stream_tags) {
      if (m_noise_streams_set.find(stream_tag.name())!=m_noise_streams_set.end()) {
	m_is_noise_event = true;
	break;
      }
    }
    
    /* 
    // Added by Louis Stream Aware info...
    const TriggerInfo* trig;
    trig = event_info->triggerInfo();
    if( trig ) {
      m_is_noise_event = m_noise_streams.size() == 0;
      //Loop on the JO defined streams
      std::vector<TriggerInfo::StreamTag> event_stream_tags = trig->streamTags();
      foreach( const std::string & stream_name, m_noise_streams ) {
	foreach( const TriggerInfo::StreamTag stream_tag, event_stream_tags ) {
	  ATH_MSG_DEBUG( "Keeping Stream Tag: " << stream_tag.type() << "_" << stream_tag.name()  );
	  if( stream_name == stream_tag.name()) {
	    m_is_noise_event = true;
	    break;
	  }
	}
      }
    }
    */
  } else ATH_MSG_DEBUG( "Cannot retrieve EventInfo" );


  ++m_event_counter;
  if ( m_is_noise_event ) ++m_noise_stream_event_counter;

  std::vector<shared_ptr<IHistoProxyBase> >& per_feb_occu = m_per_feb_hists[occ_h];
  std::vector<shared_ptr<IHistoProxyBase> >& per_feb_sign = m_per_feb_hists[sig_h];
  std::vector<shared_ptr<IHistoProxyBase> >& per_feb_posn = m_per_feb_hists[pos_noise_h];
  std::vector<shared_ptr<IHistoProxyBase> >& per_feb_negn = m_per_feb_hists[neg_noise_h];
  std::vector<shared_ptr<IHistoProxyBase> >& per_feb_time = m_per_feb_hists[time_h];
  std::vector<shared_ptr<IHistoProxyBase> >& per_feb_qual = m_per_feb_hists[quality_h];

  std::vector<shared_ptr<IHistoProxyBase> >& per_ft_occu = m_per_feedthrough_hists[occ_h];
  std::vector<shared_ptr<IHistoProxyBase> >& per_ft_sign = m_per_feedthrough_hists[sig_h];
  std::vector<shared_ptr<IHistoProxyBase> >& per_ft_posn = m_per_feedthrough_hists[pos_noise_h];
  std::vector<shared_ptr<IHistoProxyBase> >& per_ft_negn = m_per_feedthrough_hists[neg_noise_h];
  std::vector<shared_ptr<IHistoProxyBase> >& per_ft_qual = m_per_feedthrough_hists[quality_h];

  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_occu = m_per_detector_hists[occ_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_sign = m_per_detector_hists[sig_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_gain = m_per_detector_hists[gain_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_posn = m_per_detector_hists[pos_noise_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_negn = m_per_detector_hists[neg_noise_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_qual = m_per_detector_hists[quality_h];
  //  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_mfebt= m_per_detector_hists[mean_feb_time_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_qnChan= m_per_detector_hists[quality_nChannel_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_qburs= m_per_detector_hists[quality_burst_h];
  //  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_nChan_qburs= m_per_detector_hists[quality_nChannel_burst_h];
  //  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_mevbc= m_per_detector_hists[E_v_bc_h]; 
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_nfrac= m_per_detector_hists[noise_fraction_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_nfracNeg= m_per_detector_hists[noise_fraction_Neg_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_nfrac_W= m_per_detector_hists[noise_fraction_W_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_nfracNoLArNoisyRO= m_per_detector_hists[noise_fraction_NoLArNoisyRO_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_nfracTimeVetoLArNoisyRO= m_per_detector_hists[noise_fraction_TimeVetoLArNoisyRO_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_burs = m_per_detector_hists[burst_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_timeVetoBurs = m_per_detector_hists[timeVetoBurst_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_pede = m_per_detector_hists[pedestal_evolution_h];
  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_sumE_BCID = m_per_detector_hists[sumE_vs_BCID_h];

  shared_ptr<IHistoProxyBase> perdethist_occu; 
  shared_ptr<IHistoProxyBase> perdethist_sign; 
  shared_ptr<IHistoProxyBase> perdethist_gain;
  shared_ptr<IHistoProxyBase> perdethist_posn; 
  shared_ptr<IHistoProxyBase> perdethist_negn; 
  shared_ptr<IHistoProxyBase> perdethist_qual;
  //  shared_ptr<IHistoProxyBase> perdethist_mevbc; 
  double detthreshold_occupancy(0);
  double detthreshold_signal(0);
  double detthreshold_pos_noise(0);
  double detthreshold_neg_noise(0);

  //  TProfile2D_LW* perdetsamplinghist_eta_phi_signal(0);
  //  TH1I_LW* perdetsamplinghist_phi_occupancy(0);
  //  TH1I_LW* perdetsamplinghist_eta_occupancy(0);

  //  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_dtdet= m_per_detector_hists["det_time_diff"];//VB
  //  std::map<Detector,shared_ptr<IHistoProxyBase> >& per_det_lbdt = m_per_detector_hists["det_lb_time_diff"];// VB

  //  int channels_above_bcid_threshold = 0;

  std::map<Detector,unsigned int> det_n_noisy_channels =
    boost::assign::map_list_of(EMBA,0)(EMECA,0)(HECA,0)(FCALA,0)(EMBC,0)(EMECC,0)(HECC,0)(FCALC,0);

  std::map<Detector,unsigned int> det_n_noisy_channels_Neg =
    boost::assign::map_list_of(EMBA,0)(EMECA,0)(HECA,0)(FCALA,0)(EMBC,0)(EMECC,0)(HECC,0)(FCALC,0);

  std::map<Detector,unsigned int> det_n_possibly_problematic_channels =
    boost::assign::map_list_of(EMBA,0)(EMECA,0)(HECA,0)(FCALA,0)(EMBC,0)(EMECC,0)(HECC,0)(FCALC,0);

  MeanCalculator event_mean_time;
  std::map<Detector,MeanCalculator> mean_detector_times;
  std::map<Detector,std::map<HWIdentifier, MeanCalculator> > mean_feb_times;
  std::map<Detector,double> per_detector_total_energy =
    map_list_of(EMBA,0.)(EMECA,0.)(HECA,0.)(FCALA,0.)(EMBC,0.)(EMECC,0.)(HECC,0.)(FCALC,0.);


  Detector lastdet(UNDEF);
  //bool firstsampling=true;
  //Sampling lastsampling(presampler);

  // --- Loop over RawChannels ---
  foreach( const LArRawChannel &chan, *raw_channels ){

    // --- Channel Information ---
    HWIdentifier hardware_id = chan.hardwareID();

    Identifier offline_id;
    const CaloDetDescrElement *calo_element_ptr;
    HWIdentifier feb_id;
    IdentifierHash feb_hash;
    unsigned int number;
    int slot_number;
    int feedthrough_number;
    Detector det(UNDEF);
    //    Sampling sampling;

    float energy;
    float time;
    float noise;
    float significance;
    int gain;

    try {

      offline_id = m_cabel_service_tool->cnvToIdentifier( hardware_id );
      calo_element_ptr   = m_calo_description_mgr_ptr->get_element( offline_id );

      // --- skip unconnected channels ---
      if ( !calo_element_ptr ) continue;

      // --- skipp masked channels ---
      if ( m_masking_tool->cellShouldBeMasked( hardware_id ) ) continue;


      // --- monitor properly reconstructed channels only ---
      // --- provenance&0x00ff == 0x00a5 :
      //     raw channels from OFC iteration, all calib constants found in DB
      // --- provenance&0x1000 == 0x1000 :
      //     raw channels from DSP. If no constant loaded in DSP, energy==0
      if ( !( m_db_and_ofc_only ||
	      (chan.provenance()&0x00ff) == 0x00A5 ||
	      (chan.provenance()&0x1000) == 0x1000 ) ) continue;

      feb_id             = m_lar_online_id_ptr->feb_Id( hardware_id );
      feb_hash           = m_lar_online_id_ptr->feb_Hash( feb_id );

      det = m_feb_hash_to_detector.at( feb_hash );
      bool detchanged=(det!=lastdet);
      if (detchanged) {
	//There are ~200k cells, but we only change detector ~10
	//times. Thus the following is a way to cut down on the cpu
	//usage due to map lookups (even better would of course be to stop
	//using maps!).	
	shared_ptr<IHistoProxyBase> hnull((IHistoProxyBase*)(0));
	std::map<Detector,shared_ptr<IHistoProxyBase> >::iterator it;
	it = per_det_occu.find(det); perdethist_occu = ((it==per_det_occu.end())?hnull:it->second);
	it = per_det_sign.find(det); perdethist_sign = (it==per_det_sign.end()?hnull:it->second);
        it = per_det_gain.find(det); perdethist_gain = (it==per_det_gain.end()?hnull:it->second);
	it = per_det_posn.find(det); perdethist_posn = (it==per_det_posn.end()?hnull:it->second);
	it = per_det_negn.find(det); perdethist_negn = (it==per_det_negn.end()?hnull:it->second);
	it = per_det_qual.find(det); perdethist_qual = (it==per_det_qual.end()?hnull:it->second);
	//	it = per_det_mevbc.find(det); perdethist_mevbc = (it==per_det_mevbc.end()?hnull:it->second);
	detthreshold_occupancy = m_occupancy_thresholds.at( det );
	detthreshold_signal = m_signal_thresholds.at( det );
	detthreshold_pos_noise = m_pos_noise_thresholds.at( det );
	detthreshold_neg_noise = m_neg_noise_thresholds.at( det );
      }

      lastdet=det;
      //      sampling = static_cast<Sampling>( channel_sampling( det, offline_id ) );
      //if ( sampling == -1 ) continue; // skip channel if sampling is undetermined

      slot_number        = m_lar_online_id_ptr->slot( hardware_id );
      feedthrough_number = m_lar_online_id_ptr->feedthrough( hardware_id );

      number = m_lar_online_id_ptr->channel( hardware_id ) - 1;
      energy = chan.energy() * MeV ;        // energy is in fixed in MeV by DSP
      time   = chan.time()   * picosecond;  // time is fixed in ps by DSP
      gain   = chan.gain();
      //eta    = calo_element_ptr->eta();
      //phi    = calo_element_ptr->phi();
      // This noise get the value at HIGH gain
      //      noise  = m_calo_noise_tool->getNoise( calo_element_ptr, ICalorimeterNoiseTool::TOTALNOISE );
      // Get hold of noise (electronic + Pileup) at the gain the channel is configured to.
      noise  = m_calo_noise_tool->totalNoiseRMS( calo_element_ptr, chan.gain());
      if (m_useElecNoiseOnly)  noise  = m_calo_noise_tool->elecNoiseRMS( calo_element_ptr, chan.gain(), -1); // Setting NminBias to -1 as it is not set in ICaloNoiseTool by default

      /*
      if (noise1 != noise) {
	std::cout << "Vikas says: Detector and channel HWID are " << det << "   " << chan.hardwareID() << std::endl;
	std::cout << "Vikas says PROBLEM: noise1 - noise is " << noise1-noise << std::endl;
	std::cout << "Vikas says : noise and channel gain are " << noise << "  " << chan.gain()<< std::endl;
	std::cout << "Vikas says : Elec noise and pileup noise are " << m_calo_noise_tool->getNoise( calo_element_ptr, ICalorimeterNoiseTool::ELECTRONICNOISE ) 
		  << "  " <<  m_calo_noise_tool->getNoise( calo_element_ptr, ICalorimeterNoiseTool::PILEUPNOISE )<< std::endl;
	std::cout << "Vikas says : noise1 (gain 2-LOW), noise1 (gain 1-MED),  noise1 (gain 0-High ) are " << m_calo_noise_tool->totalNoiseRMS( calo_element_ptr, CaloGain::LARLOWGAIN)
		  << "  " << m_calo_noise_tool->totalNoiseRMS( calo_element_ptr, CaloGain::LARMEDIUMGAIN)
		  << "  " << m_calo_noise_tool->totalNoiseRMS( calo_element_ptr, CaloGain::LARHIGHGAIN ) << std::endl;
	
      }
      */
      significance = energy / noise ;
      //quality      = short( chan.quality() );

      if (detchanged) m_qualitySelector.selectionContext( &m_selectionContext[det] );
      
    } catch ( const LArOnlID_Exception &err ) {

      continue; // skip this channel

    } catch ( const std::out_of_range &err ) {

      ATH_MSG_WARNING( "FEB hash out of range. Detector undefined"
	  << err.what() );
      continue; // skip this channel

    } catch ( const LArID_Exception& err ) {

      ATH_MSG_WARNING( "channel offline id undefined ... skipping"
	  );
      continue; // skip this channel

    }

    // --- Fill per FEB Histograms ---
    if ( m_monitor_febs ) {

      try {

	if ( m_is_noise_event && m_monitor_occupancy && energy >  detthreshold_occupancy ) {
	  shared_ptr<IHistoProxyBase> const histo_cptr = per_feb_occu.at( feb_hash );
	  if ( histo_cptr ) histo_cptr->Fill( number );
	}

	if ( m_is_noise_event && m_monitor_signal && energy > detthreshold_signal ) {
	  shared_ptr<IHistoProxyBase> const histo_cptr = per_feb_sign.at( feb_hash );
	  if ( histo_cptr ) histo_cptr->Fill( number, energy );
	}

	if ( m_is_noise_event && m_monitor_positive_noise && significance > detthreshold_pos_noise ) {
	  shared_ptr<IHistoProxyBase> const histo_cptr = per_feb_posn.at( feb_hash );
	  if ( histo_cptr ) histo_cptr->Fill( number );
	}

	if ( m_is_noise_event && m_monitor_negative_noise && (-1.) * significance > detthreshold_neg_noise ) {
	  shared_ptr<IHistoProxyBase> const histo_cptr = per_feb_negn.at( feb_hash );
	  if ( histo_cptr ) histo_cptr->Fill( number );
	}

	if ( m_monitor_time && significance > m_time_threshold ) {
	  shared_ptr<IHistoProxyBase> const histo_cptr = per_feb_time.at( feb_hash );
	  if ( histo_cptr ) histo_cptr->Fill( time );
	}

	if ( m_monitor_quality && m_qualitySelector.Select( chan ) ) {
	  shared_ptr<IHistoProxyBase> const histo_cptr = per_feb_qual.at( feb_hash );
	  if ( histo_cptr ) histo_cptr->Fill( number );
	}

      } catch ( std::out_of_range &err ) {

	ATH_MSG_WARNING("failed per FEB monitoring "<< err.what() );

      } catch ( std::bad_cast &err ) {

	ATH_MSG_WARNING("failed per FEB monitoring "<< err.what() );

      }

    }


    // --- Fill per Feethrough Histograms ---
    if ( m_monitor_feedthroughs ) {

      try {

	if ( m_is_noise_event && m_monitor_occupancy && energy >  detthreshold_occupancy ) {
	  shared_ptr<IHistoProxyBase> histo = per_ft_occu.at( feb_hash );
	  if ( histo ) histo->Fill( number, slot_number );
	}

	if ( m_is_noise_event && m_monitor_signal && energy > detthreshold_signal ) {
	  shared_ptr<IHistoProxyBase> histo = per_ft_sign.at( feb_hash );
	  if ( histo ) histo->Fill( number, slot_number, energy );
	}

	if ( m_is_noise_event && m_monitor_positive_noise && significance > detthreshold_pos_noise ) {
	  shared_ptr<IHistoProxyBase>histo = per_ft_posn.at( feb_hash );
	  if ( histo ) histo->Fill( number, slot_number );
	}

	if ( m_is_noise_event && m_monitor_negative_noise && (-1.) * significance > detthreshold_neg_noise ) {
	  shared_ptr<IHistoProxyBase> const histo_cptr = per_ft_negn.at( feb_hash );
	  if ( histo_cptr ) histo_cptr->Fill( number, slot_number );
	}

	if ( m_monitor_quality && m_qualitySelector.Select( chan ) ) {
	  shared_ptr<IHistoProxyBase> const histo_cptr = per_ft_qual.at( feb_hash );
	  if ( histo_cptr ) histo_cptr->Fill( number, slot_number );
	}

      } catch ( std::out_of_range &err ) {

	ATH_MSG_WARNING( "failed per feedthrough monitoring " << err.what()
			  );

      }

    } // --- End fill per Feedthrough histograms ---


    // --- Fill per detector histograms ---
    if ( m_monitor_detectors ) {

      // --- Skip channel if detector is undefined ---
      if ( det == UNDEF ) continue;//TK: Only having this check inside
				   //the m_monitor_detectors block
				   //causes interference between the
				   //various options.

      try {

	// --- Calculate super-slot number ---
	const unsigned int n_slots =
	  n_slots_in_feedthroughs( det );
	const unsigned int super_slot_number =
	  feedthrough_number * n_slots + slot_number;

	// --- look up spare bin number ---
	int x_bin = super_slot_number;
	int y_bin = number + 1;


	// --- Fill occupancy histogram ---
	if ( m_is_noise_event && m_monitor_occupancy && energy > detthreshold_occupancy ) {
	  if (perdethist_occu) perdethist_occu->Fill( super_slot_number, number );
	}


	// --- fill signal histogram ---
	// Sep 30, 2011. Now with no LArEventInfo::ERROR and isATLASReady
	if ( m_is_noise_event && m_monitor_signal && energy > detthreshold_signal && isATLASReady() ) {
	  if (perdethist_sign && !isEventFlaggedByLArNoisyROAlgInTimeW) perdethist_sign->Fill( x_bin, y_bin, energy );
          if (perdethist_gain && !isEventFlaggedByLArNoisyROAlgInTimeW) perdethist_gain->Fill( x_bin, y_bin, gain );
	}



	// --- fill acceptance histogram ---
	// Sep 30, 2011. Now with no LArEventInfo::ERROR and isATLASReady
	if ( m_is_noise_event && m_monitor_positive_noise && significance > detthreshold_pos_noise && isATLASReady() && !isEventFlaggedByLArNoisyROAlgInTimeW) {
	  if (perdethist_posn) perdethist_posn->Fill( x_bin, y_bin, 100 );
	} else {
	  if (perdethist_posn) perdethist_posn->Fill( x_bin, y_bin, 0 );
	}

	// --- per detector noise acceptance ---
	// Sep 30, 2011. Now with no LArEventInfo::ERROR and isATLASReady
	if ( m_is_noise_event && m_monitor_negative_noise && (-1.) * significance > detthreshold_neg_noise && isATLASReady() && !isEventFlaggedByLArNoisyROAlgInTimeW) {
	  if (perdethist_negn) perdethist_negn->Fill( x_bin, y_bin, 100 );
	} else {
	  if (perdethist_negn) perdethist_negn->Fill( x_bin, y_bin, 0 );
	}

	// Sep 30, 2011. Now with no LArEventInfo::ERROR and isATLASReady
	if ( m_monitor_quality && m_qualitySelector.Select( chan ) && isATLASReady() && !isEventFlaggedByLArNoisyROAlgInTimeW) {
	  if (perdethist_qual) perdethist_qual->Fill( x_bin, y_bin, 100 );
	} else {
	  if (perdethist_qual) perdethist_qual->Fill( x_bin, y_bin, 0 );
	}

	/*
	// --- per detector mean energy vs bunch crossing ---  
	if ( m_monitor_signal ) {  
	  if(perdethist_mevbc) perdethist_mevbc->Fill( bunch_crossing_id, energy );
	} 
	*/ 


	//	if ( significance  > m_noise_threshold ) det_n_noisy_channels[det] += 1;
	if ( significance  > detthreshold_pos_noise ) det_n_noisy_channels[det] += 1;
	if ( (-1.) * significance  > detthreshold_neg_noise ) det_n_noisy_channels_Neg[det] += 1;
	if ( m_qualitySelector.Select( chan ) ) det_n_possibly_problematic_channels[det] += 1;
	if ( m_monitor_time && significance > m_time_threshold ) {
	  double res = m_timeResolution( significance );
	  double weight = res > 0. ? 1 / (res*res) : 0.;
	  event_mean_time.Add( time, weight );
	  mean_detector_times[det].Add( time, weight );
	  mean_feb_times[det][feb_id].Add( time, weight );
	}

	switch (det) {
	  
	case 0: 
	  per_detector_total_energy[EMBA] += energy; break;	  
	case 1: 
	  per_detector_total_energy[EMBC] += energy; break;	  
	case 2: 
	  per_detector_total_energy[EMECA] += energy; break;	  
	case 3: 
	  per_detector_total_energy[EMECC] += energy; break;	  
	case 4: 
	  per_detector_total_energy[HECA] += energy; break;	  
	case 5: 
	  per_detector_total_energy[HECC] += energy; break;	  
	case 6: 
	  per_detector_total_energy[FCALA] += energy; break;	  
	case 7: 
	  per_detector_total_energy[FCALC] += energy; break;	  
	default:
	  ATH_MSG_WARNING( "Something is wrong with choosing of detector partition " );	  
	}



      } catch ( const std::out_of_range &err ) {

	ATH_MSG_WARNING( "Failed monitoring " << err.what() );

      } catch ( const std::bad_cast &err ) {

	ATH_MSG_WARNING( "Failed monitoring " << err.what() );

      }

    } // --- end filling detector histograms

//    {// --- fill global lar histograms ---
//
//      //      if ( m_is_noise_event )
//      //	_global_lar_eta_phi_acc->Fill( eta, phi, (significance > detthreshold_neg_noise?1.:0.) );
//
//      if ( energy > m_bcid_signal_threshold ) channels_above_bcid_threshold++;
//
//    } // --- end global lar histograms ---

  } // --- End loop over LArRawChannels ---

  //  if ( bunch_crossing_id ) _global_bcid_occupancy->Fill( bunch_crossing_id, double(channels_above_bcid_threshold) );


  std::vector<Detector> detectors = list_of( EMBA )( EMBC )( EMECA )( EMECC )( HECA )( HECC )( FCALA )( FCALC );
  if ( m_monitor_burst ) {
    foreach( Detector det, detectors ) {

      double noisy_channel_percent =
	double(det_n_noisy_channels[det]) /
	double(m_det_to_nchannels[det]) * 100.;

      double noisy_channel_percent_Neg =
	double(det_n_noisy_channels_Neg[det]) /
	double(m_det_to_nchannels[det]) * 100.;

      citer_det_th2ptr hist_itr = per_det_burs.find( det );
      if ( noisy_channel_percent > m_noise_burst_percent_thresholds[det] && hist_itr != per_det_burs.end() ) {
	shared_ptr<IHistoProxyBase> histo = hist_itr->second;
	//	if ( histo ) histo->Fill( event_time_minutes );
	if ( histo ) histo->Fill( lumi_block );
      }

      hist_itr = per_det_timeVetoBurs.find( det );
      if ( noisy_channel_percent > m_noise_burst_percent_thresholds[det] && !isEventFlaggedByLArNoisyROAlgInTimeW && hist_itr != per_det_timeVetoBurs.end() ) {
	shared_ptr<IHistoProxyBase> histo = hist_itr->second;
	//	if ( histo ) histo->Fill( event_time_minutes );
	if ( histo ) histo->Fill( lumi_block );
      }

      // For positive energy
      hist_itr = per_det_nfrac.find( det );
      if ( hist_itr != per_det_nfrac.end() ) {
	shared_ptr<IHistoProxyBase> histo = hist_itr->second;
	if ( histo ) histo->Fill( noisy_channel_percent );
      }

      //For negative energy
      hist_itr = per_det_nfracNeg.find( det );
      if ( hist_itr != per_det_nfracNeg.end() ) {
	shared_ptr<IHistoProxyBase> histo = hist_itr->second;
	if ( histo ) histo->Fill( noisy_channel_percent_Neg );
      }
     
      // Weighted Flag 
      hist_itr = per_det_nfrac_W.find( det );
      if ( hist_itr != per_det_nfrac_W.end() ) {
	shared_ptr<IHistoProxyBase> histo = hist_itr->second;
	if ( histo && !isEventFlaggedByLArNoisyROAlg_W ) histo->Fill( noisy_channel_percent );
      }
      
      //New Plot. Fill when event is not flagged by LArNoisyROAlg
      hist_itr = per_det_nfracNoLArNoisyRO.find( det );
      if ( hist_itr != per_det_nfracNoLArNoisyRO.end() ) {
	shared_ptr<IHistoProxyBase> histo = hist_itr->second;
	if ( histo && !isEventFlaggedByLArNoisyROAlg) histo->Fill( noisy_channel_percent );
      }

      //New Plot. Fill when event is not flagged by LArNoisyROAlg within time window of 500ms (no LArEventInfo::ERROR)
      hist_itr = per_det_nfracTimeVetoLArNoisyRO.find( det );
      if ( hist_itr != per_det_nfracTimeVetoLArNoisyRO.end() ) {
	shared_ptr<IHistoProxyBase> histo = hist_itr->second;
	if ( histo && !isEventFlaggedByLArNoisyROAlgInTimeW) histo->Fill( noisy_channel_percent );
      }

      double possibly_problematic_channel_percent =
	double(det_n_possibly_problematic_channels[det]) /
	double(m_det_to_nchannels[det]) * 100.;

      hist_itr = per_det_qnChan.find( det );
      if ( hist_itr != per_det_qnChan.end() ) {
	shared_ptr<IHistoProxyBase> histo = hist_itr->second;
	if ( histo ) histo->Fill( det_n_possibly_problematic_channels[det] );
      }

      hist_itr = per_det_qburs.find( det );
      if ( possibly_problematic_channel_percent > m_noise_burst_percent_thresholds[det] &&
	   hist_itr != per_det_qburs.end() ) {
	shared_ptr<IHistoProxyBase> histo = hist_itr->second;
	//	if ( histo ) histo->Fill( event_time_minutes );
	if ( histo ) histo->Fill( lumi_block );
      }

      /*
      // Sep 30, 2011. Now with no LArEventInfo::ERROR
     hist_itr = per_det_nChan_qburs.find( det );
      if ( det_n_possibly_problematic_channels[det] > m_noise_burst_nChannel_thresholds[det] &&
	   hist_itr != per_det_nChan_qburs.end() ) {
	shared_ptr<IHistoProxyBase> histo = hist_itr->second;
	//	if ( histo ) histo->Fill( event_time_minutes );
	if ( histo && !isEventFlaggedByLArNoisyROAlgInTimeW) histo->Fill( lumi_block );
      }
      */

      // Sep 30, 2011. Now with no LArEventInfo::ERROR
      hist_itr = per_det_pede.find(det);
      if ( m_monitor_signal &&  hist_itr != per_det_pede.end() ) {
	shared_ptr<IHistoProxyBase> histo = hist_itr->second;
	if ( histo && !isEventFlaggedByLArNoisyROAlgInTimeW) histo->Fill( lumi_block ,per_detector_total_energy[det] );
      }

      hist_itr = per_det_sumE_BCID.find(det);
      if ( m_monitor_signal &&  hist_itr != per_det_sumE_BCID.end() ) {
	shared_ptr<IHistoProxyBase> histo = hist_itr->second;
	if ( histo ) histo->Fill( bunch_crossing_id ,per_detector_total_energy[det] );
      }

    }
  }


  if ( m_monitor_time ) {
    double t = event_mean_time.result();
    typedef std::map<HWIdentifier, MeanCalculator> element_mean_calculator;
    typedef std::map<Detector,element_mean_calculator > det_element_mean_calculator;
    foreach( const det_element_mean_calculator::value_type & a, mean_feb_times ) {
      const Detector & detector = a.first;
      foreach( const element_mean_calculator::value_type & b, a.second ) {
	const MeanCalculator &avg= b.second;
	m_per_detector_hists[mean_feb_time_h][detector]->Fill( (avg.result() - t) / nanosecond );
      }
    }
  }

  /*
  if ( m_monitor_time ) { // C-A side averge time plots// VB

    typedef std::map<Detector,MeanCalculator> det_mean_calculator;
    det_mean_calculator::const_iterator itr_end = mean_detector_times.end();
    if ( mean_detector_times.find(EMBA) != itr_end &&
	 mean_detector_times.find(EMBC) != itr_end ) {
      double dt = mean_detector_times[EMBA].result() - mean_detector_times[EMBC].result();
      //      per_det_dtdet[EMBA]->Fill( dt );
      per_det_dtdet[EMBC]->Fill( -1. * dt );
      //      per_det_lbdt[EMBA]->Fill( dt );
      per_det_lbdt[EMBC]->Fill( -1. * dt );
      if ( lumi_block < m_n_lumi_blocks ) {
	m_per_detector_hists["lb_v_time_diff"][EMBA]->Fill( lumi_block, dt );
	m_per_detector_hists["lb_v_time_diff"][EMBC]->Fill( lumi_block, -1. * dt );
      }
    }

    if ( mean_detector_times.find(EMECA) != itr_end &&
	 mean_detector_times.find(EMECC) != itr_end ) {
      double dt = mean_detector_times[EMECA].result() - mean_detector_times[EMECC].result();
      //      per_det_dtdet[EMECA]->Fill( dt );
      per_det_dtdet[EMECC]->Fill( -1. * dt );
      //      per_det_lbdt[EMECA]->Fill( dt );
      per_det_lbdt[EMECC]->Fill( -1. * dt );
      if ( lumi_block < m_n_lumi_blocks ) {
	m_per_detector_hists["lb_v_time_diff"][EMECA]->Fill( lumi_block, dt );
	m_per_detector_hists["lb_v_time_diff"][EMECC]->Fill( lumi_block, -1. * dt );
      }
    }

    if ( mean_detector_times.find(HECA) != itr_end &&
	 mean_detector_times.find(HECC) != itr_end ) {
      double dt = mean_detector_times[HECA].result() - mean_detector_times[HECC].result();
      //      per_det_dtdet[HECA]->Fill( dt );
      per_det_dtdet[HECC]->Fill( -1. * dt );
      //      per_det_lbdt[HECA]->Fill( dt );
      per_det_lbdt[HECC]->Fill( -1. * dt );
      if ( lumi_block < m_n_lumi_blocks ) {
	m_per_detector_hists["lb_v_time_diff"][HECA]->Fill( lumi_block, dt );
	m_per_detector_hists["lb_v_time_diff"][HECC]->Fill( lumi_block, -1. * dt );
      }
    }

    if ( mean_detector_times.find(FCALA) != itr_end &&
	 mean_detector_times.find(FCALC) != itr_end ) {
      double dt = mean_detector_times[FCALA].result() - mean_detector_times[FCALC].result();
      //      per_det_dtdet[FCALA]->Fill( dt );
      per_det_dtdet[FCALC]->Fill( -1. * dt );
      //      per_det_lbdt[FCALA]->Fill( dt );
      per_det_lbdt[FCALC]->Fill( -1. * dt );
      if ( lumi_block < m_n_lumi_blocks ) {
	m_per_detector_hists["lb_v_time_diff"][FCALA]->Fill( lumi_block, dt );
	m_per_detector_hists["lb_v_time_diff"][FCALC]->Fill( lumi_block, -1. * dt );
      }
    }

   }
  */

  return StatusCode::SUCCESS;

}


/*----------------------------------------------------------------------------*/
StatusCode LArRawChannelMonTool::procHistograms()
{

  ATH_MSG_DEBUG( "===> start " << name() << "::procHistograms <=== " );

  bool is_new_interval = false;
  switch ( m_interval ) {

// BT : what about endOfEventsBlock????
  case lumiBlock:
    is_new_interval = endOfLumiBlockFlag();
    break;
  case lowStat:
    is_new_interval = newLowStatFlag();
    break;
  case run:
    is_new_interval = endOfRunFlag();
    break;
  default:
    ATH_MSG_WARNING( "given bad interval job-option: " << m_interval_str
	<< " ... booking histograms per run ..." );
    is_new_interval = endOfRunFlag();
  }

  if ( is_new_interval ) {

    if ( m_event_counter != 0 ) {

      // divide the efficiency histograms by number of events

      // scale per feedthrough efficiency histograms
      if ( m_monitor_febs ) {

	std::vector<std::vector<shared_ptr<IHistoProxyBase> >*> per_feb_hists_to_scale;
	if ( m_monitor_quality )        per_feb_hists_to_scale.push_back(&m_per_feb_hists[quality_h]);

	foreach( const std::vector<shared_ptr<IHistoProxyBase> >* const th1_vect_cptr, per_feb_hists_to_scale )
	  foreach( shared_ptr<IHistoProxyBase> const histo_cptr, *th1_vect_cptr )
	  if ( histo_cptr )
	    histo_cptr->Scale( 100. / float( m_event_counter ) );

      }

      // scale per feedthrough efficiency histograms
      if ( m_monitor_feedthroughs ) {
	std::vector<std::vector<shared_ptr<IHistoProxyBase> >*> per_ft_hists_to_scale;
	if ( m_monitor_quality )        per_ft_hists_to_scale.push_back(&m_per_feedthrough_hists[quality_h]);

	foreach( const std::vector<shared_ptr<IHistoProxyBase> >* const th2_vect_cptr, per_ft_hists_to_scale )
	  foreach( shared_ptr<IHistoProxyBase> const histo_cptr, *th2_vect_cptr )
	  if ( histo_cptr )
	    histo_cptr->Scale( 100. / float( m_event_counter ) );
      }

      // --- reset event counter ---
      m_event_counter = 0;

    }

    if ( m_noise_stream_event_counter != 0 ) {
      // divide the efficiency histograms by number of events

      // scale per feedthrough efficiency histograms
      if ( m_monitor_febs ) {

	std::vector<std::vector<shared_ptr<IHistoProxyBase> >*> per_feb_hists_to_scale;
	if ( m_monitor_positive_noise ) per_feb_hists_to_scale.push_back(&m_per_feb_hists[pos_noise_h]);
	if ( m_monitor_negative_noise ) per_feb_hists_to_scale.push_back(&m_per_feb_hists[neg_noise_h]);

	foreach( const std::vector<shared_ptr<IHistoProxyBase> >* const th1_vect_cptr, per_feb_hists_to_scale )
	  foreach( shared_ptr<IHistoProxyBase> const histo_cptr, *th1_vect_cptr )
	  if ( histo_cptr )
	    histo_cptr->Scale( 100. / float( m_noise_stream_event_counter ) );

      }

      // scale per feedthrough efficiency histograms
      if ( m_monitor_feedthroughs ) {
	std::vector<std::vector<shared_ptr<IHistoProxyBase> >*> per_ft_hists_to_scale;
	if ( m_monitor_positive_noise ) per_ft_hists_to_scale.push_back(&m_per_feedthrough_hists[pos_noise_h]);
	if ( m_monitor_negative_noise ) per_ft_hists_to_scale.push_back(&m_per_feedthrough_hists[neg_noise_h]);

	foreach( const std::vector<shared_ptr<IHistoProxyBase> >* const th2_vect_cptr, per_ft_hists_to_scale )
	  foreach( shared_ptr<IHistoProxyBase> const histo_cptr, *th2_vect_cptr )
	  if ( histo_cptr )
	    histo_cptr->Scale( 100. / float( m_noise_stream_event_counter ) );
      }
      // --- reset event counter ---
      m_noise_stream_event_counter = 0;


    }

  }

  ATH_MSG_DEBUG( "===> end " << name() << "::procHistograms <=== " );

  return StatusCode::SUCCESS;

}


/*----------------------------------------------------------------------------*/
std::string LArRawChannelMonTool::threshold_histogram_title_snippet( const double& t)
{


  try {

    return " of events above " + lexical_cast<std::string>( int( t / MeV ) ) +
      " MeV as a function of ";

  } catch ( const bad_lexical_cast& err ) {

    ATH_MSG_WARNING( "Could not include threshold for histogram titles."
	<< err.what() );
    return "";

  }

}


/*----------------------------------------------------------------------------*/
std::string LArRawChannelMonTool::threshold_histogram_title_snippet( const int& t)
{

  try {

    return lexical_cast<std::string>( t ) + " sigma total noise as a function of ";

  } catch ( const bad_lexical_cast& err ) {

    ATH_MSG_WARNING( "Could not include threshold for histogram titles."
	<< err.what() );
    return "";

  }

}

bool LArRawChannelMonTool::registerHistogram( LWHist* histo, const std::string& dir, const std::string& merge){

  MonGroup histGroup(this,dir,m_interval,ATTRIB_MANAGED,"",merge);
  if ( !histGroup.regHist(histo).isSuccess() ) {

    ATH_MSG_ERROR( "Cannot book: " << histo->GetName() );
    return false;

  } else {

    ATH_MSG_DEBUG( "Booked: " << histo->GetName() );
    return true;

  }

}

IHistoProxyBase * LArRawChannelMonTool::createLWHistProxy(LWHist*h) {
  if (dynamic_cast<LWHist1D*>(h)) return new LWHistoProxy1D(static_cast<LWHist1D*>(h));
  if (dynamic_cast<LWHist2D*>(h)) return new LWHistoProxy2D(static_cast<LWHist2D*>(h));
  if (dynamic_cast<TProfile_LW*>(h)) return new LWHistoProxyProfile(static_cast<TProfile_LW*>(h));
  assert(dynamic_cast<TProfile2D_LW*>(h));
  return new LWHistoProxyProfile2D(dynamic_cast<TProfile2D_LW*>(h));
}

void  LArRawChannelMonTool::setIsATLASReady(){
  bool filterresult(true);
  if (! m_filterAtlasReady_tools.empty()) {
    ToolHandleArray<IDQFilterTool>::const_iterator ifilter(m_filterAtlasReady_tools.begin()), filterend(m_filterAtlasReady_tools.end());
    for (; filterresult && (ifilter != filterend);
	 ++ifilter) {
      filterresult = (filterresult && (*ifilter)->accept());
    }
  }
  m_atlas_ready = filterresult;
}
