/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BEAMHALOGENERATORSETTINGS_H
#define BEAMHALOGENERATORSETTINGS_H

#include <string>
#include <vector>
#include <map>

class BeamHaloParticle;

struct BeamHaloGeneratorLimits {
  
};

/** @class BeamHaloGeneratorSettings
    @author W. H. Bell <W.Bell@cern.ch>
    
    @brief A class to read a vector of strings defining particle
    filtering settings and provide a method for filtering particles
    based on type and kinematics.
*/
class BeamHaloGeneratorSettings {
 public:

  /** Construct a class with a vector of string settings to filter
      particles.*/
  BeamHaloGeneratorSettings(const std::vector<std::string>& settings);

  /** A function to parse the settings using the vector of strings
      given to the constructor of this class.
      @return 0 if successful and a status code otherwise */
  int parseSettings(void);

  /** Check if the supplied beam halo particle passes the generator
      settings. */
  bool checkParticle(BeamHaloParticle *beamHaloParticle);

  /** Print a summary of the current settings. */
  void printSettings(void);

 private:

  /** An enum mapped to different shape requirements. */
  enum enumShapeRequirements {
    NONE,
    CYLINDER};

  enum enumLimits {
    PX_LIMIT,
    PY_LIMIT,
    PZ_LIMIT,
    E_LIMIT,
    X_LIMIT,
    Y_LIMIT,
    Z_LIMIT,
    PT_LIMIT,
    PHI_LIMIT,
    ETA_LIMIT,
    R_LIMIT,
    W_LIMIT,
    ENUM_LIMITS_SIZE};

  /** A function to read the limit settings from the supplied vector of
      strings. */
  int parseLimitSetting(std::vector<std::string> *commandVector);

  /** A function to check if a particle is within a cylinder. */
  bool checkCylinder(BeamHaloParticle *beamHaloParticle);

  /** A function to check if a particle is within a range. */
  bool checkSetting(int index, double value);

  /** The name of the limits. */
  static const std::string m_limitNames[ENUM_LIMITS_SIZE];

  /** A vector of strings to configure the generator settings. */
  std::vector<std::string> m_generatorSettings;

  /** An allowed set of PDG ids where any empty vector implies all PDG
      ids are allowed. */
  std::vector<long> m_allowedPdgIds;

  /** Minimum and maximum limits. */
  std::vector<std::pair<float, float> > m_limits;

  /** A vector of flags to signal if a limit should be used or not. */
  std::vector<std::pair<bool, bool> > m_limitsEnabled;

  /** A variable to store the volume shape requirement. */
  int m_shapeRequirement;

  /** A flag to check if the settings have been parsed or not. */
  bool m_settingsParsed;
};

#endif
