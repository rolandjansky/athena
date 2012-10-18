/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHALGS_TRACKRECORDFILTER_H
#define MCTRUTHALGS_TRACKRECORDFILTER_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"

// particle table
#include "HepPDT/ParticleDataTable.hh"


#include <string>

class StoreGateSvc;
class TrackRecordFilter : public Algorithm {


public:
        TrackRecordFilter (const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~TrackRecordFilter(){};

        StatusCode initialize();
        StatusCode finalize();
        StatusCode execute();

private:
	std::string m_inputName;
	std::string m_outputName;
        double m_cutOff;
	StoreGateSvc* m_storeGate;
        const HepPDT::ParticleDataTable* m_pParticleTable;
};

#endif

