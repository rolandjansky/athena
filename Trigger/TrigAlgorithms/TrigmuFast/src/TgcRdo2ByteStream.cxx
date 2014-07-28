/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LVL2_MUON_CALIBRATION_STANDALONE

#include "MuonRDO/TgcRawData.h"
#include "MuCalDecode/TgcCalibBSData.h"
#include "GaudiKernel/MsgStream.h"

#else

#include <iostream>
#include "TgcRawData.h"
#include "TgcCalibBSData.h"

#endif


using namespace LVL2_MUON_CALIBRATION;

#ifndef  LVL2_MUON_CALIBRATION_STANDALONE
void TgcRdo2ByteStream(TgcRawData* raw, MsgStream& log,
                           TGC_BYTESTREAM_READOUTHIT& roh,
			   TGC_BYTESTREAM_READOUTTRIPLETSTRIP& tps,
			   TGC_BYTESTREAM_READOUTTRACKLET& rot,
			   TGC_BYTESTREAM_HIPT& hpt,
			   TGC_BYTESTREAM_SL& sl )
#else
void TgcRdo2ByteStream(TgcRawData* raw,
                           TGC_BYTESTREAM_READOUTHIT& roh,
			   TGC_BYTESTREAM_READOUTTRIPLETSTRIP& tps,
			   TGC_BYTESTREAM_READOUTTRACKLET& rot,
			   TGC_BYTESTREAM_HIPT& hpt,
			   TGC_BYTESTREAM_SL& sl )
#endif


{
    uint32_t word = 0x0;
    fromBS32(word, roh);
    fromBS32(word, tps);
    fromBS32(word, rot);
    fromBS32(word, hpt);
    fromBS32(word, sl);
    
    switch (raw->type())
    {
    case TgcRawData::TYPE_HIT:
        {
            //TGC_BYTESTREAM_READOUTHIT roh;
            roh.channel = raw->channel()-40;
            roh.sbId = raw->slbId();
            roh.sbType = raw->slbType();
            roh.adj = raw->isAdjacent();
            roh.ldbId = raw->sswId();
            roh.bcBitmap = bcBitmap(raw->bcTag());
            roh.tracklet = raw->tracklet();
            roh.ok = 1;
            //return(toBS32(roh));
	    return;
        }
        break;
    case TgcRawData::TYPE_TRACKLET:
        if (raw->slbType() == TgcRawData::SLB_TYPE_TRIPLET_STRIP)
        {
            //TGC_BYTESTREAM_READOUTTRIPLETSTRIP rot;
            tps.phi = raw->position();
            tps.seg = raw->segment();
            tps.sbId = raw->slbId();
            tps.ldbId = raw->sswId();
            tps.bcBitmap = bcBitmap(raw->bcTag());
            tps.slbType = TgcRawData::SLB_TYPE_TRIPLET_STRIP;
            tps.ok = 1;
            //return(toBS32(rot));
	    return;
        }
        else
        {
            //TGC_BYTESTREAM_READOUTTRACKLET rot;
            rot.rphi = raw->position();
            rot.subm = raw->subMatrix();
            rot.seg = raw->segment();
            rot.delta = raw->delta();
            rot.sbId = raw->slbId();
            rot.ldbId = raw->sswId();
            rot.bcBitmap = bcBitmap(raw->bcTag());
            rot.slbType = raw->slbType();
            if (raw->slbType() == TgcRawData::SLB_TYPE_INNER_STRIP)
                rot.slbType = 4;
            rot.ok = 1;
            //return(toBS32(rot));
	    return;
        }
        break;
    case TgcRawData::TYPE_HIPT:
        {
            //TGC_BYTESTREAM_HIPT hpt;
            hpt.delta = raw->delta();
            hpt.hitId = raw->hitId();
            hpt.hipt = raw->isHipt();
            hpt.cand = raw->index();
            hpt.chip = raw->chip();
            hpt.sector = raw->sector();
            hpt.fwd = raw->isForward();
            hpt.strip = raw->isStrip();
            hpt.bcBitmap = bcBitmap(raw->bcTag());
            //return(toBS32(hpt));
	    return;
        }
        break;
    case TgcRawData::TYPE_SL:
        {
            //TGC_BYTESTREAM_SL sl;
            sl.roi = raw->roi();
            sl.overlap = raw->isOverlap();
            sl.threshold = raw->threshold();
            //sl.bcId = ???;
            sl.sign = raw->isMuplus();
            sl.cand = raw->index();
            sl.sector = raw->sector();
            sl.fwd = raw->isForward();
            sl.bcBitmap = bcBitmap(raw->bcTag());
            sl.cand2plus = raw->cand3plus();
            //return(toBS32(sl));
	    return;
        }
        break;
    default:
#ifndef LVL2_MUON_CALIBRATION_STANDALONE    
        log << MSG::ERROR << "Invalid type " << raw->typeName() << endreq;
#else
        std::cout << "Invalid type " << raw->typeName() << std::endl;
#endif

    }
}
