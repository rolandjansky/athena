/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @file   AFPTDBasicTool.h
/// @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
/// @date   2021-11-11
/// 
/// @brief  Header file for AFPTDBasicTool used in tracks reconstruction.

#ifndef AFP_LOCRECO_AFPTDBASICKALMANTOOL_H
#define AFP_LOCRECO_AFPTDBASICKALMANTOOL_H 1

// AFP_LocReco includes
#include "AFP_LocRecoInterfaces/IAFPTDLocRecoTrackAlgTool.h"
#include "AFP_LocReco/AFPLocRecoStationBasicObj.h"
#include "AFP_LocReco/AFPTDBasicToolTrack.h"

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// xAOD includes
#include "xAODForward/AFPToFHit.h"
#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPToFTrackContainer.h"
#include "xAODForward/AFPToFTrackAuxContainer.h"

// STL includes
#include <string>
#include <list>
#include <vector>

/// Class reconstructing tracks using basic Kalman filter.
///
/// The idea of the reconstruction is presented in the talk
/// https://indico.cern.ch/event/275484/contributions/1618277/attachments/499062/689421/mdyndal_2013-10-15_AFP_Gen.pdf
class AFPTDBasicTool
  : virtual public ::IAFPTDLocRecoTrackAlgTool,
    public ::AthAlgTool
{
public:
  AFPTDBasicTool(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  virtual ~AFPTDBasicTool() override {}

  /// Read parameters from job options and print tool configuration
  virtual StatusCode initialize() override;

  /// Does nothing
  virtual StatusCode finalize() override;

  /// @brief Does actual tracks reconstruction.
  ///
  /// -# Reads ToF hits from the xAOD container.
  /// -# The output list is saved to xAOD containers and objects.
  StatusCode reconstructTracks(xAOD::AFPToFTrackContainer* outputContainer) override;

  /// @copydoc IAFPTDLocRecoTrackAlgTool::outputContainerName(xAOD::AFPToFTrackContainer*)
  const std::string& outputContainerName () const override {return m_tracksContainerName;}

  /// clear station from bars saved in #m_stationBars;
  void clearTrains()
  {m_trainBars[0].clear(); m_trainBars[1].clear(); m_trainBars[2].clear(); m_trainBars[3].clear();}


private:
  /// Name of the xAOD container to which tracks will be saved
  std::string m_tracksContainerName;

  /// AFP station ID for which tracks will be reconstructed
  int m_stationID;

  /// Name of the xAOD container with clusters to be used in track reconstruction
  std::string m_hitContainerName;

  /// Container with clusters from which tracks are reconstructed
  const xAOD::AFPToFHitContainer* m_hitContainer;

  /// Minimal number of clusters in track. If there are less clusters track is rejected (Default = 3)
  unsigned int m_minHitsNumber;

  /// Maximal Amplitude at which bar can be joined to the track

  double m_maxAllowedLength;

std::vector<double> m_TimeOffset;
std::vector<double> m_BarWeight;  

  /// Fills Station with ToF hits, dividing them into stations 
  void fillTrainWithBars();

  std::vector<const xAOD::AFPToFHit*> m_trainBars[4];
  
  /// Save reconstructed track to the xAOD container
  void saveToXAOD (const AFPTDBasicToolTrack& recoTrack, xAOD::AFPToFTrackContainer* containerToFill) const;
};


#endif // > !AFP_LOCRECO_AFPTDBASICKALMANTOOL_H
