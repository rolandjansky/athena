/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBIDENTIFIER_FixedIdDecoder_H
#define  MUONCALIBIDENTIFIER_FixedIdDecoder_H

#include "string"

/* Usage:
root CalibrationNtuple.root
root [1] TPython::Exec("import PyCintex")
root [2] PatternNtupleMaker.cd()
root [3] Segments.Draw("MuonCalib::FixedIdDecoder::mdtTube(mdt_id)")
*/

namespace MuonCalib {


class FixedIdDecoder {
public: 


//station
 static int eta(int id);
 static int phi(int id);
 static int stationName(int id);
 static std::string stationNameStr(int id);

//mdt

 static int mdtMultilayer(int id);
 static int mdtTubeLayer(int id);
 static int mdtTube(int id);
 static int mdtMezzanine(int id);
 
//csc

 static int cscChamberLayer(int id);
 static int cscWireLayer(int id);
 static int cscMeasuresPhi(int id);
 static int cscStrip(int id);

//rpc

 static int rpcDoubletR(int id);
 static int rpcDoubletZ(int id);
 static int rpcDoubletPhi(int id);
 static int rpcGasGap(int id);
 static int rpcMeasuresPhi(int id);
 static int rpcStrip(int id);

//tgc

 static int tgcGasGap(int id);
 static int tgcIsStrip(int id);
 static int tgcChannel(int id);

};

}

#endif
