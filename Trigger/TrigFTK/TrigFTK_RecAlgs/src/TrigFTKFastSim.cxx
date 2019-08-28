/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//
// filename: TrigFTKFastSim.cxx
//
// **************************************************************************
#include "TrigFTK_RecAlgs/TrigFTKFastSim.h"
#include "TrkTrack/TrackCollection.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"

namespace Par {
  const int Ipt = 0;
  const int eta = 1;
  const int phi = 2;
  const int d0 = 3;
  const int z0 = 4;
}

namespace Eff {
  const int nQoP = 3;
  const int nEta = 4;
  const int nd0 = 3;
  const int nz0 = 3;

  const double QoP_bounds[] = {0, 0.4, 0.8, 1.0};
  const double eta_bounds[] = {0, 0.5, 1.5, 2.0, 2.5};
  const double d0_bounds[]  = {0, 1.2, 1.6, 2.0};
  const double z0_bounds[]  = {0, 72,  96,  120};
  
  int get_QoP_bin(double QoP);
  int get_eta_bin(double eta);
  int get_d0_bin(double d0);
  int get_z0_bin(double d0);
}

namespace Smearing {
  const int n_Ipt_regions   = 9;
  const int n_eta_regions   = 12;
  const double Ipt_bounds[] = {0.0, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  const double eta_bounds[] = {0.0, 0.3, 0.5, 0.7, 0.9, 1.1, 1.3, 1.5, 1.7, 1.9,
                               2.1, 2.3, 2.5};

  int get_Ipt_bin(double Ipt);
  int get_eta_bin(double eta);
}

// local namespace
namespace {
  const int nIpt = 9;
  const int neta = 12;
  const int ntp = 5; // Ipt,eta,phi,d0,z0

  const int min_stage1_hits = 6;
  const int min_stage2_hits = 4;

  //const int trkTp[] = {Trk::qOverP,Trk::theta,Trk::phi0,Trk::d0,Trk::z0};
  const TString tp_name[] = {"Ipt","eta","phi","d0","z0"};

  // smearing parameter list
  class sparam {
  public:
    double fraction;
    double mean;
    double narrow;
    double wide;
  };
  sparam smearing_parameters[nIpt][neta][ntp];
  double efficiency[Eff::nQoP][Eff::nEta][Eff::nd0][Eff::nz0];
}


//-------------------------------------------------------------------------
TrigFTKFastSim::TrigFTKFastSim(const std::string &n, ISvcLocator *pSvcLoc)
  : HLT::FexAlgo(n, pSvcLoc),
    m_collection_name("FTK_TrackCollection"),
    m_smearingFilePath("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/FTK/FastSim/FTF/"),
    m_smearingFileName("ftk.64tower.2016.mc16.v2.root"),
    m_id_helper(0),
    m_pixelId(0),
    m_sctId(0),
    m_SCT_Manager(0),
    m_trkSumTool("TrackSummaryTool"),
    m_uncertaintyTool("FTK_UncertaintyTool"),
    m_rndmSvc("AtRndmGenSvc","Smearing"),
    m_randomEngineName("FTKFastSim_Smearing"),
    m_randomEngine(0)

{
  declareProperty("trackCollectionName", m_collection_name);
  declareProperty("smearingRootFilePath", m_smearingFilePath);
  declareProperty("smearingRootFileName", m_smearingFileName);
  declareProperty("TrackSummaryTool", m_trkSumTool);
  declareProperty("UncertaintyTool",m_uncertaintyTool);

  declareProperty("RndmSvc", m_rndmSvc);
  declareProperty("RndmEngine", m_randomEngineName);
}


//-------------------------------------------------------------------------
HLT::ErrorCode TrigFTKFastSim::hltInitialize() {
    
  ATH_MSG_INFO("TrigFTKFastSim::initialize()");

  ATH_MSG_INFO("Getting TrackSummary tool with name " << m_trkSumTool.name());
  StatusCode sc = m_trkSumTool.retrieve();
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("unable to locate track summary tool");
    return HLT::BAD_JOB_SETUP;
  }

  ATH_MSG_INFO("Getting Uncertainty tool with name " << m_uncertaintyTool.name());
  sc = m_uncertaintyTool.retrieve();
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("unable to locate uncertainty tool");
    return HLT::BAD_JOB_SETUP;
  }

  sc = detStore()->retrieve(m_id_helper, "AtlasID");
  if( sc.isFailure() ) {
    ATH_MSG_DEBUG("Failure retrieving ID_helper"); 
    return HLT::BAD_JOB_SETUP;
  }

  sc = detStore()->retrieve(m_pixelId, "PixelID");
  if( sc.isFailure() ) {
    ATH_MSG_DEBUG("Failure retrieving ID_helper"); 
    return HLT::BAD_JOB_SETUP;
  }

  sc = detStore()->retrieve(m_sctId, "SCT_ID");
  if( sc.isFailure() ) {
    ATH_MSG_DEBUG("Failure retrieving ID_helper"); 
    return HLT::BAD_JOB_SETUP;
  }

  sc = detStore()->retrieve(m_SCT_Manager);
  if( sc.isFailure() ) {
    ATH_MSG_DEBUG("Failure retrieving ID_helper"); 
    return HLT::BAD_JOB_SETUP;
  }

  sc = m_rndmSvc.retrieve();
  if( sc.isFailure() ) {
    ATH_MSG_DEBUG("Failure retrieving IAtRndmGenSvc");
    return HLT::BAD_JOB_SETUP;
  }

  m_randomEngine = m_rndmSvc->GetEngine(m_randomEngineName);
  if( !m_randomEngine ) {
    ATH_MSG_DEBUG("Could not get random engine '" << m_randomEngineName << "'");
    return HLT::BAD_JOB_SETUP;
  }
  
  // import smearing parameters from root file
  TString filename;
  if( m_smearingFilePath.back() == '/' ) {
    filename = m_smearingFilePath + m_smearingFileName;
  } else {
    filename = m_smearingFilePath + "/" + m_smearingFileName;
  }
  TFile *smearing = new TFile(filename);
  if (smearing==nullptr) {
    ATH_MSG_FATAL("Failed to open smearing parameter root file: " << filename);
  } else {
    ATH_MSG_INFO("using smearing parameter root file: " << filename);
  }
  TTreeReader reader("Tree_Efficiency_Smearing_function", smearing);

  TTreeReaderArray<double> *fraction[ntp];
  TTreeReaderArray<double> *mean[ntp];
  TTreeReaderArray<double> *narrow[ntp];
  TTreeReaderArray<double> *wide[ntp];

  for( int itp=0; itp<ntp; itp++ ) {
    fraction[itp] = new TTreeReaderArray<double>(reader,tp_name[itp]+"_Smearing_Function_Fraction");
    mean[itp] = new TTreeReaderArray<double>(reader,tp_name[itp]+"_Smearing_Function_Mean");
    narrow[itp] = new TTreeReaderArray<double>(reader,tp_name[itp]+"_Smearing_Function_NarrowSigma");
    wide[itp] = new TTreeReaderArray<double>(reader,tp_name[itp]+"_Smearing_Function_WideSigma");
  }
  TTreeReaderArray<double> *eff_reader = new TTreeReaderArray<double>(reader,"m_Efficiency_byRegion");
  reader.Next();
  
  for( int itp=0; itp<ntp; itp++ ) {
    int iRegion = 0;
    for( int iIpt=0; iIpt<nIpt; iIpt++ ) {
      for( int ieta=0; ieta<neta; ieta++ ) {
        smearing_parameters[iIpt][ieta][itp].fraction = (*fraction[itp])[iRegion];
        smearing_parameters[iIpt][ieta][itp].mean = (*mean[itp])[iRegion];
        smearing_parameters[iIpt][ieta][itp].narrow = (*narrow[itp])[iRegion];
        smearing_parameters[iIpt][ieta][itp].wide = (*wide[itp])[iRegion];
        
        //ATH_MSG_DEBUG(itp << " " << iIpt << " " << ieta << " " << iRegion << " " << (*wide[itp])[iRegion] << " " << (*narrow[itp])[iRegion]);
        iRegion++;
      }
    }
  }

  int iRegion = 0;
  for( int iQoP=0; iQoP<Eff::nQoP; iQoP++ ) {
    for( int ieta=0; ieta<Eff::nEta; ieta++ ) {
      for( int id0=0; id0<Eff::nd0; id0++ ) {
        for( int iz0=0; iz0<Eff::nz0; iz0++ ) {
          efficiency[iQoP][ieta][id0][iz0] = (*eff_reader)[iRegion];
        }
      }
    }
  }

  for( int itp=0; itp<ntp; itp++ ) {
    delete fraction[itp];
    delete mean[itp];
    delete narrow[itp];
    delete wide[itp];
  }
  delete eff_reader;
  smearing->Close();

  ATH_MSG_INFO("Smeared track collection name " << m_collection_name);
  ATH_MSG_INFO("TrigFTKFastSim::initialize() completed");
  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Execute HLT algorithm
///////////////////////////////////////////////////////////////////
HLT::ErrorCode TrigFTKFastSim::hltExecute(const HLT::TriggerElement* , HLT::TriggerElement* outputTE) {
  ATH_MSG_DEBUG("TrigFTKFastSim::hltExecute()");

  const TrackCollection* tracks;
  HLT::ErrorCode status = getFeature(outputTE, tracks);
  if( status == HLT::OK && tracks ){
    ATH_MSG_DEBUG("Got TrackCollection with " << tracks->size() << " HLT tracks");
  } else {
    ATH_MSG_DEBUG("Failed to get TrackCollection");
  }

  // create collection for smeared tracks
  TrackCollection* smeared_tracks = new TrackCollection;
  
  StatusCode sc = evtStore()->record(smeared_tracks, m_collection_name);
  if( sc.isFailure() ) {
    ATH_MSG_DEBUG("Failure registering " << m_collection_name); 
    return HLT::BAD_JOB_SETUP;
  } else{
    ATH_MSG_DEBUG(m_collection_name << " registered in StoreGate");
  }

  // begin iterating through tracks
  for( auto iter = tracks->begin(); iter != tracks->end(); iter++ ) {
    // find region for efficiency mapping
    double theta = (*iter)->perigeeParameters()->parameters()[Trk::theta];
    double Ipt = (*iter)->perigeeParameters()->parameters()[Trk::qOverP] / sin(theta);
    double eta = -log(std::tan( theta/2.0 ));
    double d0 = (*iter)->perigeeParameters()->parameters()[Trk::d0];
    double z0 = (*iter)->perigeeParameters()->parameters()[Trk::z0];

    int iIpt = Eff::get_QoP_bin( std::abs(Ipt)*Gaudi::Units::GeV ); // smearing functions use GeV
    if( iIpt == -1 ) {ATH_MSG_DEBUG("Ipt exceeded (eff): " << std::abs(Ipt)); continue;}
    int ieta = Eff::get_eta_bin( std::abs(eta) );
    if( ieta == -1 ) {ATH_MSG_DEBUG("eta exceeded (eff): " << std::abs(eta)); continue;}
    int id0  = Eff::get_d0_bin( std::abs(d0) );
    if( id0 == -1 ) {ATH_MSG_DEBUG("d0 exceeded (eff): " << std::abs(d0)); continue;}
    int iz0  = Eff::get_z0_bin( std::abs(z0) );
    if( iz0 == -1 ) {ATH_MSG_DEBUG("z0 exceeded (eff): " << std::abs(z0)); continue;}

    // only keep track if random_frac <= efficiency at region
    double random_frac = m_randomEngine->flat();
    if( random_frac > efficiency[iIpt][ieta][id0][iz0] ) continue;

    // find region for smearing parameters
    iIpt = Smearing::get_Ipt_bin( std::abs(Ipt)*Gaudi::Units::GeV );
    if( iIpt == -1 ) {ATH_MSG_DEBUG("Ipt exceeded (smearing): " << std::abs(Ipt)); continue;}
    ieta = Smearing::get_eta_bin( std::abs(eta) );
    if( ieta == -1 ) {ATH_MSG_DEBUG("Ipt exceeded (smearing): " << std::abs(eta)); continue;}

    // apply smearing
    double s_params[ntp];
    for( int itp=0; itp<ntp; itp++ ) {
      sparam par = smearing_parameters[iIpt][ieta][itp];
      
      random_frac = m_randomEngine->flat();
      if( random_frac > par.fraction ) {
        s_params[itp] = CLHEP::RandGauss::shoot(m_randomEngine,par.mean,par.wide);
      } else {
        s_params[itp] = CLHEP::RandGauss::shoot(m_randomEngine,par.mean,par.narrow);
      }
      //ATH_MSG_DEBUG(tp_name[itp] << " " << s_params[itp]);
    }
    
    // create output track
    Trk::TrackInfo info = (*iter)->info();
    const Trk::FitQuality* quality = new Trk::FitQuality( *((*iter)->fitQuality()) );
    DataVector<const Trk::TrackStateOnSurface>* TSoSV = new DataVector<const Trk::TrackStateOnSurface>;

    Trk::PerigeeSurface periSurf;
    double trkTheta = 2* atan( std::exp( -(eta + s_params[Par::eta]) ) );
    double trkQOverP = (*iter)->perigeeParameters()->parameters()[Trk::qOverP] + sin(theta) * s_params[Par::Ipt]/Gaudi::Units::GeV;
    double trkPhi = (*iter)->perigeeParameters()->parameters()[Trk::phi0] + s_params[Par::phi];
    double trkd0 = (*iter)->perigeeParameters()->parameters()[Trk::d0] + s_params[Par::d0];
    double trkz0 = (*iter)->perigeeParameters()->parameters()[Trk::z0] + s_params[Par::z0];

    // get HLT clusters
    const DataVector<const Trk::TrackStateOnSurface>* track_states=(*iter)->trackStateOnSurfaces();   
    if(!track_states) {
      //ATH_MSG_DEBUG(" No track_states");
      continue;
    }
  
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it=track_states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end=track_states->end();
    if( !(*it) ) {
      ATH_MSG_WARNING("TrackStateOnSurface == Null");
      continue;
    }

    // for applying stage 1/2 FTK selections
    int stage1_hits = 0, stage2_hits = 0;

    // ensure each physical layer is counted once
    // todo: replace with mapping from physical to logical layers
    bool gotPix[2][4]    = {{0}}; // gotPix[1][:] 3 endcap disks, gotPix[0][:] IBL + 3 barrels
    bool gotSCTphi[2][9] = {{0}}; // gotSCTphi[0][:] phi modules on 9 endcap discs
                                  // gotSCTphi[1][:] 4 barrel phi layers
    bool gotSCTuv[2][9]  = {{0}}; // gotSCTuv[0][:] stereo modules on the 9 endcap discs
                                  // gotSCTuv[1][:] 4 barrel stereo layers

    std::vector<const Trk::RIO_OnTrack*> hitsVec;
    for( unsigned int iSurf=0; it!=it_end; it++, iSurf++ ) {
      //ATH_MSG_DEBUG(" Surface " << iSurf);
    
      const Trk::TrackStateOnSurface* tsos=(*it);	
      if( tsos == 0 ) {
        ATH_MSG_DEBUG(" No TrackStateOnSurface");
        continue;
      }
      if( (*it)->type(Trk::TrackStateOnSurface::Measurement) ){
        //ATH_MSG_DEBUG(" Measurment Surface");
        const Trk::MeasurementBase *measurement = (*it)->measurementOnTrack();
        if( !measurement ) {
          ATH_MSG_DEBUG(" No MeasurmentOnTrack");
          continue;
        }
      
        const Trk::RIO_OnTrack *hit = dynamic_cast <const Trk::RIO_OnTrack*>(measurement);
        if(!hit ) {
          ATH_MSG_DEBUG(" No Hit");
          continue;
        }
        
        const Identifier & hitID = hit->identify();
        if( m_id_helper->is_pixel(hitID) ) {
          const Trk::RIO_OnTrack *trkHit = new InDet::PixelClusterOnTrack( dynamic_cast <const InDet::PixelClusterOnTrack&>(*measurement));
          hitsVec.push_back(trkHit);

          int layer = m_pixelId->layer_disk(hitID);
          bool barrel = m_pixelId->is_barrel(hitID);
          if( layer > 0 ) {
            if( gotPix[barrel][layer] == false ) {
              stage1_hits++;
              gotPix[barrel][layer] = true;
            }
          }
          if( layer == 0 ) {
            if( gotPix[0][0] == false ) {
              stage2_hits++;
              gotPix[0][0] = true;
            }
          }
          // ATH_MSG_DEBUG("pixelLayer " << m_pixelId->layer_disk(hitID) );
        } else if( m_id_helper->is_sct(hitID) ) {
          const Trk::RIO_OnTrack *trkHit = new InDet::SCT_ClusterOnTrack( dynamic_cast <const InDet::SCT_ClusterOnTrack&>(*measurement));
          hitsVec.push_back(trkHit);

          const InDetDD::SiDetectorElement* pDE = m_SCT_Manager->getDetectorElement( hitID );
          int layer = m_sctId->layer_disk(hitID);
          bool barrel = m_sctId->is_barrel(hitID);
          if( pDE->isStereo() ) {
            if( gotSCTuv[barrel][layer] == false ) {
              stage2_hits++;
              gotSCTuv[barrel][layer] = true;
            }
          } else {
            if( gotSCTphi[barrel][layer] == false ) {
              stage1_hits++;
              gotSCTphi[barrel][layer] = true;
            }
          }
        }  else {
          continue;
        }
        // ATH_MSG_DEBUG(" got hit - getting ID");
      }
    }

    // apply first and second stage selections
    if( stage1_hits < min_stage1_hits ) continue;
    if( stage2_hits < min_stage2_hits ) continue;

    // copy hits
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
    typePattern.set(Trk::TrackStateOnSurface::Measurement);
    for( auto it=hitsVec.begin(); it!=hitsVec.end(); it++ ) {
      Trk::TrackStateOnSurface* trackStateOnSurface = new Trk::TrackStateOnSurface(*it, 0, 0, 0, typePattern);
      TSoSV->push_back(trackStateOnSurface);
    }

    
    bool hasIBL=false;
    if ((*iter)->trackSummary()) {
      hasIBL = (*iter)->trackSummary()->get(Trk::numberOfInnermostPixelLayerHits) > 0;
    }
    
    FTK_RawTrack rawTrk;
    rawTrk.setInvPt( trkQOverP / sin(trkTheta) );
    rawTrk.setCotTh(1.0/tan(trkTheta));
    AmgSymMatrix(5) *trkTrackCovm = new AmgSymMatrix(5);
    trkTrackCovm->setZero();
    (*trkTrackCovm)(0,0)= m_uncertaintyTool->getParamCovMtx(rawTrk, hasIBL, FTKTrackParam::d0, FTKTrackParam::d0);
    (*trkTrackCovm)(1,1)= m_uncertaintyTool->getParamCovMtx(rawTrk, hasIBL, FTKTrackParam::z0, FTKTrackParam::z0);
    (*trkTrackCovm)(2,2)= m_uncertaintyTool->getParamCovMtx(rawTrk, hasIBL, FTKTrackParam::phi, FTKTrackParam::phi);
    (*trkTrackCovm)(3,3)= m_uncertaintyTool->getParamCovMtx(rawTrk, hasIBL, FTKTrackParam::theta, FTKTrackParam::theta);
    (*trkTrackCovm)(4,4)= m_uncertaintyTool->getParamCovMtx(rawTrk, hasIBL, FTKTrackParam::qOp, FTKTrackParam::qOp);

    
    const Trk::TrackParameters* trkPerigee = new Trk::Perigee( trkd0,
      trkz0,
      trkPhi,
      trkTheta,
      trkQOverP,
      periSurf,
      trkTrackCovm);
    TSoSV->push_back( new Trk::TrackStateOnSurface(NULL, trkPerigee) );

    Trk::Track *track = new Trk::Track( info, TSoSV, quality );

    // add track summary
    m_trkSumTool->updateTrack(*track);
    
    smeared_tracks->push_back( track );

    // ATH_MSG_DEBUG( "smeared track npix, nsct, phi0, pt, eta, d0, z0, chi2: " <<
    //                track->trackSummary()->get(Trk::numberOfPixelHits) << " / " <<
    //                track->trackSummary()->get(Trk::numberOfSCTHits) << " / " <<
    //                track->perigeeParameters()->parameters()[Trk::phi0] << " / " <<
    //                sin( track->perigeeParameters()->parameters()[Trk::theta] ) /
    //                track->perigeeParameters()->parameters()[Trk::qOverP] << " / " <<
    //                -log(tan(0.5*track->perigeeParameters()->parameters()[Trk::theta])) << " / " <<
    //                track->perigeeParameters()->parameters()[Trk::d0] << " / " <<
    //                track->perigeeParameters()->parameters()[Trk::z0] << " / " <<
    //                track->fitQuality()->chiSquared()/track->fitQuality()->numberDoF());

  } // end loop over tracks


  ATH_MSG_INFO("TrigFTKFastSim::hltExecute() completed");
  return HLT::OK;
}
//---------------------------------------------------------------------------
HLT::ErrorCode TrigFTKFastSim::hltFinalize() {
  
  ATH_MSG_INFO("TrigFTKFastSim::finalize()");
  
  return HLT::OK;
}
//---------------------------------------------------------------------------

// functions to convert track parameter values to bin index
int Smearing::get_Ipt_bin(double Ipt) {
  // int ibin = floor(10*Ipt)-1;
  // if( ibin<0 || ibin>Smearing::n_Ipt_regions ) return -1;
  // return ibin;
  for( int ibin=0; ibin<Smearing::n_Ipt_regions; ibin++ ) {
    if( Smearing::Ipt_bounds[ibin] <= Ipt && Ipt < Smearing::Ipt_bounds[ibin+1] ) return ibin;
  }
  return -1;
}

int Smearing::get_eta_bin(double eta) {
  // int ibin = floor(5*eta-0.5); std::cout<<ibin<<std::endl;
  // if( ibin<0 || ibin>Smearing::n_eta_regions ) return -1;
  // return ibin;
  for( int ibin=0; ibin<Smearing::n_eta_regions; ibin++ ) {
    if( Smearing::eta_bounds[ibin] <= eta && eta < Smearing::eta_bounds[ibin+1] ) return ibin;
  }
  return -1;
}

int Eff::get_QoP_bin(double QoP) {
  for( int ibin=0; ibin<Eff::nQoP; ibin++ ) {
    if( Eff::QoP_bounds[ibin] <= QoP && QoP < QoP_bounds[ibin+1] ) return ibin;
  }
  return -1;
}

int Eff::get_eta_bin(double eta) {
  for( int ibin=0; ibin<Eff::nEta; ibin++ ) {
    if( Eff::eta_bounds[ibin] <= eta && eta < eta_bounds[ibin+1] ) return ibin;
  }
  return -1;
}

int Eff::get_d0_bin(double d0) {
  for( int ibin=0; ibin<Eff::nd0; ibin++ ) {
    if( Eff::d0_bounds[ibin] <= d0 && d0 < d0_bounds[ibin+1] ) return ibin;
  }
  return -1;
}

int Eff::get_z0_bin(double z0) {
  for( int ibin=0; ibin<Eff::nz0; ibin++ ) {
    if( Eff::z0_bounds[ibin] <= z0 && z0 < z0_bounds[ibin+1] ) return ibin;
  }
  return -1;
}

//---------------------------------------------------------------------------

