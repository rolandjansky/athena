/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_MONITORING_IAFPSILAYERMONITOR_H
#define AFP_MONITORING_IAFPSILAYERMONITOR_H

#include <xAODForward/AFPSiHit.h>

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"

// forward declarations
class LWHist2D;
class TH1;
class AFPHitsMonitorTool;
class ManagedMonitorToolBase;

static const InterfaceID IID_IAFPSiLayerMonitor ("IAFPSiLayerMonitor", 1, 0);

/// @brief Interface of a class monitoring one silicon layer.
class IAFPSiLayerMonitor : virtual public IAlgTool
{
public:
  static const InterfaceID &interfaceID()
  {return IID_IAFPSiLayerMonitor;}

  /// @brief Does nothing
  virtual ~IAFPSiLayerMonitor() {}

  /// @brief Initilise tool
  virtual StatusCode initialize() = 0;

  /// @brief Finalise tool
  virtual StatusCode finalize() = 0;

  /// @brief Sets variable storing pointer to the main monitoring object.
  virtual void setParentMonitor (AFPHitsMonitorTool* parent) = 0;

  /// @brief Creates, adds axes descriptions and books histograms filled by this monitor.
  virtual void bookHistograms(ManagedMonitorToolBase* toolToStoreHistograms, std::string histsDirName = "") = 0;

  /// @brief Fills histograms which have to be filled for each hit.
  virtual void fillHistograms(const xAOD::AFPSiHit& hit) = 0;

  /// @brief Method that should be called when event processing is finished.
  ///
  /// Default implementation does nothing.
  virtual void eventEnd(){}

  /// @brief Process histograms at the end of lumiblock
  ///
  /// Default implementation does nothing.
  virtual void endOfLumiBlock(){}

  /// @brief Pixel plane ID number.
  virtual int layerID () const = 0;

  /// @brief Returns a name suffixed with station and layer numbers.
  virtual std::string makeHistName (const std::string name) const = 0;

  /// @brief Returns a title suffixed with station and layer numbers.
  virtual std::string makeHistTitle (const std::string title) const = 0;

  /// @brief Returns name of the ROOT file directory where histograms are stored
  virtual const std::string& histsDirName () const = 0;

  /// @brief Returns layer and station name used to label bins in summary histograms.
  virtual const std::string& layerFullName() const = 0;

  /// @brief Normalises number of hits in an event to 0 pile-up.
  virtual double correctHitsForPileUp (double hits) const = 0;

  /// @brief Number of pixels fired in an event.
  virtual double hitsInEvent () const = 0;
  /// @brief Returns number of pixels fired in an event corrected for pile-up.
  virtual double hitsInEventScaled () const = 0;

  /// @brief Number of pixels fired in an event in a hot-spot.
  virtual double hitsInEventHotSpot () const = 0;
  /// @brief Number of hits in hot-spot corrected for pile-up.
  virtual double hitsInEventHotSpotScaled () const = 0;
};

#endif
