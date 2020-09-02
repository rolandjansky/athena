/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  Gaudi::Property<double> m_MaxStep{this, "MaxStep", -1., "Maximum step length"};
  /** Minimum remaining kinetic energy for a track */
  Gaudi::Property<double> m_MinEkine{this, "MinEkine", -1., "Minimum remaining kinetic energy for a track"};
  /** Maximum total track length */
  Gaudi::Property<double> m_MaxTrackLength{this, "MaxTrackLength", -1., "Maximum total track length"};
  /** Maximum global time for a track */
  Gaudi::Property<double> m_MaxTime{this, "MaxTime", -1., "Maximum global time for a track"};
  /** Minimum remaining range for a track */
  Gaudi::Property<double> m_MinRange{this, "MinRange", -1., "Minimum remaining range for a track"};
  /** Use 'contains' or 'isMatch' function for string comparison */
  Gaudi::Property<std::string> m_matchType{this, "MatchType", "isMatch", "Use 'contains' or 'isMatch' function for string comparison"};
  /** List of Logical volume to which these limits should be applied */
  Gaudi::Property<std::vector<std::string> > m_logicalVolumes{this, "VolumeList" , {}, "List of Logical volume to which these limits should be applied"};
};

#endif //G4ATLASTOOLS_USERLIMITSSVC_H
