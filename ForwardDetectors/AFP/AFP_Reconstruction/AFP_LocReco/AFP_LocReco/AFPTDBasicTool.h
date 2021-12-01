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

// STL includes
#include <string>
#include <list>
#include <vector>


// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// xAOD includes
#include "xAODForward/AFPToFHit.h"
#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPToFTrackContainer.h"
#include "xAODForward/AFPToFTrackAuxContainer.h"

// AFP_LocReco includes
#include "AFP_LocReco/IAFPTDLocRecoTrackAlgTool.h"
#include "AFP_LocReco/AFPLocRecoStationBasicObj.h"
#include "AFP_LocReco/AFPTDBasicToolTrack.h"


/// Class reconstructing tracks using basic Kalman filter.
///
/// The idea of the reconstruction is presented in the talk
/// https://indico.cern.ch/event/275484/contributions/1618277/attachments/499062/689421/mdyndal_2013-10-15_AFP_Gen.pdf
class AFPTDBasicTool
  : public extends<AthAlgTool, IAFPTDLocRecoTrackAlgTool>
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
  StatusCode reconstructTracks(std::unique_ptr<xAOD::AFPToFTrackContainer>& outputContainer, const EventContext& ctx) const override;

  /// @copydoc IAFPTDLocRecoTrackAlgTool::outputContainerName(xAOD::AFPToFTrackContainer*)
  const std::string& outputContainerName () const override {return m_tracksContainerName;}


private:
  /// Name of the xAOD container to which tracks will be saved
  Gaudi::Property<std::string> m_tracksContainerName{this, "tracksContainerName", "AFPToFTrackContainer", "Name of the container in which ToF tracks are saved"};

  /// AFP station ID for which tracks will be reconstructed
  Gaudi::Property<int> m_stationID{this, "stationID", 0, "ID number of station for which tracks should be reconstructed"};

  /// Name of the xAOD container with clusters to be used in track reconstruction
  SG::ReadHandleKey<xAOD::AFPToFHitContainer> m_hitContainerKey{this, "AFPToFHitClusterContainerKey", "AFPToFHitContainer", "Name of the container with ToF hits from which ToF tracks are to be reconstructed"};

  /// Minimal number of clusters in track. If there are less clusters track is rejected (Default = 3)
  Gaudi::Property<unsigned int> m_minHitsNumber{this, "minHitsNumber", 3, "Minimal number of bars in track. If there are less bars track is rejected"};

  Gaudi::Property<double> m_maxAllowedLength{this, "maxAllowedLength", 100, "Maximal length of the bar signal at which bar can be joined to the track"};

  Gaudi::Property<std::vector<double> > m_TimeOffset{this, "TimeOffset", {}, "Array of bar time offsets with respect t0=0 at vz=0"};
  Gaudi::Property<std::vector<double> > m_BarWeight{this, "BarWeight", {1.,1.,1.,1.}, "Array of bar weights used to calculate average time"};  

  /// Fills Station with ToF hits, dividing them into stations 
  void fillTrainWithBars(std::vector<const xAOD::AFPToFHit*> my_trainBars[4], SG::ReadHandle<xAOD::AFPToFHitContainer>& hitContainer) const;

  /// clear station from bars saved in #my_trainBars;
  void clearTrains(std::vector<const xAOD::AFPToFHit*> my_trainBars[4]) const
  {my_trainBars[0].clear(); my_trainBars[1].clear(); my_trainBars[2].clear(); my_trainBars[3].clear();}  
  
  /// Save reconstructed track to the xAOD container
  void saveToXAOD (const AFPTDBasicToolTrack& recoTrack, std::unique_ptr<xAOD::AFPToFTrackContainer>& containerToFill, SG::ReadHandle<xAOD::AFPToFHitContainer>& hitContainer) const;
};


#endif // > !AFP_LOCRECO_AFPTDBASICKALMANTOOL_H
