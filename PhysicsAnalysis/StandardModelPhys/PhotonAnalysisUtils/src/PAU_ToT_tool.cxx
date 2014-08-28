/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhotonAnalysisUtils/PAU_ToT_tool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "TH2.h"
#include "TFile.h"
#include "PathResolver/PathResolver.h"

#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"

#include <algorithm>
#include <math.h>
#include <functional>

// constructor
PAU_ToT_tool::PAU_ToT_tool(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t,n,p)
{
  declareInterface<IPAU_ToT_tool>(this);
}


// destructor
PAU_ToT_tool::~PAU_ToT_tool() {}


// initialize
StatusCode PAU_ToT_tool::initialize() {

  MsgStream log(msgSvc(), name());
  log <<  MSG::INFO  << name() <<"in initialize()..." << endreq;

  // retrieve TRT-ID helper
  StoreGateSvc* detStore = 0;
  StatusCode sc = service( "DetectorStore", detStore );
  if (sc.isFailure()){
    log << MSG::ERROR << "Could not get DetectorStore" << endreq;
    return sc;
  }

  sc = detStore->retrieve(m_trtId, "TRT_ID");
  if (sc.isFailure()){
    log << MSG::ERROR << "Could not get TRT_ID helper !" << endreq;
    return StatusCode::FAILURE;
  }

  std::string CorrFileBarrel = "corr_map_barrel.root" ;
  std::string InputFilePathBarrel = PathResolver::find_file (CorrFileBarrel, "DATAPATH");
  if ( InputFilePathBarrel == "" ) {
    log << MSG::ERROR << "Can't access input ToT barrel correction file " << CorrFileBarrel << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "ToT barrel correction file found at " << InputFilePathBarrel <<endreq;
  m_inputCorrFileBarrel = new TFile( InputFilePathBarrel.c_str(), "READ" );

  std::string CorrFileEndcapPos = "corr_map_endcap_pos.root" ;
  std::string InputFilePathEndcapPos = PathResolver::find_file (CorrFileEndcapPos, "DATAPATH");
  if ( InputFilePathEndcapPos == "" ) {
    log << MSG::ERROR << "Can't access input ToT positive endcap correction file " << CorrFileEndcapPos << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "ToT positive endcap correction file found at " << InputFilePathEndcapPos <<endreq;
  m_inputCorrFileEndcapPos = new TFile( InputFilePathEndcapPos.c_str(), "READ" );

  std::string CorrFileEndcapNeg = "corr_map_endcap_neg.root" ;
  std::string InputFilePathEndcapNeg = PathResolver::find_file (CorrFileEndcapNeg, "DATAPATH");
  if ( InputFilePathEndcapNeg == "" ) {
    log << MSG::ERROR << "Can't access input ToT negative endcap correction file " << CorrFileEndcapNeg << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "ToT negative endcap correction file found at " << InputFilePathEndcapNeg <<endreq;
  m_inputCorrFileEndcapNeg = new TFile( InputFilePathEndcapNeg.c_str(), "READ" );


  return StatusCode::SUCCESS;
}


// finalize
StatusCode PAU_ToT_tool::finalize(){
  MsgStream log(msgSvc(), name());
  log <<  MSG::VERBOSE  << "... in finalize() ..." << endreq ;
  m_inputCorrFileBarrel->Close();
  m_inputCorrFileEndcapPos->Close();
  m_inputCorrFileEndcapNeg->Close();
  m_inputCorrFileBarrel->Delete();
  m_inputCorrFileEndcapPos->Delete();
  m_inputCorrFileEndcapNeg->Delete();
  return StatusCode::SUCCESS;
}



double PAU_ToT_tool::giveToTonly1bits(unsigned int BitPattern) const {

  /********  Islands ********/
  unsigned  mask2 = 0x02000000;
  int k2;
  int i_island = 0;
  std::vector<int> StartIsland;
  std::vector<int> EndIsland;
  std::vector<int> LengthIsland;
  bool StartedIsland = false;
  bool EndedIsland = true;
  for(k2=0;k2<24;++k2) {
    if ( !(BitPattern & mask2) ) {
      if (StartedIsland) {
	EndIsland.push_back(k2-1);
	LengthIsland.push_back(EndIsland[i_island]-StartIsland[i_island]+1);
	i_island = i_island+1;
	EndedIsland = true;
	StartedIsland = false;
      }
      mask2>>=1;
      if (k2==7 || k2==15) mask2>>=1;
    } else {
      if (EndedIsland) {
	StartIsland.push_back(k2);
	StartedIsland = true;
	EndedIsland = false;
      }
      if (k2==23) {
	EndIsland.push_back(k2);
	LengthIsland.push_back(EndIsland[i_island]-StartIsland[i_island]+1);
      }
      mask2>>=1;
      if (k2==7 || k2==15) mask2>>=1;
    }
  }
  
  // ToT using only bits set to 1 (remove all 0)
  double ToT_only1bits = 0;
  for (unsigned int m=0 ; m<StartIsland.size() ; m++) {
    ToT_only1bits = ToT_only1bits + LengthIsland[m]*3.125;
  }
  
  return ToT_only1bits;
}




double PAU_ToT_tool::giveToTlargerIsland(unsigned int BitPattern) const {

  /********  Islands ********/
  unsigned  mask2 = 0x02000000;
  int k2;
  int i_island = 0;
  std::vector<int> StartIsland;
  std::vector<int> EndIsland;
  std::vector<int> LengthIsland;
  bool StartedIsland = false;
  bool EndedIsland = true;
  for(k2=0;k2<24;++k2) {
    if ( !(BitPattern & mask2) ) {
      if (StartedIsland) {
	EndIsland.push_back(k2-1);
	LengthIsland.push_back(EndIsland[i_island]-StartIsland[i_island]+1);
	i_island = i_island+1;
	EndedIsland = true;
	StartedIsland = false;
      }
      mask2>>=1;
      if (k2==7 || k2==15) mask2>>=1;
    } else {
      if (EndedIsland) {
	StartIsland.push_back(k2);
	StartedIsland = true;
	EndedIsland = false;
      }
      if (k2==23) {
	EndIsland.push_back(k2);
	LengthIsland.push_back(EndIsland[i_island]-StartIsland[i_island]+1);
      }
      mask2>>=1;
      if (k2==7 || k2==15) mask2>>=1;
    }
  }
  // ToT using larger island
  double ToT_largerisland = 0;
  int i_max = -1;
  int maxLength = 0;
  for (unsigned int m=0 ; m<StartIsland.size() ; m++) {
    if (LengthIsland[m]>maxLength) {
      maxLength = LengthIsland[m];
      i_max = m;
    }
  }
  if (i_max!=-1) 
    ToT_largerisland = LengthIsland[i_max]*3.125;
  
  return ToT_largerisland;
}






double PAU_ToT_tool::correctToT(double ToT, double HitZ, double HitRtrack, double HitR, int HitPart, int Layer, int Strawlayer, TFile* corr_barrel, TFile* corr_endcap_pos, TFile* corr_endcap_neg) const {

  double newToT = 0;
  if (abs(HitPart)==1) { // Barrel
    char map_name[200];
    int i = Layer;
    int j = Strawlayer;
    snprintf(map_name,200,"map_barrel_Layer_%d_Strawlayer_%d",i,j);
    TH2F * th2 = (TH2F*) corr_barrel->Get(map_name);
    double valToT = 0;
    valToT = th2->Interpolate(HitRtrack,HitZ);
    newToT = ToT - valToT;
  } else { // End-cap
    if (HitZ>0) {
      char map_name[200];
      int i = Layer;
      snprintf(map_name,200,"map_endcap_Layer_pos_%d",i);  
      TH2F * th2 = (TH2F*) corr_endcap_pos->Get(map_name);
      double valToT = 0;
      valToT = th2->Interpolate(HitRtrack,HitR);
      newToT = ToT - valToT;
    } else {
      char map_name[200];
      int i = Layer;
      snprintf(map_name,200,"map_endcap_Layer_neg_%d",i);  
      TH2F * th2 = (TH2F*) corr_endcap_neg->Get(map_name);
      double valToT = 0;
      valToT = th2->Interpolate(HitRtrack,HitR);
      newToT = ToT - valToT;
    }
  }
  return newToT;
   
}





double  PAU_ToT_tool::getToTvariable(const Trk::TrackParticleBase* p) const
{
  MsgStream log(msgSvc(), name());
  log <<  MSG::DEBUG <<"in getToTvariable()..." << endreq;
    
  double ToTvar = -999999.;

  double Totald = 0;
  double TotalToT = 0;

  const Trk::Track* track = p->originalTrack();
  if (!track) {
    log <<  MSG::DEBUG <<"Trk::track -> Trk::TrackParticleBase link broken" << endreq;
    return ToTvar;
  }

  const DataVector<const Trk::TrackStateOnSurface>* vtsos = track->trackStateOnSurfaces(); 
  if (!vtsos) {
    log <<  MSG::DEBUG <<"No Trk::TrackStateOnSurface found" << endreq;
    return ToTvar;
  }

  DataVector<const Trk::TrackStateOnSurface>::const_iterator itr  = vtsos->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator itre = vtsos->end();
  
  for ( ; itr!=itre ; ++itr) {
    const Trk::MeasurementBase* trkM = (*itr)->measurementOnTrack();
    if (!trkM)  continue;
    
    const InDet::TRT_DriftCircleOnTrack *driftcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(trkM);        
    if (!driftcircle) continue;
    
    //int TrtHl = driftcircle->highLevel();
    //if (TrtHl==1) continue; 
    
    double TrkAnodeDist = 0;
    TrkAnodeDist = driftcircle->localParameters()[Trk::driftRadius];      
    if (!driftcircle->prepRawData()) continue;
        
    double radius = fabs(TrkAnodeDist);
    double trackx =  driftcircle->globalPosition().x();
    double tracky =  driftcircle->globalPosition().y();
    double trackz =  driftcircle->globalPosition().z();

    Identifier DCId = driftcircle->identify();
    int strawlayer = m_trtId->straw_layer(DCId);
    int layer = m_trtId->layer_or_wheel(DCId);
    int bec = m_trtId->barrel_ec(DCId);

    unsigned int BitPattern = driftcircle->prepRawData()->getWord();
    double ToTlargest = giveToTlargerIsland(BitPattern);
    double ToTonly1bits = giveToTonly1bits(BitPattern);
    
    if ( (radius <= 1.9) && (radius >= 0.01) && fabs(ToTlargest-ToTonly1bits)<1e-3 ) {
      double ToT = 0;
      ToT = correctToT(ToTlargest,trackz,radius,sqrt(trackx*trackx+tracky*tracky),bec,layer,strawlayer,m_inputCorrFileBarrel, m_inputCorrFileEndcapPos, m_inputCorrFileEndcapNeg);
      double d = 0;
      if (radius<=2) d = 2*sqrt(4-radius*radius);
      Totald += d;
      TotalToT += ToT;
    }
  }
  
  if (Totald!=0) ToTvar = TotalToT*1./Totald;
  else 
    log << MSG::DEBUG << "Totald == 0 ---> ToTvar = -999999" << endreq;
  
  return ToTvar;

}
