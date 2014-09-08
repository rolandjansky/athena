/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

// ROOT include(s)
#include "TRandom.h"

// Local include(s):
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"

#include "PATInterfaces/SystematicCode.h"     
#include "PATInterfaces/SystematicList.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h" 

namespace CP {

MuonCalibrationAndSmearingTool::MuonCalibrationAndSmearingTool( const std::string& name ) :
  asg::AsgTool( name ) {
  declareProperty( "Year", m_year = "Data12" );
  declareProperty( "Algo", m_algo = "muons" );
  declareProperty( "SmearingType", m_type = "q_pT" );
  declareProperty( "Release", m_release = "Rel17.2Sum13" );
  declareProperty( "FilesPath", m_path = "" );
}

StatusCode MuonCalibrationAndSmearingTool::initialize() {
  // Greet the user:
  ATH_MSG_INFO( "Initialising..." );

  // ::GA:: Add the others
  //if( !m_rand ) m_rand = new TRandom();
  //m_rand->SetSeed( 0 );
  m_pTmax = 100000000;
  m_Tscale = SCALE_DEFAULT;
  m_Tdata = SetData( m_year );
  m_Talgo = SetAlgorithm( m_algo );
  if( m_type == "pT" ) {
    m_Tsmear = SMEAR_PT;
    ATH_MSG_INFO( "Using " << m_type << " Smearing " );
  }
  else if( m_type=="q_pT" ) {
    m_Tsmear = SMEAR_QPT;
    ATH_MSG_INFO( "Using " << m_type << " Smearing " );
  }
  m_Trel = SetRelease( m_release );
  if( m_path == "" ) {
    char *rootcoreDir = getenv( "ROOTCOREBIN" );
    char *testareaDir = getenv( "TestArea" );
    if( rootcoreDir ) {
      m_path = std::string( rootcoreDir ) + "/data/MuonMomentumCorrections/";
    }
    else if( testareaDir ) {
      m_path = std::string( testareaDir ) + "/PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonMomentumCorrections/share/";
    }
  }
  ATH_MSG_DEBUG( "Year: " << m_year );
  ATH_MSG_DEBUG( "Algo: " << m_algo );
  ATH_MSG_DEBUG( "Type: " << m_type );
  ATH_MSG_DEBUG( "Release: " << m_release );
  ATH_MSG_DEBUG( "Directory: " << m_path );
  std::string regionsPath = m_path;
  int regionMode = 0; //simple Eta Bins
  if ( m_Trel <= REL17 ) {
    regionsPath += "Regions.dat";
  }
  else if ( m_Trel == REL17_2_REPRO ) {
    regionsPath += "RegionsRepro.dat";//16 eta bins for ID and MS
  }
  else if ( m_Trel == REL17_2_SUM13 ) {
    regionsPath += "RegionsPhi18.dat";
    regionMode = 2;//MS: 48Bins L/S split plus 11_15 and Feet in the BARREL. ID has values from 16 eta bins
    m_useNsigmaForICombine = 0;
  }
  else {
    ATH_MSG_ERROR( "Unknown release" );
    return StatusCode::FAILURE;
  }
  if( Regions( regionsPath, regionMode ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
  if( FillValues() == StatusCode::FAILURE ) return StatusCode::FAILURE;

  if( !applySystematicVariation( SystematicSet() ) ) {
    ATH_MSG_ERROR( "Unable to run with no systematic" );
    return StatusCode::FAILURE;
  }
  SystematicRegistry& registry = SystematicRegistry::getInstance();
  if( registry.registerSystematics( *this ) != SystematicCode::Ok ) return StatusCode::FAILURE;

  // Return gracefully:
  return StatusCode::SUCCESS;
}

CorrectionCode MuonCalibrationAndSmearingTool::applyCorrection( xAOD::Muon& mu ) {
  const ElementLink< xAOD::TrackParticleContainer >& id_track = mu.inDetTrackParticleLink();
  const ElementLink< xAOD::TrackParticleContainer >& ms_track = mu.muonSpectrometerTrackParticleLink();
  if( !id_track || !ms_track ) {
    ATH_MSG_DEBUG( "( !id_track || !ms_track )=true, id_track=" << id_track << "  ms_track=" << ms_track );
    return CorrectionCode::OutOfValidityRange;
  }
  else {
    ATH_MSG_DEBUG( "( !id_track || !ms_track )=false, id_track=" << id_track << "  ms_track=" << ms_track );
  }
  if( !( *id_track ) || !( *ms_track ) ){
    ATH_MSG_DEBUG( "( !(*id_track) || !(*ms_track) )=true, (*id_track)=" << *id_track << "  (*ms_track)=" << *ms_track);
    return CorrectionCode::OutOfValidityRange;
  }
  else{
    ATH_MSG_DEBUG( "( !(*id_track) || !(*ms_track) )=false, (*id_track)=" << *id_track << "  (*ms_track)=" << *ms_track );
  }
  m_ptid = ( *id_track )->pt() / 1000.;
  m_ptms = ( *ms_track )->pt() / 1000.;
  m_ptcb = mu.pt() / 1000.;
  m_eta = mu.eta();
  m_phi = mu.phi();
  const ElementLink< xAOD::TrackParticleContainer >& cb_track = mu.combinedTrackParticleLink();
  m_charge = ( *cb_track )->charge();
  ATH_MSG_DEBUG( "Input Muon Info: Pt_ID = " << m_ptid * 1000. );
  ATH_MSG_DEBUG( "                 Pt_MS = " << m_ptms * 1000. );
  ATH_MSG_DEBUG( "                 Pt_CB = " << m_ptcb * 1000. );
  ATH_MSG_DEBUG( "                   Eta = " << m_eta );
  ATH_MSG_DEBUG( "                   Phi = " << m_phi );
  ATH_MSG_DEBUG( "                Charge = " << ( ( m_charge > 0 ) ? "+" : "-" ) );
  // Retrieve the event information:
  const xAOD::EventInfo* ei = 0;
  if( evtStore()->retrieve( ei, "EventInfo" ).isFailure() ) {
    ATH_MSG_WARNING( "No EventInfo object could be retrieved" );
    ATH_MSG_WARNING( "Random number generation not configured correctly" );
  }
  // Get the run and event numbers:
  //const uint32_t runNumber = ei ? ei->runNumber() : 0;
  const unsigned long long eventNumber = ei ? ei->eventNumber() : 0;
  // Construct a seed for the random number generator:
  const UInt_t seed = 1000 * mu.phi() + eventNumber;
  gRandom->SetSeed( seed );
  //gRandom->SetSeed( 0 );
  m_smearDeltaMS = 0.;
  m_smearDeltaID = 0.;
  m_smearDeltaCB = 0.;
  m_detRegion = -1;
  if( m_phi == m_defaultInit && m_Trel == REL17_2_SUM13 ) ATH_MSG_ERROR( "You need phi of the muon for after Rel17.2Sum13!" );
  /* Detector Region */
  if( m_phi == m_defaultInit && m_Trel < REL17_2_SUM13 ) {
    //Lower release have just Eta bins, so usually the user doesn't input
    //Phi and 0 is a good default for these regions
    m_detRegion = GetRegion( m_eta,0 );
  }
  else {
    m_detRegion = GetRegion( m_eta, m_phi );
  }
  if( m_detRegion == -1 ) {
    ATH_MSG_WARNING( "Can't find correct detector region! Set smearing to 0." );
    return CorrectionCode::Error;
  }
  /* Scale Region */
  m_scaleRegion = GetScaleRegion( mu );
  m_g0 = gRandom->Gaus( 0, 1 );
  m_g1 = gRandom->Gaus( 0, 1 );
  m_g2 = gRandom->Gaus( 0, 1 );
  m_g3 = gRandom->Gaus( 0, 1 );
  m_g4 = gRandom->Gaus( 0, 1 );
  ATH_MSG_DEBUG( "Generated Random Values: g_0 = " << m_g0 ); 
  ATH_MSG_DEBUG( "                         g_1 = " << m_g1 ); 
  ATH_MSG_DEBUG( "                         g_2 = " << m_g2 ); 
  ATH_MSG_DEBUG( "                         g_3 = " << m_g3 ); 
  ATH_MSG_DEBUG( "                         g_4 = " << m_g4 ); 
  /** Definition: deltaSmear = 1+smearDET **/
  m_smearDeltaMS = GetSmearing( DET_MS, mu );
  m_smearDeltaID = GetSmearing( DET_ID, mu );
  CalcCBWeights( mu );
  m_smearDeltaCB = m_smearDeltaID*m_weightID + m_smearDeltaMS*m_weightMS;
  // Smear the pT of the muon:
  double correctedPt = mu.pt();
  if( ei->eventType( xAOD::EventInfo::IS_SIMULATION ) ) correctedPt = 1000. * CalculatePt( m_currentParameters->SubDetectorType, m_currentParameters->VarSmearingID, m_currentParameters->VarSmearingMS, m_currentParameters->Scale );
  mu.setP4( correctedPt, m_eta, m_phi );
  ATH_MSG_DEBUG( "Output Muon Info: Pt_CB = " << correctedPt );
  // Return gracefully:
  return CorrectionCode::Ok;
}

CorrectionCode MuonCalibrationAndSmearingTool::correctedCopy( const xAOD::Muon& input, xAOD::Muon*& output ) {
  // A sanity check:
  if( output ) ATH_MSG_WARNING( "Non-null pointer received. " "There's a possible memory leak!" );
  // Create a new object:
  output = new xAOD::Muon();
  output->makePrivateStore( input );
  // Use the other function to modify this object:
  return applyCorrection( *output );
}

bool MuonCalibrationAndSmearingTool::isAffectedBySystematic( const SystematicVariation& systematic ) const {
  SystematicSet sys = affectingSystematics();
  return sys.find( systematic ) != sys.end();
}

SystematicSet MuonCalibrationAndSmearingTool::affectingSystematics() const {
  SystematicSet result;
  //::: ID systematics
  result.insert( SystematicVariation( "MUONS_ID", 1 ) );
  result.insert( SystematicVariation( "MUONS_ID", -1 ) );
  //::: MS systematics
  result.insert( SystematicVariation( "MUONS_MS", 1 ) );
  result.insert( SystematicVariation( "MUONS_MS", -1 ) );
  //::: Scale systematics
  result.insert( SystematicVariation( "MUONS_SCALE", 1 ) );
  result.insert( SystematicVariation( "MUONS_SCALE", -1 ) );
  return result;
}

SystematicSet MuonCalibrationAndSmearingTool::recommendedSystematics() const {
  return affectingSystematics();
}

SystematicCode MuonCalibrationAndSmearingTool::applySystematicVariation( const SystematicSet& systConfig ) {
  // first check if we already know this systematic configuration
  boost::unordered_map< SystematicSet, ParameterSet >::iterator parIter = m_Parameters.find( systConfig );
  if( parIter != m_Parameters.end() ) {
    m_currentParameters = &parIter->second;
    return SystematicCode::Ok;
  }

  ParameterSet param;
  param.SubDetectorType = DET_CB;
  param.VarSmearingID = m_defaultInit;
  param.VarSmearingMS = m_defaultInit;
  param.Scale = 0;

  SystematicVariation syst = systConfig.getSystematicByBaseName( "MUONS_ID" );
  if( syst == SystematicVariation( "MUONS_ID", 1 ) ) {
    param.VarSmearingMS = m_smearDeltaMS;
    param.VarSmearingID = GetSystVariation( DET_ID, 1. );
  }
  else if( syst == SystematicVariation( "MUONS_ID", -1 ) ) {
    param.VarSmearingMS = m_smearDeltaMS;
    param.VarSmearingID = GetSystVariation( DET_ID, -1. );
  }
  else if( !syst.empty() ) return SystematicCode::Unsupported;

  syst = systConfig.getSystematicByBaseName( "MUONS_MS" );
  if( syst == SystematicVariation( "MUONS_MS", 1 ) ) {
    param.VarSmearingID = m_smearDeltaID;
    param.VarSmearingMS = GetSystVariation( DET_MS, 1. );
  }
  else if( syst == SystematicVariation( "MUONS_MS", -1 ) ) {
    param.VarSmearingID = m_smearDeltaID;
    param.VarSmearingMS = GetSystVariation( DET_MS, -1. );
  }
  else if( !syst.empty() ) return SystematicCode::Unsupported;

  syst = systConfig.getSystematicByBaseName( "MUONS_SCALE" );
  if( syst == SystematicVariation( "MUONS_SCALE", 1 ) ) {
    param.Scale = 1.;
  }
  else if( syst == SystematicVariation( "MUONS_SCALE", -1 ) ) {
    param.Scale = -1.;
  }
  else if( !syst.empty() ) return SystematicCode::Unsupported;

  // store this calibration for future use, and make it current
  m_currentParameters = &m_Parameters.insert( std::make_pair( systConfig, param ) ).first->second;
  return SystematicCode::Ok;
}

DATATYPE MuonCalibrationAndSmearingTool::SetData( std::string data ) {
  if( data == "Data10" ) {
    m_Tdata = DATA10;
  }
  else if( data == "Data11" ) {
    m_Tdata = DATA11;
  }
  else if( data == "Data12" ) {
    m_Tdata = DATA12;
  }
  return m_Tdata;
}

ALGOTYPE MuonCalibrationAndSmearingTool::SetAlgorithm( std::string algo ) {
  if( algo=="muid" ) {
    m_Talgo=MUID;
  }
  else if( algo=="staco" ) {
    m_Talgo=STACO;
  }
  else if( algo=="muons" ) {
    m_Talgo=MUONS;
  }
  return m_Talgo;
}

RELTYPE MuonCalibrationAndSmearingTool::SetRelease( std::string rel ) {
  if( rel=="Rel16.6" ) {
    m_Trel=REL16_6;
  }
  else if( rel=="Rel17" ) {
    m_Trel=REL17;
  }
  else if( rel=="Rel17.2" ) {
    m_Trel=REL17_2;
  }
  else if( rel=="Rel17.2Repro" ) {
    m_Trel=REL17_2_REPRO;
  }
  else if( rel=="Rel17.2Sum13" ) {
    m_Trel=REL17_2_SUM13;
  }
  return m_Trel;
}

double MuonCalibrationAndSmearingTool::ScaleApply( const double pt, const double S1, const double S2,
    double S, const double S_enLoss ) const {
  bool useScale = true;
  bool apply_to_data = false;
  /* general-purpose function to apply scale/curvature corrections */
  if ( m_detRegion<0 || m_detRegion>=m_nb_regions ) return pt;
  //ATH_MSG_DEBUG( "m_detRegion="<<m_detRegion );
  //ATH_MSG_DEBUG( "S="<<S );
  //ATH_MSG_DEBUG( "S_EnLoss="<<S_enLoss );
  //ATH_MSG_DEBUG( "S1="<<S1 );
  //ATH_MSG_DEBUG( "S2="<<S2 );
  //if(dbg) cout<<"m_detRegion="<<m_detRegion<<"; S="<<S<<"; S_EnLoss="<<S_enLoss<<"; S1="<<S1<<"; S2="<<S2<<endl;
  if( !useScale ) return pt;
  // default momentum scale (derived form template fits)
  if( m_Tscale == SCALE_DEFAULT ) {
    return apply_to_data ? pt/S : ( pt*S + S_enLoss );
  }
  // charge-dependent scales: kp,km == pt(data)/pt(mc)
  if( m_Tscale == SCALE_KPKM ) {
    if( m_charge == 0 ) {
      ATH_MSG_ERROR( " must provide muon charge via Event() to use advanced pT scaling" );
      //cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"ERROR: must provide muon charge via Event() to use advanced pT scaling" << endl;
      return pt;
    }
    const double kpkm = ( m_charge>0 ? S1 : S2 )/100.0;
    return apply_to_data ? pt/kpkm : pt*kpkm;
  }
  // overall scale: K == pt(data)/pt(mc) and curvature split: 2*C == 1/pt+ - 1/pt-
  // both corrections can be applied one after another or one at a time
  if( m_Tscale >= SCALE_KC ) {
    if( m_charge == 0 ) {
      ATH_MSG_ERROR( "must provide muon charge via Event() to use advanced pT scaling" );
      //cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"ERROR: must provide muon charge via Event() to use advanced pT scaling" << endl;
      return pt;
    }
    const double K = S1/100.0;
    const double C = S2/1e6;
    // scale pt (unless only curvature splitting is requested)
    const double ptsc = ( m_Tscale==SCALE_C ) ? pt : ( apply_to_data ? pt/K : pt*K );
    // calculate curvature and apply a correction, if needed:
    double curv=1/ptsc;
    if( m_Tscale != SCALE_K ) {
      // if correcting a curvature offset in data:
      if( apply_to_data ) curv += m_charge<0 ? C  : -C;
      // if putting a curvature offset into MC to better match the data
      else curv += m_charge<0 ? -C  : +C;
    }
    return curv == 0 ? m_pTmax : fabs( 1.0/curv );
  }
  return pt;
}


double MuonCalibrationAndSmearingTool::CalculatePt( const int DetType, const double inSmearID, const double inSmearMS, const double scaleVar ) const {
  //////////////////////////////////////////////////////////////////////////////////////////
  //Scales have not been applied to the present smearings, get them before proceeding
  //////////////////////////////////////////////////////////////////////////////////////////
  double scaleID = 0., enLossCorrMS = 0., scaleMS = 0., scaleCB = 0.;//initialize all to 0
  //These are alternative scale corrections (KPKM,KC,K,C) they are != 0. if Tscale != SCALE_DEFAULT.
  double s1_ID = 0., s2_ID = 0., s1_MS = 0., s2_MS = 0., s1_CB = 0., s2_CB = 0.;//Description of these is in ApplyScale
  if( fabs( scaleVar ) != 1. && scaleVar != 0. ) ATH_MSG_ERROR( "Unpredicted scale variation of Delta "<<scaleVar<<" sigmas!" );
  if( m_Tscale == SCALE_DEFAULT ) {
    if( m_scale_ID[m_detRegion] != -1 ) {
      if( m_Trel >= REL17_2_SUM13 ) {
        scaleID = scaleVar > 0. ? m_scaleSystUp_ID[m_detRegion] : m_scaleSystDw_ID[m_detRegion];
      }
      else {
        scaleID = m_scaleSyst_ID[m_detRegion];
      }
      scaleID = m_scale_ID[m_detRegion] + scaleVar*scaleID;
    }
    else {
      scaleID = ( m_Trel >= REL17_2_SUM13 ) ? 0. : 1.;
    }
    if( m_scale_MS[m_detRegion] != -1 ) {
      if( m_Trel >= REL17_2_SUM13 ) {
        scaleMS = scaleVar > 0. ? m_scaleSystUp_MS[m_detRegion] : m_scaleSystDw_MS[m_detRegion];
        enLossCorrMS = scaleVar > 0. ? m_enLossSystUp_MS[m_detRegion] : m_enLossSystDw_MS[m_detRegion];
      }
      else {
        scaleMS = m_scaleSyst_MS[m_detRegion];
      }
      scaleMS =  m_scale_MS[m_detRegion] + scaleVar*scaleMS;
      if( true ) enLossCorrMS = m_enLoss_MS[m_detRegion] + scaleVar*enLossCorrMS;
    }
    else {
      scaleMS = ( m_Trel >= REL17_2_SUM13 ) ? 0. : 1.;
    }
    if( m_Trel >= REL17_2_SUM13 ) { //NB: REL17_2_SUM13 has scale parametrized around 0, not 1!!
      scaleID += 1.;
      scaleMS += 1.;
    }
    if( m_scale_CB[m_detRegion] != -1 ) {
      scaleCB = m_scale_CB[m_detRegion] + scaleVar*m_scaleSyst_CB[m_detRegion];
    }
    else {
      scaleCB = ( scaleID*m_weightID + ( scaleMS + enLossCorrMS/m_ptms )*m_weightMS );
    }
    /*if( dbg )
      std::cout<<__FILE__<<" "<<__LINE__<<": m_weightID="<<m_weightID<<"; m_weightMS="<<m_weightMS
               <<"; enLossCorrMS="<<enLossCorrMS<<"; scaleID="<<scaleID<<"; scaleMS="<<scaleMS<<"; scaleCB="<<scaleCB<<std::endl;*/
    //ATH_MSG_DEBUG( "m_weightID="<<m_weightID<<"; m_weightMS="<<m_weightMS<<"; enLossCorrMS="<<enLossCorrMS<<"; scaleID="<<scaleID<<"; scaleMS="<<scaleMS<<"; scaleCB="<<scaleCB );
  }
  else {
    if( unsigned( m_scaleRegion ) >= m_S1_CB.size() || unsigned( m_scaleRegion ) >= m_S2_CB.size() ) {
      ATH_MSG_ERROR( " " << m_scaleRegion << " length of scaleRegion1: " << m_S1_CB.size() );
      ATH_MSG_ERROR( " " << m_scaleRegion << " length of scaleRegion2: " << m_S2_CB.size() );
      //std::cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR:: "<<scaleRegion<<" length of scaleRegion1: "<<m_S1_CB.size()<<std::endl;
      //std::cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR:: "<<scaleRegion<<" length of scaleRegion2: "<<m_S2_CB.size()<<std::endl;
      return -1;
    }
    s1_ID = m_S1_ID[m_scaleRegion];
    s2_ID = m_S2_ID[m_scaleRegion];
    s1_MS = m_S1_MS[m_scaleRegion];
    s2_MS = m_S2_MS[m_scaleRegion];
    s1_CB = m_S1_CB[m_scaleRegion];
    s2_CB = m_S2_CB[m_scaleRegion];
    //+++++ These variations were available for 2011 and should be checked again +++++++++++
    if( m_Tscale == SCALE_KC || m_Tscale == SCALE_K ) {
      // here, we scale up or down the overall pT scale: called SCALE_K variation
      s1_ID += scaleVar*m_S1Corr_ID[m_scaleRegion];
      s1_MS += scaleVar*m_S1Corr_MS[m_scaleRegion];
      s1_CB += scaleVar*m_S1Corr_CB[m_scaleRegion];
      // here, we scale up or down the curvature offset: called SCALE_C variation
      s2_ID += scaleVar*m_S2Corr_ID[m_scaleRegion];
      s2_MS += scaleVar*m_S2Corr_MS[m_scaleRegion];
      s2_CB += scaleVar*m_S2Corr_CB[m_scaleRegion];
    }
    if( m_Tscale == SCALE_KPKM ) {
      // m_S1Corr/m_S2Corr correspond to the component of kp/km error from Z mass fit: called SCALE_K variation
      // m_S1ACorr/m_S2ACorr correspond to the component of kp/km error from R0 fit: called SCALE_C variation
      s1_ID += scaleVar*m_S1ACorr_ID[m_scaleRegion] + scaleVar*m_S1Corr_ID[m_scaleRegion];
      s2_ID += scaleVar*m_S2ACorr_ID[m_scaleRegion] - scaleVar*m_S2Corr_ID[m_scaleRegion];
      s1_MS += scaleVar*m_S1ACorr_MS[m_scaleRegion] + scaleVar*m_S1Corr_MS[m_scaleRegion];
      s2_MS += scaleVar*m_S2ACorr_MS[m_scaleRegion] - scaleVar*m_S2Corr_MS[m_scaleRegion];
      s1_CB += scaleVar*m_S1ACorr_CB[m_scaleRegion] + scaleVar*m_S1Corr_CB[m_scaleRegion];
      s2_CB += scaleVar*m_S2ACorr_CB[m_scaleRegion] - scaleVar*m_S2Corr_CB[m_scaleRegion];
    }
  }
  //////////////////////////////////////////////////////////////////////////////////////////
  double tmpDelta= 1.;
  double outPtID = m_ptid, outPtMS = m_ptms, outPtCB = m_ptcb;
  if( DetType == DET_ID ) {
    //Load the ID scale and smearing that you will use
    tmpDelta = ( inSmearID == m_defaultInit ) ? 1.+ m_smearDeltaID : 1.+ inSmearID;
    //ATH_MSG_DEBUG( "smearDeltaID="<<m_smearDeltaID<<"; scaleID="<<scaleID<<"; initial outPtID="<<outPtID );
    //if( dbg )	std::cout<<__FILE__<<" "<<__LINE__
    //<<": tmpDelta="<<tmpDelta<<"; smearDeltaID="<<m_smearDeltaID<<"; scaleID="<<scaleID<<"; initial outPtID="<<outPtID<<std::endl;
    //In these releases the smearing was applied to the pt before the scale
    if( m_Trel < REL17_2_SUM13 ) {
      if( m_Tsmear == SMEAR_PT )  outPtID = outPtID*tmpDelta;
      if( m_Tsmear == SMEAR_QPT ) outPtID = ( tmpDelta == 0 ) ? m_pTmax : outPtID/tmpDelta;
    }
    outPtID = ScaleApply( fabs( outPtID ), s1_ID, s2_ID, scaleID );
    if( m_Trel >= REL17_2_SUM13 ) {
      if( m_Tsmear == SMEAR_PT )  outPtID = outPtID*tmpDelta;
      if( m_Tsmear == SMEAR_QPT ) outPtID = ( tmpDelta == 0 ) ? m_pTmax : outPtID/tmpDelta;
    }
    return outPtID;
  }
  if( DetType == DET_MS ) {
    //Load the MS scale and smearing that you will use
    tmpDelta = ( inSmearMS == m_defaultInit ) ? 1.+ m_smearDeltaMS : 1.+ inSmearMS;
    //ATH_MSG_DEBUG( "tmpDelta="<<tmpDelta<<"; smearDeltaMS="<<m_smearDeltaMS<<"; scaleMS="<<scaleMS<<"; initial outPtMS="<<outPtMS );
    //if( dbg ) std::cout<<__FILE__<<" "<<__LINE__
    //                     <<": tmpDelta="<<tmpDelta<<"; smearDeltaMS="<<m_smearDeltaMS<<"; scaleMS="<<scaleMS<<"; initial outPtMS="<<outPtMS<<std::endl;
    //In these releases the smearing was applied to the pt before the scale
    if( m_Trel < REL17_2_SUM13 ) {
      if( m_Tsmear == SMEAR_PT )  outPtMS = outPtMS*tmpDelta;
      if( m_Tsmear == SMEAR_QPT ) outPtMS = ( tmpDelta == 0 ) ? m_pTmax : outPtMS/tmpDelta;
    }
    outPtMS = ScaleApply( fabs( outPtMS ), s1_MS, s2_MS, scaleMS, enLossCorrMS );
    if( m_Trel >= REL17_2_SUM13 ) {
      if( m_Tsmear == SMEAR_PT )  outPtMS = outPtMS*tmpDelta;
      if( m_Tsmear == SMEAR_QPT ) outPtMS = ( tmpDelta == 0 ) ? m_pTmax : outPtMS/tmpDelta;
    }
    return outPtMS;
  }
  if( DetType == DET_CB ) {
    if( inSmearID != m_defaultInit && inSmearMS != m_defaultInit ) {
      tmpDelta = 1. + inSmearID*m_weightID + inSmearMS*m_weightMS;
    }
    else if ( inSmearID == m_defaultInit && inSmearMS == m_defaultInit ) {
      tmpDelta = 1. + m_smearDeltaCB;
    }
    else {
      ATH_MSG_ERROR( "only one ID or MS var is defined for CB var!" );
      //std::cout<<__FILE__<<" "<<__LINE__<<": ERROR! only one ID or MS var is defined for CB var!"<<std::endl;
    }
    //ATH_MSG_DEBUG( "tmpDelta="<<tmpDelta<<"; smearDeltaCB="<<m_smearDeltaCB<<"; scaleCB="<<scaleCB<<"; initial outPtCB="<<outPtCB );
    //if( dbg ) std::cout<<__FILE__<<" "<<__LINE__
    //                     <<": tmpDelta="<<tmpDelta<<"; smearDeltaCB="<<m_smearDeltaCB<<"; scaleCB="<<scaleCB<<"; initial outPtCB="<<outPtCB<<std::endl;
    //In these releases the smearing was applied to the pt before the scale
    if( m_Trel < REL17_2_SUM13 ) {
      if( m_Tsmear == SMEAR_PT )  outPtCB = outPtCB*tmpDelta;
      if( m_Tsmear == SMEAR_QPT ) outPtCB = ( tmpDelta == 0 ) ? m_pTmax : outPtCB/tmpDelta;
    }
    //ATH_MSG_DEBUG( "tmpDelta="<<tmpDelta<<"; outPtCB="<<outPtCB );
    //if( dbg ) std::cout<<__FILE__<<" "<<__LINE__<<": tmpDelta="<<tmpDelta<<"; outPtCB="<<outPtCB<<std::endl;
    outPtCB = ScaleApply( fabs( outPtCB ), s1_CB, s2_CB, scaleCB );
    if( m_Trel >= REL17_2_SUM13 ) {
      if( m_Tsmear == SMEAR_PT )  outPtCB = outPtCB*tmpDelta;
      if( m_Tsmear == SMEAR_QPT ) outPtCB = ( tmpDelta == 0 ) ? m_pTmax : outPtCB/tmpDelta;
    }
    return outPtCB;
  }
  return 0.;
}



StatusCode MuonCalibrationAndSmearingTool::FillValues() {
  Clean();
  double tmpval;
  int i=0;
  std::string tmpname;
  std::string scale_val = m_path+"Scale_values_"+m_algo+"_"+m_release+".dat";
  if( m_year == "Data11" && m_Trel >= REL17_2_SUM13 )
    scale_val = m_path+"Data11_Scale_values_"+m_algo+"_"+m_release+".dat";
  std::string data_val  = m_path+m_year+"_values_"+m_algo+"_"+m_release+".dat";
  std::string mc_val    = m_path+"MC_values_"+m_algo+"_"+m_release+".dat";
  std::string corr_val  = m_path+m_year+"_CorrMatrix_"+m_algo+"_"+m_release+".dat";
  /* filling scale vectors*/
  m_InValues.open( scale_val.c_str() );
  i=0;
  if( !m_InValues.good() ) {
    ATH_MSG_ERROR( "File " << scale_val <<" not found!!" );
    return StatusCode::FAILURE;
  }
  else {
    while( m_InValues.good() && i<m_nb_regions ) {
      tmpval=0;
      if( i==0 ) {
        getline( m_InValues,tmpname );
      }
      if( m_Trel >= REL17_2_SUM13 ) {
        //These have a different order and Up/Dw split sys
        m_InValues>>tmpval;
        m_scale_ID.push_back( tmpval );
        m_InValues>>tmpval;
        m_enLoss_MS.push_back( tmpval );
        m_InValues>>tmpval;
        m_scale_MS.push_back( tmpval );
        m_InValues>>tmpval;
        m_scaleSystUp_ID.push_back( tmpval );
        m_InValues>>tmpval;
        m_enLossSystUp_MS.push_back( tmpval );
        m_InValues>>tmpval;
        m_scaleSystUp_MS.push_back( tmpval );
        m_InValues>>tmpval;
        m_scaleSystDw_ID.push_back( tmpval );
        m_InValues>>tmpval;
        m_enLossSystDw_MS.push_back( tmpval );
        m_InValues>>tmpval;
        m_scaleSystDw_MS.push_back( tmpval );
      }
      else {
        m_enLoss_MS.push_back( 0. );
        m_enLossSyst_MS.push_back( 0. );
        m_InValues>>tmpval;
        m_scale_ID.push_back( tmpval );
        m_InValues>>tmpval;
        m_scaleSyst_ID.push_back( tmpval );
        m_InValues>>tmpval;
        m_scale_MS.push_back( tmpval );
        m_InValues>>tmpval;
        m_scaleSyst_MS.push_back( tmpval );
      }
      //CB is always on the cfg file althoug rarely used
      m_InValues>>tmpval;
      m_scale_CB.push_back( tmpval );
      m_InValues>>tmpval;
      m_scaleSyst_CB.push_back( tmpval );
      i++;
    }
  }
  m_InValues.close();
  m_InValues.clear();
  /* filling data vectors*/
  m_InValues.open( data_val.c_str() );
  i=0;
  if( !m_InValues.good() ) {
    ATH_MSG_ERROR( "File " << data_val << " not found!!" );
    return StatusCode::FAILURE;
  }
  else {
    while( m_InValues.good() && i < m_nb_regions ) {
      /*main values*/
      tmpval=0;
      if( i==0 ) {
        getline( m_InValues,tmpname );
      }
      m_InValues>>tmpval;
      m_p1_ID.push_back( tmpval );
      m_InValues>>tmpval;
      m_p2_ID.push_back( tmpval );
      m_InValues>>tmpval;
      m_p2_ID_TAN.push_back( tmpval );
      m_InValues>>tmpval;
      if( m_Trel >= REL17_2_SUM13 ) {
        m_p0_MS.push_back( tmpval );
        m_InValues>>tmpval;
      }
      else {
        m_p0_MS.push_back( 0 );
      }
      m_p1_MS.push_back( tmpval );
      m_InValues>>tmpval;
      m_p2_MS.push_back( tmpval );
      //Syst and stat uncertianties: they are split and symmetrized before Rel17.2Sum13, after
      // that, the stat is added in ^2 and they are split in Up/Dw
      if( m_Trel >= REL17_2_SUM13 ) {
        m_InValues>>tmpval;
        m_SUp_p1_ID.push_back( tmpval );
        m_InValues>>tmpval;
        m_SUp_p2_ID.push_back( tmpval );
        m_InValues>>tmpval;
        m_SUp_p2_ID_TAN.push_back( tmpval );
        m_InValues>>tmpval;
        m_SUp_p0_MS.push_back( tmpval );
        m_InValues>>tmpval;
        m_SUp_p1_MS.push_back( tmpval );
        m_InValues>>tmpval;
        m_SUp_p2_MS.push_back( tmpval );
        m_InValues>>tmpval;
        m_SDw_p1_ID.push_back( tmpval );
        m_InValues>>tmpval;
        m_SDw_p2_ID.push_back( tmpval );
        m_InValues>>tmpval;
        m_SDw_p2_ID_TAN.push_back( tmpval );
        m_InValues>>tmpval;
        m_SDw_p0_MS.push_back( tmpval );
        m_InValues>>tmpval;
        m_SDw_p1_MS.push_back( tmpval );
        m_InValues>>tmpval;
        m_SDw_p2_MS.push_back( tmpval );
      }
      else {
        m_E_p0_MS.push_back( 0 );
        m_S_p0_MS.push_back( 0 );
        //Stat
        m_InValues>>tmpval;
        m_E_p1_ID.push_back( tmpval );
        m_InValues>>tmpval;
        m_E_p2_ID.push_back( tmpval );
        m_InValues>>tmpval;
        m_E_p2_ID_TAN.push_back( tmpval );
        m_InValues>>tmpval;
        m_E_p1_MS.push_back( tmpval );
        m_InValues>>tmpval;
        m_E_p2_MS.push_back( tmpval );
        /*systematic*/
        m_InValues>>tmpval;
        m_S_p1_ID.push_back( tmpval );
        m_InValues>>tmpval;
        m_S_p2_ID.push_back( tmpval );
        m_InValues>>tmpval;
        m_S_p2_ID_TAN.push_back( tmpval );
        m_InValues>>tmpval;
        m_S_p1_MS.push_back( tmpval );
        m_InValues>>tmpval;
        m_S_p2_MS.push_back( tmpval );
      }
    }
  }
  m_InValues.close();
  m_InValues.clear();
  /* filling mc vectors*/
  m_InValues.open( mc_val.c_str() );
  i=0;
  if( !m_InValues.good() ) {
    ATH_MSG_ERROR( "File " << mc_val << " not found!!" );
    return StatusCode::FAILURE;
  }
  else {
    while( m_InValues.good() && i<m_nb_regions ) {
      tmpval=0;
      if( i==0 ) {
        getline( m_InValues,tmpname );
      }
      m_InValues>>tmpval;
      m_MC_p1_ID.push_back( tmpval );
      m_InValues>>tmpval;
      m_MC_p2_ID.push_back( tmpval );
      m_InValues>>tmpval;
      m_MC_p2_ID_TAN.push_back( tmpval );
      m_InValues>>tmpval;
      m_MC_p0_MS.push_back( tmpval );
      m_InValues>>tmpval;
      m_MC_p1_MS.push_back( tmpval );
      m_InValues>>tmpval;
      m_MC_p2_MS.push_back( tmpval );
    }
  }
  m_InValues.close();
  m_InValues.clear();
  /* filling correlation vectors*/
  m_InValues.open( corr_val.c_str() );
  i=0;
  if( !m_InValues.good() ) {
    ATH_MSG_ERROR( "File " << corr_val << " not found!!" );
    return StatusCode::FAILURE;
  }
  else {
    while( m_InValues.good() && i<m_nb_regions ) {
      tmpval=0;
      if( i==0 ) {
        getline( m_InValues,tmpname );
      }
      m_InValues>>tmpval;
      m_CorrMatC0.push_back( tmpval );
      m_CorrMatTanC0.push_back( tmpval );
      m_InValues>>tmpval;
      m_CorrMatC1.push_back( tmpval );
      m_CorrMatTanC1.push_back( tmpval );
      m_InValues>>tmpval;
      m_CorrMatC2.push_back( tmpval );
      m_CorrMatTanC2.push_back( tmpval );
      m_InValues>>tmpval;
      m_CorrMatC3.push_back( tmpval );
      m_CorrMatTanC3.push_back( tmpval );
      m_InValues>>tmpval;
      m_CorrMatC4.push_back( tmpval );
      m_CorrMatTanC4.push_back( tmpval );
      m_InValues>>tmpval;
      m_CorrMatC5.push_back( tmpval );
      m_CorrMatTanC5.push_back( tmpval );
    }
  }
  m_InValues.close();
  m_InValues.clear();
  return StatusCode::SUCCESS;
}

int MuonCalibrationAndSmearingTool::GetScaleRegion( xAOD::Muon& mu ) {
  if( m_scaleBins.empty() ) {
    return -1;
  }
  double _min = m_scaleBins[0];
  double _max = m_scaleBins[m_scaleBins.size()-1];
  if( mu.eta()<_min ) {
    return 0;
  }
  if( mu.eta()>=_max ) {
    return m_scaleBins.size() - 1;
  }
  std::vector<double>::iterator lb = lower_bound( m_scaleBins.begin(),m_scaleBins.end(),mu.eta()+1e-9 ); // [first,last)
  return( std::min( static_cast<int>( distance( m_scaleBins.begin(),lb ) ) , static_cast<int>( m_scaleBins.size()-1 ) ) );
}

double MuonCalibrationAndSmearingTool::GetSmearing( int DetType, xAOD::Muon& mu ) {
  bool useTan2 = true;
  if ( m_detRegion < 0 || m_detRegion >= m_nb_regions ) return 0; //++++++ HOW TO IMPROVE THIS CHECK?!
  double smear = 0.;
  if ( DetType == DET_MS ) {
    if( m_ptms == 0 ) {
      return 0;
    }
    else {
      smear =  m_p0_MS[m_detRegion]*m_g0/m_ptms + m_p1_MS[m_detRegion]*m_g1 + m_p2_MS[m_detRegion]*m_g2*m_ptms;
      //ATH_MSG_DEBUG( "m_p0_MS[m_detRegion]:"<<m_p0_MS[m_detRegion]<<"; m_p1_MS[m_detRegion]:"<<m_p1_MS[m_detRegion]<<"; m_p2_MS[m_detRegion]="<<m_p2_MS[m_detRegion]<<"; ptms="<<m_ptms<<"; smear:"<<smear );
      //if(dbg) cout<<"m_p0_MS[m_detRegion]:"<<m_p0_MS[m_detRegion]<<"; m_p1_MS[m_detRegion]:"<<m_p1_MS[m_detRegion]<<"; m_p2_MS[m_detRegion]="<<m_p2_MS[m_detRegion]
      //	   <<"; ptms="<<ptms<<"; smear:"<<smear<<endl;
      return smear;
    }
  }
  else if( DetType==DET_ID ) {
    if ( useTan2 && m_p2_ID_TAN[m_detRegion] != 0. ) {
      smear = m_p1_ID[m_detRegion]*m_g3 + m_p2_ID_TAN[m_detRegion]*m_g4*m_ptid*sinh( mu.eta() )*sinh( mu.eta() );
      //ATH_MSG_DEBUG( "m_p1_ID[m_detRegion]:"<<m_p1_ID[m_detRegion]<<"; m_p2_ID_TAN[m_detRegion]="<<m_p2_ID_TAN[m_detRegion]<<"; ptid="<<m_ptid<<"; smearTAN:"<<smear );
      //if(dbg) cout<<"m_p1_ID[m_detRegion]:"<<m_p1_ID[m_detRegion]<<"; m_p2_ID_TAN[m_detRegion]="<<m_p2_ID_TAN[m_detRegion]
      //	   <<"; ptid="<<ptid<<"; smearTAN:"<<smear<<endl;
      return smear;
    }
    else {
      smear = m_p1_ID[m_detRegion]*m_g3 + m_p2_ID[m_detRegion]*m_g4*m_ptid;
      //ATH_MSG_DEBUG( "m_p1_ID[m_detRegion]:"<<m_p1_ID[m_detRegion]<<"; m_p2_ID[m_detRegion]="<<m_p2_ID[m_detRegion]<<"; ptid="<<m_ptid<<"; smear:"<<smear );
      //if(dbg) cout<<"m_p1_ID[m_detRegion]:"<<m_p1_ID[m_detRegion]<<"; m_p2_ID[m_detRegion]="<<m_p2_ID[m_detRegion]
      //	   <<"; ptid="<<ptid<<"; smear:"<<smear<<endl;
      return smear;
    }
  }
  else {
    ATH_MSG_ERROR( " DetType not defined "<<DetType );
    //      std::cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR:: DetType not defined "<<DetType<<std::endl;
  }
  return 0;
}

void MuonCalibrationAndSmearingTool::CalcCBWeights( xAOD::Muon& mu ) {
  //Calculate the weights of ID and MS used for CB. Two ways are implemented:
  // - weight by ^2 of exp reso
  // - from equation pT_CB = R(pT_ID) + (1-R)(pT_MS)
  //(weights should be correctly normalized)
  m_weightID = 0.5;
  m_weightMS = 0.5;
  if ( m_detRegion < 0 || m_detRegion >= m_nb_regions ) return;
  if ( m_ptcb==0 ) {
    //cout<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass WARNING:: ptcb==0"<<endl;
    ATH_MSG_ERROR( "Combined Pt = 0" );
    return;
  }
  const bool doNotAddSmearing = true;
  double SigmaMS = ExpectedResolution( DET_MS,mu, doNotAddSmearing );
  double SigmaID = ExpectedResolution( DET_ID,mu, doNotAddSmearing );
  //if(dbg) cout<<"SigmaMS="<<SigmaMS<<"; SigmaID="<<SigmaID<<endl;
  double Nsigma = m_useNsigmaForICombine;
  if( fabs( m_ptcb-m_ptms ) > Nsigma*SigmaMS*m_ptcb || fabs( m_ptcb-m_ptid ) > Nsigma*SigmaID*m_ptcb ) {
    double R=1, Rplus;
    if( fabs( m_ptcb-m_ptms ) == fabs( m_ptcb-m_ptid ) ) return; //This case returns weightID = weightMS = 0.5
    if( fabs( m_ptcb-m_ptms ) != 0 && fabs( m_ptcb-m_ptms ) > fabs( m_ptcb-m_ptid ) ) {
      R = ( m_ptid - m_ptcb )/( m_ptcb - m_ptms ); /* R~wMS/wID */
      Rplus = 1 + R;
      if ( Rplus != 0 && R > 0 ) {
        m_weightID = 1/Rplus;
        m_weightMS = R/Rplus;
        return;
      }
    }
    if ( fabs( m_ptcb-m_ptid ) != 0 && fabs( m_ptcb-m_ptms ) < fabs( m_ptcb-m_ptid ) ) {
      R = ( m_ptms - m_ptcb )/( m_ptcb - m_ptid ); /* R~wID/wMS */
      Rplus = 1 + R;
      if ( Rplus != 0 && R > 0 ) {
        m_weightID = R/Rplus;
        m_weightMS = 1/Rplus;
        return;
      }
    }
  }

  double wMS = m_ptms/m_ptcb/pow(SigmaMS,2);
  double wID = m_ptid/m_ptcb/pow(SigmaID,2);
  m_weightID =  wID/(wMS + wID);
  m_weightMS =  wMS/(wMS + wID);
  return;
}

void MuonCalibrationAndSmearingTool::CleanScales() {
  m_Tscale = SCALE_DEFAULT;
  m_scaleBins.clear();
  m_S1_ID.clear();
  m_S2_ID.clear();
  m_S1_MS.clear();
  m_S2_MS.clear();
  m_S1_CB.clear();
  m_S2_CB.clear();
  m_S1Corr_ID.clear();
  m_S2Corr_ID.clear();
  m_S1Corr_MS.clear();
  m_S2Corr_MS.clear();
  m_S1Corr_CB.clear();
  m_S2Corr_CB.clear();
  m_S1ACorr_ID.clear();
  m_S2ACorr_ID.clear();
  m_S1ACorr_MS.clear();
  m_S2ACorr_MS.clear();
  m_S1ACorr_CB.clear();
  m_S2ACorr_CB.clear();
}

double MuonCalibrationAndSmearingTool::ExpectedResolution( const std::string &DetType,xAOD::Muon& mu, const bool mc ) const {
  if( DetType=="MS" ) {
    return ExpectedResolution( DET_MS,mu,mc );
  }
  else if( DetType=="ID" ) {
    return ExpectedResolution( DET_ID,mu,mc );
  }
  else if( DetType=="CB" ) {
    return ExpectedResolution( DET_CB,mu,mc );
  }
  else {
    ATH_MSG_ERROR( "wrong DetType in input "<<DetType );
    //cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR: wrong DetType in input "<<DetType<<endl;
    return 0.;
  }
}

double MuonCalibrationAndSmearingTool::ExpectedResolution( const int DetType,xAOD::Muon& mu, const bool mc ) const {
  bool useTan2 = true;
  /** do the average with the EXPECTED resolutions **/
  if ( m_detRegion<0 || m_detRegion>=m_nb_regions ) return 0;
  double expRes = 0.;
  if ( DetType == DET_MS ) {
    //if (ptms==0) cout<<__FILE__<<" line:"<<__LINE__<<" SmearingClass WARNING:: ptms==0"<<endl;
    double p0 = mc ? m_MC_p0_MS[m_detRegion] : ( m_MC_p0_MS[m_detRegion] + m_p0_MS[m_detRegion] );
    double p1 = mc ? m_MC_p1_MS[m_detRegion] : ( m_MC_p1_MS[m_detRegion] + m_p1_MS[m_detRegion] );
    double p2 = mc ? m_MC_p2_MS[m_detRegion] : ( m_MC_p2_MS[m_detRegion] + m_p2_MS[m_detRegion] );
    expRes =  sqrt( pow( p0/m_ptms, 2 ) + pow( p1, 2 ) + pow( p2*m_ptms ,2 ) );
    //if(dbg) cout<<__LINE__<<" Std MS RES: "<<expRes<<endl;
    return expRes; //+++++No SYS!!!
  }
  else if ( DetType == DET_ID ) {
    if ( m_ptid==0 ) ATH_MSG_WARNING( "ptid==0" );
    //if (ptid==0) cout<<__FILE__<<" line:"<<__LINE__<<" SmearingClass WARNING:: ptid==0"<<endl;
    double p1 = mc ? m_MC_p1_ID[m_detRegion] : ( m_MC_p1_ID[m_detRegion] + m_p1_ID[m_detRegion] );
    double p2 = mc ? m_MC_p2_ID[m_detRegion] : ( m_MC_p2_ID[m_detRegion] + m_p2_ID[m_detRegion] );
    if ( m_MC_p2_ID_TAN[m_detRegion] != 0 && useTan2 ) {
      p2 = mc ? m_MC_p2_ID_TAN[m_detRegion] : ( m_MC_p2_ID_TAN[m_detRegion] + m_p2_ID_TAN[m_detRegion] );
      p2 = p2*sinh( mu.eta() )*sinh( mu.eta() );
    }
    expRes = sqrt( pow( p1, 2 ) + pow( p2*m_ptid ,2 ) );
    //if(dbg) cout<<__LINE__<<" Std ID RES: "<<expRes<<endl;
    return expRes; //+++++No SYS!!!
  }
  else if ( DetType == DET_CB ) {
    //if (ptcb==0) cout<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass WARNING:: ptcb==0"<<endl;//+++++++
    // Due to complicated maths, the expected combined resolution
    // is given by this equation (note: all sigmas are fractional uncertainties):
    // sigma_CB = sqrt(2) * sigma_ID * sigma_MS * pTMS * pTID / {pTCB * sqrt({sigma_ID*pTID}^2 + {sigma_MS*pTMS}^2)}
    // Do a little recursive calling to make things easier to read
    // Turn these into *absolute* uncertainties to make life easier
    double sigmaID = ExpectedResolution( DET_ID,mu,mc ) * m_ptid;
    double sigmaMS = ExpectedResolution( DET_MS,mu,mc ) * m_ptms;
    double denominator = ( m_ptcb ) * sqrt( sigmaID*sigmaID + sigmaMS*sigmaMS );
    return denominator ? sqrt( 2. ) * sigmaID * sigmaMS / denominator : 0.;
  }
  else {
    ATH_MSG_ERROR( "wrong DetType in input "<<DetType );
    return 0.;
  }
}

void MuonCalibrationAndSmearingTool::Clean() {
  m_scale_ID.clear();
  m_enLoss_MS.clear();
  m_scale_MS.clear();
  m_scale_CB.clear();
  m_scaleSyst_ID.clear();
  m_enLossSyst_MS.clear();
  m_scaleSyst_MS.clear();
  m_scaleSyst_CB.clear();
  m_scaleSystUp_ID.clear();
  m_enLossSystUp_MS.clear();
  m_scaleSystUp_MS.clear();
  m_scaleSystDw_ID.clear();
  m_enLossSystDw_MS.clear();
  m_scaleSystDw_MS.clear();
  CleanScales();
  m_p1_ID.clear();
  m_p2_ID.clear();
  m_p2_ID_TAN.clear();
  m_p0_MS.clear();
  m_p1_MS.clear();
  m_p2_MS.clear();
  m_E_p1_ID.clear();
  m_E_p2_ID.clear();
  m_E_p2_ID_TAN.clear();
  m_E_p0_MS.clear();
  m_E_p1_MS.clear();
  m_E_p2_MS.clear();
  m_S_p1_ID.clear();
  m_S_p2_ID.clear();
  m_S_p2_ID_TAN.clear();
  m_S_p0_MS.clear();
  m_S_p1_MS.clear();
  m_S_p2_MS.clear();
  m_SUp_p1_ID.clear();
  m_SUp_p2_ID.clear();
  m_SUp_p2_ID_TAN.clear();
  m_SUp_p0_MS.clear();
  m_SUp_p1_MS.clear();
  m_SUp_p2_MS.clear();
  m_SDw_p1_ID.clear();
  m_SDw_p2_ID.clear();
  m_SDw_p2_ID_TAN.clear();
  m_SDw_p0_MS.clear();
  m_SDw_p1_MS.clear();
  m_SDw_p2_MS.clear();
  m_MC_p1_ID.clear();
  m_MC_p2_ID.clear();
  m_MC_p2_ID_TAN.clear();
  m_MC_p0_MS.clear();
  m_MC_p1_MS.clear();
  m_MC_p2_MS.clear();
  m_CorrMatC0.clear();
  m_CorrMatC1.clear();
  m_CorrMatC2.clear();
  m_CorrMatC3.clear();
  m_CorrMatC4.clear();
  m_CorrMatC5.clear();
  m_CorrMatTanC0.clear();
  m_CorrMatTanC1.clear();
  m_CorrMatTanC2.clear();
  m_CorrMatTanC3.clear();
  m_CorrMatTanC4.clear();
  m_CorrMatTanC5.clear();
  m_InValues.clear();
}

void MuonCalibrationAndSmearingTool::PrintRegions() const {
  if( m_doMacroRegions ) {
    for( std::map<int, int>::const_iterator it = m_macroRegionIdxMap.begin(); it!=m_macroRegionIdxMap.end(); ++it ) {
      int bReg = it->first, mReg = it->second;
      ATH_MSG_INFO( "Base region n "<<bReg );
      ATH_MSG_INFO( "phi_min="<<m_phi_min[bReg]<<", phi_max="<<m_phi_max[bReg] );
      ATH_MSG_INFO( "eta_min="<<m_eta_min[bReg]<<", eta_max="<<m_eta_max[bReg] );
      ATH_MSG_INFO( "included in Macro region N "<<mReg<<" with innerEta="<<m_macroRegionInnerEta[mReg] );
    }
  }
  else {
    for( int bReg=0; bReg<m_nb_regions; ++bReg ) {
      ATH_MSG_INFO( "Base region n "<<bReg<<": phi_min="<<m_phi_min[bReg]<<", phi_max="<<m_phi_max[bReg] );
      ATH_MSG_INFO( "eta_min="<<m_eta_min[bReg]<<", eta_max="<<m_eta_max[bReg] );
    }
  }
}

unsigned int MuonCalibrationAndSmearingTool::GetNRegions() const {
  if( m_doMacroRegions ) return m_macroRegionName.size();
  else return m_nb_regions;
}

StatusCode MuonCalibrationAndSmearingTool::Regions( std::string inRegionFile, int doMacroRegionsFlag ) {
  m_eta_min.clear();
  m_eta_max.clear();
  m_phi_min.clear();
  m_phi_max.clear();
  m_loadNames = false;
  m_doMacroRegions = false;
  m_verb = false;
  std::string tmpname;
  double tmpval;
  int i=0;
  std::ifstream inFile( inRegionFile.c_str() );
  if( !inFile.good() ) {
    ATH_MSG_ERROR( "File not good" );
    return StatusCode::FAILURE;
  }
  else {
    while( !inFile.eof() ) {
      tmpval=0;
      tmpname="";
      if( i==0 ) {
        getline( inFile,tmpname );
        //Check if I am loading the name of the regions in the file
        if( tmpname.find( "name" ) != std::string::npos && !m_loadNames ) m_loadNames=true;
      }
      else {
        inFile>>tmpval;
        if( inFile.eof() ) break; //this will break immediatly as soon as the new line is empty
        m_eta_min.push_back( tmpval );
        inFile>>tmpval;
        m_eta_max.push_back( tmpval );
        inFile>>tmpval;
        m_phi_min.push_back( tmpval );
        inFile>>tmpval;
        m_phi_max.push_back( tmpval );
        if( m_loadNames ) { //Fill the name vector only if you have them!
          inFile>>tmpname;
          m_names.push_back( tmpname );
        }
      }
      ++i;
    }
  }
  inFile.close();
  m_nb_regions = ( int )m_eta_min.size();
  if( doMacroRegionsFlag ) { //In case I want macroRegions, I need to define the criteria here
    m_doMacroRegions =true;
    switch( doMacroRegionsFlag ) {
    case 1:
      //Collects all the Large and Small sectors if they belong to the same eta bin
      CollectMacroRegionsSL();
      break;
    case 2:
      //Large,Small sectors split plus Feet(12+14) and 11+15 sector split in Barrel
      CollectMacroRegionsSL_SplitBAR();
      break;
    case 3:
      //Collects all the Large and Small, Up and Down sectors if they belong to the same eta bin
      CollectMacroRegionsSL_UpDn();
      break;
    default:
      ATH_MSG_ERROR( "doMacroRegionFlag=" << doMacroRegionsFlag << " not defined!" );
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

int MuonCalibrationAndSmearingTool::GetRegion( const double eta, const double phi ) const {
  int ret_k =-1;
  for( int k=0; k < m_nb_regions; ++k ) {
    if( eta>=m_eta_min[k] && eta<m_eta_max[k] ) {
      if( m_phi_min[k] <= m_phi_max[k] ) { //Standard case
        if( phi>=m_phi_min[k] && phi<m_phi_max[k] ) {
          ret_k = k;
          break;
        }
      }
      else {  //Circular closure of Phi, min and max are excanged in the .dat file
        if( phi>=m_phi_min[k] || phi<m_phi_max[k] ) {
          ret_k = k;
          break;
        }
      }
    }
  }
  if( ret_k == -1 ) {
    ATH_MSG_WARNING( "Region corresponding to Eta="<<eta<<", Phi="<<phi<<" NOT FOUND!" );
    return -1;
  }
  if( m_doMacroRegions ) return m_macroRegionIdxMap.find( ret_k )->second;
  return ret_k;
}

float MuonCalibrationAndSmearingTool::GetRegionInnerEta( const int r_i ) const { //Return Eta closer to the origin
  if( m_doMacroRegions ) {
    if( r_i>=0 && r_i < ( int )m_macroRegionName.size() ) return m_macroRegionInnerEta[r_i];
  }
  else {
    if( r_i >= 0 && r_i < m_nb_regions ) {
      if( m_eta_min[r_i] >= 0. ) return m_eta_min[r_i];
      else return m_eta_max[r_i];
    }
  }
  ATH_MSG_WARNING( "Region Inner Eta corresponding to Region index="<<r_i<<" NOT FOUND!" );
  return -999.;
}

std::string MuonCalibrationAndSmearingTool::GetRegionName( const int r_i ) const {
  if( m_loadNames ) {
    if( m_doMacroRegions ) {
      if( r_i>=0 && r_i < ( int )m_macroRegionName.size() ) return m_macroRegionName[r_i];
    }
    else {
      if( r_i>=0 && r_i < m_nb_regions ) return m_names[r_i];
    }
    ATH_MSG_WARNING( "Region Name corresponding to Region index="<<r_i<<" NOT FOUND!" );
  }
  ATH_MSG_WARNING( " Region Names are not set!" );
  return "NAN";
}

std::string MuonCalibrationAndSmearingTool::GetRegionName( const double eta, const double phi ) const {
  if( m_loadNames ) return GetRegionName( GetRegion( eta, phi ) );
  ATH_MSG_WARNING( "Region Names are not set!" );
  return "NAN";
}

//Collects all the Large and Small sectors if they belong to the same eta bin
void MuonCalibrationAndSmearingTool::CollectMacroRegionsSL() {
  double etaMin = -999., etaMax = -999.;
  int macroRegionIdx = 0;
  for( int k=0; k < m_nb_regions; ++k ) {
    if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ) {
      //Build a new macroRegion
      etaMin = m_eta_min[k];
      etaMax = m_eta_max[k];
      //One of the following is true
      std::string macroRegName = m_names[k].substr( 0, m_names[k].find( "EL" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "BL" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "ES" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "BS" ) );
      m_macroRegionName.push_back( macroRegName+"Large" );
      m_macroRegionName.push_back( macroRegName+"Small" );
      //insert twice the innerEta, for Large and for Small sectors
      for( int i=0; i<2; ++i ) {
        if( etaMin >= 0. ) m_macroRegionInnerEta.push_back( etaMin );
        else         m_macroRegionInnerEta.push_back( etaMax );
      }
      macroRegionIdx+=2;
    }
    if( m_names[k].find( "EL" ) != std::string::npos || m_names[k].find( "BL" ) != std::string::npos )
      m_macroRegionIdxMap[k] = macroRegionIdx-2;//Large sectors
    if( m_names[k].find( "ES" ) != std::string::npos || m_names[k].find( "BS" ) != std::string::npos )
      m_macroRegionIdxMap[k] = macroRegionIdx-1;//Small sectors
  }
}

//Collects all the Large and Small sectors if they belong to the same eta bin
void MuonCalibrationAndSmearingTool::CollectMacroRegionsSL_UpDn() {
  double etaMin = -999., etaMax = -999.;
  int macroRegionIdx = 0;
  for( int k=0; k < m_nb_regions; ++k ) {
    if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ) {
      //Build a new macroRegion
      etaMin = m_eta_min[k];
      etaMax = m_eta_max[k];
      //One of the following is true
      std::string macroRegName = m_names[k].substr( 0, m_names[k].find( "EL" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "BL" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "ES" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "BS" ) );
      m_macroRegionName.push_back( macroRegName+"LargeDn" );
      m_macroRegionName.push_back( macroRegName+"SmallDn" );
      m_macroRegionName.push_back( macroRegName+"LargeUp" );
      m_macroRegionName.push_back( macroRegName+"SmallUp" );
      //insert 4 time the innerEta, for Large and Small sectors times Up and Dn
      for( int i=0; i<4; ++i ) {
        if( etaMin >= 0. ) m_macroRegionInnerEta.push_back( etaMin );
        else         m_macroRegionInnerEta.push_back( etaMax );
      }
      macroRegionIdx+=4;
    }
    if( m_names[k].find( "EL" ) != std::string::npos || m_names[k].find( "BL" ) != std::string::npos ) { //Large sectors
      if( m_names[k].find( "11" ) != std::string::npos || m_names[k].find( "13" ) != std::string::npos ||
          m_names[k].find( "15" ) != std::string::npos ) {
        m_macroRegionIdxMap[k] = macroRegionIdx-4;//Down Large sectors (within 10 to 16)
      }
      else {
        m_macroRegionIdxMap[k] = macroRegionIdx-2; //Up, large sectors
      }
    }
    if( m_names[k].find( "ES" ) != std::string::npos || m_names[k].find( "BS" ) != std::string::npos ) { //Small sectors
      if( m_names[k].find( "10" ) != std::string::npos || m_names[k].find( "12" ) != std::string::npos ||
          m_names[k].find( "14" ) != std::string::npos || m_names[k].find( "16" ) != std::string::npos ) {
        m_macroRegionIdxMap[k] = macroRegionIdx-3; //Down Small sectors (from 10 to 16). Should I remove 10 and 16? ++++++
      }
      else {
        m_macroRegionIdxMap[k] = macroRegionIdx-1; ; //Up, Small sectors
      }
    }
  }
}

//Collects all the Large and Small sectors if they belong to the same eta bin and splits
// the barrel 12,14 smalls and 11+15 larges that may need different calibrations
void MuonCalibrationAndSmearingTool::CollectMacroRegionsSL_SplitBAR() {
  double etaMin = -999., etaMax = -999.;
  int macroRegionIdx = 0;
  for( int k=0; k < m_nb_regions; ++k ) {
    if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ) { //Build a new macroRegion
      etaMin = m_eta_min[k];
      etaMax = m_eta_max[k];
      std::string macroRegName ="";
      if( m_names[k].find( "EL" )!= std::string::npos || m_names[k].find( "ES" ) != std::string::npos ) {
        //We are in the End-Cap sectors so we split only Large and Small sectors
        macroRegName = m_names[k].substr( 0, m_names[k].find( "EL" ) );
        macroRegName = macroRegName.substr( 0, m_names[k].find( "ES" ) );
        //insert 4 time the innerEta, for Large and Small sectors
        m_macroRegionName.push_back( macroRegName+"Large" );
        m_macroRegionName.push_back( macroRegName+"Small" );
        for( int i=0; i<2; ++i ) {
          if( etaMin >= 0. ) m_macroRegionInnerEta.push_back( etaMin );
          else               m_macroRegionInnerEta.push_back( etaMax );
        }
        macroRegionIdx+=2;
      }
      else {  //We are in the Barrels sectors so we split Large, Small, 12+14, 11+15 sectors
        macroRegName = m_names[k].substr( 0, m_names[k].find( "BL" ) );
        macroRegName = macroRegName.substr( 0, m_names[k].find( "BS" ) );
        //insert 4 time the innerEta, for Large and Small sectors times Up and Dn
        m_macroRegionName.push_back( macroRegName+"Large" );
        m_macroRegionName.push_back( macroRegName+"Small" );
        m_macroRegionName.push_back( macroRegName+"Large11_15" );
        m_macroRegionName.push_back( macroRegName+"SmallFeet" );
        for( int i=0; i<4; ++i ) {
          if( etaMin >= 0. ) m_macroRegionInnerEta.push_back( etaMin );
          else               m_macroRegionInnerEta.push_back( etaMax );
        }
        macroRegionIdx+=4;
      }
    }
    if( m_names[k].find( "EL" ) != std::string::npos ) { //End-Cap Large sectors
      m_macroRegionIdxMap[k] = macroRegionIdx-2;
    }
    else if( m_names[k].find( "ES" ) != std::string::npos ) { //End-Cap Small sectors
      m_macroRegionIdxMap[k] = macroRegionIdx-1; //
    }
    else if ( m_names[k].find( "BL" ) != std::string::npos ) { //Barrel Large sectors
      if( m_names[k].find( "11" ) != std::string::npos || m_names[k].find( "15" ) != std::string::npos ) {
        m_macroRegionIdxMap[k] = macroRegionIdx-2;//Barrel Large sectors with different alignment (11,15)
      }
      else {
        m_macroRegionIdxMap[k] = macroRegionIdx-4; //Standard Barrel Large sectors
      }
    }
    else if( m_names[k].find( "BS" ) != std::string::npos ) { //Barrel Small sectors
      if( m_names[k].find( "12" ) != std::string::npos || m_names[k].find( "14" ) != std::string::npos ) {
        m_macroRegionIdxMap[k] = macroRegionIdx-1; //Feet Small sectors (12+14, Down).
      }
      else {
        m_macroRegionIdxMap[k] = macroRegionIdx-3; //All standard Barrel Small sectors
      }
    }
  }
}

double MuonCalibrationAndSmearingTool::GetSystVariation( int DetType, double var ) {

  if( var != 1. && var != -1 ) {
    ATH_MSG_WARNING( "Systematic variation is not +/- 1 sigma, returning 0." );
    return 0;
  }
  if( m_detRegion < 0 || m_detRegion >= m_nb_regions ) return 0;

  double p0_MS_var = 0., p1_MS_var = 0., p2_MS_var = 0., p1_ID_var = 0., p2_ID_var = 0., p2_ID_TAN_var = 0.;   
  double newSmear = 0.;
  if( DetType == DET_MS ) {
    if( m_ptms == 0 ) {  
      return 0;
    } else {
      if( m_Trel < REL17_2_SUM13 ) {
        p0_MS_var = pow( m_E_p0_MS[ m_detRegion ] * m_E_p0_MS[ m_detRegion ] + m_S_p0_MS[ m_detRegion ] * m_S_p0_MS[ m_detRegion ], 0.5 );
        p1_MS_var = pow( m_E_p1_MS[ m_detRegion] * m_E_p1_MS[ m_detRegion ] + m_S_p1_MS[ m_detRegion ] * m_S_p1_MS[ m_detRegion ], 0.5 );
        p2_MS_var = pow( m_E_p2_MS[ m_detRegion] * m_E_p2_MS[ m_detRegion ] + m_S_p2_MS[ m_detRegion ] * m_S_p2_MS[ m_detRegion ], 0.5 );
      } else {//Syst are not more symmetric in the latest release, the stat/sys add in quadrature is already done
        p0_MS_var = var > 0. ? m_SUp_p0_MS[ m_detRegion ] : m_SDw_p0_MS[ m_detRegion ];
        p1_MS_var = var > 0. ? m_SUp_p1_MS[ m_detRegion ] : m_SDw_p1_MS[ m_detRegion ];
        p2_MS_var = var > 0. ? m_SUp_p2_MS[ m_detRegion ] : m_SDw_p2_MS[ m_detRegion ];
      }

      p0_MS_var = m_p0_MS[ m_detRegion ] + var * p0_MS_var;
      p1_MS_var = m_p1_MS[ m_detRegion ] + var * p1_MS_var;
      p2_MS_var = m_p2_MS[ m_detRegion ] + var * p2_MS_var;

      if( p0_MS_var < 0. ) p0_MS_var = 0.; //Truncation of unphysical variations
      if( p1_MS_var < 0. ) p1_MS_var = 0.;
      if( p2_MS_var < 0. ) p2_MS_var = 0.;
      newSmear = ( p0_MS_var * m_g0 / m_ptms + p1_MS_var * m_g1 + p2_MS_var * m_g2 * m_ptms );
      return newSmear;
    }
  } else if( DetType == DET_ID ) {
    if( m_Trel < REL17_2_SUM13 ) {
      p1_ID_var     = pow( m_E_p1_ID[ m_detRegion ] * m_E_p1_ID[ m_detRegion ] + m_S_p1_ID[ m_detRegion ] * m_S_p1_ID[ m_detRegion ], 0.5 );
      p2_ID_var     = pow( m_E_p2_ID[ m_detRegion ] * m_E_p2_ID[ m_detRegion ] + m_S_p2_ID[ m_detRegion ] * m_S_p2_ID[ m_detRegion ], 0.5 );
      p2_ID_TAN_var = pow( m_E_p2_ID_TAN[ m_detRegion ] * m_E_p2_ID_TAN[ m_detRegion ] + m_S_p2_ID_TAN[ m_detRegion ] * m_S_p2_ID_TAN[ m_detRegion ], 0.5 );
    } else {//Syst are not more symmetric in the latest release, the stat/sys add in quadrature is already done
      p1_ID_var     = var > 0. ? m_SUp_p1_ID[ m_detRegion ] : m_SDw_p1_ID[ m_detRegion ];
      p2_ID_var     = var > 0. ? m_SUp_p2_ID[ m_detRegion ] : m_SDw_p2_ID[ m_detRegion ];
      p2_ID_TAN_var = var > 0. ? m_SUp_p2_ID_TAN[ m_detRegion ] : m_SDw_p2_ID_TAN[ m_detRegion ];
    }
    p1_ID_var     = m_p1_ID[ m_detRegion ] + var * p1_ID_var;
    p2_ID_var     = m_p2_ID[ m_detRegion ] + var * p2_ID_var;
    p2_ID_TAN_var = m_p2_ID_TAN[ m_detRegion ] + var * p2_ID_TAN_var;

    if( p1_ID_var < 0. ) p1_ID_var = 0.; //Truncation of unphysical variations
    if( p2_ID_var < 0. ) p2_ID_var = 0.;
    if( p2_ID_TAN_var < 0. ) p2_ID_TAN_var = 0.;

    bool useTan2 = true;
    if( useTan2 && m_p2_ID_TAN[ m_detRegion ] != 0 ) {         
      newSmear = ( p1_ID_var * m_g3 + p2_ID_TAN_var * m_g4 * m_ptid * sinh( m_eta ) * sinh( m_eta ) );
    } else {
      newSmear = ( p1_ID_var * m_g3 + p2_ID_var * m_g4 * m_ptid );
    }
    return newSmear;
  }
  return 0;
}

} // namespace CP
