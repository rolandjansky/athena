/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUFASTPATTERNFINDER_H
#define  TRIGL2MUONSA_MUFASTPATTERNFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "MdtCalibSvc/MdtCalibrationTool.h"
#include "MuonRoad.h"
#include "MdtData.h"
#include "TrackData.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonSA {

struct MdtLayerHits
{
  unsigned int ntot;
  unsigned int ntot_all;
  unsigned int ndigi;
  unsigned int ndigi_all;
  double ResSum;
  std::vector<unsigned int> indexes;
};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class MuFastPatternFinder: public AthAlgTool
{
   public:

      MuFastPatternFinder(const std::string& type, 
			   const std::string& name,
			   const IInterface*  parent);
    
      virtual StatusCode initialize() override;
    
   private:

      double calc_residual(double aw,double bw,double x,double y);
      void  doMdtCalibration(TrigL2MuonSA::MdtHitData& mdtHit, double track_phi, double phi0, bool isEndcap);

   public:

      StatusCode findPatterns(const TrigL2MuonSA::MuonRoad& muonRoad,
			      TrigL2MuonSA::MdtHits&        mdtHits,
			      std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns);

   private:
      // MDT calibration service
      ToolHandle<MdtCalibrationTool> m_mdtCalibrationTool{this, "CalibrationTool", "MdtCalibrationTool"};

      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

} // namespace TrigL2MuonSA

#endif  // MUFASTPATTERNFINDER_H
