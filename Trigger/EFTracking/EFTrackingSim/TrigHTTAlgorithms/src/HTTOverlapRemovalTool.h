/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file HTTOverlapRemovalTool.h
* @author Zhaoyuan.Cui@cern.ch
* @date Dec.4, 2020
* @brief Overlap removal tool for HTTTrack.
*/

#ifndef HTTOVERLAPREMOVALTOOL_H
#define HTTOVERLAPREMOVALTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"

#include <string>
#include <vector>
#include <ostream>

enum class ORAlgo {Normal, InvertGrouping};
/**
* @class HTTOverlapRemovalTool
* @brief Remove (mark) duplicate tracks
* This tool takes HTTTrack as input and mark their status of passing/failing the overlap removal criteria.
*/
class HTTOverlapRemovalTool: public AthAlgTool {
public:

  HTTOverlapRemovalTool (const std::string&, const std::string&, const IInterface*);
  HTTOverlapRemovalTool() = delete;

  StatusCode initialize() override;

  StatusCode runOverlapRemoval(std::vector<HTTRoad*>& roads);

  // Original Overlap Removal function
  // Compare chi2 and common number of hits
  StatusCode runOverlapRemoval(std::vector<HTTTrack>& tracks);

  void setDoSecondStage(bool v) { m_do2ndStage = v; }
  bool getDoSecondStage() const { return m_do2ndStage; }
  ORAlgo getAlgorithm() const {return m_algo;}
  //  Find the one track from the duplicate group with the minium Chi2 and maximum # of hits
  void findMinChi2MaxHit(const std::vector<int>& duplicates, std::vector<HTTTrack>& tracks);
  //  Find number of common hits between two tracks
  int findNCommonHits(const HTTTrack&, const HTTTrack&);
  int findNonOverlapHits(const HTTTrack&, const HTTTrack&);

  StatusCode removeOverlapping(HTTTrack & track1, HTTTrack & track2);
  static bool compareTrackQuality(const HTTTrack & track1, const HTTTrack & track2);

private:


  Gaudi::Property <bool> m_do2ndStage { this, "DoSecondStage", false, "Do second stage of not?"};
  Gaudi::Property <int> m_NumOfHitPerGrouping { this, "NumOfHitPerGrouping", 5, "Number of hits pre grouping"};
  Gaudi::Property <float> m_minChi2 { this, "MinChi2", 40, "Minimum chi2 requirement for tracks being considered in OR"};
  Gaudi::Property <bool> m_roadSliceOR { this, "roadSliceOR", true, "Do slice-wise overlap removal for roads (Hough-only)"};
  Gaudi::Property <int> m_localMaxWindowSize { this, "localMaxWindowSize", 0, "Only create roads that are a local maximum within this window size. Set this to 0 to turn off local max filtering"};
  Gaudi::Property <unsigned> m_imageSize_x { this, "nBins_x", 0, "number of bins in, eg, phi_track"};
  Gaudi::Property <unsigned> m_imageSize_y { this, "nBins_y", 0, "number of bins in, eg, q/pT"};
  Gaudi::Property <std::string> m_algorithm { this, "ORAlgo", "Normal", "Overlap removal algorithm"};
  Gaudi::Property <bool> m_doFastOR { this, "doFastOR", false, "Use fast overlap removal algorithm instead of default"};

  int m_totLayers = 0;                 //  Total number of layers used for a track
  ORAlgo m_algo;                    //  Internal ORAlgo enum for faster compare


  StatusCode runOverlapRemoval_fast(std::vector<HTTTrack>& tracks);

  //  ServiceHandle
  ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping{this,"TrigHTTMappingSvc","TrigHTTMappingSvc"};   //  Get the number of layer through map

};

#endif // HTTOverlapRemovalTool_h
