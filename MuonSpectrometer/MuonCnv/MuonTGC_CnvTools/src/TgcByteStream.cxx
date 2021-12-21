/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcByteStream.h"

#include "TgcByteStreamData.h"
#include "MuonRDO/TgcRdo.h"

#include "GaudiKernel/MsgStream.h"

Muon::TgcByteStream::TgcByteStream()
{
}

Muon::TgcByteStream::~TgcByteStream()
{
}

void Muon::TgcByteStream::rdo2ByteStream(const TgcRdo* rdo, ByteStream& bs, MsgStream& log)
{
  bool p_debug = (log.level() <= MSG::DEBUG);

  if(p_debug) {
    log << MSG::DEBUG << "TgcByteStream::rdo2ByteStream" << endmsg;
  }
  
  ByteStream headerBS, statusBS, countersBS, dataBS, footerBS;
  TGC_BYTESTREAM_FRAGMENTCOUNT counters[7] = {
    {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 8}, {0, 9}
  };
  TGC_BYTESTREAM_LOCALSTATUS ls;
  
  if(p_debug) {
    log << MSG::DEBUG << "Encoding " << rdo->size() << " elements" << endmsg;
  }

  for (const TgcRawData* raw : *rdo)
    {
      switch(raw->type())
        {
        case TgcRawData::TYPE_HIT:
	  ls.hitsRO = 1;
	  counters[1].count++;
	  {
	    TGC_BYTESTREAM_READOUTHIT roh;
	    roh.channel = raw->channel()-40;
	    roh.sbId = raw->slbId();
            // revised document : https://twiki.cern.ch/twiki/pub/Main/TgcDocument/ROBformat_V4_0_v2.pdf
	    // (old ver. : http://cern.ch/atlas-tgc/doc/ROBformat.pdf)
	    // Table 7 : SB type, bits 15..13
	    // 0,1: doublet wire, strip
	    // 2,3: triplet wire, strip triplet;
	    // 4  : inner wire and strip
	    // TgcRawData::SlbType is defined in TgcRawData.h
	    // 0: SLB_TYPE_DOUBLET_WIRE,
	    // 1: SLB_TYPE_DOUBLET_STRIP,
	    // 2: SLB_TYPE_TRIPLET_WIRE,
	    // 3: SLB_TYPE_TRIPLET_STRIP,
	    // 4: SLB_TYPE_INNER_WIRE,
	    // 5: SLB_TYPE_INNER_STRIP,
	    // 6: SLB_TYPE_UNKNOWN
	    roh.sbType = raw->slbType();
	    
	    // SBLOCs for EIFI are different in online (ByteStream) and offline (RDO).
	    // bug #57051: Wrong numbering of SBLOC for Inner Stations (EI/FI) in 12-fold TGC cablings
	    // ByteStream : slbId = SBLOC + ip*2 (ip=0, 1, 2), SBLOC = 8 or 9 (EI), 0 or 1 (FI)
	    //              slbId =  8, 10, 12,  9, 11, 13,  0,  2,  4,  1,  3,  5
	    // RDO        : slbId = SBLOC + ip*4 (ip=0, 1, 2), SBLOC = 1 or 3 (EI), 0 or 2 (FI)
	    //              slbId =  1,  5,  9,  3,  7, 11,  0,  4,  8,  2,  6, 10
	    if(roh.sbType==TgcRawData::SLB_TYPE_INNER_WIRE || 
	       roh.sbType==TgcRawData::SLB_TYPE_INNER_STRIP) {
	      if(raw->slbId()%2==0) roh.sbId =  raw->slbId()    /2;
	      else                  roh.sbId = (raw->slbId()+15)/2;
	    }

	    roh.adj = raw->isAdjacent();
	    roh.ldbId = raw->sswId();
	    roh.bcBitmap = bcBitmap(raw->bcTag());
	    roh.tracklet = raw->tracklet();
	    roh.ok = 1;
	    dataBS.push_back(toBS32(roh));
	  }
	  break;
        case TgcRawData::TYPE_TRACKLET:
	  ls.trkRO = 1;
	  counters[2].count++;
	  if(raw->slbType() == TgcRawData::SLB_TYPE_TRIPLET_STRIP)
            {
	      TGC_BYTESTREAM_READOUTTRIPLETSTRIP rot;
	      rot.phi = raw->position();
	      rot.seg = raw->segment();
	      rot.sbId = raw->slbId();
	      rot.ldbId = raw->sswId();
	      rot.bcBitmap = bcBitmap(raw->bcTag());
	      rot.slbType = TgcRawData::SLB_TYPE_TRIPLET_STRIP;
	      rot.ok = 1;
	      dataBS.push_back(toBS32(rot));
            }
	  else
            {
	      TGC_BYTESTREAM_READOUTTRACKLET rot;
	      rot.rphi = raw->position();
	      rot.subm = raw->subMatrix();
	      rot.seg = raw->segment();
	      rot.delta = raw->delta();
	      rot.sbId = raw->slbId();
	      rot.ldbId = raw->sswId();
	      rot.bcBitmap = bcBitmap(raw->bcTag());
              // revised document : https://twiki.cern.ch/twiki/pub/Main/TgcDocument/ROBformat_V4_0_v2.pdf
              // (old ver. : http://cern.ch/atlas-tgc/doc/ROBformat.pdf)
	      // Table 8 : Slave Board type, bits 30..28
	      // 0,1: doublet wire, strip
	      // 2,3: triplet wire, strip triplet; 
	      // 4,5: inner wire, strip
	      // TgcRawData::SlbType is defined in TgcRawData.h 
	      // 0: SLB_TYPE_DOUBLET_WIRE,
	      // 1: SLB_TYPE_DOUBLET_STRIP,
	      // 2: SLB_TYPE_TRIPLET_WIRE,
	      // 3: SLB_TYPE_TRIPLET_STRIP,
	      // 4: SLB_TYPE_INNER_WIRE,
	      // 5: SLB_TYPE_INNER_STRIP,
	      // 6: SLB_TYPE_UNKNOWN
	      rot.slbType = raw->slbType();
	      rot.ok = 1;
	      dataBS.push_back(toBS32(rot));
            }
	  break;
        case TgcRawData::TYPE_HIPT:
	  ls.hipt = 1;
	  counters[5].count++;
	  if(raw->isStrip() == 1 && raw->sector() & 4 ){
            TGC_BYTESTREAM_HIPT_INNER hpt;
            hpt.inner = raw->inner();
            //hpt.hipt = raw->isHipt();
            //hpt.cand = raw->index();
            //hpt.chip = raw->chip();
            hpt.sector = raw->sector();
            //hpt.fwd = raw->isForward();
            hpt.strip = raw->isStrip();
            hpt.bcBitmap = bcBitmap(raw->bcTag());
            dataBS.push_back(toBS32(hpt));
          }else{
            TGC_BYTESTREAM_HIPT hpt;
            hpt.delta = raw->delta();
            hpt.sub = raw->hsub();
            hpt.hitId = raw->hitId();
            hpt.hipt = raw->isHipt();
            hpt.cand = raw->index();
            hpt.chip = raw->chip();
            hpt.sector = raw->sector();
            hpt.fwd = raw->isForward();
            hpt.strip = raw->isStrip();
            hpt.bcBitmap = bcBitmap(raw->bcTag());
            dataBS.push_back(toBS32(hpt));
          }
          break;
        case TgcRawData::TYPE_SL:
          ls.sl = 1;
          counters[6].count++;
          {
            TGC_BYTESTREAM_SL sl;
            sl.roi = raw->roi();
            sl.overlap = raw->isOverlap();
            sl.veto = raw->isVeto();
            sl.threshold = raw->threshold();
            sl.sign = raw->isMuplus();
            sl.cand = raw->index();
            sl.sector = raw->sector();
            sl.fwd = raw->isForward();
            sl.bcBitmap = bcBitmap(raw->bcTag());
            sl.cand2plus = raw->cand3plus();
            dataBS.push_back(toBS32(sl));
          }
          break;
        default:
          log << MSG::ERROR << "Invalid type " << raw->typeName() << endmsg;
          break;
        }
    }
  
  statusBS.push_back(toBS32(ls));
  statusBS.push_back(rdo->orbit());
  
  for(int iCnt = 0; iCnt < 7; iCnt++)
    {
      if(counters[iCnt].count > 0)
        {
	  countersBS.push_back(toBS32(counters[iCnt]));
        }
    }
  
  bs.clear();
  bs.insert(bs.end(), countersBS.begin(), countersBS.end());
  bs.insert(bs.end(), dataBS.begin(),     dataBS.end());
  
  if(p_debug) {
    log << MSG::DEBUG << "TgcByteStream::rdo2ByteStream done" << endmsg;
  }
}

void Muon::TgcByteStream::byteStream2Rdo(const ByteStream& bs, TgcRdo& rdo, uint32_t source_id, 
					 MsgStream& log)
{
  // Check that we are filling the right collection
  
  bool p_debug = (log.level() <= MSG::DEBUG);

  TGC_BYTESTREAM_SORUCEID sid;
  fromBS32(source_id, sid);
  
  if(rdo.identify() != TgcRdo::calculateOnlineId(sid.side, sid.rodid))
    {
      if(p_debug) {
	log << MSG::DEBUG << "Error: input TgcRdo id does not match bytestream id" << endmsg;
      }
      return;
    }
  
  rdo.setOnlineId(sid.side, sid.rodid); 
  
  TGC_BYTESTREAM_FRAGMENTCOUNT counters[7] = {
    {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 8}, {0, 9}
  };
  TGC_BYTESTREAM_FRAGMENTCOUNT counter;
  
  
  int iBs = 0;
  for(int iCnt = 0; iCnt < 7; iCnt++)
    {
      fromBS32(bs[iBs], counter);
      if(counter.id == counters[iCnt].id)
        {
	  counters[iCnt].count = counter.count;
	  iBs++;
        }
    }
  
  for(int iCnt = 0; iCnt < 7; iCnt++)
    {
      if(counters[iCnt].count == 0) continue;
      switch(counters[iCnt].id)
        {
        case 1: // Raw data format (SSW format)
	  {
	    if(p_debug) {
	      log << MSG::DEBUG 
		  << "fragment" << counters[iCnt].id << " " 
		  << counters[iCnt].count << "words" << endmsg;
	    }
	    for(unsigned iFrag = 0; iFrag < counters[iCnt].count; iFrag++)
	      {
		if(p_debug) {
		  log << MSG::DEBUG 
		      << "WORD" << iFrag << ":" << MSG::hex << bs[iBs] << endmsg;
		}
		iBs++;
	      }
	    break;
	  }

        case 2: // TgcRawData::TYPE_HIT
	  {
	    if(p_debug) {
	      log << MSG::DEBUG << "fragment" 
		  << counters[iCnt].id << " " << counters[iCnt].count
		  << "words" << endmsg;
	    }
	    TGC_BYTESTREAM_READOUTHIT roh;
	    for(unsigned iFrag = 0; iFrag < counters[iCnt].count; iFrag++)
	      {
		if(p_debug) {
		  log << MSG::DEBUG <<"WORD" << iFrag << ":" << MSG::hex << bs[iBs] << endmsg;
		}
		fromBS32(bs[iBs++], roh);
		  
		  
		if(p_debug) {
		  log<< MSG::DEBUG
		     << " rdo.subDetectorId():" << rdo.subDetectorId()
		     << " rdo.rodId():" <<rdo.rodId()
		     << " roh.ldbId:" <<roh.ldbId
		     << " roh.sbId:" <<roh.sbId
		     << " rdo.l1Id():"<<rdo.l1Id()
		     << " rdo.bcId():"<<rdo.bcId()<<endmsg;
		} 
		  
		uint16_t slbId = roh.sbId;
                // SBLOCs for EIFI are different in online (ByteStream) and offline (RDO).
                // bug #57051: Wrong numbering of SBLOC for Inner Stations (EI/FI) in 12-fold TGC cablings
		// ByteStream : slbId = SBLOC + ip*2 (ip=0, 1, 2), SBLOC = 8 or 9 (EI), 0 or 1 (FI)
                //              slbId =  8, 10, 12,  9, 11, 13,  0,  2,  4,  1,  3,  5
                // RDO        : slbId = SBLOC + ip*4 (ip=0, 1, 2), SBLOC = 1 or 3 (EI), 0 or 2 (FI)
                //              slbId =  1,  5,  9,  3,  7, 11,  0,  4,  8,  2,  6, 10
                if(roh.sbType==TgcRawData::SLB_TYPE_INNER_WIRE ||
                   roh.sbType==TgcRawData::SLB_TYPE_INNER_STRIP) {
                  if(roh.sbId<8) slbId =  roh.sbId   *2;
                  else           slbId = (roh.sbId-8)*2+1;
                }

		TgcRawData* raw =  new TgcRawData(bcTag(roh.bcBitmap),
						  rdo.subDetectorId(),
						  rdo.rodId(),
						  roh.ldbId,
						  slbId,
						  rdo.l1Id(),
						  rdo.bcId(),
                                                  // revised document : https://twiki.cern.ch/twiki/pub/Main/TgcDocument/ROBformat_V4_0_v2.pdf
                                                  // (old ver. : http://cern.ch/atlas-tgc/doc/ROBformat.pdf)
                                                  // Table 7 : SB type, bits 15..13
                                                  // 0,1: doublet wire, strip
                                                  // 2,3: triplet wire, strip triplet;
                                                  // 4  : inner wire and strip
                                                  // TgcRawData::SlbType is defined in TgcRawData.h
                                                  // 0: SLB_TYPE_DOUBLET_WIRE,
                                                  // 1: SLB_TYPE_DOUBLET_STRIP,
                                                  // 2: SLB_TYPE_TRIPLET_WIRE,
                                                  // 3: SLB_TYPE_TRIPLET_STRIP,
                                                  // 4: SLB_TYPE_INNER_WIRE,
                                                  // 5: SLB_TYPE_INNER_STRIP,
                                                  // 6: SLB_TYPE_UNKNOWN
						  (TgcRawData::SlbType)roh.sbType,
						  (bool)roh.adj,
						  roh.tracklet,
						  roh.channel+40);
		rdo.push_back(raw);
	      }
	    break;
	  }
        case 3: // TgcRawData::TYPE_TRACKLET
	  {
	    if(p_debug) {
	      log << MSG::DEBUG << "fragment"
		  << counters[iCnt].id << " " << counters[iCnt].count 
		  << "words" << endmsg;
	    }
	    TGC_BYTESTREAM_READOUTTRIPLETSTRIP rostrip;
	    TGC_BYTESTREAM_READOUTTRACKLET rotrk;
	    for(unsigned iFrag = 0; iFrag < counters[iCnt].count; iFrag++)
	      {
		if(p_debug) {
		  log<< MSG::DEBUG << "WORD"
		     << iFrag << ":" << MSG::hex << bs[iBs] << endmsg;
		}
		fromBS32(bs[iBs], rostrip);
		
		if(rostrip.slbType == TgcRawData::SLB_TYPE_TRIPLET_STRIP)
		  {
		    TgcRawData* raw = new TgcRawData(bcTag(rostrip.bcBitmap),
						     rdo.subDetectorId(),
						     rdo.rodId(),
						     rostrip.ldbId,
						     rostrip.sbId,
						     rdo.l1Id(),
						     rdo.bcId(),
						     TgcRawData::SLB_TYPE_TRIPLET_STRIP,
						     0,
						     rostrip.seg,
						     rostrip.subc,
						     rostrip.phi);
		    rdo.push_back(raw);
		  }
		else
		  {
		    fromBS32(bs[iBs], rotrk);
		    TgcRawData* raw = new TgcRawData(bcTag(rotrk.bcBitmap),
						     rdo.subDetectorId(),
						     rdo.rodId(),
						     rotrk.ldbId,
						     rotrk.sbId,
						     rdo.l1Id(),
						     rdo.bcId(),
                                                     // revised document : https://twiki.cern.ch/twiki/pub/Main/TgcDocument/ROBformat_V4_0_v2.pdf
                                                     // (old ver. : http://cern.ch/atlas-tgc/doc/ROBformat.pdf)
						     // Table 8 : Slave Board type, bits 30..28
						     // 0,1: doublet wire, strip
						     // 2,3: triplet wire, strip triplet;
						     // 4,5: inner wire, strip
						     // TgcRawData::SlbType is defined in TgcRawData.h
						     // 0: SLB_TYPE_DOUBLET_WIRE,
						     // 1: SLB_TYPE_DOUBLET_STRIP,
						     // 2: SLB_TYPE_TRIPLET_WIRE,
						     // 3: SLB_TYPE_TRIPLET_STRIP,
						     // 4: SLB_TYPE_INNER_WIRE,
						     // 5: SLB_TYPE_INNER_STRIP,
						     // 6: SLB_TYPE_UNKNOWN
						     (TgcRawData::SlbType)rotrk.slbType,
						     rotrk.delta,
						     rotrk.seg,
						     0,
						     rotrk.rphi);
                    rdo.push_back(raw);
		  }
		iBs++;
	      }
	    break;
	  }
        case 8: // TgcRawData::TYPE_HIPT
	  {
	    if(p_debug) {
	      log << MSG::DEBUG << "fragment"
		  << counters[iCnt].id << " " << counters[iCnt].count 
		  << "words" << endmsg;
	    }
	    TGC_BYTESTREAM_HIPT       hpt;
	    TGC_BYTESTREAM_HIPT_INNER hptinner;
	    for(unsigned iFrag = 0; iFrag < counters[iCnt].count; iFrag++)
	      {
		if(p_debug) {
		  log << MSG::DEBUG << "WORD"
		      << iFrag << ":" << MSG::hex << bs[iBs] << endmsg;
		}
		fromBS32(bs[iBs], hptinner);
		if(hptinner.sector & 4){
                  TgcRawData* raw = new TgcRawData(bcTag(hpt.bcBitmap),
                                                   rdo.subDetectorId(),
                                                   rdo.rodId(),
                                                   rdo.l1Id(),
                                                   rdo.bcId(),
                                                   hptinner.strip,
                                                   0,
                                                   hptinner.sector,
                                                   0,
                                                   0,
                                                   0,
                                                   0,
                                                   0,
                                                   0,
                                                   hptinner.inner);
                  rdo.push_back(raw);
                }else{
                  fromBS32(bs[iBs], hpt);
                  TgcRawData* raw = new TgcRawData(bcTag(hpt.bcBitmap),
                                                   rdo.subDetectorId(),
                                                   rdo.rodId(),
                                                   rdo.l1Id(),
                                                   rdo.bcId(),
                                                   hpt.strip,
                                                   hpt.fwd,
                                                   hpt.sector,
                                                   hpt.chip,
                                                   hpt.cand,
                                                   hpt.hipt,
                                                   hpt.hitId,
                                                   hpt.sub,
                                                   hpt.delta,
                                                   0);
                  rdo.push_back(raw);
                }
                iBs++;
              }
            break;
	  }
        case 9: // TgcRawData::TYPE_SL
	  {
	    if(p_debug) {
	      log << MSG::DEBUG << "fragment"
		  << counters[iCnt].id << " " << counters[iCnt].count 
		  << "words" << endmsg;
	    }
	    TGC_BYTESTREAM_SL sl;
	    for(unsigned iFrag = 0; iFrag < counters[iCnt].count; iFrag++)
	      {
		if(p_debug) {
		  log << MSG::DEBUG << "WORD"
		      << iFrag << ":" << MSG::hex << bs[iBs] << endmsg;
		}
		fromBS32(bs[iBs++], sl);
		
		TgcRawData* raw = new TgcRawData(bcTag(sl.bcBitmap),
						 rdo.subDetectorId(),
						 rdo.rodId(),
						 rdo.l1Id(),
						 rdo.bcId(),
						 sl.cand2plus,
						 sl.fwd,
						 sl.sector,
						 sl.cand,
						 sl.sign,
						 sl.threshold,
						 sl.overlap,
						 sl.veto,
						 sl.roi);
		rdo.push_back(raw);
	      }
	    break;
	  }
        default:
	  if(p_debug) {
	    log << MSG::DEBUG 
		<< "Error: TgcByteStream::byteStream2Rdo Unsupported fragment type " 
		<< counters[iCnt].id << endmsg;
	  }
	  break;
        }
    }
  
  if(p_debug) {
    log << MSG::DEBUG << "Decoded " << MSG::dec << rdo.size() 
        << " elements" << endmsg;
    log << MSG::DEBUG << "TgcByteStream::byteStream2Rdo done" << endmsg;
  }
}
