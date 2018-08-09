/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


/**
@mainpage AFP_Monitoring Package

This package is responsible for creating histograms used in online and
offline Athena monitoring. The idea behind the design is to represent
each hardware element with a separate class e.g. provide a class for
monitoring a layer of a silicon detector or a station.

@section HowToMain How to...

@subsection newPixelLayerDistribution add new distribution to monitor a pixel layer.

The implementation of a new distribution which should be filled for
every layer in each station needs to be done in the AFPSiLayerMonitor
class.

First a member storing a histogram pointer needs to be
added to the class defined in AFPSiLayerMonitor.h.

Next, create a new histogram object, add axes labels and book
histogram in the main tool in the method
AFPSiLayerMonitor::bookHistograms(). When booking, monitoring
parameters like how often should the histogram be rebuilt (e.g. every
luminosity block or every run). The monitoring parameters are set via
the object ManagedMonitorToolBase::MonGroup.

If the histogram should be filled with quantities which are calculated **based on individual hits** the fill method should be added to
AFPSiLayerMonitor::fillHistograms(). If the histogram should be filled
with quantities which are calculated based on **all hits in one
event** in the given plane the fill method should be added to
AFPSiLayerMonitor::eventEnd().

In case of preparing a distribution of quantities based on all hits in
an event in plane it is possible that extra calculations will have to
be performed. The calculations and extra variables needed for the
calculations can be implemented in extra methods and members added to
the class AFPSiLayerMonitor.

The histogram will be **automatically deleted** by the monitoring
service so there is no need to free memory.


@subsection newPixelStationDistribution add new distribution to monitor a station with tracking detectors.

The implementation of a new distribution which should be filled for every
station needs to be done in the AFPSiStationMonitor class.

First a member storing a histogram pointer needs to be
added to the class defined in AFPSiStationMonitor.h

Next, create a new histogram object, add axes labels and book
histogram in the main tool in the method
AFPSiStationMonitor::bookHistograms(). When booking, monitoring
parameters like how often should the histogram be rebuilt (e.g. every
luminosity block or every run). The monitoring parameters are set via
the object ManagedMonitorToolBase::MonGroup.

If the histogram should be filled with quantities which are calculated **based on individual hits** the fill method should be added to
AFPSiStationMonitor::fillHistograms(). If the histogram should be filled
with quantities which are calculated based on **all hits in one
event** in the given station the fill method should be added to
AFPSiStationMonitor::eventEnd().

In case of preparing a distribution of quantities based on all hits in
an event in station it is possible that extra calculations will have to
be performed. The calculations and extra variables needed for the
calculations can be implemented in extra methods and members added to
the class AFPSiStationMonitor.

The histogram will be **automatically deleted** by the monitoring
service so there is no need to free memory.



@subsection newSummaryLayerDistribution add new summary distribution with means for each plane.

In order to add a new distribution which will have on X-axis names of planes and on Y axis mean value of a given distribution 
station the following actions must be taken.

@subsubsection newSummaryLayerDistributionHit Distributions filled for every hit.

If the distribution should be filled for **every hit** a method
providing value used for filling the distribution must be available in
xAOD::AFPSiHit class.  Next in AFPHitsMonitorTool::initialize() method
AFPHitsMonitorTool::m_summaryManager object should be used to create a
new summary distribution with method
AFPSiLayerSummaryManager::createSummaryHits(). The first argument
should be `this`, the second is ManagedMonitorToolBase::MonGroup
object defining rebooking frequency (lumi-block or run) and in which
ROOT file directory the histogram should be saved, next are name and
title of the histogram, the last argument is pointer to the method
calculating quantity of the distribution. The method creating the
distribution returns pointer to the created object, thus allowing
setting extra properties of the object e.g. axes labels.

For example if a new distribution with name `h_myHist` and title `My
Hist Title`, which should be rebooked every luminosity block and
filled with value returned by function `xAOD::AFPSiHit::myValue()` the
following code is needed:
@code{.cpp}
AFPSiLayerSummaryProfileBase* myHist = m_summaryManager->createSummaryHits(this, managedBookingLumiBlock, "h_myHist", "My Hist Title", &xAOD::AFPSiHit::myValue);
// the lines below set axis label
TProfile* prof = myHist->profile();
prof->SetYTitle("myHist Y-axis label");
@endcode

The distribution will be automatically filled with proper value in the method AFPSiLayerMonitor::fillHistograms().


@subsubsection newSummaryLayerDistributionEventEnd Distributions filled once per event per plane.

If the distribution should be filled **once per event** per plane a
method providing value used for filling the distribution must be
implemented in AFPSiLayerMonitor class and added to its interface
IAFPSiLayerMonitor. Next in AFPHitsMonitorTool::initialize() method
AFPHitsMonitorTool::m_summaryManager object should be used to create a
new summary distribution with method
AFPSiLayerSummaryManager::createSummaryEventEnd(). The first argument
should be `this`, the second is ManagedMonitorToolBase::MonGroup
object defining rebooking frequency (lumi-block or run) and in which
ROOT file directory the histogram should be saved, next are name and
title of the histogram, the last argument is pointer to the method
calculating quantity of the distribution. The method creating the
distribution returns pointer to the created object, thus allowing
setting extra properties of the object e.g. axes labels.

For example if a new distribution with name `h_myHist` and title `My
Hist Title`, which should be rebooked every luminosity block and
filled with value returned by function `AFPSiLayerMonitor::myValue()` the
following code is needed:
@code{.cpp}
AFPSiLayerSummaryProfileBase* myHist = m_summaryManager->createSummaryEventEnd(this, managedBookingLumiBlock, "h_myHist", "My Hist Title", &IAFPSiLayerMonitor::myValue);
// the lines below set axis label
TProfile* prof = myHist->profile();
prof->SetYTitle("myHist Y-axis label");
@endcode

The distribution will be automatically filled with proper value in the method AFPSiLayerMonitor::fillHistograms().

*/
