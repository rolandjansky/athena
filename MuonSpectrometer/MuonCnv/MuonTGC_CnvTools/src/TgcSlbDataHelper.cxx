/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/* 
   SLB data helper class : store bit map
   reconstruct to Hits and  Coincidences
   convert to celladdr and data

   modified for new SL format H.Kurashige  Aug.2008
 
   author Tadashi Maeno
   author Hisaya Kurashige  Jan. 2008
   author Susumu Oda Jun. 2009
*/

#include "TgcSlbDataHelper.h"
#include "MuonRDO/TgcRawData.h"
#include "TgcSlbData.h"
#include "AthenaKernel/getMessageSvc.h"

Muon::TgcSlbDataHelper::TgcSlbDataHelper(void)
  : AthMessaging (Athena::getMessageSvc(), "Muon::TgcSlbDataHelper")
{
}

Muon::TgcSlbDataHelper::~TgcSlbDataHelper(void)
{
}
 
// reconstruct to Hits. subDetectorID and ROD ID are dummy
void Muon::TgcSlbDataHelper::convertToHits(uint16_t subDetectorId,
					   uint16_t rodId,
					   const TgcSlbData * slb,
					   std::vector<TgcRawData *> &vChannel) const
{
  TgcRawData::SlbType type = TgcRawData::SLB_TYPE_UNKNOWN;
  if((slb->getType() == TgcSlbData::SLB_SL_F) || 
     (slb->getType() == TgcSlbData::SLB_SL_E)) {
    return;
  } else {
    if(slb->getType() == TgcSlbData::SLB_LPT_D_W) {
      type = TgcRawData::SLB_TYPE_DOUBLET_WIRE;
    } else if(slb->getType() == TgcSlbData::SLB_LPT_D_S) {
      type = TgcRawData::SLB_TYPE_DOUBLET_STRIP;
    } else if(slb->getType() == TgcSlbData::SLB_LPT_T_W) {
      type = TgcRawData::SLB_TYPE_TRIPLET_WIRE;
    } else if(slb->getType() == TgcSlbData::SLB_LPT_T_S) {
      type = TgcRawData::SLB_TYPE_TRIPLET_STRIP;
    } else if(slb->getType() == TgcSlbData::SLB_LPT_I_W || 
	      slb->getType() == TgcSlbData::SLB_LPT_I_S) {
      type = TgcRawData::SLB_TYPE_INNER_WIRE;
      // Readout formatted Data use TgcRawData::SLB_TYPE_INNER_WIRE for Inner Strip Hits. 
      // http://cern.ch/atlas-tgc/doc/ROBformat.pdf Table 7 : SB type, bits 15..13
      // Since EI/FI Wire and Strip hold one SLB in common, 
      // we cannot use SLB type to distinguish Wire and Strip.
    } 
  }
  
  bool isFirstHit = true;
  for(unsigned int iBc=TgcSlbData::BC_PREVIOUS; iBc<TgcSlbData::N_BC; ++iBc) {
    const bool * bitArray = slb->getBitArray(iBc);
    
    // bit 0..39 is assigned for Trigger Data, ignore
    const int TRIGGER_DATA_OFFSET=40;
    for(int ibit=TRIGGER_DATA_OFFSET; ibit<TgcSlbData::BITMAP_SIZE; ++ibit) {
      if(*(bitArray+ibit)) {
	// cretae TgcRaw 
	TgcRawData * rCh = 
	  new TgcRawData(iBc,
			 subDetectorId, rodId,
			 slb->getSswId(), slb->getSbLoc(),
			 slb->getL1Id(),  slb->getBcId(),
			 type,
			 isAdjacent(ibit),
			 0,
			 ibit);
	vChannel.push_back(rCh);
	
	// DEBUG PRINT
	if(isFirstHit) {
          ATH_MSG_DEBUG( "Tgc TgcSlbDataHelper::convertToHits :"
                         << " slb type =" << slb->getType() );
	  isFirstHit =false;
	}
        ATH_MSG_DEBUG( "TgcRawData : HIT "
                       << " BC:" << iBc 
                       << " subDetId:" << subDetectorId 
                       << " rodId:" << rodId
                       << " sswId:" << slb->getSswId() 
                       << " sbLoc:" << slb->getSbLoc() 
                       << " slbId:" << slb->getSlbId()
                       << " bit#:" << ibit );
      } //endif
    } // loop ibit
  } // loop iBC
}

// reconstruct to Coincidences. subDetectorID and ROD ID are dummy
void Muon::TgcSlbDataHelper::convertToCoincidences(uint16_t subDetectorId,
						   uint16_t rodId,
						   const TgcSlbData * slb,
						   std::vector<TgcRawData *> &vChannel) const
{
  bool hit[8];
  int delta[8];
  int pos[8];

  bool isFirstHit = true;

  for(unsigned int iBc=TgcSlbData::BC_PREVIOUS; iBc<TgcSlbData::N_BC; ++iBc) {
    const bool * bitArray = slb->getBitArray(iBc);
    bool forward;
    uint16_t sector;  
    
    TgcRawData * rCh;
    switch(slb->getType()) {
    case TgcSlbData::SLB_LPT_D_W: // Doublet Wire
      getLPTforDoublet(bitArray, hit, delta, pos);
 
      for(int i=0; i<N_LPT_D; ++i) {
	if(!hit[i]) continue;
	//create TgcRawData object
	rCh = new TgcRawData(iBc,
			     subDetectorId, rodId,
			     slb->getSswId(), slb->getSbLoc(),
			     slb->getL1Id(), slb->getBcId(),
			     TgcRawData::SLB_TYPE_DOUBLET_WIRE/*DOUBLET WIRE*/,     
			     delta[i], 
			     0, i, pos[i]);
	vChannel.push_back(rCh);
 
	// DEBUG PRINT
	if(isFirstHit) {
          ATH_MSG_DEBUG( "Tgc TgcSlbDataHelper::convertCoincidence :"
                         << " slb type ="  << slb->getType() );
	  isFirstHit = false;
	}
        ATH_MSG_DEBUG( ((slb->getType()==TgcSlbData::SLB_LPT_D_W) ? 
                        "TgcRawData : LPT_D_W " : "TgcRawData : LPT_D_S ")
                       << " BC:" << iBc 
                       << " subDetId:" << subDetectorId 
                       << " rodId:" << rodId
                       << " sswId:" << slb->getSswId() 
                       << " sbLoc:" << slb->getSbLoc() 
                       << " slbId:" << slb->getSlbId()
                       << " sub: " << i
                       << " pos:" << pos[i] 
                       << " delta:" << delta[i] );
      } // loop block (i)        
      break;

    case TgcSlbData::SLB_LPT_D_S: // Doublet Strip
      getLPTforDoublet(bitArray, hit, delta, pos);
      
      for(int i=0; i<N_LPT_D; ++i) {
	if(!hit[i]) continue;
	//create TgcRawData object
	rCh = new TgcRawData(iBc,
			     subDetectorId, rodId,
			     slb->getSswId(), slb->getSbLoc(),
			     slb->getL1Id(), slb->getBcId(),
			     TgcRawData::SLB_TYPE_DOUBLET_STRIP/*DOUBLET STRIP*/,    
			     delta[i],
			     0, i, pos[i]);
	vChannel.push_back(rCh);
	 
	if(isFirstHit) {
          ATH_MSG_DEBUG( "Tgc TgcSlbDataHelper::convertCoincidence :"
                         << " slb type =" << slb->getType() );
	  isFirstHit = false;
	}
        ATH_MSG_DEBUG( ((slb->getType()==TgcSlbData::SLB_LPT_D_W) ?
                        "TgcRawData : LPT_D_W " : "TgcRawData : LPT_D_S ")
                       << " BC:" << iBc 
                       << " subDetId:" << subDetectorId 
                       << " rodId:" << rodId
                       << " sswId:" << slb->getSswId() 
                       << " sbLoc:" << slb->getSbLoc() 
                       << " slbId:" << slb->getSlbId()
                       << " sub: " << i
                       << " pos:" << pos[i] 
                       << " delta:" << delta[i] );
      } // loop block (i) 
      break;

    case TgcSlbData::SLB_LPT_T_W: // Triplet Wire
      getLPTforTripletWire(bitArray, hit, pos);
      for(int i=0; i<N_LPT_TW; ++i) {
	if(!hit[i]) continue;
	delta[i]=0;
	//create TgcRawData object
	rCh = new TgcRawData(iBc,
			     subDetectorId, rodId,
			     slb->getSswId(), slb->getSbLoc(),
			     slb->getL1Id(), slb->getBcId(),
			     TgcRawData::SLB_TYPE_TRIPLET_WIRE/*TRIPLET WIRE*/,    
			     delta[i],
			     0, i, pos[i]);
	vChannel.push_back(rCh);
	if(isFirstHit) {
          ATH_MSG_DEBUG( "Tgc TgcSlbDataHelper::convertCoincidence :"
                         << " slb type =" << slb->getType() );
	  isFirstHit = false;
	}
        ATH_MSG_DEBUG( "TgcRawData : LPT_T_W "
                       << " BC:" << iBc 
                       << " subDetId:" << subDetectorId 
                       << " rodId:" << rodId
                       << " sswId:" << slb->getSswId() 
                       << " sbLoc:" << slb->getSbLoc() 
                       << " slbId:" << slb->getSlbId()
                       << " sub: " << i
                       << " pos:" << pos[i]  );
      }	// loop block  
      break;
      
    case TgcSlbData::SLB_LPT_T_S: // Triplet Strip
      getLPTforTripletStrip(bitArray, hit, pos);
      for(int i=0; i<N_LPT_TS; ++i) {
	if(!hit[i]) continue;
	delta[i]=0;
	int seg = 0;
	int subc =i;
	if(i>3) {
	  seg = 1;
	  subc = i-4;
	}
	//create TgcRawData object
	rCh = new TgcRawData(iBc,
			     subDetectorId, rodId,
			     slb->getSswId(), slb->getSbLoc(),
			     slb->getL1Id(), slb->getBcId(),
			     TgcRawData::SLB_TYPE_TRIPLET_STRIP/*TRIPLET STRIP*/,    
			     delta[i], 
			     seg, subc, pos[i]);
	vChannel.push_back(rCh);
	if(isFirstHit) {
          ATH_MSG_DEBUG( "Tgc TgcSlbDataHelper::convertCoincidence :"
                         << " slb type =" << slb->getType() );
	  isFirstHit = false;
	}
        ATH_MSG_DEBUG( "TgcRawData : LPT_T_S "
                       << " BC:" << iBc 
                       << " subDetId:" << subDetectorId 
                       << " rodId:" << rodId
                       << " sswId:" << slb->getSswId()
                       << " sbLoc:" << slb->getSbLoc() 
                       << " slbId:" << slb->getSlbId()
                       << " seg: " << seg
                       << " sub: " << subc
                       << " pos:" << pos[i] );
      }	// loop block (i)  
      break;
      
    ///// Tracklet Inner, implemented on March 12, 2010 by Susumu Oda ///// 
    // Since EI/FI Wire and Strip hold one SLB in common, 
    // we cannot use SLB type to distinguish Wire and Strip. 
    case TgcSlbData::SLB_LPT_I_W: 
    case TgcSlbData::SLB_LPT_I_S: 
      getLPTforInner(bitArray, hit);
      for(int i=0; i<N_LPT_I; ++i) {
	if(!hit[i]) continue;
	rCh = new TgcRawData(iBc, // uint16_t bcTag  
			     subDetectorId, // uint16_t subDetectorId
			     rodId, // uint16_t rodId
			     slb->getSswId(), // uint16_t sswId
			     slb->getSbLoc(), // uint16_t slbId
			     slb->getL1Id(), // uint16_t l1Id 
			     slb->getBcId(), // uint16_t bcId 
			     (i<(N_LPT_I/2) ? TgcRawData::SLB_TYPE_INNER_STRIP 
			      : TgcRawData::SLB_TYPE_INNER_WIRE), // TgcRawData::SlbType slbType 
			     // TRIGA HIT (bits 0..3) is for Strip and TRIGB HIT (bits 4..7) is for Wire
			     // https://twiki.cern.ch/twiki/pub/Main/TgcDocument/celladdress2_asic_rev2.pdf
			     0, // int16_t delta, always for Tracklet Inner  
			     static_cast<uint16_t>(i>=(N_LPT_I/2)), // uint16_t seg, wire=1;strip=0 
			     i%(N_LPT_I/2), // uint16_t sub, Inner 0..3, TRIG0=sub0, TRIG1=sub1, TRIG2=sub2, TRIG3=sub3  
			     // https://twiki.cern.ch/twiki/pub/Main/TgcDocument/celladdress2_asic_rev2.pdf 
			     0); // uint16_t rphi, always for Tracklet Inner 
	vChannel.push_back(rCh);
	if(isFirstHit) {
          ATH_MSG_DEBUG( "Tgc TgcSlbDataHelper::convertCoincidence :"
                         << " slb type =" << slb->getType() );
	  isFirstHit = false;
	}
        ATH_MSG_DEBUG( "TgcRawData : " 
                       << (rCh->slbType()==TgcRawData::SLB_TYPE_INNER_STRIP ? "LPT_I_S" : "LPT_I_W") 
                       << " BC:" << rCh->bcTag() 
                       << " subDetId:" << rCh->subDetectorId() 
                       << " rodId:" << rCh->rodId()
                       << " sswId:" << rCh->sswId()
                       << " sbLoc:" << rCh->slbId() 
                       << " slbId:" << slb->getSlbId()
                       << " seg: " << rCh->segment()
                       << " sub: " << rCh->subMatrix()
                       << " pos:" << rCh->position() );
      }	
      
      break;

    case TgcSlbData::SLB_SL_F: // Forward Sector Logic
    case TgcSlbData::SLB_SL_E: // Endcap Sector Logic
      
      forward = (slb->getType() == TgcSlbData::SLB_SL_F);
      sector = getSector(forward, subDetectorId, rodId, slb);  
 
      // SL Output
      bool cand3plus;
      bool hitSL[2];
      bool muplus[2];
      uint16_t threshold[2];
      bool overlap[2];
      uint16_t roi[2];
      if(forward) {
	TgcSlbDataHelper::getSL_F(bitArray,  cand3plus,
				  hitSL,
				  muplus,  threshold,
				  overlap,       roi) ;
      } else {
	TgcSlbDataHelper::getSL_E(bitArray,  cand3plus,
				  hitSL,
				  muplus,  threshold,
				  overlap,       roi) ;
      }
	
      for(size_t i=0; i<2; ++i) {
	if(!hitSL[i]) continue;
	//create TgcRawData object
        bool veto[2] = {false};
	rCh = new TgcRawData(iBc, 
			     subDetectorId, rodId,
			     slb->getL1Id(), slb->getBcId(),
			     cand3plus,
			     forward,
			     sector,    
			     i,
			     muplus[i],
			     threshold[i],
			     overlap[i],
			     veto[i],
			     roi[i]);
	vChannel.push_back(rCh);
	if(isFirstHit) {
          ATH_MSG_DEBUG( "Tgc TgcSlbDataHelper::convertCoincidence :"
                         << " slb type =" << slb->getType() );
	  isFirstHit = false;
	}
        ATH_MSG_DEBUG( "TgcRawData : SL "
                       << " BC:" << iBc 
                       << " subDetId:" << subDetectorId 
                       << " rodId:" << rodId
                       << " sswId:" << slb->getSswId()
                       << " sbLoc:" << slb->getSbLoc() 
                       << " slbId:" << slb->getSlbId()
                       << " roi:" << roi[i] 
                       << " pt:" << threshold[i] );
      } // loop candidate (i)

      // SL Input = Hpt Output
      bool hitP[N_HPT_E];
      bool strip[N_HPT_E];
      uint16_t chip[N_HPT_E];
      uint16_t index[N_HPT_E];
      bool hipt[N_HPT_E];
      uint16_t hitId[N_HPT_E];
      uint16_t sub[N_HPT_E];
      int16_t deltaHPT[N_HPT_E];

      size_t numberOfHit;
      if(forward) {
	numberOfHit = N_HPT_F;
	TgcSlbDataHelper::getHPT_F(bitArray,  
				   hitP,    strip,   
				   chip,    index,
				   hipt,    hitId,
				   sub,     deltaHPT); 
      } else {
	numberOfHit = N_HPT_E;
	TgcSlbDataHelper::getHPT_E(bitArray,  
				   hitP,    strip,   
				   chip,    index,
				   hipt,    hitId,
				   sub,     deltaHPT);
      }
	
      for(size_t i=0; i<numberOfHit; ++i) {
	if(!hitP[i]) continue;

        // check HighPT data 
        if(!isValid_HPT(hitP[i],    strip[i],   
			chip[i],    index[i],
			hipt[i],    hitId[i],
			sub[i],     deltaHPT[i])) {
	  std::string hpt_Type="E";
	  if(forward) hpt_Type ="F";
	  if(strip[i]) hpt_Type+="S";
	  else hpt_Type +="W";

	  if(isFirstHit) {
            ATH_MSG_DEBUG( "Tgc TgcSlbDataHelper::convertCoincidence :"
                           << " slb type =" << slb->getType() );
	    isFirstHit = false;
	  }
          ATH_MSG_DEBUG( "invalid HPT data " << hpt_Type
                         << " BC:" << iBc 
                         << " subDetId:" << subDetectorId 
                         << " rodId:" << rodId
                         << " sswId:" << slb->getSswId()
                         << " sbLoc:" << slb->getSbLoc() 
                         << " slbId:" << slb->getSlbId()
                         << std::dec                      
                         << " chip:" << chip[i] 
                         << " index:" << index[i]
                         << " hitId:" << hitId[i] 
                         << " delta:" << deltaHPT[i] );
          continue; // skip making TgcRawData
        }

	//create TgcRawData object
        {
          uint16_t inner[N_HPT_E] = {0};
          rCh = new TgcRawData(iBc, 
                               subDetectorId, rodId,
                               slb->getL1Id(), slb->getBcId(),
                               strip[i],
                               forward,
                               sector,    
                               chip[i],
                               index[i],
                               hipt[i],
                               hitId[i],
                               sub[i],
                               deltaHPT[i],
                               inner[i]);
        }
	vChannel.push_back(rCh);
	if(isFirstHit) {
          ATH_MSG_DEBUG( "Tgc TgcSlbDataHelper::convertCoincidence :"
                         << " slb type =" << slb->getType() );
	  isFirstHit = false;
	}
        auto hptType = [&]() {
          std::string hptType="E";
          if(forward) hptType ="F";
          if(strip[i]) hptType+="S";
          else hptType +="W";
          return hptType;
        };
        ATH_MSG_DEBUG( "TgcRawData : HPT " << hptType()
                       << " BC:" << iBc 
                       << " subDetId:" << subDetectorId 
                       << " rodId:" << rodId
                       << " sswId:" << slb->getSswId() 
                       << " sbLoc:" << slb->getSbLoc() 
                       << " slbId:" << slb->getSlbId()
                       << std::dec
                       << " chip:" << chip[i] 
                       << " index:" << index[i]
                       << " hitId:" << hitId[i] 
                       << " delta:" << deltaHPT[i] );
      }

      break;

    default:
      break;
      
    } // end switch
  }  // end BCtag loop
}

// set SLB Type based on  sswId, moduleType
bool Muon::TgcSlbDataHelper::setType(uint16_t ,    // subDetectorId
				     uint16_t ,   // rodId 
				     TgcSlbData * slb, 
				     int          //moduleType
				     )  const
{
  bool ret = true;
  switch(slb->getSswId()) {
  case 9: // SL 
    if((slb->getSbLoc() == 4) || (slb->getSbLoc() == 5)) {
      slb->setType(TgcSlbData::SLB_SL_F);
    } else {
      slb->setType(TgcSlbData::SLB_SL_E);
    }
    break;

  case 8: // EI/FI
    // October 24, 2008, Susumu Oda
    // This is not correct. 
    // One SLB for EI/FI is shared by EI/FI wire and 
    // EI/FI strip. Wire and strip should be defined 
    // with not sbLoc but bit position. A,B-inputs 
    // are used by strip and C,D-inputs are used by wire. 
    // Since there is no immediate solution and no 
    // necessitiy, I leave the following several lines.  
    if((slb->getSbLoc() == 0) || (slb->getSbLoc() == 2)) {
      slb->setType(TgcSlbData::SLB_LPT_I_W);
    } else {
      slb->setType(TgcSlbData::SLB_LPT_I_S);
    }
    break;
 
  case 0:    // EWT/EST
  case 1:    // EWT/EST
  case 2:    // FWT/FST
    if(slb->getSbLoc() >=16) {
      slb->setType(TgcSlbData::SLB_LPT_T_S);
    } else {
      slb->setType(TgcSlbData::SLB_LPT_T_W);
    }
    break;
    
  case 3:   // EWD/ESD
  case 4:   // EWD/ESD
  case 5:   // EWD/ESD
  case 6:   // EWD/ESD
  case 7:   // FWD/FSD
    if(slb->getSbLoc() >=16) {
      slb->setType(TgcSlbData::SLB_LPT_D_S);
    } else {
      slb->setType(TgcSlbData::SLB_LPT_D_W);
    }
    break;

  default:
    slb->setType(TgcSlbData::SLB_NONE);
    ret = false;
    break;
  }
  return ret;
}

// Adjacent or not
bool Muon::TgcSlbDataHelper::isAdjacent(int ibit) const
{
  bool value=false;
  value = value || ((ibit<200) && (ibit>193)); 
  value = value || ((ibit<162) && (ibit>149)); 
  value = value || ((ibit<118) && (ibit>109)); 
  value = value || ((ibit<78) && (ibit>73)); 
  value = value || ((ibit<42) && (ibit>39)); 

  return value;
}

// set sector for HpT/SL  based on slbId 
uint16_t Muon::TgcSlbDataHelper::getSector(bool forward, 
					   uint16_t , // subDetectorId
					   uint16_t , // rodId
					   const TgcSlbData * slb) const
{
  uint16_t sector = 0;
  
  if(forward) {
    sector = slb->getSbLoc()-4;
  } else {
    sector = slb->getSbLoc();
  }
  return sector;
}

void Muon::TgcSlbDataHelper::getLPTforDoublet(const bool * bitArray,
					      bool hit[],
					      int delta[],
					      int pos[]) const
{
  // 2 x (10 bits of TRIG data)  
  //  0 PT0 PT1 PT2 SGN PS0 PS1 PS2 PS3 PS4

  size_t ibit=0;
  for(int blk=0; blk<N_LPT_D; blk++) {
    uint32_t binary = getVal(bitArray, ibit, 9);
    hit[blk] = false;
    // Delta
    delta[blk] = binary & 0x07;
    if(delta[blk]==0) {
      // check sign
      if(binary &0x08) hit[blk] = true;
    } else {
      hit[blk] = true;
      if((binary & 0x08)==0) delta[blk] *= -1;
    }
    // position
    if(hit[blk]) {
      pos[blk] = (binary & 0x1F0) >>4;
    } else {
      pos[blk] = 0;
    }
    ibit +=10;
  }
}

void Muon::TgcSlbDataHelper::getLPTforTripletWire(const bool * bitArray,
						  bool hit[],
						  int pos[]) const
{
  // 3 x ( 6/7 bits of TRIG Data )
  size_t ibit=0;
  int blk;
  uint32_t binary;

  // TRIG0
  //  PS0 PS1 PS2 PS3 PS4 HIT
  blk=0;
  binary = getVal(bitArray, ibit, 6);
  pos[blk] = 0;
  hit[blk] = ((binary & 0x20)!=0);
  // position
  if(hit[blk]) pos[blk] = (binary & 0x1F);
  ibit += 6;
  
  // TRIG1
  //  PS0 PS1 PS2 0 PS3 PS4 HIT
  blk=1;
  binary = getVal(bitArray, ibit, 3);
  pos[blk] = 0;
  hit[blk] = ((binary & 0x04) !=0);
  // position
  if(hit[blk]) pos[blk] = (binary & 0x03)*8;
  ibit += 4;
  binary = getVal(bitArray, ibit, 3);
  if(hit[blk]) pos[blk] += (binary & 0x07);
  ibit += 3;

  // TRIG2
  //  PS0 PS1 PS2 PS3 PS4 HIT
  blk=2;
  binary = getVal(bitArray, ibit, 6);
  pos[blk] = 0;
  hit[blk] = ((binary & 0x20) !=0);
  // position
  if(hit[blk]) pos[blk] = (binary & 0x1F);
  
}

void Muon::TgcSlbDataHelper::getLPTforTripletStrip(const bool * bitArray,
						   bool hit[],
						   int pos[]) const
{
  // 8 x (5bits of TRIG datat) 
  //  PS0 PS1 PS2 PS3 HIT

  size_t ibit=0;
  for(int blk=0; blk<N_LPT_TS; blk++) {
    uint32_t binary= getVal(bitArray, ibit, 5);
    pos[blk] = 0;
    hit[blk] = ((binary & 0x10) !=0);
    // position
    if(hit[blk]) pos[blk] = (binary & 0x0F);
    ibit += 5;
  }
}
  
void Muon::TgcSlbDataHelper::getLPTforInner(const bool * bitArray,
					    bool hit[]) const
{
  // 2 x (4bits of TRIG data) 
  //  TRIG3 TRIG2 TRIG1 TRIG0 
  // https://twiki.cern.ch/twiki/pub/Main/TgcDocument/celladdress2_asic_rev2.pdf

  size_t ibit=0;
  for(int blk=0; blk<N_LPT_I; blk++) {
    uint32_t binary= getVal(bitArray, ibit, 1);
    hit[blk] = (binary & 0x1);
    ibit += 1;
  }
}
  
void Muon::TgcSlbDataHelper::getSL_F(const bool * bitArray,
				      bool& cand3plus,
				      bool hit[],
				      bool muplus[],
				      uint16_t pt[],
				      bool overlap[],
				      uint16_t roi[]) const
{
  // 32bits of TRIG data to MuCTPI

  size_t ibit;
  uint32_t binary;

  // clear results
  for(size_t icand=0;icand <2 ; icand ++) {
    hit[icand] = false;
    muplus[icand] = false;
    pt[icand] = 7;
    overlap[icand] = false;
    roi[icand] = 0;
  }
  cand3plus = false;  


  // Morethan2: bit 199
  cand3plus = (*(bitArray+199));

  //1st Candidate
  // PT1<0:2> : bit 180 -178 
  ibit =178;   
  binary = getVal(bitArray, ibit, 3);
  pt[0] = binary;

  // check hit or not
  // NOHIT: pT ==7 or pT==0
  hit[0] = !((binary ==7) || (binary == 0));

  if(hit[0]) {  
    // Charge1 : bit 169
    muplus[0] = (*(bitArray + 169));
    
    // ROI1<0:5> : bit 198 -193
    ibit = 193;
    binary = getVal(bitArray, ibit, 6);
    roi[0] = binary;
  } 
  
  // 2nd candidate
  // PT2<0:2> :  bit 177 -175
  ibit = 175;
  binary = getVal(bitArray, ibit, 3);
  pt[1] = binary;

  // check hit or not
  // NOHIT: pT ==7 or pT==0
  hit[1] = !((binary ==7) || (binary == 0));
  if(hit[1]) {
    // Charge2 : bit 168
    muplus[1] = (*(bitArray + 168));
  
    // ROI2<0:5>  : bit 189 -184
    ibit =184;
    binary  = getVal(bitArray, ibit, 6);
    roi[1] = binary;
  }
  
}

void Muon::TgcSlbDataHelper::getSL_E(const bool * bitArray,
				     bool& cand3plus,
				     bool hit[],
				     bool muplus[],
				     uint16_t pt[],
				      bool overlap[],
				     uint16_t roi[]) const
{
  // 32bits of TRIG data to MuCTPI

  size_t ibit;
  uint32_t binary;
  
  // clear results
  for(size_t icand=0;icand <2 ; icand ++) {
    hit[icand] = false;
    muplus[icand] = false;
    pt[icand] = 7;
    overlap[icand] = false;
    roi[icand] = 0;
  }
  cand3plus = false;  

  // Morethan2: bit 199
  cand3plus = (*(bitArray+199));

  //1st Candidate
  // PT1<0:2> : bit 180 -178 
  ibit =178;   
  binary = getVal(bitArray, ibit, 3);
  pt[0] = binary;

  // check hit or not
  // NOHIT: pT ==7 or pT==0
  hit[0] = !((binary ==7) || (binary == 0));

  if(hit[0]) {  
    // Charge1 : bit 169
    muplus[0] = (*(bitArray + 169));
    
    // ROI1<0:7> : bit 198 -191
    ibit =191;
    binary = getVal(bitArray, ibit, 8);
    roi[0] = binary;
  }

  // 2nd candidate
  // PT2<0:2> :  bit 177 -175
  ibit =175;
  binary = getVal(bitArray, ibit, 3);
  pt[1] = binary;

  // check hit or not
  // NOHIT: pT ==7 or pT==0
  hit[1] = !((binary ==7) || (binary == 0));

  if(hit[1]) {
    // Charge2 : bit 168
    muplus[1] = (*(bitArray + 168));
  
    // ROI2<0:7>  : bit 189 -182
    ibit =182;
    binary = getVal(bitArray, ibit, 8);
    roi[1] = binary;
  }
  
}

void Muon::TgcSlbDataHelper::getHPT_F(const bool * bitArray,
				      bool hit[],
				      bool strip[],
				      uint16_t chip[],
				      uint16_t index[],
				      bool hipt[],
				      uint16_t hitId[],
				      uint16_t pos[],
				      int16_t delta[]) const
{
  int16_t dR;
  bool sign;
  size_t ibit;
  int width;
  size_t ichip;

  size_t icand =0; 

  // chip 0 for wire : bit 159 ..140
  //  1st hit :159-150, 2nd hit :149-140 
  //   dR<0:3>, Sign, H/L, Pos, ID<0:2> 
  ichip = 0;
  ibit = 160; // start position
  for(size_t idx = 0; idx <2; ++idx) {
    chip[icand] = ichip;
    strip[icand] = false;
    index[icand] = 1 - idx;
    // dR 
    width = 4;
    ibit -= width;
    dR = getVal(bitArray, ibit, width);
    //sign 
    width = 1;
    ibit -= width;
    sign = *(bitArray+ibit);
    // calculate delta with sign  
    if(sign) {
      hit[icand] = true;
      delta[icand] = dR;
    } else {
      if(dR==0) {
        hit[icand] = false;
        delta[icand] = 0;
      } else {
        hit[icand] = true;
        delta[icand] = -dR;
      } 
    }
 
    //  H/L
    width = 1;
    ibit -= width;
    hipt[icand] = *(bitArray + ibit);

    //  pos
    width = 1;
    ibit -= width;
    pos[icand] = getVal(bitArray, ibit, width);

    // Id
    width = 3;
    ibit -=width;
    hitId[icand] = getVal(bitArray, ibit, width);

    icand +=1;
  }

  // chip 1 for wire : bit 135 ..120
  //  1st hit :135-128, 2nd hit :127-120 
  //   dR<0:3>, Sign, H/L, Pos, ID<0> 
  ichip=1;
  ibit = 136;  //start position
  for(size_t idx = 0; idx <2; ++idx) {
    chip[icand] = ichip;
    strip[icand] = false; 
    index[icand] = 1 - idx;
    // dR 
    width = 4;
    ibit -= width;
    dR = getVal(bitArray, ibit, width);
    //sign 
    width = 1;
    ibit -= width;
    sign = *(bitArray+ibit);
    // calculate delta with sign  
    if(sign) {
      hit[icand] = true;
      delta[icand] = dR;
    } else {
      if(dR==0) {
        hit[icand] = false;
        delta[icand] = 0;
      } else {
        hit[icand] = true;
        delta[icand] = -dR;
      } 
    }
    
    //  H/L
    width = 1;
    ibit -= width;
    hipt[icand] = *(bitArray + ibit);
    
    //  pos
    width = 1;
    ibit -= width;
    pos[icand] = getVal(bitArray, ibit, width);
    
    // Id
    width = 1;
    ibit -=width;
    hitId[icand] = getVal(bitArray, ibit, width);
    if(hitId[icand] == 0 && hit[icand]) hitId[icand] = 2;

    icand +=1;
  }
  
  // chip 0 for strip : bit 119 ..104
  //  1st hit :119-113, 2nd hit :111-105 
  //   dPhi<0:2>, Sign, H/L, Pos, ID<0> 
  int16_t dPhi;
  ichip = 0; // chip#0 for strip
  ibit = 120;
  for(size_t idx = 0; idx <2; ++idx) {
    chip[icand] = ichip;
    strip[icand] = true;
    index[icand] = 1 - idx;

    // dPhi 
    width = 3;
    ibit -= width;
    dPhi = getVal(bitArray, ibit, width);
    //sign 
    width = 1;
    ibit -= width;
    sign = *(bitArray+ibit);
    // calculate delta with sign  
    if(sign) {
      hit[icand] = true;
      delta[icand] = dPhi;
    } else {
      if(dPhi ==0) {
        hit[icand] = false;
        delta[icand] = 0;
      } else {
        hit[icand] = true;
        delta[icand] = -dPhi;
      } 
    }
    
    //  H/L
    width = 1;
    ibit -= width;
    hipt[icand] = *(bitArray + ibit);
    
    //  pos
    width = 1;
    ibit -= width;
    pos[icand] = getVal(bitArray, ibit, width);
    
    // Id
    // Only HitId<0> is sent 
    width = 1;
    ibit -=width;
    hitId[icand] = getVal(bitArray, ibit, width);
    if(hitId[icand] == 0 && hit[icand]) hitId[icand] = 2;

    // dummy bit 
    ibit -= 1;
 
    icand += 1;
  }

}

void Muon::TgcSlbDataHelper::getHPT_E(const bool * bitArray,
				       bool hit[],
				       bool strip[],
				       uint16_t chip[],
				       uint16_t index[],
				       bool hipt[],
				       uint16_t hitId[],
				       uint16_t pos[],
				       int16_t delta[]) const
{
  int16_t dR;
  bool sign;
  size_t ibit;
  int width;
  size_t ichip;

  size_t icand =0; 

  // chip 0 for wire : bit 159 ..152
  //  1st hit :159-153
  //   dR<0:3>, Sign, H/L, Pos 
  ichip = 0;
  ibit = 160; // start position

  chip[icand] = ichip;
  index[icand] = 0;  // only 1 hit in chip 0
  strip[icand] = false;

  // dR  : bit 159-156
  width = 4;
  ibit -= width;
  dR = getVal(bitArray, ibit, width);
  //sign   : bit 155
  width = 1;
  ibit -= width;
  sign = *(bitArray+ibit);
  // calculate delta with sign  
  if(sign) {
    hit[icand] = true;
    delta[icand] = dR;
  } else {
    if(dR==0) {
      hit[icand] = false;
      delta[icand] = 0;
    } else {
      hit[icand] = true;
      delta[icand] = -dR;
    } 
  }

  //  H/L   : bit 154
  width = 1;
  ibit -= width;
  hipt[icand] = *(bitArray + ibit);
  
  //  pos   : bit 153
  width = 1;
  ibit -= width;
  pos[icand] = getVal(bitArray, ibit, width);
  
  // Id
  //  Id is set to 1  
  if(hit[icand]) hitId[icand] = 1;

  icand +=1;
 
  // chip 1, 2, 3 for Wire
  //   chip 1 : 1st hit :151-142, 2nd hit :141-132 
  //   chip 2 : 1st hit :131-122, 2nd hit :121-112 
  //   chip 3 : 1st hit :111-102, 2nd hit :101- 92 
  //   dR<0:3>, Sign, H/L, Pos, ID<0:2> 
  ibit = 152; // start position 

  for(size_t ichip=1; ichip<=3; ++ichip) {
    for(size_t idx = 0; idx <2; ++idx) {
      chip[icand] = ichip;
      strip[icand] = false;
      index[icand] = 1 - idx;
      // dR  
      width = 4;
      ibit -= width;
      dR = getVal(bitArray, ibit, width);
      //sign 
      width = 1;
      ibit -= width;
      sign = *(bitArray+ibit);
      // calculate delta with sign  
      if(sign) {
	hit[icand] = true;
	delta[icand] = dR;
      } else {
	if(dR==0) {
	  hit[icand] = false;
	  delta[icand] = 0;
	} else {
	  hit[icand] = true;
	  delta[icand] = -dR;
	} 
      }

      //  H/L
      width = 1;
      ibit -= width;
      hipt[icand] = *(bitArray + ibit);
      
      //  pos
      width = 1;
      ibit -= width;
      pos[icand] = getVal(bitArray, ibit, width);
      
      // Id
      width = 3;
      ibit -=width;
      hitId[icand] = getVal(bitArray, ibit, width);
      
      icand +=1;
    }
  }

  // chip 0,1 for strip
  // chip 0 :  1st hit : 87-79, 2nd hit :78-70 
  //   dPhi<0:2>, Sign, H/L, Pos, ID<0:2> 
  // chip 1 :  1st hit : 63-56, 2nd hit :55-48 
  //   dPhi<0:2>, Sign, H/L, Pos, ID<0>, ID<2> 

  int16_t dPhi;
  ibit = 88; // chip0 start position 
  for(size_t ichip=0; ichip<=1; ++ichip) {
    if(ichip==1) ibit = 64; // chip1 start position 

    for(size_t idx = 0; idx < 2; ++idx) {
      chip[icand] = ichip;
      strip[icand] = true;
      index[icand] = 1 - idx;

      // dPhi 
      width = 3;
      ibit -= width;
      dPhi = getVal(bitArray, ibit, width);
      //sign 
      width = 1;
      ibit -= width;
      sign = *(bitArray+ibit);
      // calculate delta with sign  
      if(sign) {
	hit[icand] = true;
	delta[icand] = dPhi;
      } else {
	if(dPhi==0) {
	  hit[icand] = false;
	  delta[icand] = 0;
	} else {
	  hit[icand] = true;
	  delta[icand] = -dPhi;
	} 
      }
    
      //  H/L
      width = 1;
      ibit -= width;
      hipt[icand] =  *(bitArray + ibit);
      
      //  pos
      width = 1;
      ibit -= width;
      pos[icand] = getVal(bitArray, ibit, width);
      
      // Id
      if(ichip==0) {
	// chip0 
	width = 3;
	ibit -= width;
	hitId[icand] = getVal(bitArray, ibit, width);

      } else {
	// chip 1
        //  2bit : hitId<1>,hitId<0>
	width = 2;
	ibit -= width;
	hitId[icand] = 0;
	if(hit[icand]) {
	  hitId[icand] = getVal(bitArray, ibit, width);

	  // duplicate hit for T5 and T6
          //  because of bug in strip endcap HPT
	  chip[icand+1]  = ichip;
	  strip[icand+1] = true;
	  index[icand+1] = 1 - idx;
	  hit[icand+1]   = true;
	  delta[icand+1] = delta[icand];
	  pos[icand+1]   = pos[icand];
	  hitId[icand+1] = hitId[icand] + 4;
	  hipt[icand+1]  =  hipt[icand];
	} else {
	  // fill no HIT
	  chip[icand+1]  = ichip;
	  strip[icand+1] = true;
	  index[icand+1] = 1 - idx;
	  hit[icand+1]   = false;
	  delta[icand+1] = 0;
	  pos[icand+1]   = 0;
	  hitId[icand+1] = 0;
   	  hipt[icand+1]  = 0;
	}
	icand++; 
      }

      icand += 1;
    }
  }

}

// check consistencies of HPT output
bool Muon::TgcSlbDataHelper::isValid_HPT(bool hit,
					 bool strip,
					 uint16_t , //chip
					 uint16_t , //index
					 bool hipt,
					 uint16_t hitId,
					 uint16_t , //pos
					 int16_t delta) const
{
  bool isOK = true;
 
  // check hitId
  if(hit) {
    // check hitId
    isOK = (hitId !=0) && (hitId !=7);
    // check delta
    if(!hipt || strip) {
      isOK = isOK && (abs(delta)<8);
    }     
  } else {    
    // check hitId
    isOK = (hitId==0);
  }
  
  return isOK; 
}

uint16_t Muon::TgcSlbDataHelper::getVal(const bool* bitArray, size_t start, size_t width) const
{
  // utility function to get a value of 
  // bitArray[ start , start + width -1]
  //    start            : MSB
  //    start + width -1 : LSB   
  uint16_t binary=0;
  size_t ibit = start+width-1;
  for(size_t i= 0; i<width; ++i) {
    if(bitArray[ibit]) {
      binary += (1<<i);
    }
    ibit -=1;
  }
  return binary;
}
