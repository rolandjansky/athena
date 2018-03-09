/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/VolumeUtils.h"

// STL includes
#include <set>

// Geant4 includes
#include "G4LogicalVolumeStore.hh"

namespace
{
  //---------------------------------------------------------------------------
  /// @brief Helper function for matching strings with wildcards.
  /// It's a recursive function that checks if two given strings match.
  /// The first string may contain wildcard characters.
  //---------------------------------------------------------------------------
  bool matchStrings(const char* first, const char* second)
  {
    // If we reach at the end of both strings, we are done
    if (*first == '\0' && *second == '\0')
      return true;

    // Make sure that the characters after '*' are present in second string.
    // This function assumes that the first string will not contain two
    // consecutive '*'
    if (*first == '*' && *(first+1) != '\0' && *second == '\0')
      return false;

    // If the current characters of both strings match
    if (*first == *second)
      return matchStrings(first+1, second+1);

    // If there is *, then there are two possibilities
    // a) We consider current character of second string
    // b) We ignore current character of second string.
    if (*first == '*')
      return matchStrings(first+1, second) || matchStrings(first, second+1);
    return false;
  }
}

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Search for logical volumes in the G4 volume store
  //---------------------------------------------------------------------------
  std::set<std::string> findLogicalVolumes(const std::string& pattern)
  {
    // Use a set because there can be multiple occurences with same name
    std::set<std::string> foundVolumes;

    // Iterate over the G4 volumes and look for matches
    auto logicalVolumeStore = G4LogicalVolumeStore::GetInstance();
    for(auto logvol : *logicalVolumeStore) {
      if( matchStrings( pattern.data(), logvol->GetName() ) ) {
        foundVolumes.insert( logvol->GetName() );
      }
    }

    return foundVolumes;
  }

  //---------------------------------------------------------------------------
  // Search for multiple logical volumes in the G4 volume store
  //---------------------------------------------------------------------------
  std::vector<std::string>
  findLogicalVolumes(const std::vector<std::string>& patterns,
                     MsgStream& msg)
  {
    std::set<std::string> parsedVolumes;
    for(const auto& pattern : patterns) {
      const auto patternVols = findLogicalVolumes(pattern);
      if(patternVols.empty()) {
        msg << MSG::WARNING << "No volume found matching pattern: "
            << pattern << endreq;
      }
      parsedVolumes.insert( patternVols.begin(), patternVols.end() );
    }
    return std::vector<std::string>( parsedVolumes.begin(), parsedVolumes.end() );
  }

}
