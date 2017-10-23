/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_IMUONCALIBRATIONANDSMEARINGTOOL_H
#define MCAST_IMUONCALIBRATIONANDSMEARINGTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODMuon/Muon.h"

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicVariation.h"

namespace CP {

class IMuonCalibrationAndSmearingTool : public virtual asg::IAsgTool, virtual public CP::ISystematicsTool {

  //::: Declare the interface that the class provides
  ASG_TOOL_INTERFACE( CP::IMuonCalibrationAndSmearingTool )
public:
  struct InfoHelper{
    double ptms = 0;
    double ptid = 0;
    double ptcb = 0;
    double eta = 0;
    double phi = 0;
    double g0;
    double g1;
    double g2;
    double g3;
    double g4;
    int    charge = 1;
    int    detRegion = 0;
    int    scaleRegion = 0;
    std::vector < float >  cbParsA;
    std::vector < float >  cbCovMat;
    double weightMS = 0;
    double weightID = 0;
    double smearDeltaMS = 0;
    double smearDeltaID = 0;
    double smearDeltaCB = 0;
  };

public:
  //::: Apply the correction on a modifyable object
  virtual CorrectionCode applyCorrection( xAOD::Muon& mu ) const = 0;
  //::: Create a corrected copy from a constant muon
  virtual CorrectionCode correctedCopy( const xAOD::Muon& input, xAOD::Muon*& output ) const = 0;
  //::: Is the tool affected by a specific systematic?
  virtual bool isAffectedBySystematic( const SystematicVariation& systematic ) const = 0;
  //::: Which systematics have an effect on the tool's behaviour?
  virtual SystematicSet affectingSystematics() const = 0;
  //::: Systematics to be used for physics analysis
  virtual SystematicSet recommendedSystematics() const = 0;
  //::: Use specific systematic
  virtual SystematicCode applySystematicVariation ( const SystematicSet& systConfig ) = 0;
  //::: External setting of random seed
  //virtual void setRandomSeed( unsigned seed ) = 0;
  //::: expectedResolution
  virtual double expectedResolution( const std::string& DetType, xAOD::Muon& mu, const bool mc, InfoHelper& muonInfo ) const=0;

  virtual CorrectionCode applyStatCombination( const ElementLink< xAOD::TrackParticleContainer >& inDetTrackParticle,
                                               const ElementLink< xAOD::TrackParticleContainer >& extrTrackParticle ,
                                               int charge,
                                               AmgVector(5)& parsCB,
                                               AmgSymMatrix(5)& covCB,
                                               double& chi2) const = 0;
  virtual CorrectionCode applyStatCombination( xAOD::Muon& mu, InfoHelper& muonInfo ) const =0 ;

  //virtual void setUseStatCombination(bool flag) = 0;

  virtual  CorrectionCode applySagittaBiasCorrectionAuto(const int DetType, xAOD::Muon& mu, bool isMC, const unsigned int SytCase, InfoHelper& muonInfo) const=0;
  virtual  CorrectionCode CorrectForCharge(double p2, double& pt, int q, bool isMC) const=0;
  virtual  CorrectionCode applyiSagittaBiasCorrection(const unsigned int SgCorrType, xAOD::Muon& mu, unsigned int iter, bool stop, bool isMC, InfoHelper& muonInfo) const=0;


}; // class IMuonCalibrationAndSmearingTool

} // namespace CP

#endif // MCAST_IMUONCALIBRATIONANDSMEARINGTOOL_H
