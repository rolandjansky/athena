/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTruthBase_RECORDINGENVELOPE_H
#define MCTruthBase_RECORDINGENVELOPE_H

// Athena headers
#include "TrackRecord/TrackRecordCollection.h"
#include "StoreGate/WriteHandle.h"

// STL headers
#include <string>

class G4LogicalVolume;
class G4Step;
class G4VPhysicalVolume;

/// @class RecordingEnvelope
/// @brief Responsible for finding the G4LogicalVolume pointer for
/// each recording envelope and for creating and filling the
/// corresponding TrackRecordCollection.

class RecordingEnvelope
{
public:

  /// Constructor
  RecordingEnvelope(const std::string& envelopeVolumeName, const std::string& trackRecordCollectionName);

  /// Destructor
  ~RecordingEnvelope();

  /// Finds the pointer to the G4LogicalVolume called
  /// m_envelopeVolumeName and the number of levels beneath the world
  /// volume that it lies.
  bool Initialize();

  /// Called at the start of each G4 event. Recreates the
  /// TrackRecordCollection if necessary.
  void BeginOfEvent();

  /// Returns the number of levels beneath the world volume that the
  /// G4LogicalVolume associated with this recording envelope lies.
  inline int GetLevel() const;

  /// Returns the name of the recording envelope volume.
  inline std::string GetVolumeName() const;

  /// Returns the pointer to the G4LogicalVolume used by this
  /// recording envelope.
  inline const G4LogicalVolume* GetLogicalVolume() const;

  /// Returns the name of the TrackRecordCollection to which tracks
  /// crossing this recording envelope should be written.
  inline std::string GetTrackRecordCollectionName() const;

  /// Add a TrackRecord to the TrackRecordCollection owned by this
  /// recording envelope based on the information in aStep.
  void AddTrackRecord(const G4Step* aStep);
private:
  /// Recursively called method used to hunt for the G4LogicalVolume
  /// associated with this recording envelope by the Initialize
  /// method.
  bool checkDaughters(const G4LogicalVolume *possibleParent, const G4VPhysicalVolume *thePhysicalVolume, int& level) const;

  /// The number of levels beneath the world that the G4LogicalVolume
  /// called m_envelopeVolumeName lies.
  int m_level;

  /// Pointer to the G4LogicalVolume used by this recording
  /// envelope.

  G4LogicalVolume *m_logicalVolume;

  /// Name of the G4LogicalVolume used by this recording envelope.
  const std::string m_envelopeVolumeName;

  /// WriteHandle to the TrackRecordCollection used by this envelope.
  SG::WriteHandle<TrackRecordCollection> m_trackRecordCollection;
};

std::string RecordingEnvelope::GetVolumeName() const
{
  return m_envelopeVolumeName;
}

const G4LogicalVolume* RecordingEnvelope::GetLogicalVolume() const
{
  return m_logicalVolume;
}

std::string RecordingEnvelope::GetTrackRecordCollectionName() const
{
  return m_trackRecordCollection.name();
}

int RecordingEnvelope::GetLevel() const
{
  return m_level;
}

#endif //MCTruthBase_RECORDINGENVELOPE_H
