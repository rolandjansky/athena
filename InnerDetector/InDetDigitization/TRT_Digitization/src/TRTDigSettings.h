/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#ifndef TRTDIGSETTINGS_H
#define TRTDIGSETTINGS_H

#include <vector>
#include <map>
#include <string>

#include "AthenaKernel/MsgStreamMember.h"

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
  double strawLengthBarrel() const;
  double deadRegionLengthBarrel() const;
  double innerRadiusEndcap() const;
  double outerRadiusEndcap() const;

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
  bool m_useMagneticFieldMap;
  bool m_getT0FromData;
  unsigned int m_storeSDO;            /**< Time window in which to store SDOs*/

  double m_ionisationPotential;       /**< Mean ionisation potential */
  double m_ionisationPotentialArgon;  /**< Mean ionisation potential for Argon mixture */
  double m_ionisationPotentialKrypton;  /**< Mean ionisation potential for Krypton mixture */
  double m_smearingFactor;            /**< Cluster energy smearing factor */
  double m_smearingFactorArgon;       /**< Cluster energy smearing factor for Argon mixture */
  double m_smearingFactorKrypton;       /**< Cluster energy smearing factor for Krypton mixture */

  double m_timeInterval;             /**< Time interval covered by each digit*/
  unsigned int m_numberOfBins;        /**< No. bins used internally */
  unsigned int m_numberOfBinsInEncodingBin;  /**< No. internal bins for each
					      *   readout bit */
  double m_minDiscriminatorWidth;     /**< Min. discriminator width */
  double m_discriminatorSettlingTime; /**< Discriminator settling time */
  double m_discriminatorDeadTime;     /**< Discriminator dead time */
  double m_signalPropagationSpeed;/**< Signal propagation time in signal wire*/
  double m_distanceToTimeFactor; /**< Fudge factor: time to distance */

  double m_lowThresholdBar;         /**< Low threshold discriminator setting */
  double m_lowThresholdEC;         /**< Low threshold discriminator setting */

  double m_lowThresholdBarArgon;         /**< Low threshold discriminator setting for Argon mixture */
  double m_lowThresholdECArgon;         /**< Low threshold discriminator setting for Argon mixture */

  double m_lowThresholdBarKrypton;         /**< Low threshold discriminator setting for Krypton mixture */
  double m_lowThresholdECKrypton;         /**< Low threshold discriminator setting for Krypton mixture */

  double m_highThresholdBarShort;        /**< High threshold discriminator setting */
  double m_highThresholdBarLong;        /**< High threshold discriminator setting */
  double m_highThresholdECAwheels;        /**< High threshold discriminator setting */
  double m_highThresholdECBwheels;        /**< High threshold discriminator setting */

  double m_highThresholdBarShortArgon;        /**< High threshold discriminator setting Argon */
  double m_highThresholdBarLongArgon;        /**< High threshold discriminator setting Argon */
  double m_highThresholdECAwheelsArgon;        /**< High threshold discriminator setting Argon */
  double m_highThresholdECBwheelsArgon;        /**< High threshold discriminator setting Argon */

  double m_highThresholdBarShortKrypton;        /**< High threshold discriminator setting Krypton */
  double m_highThresholdBarLongKrypton;        /**< High threshold discriminator setting Krypton */
  double m_highThresholdECAwheelsKrypton;        /**< High threshold discriminator setting Krypton */
  double m_highThresholdECBwheelsKrypton;        /**< High threshold discriminator setting Krypton */

  int m_htT0shiftBarShort; /** HT T0 shift */
  int m_htT0shiftBarLong;
  int m_htT0shiftECAwheels;
  int m_htT0shiftECBwheels;

  int m_ltT0shiftBarShortXe; /** LT T0 shift */
  int m_ltT0shiftBarShortKr;
  int m_ltT0shiftBarShortAr;
  int m_ltT0shiftBarLongXe;
  int m_ltT0shiftBarLongKr;
  int m_ltT0shiftBarLongAr;
  int m_ltT0shiftECAwheelsXe;
  int m_ltT0shiftECAwheelsKr;
  int m_ltT0shiftECAwheelsAr;
  int m_ltT0shiftECBwheelsXe;
  int m_ltT0shiftECBwheelsKr;
  int m_ltT0shiftECBwheelsAr;

  double m_innerRadiusOfStraw;   /**< Inner radius of straw */
  double m_outerRadiusOfWire;    /**< Radius of drift wire */
  double m_lengthOfDeadRegion;   /**< Length of dead region at straw end */

  bool m_timeCorrection; /**< Corrrect for flight and wire propagation delays*/

  unsigned int m_numberOfCrossingsBeforeMain; /**< For pileup: BCs before main
					       *   event */
  double m_pileUpSDOsMinEkin;      /**< Min. Ekin for pileup particle to appear in MC-truth */
  double m_maxVertexDisplacement;  /**< Max. vertex displacement */
  double m_timeOffsetCalcVertexX;  /**< x-coord. of point where particle is assumed to originate */
  double m_timeOffsetCalcVertexY;  /**< y-coord. of point where particle is assumed to originate */
  double m_timeOffsetCalcVertexZ;  /**< z-coord. of point where particle is assumed to originate */
  bool m_electronicsAreAtFarEnd;   /**< Electronics assumed at far end of straw */
  bool m_timeshiftsSymmetricForPhiSectors;  /**< Time offset assumed phi symmetric */

  double m_trEfficiencyBarrel; /**< Transition Radiation efficiency, barrel */
  double m_trEfficiencyEndCapA; /**< Transition Radiation efficiency, end cap A*/
  double m_trEfficiencyEndCapB; /**< Transition Radiation efficiency, end cap B*/
  double m_trEfficiencyBarrelArgon; /**< Transition Radiation efficiency, barrel, Argon */
  double m_trEfficiencyEndCapAArgon; /**< Transition Radiation efficiency, end cap A, Argon */
  double m_trEfficiencyEndCapBArgon; /**< Transition Radiation efficiency, end cap B, Argon */
  double m_trEfficiencyBarrelKrypton; /**< Transition Radiation efficiency, barrel, Krypton */
  double m_trEfficiencyEndCapAKrypton; /**< Transition Radiation efficiency, end cap A, Krypton */
  double m_trEfficiencyEndCapBKrypton; /**< Transition Radiation efficiency, end cap B, Krypton */

  bool m_noiseInUnhitStraws;   /**< Simulate noise in unhit straws */
  bool m_noiseInSimhits;       /**< Simulate noise in hit straws */

  bool m_isCTB;                /**< Data is from CTB (Combined Test Beam) */

  bool m_killEndCap;           /**< Disable end cap */
  bool m_killBarrel;           /**< Disable barrel */
  double m_jitterTimeOffset;   /**< Event time not correlated with LHC clock */
  bool m_doCosmicTimingPit;    /**< Cosmics pit setup */
  bool m_doCrosstalk;          /**< Crosstalk noise */

  unsigned int m_cosmicFlag;   /**< Data is cosmics */
  bool m_isOverlay;   /**< Doing overlay */

  double m_fastElectronicsNoisePulseDistance; /**< Fast electronics noise pulse distance (time) */
  double m_slowPeriodicNoisePulseDistance;    /**< Slow periodic noise pulse distance (time) */
  double m_slowPeriodicNoisePulseFraction;    /**< Slow periodic noise pulse fraction */
  double m_averageNoiseLevel;                 /**< Average noise level */
  double m_crosstalkNoiseLevel;               /**< Average cross talk noise level */
  double m_crosstalkNoiseLevelOtherEnd;       /**< Average cross talk noise level - other end */

  double m_relativeLowThresholdFluctuation;   /**< Relative low threshold fluctuation */
  double m_relativeHighThresholdFluctuation;  /**< Relative high threshold fluctuation */
  double m_solenoidFieldStrength;             /**< Solenoid field strength */

  double m_strawLengthBarrel;
  //  double m_deadRegionLengthBarrel;
  double m_innerRadiusEndcap;
  double m_outerRadiusEndcap;

  bool m_useAttenuation;
  double m_attenuationLength;

  double m_trtRangeCutProperty; /**< Electrons xenon range cut in TRT simulation  */

  //Now follows infrastructure for handling all the parameters in a robust way:

  /** Define new double variable */
  void defineNewVariable(std::string name,
			 double * datamember,
			 std::string description,
			 std::string unitname,
			 double unitval,
			 double lowrange,
			 double highrange);

  /** Define new unsigned int variable */
  void defineNewUIntVariable(std::string name,
			     unsigned int * datamember,
			     std::string description,
			     unsigned int lowrange,
			     unsigned int highrange);

  /** Define new int variable */
  void defineNewIntVariable(std::string name,
			    int * datamember,
			    std::string description,
			    int lowrange,
			    int highrange);

  /** Define new bool variable */
  void defineNewBoolVariable(std::string name,
			     bool * datamember,
			     std::string description);

  /** Settable parameter, double */
  struct doubleparameter {
    std::string description;
    double * directvaraddress;
    std::string unitname;  double unit;
    double okrange_low; double okrange_high;
    double valueSetByUser;
  };

  /** Settable parameter, int and bool */
  struct intboolparameter {
    std::string description;
    //Two of these are put to null to indicate type:
    int * directvaraddress_int;
    unsigned int * directvaraddress_uint;
    bool * directvaraddress_bool;
    int okrange_low; int okrange_high;
    int valueSetByUser;
  };

  /** Map of settable parameters (double) */
  std::map <std::string,doubleparameter>  m_doubleparMap;
  /** Map of settable parameters (int & bool) */
  std::map <std::string,intboolparameter> m_intboolparMap;

  double m_propertyNotSetMagicNumber;
  int m_propertyNotSetMagicNumber_int;

  mutable Athena::MsgStreamMember m_msg;

};

#include "TRTDigSettings.icc"

#endif
