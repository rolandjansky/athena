
/**
   @namespace Monitored
   @brief athenaMT monitoring infrastructure
   
   The Monitored namespace collects the infrastructure to create histograms from
   quantitities within an athena component. While this infrastrcucture has been
   created specifically for the use in athenaMT it is of course also usable in
   single-threaded athena. To make use of this infrastructure the following
   steps are needed:
   
   1) Add a GenericMonitoringTool instace to your component
   \code
      declareProperty("MonTool", m_monTool=VoidMonitoringTool(this), "Monitoring tool");
   \endcode
   Note that by default we are using an empty ToolHandle, i.e. there is no
   monitoring tool attached by default. This should instead be done in the
   python configuration of the component:
   \code
      from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool
      myalg.MonTool = GenericMonitoringTool('MonTool')
   \endcode

   2) Retrieve the monitoring tool during initialize() if the ToolHandle is not
   empty:
   \code
      if (!m_monTool.empty()) CHECK(m_monTool.retrieve());
   \endcode

   3) Declare the monitored quantities to the monitoring framework.
   Several classes are available to export different types to the monitoring
   framwork: See the detailed documentation of Monitored::MonitoredScalar,
   Monitored::MonitoredCollection or Monitored::MonitoredTimer. For example, a monitored scalar object
   is declared via Monitored::MonitoredScalar::declare(std::string name, const T&
   defaultVaule):

   @copydetails Monitored::MonitoredScalar::declare(std::string name, const T& defaultVaule)

   4) Group monitored variables within a Monitored::MonitoredScope that will take care of
   filling histogram once it goes out of scope:

   @copydoc Monitored::MonitoredScope

   5) Configure the list of histograms in python
   \code
      from AthenaMonitoring.GenericMonitoringTool import defineHistogram
      monTool.Histograms = [defineHistogram('nTracks', path='EXPERT', type='TH1F', title='Counts',
                                             xbins=10, xmin=0, xmax=10)]
   \endcode

   Additional documentation:
   - The MonitoredAlg standalone example and its MonitoredOptions.py job
   options
   - <a href="https://gitlab.cern.ch/atlas/athena/blob/master/Control/AthenaMonitoring/test/GenericMonFilling_test.cxx">GenericMonFilling_test.cxx</a>
   unit test for an example of all features.
 */


