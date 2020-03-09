/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_USERLIMITSSVC_H
#define G4ATLASTOOLS_USERLIMITSSVC_H

// Base classes
#include "G4AtlasInterfaces/IUserLimitsSvc.h"
#include "AthenaBaseComps/AthService.h"

// STL library
#include <string>
#include <vector>

/** @class UserLimitsSvc UserLimitsSvc.h "G4AtlasTools/UserLimitsSvc.h"
 *
 *  Concrete Tool to apply G4 User Limits
 */

class UserLimitsSvc final : public extends<AthService, IUserLimitsSvc> {
 public:
  // Standard constructor and destructor
  UserLimitsSvc(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~UserLimitsSvc() = default;

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

#endif //G4ATLASTOOLS_USERLIMITSSVC_H
