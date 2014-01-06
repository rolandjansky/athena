/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonTrackRecordValAlg_H
#define MuonTrackRecordValAlg_H

#include "GaudiKernel/Algorithm.h"

#include "GaudiKernel/MsgStream.h"

#include "MuonHitTest/ValAlgInterface.h"

#include <string>

template <class ConcreteAlgorithm> class AlgFactory;

class MuonTrackRecordValAlg: public Algorithm, private ValAlgInterface {

friend class AlgFactory<MuonTrackRecordValAlg>;

public:
	MuonTrackRecordValAlg(const std::string& name, ISvcLocator* pSvcLocator);
	~MuonTrackRecordValAlg();

	StatusCode initialize();
        StatusCode finalize();
        StatusCode execute();

private:
//std::string m_recordKey;    // SG key for the Trk Rec collection
bool m_OnlyMuon;            // Store only muons
bool m_dumpTrackRecord;	
//
     // member data
     //
     StoreGateSvc*  m_sgSvc;
	MsgStream log;
	double m_cutOff;
        int    PDGCode ;
        double PosX    ;
        double PosY    ;
        double PosZ    ;
        double PX      ;
        double PY      ;
        double PZ      ;      
        double Pt      ;
	
     // properties


};

#endif
