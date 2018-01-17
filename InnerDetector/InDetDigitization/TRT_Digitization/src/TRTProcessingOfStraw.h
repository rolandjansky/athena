/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTPROCESSINGOFSTRAW_H
#define TRTPROCESSINGOFSTRAW_H

//Hit classes
#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHit.h"
//Particle Table
#include "HepPDT/ParticleDataTable.hh"

#include <vector>

#include "TRTElectronicsProcessing.h"

// #include "CommissionEvent/ComTime.h"

#include "GaudiKernel/ServiceHandle.h"
#include "CLHEP/Random/RandomEngine.h"
//#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "AthenaKernel/MsgStreamMember.h"

#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"

class TRTDigit;
class TRTTimeCorrection;
class TRTNoise;
class TRTDigCondBase;

class TRTUncompressedHit;
class IAtRndmGenSvc;
class ITRT_PAITool;
class ITRT_SimDriftTimeTool;

namespace MagField { class IMagFieldSvc; }

namespace InDetDD { class TRT_DetectorManager; }

class TRTDigSettings;

/**
 * TRT Digitization: Processing of a TRT Straws. @n
 * The main controlling function
 * in @c ProcessStraw(). See detailed description of this.
 */
class TRTProcessingOfStraw {
public:
  /** Constructor: Calls Initialize method */
  TRTProcessingOfStraw( const TRTDigSettings*,
			const InDetDD::TRT_DetectorManager*,
			ITRT_PAITool*,
			ITRT_SimDriftTimeTool*,
			ServiceHandle <IAtRndmGenSvc> atRndmGenSvc,
			TRTElectronicsProcessing * ep,
			TRTNoise * noise,
			TRTDigCondBase* digcond,
			const HepPDT::ParticleDataTable*,
			const TRT_ID*,
		        ITRT_PAITool* = NULL,
		        ITRT_PAITool* = NULL);
  /** Destructor */
  ~TRTProcessingOfStraw();

  typedef TimedHitCollection<TRTUncompressedHit>::const_iterator
    hitCollConstIter;

  /**
   * Process this straw all the way from Geant4 @e hit to output @e digit.@n
   * Steps:
   * -# Loop over the simhits in this straw
   *    and produce a list of primary ionisation clusters.
   * -# Use the cluster list along with
   *    gas and wire properties, to create a list of energy
   *    deposits (i.e. potential fluctuations) reaching the
   *    frontend electronics.
   * -# Simulate how the FE
   *    turns the results into an output digit. This
   *    includes the shaping/amplification and subsequent
   *    discrimination as well as addition of noise.
   *
   * @param i:        Geant4 hit collection iterator
   * @param e:        last hit in collection
   * @param outdigit: The 27 bit digit
   * (bits: 8 low + 1 high + 8 low + 1 high + 8 low + 1 high)
   */
  void ProcessStraw (hitCollConstIter i,
		     hitCollConstIter e,
		     TRTDigit& outdigit,
		     bool & m_alreadyPrintedPDGcodeWarning,
		     double m_cosmicEventPhase, //const ComTime* m_ComTime,
                     int strawGasType,
		     bool emulationArflag,
		     bool emulationKrflag );

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLvl (MSG::Level lvl) { return m_msg.get().level() <= lvl; }

private:

  //NB copy-constructor and assignment operator declared, but not defined.
  TRTProcessingOfStraw(const TRTProcessingOfStraw&);
  TRTProcessingOfStraw& operator= (const TRTProcessingOfStraw&);

  /** Initialize */
  void Initialize(ServiceHandle <IAtRndmGenSvc> atRndmGenSvc) ;

  const TRTDigSettings* m_settings;
  const InDetDD::TRT_DetectorManager* m_detmgr;
  ITRT_PAITool* m_pPAItoolXe;
  ITRT_PAITool* m_pPAItoolAr;
  ITRT_PAITool* m_pPAItoolKr;
  ITRT_SimDriftTimeTool* m_pSimDriftTimeTool;

  /** TR efficiency: this fraction of TR photons will be kept */
  double m_trEfficiencyBarrel;
  double m_trEfficiencyEndCapA;
  double m_trEfficiencyEndCapB;

  /** Time to be corrected for flight and wire propagation delays false when beamType='cosmics' */
  bool m_timeCorrection;

  double m_ionisationPotential;
  double m_signalPropagationSpeed;
  double m_attenuationLength;

  bool m_useAttenuation;
  bool m_useMagneticFieldMap;

  double m_smearingFactor;
  double m_maxCrossingTime;
  double m_minCrossingTime;
  double m_shiftOfZeroPoint;
  // double m_time_y_eq_zero;

  double m_innerRadiusOfStraw;
  double m_outerRadiusOfWire;
  double m_outerRadiusEndcap;
  double m_innerRadiusEndcap;
  double m_strawLengthBarrel;

  double m_solenoidFieldStrength;

  // const ComTime* m_ComTime;

  TRTTimeCorrection*        m_pTimeCorrection;
  TRTElectronicsProcessing* m_pElectronicsProcessing;
  TRTNoise*                 m_pNoise;
  TRTDigCondBase*           m_pDigConditions;

  const HepPDT::ParticleDataTable* m_pParticleTable;

  /** Primary ionisation cluster */
  class cluster {
    public:
      cluster(double e, double t, double x, double y, double z) :
        energy(e), time(t), xpos(x), ypos(y), zpos(z) {};
      double energy;
      double time;
      double xpos;
      double ypos;
      double zpos;
  };

  std::vector<cluster> m_clusterlist;
  std::vector<TRTElectronicsProcessing::Deposit> m_depositList;

  /**
   * This is the main function for re-simulation of the ionisation
   * in the active gas via the
   * PAI model. From the given G4 step, ionisation clusters are distributed
   * randomly (mean free path given by @c TRT_PAI_Process::GetMeanFreePath())
   * along path.
   * Cluster energies are given by @c TRT_PAI_Process::GetEnergyTransfer().
   * @param scaledKineticEnergy: The kinetic energy a proton would have had
   *                             if it had the same Lorentz gamma factor as
   *                             the particle in question.
   * @param particleCharge:      Particle charge
   * @param timeOfHit:           Time of hit
   * @param prex:                PreStepPoint @a x coordinate
   * @param prey:                PreStepPoint @a y coordinate
   * @param prez:                PreStepPoint @a z coordinate
   * @param postx:               PostStepPoint @a x coordinate
   * @param posty:               PostStepPoint @a y coordinate
   * @param postz:               PostStepPoint @a z coordinate
   * @param clusterlist:         List of ionisation clusters along step
   */
  void addClustersFromStep ( const double& scaledKineticEnergy,
			     const double& particleCharge,
			     const double& timeOfHit,
			     const double& prex,
			     const double& prey,
			     const double& prez,
			     const double& postx,
			     const double& posty,
			     const double& postz,
			     std::vector<cluster>& clusterlist,
			     int strawGasType);
  /**
   * Transform the ioniation clusters along the particle trajectory inside a
   * straw to energy deposits (i.e. potential fluctuations) reaching the
   * front-end electronics.
   * Effects taken into effect are:
   * - number of primary electrons from deposited energy
   * - stocastic recapture of drift electrons
   * - electron drift in the magnetic field (different direction
   *   end-cap/barrel). Optionally the drift time includes spread.
   * - Optionally signal wire propagation dealys are included.
   *   Each drift electron then gives rise to two signal: direct and reflected.
   *
   * @param hitID: Id of straw
   * @param clusters: ionisation clusters along particle trajectory
   * @param deposits: energy deposits on wire
   */
  void ClustersToDeposits (const int& hitID,
			   const std::vector<cluster>& clusters,
			   std::vector<TRTElectronicsProcessing::Deposit>& deposits,
			   Amg::Vector3D TRThitGlobalPos,
                           double m_cosmicEventPhase, // const ComTime* m_ComTime
                           int strawGasType);

  std::vector<double> m_drifttimes;     // electron drift times
  std::vector<double> m_expattenuation; // tabulation of exp()
  unsigned int  m_maxelectrons;         // maximum number of them (minmum is 100 for the Gaussian approx to be ok);

  CLHEP::HepRandomEngine * m_pHRengine;

  bool m_alreadywarnedagainstpdg0;

  Amg::Vector3D getGlobalPosition( int hitID, const TimedHitPtr<TRTUncompressedHit> *theHit );

  unsigned int getRegion(int hitID);

  //Magnetic field stuff
  ServiceHandle < MagField::IMagFieldSvc > m_magneticfieldsvc;
  mutable Athena::MsgStreamMember m_msg;

protected:
  const TRT_ID* m_id_helper;

};

#endif
