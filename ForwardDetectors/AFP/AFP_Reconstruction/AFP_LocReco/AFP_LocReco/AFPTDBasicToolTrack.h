/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @file   AFPTDBasicToolTrack.h
/// @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
/// @date   2021-11-11
/// 
/// @brief  Header file for AFPTDBasicToolTrack used in ToF tracks reconstruction.

#ifndef AFP_LOCRECO_AFPTDBASICKALMANTOOLTRACK_H
#define AFP_LOCRECO_AFPTDBASICKALMANTOOLTRACK_H 1

#include "xAODForward/AFPToFHit.h"

#include <list>

/// Class representing a reconstructed basic ToF track.
class AFPTDBasicToolTrack
{
public:

  /// @brief Description here

  AFPTDBasicToolTrack (int stationID, int TrainID , double Time, int Size, int Sat);

  int m_stationID;
  int m_trainID;
  int m_trainSize;
  int m_trainNSat;
  double m_trainTime;

  /// @copydoc AFPTDBasicToolTrack::m_barsInTrain
  const std::list<const xAOD::AFPToFHit*>& barInTrain () const {return m_barInTrain;}

  /// Adds a new bar to the track
inline  void addBar (const xAOD::AFPToFHit* bar) {m_barInTrain.push_back(bar);};

protected:

  /// Bars used to reconstruct the train
  std::list<const xAOD::AFPToFHit*> m_barInTrain;

};

#endif
