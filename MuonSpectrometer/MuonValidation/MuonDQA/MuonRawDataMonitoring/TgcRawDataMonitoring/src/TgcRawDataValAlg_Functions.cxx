/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package: TgcRawDataValAlg
// Authors:  N. Benekos(Illinois), M. Ishino(ICEPP), H.Kurashige (Kobe), A.Ishikawa (Kobe)  akimasa.ishikawa@cern.ch, M.King(Kobe) kingmgl@stu.kobe-u.ac.jp
// Original: Apr. 2008
// Modified: June 2011
//
// DESCRIPTION:
// Subject: TGCLV1-->Offline Muon Data Quality/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"

// GeoModel
#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include "Identifier/Identifier.h"

// MuonRDO
#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoIdHash.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
 
#include "TgcRawDataMonitoring/TgcRawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include <TH2.h>
#include <TBox.h>
#include <inttypes.h> 

#include <sstream>
#include <cmath>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// Private function to select tgc chambersId of the spectrometer
void
TgcRawDataValAlg::tgcchamberId(){
  ATH_MSG_DEBUG( "in TGC ChambersIDvector"  );

  std::vector<Identifier>::const_iterator  idlast =  m_idHelperSvc->tgcIdHelper().module_end();

  IdContext tgcModuleContext = m_idHelperSvc->tgcIdHelper().module_context();
  Identifier Id;

  for(std::vector<Identifier>::const_iterator i = m_idHelperSvc->tgcIdHelper().module_begin(); 
      i != idlast; i++){
      Id=*i;
      std::string extid = m_idHelperSvc->tgcIdHelper().show_to_string(Id);

      int stationName = int(m_idHelperSvc->tgcIdHelper().stationName(Id)); //unsigned integer 41:T1F 42:T1E 43:T2F 44:T2E 45:T3F 46:T3E 47:T4F 48:T4E (T4 means inner small wheel TGCs, EI/FI)
      int stationEta  = int(m_idHelperSvc->tgcIdHelper().stationEta(Id));  //backward:[-5,-1], forward:[1,5], (1 or -1 at lowest R)
      int stationPhi  = int(m_idHelperSvc->tgcIdHelper().stationPhi(Id));  //forward:[1:24], endcap:[1:48], EI:[1:21], FI:[1:24]

      std::string type = "TGC";
      std::string hardware_name=convertChamberName(stationName, stationEta, stationPhi, type) ;
    } 
  return;
}


// Returns hard coded number of wires for a station index.
int
TgcRawDataValAlg::getNumberOfWires( const int istationName, const int istationEta, const int istationPhi ) const{
  //      TGC1   TGC2   TGC3    EIFI
  // e5           110     96    
  // e4     92    103    106    EI, phi2,11,13,14,15,16,20,21 =>16
  // e3     62     32     32    EI, phi others                =>24
  // e2     23     32     30    
  // e1     24     32     31    FI, phi2,5,8,11,14,17,20,23   =>30
  //  f    105    125    122    FI, phi others                =>32
  
  int nWire  = 0;
  if(istationName==42){ //TGC1 Endcap //41:T1F 42:T1E 43:T2F 44:T2E 45:T3F 46:T3E 47:T3F 48:T4E (T4 means inner small wheel TGCs, EI/FI)
    if(istationEta==1){ // T3 (E4) wire 1ch-91ch (total 91ch) 
      nWire = 92;
    } else  if(istationEta==2){ //T6 (E3) 91ch-152ch (62ch)
      nWire = 62;
    } else if(istationEta==3){ //T7 (E2) 152ch-174ch (23ch)
      nWire = 23;
    } else if(istationEta==4){ //T8 (E1) 173ch-196ch(24ch)
      nWire = 24;
    }
  } else if(istationName==41){ // TGC1 Forward
    nWire = 105;
  }  else if(istationName==44){ // TGC2 Endcap
    if(istationEta==1){ // T4 (E5) wire 1ch-91ch (total 91ch) 
      nWire = 110;
    } else  if(istationEta==2){ //T6 (E4) 109ch-211ch (103ch)
      nWire = 103;
    } else if(istationEta==3){ //T7 (E3) 211ch-242ch (23ch)
      nWire = 32;
    } else if(istationEta==4){ //T8 (E2) 241ch-272ch (32ch)
      nWire = 32;
    } else if(istationEta==5){ //T9 (E1) 271ch-302ch(32ch)
      nWire = 32;
    }
  } else if(istationName==43){ // TGC2 Forward
    nWire = 125;
  }  else if(istationName==46){ // TGC3 Endcap
    if(istationEta==1){ // T5 (E5) wire 1ch-110ch (total 110ch) 
      nWire = 96;
    } else  if(istationEta==2){ //T6 (E4) 95ch-200ch (106ch)
      nWire = 106;
    } else if(istationEta==3){ //T7 (E3) 200ch-231ch (32ch)
      nWire = 32;
    } else if(istationEta==4){ //T8 (E2) 231ch-2
      nWire = 30;
    } else if(istationEta==5){ //T9 (E1) 260ch-290ch(31ch)
      nWire = 31;
    }
  } else if(istationName==45){ // TGC3 Forward
    nWire = 122;
  } else if(istationName==48){ // EI
    //stationPhi 1-8 -> 1-8
    //           9-15 -> 10-16
    //           16-18 -> 18-20
    //           19-21 -> 22-24
    if(istationPhi==2 ||istationPhi==11||istationPhi==13||istationPhi==14||
       istationPhi==15||istationPhi==16||istationPhi==20||istationPhi==21){
      nWire=16;// short
    }else{
      //long  24
      nWire=24;// long
    }
  } else if(istationName==47){ // FI
    if(istationPhi==2 ||istationPhi==5 ||istationPhi==8 ||istationPhi==11||
       istationPhi==14||istationPhi==17||istationPhi==20||istationPhi==23){
      nWire=30;// short
    }else{
      nWire=32;// long
    }
  }
  
  return nWire;
}


// Returns hard coded number of wires for a station index and layer
// Layer variable alters the number of Wires for TGC1 slightly for layers 1 vs 2&3
int 
TgcRawDataValAlg::getNumberOfWires( const int istationName, const int layer, const int istationEta, const int istationPhi ) const{
  //      TGC1                 TGC2   TGC3    EIFI
  //        l1     l2     l3           
  // e5                         110     96    
  // e4     92     91     91    103    106    EI, phi2,11,13,14,15,16,20,21 =>16
  // e3     61     62     62     32     32    EI, phi others                =>24
  // e2     23     23     23     32     30    
  // e1     24     24     24     32     31    FI, phi2,5,8,11,14,17,20,23   =>30
  //  f    105    104    105    125    122    FI, phi others                =>32
  
  int nWire  = 0;
  if(istationName==42){ //TGC1 Endcap //41:T1F 42:T1E 43:T2F 44:T2E 45:T3F 46:T3E 47:T3F 48:T4E (T4 means inner small wheel TGCs, EI/FI)

    if(layer==0){
      if(istationEta==1){ // T3 (E4) wire 1ch-91ch (total 91ch) 
        nWire = 92;
      } else  if(istationEta==2){ //T6 (E3) 91ch-152ch (62ch)
        nWire = 61;
      } else if(istationEta==3){ //T7 (E2) 152ch-174ch (23ch)
        nWire = 23;
      } else if(istationEta==4){ //T8 (E1) 173ch-196ch(24ch)
        nWire = 24;
      }
    }else if( layer ==1 || layer ==2){
      if(istationEta==1){ // T3 (E4) wire 1ch-91ch (total 91ch) 
        nWire = 91;
      } else  if(istationEta==2){ //T6 (E3) 91ch-152ch (62ch)
        nWire = 62;
      } else if(istationEta==3){ //T7 (E2) 152ch-174ch (23ch)
        nWire = 23;
      } else if(istationEta==4){ //T8 (E1) 173ch-196ch(24ch)
        nWire = 24;
      }
    }

  } else if(istationName==41){ // TGC1 Forward

    if(layer==0 || layer ==2) nWire = 105;
    else if(layer==1) nWire = 104;

  }  else if(istationName==44){ // TGC2 Endcap
    if(istationEta==1){ // T4 (E5) wire 1ch-91ch (total 91ch) 
      nWire = 110;
    } else  if(istationEta==2){ //T6 (E4) 109ch-211ch (103ch)
      nWire = 103;
    } else if(istationEta==3){ //T7 (E3) 211ch-242ch (23ch)
      nWire = 32;
    } else if(istationEta==4){ //T8 (E2) 241ch-272ch (32ch)
      nWire = 32;
    } else if(istationEta==5){ //T9 (E1) 271ch-302ch(32ch)
      nWire = 32;
    }
  } else if(istationName==43){ // TGC2 Forward
    nWire = 125;
  }  else if(istationName==46){ // TGC3 Endcap
    if(istationEta==1){ // T5 (E5) wire 1ch-110ch (total 110ch) 
      nWire = 96;
    } else  if(istationEta==2){ //T6 (E4) 95ch-200ch (106ch)
      nWire = 106;
    } else if(istationEta==3){ //T7 (E3) 200ch-231ch (32ch)
      nWire = 32;
    } else if(istationEta==4){ //T8 (E2) 231ch-2
      nWire = 30;
    } else if(istationEta==5){ //T9 (E1) 260ch-290ch(31ch)
      nWire = 31;
    }
  } else if(istationName==45){ // TGC3 Forward
    nWire = 122;
  } else if(istationName==48){ // EI
    //missing stationPhi 9,16,19 relative to FI, so stationPhi [1:21]
    if(istationPhi==2 ||istationPhi==11||istationPhi==13||istationPhi==14|| 
       istationPhi==15||istationPhi==16||istationPhi==20||istationPhi==21){
      nWire=16;// short
    }else{
      nWire=24;// long
    }
  } else if(istationName==47){ // FI
    if(istationPhi==2 ||istationPhi==5 ||istationPhi==8 ||istationPhi==11|| 
       istationPhi==14||istationPhi==17||istationPhi==20||istationPhi==23){
      nWire=30;// short
    }else{
      nWire=32;// long
    }
  }
  
  return nWire;
}


//wire number in phi48
int 
TgcRawDataValAlg::getWireNumberPerPhi48(Identifier id) const{
  int stationName=m_idHelperSvc->tgcIdHelper().stationName(id);//41:T1F 42:T1E 43:T2F 44:T2E 45:T3F 46:T3E 47:T3F 48:T4E (T4 means inner small wheel TGCs, EI/FI)
  
  if(stationName==47||stationName==48){
    ATH_MSG_WARNING( "getWireNumberPerPhi48 is not implemented yet for EIFI"  );
    return -1;
  }
  
  int GasGap=m_idHelperSvc->tgcIdHelper().gasGap(id);//[1-3]
  int stationPhi=m_idHelperSvc->tgcIdHelper().stationPhi(id);//[1:24], [1:48]
  int stationEta=m_idHelperSvc->tgcIdHelper().stationEta(id);//[-5:-1][1:5]
  int channel=m_idHelperSvc->tgcIdHelper().channel(id);//channel
  int isStrip=m_idHelperSvc->tgcIdHelper().isStrip(id);//channel
  
  int tgc, gasgap, eta, phi;
  getEtaPhiBasedNumbering(stationName, GasGap, stationEta, stationPhi,
                          tgc, gasgap, eta, phi);//output [0:2] [0:2] [0:5] [0:47]
  
  int n = getWireNumberPerPhi48(tgc, isStrip, channel, eta);

  return n;
}
int
TgcRawDataValAlg::getWireNumberPerPhi48(int tgc, int ws, int channel, int eta) const{
  if(tgc==3){
    ATH_MSG_WARNING( "getWireNumberPerPhi48 is not implemented yet for EIFI"  );
    return -1;
  }
  int n;
  if(ws==0 && tgc>=0)// Wire
    n=channel+m_offset[tgc][eta];
  else// Strip
    n=channel+eta*32;

  return n;
}


// Calculates indexes for Sector numbering scheme
// inputs are 1-base numbers from idHelper, outputs are 0-base indexes ([Midstation][Inner])
// layer[0:8] = Layer index, sector[0:11][0:3] = Sector index
// eta[0:5][0:1] = Eta index, phi[0:3][0:5] = Phi index inside sector
// Sector based phi indexing is offset by +2(Endcap)/+1(Forward&EIFI) from StationPhi
void
TgcRawDataValAlg::getSectorBasedNumbering(int StationName, int GasGap, int StationEta, int StationPhi,//input
                                          int& layer, int& sector, int& eta, int& phi) const{//output
  // Layer [0:8]
  layer=GasGap-1;
  if(StationName==43||StationName==44){
    layer+=3;
  }else if(StationName==45||StationName==46){
    layer+=5;
  }else if(StationName==47||StationName==48){
    layer+=7;
  }
  
  // Midstation: 
  // Sector[0:11], 
  // etaIndex[0:5], 0 is Forward, [1:5] are Endcap
  // phiIndex[0:3], Endcap has [0:3] chambers per sector, Forward has [0:1]
  if(StationName==42||StationName==44||StationName==46){//Endcap
    eta=std::abs(StationEta);
    phi=StationPhi+1;// offset +2, StationPhi is 1-based, phi is 0-based
    sector=phi/4;
    phi%=4;// convert to index inside sector (phi[0:3])
    if(sector>11)sector=0;
  }
  else if(StationName==41||StationName==43||StationName==45){//Forward
    eta=0;
    phi=StationPhi;// offset +1, StationPhi is 1-based, phi is 0-based
    sector=phi/2;
    phi%=2;// convert to index inside sector (phi[0:1])
    if(sector>=12)sector=0;
  }
  
  // Inner TGC:
  // Sector[0:3],
  // etaIndex[0:1], 0 is FI, 1 is EI
  // phiIndex[0:5], FI has [0:5], EI sector0 has [0:5], other sectors [0:4]
  else if(StationName==47){// FI
    eta=0;
    phi = StationPhi;// offset +1, StationPhi is 1-based, phi is 0-based
    sector=phi/6;
    phi%=6;
    if(sector>=4)sector=0;
  }
  else if(StationName==48){// EI
    eta=1;
    phi = StationPhi;// offset +1, StationPhi is 1-based, phi is 0-based
    // EI phi index is more complicated because it has 3 missing chambers, StationPhi = [1:21]
    // Relative to FI; gaps at StationPhi=9,17,21.  i.e. various positions in sectors
    // However, gaps are inserted into indexing at the end of sectors to keep numbering continuous
    if(phi>=11)phi++;// phi=11 is end of sector=1, actual gap is phi=9
    if(phi>=17)phi++;// phi=17 is end of sector=2, is also the actual location of the gap
    if(phi>=23)phi++;// phi=23 is end of sector=3, actual gap is phi=21
    
    sector=phi/6;
    phi%=6;
    if(sector>=4)sector=0;
  }

  return;
}


// Calculate indexes for EtaPhi numbering scheme
// inputs are 1-base numbers from idHelper, outputs are 0-base indexes
// tgc[0:3] TGC index, gasgap[0:2] GasGap index,
// eta[0:5] StationEta index, phi[0:47](Mid)[0:23](EIFI) StationPhi index
void
TgcRawDataValAlg::getEtaPhiBasedNumbering(int StationName, int GasGap, int StationEta, int StationPhi,//input
                                          int& tgc, int& gasgap, int& eta, int& phi) const{//output
  // Tgc [0:3]
  tgc=0;
  if(StationName==41||StationName==42){
    tgc=0;
  }else if(StationName==43||StationName==44){
    tgc=1;
  }else if(StationName==45||StationName==46){
    tgc=2;
  }else if(StationName==47||StationName==48){
    tgc=3;
  }else{
    tgc=-1;
  }
  
  // GasGap [0:2]
  gasgap=GasGap-1;
  
  // Eta [0:5]
  eta=0;
  if(StationName==42||StationName==44||StationName==46||StationName==48){//Endcap
    eta=std::abs(StationEta);
  }else if(StationName==41||StationName==43||StationName==45||StationName==47){//Forward
    eta=0;
  }else{
    eta=-1;
  }
  // Phi [0:47]
  phi=StationPhi-1;
  
  return;
}

// Convert StationName[41:48] to TGC index [0:3]
int
TgcRawDataValAlg::StationName2TgcIndex(int stationName){
  int tgc=-1;
  if(stationName==41||stationName==42){
    tgc=0;
  }else if(stationName==43||stationName==44){
    tgc=1;
  }else if(stationName==45||stationName==46){
    tgc=2;
  }else if(stationName==47||stationName==48){
    tgc=3;
  }
  return tgc;
}

// Converts Midstation phi48[0:47] to
// Midstation sector number [0:11] and phi index inside that sector [0:3]
void
TgcRawDataValAlg::phi48ToSectorPhi4(int phi48,//input
                                    int& sector, int& phi4){//output
  // phi48 is aligned with the positional phi axis (i.e. start of phi48=0 is at phi=0)
  // sector=0 is centered on phi=0, so phi48 needs to be shifted by +2 to give sector indexing
  sector = (phi48+2)/4;
  if(sector>11)sector=0;
  
  phi4=(phi48+2)%4;
  
  return;
}

// Converts EI phi21[0:20] to EIFI phi24[0:23]
// Fills in holes at stationPhi 9, 16 & 19
int
TgcRawDataValAlg::EIphi21toEIFIphi24(int phi21) const{
  //stationPhi 0-7   ->  0-7
  //           8-14  ->  9-15
  //           15-17 -> 17-19
  //           18-20 -> 21-23
  if(phi21<8){
    return phi21;
  }else if(phi21<15){
    return phi21+1;
  }else if(phi21<18){
    return phi21+2;
  }else{
    return phi21+3;
  }
}

// Gets number to index different doublet, triplet and EIFI chambers in phi, (no indexing in eta is performed)
// Uses sector based numbering ([Midstation][Inner])
// station[42:48], standard station numbering from idHelper
// sector[0:11][0:3] = sector index from getSectorBasedNumbering
// sectorPhi[0:3][0:5] = phi index inside sector from getSectorBasedNumbering
int 
TgcRawDataValAlg::getTripletDoubletID(int station, int sector, int sectorPhi){
  int id=-1;
  
  if( station == 42 ){//ET
    id = sector*12 + sectorPhi;
  }else if (station == 41){//FT
    id = sector*12 + 4 + sectorPhi;
  }else if (station == 44 || station == 46){//ED
    id = sector*12 + 6 + sectorPhi;
  }else if (station == 43 || station == 45){//FD
    id = sector*12 + 10 + sectorPhi;
  }else if (station == 47 || station == 48){//EIFI
    id = 144 + sector ;
  }
  
  return id;
}


int
TgcRawDataValAlg::nWireStripMap(int ws, int binx, int biny){//[0:1][1:43][1:48]
  int layer=0;
  int eta = binx-1;
  int station =0;
//  if     (binx<= 5) {layer = 0; eta =  5 - binx; station = 42;}
//  else if(binx<=10) {layer = 1; eta = 10 - binx; station = 42;}
//  else if(binx<=15) {layer = 2; eta = 15 - binx; station = 42;}
//  else if(binx<=21) {layer = 3; eta = 21 - binx; station = 44;}
//  else if(binx<=27) {layer = 4; eta = 27 - binx; station = 44;}
//  else if(binx<=33) {layer = 5; eta = 33 - binx; station = 46;}
//  else if(binx<=39) {layer = 6; eta = 39 - binx; station = 46;}
//  else if(binx<=41) {layer = 7; eta = 41 - binx; station = 48;}
//  else if(binx<=43) {layer = 8; eta = 43 - binx; station = 48;}
  if( binx%7>=1 && binx%7<=3 && binx<=28) {layer = (binx-1)%7; eta=4-(binx-1)/7; station = 42;} 
  else if( binx%7>=4 && binx%7<=5 && binx<=28) {layer = (binx-1)%7; eta=5-(binx-1)/7; station = 44;} 
  else if( (binx-1)%7>=5 && binx<=28) {layer = (binx-1)%7; eta=5-(binx-1)/7; station = 46;} 
  else if( binx>28 && binx<=30) {layer = binx-28+3-1; eta=1; station = 44; }
  else if( binx>30 && binx<=32) {layer = binx-28+3-1; eta=1; station = 46; }
  else if( binx>32 && binx<=35) {layer = binx-32-1; eta=0; station = 42; }
  else if( binx>35 && binx<=37) {layer = binx-32-1; eta=0; station = 44; }
  else if( binx>37 && binx<=39) {layer = binx-32-1; eta=0; station = 46; }
  else if( binx>39 && binx<=41) {layer = binx-32-1; eta=1; station = 48; }
  else if( binx>41 && binx<=43) {layer = binx-34-1; eta=0; station = 48; }

  if(eta==0) station-=1;

  int nwire = getNumberOfWires( station, layer, eta, biny );

  if(ws==STRP){
    if(nwire==0) return 0;
    return 32;
  }

  return nwire;
}

void
TgcRawDataValAlg::getChamberMapIndexes(int eta, int phi48, int layer, int &binx, int &biny){
  // binx is 0-based
  //if( layer == 0 )      binx =  4-eta;
  //else if( layer == 1 ) binx =  9-eta;
  //else if( layer == 2 ) binx = 14-eta;
  //else if( layer == 3 ) binx = 20-eta;
  //else if( layer == 4 ) binx = 26-eta;
  //else if( layer == 5 ) binx = 32-eta;
  //else if( layer == 6 ) binx = 38-eta;
  //else if( layer == 7 ) binx = 40-eta;
  //else if( layer == 8 ) binx = 42-eta;
  
  if ( layer < 3){
    if(eta>0 && eta<=4) binx = 7*(4-eta) + layer;  // E1-E4
    else if(eta==0) binx = 32 + layer;  // F
  } else if (layer < 7){
    if(eta>1 && eta<=5) binx = 7*(5-eta) + layer;  // E1-E4
    else if(eta==1) binx = 28 + layer-3; // E5 
    else if(eta==0) binx = 32 + layer; // F 
  } else if (layer < 9){
    if(eta==1) binx = 39 + layer-7; // EI
    else if(eta==0) binx = 41 + layer-7; // FI
  }
  
  // biny is 1-based
  biny = phi48+2;
  if(biny>=48) biny-=48;
  biny++;
}

 
//check same chamber and neibouring channel or not
int
TgcRawDataValAlg::compareID( int ch0,//inner |Z|
                                 int ch1,//outter|Z|
                                 int dmin,
                                 int dmax){
  if((ch0 - ch1) >=dmin &&
     (ch0 - ch1) <=dmax ){
    return 1;
  }
  else{
    return 0;
  }
}

///////////////////////////////////////////////////////////////////////////
// Put blank gray box over histogram area
void
TgcRawDataValAlg::putBox(TH2* h, float x1, float y1, float x2, float y2){
  TBox* box = new TBox(x1, y1, x2, y2);
  //  box->SetFillColor(kGray+1);
  //  box->SetLineColor(kGray+1);
  //  box->SetFillStyle(3013);
  //  box->SetFillStyle(3004);
  
  box->SetFillColor(kGray);
  box->SetLineColor(kGray);

  //  box->SetLineStyle(3);

  (h->GetListOfFunctions())->Add(box);    
  //  delete box; // will cause crash or not save box // is deleted along with h
}
void
TgcRawDataValAlg::BlankPhi24(TH2 *h, int binx){
  float x1=h->GetXaxis()->GetBinLowEdge(binx);
  float x2=h->GetXaxis()->GetBinUpEdge(binx);
  for(int i=0;i<24;i++){
    int biny = (i+1)*2;
    float y1=h->GetYaxis()->GetBinLowEdge(biny);
    float y2=h->GetYaxis()->GetBinUpEdge(biny);
    putBox(h, x1, y1, x2, y2);
  }
}
void
TgcRawDataValAlg::BlankStationMap(TH2 *h){
  // old binning strategy
  // BlankPhi24(h, 5);
  // BlankPhi24(h, 10);
  // BlankPhi24(h, 15);
  // BlankPhi24(h, 21);
  // BlankPhi24(h, 27);
  // BlankPhi24(h, 33);
  // BlankPhi24(h, 39);
  // BlankPhi24(h, 40);
  // BlankPhi24(h, 41);
  // BlankPhi24(h, 42);
  // BlankPhi24(h, 43);

  // int x1=h->GetXaxis()->GetBinLowEdge(40);
  // int x2=h->GetXaxis()->GetBinUpEdge(40);
  // int x3=h->GetXaxis()->GetBinLowEdge(42);
  // int x4=h->GetXaxis()->GetBinUpEdge(42);
  // int y1=h->GetYaxis()->GetBinLowEdge(19);
  // int y2=h->GetYaxis()->GetBinUpEdge(19);
  // putBox(h, x1, y1, x2, y2);
  // putBox(h, x3, y1, x4, y2);
  // y1=h->GetYaxis()->GetBinLowEdge(35);
  // y2=h->GetYaxis()->GetBinUpEdge(35);
  // putBox(h, x1, y1, x2, y2);
  // putBox(h, x3, y1, x4, y2);
  // y1=h->GetYaxis()->GetBinLowEdge(43);
  // y2=h->GetYaxis()->GetBinUpEdge(43);
  // putBox(h, x1, y1, x2, y2);
  // putBox(h, x3, y1, x4, y2);

  BlankPhi24(h, 33);
  BlankPhi24(h, 34);
  BlankPhi24(h, 35);
  BlankPhi24(h, 36);
  BlankPhi24(h, 37);
  BlankPhi24(h, 38);
  BlankPhi24(h, 39);
  BlankPhi24(h, 40);
  BlankPhi24(h, 41);
  BlankPhi24(h, 42);
  BlankPhi24(h, 43);

  
  int x1=h->GetXaxis()->GetBinLowEdge(40);
  int x2=h->GetXaxis()->GetBinUpEdge(40);
  int x3=h->GetXaxis()->GetBinLowEdge(41);
  int x4=h->GetXaxis()->GetBinUpEdge(41);
  int y1=h->GetYaxis()->GetBinLowEdge(19);
  int y2=h->GetYaxis()->GetBinUpEdge(19);
  putBox(h, x1, y1, x2, y2);
  putBox(h, x3, y1, x4, y2);
  y1=h->GetYaxis()->GetBinLowEdge(35);
  y2=h->GetYaxis()->GetBinUpEdge(35);
  putBox(h, x1, y1, x2, y2);
  putBox(h, x3, y1, x4, y2);
  y1=h->GetYaxis()->GetBinLowEdge(43);
  y2=h->GetYaxis()->GetBinUpEdge(43);
  putBox(h, x1, y1, x2, y2);
  putBox(h, x3, y1, x4, y2);
}

void
TgcRawDataValAlg::BlankStripStationMap(TH2 *h){
  int binsY = 48;//h->GetYaxis()->GetNBins();
  float x1=h->GetXaxis()->GetBinLowEdge(2);
  float x2=h->GetXaxis()->GetBinUpEdge(2);
  for(int i=1;i<=binsY;i++){
    int biny = i;
    float y1=h->GetYaxis()->GetBinLowEdge(biny);
    float y2=h->GetYaxis()->GetBinUpEdge(biny);
    putBox(h, x1, y1, x2, y2);
  }
  x1=h->GetXaxis()->GetBinLowEdge(9);
  x2=h->GetXaxis()->GetBinUpEdge(9);
  for(int i=1;i<=binsY;i++){
    int biny = i;
    float y1=h->GetYaxis()->GetBinLowEdge(biny);
    float y2=h->GetYaxis()->GetBinUpEdge(biny);
    putBox(h, x1, y1, x2, y2);
  }
  x1=h->GetXaxis()->GetBinLowEdge(16);
  x2=h->GetXaxis()->GetBinUpEdge(16);
  for(int i=1;i<=binsY;i++){
    int biny = i;
    float y1=h->GetYaxis()->GetBinLowEdge(biny);
    float y2=h->GetYaxis()->GetBinUpEdge(biny);
    putBox(h, x1, y1, x2, y2);
  }
  x1=h->GetXaxis()->GetBinLowEdge(23);
  x2=h->GetXaxis()->GetBinUpEdge(23);
  for(int i=1;i<=binsY;i++){
    int biny = i;
    float y1=h->GetYaxis()->GetBinLowEdge(biny);
    float y2=h->GetYaxis()->GetBinUpEdge(biny);
    putBox(h, x1, y1, x2, y2);
  }
  x1=h->GetXaxis()->GetBinLowEdge(34);
  x2=h->GetXaxis()->GetBinUpEdge(34);
  for(int i=1;i<=binsY;i++){
    int biny = i;
    if(i%2==0)continue;  // even bins already blanked by BlankPhi24() 
    float y1=h->GetYaxis()->GetBinLowEdge(biny);
    float y2=h->GetYaxis()->GetBinUpEdge(biny);
    putBox(h, x1, y1, x2, y2);
  }

}
