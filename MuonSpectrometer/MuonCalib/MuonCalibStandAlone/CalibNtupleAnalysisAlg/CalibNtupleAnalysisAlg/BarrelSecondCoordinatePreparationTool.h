/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 23.01.2008, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_BarrelSecondCoordinatePreparationToolH
#define MuonCalib_BarrelSecondCoordinatePreparationToolH

//:::::::::::::::::::::::::::::::::::::::::::::::::
//:: CLASS BarrelSecondCoordinatePreparationTool ::
//:::::::::::::::::::::::::::::::::::::::::::::::::

/// \class BarrelSecondCoordinatePreparationTool
///
/// This class can be used to recalculate the second coordinates of MDT hits
/// using raw RPC hits. It is limited to the application to barrel MDT chambers.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 23.01.2009

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// MuonCalib //
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

// AtlasCore //
// AtlasConditions //

//CLHEP
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

namespace MuoinGM {

class MuonDetectorManager;

}

namespace MuonCalib {

class IIdToFixedIdTool;
class MuonCalibRawRpcHit;
class BarrelSecondCoordinatePreparationTool : public AthAlgTool, virtual public CalibSegmentPreparationTool {

 public:
// Constructor //
  BarrelSecondCoordinatePreparationTool(const std::string &t,
    const std::string &n, const IInterface *p); 

// Destructor //
  ~BarrelSecondCoordinatePreparationTool(void); 

// Methods //
// methods required by the base classes //
  StatusCode initialize(void);
  ///< initialization of the tools
  StatusCode finalize(void);
                            ///< finalization of the tool
  void prepareSegments( const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> &segments);
  ///< Method to calculate the second coordinate of 
  ///< MDT hits in the barrel based on RPC hits.
  ///< \param event Current event (contains raw hits
  ///<              and other useful objects).
  ///> \param segments Vector of segments to be
  ///>                 prepared.

  bool handleRPChits(MuonCalibSegment &MDT_segment, std::vector<MuonCalibRawRpcHit *> &raw_hits);

  int rpcFit(std::vector<CLHEP::HepVector> &RPC_hits, std::vector<int> in_sect, 
    std::vector<int> num_same, double max_r, CLHEP::HepVector &tr_par, double &angle_err);

 private:
  const MuonGM::MuonDetectorManager *m_detMgr; // pointer to the muon detector manager
  const MuonCalib::IIdToFixedIdTool *m_id_tool; // identifier converter

  FILE* m_rpcHitsFile; //File to store RPC entries
  bool write_rpc_hits;

//    RPCTrackAssociator m_RPC_track_associator; // tool for the determination of
                                                 // the second coordinates of MDT hits

};

}  //namespace MuonCalib  
#endif
