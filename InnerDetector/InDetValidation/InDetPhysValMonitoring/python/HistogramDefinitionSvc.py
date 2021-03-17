# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from InDetRecExample.TrackingCommon import setDefaults


def getHistogramDefinitionSvc(**kwargs):
    import InDetPhysValMonitoring.InDetPhysValMonitoringConf
    kwargs = setDefaults(kwargs,
                         DefinitionSource="InDetPVMPlotDefRun2.xml",
                         DefinitionFormat="text/xml")
    return InDetPhysValMonitoring.InDetPhysValMonitoringConf.HistogramDefinitionSvc(**kwargs)
