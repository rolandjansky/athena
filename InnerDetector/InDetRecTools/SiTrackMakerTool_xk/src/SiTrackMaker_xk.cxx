/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::SiTrackMaker_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "SiTrackMakerTool_xk/SiTrackMaker_xk.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "SiSPSeededTrackFinderData/SiTrackMakerEventData_xk.h"
#include "TrkCaloClusterROI/CaloClusterROI.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include <iomanip>
#include <ostream>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiTrackMaker_xk::SiTrackMaker_xk
(const std::string& t,const std::string& n,const IInterface* p)
  : base_class(t, n, p),
    m_totalInputSeeds{ },
    m_totalUsedSeeds{ },
    m_totalNoTrackPar{ },
    m_totalBremSeeds{ }, 
    m_twoClusters{ },
    m_wrongRoad{ },
    m_wrongInit{ },
    m_noTrack{ },
    m_notNewTrack{ },
    m_bremAttempt{ },
    m_outputTracks{ },
    m_extraTracks{ },
    m_bremTracks{ },
    m_seedsWithTrack{ },
    m_deSize{ },
    m_usedSeedsEta( SiCombinatorialTrackFinderData_xk::kNSeedTypes, std::vector<double>(SiCombinatorialTrackFinderData_xk::kNRapidityRanges, 0.) ),
    m_seedsWithTracksEta( SiCombinatorialTrackFinderData_xk::kNSeedTypes, std::vector<double>(SiCombinatorialTrackFinderData_xk::kNRapidityRanges, 0.) )
{

}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiTrackMaker_xk::initialize()
{
  // Get beam geometry
  //
  if (not m_beamSpotKey.empty()) {
    ATH_CHECK( m_beamSpotKey.initialize() );
  }

  if (m_fieldmode == "NoField") m_fieldModeEnum = Trk::NoField;
  else if (m_fieldmode == "MapSolenoid") m_fieldModeEnum = Trk::FastField;
  else m_fieldModeEnum = Trk::FullField;

  // Get detector elements road maker tool
  //
  if ( m_roadmaker.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_roadmaker );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "Retrieved tool " << m_roadmaker );
  }

  // Get combinatorial track finder tool
  //
  if ( m_tracksfinder.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_tracksfinder );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "Retrieved tool " << m_tracksfinder );
  }

  // Get seed to track conversion tool
  //
  if (m_seedsegmentsWrite) {
    if (m_seedtrack.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_seedtrack );
      return StatusCode::FAILURE;

    } else {
      ATH_MSG_INFO( "Retrieved tool " << m_seedtrack );
    }
  } else {
    m_seedtrack.disable();
  }

  m_heavyion = false;


  // TrackpatternRecoInfo preparation 
  //
  if        (m_patternName == "SiSpacePointsSeedMaker_Cosmic"     )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_Cosmic     );
  } else if (m_patternName == "SiSpacePointsSeedMaker_HeavyIon"   )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_HeavyIon   );
    m_heavyion = true;
  } else if (m_patternName == "SiSpacePointsSeedMaker_LowMomentum")  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_LowMomentum);
  } else if (m_patternName == "SiSpacePointsSeedMaker_BeamGas"    )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_BeamGas    );
  } else if (m_patternName == "SiSpacePointsSeedMaker_ForwardTracks"    )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_ForwardTracks);
  } else if (m_patternName == "SiSpacePointsSeedMaker_LargeD0"    )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_LargeD0    );
  } else if (m_patternName == "SiSpacePointsSeedMaker_SLHCConversionTracks")  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_SLHCConversionTracks);
  } else {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSPSeededFinder                  );
  } 

  if (m_cosmicTrack) m_seedsfilter = 3;

  ATH_CHECK(m_caloCluster.initialize(m_useBremModel && m_useCaloSeeds));
  ATH_CHECK(m_caloHad.initialize( !m_useSSSfilter && m_useHClusSeed) );

  if (m_pTmin < 20.) m_pTmin = 20.;


  for (int i=0; i!=SiCombinatorialTrackFinderData_xk::kNSeedTypes; ++i) {
    m_totalInputSeeds[i] = 0;
    m_totalNoTrackPar[i]  = 0;
    m_totalUsedSeeds[i]  = 0.;
    m_outputTracks[i]      = 0;
    m_totalBremSeeds[i]  = 0;
    m_bremTracks[i]     = 0;
    m_twoClusters[i]     = 0;
    m_wrongRoad[i]       = 0;
    m_wrongInit[i]       = 0;
    m_noTrack[i]         = 0;
    m_notNewTrack[i]     = 0;
    m_bremAttempt[i]     = 0;
    m_seedsWithTrack[i]        = 0;
    m_deSize[i]          = 0.;
    for(int j=0; j!=SiCombinatorialTrackFinderData_xk::kNRapidityRanges ;++j) { m_usedSeedsEta[i][j]=0.; m_seedsWithTracksEta[i][j]=0.;}
  }

  ////////////////////////////////////////////////////////////////////////////////
  ATH_CHECK( m_fieldCondObjInputKey.initialize());
  ////////////////////////////////////////////////////////////////////////////////
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiTrackMaker_xk::finalize()
{
  MsgStream &out = msg(MSG::INFO);
  out << "::finalize() -- statistics:" <<std::endl;
  dumpStatistics(out);
  out<<endmsg; 
  return AlgTool::finalize();
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiTrackMaker_xk::dumpStatistics(MsgStream &out) const
{

  out<<"|----------------------|--------------|--------------|--------------|--------------|--------------|"
       <<std::endl;
  out<<"| Kind of seed         |     PPP      |     PPS      |     PSS      |     SSS      |     ALL      |"
       <<std::endl;
  out<<"|----------------------|--------------|--------------|--------------|--------------|--------------|"
       <<std::endl;
  out<<"| Input  seeds         | "
     <<std::setw(12)<<m_totalInputSeeds[0]<<" | "
     <<std::setw(12)<<m_totalInputSeeds[1]<<" | "
     <<std::setw(12)<<m_totalInputSeeds[2]<<" | "
     <<std::setw(12)<<m_totalInputSeeds[3]<<" | "
     <<std::setw(12)<<(m_totalInputSeeds[0]+m_totalInputSeeds[1]+m_totalInputSeeds[2]+m_totalInputSeeds[3])
     <<" |"<<std::endl;

  out<<"| No track parameters  | "
     <<std::setw(12)<<m_totalNoTrackPar[0]<<" | " 
     <<std::setw(12)<<m_totalNoTrackPar[1]<<" | " 
     <<std::setw(12)<<m_totalNoTrackPar[2]<<" | " 
     <<std::setw(12)<<m_totalNoTrackPar[3]<<" | " 
     <<std::setw(12)<<(m_totalNoTrackPar[0]+m_totalNoTrackPar[1]+ m_totalNoTrackPar[2]+m_totalNoTrackPar[3])
     <<" |"<<std::endl;

  out<<"| Used   seeds         | "
     <<std::setw(12)<<m_totalUsedSeeds[0]<<" | " 
     <<std::setw(12)<<m_totalUsedSeeds[1]<<" | " 
     <<std::setw(12)<<m_totalUsedSeeds[2]<<" | " 
     <<std::setw(12)<<m_totalUsedSeeds[3]<<" | " 
     <<std::setw(12)<<(m_totalUsedSeeds[0]+m_totalUsedSeeds[1]+ m_totalUsedSeeds[2]+m_totalUsedSeeds[3])
     <<" |"<<std::endl;

  out<<"| Used   seeds  brem   | "
     <<std::setw(12)<<m_totalBremSeeds[0]<<" | " 
     <<std::setw(12)<<m_totalBremSeeds[1]<<" | " 
     <<std::setw(12)<<m_totalBremSeeds[2]<<" | " 
     <<std::setw(12)<<m_totalBremSeeds[3]<<" | " 
     <<std::setw(12)<<(m_totalBremSeeds[0]+m_totalBremSeeds[1]+m_totalBremSeeds[2]+m_totalBremSeeds[3]) 
     <<" |"<<std::endl;

  double tdetsize = 0.;
  int    goodseed = 0;
  for(int i=0; i!=SiCombinatorialTrackFinderData_xk::kNSeedTypes; i++) {
    tdetsize+= m_deSize[i];
    goodseed+= m_totalUsedSeeds[i];
    if(m_totalUsedSeeds[i] > 0.) m_deSize[i]= m_deSize[i]/m_totalUsedSeeds[i]; 
  }
  if(goodseed > 0) tdetsize/=double(goodseed);

  out<<"| Det elements in road | "
     <<std::setw(12)<<std::setprecision(4)<<m_deSize[0]<<" | " 
     <<std::setw(12)<<std::setprecision(5)<<m_deSize[1]<<" | " 
     <<std::setw(12)<<std::setprecision(5)<<m_deSize[2]<<" | " 
     <<std::setw(12)<<std::setprecision(5)<<m_deSize[3]<<" | " 
     <<std::setw(12)<<std::setprecision(5)<<tdetsize 
     <<" |"<<std::endl;


  out<<"| Two clusters on DE   | "
     <<std::setw(12)<<m_twoClusters[0]<<" | " 
     <<std::setw(12)<<m_twoClusters[1]<<" | " 
     <<std::setw(12)<<m_twoClusters[2]<<" | " 
     <<std::setw(12)<<m_twoClusters[3]<<" | " 
     <<std::setw(12)<<(m_twoClusters[0]+m_twoClusters[1]+m_twoClusters[2]+m_twoClusters[3]) 
     <<" |"<<std::endl;

  out<<"| Wrong DE road        | "
     <<std::setw(12)<<m_wrongRoad[0]<<" | " 
     <<std::setw(12)<<m_wrongRoad[1]<<" | " 
     <<std::setw(12)<<m_wrongRoad[2]<<" | " 
     <<std::setw(12)<<m_wrongRoad[3]<<" | " 
     <<std::setw(12)<<(m_wrongRoad[0]+m_wrongRoad[1]+m_wrongRoad[2]+m_wrongRoad[3]) 
     <<" |"<<std::endl;

  out<<"| Wrong initialization | "
     <<std::setw(12)<<m_wrongInit[0]<<" | " 
     <<std::setw(12)<<m_wrongInit[1]<<" | " 
     <<std::setw(12)<<m_wrongInit[2]<<" | " 
     <<std::setw(12)<<m_wrongInit[3]<<" | " 
     <<std::setw(12)<<(m_wrongInit[0]+m_wrongInit[1]+m_wrongInit[2]+m_wrongInit[3]) 
     <<" |"<<std::endl;

  out<<"| Can not find track   | "
     <<std::setw(12)<<m_noTrack[0]<<" | " 
     <<std::setw(12)<<m_noTrack[1]<<" | " 
     <<std::setw(12)<<m_noTrack[2]<<" | " 
     <<std::setw(12)<<m_noTrack[3]<<" | " 
     <<std::setw(12)<<(m_noTrack[0]+m_noTrack[1]+m_noTrack[2]+m_noTrack[3]) 
     <<" |"<<std::endl;

  out<<"| It is not new track  | "
     <<std::setw(12)<<m_notNewTrack[0]<<" | " 
     <<std::setw(12)<<m_notNewTrack[1]<<" | " 
     <<std::setw(12)<<m_notNewTrack[2]<<" | " 
     <<std::setw(12)<<m_notNewTrack[3]<<" | " 
     <<std::setw(12)<<(m_notNewTrack[0]+m_notNewTrack[1]+m_notNewTrack[2]+m_notNewTrack[3]) 
     <<" |"<<std::endl;

  out<<"| Attempts brem model  | "
     <<std::setw(12)<<m_bremAttempt[0]<<" | " 
     <<std::setw(12)<<m_bremAttempt[1]<<" | " 
     <<std::setw(12)<<m_bremAttempt[2]<<" | " 
     <<std::setw(12)<<m_bremAttempt[3]<<" | " 
     <<std::setw(12)<<(m_bremAttempt[0]+m_bremAttempt[1]+m_bremAttempt[2]+m_bremAttempt[3]) 
     <<" |"<<std::endl;

  out<<"| Output tracks        | "
     <<std::setw(12)<<m_outputTracks[0]<<" | "  
     <<std::setw(12)<<m_outputTracks[1]<<" | " 
     <<std::setw(12)<<m_outputTracks[2]<<" | "  
     <<std::setw(12)<<m_outputTracks[3]<<" | "  
     <<std::setw(12)<<(m_outputTracks[0]+m_outputTracks[1]+m_outputTracks[2]+m_outputTracks[3])
     <<" |"<<std::endl;

  out<<"| Output extra tracks  | "
     <<std::setw(12)<<m_extraTracks[0]<<" | "  
     <<std::setw(12)<<m_extraTracks[1]<<" | " 
     <<std::setw(12)<<m_extraTracks[2]<<" | "  
     <<std::setw(12)<<m_extraTracks[3]<<" | "  
     <<std::setw(12)<<(m_extraTracks[0]+m_extraTracks[1]+m_extraTracks[2]+m_extraTracks[3])
     <<" |"<<std::endl;

  out<<"| Output tracks brem   | "
     <<std::setw(12)<<m_bremTracks[0]<<" | "  
     <<std::setw(12)<<m_bremTracks[1]<<" | " 
     <<std::setw(12)<<m_bremTracks[2]<<" | "  
     <<std::setw(12)<<m_bremTracks[3]<<" | "  
     <<std::setw(12)<<(m_bremTracks[0]+m_bremTracks[1]+m_bremTracks[2]+m_bremTracks[3])
     <<" |"<<std::endl;

  out<<"|----------------------|--------------|--------------|--------------|--------------|--------------|--------------|"
     <<std::endl;
   out<<"| Seeds  with  track   | "
     <<std::setw(12)<<m_seedsWithTrack[0]<<" | "
     <<std::setw(12)<<m_seedsWithTrack[1]<<" | "
     <<std::setw(12)<<m_seedsWithTrack[2]<<" | "
     <<std::setw(12)<<m_seedsWithTrack[3]<<" | "
     <<std::setw(12)<<(m_seedsWithTrack[0]+m_seedsWithTrack[1]+m_seedsWithTrack[2]+m_seedsWithTrack[3])
     <<" | Number seeds |"<<std::endl;
  out<<"|----------------------|--------------|--------------|--------------|--------------|--------------|--------------|"
     <<std::endl;

  std::vector<std::pair<int,std::string>> rapidityTablePrint;
  //Defining the range values to be printed
  rapidityTablePrint.push_back(std::make_pair(0,std::string("| Track/Used   0.0-0.5 | ")));
  rapidityTablePrint.push_back(std::make_pair(1,std::string("|              0.5-1.0 | ")));
  rapidityTablePrint.push_back(std::make_pair(1,std::string("|              1.0-1.5 | ")));
  rapidityTablePrint.push_back(std::make_pair(3,std::string("|              1.5-2.0 | ")));
  rapidityTablePrint.push_back(std::make_pair(4,std::string("|              2.0-2.5 | ")));
  rapidityTablePrint.push_back(std::make_pair(5,std::string("|              2.5-3.0 | ")));
  rapidityTablePrint.push_back(std::make_pair(6,std::string("|              3.0-3.5 | ")));
  rapidityTablePrint.push_back(std::make_pair(7,std::string("|              3.5-4.0 | ")));


  for(int i=0; i!=SiCombinatorialTrackFinderData_xk::kNRapidityRanges; ++i) {
   
    std::array<double,SiCombinatorialTrackFinderData_xk::kNSeedTypes+1> pu;
    pu.fill(0);

    double totalUsedSeedsEta = 0.;

    for(int j = 0; j != SiCombinatorialTrackFinderData_xk::kNSeedTypes; ++j){

      if(m_usedSeedsEta[j][i]!=0.) pu[j]=m_seedsWithTracksEta[j][i]/m_usedSeedsEta[j][i];
      totalUsedSeedsEta += m_usedSeedsEta[j][i];

    } 

    if(totalUsedSeedsEta!=0.) {

      for(int j = 0; j != SiCombinatorialTrackFinderData_xk::kNSeedTypes; ++j){

        pu[SiCombinatorialTrackFinderData_xk::kNSeedTypes] += m_seedsWithTracksEta[j][i];
 
      }
      pu[SiCombinatorialTrackFinderData_xk::kNSeedTypes] /= totalUsedSeedsEta;
    }

    out<<rapidityTablePrint.at(i).second;
    out<<std::setw(12)<<std::setprecision(4)<<pu[0]<<" | "  
       <<std::setw(12)<<std::setprecision(4)<<pu[1]<<" | "  
       <<std::setw(12)<<std::setprecision(4)<<pu[2]<<" | "  
       <<std::setw(12)<<std::setprecision(4)<<pu[3]<<" | "  
       <<std::setw(12)<<std::setprecision(4)<<pu[4]<<" | "
       <<std::setw(12)<<static_cast<int>(m_seedsWithTracksEta[0][i])+static_cast<int>(m_seedsWithTracksEta[1][i])+static_cast<int>(m_seedsWithTracksEta[2][i])+static_cast<int>(m_seedsWithTracksEta[3][i])
       <<" |"<<std::endl;
  }


  out<<"|----------------------|--------------|--------------|--------------|--------------|--------------|--------------|"
     <<std::endl;

  return out;

}

MsgStream& InDet::SiTrackMaker_xk::dump(SiTrackMakerEventData_xk& data, MsgStream& out) const
{
  out<<std::endl;
  if (data.nprint()) return dumpevent(data, out);
  return dumpconditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiTrackMaker_xk::dumpconditions(MsgStream& out) const
{

  int n      = 62-m_tracksfinder.type().size();
  std::string s4; for (int i=0; i<n; ++i) s4.append(" "); s4.append("|");

  std::string fieldmode[9] ={"NoField"       ,"ConstantField","SolenoidalField",
                             "ToroidalField" ,"Grid3DField"  ,"RealisticField" ,
                             "UndefinedField","AthenaField"  , "?????"         };

  Trk::MagneticFieldMode fieldModeEnum(m_fieldModeEnum);

  // Get AtlasFieldCache
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (fieldCondObj) {
    MagField::AtlasFieldCache fieldCache;
    fieldCondObj->getInitializedCache(fieldCache);
    if (!fieldCache.solenoidOn()) fieldModeEnum = Trk::NoField;
  }

  Trk::MagneticFieldProperties fieldprop(fieldModeEnum);
  int mode = fieldprop.magneticFieldMode();
  if (mode<0 || mode>8 ) mode = 8; 

  n     = 62-fieldmode[mode].size();
  std::string s3; for (int i=0; i<n; ++i) s3.append(" "); s3.append("|");

  n     = 62-m_roadmaker.type().size();
  std::string s6; for (int i=0; i<n; ++i) s6.append(" "); s6.append("|");

  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  out<<"| Tool for road builder   | "<<m_roadmaker  .type() <<s6<<std::endl;
  out<<"} Tool for track finding  | "<<m_tracksfinder.type()<<s4<<std::endl;
  out<<"| Use association tool ?  | "
     <<std::setw(12)<<m_useassoTool
     << std::endl;
  out<<"| Magnetic field mode     | "<<fieldmode[mode]      <<s3<<std::endl;
  out<<"| Seeds filter level      | "<<std::setw(12)<<m_seedsfilter
     <<"                                                  |"<<std::endl;
  out<<"| Track length wrong cl.  | "<<std::setw(12)<<m_wrongcluster
     <<"                                                  |"<<std::endl;
  out<<"| Min pT of track (MeV)   | "<<std::setw(12)<<std::setprecision(5)<<m_pTmin
     <<"                                                  |"<<std::endl;
  out<<"| Max Xi2 for cluster     | "<<std::setw(12)<<std::setprecision(5)<<m_xi2max
     <<"                                                  |"<<std::endl;
  out<<"| Max Xi2 for outlayer    | "<<std::setw(12)<<std::setprecision(5)<<m_xi2maxNoAdd
     <<"                                                  |"<<std::endl;
  out<<"| Max Xi2 for link        | "<<std::setw(12)<<std::setprecision(5)<<m_xi2maxlink
     <<"                                                  |"<<std::endl;
  out<<"| Min number of clusters  | "<<std::setw(12)<<m_nclusmin
     <<"                                                  |"<<std::endl;
  out<<"| Min number of wclusters | "<<std::setw(12)<<m_nwclusmin
     <<"                                                  |"<<std::endl;
  out<<"| Max number holes        | "<<std::setw(12)<<m_nholesmax
     <<"                                                  |"<<std::endl;
  out<<"| Max holes  gap          | "<<std::setw(12)<<m_dholesmax
     <<"                                                  |"<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  return out;
}


///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiTrackMaker_xk::dumpevent(SiTrackMakerEventData_xk& data, MsgStream& out) const
{
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Number input seeds      | "<<std::setw(12)<<data.inputseeds()
     <<"                              |"<<std::endl;
  out<<"| Number good  seeds      | "<<std::setw(12)<<data.goodseeds()
     <<"                              |"<<std::endl;
  out<<"| Number output tracks    | "<<std::setw(12)<<data.findtracks() 
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Initiate track finding tool for new event
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::newEvent(const EventContext& ctx, SiTrackMakerEventData_xk& data, bool PIX, bool SCT) const
{
  data.xybeam()[0] = 0.;
  data.xybeam()[1] = 0.;
  if (not m_beamSpotKey.empty()) {
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
    if (beamSpotHandle.isValid()) {
      data.xybeam()[0] = beamSpotHandle->beamPos()[0];
      data.xybeam()[1] = beamSpotHandle->beamPos()[1];
    }
  }
  
  data.pix() = PIX and m_usePix;
  data.sct() = SCT and m_useSct;
  bool simpleTrack = false;

  InDet::TrackQualityCuts trackquality = setTrackQualityCuts(simpleTrack);

  // New event for track finder tool
  //
  m_tracksfinder->newEvent(ctx, data.combinatorialData(), m_trackinfo, trackquality);

  // Erase cluster to track association
  //
  if (m_seedsfilter) data.clusterTrack().clear();

  // Erase statistic information
  //
  data.inputseeds() = 0;
  data.goodseeds()  = 0;
  data.findtracks() = 0;
  for(int i=0; i!=SiCombinatorialTrackFinderData_xk::kNStatAllTypes; ++i) { for(int k = 0; k!=SiCombinatorialTrackFinderData_xk::kNSeedTypes; ++k) data.summaryStatAll()[i][k]; }
  for(int i=0; i!=SiCombinatorialTrackFinderData_xk::kNStatEtaTypes; ++i) { for(int k = 0; k!=SiCombinatorialTrackFinderData_xk::kNSeedTypes; ++k) { for(int r=0; r!=SiCombinatorialTrackFinderData_xk::kNRapidityRanges; ++r) data.summaryStatUsedInTrack()[i][k][r]; } }  

  // Retrieve 
  //
  if (m_useBremModel && m_useCaloSeeds) {

    data.caloF().clear();
    data.caloR().clear();
    data.caloZ().clear();

    if (!m_caloCluster.key().empty()) {
      SG::ReadHandle<CaloClusterROI_Collection> calo_cluster(m_caloCluster, ctx);
      if (calo_cluster.isValid()) {
        for (const Trk::CaloClusterROI *c : * calo_cluster) {
          data.caloF().push_back( c->globalPosition().phi ());
          data.caloR().push_back( c->globalPosition().perp());
          data.caloZ().push_back( c->globalPosition().z   ());
        }
      }
    }
  }

  if (!m_useSSSfilter && m_useHClusSeed) {
    data.hadF().clear();
    data.hadR().clear();
    data.hadZ().clear();

    if (!m_caloHad.key().empty()) {
      SG::ReadHandle<CaloClusterROI_Collection> calo_had(m_caloHad, ctx);
      if (calo_had.isValid()) {
        for (const Trk::CaloClusterROI *c : * calo_had) {
          data.hadF().push_back( c->globalPosition().phi ());
          data.hadR().push_back( c->globalPosition().perp());
          data.hadZ().push_back( c->globalPosition().z   ());
        }
      }
    }
  }
  if (m_seedsegmentsWrite) m_seedtrack->newEvent(data.conversionData(), m_trackinfo, m_patternName);
}

///////////////////////////////////////////////////////////////////
// Initiate track finding tool for new event
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::newTrigEvent(const EventContext& ctx, SiTrackMakerEventData_xk& data, bool PIX, bool SCT) const
{
  data.pix()        = PIX && m_usePix;
  data.sct()        = SCT && m_useSct;
  bool simpleTrack  = true;

  InDet::TrackQualityCuts trackquality = setTrackQualityCuts(simpleTrack);

  // New event for track finder tool
  //
  m_tracksfinder->newEvent(ctx, data.combinatorialData(), m_trackinfo, trackquality);

  // Erase cluster to track association
  //
  if (m_seedsfilter) data.clusterTrack().clear(); 

  // Erase statistic information
  //
  data.inputseeds() = 0;
  data.goodseeds()  = 0;
  data.findtracks() = 0;
  for(int i=0; i!=SiCombinatorialTrackFinderData_xk::kNStatAllTypes; ++i) { for(int k = 0; k!=SiCombinatorialTrackFinderData_xk::kNSeedTypes; ++k) data.summaryStatAll()[i][k]; }
  for(int i=0; i!=SiCombinatorialTrackFinderData_xk::kNStatEtaTypes; ++i) { for(int k = 0; k!=SiCombinatorialTrackFinderData_xk::kNSeedTypes; ++k) { for(int r=0; r!=SiCombinatorialTrackFinderData_xk::kNRapidityRanges; ++r) data.summaryStatUsedInTrack()[i][k][r]; } }

}

///////////////////////////////////////////////////////////////////
// Finalize track finding tool for given event
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::endEvent(SiTrackMakerEventData_xk& data) const
{
  // End event for track finder tool
  //
  m_tracksfinder->endEvent(data.combinatorialData());

  //correction to exclude memory fragmentation
  data.clusterTrack().clear();

  // end event for seed to track tool
  if (m_seedsegmentsWrite) m_seedtrack->endEvent(data.conversionData());

  // fill statistics
  {
    std::lock_guard<std::mutex> lock(m_counterMutex);

    for(int K = 0; K != SiCombinatorialTrackFinderData_xk::kNSeedTypes; ++K) {
      for(int r = 0; r != 8; ++r) {
        m_usedSeedsEta[K][r] += data.summaryStatUsedInTrack()[kUsedSeedsEta][K][r];
        m_seedsWithTracksEta[K][r] += data.summaryStatUsedInTrack()[kSeedsWithTracksEta][K][r];
      }
    }
  }
    for (int K = 0; K != SiCombinatorialTrackFinderData_xk::kNSeedTypes; ++K) { 
      m_totalInputSeeds[K] +=  data.summaryStatAll()[kTotalInputSeeds][K]; 
      m_totalUsedSeeds[K] = m_totalUsedSeeds[K] + data.summaryStatAll()[kTotalUsedSeeds][K];
      m_totalNoTrackPar[K] += data.summaryStatAll()[kTotalNoTrackPar][K];
      m_totalBremSeeds[K] += data.summaryStatAll()[kTotalBremSeeds][K];
      m_twoClusters[K] += data.summaryStatAll()[kTwoClusters][K];
      m_wrongRoad[K] += data.summaryStatAll()[kWrongRoad][K];
      m_wrongInit[K] += data.summaryStatAll()[kWrongInit][K];
      m_noTrack[K] += data.summaryStatAll()[kNoTrack][K];
      m_notNewTrack[K] += data.summaryStatAll()[kNotNewTrack][K];
      m_bremAttempt[K] += data.summaryStatAll()[kBremAttempt][K];
      m_outputTracks[K] += data.summaryStatAll()[kOutputTracks][K];
      m_extraTracks[K] += data.summaryStatAll()[kExtraTracks][K];
      m_bremTracks[K] += data.summaryStatAll()[kBremTracks][K];
      m_seedsWithTrack[K] += data.summaryStatAll()[kSeedsWithTracks][K];
      m_deSize[K] = m_deSize[K] + data.summaryStatAll()[kDESize][K];

  }
  // Print event information 
  //
  if (msgLevel()<=0) {
    data.nprint() = 1;
    dump(data, msg(MSG::DEBUG));
  }
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

std::list<Trk::Track*> InDet::SiTrackMaker_xk::getTracks
(const EventContext& ctx, SiTrackMakerEventData_xk& data, const std::vector<const Trk::SpacePoint*>& Sp) const
{
  ++data.inputseeds();
  
  int K = kindSeed(Sp);     
  int r = rapidity(Sp); 

  ++data.summaryStatAll()[kTotalInputSeeds][K];

  std::list<Trk::Track*> tracks;
  if (!data.pix() && !data.sct()) return tracks;
  
  bool good;
  !m_seedsfilter ? good=true : good=newSeed(data, Sp);  

  if (!good) return tracks;

  data.dbm() = isDBMSeeds(*Sp.begin());

  // Get AtlasFieldCache
  MagField::AtlasFieldCache fieldCache;

  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (fieldCondObj == nullptr) {
      ATH_MSG_ERROR("InDet::SiTrackMaker_xk::getTracks: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
      return tracks;
  }
  fieldCondObj->getInitializedCache (fieldCache);
  
  // Get initial parameters estimation
  //
  bool sss = false;
  const Trk::TrackParameters* Tp = nullptr;
  if (data.dbm()) Tp = getAtaPlaneDBM(fieldCache, data, Sp);
  else Tp = getAtaPlane(fieldCache, data, sss && m_useHClusSeed, Sp);
  if (!Tp) 
  {
     ++data.summaryStatAll()[kTotalNoTrackPar][K];
     return tracks;
  }
  ++data.goodseeds(); 

  // Get detector elements road
  //
  std::list<const InDetDD::SiDetectorElement*> DE;
  if (!m_cosmicTrack) m_roadmaker->detElementsRoad(ctx, fieldCache, *Tp,Trk::alongMomentum,   DE, data.roadMakerData());
  else                m_roadmaker->detElementsRoad(ctx, fieldCache, *Tp,Trk::oppositeMomentum,DE, data.roadMakerData());

  if (!data.pix() || !data.sct() || data.dbm()) detectorElementsSelection(data, DE);

  if ( static_cast<int>(DE.size())  <   m_nclusmin) {
    delete Tp;
    return tracks;
  }

  data.summaryStatAll()[kDESize][K] += double(DE.size());
  ++data.summaryStatAll()[kTotalUsedSeeds][K];

  std::list<Amg::Vector3D> Gp;

  ++data.summaryStatUsedInTrack()[kUsedSeedsEta][K][r];

  // Find possible list of tracks using space points space points information
  //
  if (!m_useBremModel) {
    tracks = m_tracksfinder->getTracks        (data.combinatorialData(), *Tp, Sp, Gp, DE, data.clusterTrack(),ctx);
  } else if (!m_useCaloSeeds) {
    ++data.summaryStatAll()[kTotalBremSeeds][K];
    tracks = m_tracksfinder->getTracksWithBrem(data.combinatorialData(), *Tp, Sp, Gp, DE, data.clusterTrack(), false,ctx);
  } else if (isCaloCompatible(data)) {
    ++data.summaryStatAll()[kTotalBremSeeds][K];
    tracks = m_tracksfinder->getTracksWithBrem(data.combinatorialData(), *Tp, Sp, Gp, DE, data.clusterTrack(), true,ctx);
  } else {
    tracks = m_tracksfinder->getTracks        (data.combinatorialData(), *Tp, Sp, Gp, DE, data.clusterTrack(),ctx);
  }

  std::array<bool,SiCombinatorialTrackFinderData_xk::kNCombStats> inf{0,0,0,0,0,0};   m_tracksfinder->fillStatistic(data.combinatorialData(),inf);                                      
  for (size_t p =0; p<inf.size(); ++p){
    if(inf[p]) ++data.summaryStatAll()[m_indexToEnum[p]][K];
  } 

  if (m_seedsfilter) {
    std::list<Trk::Track*>::iterator t = tracks.begin();
    while (t!=tracks.end()) {
      if (!isNewTrack(data, *t)) {
        delete (*t);
        tracks.erase(t++);
      } else {
        if((*t)->info().trackProperties(Trk::TrackInfo::BremFit)) ++data.summaryStatAll()[kBremTracks][K];
        clusterTrackMap(data, *t++);
      }
    }
  }
  data.findtracks() += tracks.size();

  if(tracks.size() != 0) {
    ++data.summaryStatAll()[kSeedsWithTracks][K];
    ++data.summaryStatUsedInTrack()[kSeedsWithTracksEta][K][r];
    data.summaryStatAll()[kOutputTracks][K] += tracks.size();
    data.summaryStatAll()[kExtraTracks][K] += (tracks.size()-1);
  }                       

  // Call seed to track execution
  //
  if (m_seedsegmentsWrite) m_seedtrack->executeSiSPSeedSegments(data.conversionData(), Tp, tracks.size(), Sp);

  delete Tp;
  return tracks;

}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

std::list<Trk::Track*> InDet::SiTrackMaker_xk::getTracks
(const EventContext& ctx, SiTrackMakerEventData_xk& data, const Trk::TrackParameters& Tp, const std::list<Amg::Vector3D>& Gp) const
{
  ++data.inputseeds();
  std::list<Trk::Track*> tracks;
  if (!data.pix() && !data.sct()) return tracks;

  ++data.goodseeds();

  // Get AtlasFieldCache
  MagField::AtlasFieldCache fieldCache;

  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (fieldCondObj == nullptr) {
      ATH_MSG_ERROR("InDet::SiTrackMaker_xk::getTracks: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
      return tracks;
  }
  fieldCondObj->getInitializedCache (fieldCache);
  
  // Get detector elements road
  //
  std::list<const InDetDD::SiDetectorElement*> DE;
  if (!m_cosmicTrack) m_roadmaker->detElementsRoad(ctx, fieldCache, Tp,Trk::alongMomentum,   DE, data.roadMakerData());
  else                m_roadmaker->detElementsRoad(ctx, fieldCache, Tp,Trk::oppositeMomentum,DE, data.roadMakerData());

  if (!data.pix() || !data.sct()) detectorElementsSelection(data, DE);

  if (static_cast<int>(DE.size()) < m_nclusmin) return tracks; 

  // Find possible list of tracks with trigger track parameters or global positions
  //
  std::vector<const Trk::SpacePoint*>  Sp;
  
  if (!m_useBremModel) {
    tracks = m_tracksfinder->getTracks        (data.combinatorialData(), Tp, Sp, Gp, DE, data.clusterTrack(),ctx);
  } else if (!m_useCaloSeeds) {
   tracks = m_tracksfinder->getTracksWithBrem(data.combinatorialData(), Tp, Sp, Gp, DE, data.clusterTrack(), false,ctx);
  } else if (isCaloCompatible(data)) {
    tracks = m_tracksfinder->getTracksWithBrem(data.combinatorialData(), Tp, Sp, Gp, DE, data.clusterTrack(), true,ctx);
  } else {
    tracks = m_tracksfinder->getTracks        (data.combinatorialData(), Tp, Sp, Gp, DE, data.clusterTrack(),ctx);
  }
  
  if (m_seedsfilter) {
    std::list<Trk::Track*>::iterator t = tracks.begin();
    while (t!=tracks.end()) {
      if (!isNewTrack(data, *t)) {
        delete (*t);
        tracks.erase(t++);
      } else {
        clusterTrackMap(data, *t++);
      }
    }
  }
  data.findtracks() += tracks.size();
  return tracks;
}

///////////////////////////////////////////////////////////////////
// Space point seed parameters extimation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* InDet::SiTrackMaker_xk::getAtaPlane
(MagField::AtlasFieldCache& fieldCache,
 SiTrackMakerEventData_xk& data,
 bool sss,
 const std::vector<const Trk::SpacePoint*>& SP) const
{
  if (SP.size() < 3) return nullptr;

  const Trk::PrepRawData* cl  = SP[0]->clusterList().first;
  if (!cl) return nullptr;
  const Trk::PlaneSurface* pla = 
    static_cast<const Trk::PlaneSurface*>(&cl->detectorElement()->surface());
  if (!pla) return nullptr;

  double p0[3],p1[3],p2[3]; 
  if (!globalPositions(*(SP[0]),*(SP[1]),*(SP[2]),p0,p1,p2)) return nullptr;

  double x0 = p0[0]   ;
  double y0 = p0[1]   ;
  double z0 = p0[2]   ;
  double x1 = p1[0]-x0;
  double y1 = p1[1]-y0;
  double x2 = p2[0]-x0;
  double y2 = p2[1]-y0;
  double z2 = p2[2]-z0; 

  double u1 = 1./sqrt(x1*x1+y1*y1)       ;
  double rn = x2*x2+y2*y2                ;
  double r2 = 1./rn                      ;
  double a  = x1*u1                      ;
  double b  = y1*u1                      ;
  double u2 = (a*x2+b*y2)*r2             ;
  double v2 = (a*y2-b*x2)*r2             ;
  double A  = v2/(u2-u1)                 ;
  double B  = 2.*(v2-A*u2)               ;
  double C  = B/sqrt(1.+A*A)             ;
  double T  = z2*sqrt(r2)/(1.+.04*C*C*rn);
  
  const Amg::Transform3D& Tp = pla->transform();

  double Ax[3] = {Tp(0,0),Tp(1,0),Tp(2,0)}; 
  double Ay[3] = {Tp(0,1),Tp(1,1),Tp(2,1)}; 
  double D [3] = {Tp(0,3),Tp(1,3),Tp(2,3)}; 
  
  double   d[3] = {x0-D[0],y0-D[1],z0-D[2]};

  data.par()[0] = d[0]*Ax[0]+d[1]*Ax[1]+d[2]*Ax[2];
  data.par()[1] = d[0]*Ay[0]+d[1]*Ay[1]+d[2]*Ay[2];

  Trk::MagneticFieldMode fieldModeEnum(m_fieldModeEnum);
  if (!fieldCache.solenoidOn()) fieldModeEnum = Trk::NoField;
  Trk::MagneticFieldProperties fieldprop(fieldModeEnum);
  if (fieldprop.magneticFieldMode() > 0) {

    double H[3],gP[3] ={x0,y0,z0};
    fieldCache.getFieldZR(gP, H);

    if (fabs(H[2])>.0001) {
      data.par()[2] = atan2(b+a*A,a-b*A);
      data.par()[3] = atan2(1.,T)       ;  
      data.par()[5] = -C/(300.*H[2])    ;
    } else {
      T    =  z2*sqrt(r2)  ;
      data.par()[2] = atan2(y2,x2);
      data.par()[3] = atan2(1.,T) ;
      data.par()[5] = 1./m_pTmin  ;
    }
  } else {
    T    = z2*sqrt(r2)   ;
    data.par()[2] = atan2(y2,x2);
    data.par()[3] = atan2(1.,T) ;
    data.par()[5] = 1./m_pTmin  ;
  }
  
  if (fabs(data.par()[5])*m_pTmin > 1.1) return nullptr;
  data.par()[4] = data.par()[5]/sqrt(1.+T*T);
  data.par()[6] = x0                              ;
  data.par()[7] = y0                              ;
  data.par()[8] = z0                              ;

  if (sss && !isHadCaloCompatible(data)) return nullptr;

  return pla->createTrackParameters(data.par()[0],data.par()[1],data.par()[2],data.par()[3],data.par()[4],0); 
}

///////////////////////////////////////////////////////////////////
// Space point seed parameters extimation for DBM seed
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* InDet::SiTrackMaker_xk::getAtaPlaneDBM
(MagField::AtlasFieldCache& fieldCache,
 SiTrackMakerEventData_xk& data,
 const std::vector<const Trk::SpacePoint*>& SP) const
{
  if (SP.size() < 3) return nullptr;

  const Trk::PrepRawData* cl  = SP[0]->clusterList().first;
  if (!cl) return nullptr;
  const Trk::PlaneSurface* pla = 
    static_cast<const Trk::PlaneSurface*>(&cl->detectorElement()->surface());
  if (!pla) return nullptr;

  double p0[3],p1[3],p2[3]; 
  if (!globalPositions(*(SP[0]),*(SP[1]),*(SP[2]),p0,p1,p2)) return nullptr;

  double x0 = data.xybeam()[0]-p0[0];
  double y0 = data.xybeam()[1]-p0[1];
  double x2 = p2[0]      -p0[0];
  double y2 = p2[1]      -p0[1];
  double z2 = p2[2]      -p0[2];

  double u1 = -1./sqrt(x0*x0+y0*y0);
  double rn = x2*x2+y2*y2          ;
  double r2 = 1./rn                ;
  double a  =  x0*u1               ;
  double b  =  y0*u1               ;
  double u2 = (a*x2+b*y2)*r2       ;
  double v2 = (a*y2-b*x2)*r2       ;
  double A  = v2/(u2-u1)           ;
  double B  = 2.*(v2-A*u2)         ;
  double C  = B/sqrt(1.+A*A)       ;
  double T  = z2*sqrt(r2)          ;
  
  const Amg::Transform3D& Tp = pla->transform();

  double Ax[3] = {Tp(0,0),Tp(1,0),Tp(2,0)}; 
  double Ay[3] = {Tp(0,1),Tp(1,1),Tp(2,1)}; 
  double D [3] = {Tp(0,3),Tp(1,3),Tp(2,3)}; 
  
  double   d[3] = {p0[0]-D[0],p0[1]-D[1],p0[2]-D[2]};

  data.par()[0] = d[0]*Ax[0]+d[1]*Ax[1]+d[2]*Ax[2];
  data.par()[1] = d[0]*Ay[0]+d[1]*Ay[1]+d[2]*Ay[2];

  Trk::MagneticFieldMode fieldModeEnum(m_fieldModeEnum);
  if (!fieldCache.solenoidOn()) fieldModeEnum = Trk::NoField;
  Trk::MagneticFieldProperties fieldprop(fieldModeEnum);
  if (fieldprop.magneticFieldMode() > 0) {
    double H[3],gP[3] ={p0[0],p0[1],p0[2]};

    fieldCache.getFieldZR(gP, H);

    if (fabs(H[2])>.0001) {
      data.par()[2] = atan2(b+a*A,a-b*A);
      data.par()[3] = atan2(1.,T)       ;  
      data.par()[5] = -C/(300.*H[2])    ;
    } else {
      T    =  z2*sqrt(r2)  ;
      data.par()[2] = atan2(y2,x2);
      data.par()[3] = atan2(1.,T) ;
      data.par()[5] = 1./1000.    ;
    }
  } else {
    T    = z2*sqrt(r2)   ;
    data.par()[2] = atan2(y2,x2);
    data.par()[3] = atan2(1.,T) ;
    data.par()[5] = 1./1000     ;
  }

  if (fabs(data.par()[5])*20. > 1.1) return nullptr;
  data.par()[4] = data.par()[5]/sqrt(1.+T*T);
  data.par()[6] = p0[0]                           ;
  data.par()[7] = p0[1]                           ;
  data.par()[8] = p0[2]                           ;
  return pla->createTrackParameters(data.par()[0],data.par()[1],data.par()[2],data.par()[3],data.par()[4],0); 
}

///////////////////////////////////////////////////////////////////
// Set track quality cuts
///////////////////////////////////////////////////////////////////

InDet::TrackQualityCuts InDet::SiTrackMaker_xk::setTrackQualityCuts(bool simpleTrack) const
{
  InDet::TrackQualityCuts trackquality;
  // Integer cuts
  //
  trackquality.setIntCut("MinNumberOfClusters" ,m_nclusmin   );
  trackquality.setIntCut("MinNumberOfWClusters",m_nwclusmin  );
  trackquality.setIntCut("MaxNumberOfHoles"    ,m_nholesmax  ); 
  trackquality.setIntCut("MaxHolesGae"         ,m_dholesmax  ); 

  if (m_useassoTool)                  trackquality.setIntCut("UseAssociationTool",1);
  else                                trackquality.setIntCut("UseAssociationTool",0);
  if (m_cosmicTrack)                  trackquality.setIntCut("CosmicTrack"       ,1); 
  else                                trackquality.setIntCut("CosmicTrack"       ,0);
  if (simpleTrack)                    trackquality.setIntCut("SimpleTrack"       ,1);
  else                                trackquality.setIntCut("SimpleTrack"       ,0);
  if (m_multitracks)                  trackquality.setIntCut("doMultiTracksProd" ,1);
  else                                trackquality.setIntCut("doMultiTracksProd" ,0);

  // Double cuts
  //
  trackquality.setDoubleCut("pTmin"              ,m_pTmin      );
  trackquality.setDoubleCut("pTminBrem"          ,m_pTminBrem  );
  trackquality.setDoubleCut("MaxXi2forCluster"   ,m_xi2max     );
  trackquality.setDoubleCut("MaxXi2forOutlier"   ,m_xi2maxNoAdd);
  trackquality.setDoubleCut("MaxXi2forSearch"    ,m_xi2maxlink );
  trackquality.setDoubleCut("MaxXi2MultiTracks"  ,m_xi2multitracks);

  return trackquality;
}

///////////////////////////////////////////////////////////////////
// Detector elements selection
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::detectorElementsSelection(SiTrackMakerEventData_xk& data,
                                                       std::list<const InDetDD::SiDetectorElement*>& DE) const
{
  std::list<const InDetDD::SiDetectorElement*>::iterator d = DE.begin();
  if (!data.dbm()) {
    while (d!=DE.end()) {
      if ((*d)->isPixel()) {
        if (!data.pix()) {
          d = DE.erase(d);
          continue;
        }
      } else if (!data.sct()) {
        d = DE.erase(d);
        continue;
      }
      ++d;
    }
  } else {
    while (d!=DE.end()) {
      if (!(*d)->isDBM() ) {
        if ((*d)->isSCT() || (*d)->isEndcap()) {
          d = DE.erase(d);
          continue;
        }
        const Amg::Transform3D& T = (*d)->surface().transform(); 
        if (T(0,3)*T(0,3)+T(1,3)*T(1,3) > (43.*43) ) {
          d = DE.erase(d);
          continue;
        }
      }
      ++d;
    }
  }
} 

///////////////////////////////////////////////////////////////////
// Clusters from seed comparison with clusters associated with track
//
// Seeds is good only if no tracks has hits in all clusters from given seed (nt!=n)
// where, n  - number clusters in seed
//        nt - number clusters with track information
//
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::newSeed(SiTrackMakerEventData_xk& data, const std::vector<const Trk::SpacePoint*>& Sp) const
{
  std::multiset<const Trk::Track*> trackseed;
  std::multimap<const Trk::PrepRawData*,const Trk::Track*>::const_iterator pt,pte = data.clusterTrack().end();
  std::vector<const Trk::SpacePoint*>::const_iterator s=Sp.begin(),se=Sp.end();

  size_t n = 0;
  for (;s!=se; ++s) {

    const Trk::PrepRawData* p = (*s)->clusterList().first; 
   
    for (pt = data.clusterTrack().find(p); pt!=pte; ++pt) {
      if ((*pt).first!=p) break;
      trackseed.insert((*pt).second);
    }
    ++n;
    p = (*s)->clusterList().second;
    if (!p) continue;

    for (pt = data.clusterTrack().find(p); pt!=pte; ++pt) {
      if ((*pt).first!=p) break;
      trackseed.insert((*pt).second);
    }
    ++n;

  }
   if(trackseed.size() < n) return true;
   if( m_heavyion && n==3 ) return true;
    
  std::multiset<const Trk::Track*>::iterator t = trackseed.begin(), te = trackseed.end();

  const Trk::Track* tr  = (*t)                             ;
  
  size_t               nt  = 1                                ;

   for(++t; t!=te; ++t) {

    if((*t) != tr) {
      tr = (*t);
      nt =    1;
      continue ;
    }
    if(++nt == n) return false;
  }
  return nt!=n;
}

///////////////////////////////////////////////////////////////////
//// Kind of seed calculation 0-PPP, 1-PPS, 2-PSS, 3-SSS another -1
/////////////////////////////////////////////////////////////////////


int InDet::SiTrackMaker_xk::kindSeed(const std::vector<const Trk::SpacePoint*>& Sp) const
{
  if(Sp.size()!=3) return 4;
  
  std::vector<const Trk::SpacePoint*>::const_iterator s=Sp.begin(),se=Sp.end();
  
  int n = 0;
  for(; s!=se; ++s) {if((*s)->clusterList().second) ++n;}
  return n;
}

int InDet::SiTrackMaker_xk::rapidity(const std::vector<const Trk::SpacePoint*>& Sp) const
{ 
  if(Sp.size() < 2) return 0;

  //std::vector<const Trk::SpacePoint*>::const_iterator s = Sp.begin();
  
  double x = Sp[0]->globalPosition().x(); 
  double y = Sp[0]->globalPosition().y();
  double z = Sp[0]->globalPosition().z();
  //++s;
  x = Sp[1]->globalPosition().x()-x; 
  y = Sp[1]->globalPosition().y()-y;
  z = Sp[1]->globalPosition().z()-z;
  int n = int(2.*std::abs(log(tan(.5*atan2(sqrt(x*x+y*y),z)))));
  if(n > 7) n = 7;
  return n;
}

///////////////////////////////////////////////////////////////////
// Clusters-track multimap production
///////////////////////////////////////////////////////////////////

void  InDet::SiTrackMaker_xk::clusterTrackMap(SiTrackMakerEventData_xk& data, Trk::Track* Tr) const
{
  DataVector<const Trk::MeasurementBase>::const_iterator 
    m  = Tr->measurementsOnTrack()->begin(), 
    me = Tr->measurementsOnTrack()->end  ();

  for (; m!=me; ++m) {
    const Trk::PrepRawData* prd = static_cast<const Trk::RIO_OnTrack*>(*m)->prepRawData();
    if (prd) data.clusterTrack().insert(std::make_pair(prd, Tr));
  }
}
 
///////////////////////////////////////////////////////////////////
// Test is it new track
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::isNewTrack(SiTrackMakerEventData_xk& data, Trk::Track* Tr) const
{
  const Trk::PrepRawData* prd   [100];
  std::multimap<const Trk::PrepRawData*,const Trk::Track*>::const_iterator 
    ti,t[100],te = data.clusterTrack().end();

  int n = 0;

  DataVector<const Trk::MeasurementBase>::const_iterator 
    m  = Tr->measurementsOnTrack()->begin(), 
    me = Tr->measurementsOnTrack()->end  ();

  for (; m!=me; ++m) {
  
    const Trk::PrepRawData* pr = static_cast<const Trk::RIO_OnTrack*>(*m)->prepRawData();
    if (pr) {
      prd[n] =pr;
      t  [n] = data.clusterTrack().find(prd[n]);
      if (t[n]==te) return true;
      ++n;
    }
  }

  if (!n) return true;
  int nclt = n;
  
  for (int i=0; i!=n; ++i) {
    int nclmax = 0;
    for (ti=t[i]; ti!=te; ++ti) {
      if ( (*ti).first != prd[i] ) break;
      int ncl = (*ti).second->measurementsOnTrack()->size();
      if (ncl > nclmax) nclmax = ncl;
    }   
    if (nclt > nclmax) return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Calculation global position for space points
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::globalPositions
(const Trk::SpacePoint& s0, const Trk::SpacePoint& s1, const Trk::SpacePoint& s2,
 double* p0, double* p1, double* p2) const
{

  p0[0] = s0.globalPosition().x();
  p0[1] = s0.globalPosition().y();
  p0[2] = s0.globalPosition().z();

  p1[0] = s1.globalPosition().x();
  p1[1] = s1.globalPosition().y();
  p1[2] = s1.globalPosition().z();

  p2[0] = s2.globalPosition().x();
  p2[1] = s2.globalPosition().y();
  p2[2] = s2.globalPosition().z();
 
  if (!s0.clusterList().second && !s1.clusterList().second && !s2.clusterList().second) return true;

  double dir0[3],dir1[3],dir2[3]; 

  globalDirections(p0,p1,p2,dir0,dir1,dir2);

  if (s0.clusterList().second && !globalPosition(s0,dir0,p0)) return false;
  if (s1.clusterList().second && !globalPosition(s1,dir1,p1)) return false;
  if (s2.clusterList().second && !globalPosition(s2,dir2,p2)) return false;
  
  return true;
}

///////////////////////////////////////////////////////////////////
// Calculation global position for space points
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::globalPosition
(const Trk::SpacePoint& sp, double* dir,double* p) const
{
  const Trk::PrepRawData*  c0  = sp.clusterList().first;
  const Trk::PrepRawData*  c1  = sp.clusterList().second;
 
  const InDetDD::SiDetectorElement* de0 = static_cast<const InDet::SiCluster*>(c0)->detectorElement(); 
  const InDetDD::SiDetectorElement* de1 = static_cast<const InDet::SiCluster*>(c1)->detectorElement(); 

  Amg::Vector2D localPos = c0->localPosition();  
  std::pair<Amg::Vector3D,Amg::Vector3D> e0
    (de0->endsOfStrip(InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0.))); 
 
  localPos = c1->localPosition();                
  std::pair<Amg::Vector3D,Amg::Vector3D> e1
    (de1->endsOfStrip(InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0.))); 
 
  double a0[3] = {e0.second.x()-e0.first.x(), e0.second.y()-e0.first.y(), e0.second.z()-e0.first.z()};
  double a1[3] = {e1.second.x()-e1.first.x(), e1.second.y()-e1.first.y(), e1.second.z()-e1.first.z()};
  double dr[3] = {e1.first .x()-e0.first.x(), e1.first .y()-e0.first.y(), e1.first .z()-e0.first.z()};
  
  double d0    = m_distmax/sqrt(a0[0]*a0[0]+a0[1]*a0[1]+a0[2]*a0[2]);

  // u = a1 x dir and  v = a0 x dir
  //
  double u[3]  = {a1[1]*dir[2]-a1[2]*dir[1],a1[2]*dir[0]-a1[0]*dir[2],a1[0]*dir[1]-a1[1]*dir[0]};
  double v[3]  = {a0[1]*dir[2]-a0[2]*dir[1],a0[2]*dir[0]-a0[0]*dir[2],a0[0]*dir[1]-a0[1]*dir[0]};

  double du    = a0[0]*u[0]+a0[1]*u[1]+a0[2]*u[2];

  if (du==0. ) return false;
  
  //these need checking; original code calculated dv and used
  //s1 = -(dr[0]*v[0]+dr[1]*v[1]+dr[2]*v[2])/dv;
  double s0 = (dr[0]*u[0]+dr[1]*u[1]+dr[2]*u[2])/du;
  double s1 = (dr[0]*v[0]+dr[1]*v[1]+dr[2]*v[2])/du;

  if (s0 < -d0 || s0 > 1.+d0 ||  s1 < -d0 || s1 > 1.+d0) return false;

  p[0] = e0.first.x()+s0*a0[0]; 
  p[1] = e0.first.y()+s0*a0[1]; 
  p[2] = e0.first.z()+s0*a0[2];

  return true;
}

///////////////////////////////////////////////////////////////////
// Test is it track with calo seed compatible
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::isCaloCompatible(SiTrackMakerEventData_xk& data) const
{
  const double pi = M_PI, pi2 = 2.*M_PI;

  if (data.caloF().empty()) return false;   

  std::list<double>::const_iterator f = data.caloF().begin(), fe = data.caloF().end();
  std::list<double>::const_iterator r = data.caloR().begin();
  std::list<double>::const_iterator z = data.caloZ().begin();

  double F = data.par()[2]                           ;
  double E = -log(tan(.5*data.par()[3]))             ;
  double R = sqrt(data.par()[6]*data.par()[6]+data.par()[7]*data.par()[7]);
  double Z = data.par()[8]                           ;                           
  
  for (; f!=fe; ++f) {
    double df = fabs(F-(*f));
    if (df > pi) df = fabs(pi2-df);
    if (df < m_phiWidth) {
      double dR = (*r)-R;
      double dZ = (*z)-Z;
      if (fabs(E-atanh(dZ/sqrt(dR*dR+dZ*dZ)) ) < m_etaWidth) return true;
    }
    ++r; ++z;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Test track is compatible withi had calo seed
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::isHadCaloCompatible(SiTrackMakerEventData_xk& data) const
{
  const double pi = M_PI, pi2 = 2.*M_PI;

  if (data.hadF().empty() || fabs(data.par()[5])*m_pTminSSS > 1.) return false;   

  std::list<double>::const_iterator f = data.hadF().begin(), fe = data.hadF().end();
  std::list<double>::const_iterator r = data.hadR().begin();
  std::list<double>::const_iterator z = data.hadZ().begin();

  double F = data.par()[2]                           ;
  double E = -log(tan(.5*data.par()[3]))             ;
  double R = sqrt(data.par()[6]*data.par()[6]+data.par()[7]*data.par()[7]);
  double Z = data.par()[8]                           ;                           
  
  for (; f!=fe; ++f) {
    double df = fabs(F-(*f));
    if (df > pi) df = fabs(pi2-df);
    if (df < m_phiWidth) {
      double dR = (*r)-R;
      double dZ = (*z)-Z;
      if (fabs(E-atanh(dZ/sqrt(dR*dR+dZ*dZ))) < m_etaWidth) return true;
    }
    ++r;
    ++z;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Test is it DBM seed
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::isDBMSeeds(const Trk::SpacePoint* s) const
{
  const InDetDD::SiDetectorElement* de= 
    static_cast<const InDetDD::SiDetectorElement*>(s->clusterList().first->detectorElement());
  return de && de->isDBM();
}

///////////////////////////////////////////////////////////////////
// Calculation global direction for positions of space points
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::globalDirections
(double* p0, double* p1, double* p2, double* d0, double* d1, double* d2) const
{
  double x01 = p1[0]-p0[0]      ;
  double y01 = p1[1]-p0[1]      ;
  double x02 = p2[0]-p0[0]      ;
  double y02 = p2[1]-p0[1]      ;

  double d01 = x01*x01+y01*y01  ; 
  double x1  = sqrt(d01)        ;
  double u01 = 1./x1            ;
  double a   = x01*u01          ; 
  double b   = y01*u01          ;
  double x2  = a*x02+b*y02      ;
  double y2  = a*y02-b*x02      ;
  double d02 = x2*x2+y2*y2      ;
  double u02 = x2/d02           ;
  double v02 = y2/d02           ;

  double A0  =  v02 /(u02-u01)  ;
  double B0  = 2.*(v02-A0*u02)  ;

  double C2  = (1.-B0*y2)       ;
  double S2  = (A0+B0*x2)       ;

  double T   = (p2[2]-p0[2])/sqrt(d02);
  double Se  = 1./sqrt(1.+T*T)        ;
  double Ce  = Se*T                   ;
  Se        /= sqrt(1.+A0*A0)         ;
  double Sa  = Se*a                   ;
  double Sb  = Se*b                   ;

  d0[0] = Sa   -Sb*A0; d0[1]= Sa*A0+Sb   ; d0[2]=Ce;  
  d1[0] = Sa   +Sb*A0; d1[1]= Sb   -Sa*A0; d1[2]=Ce;  
  d2[0] = Sa*C2-Sb*S2; d2[1]= Sa*S2+Sb*C2; d2[2]=Ce;  
}
