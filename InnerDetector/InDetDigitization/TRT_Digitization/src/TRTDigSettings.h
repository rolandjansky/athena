/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
//                                                             //
// File: TRTDigSettings.h                                      //
//                                                             //
// Author: Thomas Kittelmann <kittel@nbi.dk>                   //
//                                                             //
// First version: April 2005. Rewritten November 2005.         //
//                                                             //
// Class containing parameters and settings used by            //
// TRT digitization.                                           //
//                                                             //
/////////////////////////////////////////////////////////////////

#ifndef TRT_DIGITIZATION_TRTDIGSETTINGS_H
#define TRT_DIGITIZATION_TRTDIGSETTINGS_H

#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

#include <map>
#include <string>
#include <vector>

class Algorithm;
class AlgTool;
namespace InDetDD { class TRT_DetectorManager; }

/**
 * Class containing parameters and settings used by TRT digitization.
 * @author Thomas Kittelmann
 */
class TRTDigSettings {

public:

  /** Parameters and settings for TRT digitization */
  TRTDigSettings();

  /**
   * Initialize: fill defaults and overwrite by user values where appropriate
   */
  void initialize( const InDetDD::TRT_DetectorManager* detmgr);

  StatusCode DigSettingsFromCondDB(int m_dig_vers_from_condDB);

  /** Add properties for overrideable parameters */
  void addPropertiesForOverrideableParameters( Algorithm * alg );

  /** Add properties for overrideable parameters  -  tool version */
  void addPropertiesForOverrideableParameters( AlgTool * algt );

  /** Printing of TRT digitization settings */
  void print( const std::string& front = "" ) const;

  /** Print TRT digitization settings overrideable parameters */
  void printFlagsForOverrideableParameters( const std::string& front = "" ) const;

  //==============================================================//
  //                       Accessor methods                       //
  //==============================================================//

  int digVers() const;

  bool useAttenuation() const;
  double attenuationLength() const;

  /** Get assumed Transition Radiation efficiency in barrel */
  double trEfficiencyBarrel(int strawGasType) const;

  /** Get assumed Transition Radiation efficiency in end caps */
  double trEfficiencyEndCapA(int strawGasType) const;
  double trEfficiencyEndCapB(int strawGasType) const;

  /** Get ionisation potential */
  double ionisationPotential(int strawGasType) const;

  /** Get smearing factor */
  double smearingFactor(int strawGasType) const;

  //--- Digitization parameters:

  unsigned int digVersion() const;
  bool useMagneticFieldMap() const;

  /** Get time interval covered by each digit */
  double timeInterval() const;
  /** Get number of time bins used for internal shaping of signal */
  unsigned int numberOfBins() const;
  /** Get number of internal time bins corresponding to one low threshold bit*/
  unsigned int numberOfBinsInEncodingBin() const;
  /** Get the minimum width of discriminator */
  double minDiscriminatorWidth() const;
  /** Get discriminator settling time */
  double discriminatorSettlingTime() const;
  /** Get discriminator dead time */
  double discriminatorDeadTime() const;

  /** Get discriminator setting for low threshold */
  double lowThresholdBar(int strawGasType) const;
  double lowThresholdEC(int strawGasType) const;

  /** Get discriminator setting for high threshold */
  double highThresholdBarShort(int strawGasType) const;
  double highThresholdBarLong(int strawGasType) const;
  double highThresholdECAwheels(int strawGasType) const;
  double highThresholdECBwheels(int strawGasType) const;

  /** T0 shift for HT */
  int htT0shiftBarShort()  const;
  int htT0shiftBarLong()   const;
  int htT0shiftECAwheels() const;
  int htT0shiftECBwheels() const;

  /** T0 for LT */
  int ltT0shiftBarShort(int strawGasType)  const;
  int ltT0shiftBarLong(int strawGasType)   const;
  int ltT0shiftECAwheels(int strawGasType) const;
  int ltT0shiftECBwheels(int strawGasType) const;

  //--- Common straw geometry parameters:

  /** Get inner radius of straw */
  double innerRadiusOfStraw() const;

  /** Get radius of signal wire */
  double outerRadiusOfWire() const;

  /** Get length of dead region at end of straws */
  double lengthOfDeadRegion() const;

  /** Query whether time is corrected for flight and wire propagation delays.
      This is false for beamType='cosmics' **/
  bool timeCorrection() const;

  /** Get wire signal propagation speed */
  double signalPropagationSpeed() const;

  //--- Calculating electronics time shifts:

  /** Get "distanceToTimeFactor" (fugde factor) */
  double distanceToTimeFactor() const;
  /** Get max vertex displacement */
  double maxVertexDisplacement() const;
  /** Get x value of point where particles are assumed to originate */
  double timeOffsetCalcVertexX() const;
  /** Get y value of point where particles are assumed to originate */
  double timeOffsetCalcVertexY() const;
  /** Get x value of point where particles are assumed to originate */
  double timeOffsetCalcVertexZ() const;
  /** Query whether electronics is assumed to be at far end of straw */
  bool electronicsAreAtFarEnd() const;
  /** Query whether time offsets are assumed to be symmetric in phi */
  bool timeshiftsSymmetricForPhiSectors() const;

  //---  Pile up:

  /** Get number of simulated bunch crossings before main event (pile up) */
  unsigned int numberOfCrossingsBeforeMain() const;
  /** Get minimum Ekin for pileup track to enter MC-truth */
  double pileUpSDOsMinEkin() const;//MC Truth saving for pile-up events

  //--- Noise parameters:

  /** Query whether simulation of noise in unhit straws */
  bool noiseInUnhitStraws() const;
  /** Query whether simulation of noise in hit straws */
  bool noiseInSimhits() const;

  //--- Composition of electronics noise:

  /** Get fast electronics noise pulse distance (time) */
  double fastElectronicsNoisePulseDistance() const;
  /** Get slow periodic noise pulse distance (time) */
  double slowPeriodicNoisePulseDistance() const;
  /** Get slow periodic noise pulse fraction */
  double slowPeriodicNoisePulseFraction() const;

  /** Get average noise level */
  double averageNoiseLevel() const;

  /** Get average cross talk noise level */
  double crossTalkNoiseLevel() const;
  double crossTalkNoiseLevelOtherEnd() const;

  //--- Threshold fluctuations:

  /** Get relative low threshold fluctuations (evt to evt & straw to straw) */
  double relativeLowThresholdFluctuation() const;  // 0 means no LT fluctuation
  /** Get relative high threshold fluctuations (evt to evt & straw to straw)*/
  double relativeHighThresholdFluctuation() const; // 0 means no HT fluctuation

  //--- Field (for effects on drift time):

  /** Get solenoid field strength */
  double solenoidFieldStrength() const;

  //--- Special runs:

  /** Get flag used for cosmics runs */
  unsigned int cosmicFlag() const;
  /** Query whether CTB (Combined Test Beam 2004) flag is set */
  bool isCTB() const;
  /** Get flag used for overlay */
  bool isOverlay() const;

  bool killEndCap() const;          /**< Is end cap killed */
  bool killBarrel() const;          /**< Is barrel killed */
  /** In cosmics, events not correlated with LHC clock */
  double jitterTimeOffset() const;
  /** Cosmics timing corresponding to pit setup? */
  bool doCosmicTimingPit() const;

  /** Switch on crosstalk noise based on Cosmic 08 measurements */
  bool doCrosstalk() const;

  unsigned int storeSDO() const;

  bool getT0FromData() const;

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

  MsgStream& msg() const { return m_msg.get(); }
  void display (const std::string& msg, int lvl = (int)MSG::INFO) const;
  void info    (const std::string& msg) const { display (msg, (int)MSG::INFO); }
  void debug   (const std::string& msg) const { display (msg, (int)MSG::DEBUG); }
  void warning (const std::string& msg) const { display (msg, (int)MSG::WARNING); }
  void setLvl  (int lvl = (int)MSG::INFO);
  void setLvl  (const std::string& lvl);

  //Delta-ray suppression switch
  double TrtRangeCutProperty() const;

private:
  //==============================================================//
  //                           Private                            //
  //==============================================================//

  /** Define settable variables */
  void defineVariables();

  /** Fill default values into settable variables */
  void fillDefaults(const InDetDD::TRT_DetectorManager* detmgr);

  /** Overwrite default values by uses values */
  void processOverrides();

  //---First a list of data members used for holding the parameters:
  unsigned int m_digversion;
  bool m_useMagneticFieldMap = false;
  bool m_getT0FromData = false;
  unsigned int m_storeSDO = 0U;            /**< Time window in which to store SDOs*/

  double m_ionisationPotential = 0.0;       /**< Mean ionisation potential */
  double m_ionisationPotentialArgon = 0.0;  /**< Mean ionisation potential for Argon mixture */
  double m_ionisationPotentialKrypton = 0.0;  /**< Mean ionisation potential for Krypton mixture */
  double m_smearingFactor = 0.0;            /**< Cluster energy smearing factor */
  double m_smearingFactorArgon = 0.0;       /**< Cluster energy smearing factor for Argon mixture */
  double m_smearingFactorKrypton = 0.0;       /**< Cluster energy smearing factor for Krypton mixture */

  double m_timeInterval = 0.0;             /**< Time interval covered by each digit*/
  unsigned int m_numberOfBins = 0U;        /**< No. bins used internally */
  unsigned int m_numberOfBinsInEncodingBin = 0U;  /**< No. internal bins for each
                                              *   readout bit */
  double m_minDiscriminatorWidth = 0.0;     /**< Min. discriminator width */
  double m_discriminatorSettlingTime = 0.0; /**< Discriminator settling time */
  double m_discriminatorDeadTime = 0.0;     /**< Discriminator dead time */
  double m_signalPropagationSpeed = 0.0;/**< Signal propagation time in signal wire*/
  double m_distanceToTimeFactor = 0.0; /**< Fudge factor: time to distance */

  double m_lowThresholdBar = 0.0;         /**< Low threshold discriminator setting */
  double m_lowThresholdEC = 0.0;         /**< Low threshold discriminator setting */

  double m_lowThresholdBarArgon = 0.0;         /**< Low threshold discriminator setting for Argon mixture */
  double m_lowThresholdECArgon = 0.0;         /**< Low threshold discriminator setting for Argon mixture */

  double m_lowThresholdBarKrypton = 0.0;         /**< Low threshold discriminator setting for Krypton mixture */
  double m_lowThresholdECKrypton = 0.0;         /**< Low threshold discriminator setting for Krypton mixture */

  double m_highThresholdBarShort = 0.0;        /**< High threshold discriminator setting */
  double m_highThresholdBarLong = 0.0;        /**< High threshold discriminator setting */
  double m_highThresholdECAwheels = 0.0;        /**< High threshold discriminator setting */
  double m_highThresholdECBwheels = 0.0;        /**< High threshold discriminator setting */

  double m_highThresholdBarShortArgon = 0.0;        /**< High threshold discriminator setting Argon */
  double m_highThresholdBarLongArgon = 0.0;        /**< High threshold discriminator setting Argon */
  double m_highThresholdECAwheelsArgon = 0.0;        /**< High threshold discriminator setting Argon */
  double m_highThresholdECBwheelsArgon = 0.0;        /**< High threshold discriminator setting Argon */

  double m_highThresholdBarShortKrypton = 0.0;        /**< High threshold discriminator setting Krypton */
  double m_highThresholdBarLongKrypton = 0.0;        /**< High threshold discriminator setting Krypton */
  double m_highThresholdECAwheelsKrypton = 0.0;        /**< High threshold discriminator setting Krypton */
  double m_highThresholdECBwheelsKrypton = 0.0;        /**< High threshold discriminator setting Krypton */

  int m_htT0shiftBarShort = 0; /** HT T0 shift */
  int m_htT0shiftBarLong = 0;
  int m_htT0shiftECAwheels = 0;
  int m_htT0shiftECBwheels = 0;

  int m_ltT0shiftBarShortXe = 0; /** LT T0 shift */
  int m_ltT0shiftBarShortKr = 0;
  int m_ltT0shiftBarShortAr = 0;
  int m_ltT0shiftBarLongXe = 0;
  int m_ltT0shiftBarLongKr = 0;
  int m_ltT0shiftBarLongAr = 0;
  int m_ltT0shiftECAwheelsXe = 0;
  int m_ltT0shiftECAwheelsKr = 0;
  int m_ltT0shiftECAwheelsAr = 0;
  int m_ltT0shiftECBwheelsXe = 0;
  int m_ltT0shiftECBwheelsKr = 0;
  int m_ltT0shiftECBwheelsAr = 0;

  double m_innerRadiusOfStraw = 0.0;   /**< Inner radius of straw */
  double m_outerRadiusOfWire = 0.0;    /**< Radius of drift wire */
  double m_lengthOfDeadRegion = 0.0;   /**< Length of dead region at straw end */

  bool m_timeCorrection = false; /**< Corrrect for flight and wire propagation delays*/

  unsigned int m_numberOfCrossingsBeforeMain = 0U; /**< For pileup: BCs before main
                                               *   event */
  double m_pileUpSDOsMinEkin = 0.0;      /**< Min. Ekin for pileup particle to appear in MC-truth */
  double m_maxVertexDisplacement = 0.0;  /**< Max. vertex displacement */
  double m_timeOffsetCalcVertexX = 0.0;  /**< x-coord. of point where particle is assumed to originate */
  double m_timeOffsetCalcVertexY = 0.0;  /**< y-coord. of point where particle is assumed to originate */
  double m_timeOffsetCalcVertexZ = 0.0;  /**< z-coord. of point where particle is assumed to originate */
  bool m_electronicsAreAtFarEnd = false;   /**< Electronics assumed at far end of straw */
  bool m_timeshiftsSymmetricForPhiSectors = false;  /**< Time offset assumed phi symmetric */

  double m_trEfficiencyBarrel = 0.0; /**< Transition Radiation efficiency, barrel */
  double m_trEfficiencyEndCapA = 0.0; /**< Transition Radiation efficiency, end cap A*/
  double m_trEfficiencyEndCapB = 0.0; /**< Transition Radiation efficiency, end cap B*/
  double m_trEfficiencyBarrelArgon = 0.0; /**< Transition Radiation efficiency, barrel, Argon */
  double m_trEfficiencyEndCapAArgon = 0.0; /**< Transition Radiation efficiency, end cap A, Argon */
  double m_trEfficiencyEndCapBArgon = 0.0; /**< Transition Radiation efficiency, end cap B, Argon */
  double m_trEfficiencyBarrelKrypton = 0.0; /**< Transition Radiation efficiency, barrel, Krypton */
  double m_trEfficiencyEndCapAKrypton = 0.0; /**< Transition Radiation efficiency, end cap A, Krypton */
  double m_trEfficiencyEndCapBKrypton = 0.0; /**< Transition Radiation efficiency, end cap B, Krypton */

  bool m_noiseInUnhitStraws = false;   /**< Simulate noise in unhit straws */
  bool m_noiseInSimhits = false;       /**< Simulate noise in hit straws */

  bool m_isCTB = false;                /**< Data is from CTB (Combined Test Beam) */

  bool m_killEndCap = false;           /**< Disable end cap */
  bool m_killBarrel = false;           /**< Disable barrel */
  double m_jitterTimeOffset = 0.0;   /**< Event time not correlated with LHC clock */
  bool m_doCosmicTimingPit = false;    /**< Cosmics pit setup */
  bool m_doCrosstalk = false;          /**< Crosstalk noise */

  unsigned int m_cosmicFlag = 0U;   /**< Data is cosmics */
  bool m_isOverlay = false;   /**< Doing overlay */

  double m_fastElectronicsNoisePulseDistance = 0.0; /**< Fast electronics noise pulse distance (time) */
  double m_slowPeriodicNoisePulseDistance = 0.0;    /**< Slow periodic noise pulse distance (time) */
  double m_slowPeriodicNoisePulseFraction = 0.0;    /**< Slow periodic noise pulse fraction */
  double m_averageNoiseLevel = 0.0;                 /**< Average noise level */
  double m_crosstalkNoiseLevel = 0.0;               /**< Average cross talk noise level */
  double m_crosstalkNoiseLevelOtherEnd = 0.0;       /**< Average cross talk noise level - other end */

  double m_relativeLowThresholdFluctuation = 0.0;   /**< Relative low threshold fluctuation */
  double m_relativeHighThresholdFluctuation = 0.0;  /**< Relative high threshold fluctuation */
  double m_solenoidFieldStrength = 0.0;             /**< Solenoid field strength */

  bool m_useAttenuation = false;
  double m_attenuationLength = 0.0;

  double m_trtRangeCutProperty = 0.0; /**< Electrons xenon range cut in TRT simulation  */

  //Now follows infrastructure for handling all the parameters in a robust way:

  /** Define new double variable */
  void defineNewVariable(const std::string & name,
                         double * datamember,
                         const std::string & description,
                         const std::string & unitname,
                         double unitval,
                         double lowrange,
                         double highrange);

  /** Define new unsigned int variable */
  void defineNewUIntVariable(const std::string & name,
                             unsigned int * datamember,
                             const std::string & description,
                             unsigned int lowrange,
                             unsigned int highrange);

  /** Define new int variable */
  void defineNewIntVariable(const std::string & name,
                            int * datamember,
                            const std::string & description,
                            int lowrange,
                            int highrange);

  /** Define new bool variable */
  void defineNewBoolVariable(const std::string & name,
                             bool * datamember,
                             const std::string & description);

  /** Settable parameter, double */
  struct doubleparameter {
    std::string description;
    double * directvaraddress{};
    std::string unitname;  
    double unit{};
    double okrange_low{}; 
    double okrange_high{};
    double valueSetByUser{};
  };

  /** Settable parameter, int and bool */
  struct intboolparameter {
    std::string description;
    //Two of these are put to null to indicate type:
    int * directvaraddress_int{};
    unsigned int * directvaraddress_uint{};
    bool * directvaraddress_bool{};
    int okrange_low{}; int okrange_high{};
    int valueSetByUser{};
  };

  /** Map of settable parameters (double) */
  std::map <std::string,doubleparameter>  m_doubleparMap;
  /** Map of settable parameters (int & bool) */
  std::map <std::string,intboolparameter> m_intboolparMap;

  double m_propertyNotSetMagicNumber{};
  int m_propertyNotSetMagicNumber_int{};

  mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;

};

#include "TRTDigSettings.icc"

#endif
