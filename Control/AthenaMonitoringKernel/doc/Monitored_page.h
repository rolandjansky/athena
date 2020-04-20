/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
   @defgroup MonAPI athenaMT monitoring infrastructure

   Summary of the user-relevant Monitored interfaces.

   The Monitored namespace collects the infrastructure to create histograms from
   quantitities within an athena component. It is used in the HLT for online monitoring
   and the offline DQ framework. The core framework is documented here while more specific
   instructions can be found here:
   - <a href="https://atlassoftwaredocs.web.cern.ch/guides/trigger/monitoring">Online HLT monitoring</a>
   - <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/DQRun3FrameworkTutorial">Offline DQ monitoring</a>

   The core framework consists of the following classes:
   - Monitored::Scalar to monitor scalar (double or string) values,
   - Monitored::Collection to monitor collection of objects,
   - Monitored::Timer and Monitored::ScopedTimer to monitor time differences,
   - Monitored::Group to perform the histogram filling,
   - GenericMonitoringTool to configure the available histograms via GenericMonitoringTool.defineHistogram.
*/

/**
   @namespace Monitored
   Namespace for the @ref MonAPI
*/
