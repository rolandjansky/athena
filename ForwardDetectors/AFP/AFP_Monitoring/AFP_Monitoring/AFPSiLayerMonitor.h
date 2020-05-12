/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_MONITORING_AFPSILAYERMONITOR_H
#define AFP_MONITORING_AFPSILAYERMONITOR_H

#include "AFP_Monitoring/IAFPSiLayerMonitor.h"

// FrameWork includes
#include <xAODForward/AFPSiHit.h>
#include "AthenaBaseComps/AthAlgTool.h"

// forward declarations
class LWHist2D;
class TH1;
class AFPHitsMonitorTool;


/// @brief Class monitoring one silicon layer.
///
/// Python properties:
/// * `pixelLayerID` - ID number of pixel layer #m_pixelLayerID (default: -1)
/// * `stationID` - ID number of station in which is the monitored layer #m_stationID (default: -1)
/// * `hitsScaleFactor` - Scale factor for normalising hits in event to pile-up #m_hitsScaleFactor (default: 0.04)  
/// * `hotSpotStartRow` - First row of the hot-spot (included in the hot-spot) #m_hotSpotStartRow (default: 0)
/// * `hotSpotEndRow` - Last row of the hot-spot (included in the hot-spot) #m_hotSpotEndRow (default: 50)
/// * `hotSpotStartCol` - First column of the hot-spot (included in the hot-spot) #m_hotSpotStartCol (default: 0)
/// * `hotSpotEndCol` - Last column of the hot-spot (included in the hot-spot) #m_hotSpotEndCol (default: 30)

class AFPSiLayerMonitor : virtual public IAFPSiLayerMonitor, public AthAlgTool
{
public:
  /// @brief Declares python properites
  AFPSiLayerMonitor(const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  /// Does nothing  
  ~AFPSiLayerMonitor();

  /// Constructs #m_layerFullName from #m_stationID and #m_pixelLayerID
  StatusCode initialize() override;

  /// Does nothing
  StatusCode finalize() override;

  /// @brief @copybrief IAFPSiLayerMonitor::setParentMonitor()
  /// 
  /// Sets #m_parentMonitor
  void setParentMonitor (AFPHitsMonitorTool* parent) override {m_parentMonitor = parent;}

  /// @copybrief IAFPSiLayerMonitor::bookHistograms()
  void bookHistograms(ManagedMonitorToolBase* toolToStoreHistograms, std::string histsDirName = "") override;

  /// @copybrief IAFPSiLayerMonitor::fillHistograms()
  ///
  /// These are histograms with "number of hits" on Y axis. It also
  /// fills summary histograms from
  /// AFPHitsMonitorTool::m_summaryManager in #m_parentMonitor
  void fillHistograms(const xAOD::AFPSiHit& hit) override;

  /// @copybrief IAFPSiLayerMonitor::eventEnd()
  ///
  /// It fills histograms with "events" on Y axis. It also fills
  /// summary histograms from AFPHitsMonitorTool::m_summaryManager in
  /// #m_parentMonitor.
  ///
  /// @note At the end this method clears buffers for an event
  /// e.g. #m_hitsInEvent, #m_hitsInEventHotSpot.
  void eventEnd() override;

  /// @copybrief IAFPSiLayerMonitor::endOfLumiBlock()
  ///
  /// Does nothing
  void endOfLumiBlock() override; 

  /// @copybrief IAFPSiLayerMonitor::layerID()
  ///
  /// @return #m_pixelLayerID.
  int layerID () const override {return m_pixelLayerID;}

  /// @copybrief IAFPSiLayerMonitor::makeHistName()
  std::string makeHistName (const std::string name) const override;

  /// @copybrief IAFPSiLayerMonitor::makeHistTitle()
  std::string makeHistTitle (const std::string title) const override;

  /// @copybrief IAFPSiLayerMonitor::histsDirName()
  ///
  /// @return #m_histsDirName
  const std::string& histsDirName () const override {return m_histsDirName;}

  /// @copybrief IAFPSiLayerMonitor::layerFullName()
  const std::string& layerFullName() const override {return m_layerFullName;}

  /// @copybrief IAFPSiLayerMonitor::correctHitsForPileUp()
  ///
  /// The output is calculated according to the following formula
  /// \f[ n_{\textrm{corr}} = n_{\textrm{obs}} - a*\mu \f]
  /// where:
  /// * \f$ n_{\textrm{corr}} \f$ - is number of hits corrected for pile-up
  /// * \f$ n_{\textrm{obs}} \f$ - is observed/measured number of hits i.e. NOT corrected for pile-up
  /// * \f$ a \f$ - is a correction factor #m_hitsScaleFactor
  /// * \f$ \mu \f$ - is an average number of interactions in a bunch crossing (pile-up)
  virtual double correctHitsForPileUp (double hits) const override;

  /// @copybrief IAFPSiLayerMonitor::hitsInEvent()
  ///
  /// @return #m_hitsInEvent
  double hitsInEvent () const override {return m_hitsInEvent;}

  /// @copybrief IAFPSiLayerMonitor::hitsInEventScaled()
  double hitsInEventScaled () const override {return correctHitsForPileUp(m_hitsInEvent);}

  /// @copybrief IAFPSiLayerMonitor::hitsInEventHotSpot()
  ///
  /// The hot-spot is defined by: #m_hotSpotStartRow,
  /// #m_hotSpotEndRow, #m_hotSpotStartCol,
  /// #m_hotSpotEndCol. Definition includes boundries i.e. <= and >=
  /// are used.
  ///
  /// @return #m_hitsInEventHotSpot
  double hitsInEventHotSpot () const override {return m_hitsInEventHotSpot;}

  /// @copybrief IAFPSiLayerMonitor::hitsInEventHotSpotScaled()
  double hitsInEventHotSpotScaled () const override {return correctHitsForPileUp(m_hitsInEventHotSpot);}

protected:
  int m_pixelLayerID;		///< ID number of the silicon pixel layer
  int m_stationID;		///< ID number of the station where the pixel layer is mounted
  std::string m_layerFullName;	///< Layer and station name used to label summary histograms bins

  AFPHitsMonitorTool* m_parentMonitor; ///< Pointer to the parent monitoring tool
  std::string m_histsDirName;	       ///< Name of the ROOT file directory where histograms will be saved

  double m_hitsScaleFactor;	///< Factor for correcting number of hits for pile-up (see AFPSiLayerMonitor::correctHitsForPileUp()
  unsigned int m_hitsInEvent;	///< Number of counted pixels fired in the current event.

  // ===== histograms =====
  LWHist2D* m_hitMap;		///< 2D distribution of hits in a layer row vs column (336 x 80)

  /// @brief Distribution of number of hits in an event.
  ///
  /// @note Must be TH1 because overflows need to be included in mean
  TH1* m_hitMultiplicity;

  /// @brief Distribution of number of time-over-threshold i.e. signal strength for each hit.
  TH1* m_timeOverThreshold;

  /// Defines the first row of a hot spot (including this row).
  int m_hotSpotStartRow;
  /// Defines the last row of a hot spot (including this row).
  int m_hotSpotEndRow;
  /// Defines the first column of a hot spot (including this column).
  int m_hotSpotStartCol;
  /// Defines the last column of a hot spot (including this column).
  int m_hotSpotEndCol;
  /// Counts number of hits in a hot-spot in an event
  unsigned int m_hitsInEventHotSpot;
  /// @brief Distribution of number of hits in a hot-spot in an event.
  ///
  /// The hot-spot is defined by: #m_hotSpotStartRow,
  /// #m_hotSpotEndRow, #m_hotSpotStartCol,
  /// #m_hotSpotEndCol. Definition includes boundries i.e. <= and >=
  /// are used.
  ///
  /// @note Must be TH1 because overflows need to be included in mean
  TH1* m_hitMultiplicityHotSpot; 
};

#endif
