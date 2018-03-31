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

static const InterfaceID IID_IAFPSiLayerMonitor ("IAFPSiLayerMonitor", 1, 0);

class IAFPSiLayerMonitor : virtual public IAlgTool
{
public:
  static const InterfaceID &interfaceID()
  {return IID_IAFPSiLayerMonitor;}

  virtual ~IAFPSiLayerMonitor() {}

  /// Initilise tool
  virtual StatusCode initialize() = 0;

  /// Finalise tool
  virtual StatusCode finalize() = 0;

  virtual void setParentMonitor (AFPHitsMonitorTool* parent) = 0;
  
  virtual void bookHistograms(ManagedMonitorToolBase* toolToStoreHistograms, std::string histsDirName = "") = 0;
  virtual void fillHistograms(const xAOD::AFPSiHit& hit) = 0;
  virtual void eventEnd(){}		///< method that should be called when event processing is finished

  virtual void endOfLumiBlock(){} ///< Process histograms at the end of lumiblock

  virtual int layerID () const = 0;

  virtual std::string makeHistName (const std::string name) const = 0; ///< create a name suffixed with station and layer numbers
  virtual std::string makeHistTitle (const std::string title) const = 0; ///< create a title suffixed with station and layer numbers

  virtual const std::string& histsDirName () const = 0;

  virtual const std::string& layerFullName() const = 0;

  virtual double correctHitsForPileUp (double hits) const = 0;

  virtual double hitsInEvent () const = 0;
  virtual double hitsInEventScaled () const = 0;

  virtual double hitsInEventHotSpot () const = 0;
  virtual double hitsInEventHotSpotScaled () const = 0;
};

#endif
