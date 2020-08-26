/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNERRORTOOL_ALIGNMENTERRORTOOL_H
#define MUONALIGNERRORTOOL_ALIGNMENTERRORTOOL_H

#include "TrkToolInterfaces/ITrkAlignmentDeviationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonAlignErrorBase/AlignmentDeviation.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCondAlg/MuonAlignmentErrorData.h" // for accessing info from the DB
#include "StoreGate/ReadCondHandleKey.h"

#include <boost/regex.hpp>
#include <iosfwd>
#include <string>

namespace Trk {
  class RIO_OnTrack;
}

namespace MuonAlign {

  class AlignmentErrorTool : public Trk::ITrkAlignmentDeviationTool, virtual public AthAlgTool {

    public:

      AlignmentErrorTool(const std::string&,const std::string&,const IInterface*);
      virtual ~AlignmentErrorTool()=default;
      virtual StatusCode initialize();
      virtual void makeAlignmentDeviations (const Trk::Track& track, std::vector<Trk::AlignmentDeviation*>& deviations) const;

    private:

      ToolHandle<MuonCalib::IIdToFixedIdTool> m_idTool{this,"idTool","MuonCalib::IdToFixedIdTool"};
      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

      // Struct for per-Station Deviations Information //
      struct deviationSummary_t {
	deviationSummary_t();
	~deviationSummary_t()=default;
        //static int i_instance; // TMP DEBUG

        // SYSTEMATIC UNCERTAINTIES
	double traslation;
	double rotation;
        // RULE
	boost::regex stationName;
	boost::regex multilayer;
        // SET OF HITS SATISFYING THE RULE
	std::vector<const Trk::RIO_OnTrack*> hits;
        // USEFUL NUMBERS
        Amg::Vector3D sumP;
        Amg::Vector3D sumU;
        Amg::Vector3D sumV;
        double sumW2;
      };

      // SOME USEFUL METHODS //
      // GET STATION EXACT NAME, FROM: https://svnweb.cern.ch/cern/wsvn/atlas-giraudpf/giraudpf/MuonSpectrometer/MuonAlignment/MuonAlignTrk/trunk/MuonAlignTrk/MuonFixedId.h
      std::string hardwareName(MuonCalib::MuonFixedId calibId) const;
      std::string side(MuonCalib::MuonFixedId calibId) const;
      std::string sectorString(MuonCalib::MuonFixedId calibId) const;
      int sector(MuonCalib::MuonFixedId calibId) const;
      int hardwareEta(MuonCalib::MuonFixedId calibId) const;
      bool isSmallSector(MuonCalib::MuonFixedId calibId) const;

      SG::ReadCondHandleKey<MuonAlignmentErrorData> m_readKey{this, "ReadKey", "MuonAlignmentErrorData", "Key of MuonAlignmentErrorData"};

  };
}


#endif

