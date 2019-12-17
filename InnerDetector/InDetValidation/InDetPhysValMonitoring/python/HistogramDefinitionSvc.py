# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from ConfigUtils import setDefaults
#,dumpConfigurables

import InDetPhysValMonitoring.InDetPhysValMonitoringConf
def getHistogramDefinitionSvc(**kwargs) :
  '''
  Default HistogramDefinitionSvc
  '''
  kwargs = setDefaults(kwargs,
                       DefinitionSource="InDetPVMPlotDefRun2.xml",
                       DefinitionFormat="text/xml")
  return InDetPhysValMonitoring.InDetPhysValMonitoringConf.HistogramDefinitionSvc(**kwargs)

