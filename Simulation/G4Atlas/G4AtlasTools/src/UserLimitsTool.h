/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_USERLIMITSTOOL_H
#define G4ATLASTOOLS_USERLIMITSTOOL_H

// Base classes
#include "G4AtlasInterfaces/IUserLimitsTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

// STL library
#include <string>
#include <vector>

/** @class UserLimitsTool UserLimitsTool.h "G4AtlasTools/UserLimitsTool.h"
 *
 *  Concrete Tool to apply G4 User Limits
 */

class UserLimitsTool final : public extends<AthAlgTool, IUserLimitsTool> {
 public:
  // Standard constructor and destructor
  UserLimitsTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~UserLimitsTool() {}

  virtual StatusCode initialize() override final;

 private:
  /** Functions for string comparison */
  bool isMatch(const std::string& pattern, const std::string logicalVolume) const;
  bool contains(const std::string& pattern, const std::string logicalVolume) const;
  /** Maximum step length */
  double m_MaxStep;
  /** Minimum remaining kinetic energy for a track */
  double m_MinEkine;
  /** Maximum total track length */
  double m_MaxTrackLength;
  /** Maximum global time for a track */
  double m_MaxTime;
  /** Minimum remaining range for a track */
  double m_MinRange;
  /** Use 'contains' or 'isMatch' function for string comparison */
  std::string m_matchType;
  /** List of Logical volume to which these limits should be applied */
  std::vector<std::string> m_logicalVolumes;
};

#endif //G4ATLASTOOLS_USERLIMITSTOOL_H
