/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define AlfaLocalHits_cxx

// C, C++ headers
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "ALFA_GloRec/AlfaLocalHits.h"
#include "ALFA_LocRecEv/ALFA_LocRecEvent.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvent.h"

using std::cout;
using std::endl;

////////////////////////////////////////////
//
//  implementation of AlfaLocalHit
//  which stores information about one hit
//
////////////////////////////////////////////
AlfaLocalHit::AlfaLocalHit(){
	m_pot        = 0;
	m_potname    = "";
	m_ishit      = 0;
	m_x          = 0.0;
	m_y          = 0.0;
	m_z          = 0.0;
	m_LocRecCorr = 0;
}


AlfaLocalHit::AlfaLocalHit(const ALFA_LocRecCorrEvent * LocRecCorrHit){
  // explicit constructor 
  SetHit(LocRecCorrHit);
}

AlfaLocalHit::AlfaLocalHit(const AlfaLocalHit * h){
  m_pot = h->GetPotNum();
  m_potname = GetMDname(m_pot);
  m_ishit = 1;
  m_x = h->GetX();
  m_y = h->GetY();
  m_z = h->GetZ();
  m_LocRecCorr = h->GetLocRecCorr();
}


void AlfaLocalHit::SetHit(const ALFA_LocRecCorrEvent * LocRecCorrHit){
  m_pot = LocRecCorrHit->getPotNum();
  //cout << "AlfaLocalHit::SetHit... m_pot = " << m_pot << endl;
  m_potname = GetMDname(m_pot);
  m_ishit = 1;
  m_x = LocRecCorrHit->getXpositionLHC();
  m_y = LocRecCorrHit->getYpositionLHC();
  m_LocRecCorr = LocRecCorrHit;
  AssignZ();
}


void AlfaLocalHit::AssignZ(){
  if(m_pot < 0) {
	cout << "AlfaLocalHit::AssignZ() m_pot not assigned" << endl;
    exit(0);
  }
  m_z = 1.e10;
  if(m_potname.Contains("B7L1U")) m_z = 241528.;
  if(m_potname.Contains("A7L1U")) m_z = 237388.;
  if(m_potname.Contains("B7L1L")) m_z = 241538.;
  if(m_potname.Contains("A7L1L")) m_z = 237398.;
  if(m_potname.Contains("B7R1U")) m_z = -241548.;
  if(m_potname.Contains("A7R1U")) m_z = -237408.;
  if(m_potname.Contains("B7R1L")) m_z = -241538.;
  if(m_potname.Contains("A7R1L")) m_z = -237398.;
  if(!(m_z < 1.e10)){
	cout << "AlfaLocalHit::AssignZ: z coordinate was not assigned for the hit" << endl;
    exit(0);
  }
}

void AlfaLocalHit::Print(){
  cout << "pot = "  << m_pot
       << " i.e "   << m_potname.Data()
       << " x = "   << m_x
       << " m_y = " << m_y
	  ;
}

int AlfaLocalHit::GetMDindex(const char * name){
  TString n(name);
    if(n.Contains("B7L1U")) return 0;
    if(n.Contains("B7L1L")) return 1;
    if(n.Contains("A7L1U")) return 2;
    if(n.Contains("A7L1L")) return 3; 
    if(n.Contains("A7R1U")) return 4;
    if(n.Contains("A7R1L")) return 5;
    if(n.Contains("B7R1U")) return 6;
    if(n.Contains("B7R1L")) return 7;
	cout << "AlfaLocalHit::GetMDindex ... Not recognised detector name: " << n.Data() << endl;
    exit(0);
}
TString AlfaLocalHit::GetMDname(int i){
  if(i == 0) return TString("B7L1U");
  if(i == 1) return TString("B7L1L");
  if(i == 2) return TString("A7L1U");
  if(i == 3) return TString("A7L1L"); 
  if(i == 4) return TString("A7R1U");
  if(i == 5) return TString("A7R1L");
  if(i == 6) return TString("B7R1U");
  if(i == 7) return TString("B7R1L");
  cout << "AlfaLocalHit::GetMDname ... Not recognised detector number: " << i << endl;
  exit(0);
}



//////////////////////////////////////////
//
//  implementation of AlfaLocalHits
//  which stores ithe AlfaLocalHit objects
//
////////////////////////////////////////////
// .. default ctor.
AlfaLocalHits::AlfaLocalHits(){
  m_hits.clear();
  m_paths.clear();
  m_nhits  = m_hits.size();
  m_npaths = m_paths.size();
  m_pathpattern ="";
}
void AlfaLocalHits::update(){
  m_nhits   = m_hits.size();
  m_npaths = m_paths.size();
}
int AlfaLocalHits::AddHit(AlfaLocalHit h){
  m_hits.push_back(h);
  update();
  return m_hits.size();
}
void AlfaLocalHits::Reset(){
  m_hits.clear();
  m_paths.clear();
  m_pathpattern ="";
  update();
}
void AlfaLocalHits::ResetPaths(){
  m_paths.clear();
  m_pathpattern ="";
  update();
}

// .. size
int AlfaLocalHits::size(){
  if(m_nhits != (int) m_hits.size()){
	cout << "Class AlfaLocalHits: m_nhits != m_hits.size() !!! " << endl;
    exit(0);
  }
  return m_nhits;
}
// .. Dump
void AlfaLocalHits::Dump(){
  if(m_nhits == 0){
	cout << "Cannot dump the AlfaLocalHits object, it has zero size." << endl;
  }else{
	cout << "Dumping object of AlfaLocalHits class:" << endl;
    for(Int_t i = 0; i < m_nhits; i++){
	  cout << "at " << i << " ";
      m_hits.at(i).Print();
	  cout << endl;
    }
  }
}
void AlfaLocalHits::DumpPaths(){
  cout << "Dumping paths for " << m_pathpattern.Data() << " pattern" << endl;
  for(int i = 0; i < m_npaths; i++){
    m_paths.at(i).Dump();
  }
}

void AlfaLocalHits::ApplyPathPattern(const char * pattern){
/////////////////////////////////////////////////////////////////	
// ... apply path pattern to all the hits
//     paths will be the same objects contaioning
//     only the wanted hits
//     pattern examples: B7L1U_A7L1U_X_B7R1L etc. X ... anything
/////////////////////////////////////////////////////////////////
  m_pathpattern = pattern;
  TString ts_pattern(pattern);
  TObjArray toa_tokenized_pattern(4,0);
  toa_tokenized_pattern = * ts_pattern.Tokenize("_");
  if(toa_tokenized_pattern.GetEntries() != 4){
	cout << "AlfaLocalHits: Pattern of wrong length!!! " << ts_pattern.Data() << endl;
    exit(0);
  }
  TString patternitem("");
  AlfaLocalHits alh_tmp;
  for(int i1 = 0; i1 < m_nhits; i1++){
    int i1last = (i1 == m_nhits-1);
    AlfaLocalHit h1(m_hits.at(i1));
    TString patternitem0 = TString(((TObjString*) toa_tokenized_pattern.At(0))->GetString());
    if((h1.GetPotName().Contains(patternitem0))
    || (i1last && patternitem0.Contains("X"))
    ){
      for(int i2 = 0; i2 < m_nhits; i2++){
        int i2last = (i2 == m_nhits-1);
        if(i2 == i1 && !i2last) continue;
        AlfaLocalHit h2(m_hits.at(i2));
        TString patternitem1 = TString(((TObjString*) toa_tokenized_pattern.At(1))->GetString());
        if((h2.GetPotName().Contains(patternitem1))
        ||(i2last && patternitem1.Contains("X"))
        ){
          for(int i3 = 0; i3 < m_nhits; i3++){
            int i3last = (i3 == m_nhits-1);
            if(i3 == i1 && !i3last) continue;
            if(i3 == i2 && !i3last) continue;
            AlfaLocalHit h3(m_hits.at(i3));
            TString patternitem2 = TString(((TObjString*) toa_tokenized_pattern.At(2))->GetString());
            if((h3.GetPotName().Contains(patternitem2))
            ||(i3last && patternitem2.Contains("X"))
            ){
              for(int i4 = 0; i4 < m_nhits; i4++){
                int i4last = (i4 == m_nhits-1);
                if(i4 == i1 && !i4last) continue;
                if(i4 == i2 && !i4last) continue;
                if(i4 == i3 && !i4last) continue;
                AlfaLocalHit h4(m_hits.at(i4));
                TString patternitem3 = TString(((TObjString*) toa_tokenized_pattern.At(3))->GetString());
                if((h4.GetPotName().Contains(patternitem3))
                ||(i4last && patternitem3.Contains("X"))
                ){
                  if(patternitem0!='X') {alh_tmp.AddHit(h1);}
                  if(patternitem1!='X') {alh_tmp.AddHit(h2);}
                  if(patternitem2!='X') {alh_tmp.AddHit(h3);}
                  if(patternitem3!='X') {alh_tmp.AddHit(h4);}
                }                       
              }                       
            }         
          }
        }
      }
    }
  }
  alh_tmp.SetPathPattern(m_pathpattern.Data());
  m_paths.push_back(alh_tmp);
  update();
}

void AlfaLocalHits::SetPathPattern(const char * pattern){
  m_pathpattern = pattern;
}

AlfaLocalHits * AlfaLocalHits::GetPathHits(int ipath) {
  if(ipath >= m_npaths){
	cout << "Requesting path with overflow index" << endl;
    exit(0);
  }
  return &(m_paths.at(ipath));
}


AlfaLocalHit * AlfaLocalHits::GetHit(int ihit) {
  if(ihit >= m_nhits){
	cout << "Requesting hit with overflow index" << endl;
    exit(0);
  }
  return &(m_hits.at(ihit));
}



////////////////////////////////////////////
//
//  implementation of AlfaTrack
//  which stores information about one track
//
////////////////////////////////////////////
AlfaTrackCand::AlfaTrackCand(AlfaLocalHits * hits){
  // ... initialize
  m_arm = -1.;
  m_x = 1.e10;
  m_y = 1.e10;
  m_z = 1.e10;
  m_xslope = 1.e10;
  m_yslope = 1.e10;
  m_nearlocalhit = 0;
  m_farlocalhit = 0;

  // ... asign hits that create the track
  m_trackcandhits = hits;

  // ... calculate track kinematics and project track back to "pot planes"
  if(AlfaTrackCand::IsLeftOrRightTrack()) {
    AlfaTrackCand::CalcTrack();
    if(m_nearlocalhit && m_farlocalhit) AlfaTrackCand::CalcImpactPoints();
  }else if(AlfaTrackCand::IsLeftAndRightTrack()){
	cout << "Left and right side ALFA tracks at one time not supported in AlfaTrackCand class" << endl;
	cout << "Consider using another class for them or change path pattern." << endl;
  }
}

int AlfaTrackCand::IsLeftTrack(){
  if((m_trackcandhits->GetPathPattern().Contains("B7L1U")
  &&  m_trackcandhits->GetPathPattern().Contains("A7L1U"))
  || (m_trackcandhits->GetPathPattern().Contains("B7L1L")
  &&  m_trackcandhits->GetPathPattern().Contains("A7L1L"))
  ){
    MsgStream LogStream(Athena::getMessageSvc(), "ALFA_TrackCand::IsLeftTrack");
    LogStream << MSG::DEBUG << "Track candidate is left" << endmsg;
    return 1;
  }else{
    return 0;
  }
}

int AlfaTrackCand::IsRightTrack(){
  if((m_trackcandhits->GetPathPattern().Contains("B7R1U")
  &&  m_trackcandhits->GetPathPattern().Contains("A7R1U"))
  || (m_trackcandhits->GetPathPattern().Contains("B7R1L")
  &&  m_trackcandhits->GetPathPattern().Contains("A7R1L"))
  ){
    MsgStream LogStream(Athena::getMessageSvc(), "ALFA_TrackCand::IsRightTrack");
    LogStream << MSG::DEBUG << "Track candidate is right" << endmsg;
    return 1;
  }else{
    return 0;
  }
}

int AlfaTrackCand::IsLowerArmTrack(){
  if((m_trackcandhits->GetPathPattern().Contains("B7L1L")
  &&  m_trackcandhits->GetPathPattern().Contains("A7L1L"))
  || (m_trackcandhits->GetPathPattern().Contains("B7R1L")
  &&  m_trackcandhits->GetPathPattern().Contains("A7R1L"))
  ){
    MsgStream LogStream(Athena::getMessageSvc(), "ALFA_TrackCand::IsLowerArmTrack");
    LogStream << MSG::DEBUG << "Track candidate is in the lower arm" << endmsg;
    return 1;
  }else{
    return 0;
  }
}

int AlfaTrackCand::IsUpperArmTrack(){
  if((m_trackcandhits->GetPathPattern().Contains("B7L1U")
  &&  m_trackcandhits->GetPathPattern().Contains("A7L1U"))
  || (m_trackcandhits->GetPathPattern().Contains("B7R1U")
  &&  m_trackcandhits->GetPathPattern().Contains("A7R1U"))
  ){
    MsgStream LogStream(Athena::getMessageSvc(), "ALFA_TrackCand::IsUpperArmTrack");
    LogStream << MSG::DEBUG << "Track candidate is in the upper arm" << endmsg;
    return 1;
  }else{
    return 0;
  }
}


int AlfaTrackCand::IsLeftOrRightTrack(){
  if(AlfaTrackCand::IsLeftTrack() || AlfaTrackCand::IsRightTrack()){
    return 1;
  }else{
    return 0;
  }
}

int AlfaTrackCand::IsLeftAndRightTrack(){
  if(AlfaTrackCand::IsLeftTrack() && AlfaTrackCand::IsRightTrack()){
    return 1;
  }else{
    return 0;
  }
}

void AlfaTrackCand::CalcTrack(){
  float xM = 1.e10;
  float xm = 1.e10;
  float yM = 1.e10;
  float ym = 1.e10;
  float zM = 1.e10;
  float zm = 1.e10;
  for(int i = 0; i < m_trackcandhits->GetNhits(); i++){
    if(AlfaTrackCand::IsLeftTrack()){
      if(m_trackcandhits->GetHit(i)->GetPotName().Contains("B7")){
        xM = m_trackcandhits->GetHit(i)->GetX();
        yM = m_trackcandhits->GetHit(i)->GetY();
        zM = m_trackcandhits->GetHit(i)->GetZ();
        m_farlocalhit = m_trackcandhits->GetHit(i);
      }else if(m_trackcandhits->GetHit(i)->GetPotName().Contains("A7")){
        xm = m_trackcandhits->GetHit(i)->GetX();
        ym = m_trackcandhits->GetHit(i)->GetY();
        zm = m_trackcandhits->GetHit(i)->GetZ();
        m_nearlocalhit = m_trackcandhits->GetHit(i);
      }
      if(AlfaTrackCand::IsLowerArmTrack()){
        m_arm = 2;
      }else if(AlfaTrackCand::IsUpperArmTrack()){
        m_arm = 1;
      }else{
		cout << "AlfaTrackCand::CalcTrack() ... track is in left but unknown upper/lower arm" << endl;
        exit(0);  
      }
    }else if(AlfaTrackCand::IsRightTrack()){
      if(m_trackcandhits->GetHit(i)->GetPotName().Contains("B7")){
        xm = m_trackcandhits->GetHit(i)->GetX();
        ym = m_trackcandhits->GetHit(i)->GetY();
        zm = m_trackcandhits->GetHit(i)->GetZ();
        m_nearlocalhit = m_trackcandhits->GetHit(i);
      }else if(m_trackcandhits->GetHit(i)->GetPotName().Contains("A7")){
        xM = m_trackcandhits->GetHit(i)->GetX();
        yM = m_trackcandhits->GetHit(i)->GetY();
        zM = m_trackcandhits->GetHit(i)->GetZ();
        m_farlocalhit = m_trackcandhits->GetHit(i);
      }
      if(AlfaTrackCand::IsLowerArmTrack()){
        m_arm = 4;
      }else if(AlfaTrackCand::IsUpperArmTrack()){
        m_arm = 3;
      }else{
		cout << "AlfaTrackCand::CalcTrack() ... track is in right but unknown upper/lower arm" << endl;
        exit(0);  
      }
    }else{
	  cout << "AlfaTrackCand::CalcTrack() ... track must be left or right" << endl;
      exit(0);
    }
  }

  m_x = (xM + xm) / 2.;
  m_y = (yM + ym) / 2.;
  m_z = (zM + zm) / 2.;
  m_xslope = (xM - xm) / (zM - zm);
  m_yslope = (yM - ym) / (zM - zm);
  
}


void AlfaTrackCand::CalcImpactPoints() {
  // ... distance from intercept to xy plane at pot z is 
  //     half of the distance between the pots
  // ... the direction vector starting at x,y needs to be
  //     scaled to touch the plane at pot's z
  double z_nearpot            = m_nearlocalhit->GetZ();
  double scalefactortonearpot = z_nearpot - m_z;
  double ximpact_nearpot      = m_x + m_xslope * scalefactortonearpot;
  double yimpact_nearpot      = m_y + m_yslope * scalefactortonearpot;
  double zimpact_nearpot      = z_nearpot;
  m_nearimpactpoint.setX(ximpact_nearpot);
  m_nearimpactpoint.setY(yimpact_nearpot);
  m_nearimpactpoint.setZ(zimpact_nearpot);

  double z_farpot            = m_farlocalhit->GetZ();
  double scalefactortofarpot = z_farpot - m_z;
  double ximpact_farpot      = m_x + m_xslope * scalefactortofarpot;
  double yimpact_farpot      = m_y + m_yslope * scalefactortofarpot;
  double zimpact_farpot      = z_farpot;
  m_farimpactpoint.setX(ximpact_farpot);
  m_farimpactpoint.setY(yimpact_farpot);
  m_farimpactpoint.setZ(zimpact_farpot);
  
}


void AlfaTrackCand::Dump() {
  //cout.precision(8);
  cout << "Dumping AlfaTrackCand:" << endl;
  cout << " arm = "  << std::setprecision(8) << m_arm
       << " x = "  << std::setprecision(8) << m_x
       << " y = "  << std::setprecision(8) << m_y
       << " z = "  << std::setprecision(8) << m_z
       << " x'= "  << std::setprecision(8) << m_xslope
       << " y' = " << std::setprecision(8) << m_yslope
	   << endl;
}



