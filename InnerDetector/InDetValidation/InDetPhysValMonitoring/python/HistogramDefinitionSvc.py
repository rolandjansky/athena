# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ConfigUtils import injectNameArgument,checkKWArgs,_args
#,dumpConfigurables

import InDetPhysValMonitoring.InDetPhysValMonitoringConf
class HistogramDefinitionSvc(object) :
  '''
  Namespace for Histogram definition services
  '''
  def __init__(self) :
     raise('must not be instantiated. Only child classes should be instantiated.')

  class HistogramDefinitionSvc(InDetPhysValMonitoring.InDetPhysValMonitoringConf.HistogramDefinitionSvc) :
      '''
      Default HistogramDefinitionSvc 
      '''
      @injectNameArgument
      def __new__(cls, *args, **kwargs) :
          return InDetPhysValMonitoring.InDetPhysValMonitoringConf.HistogramDefinitionSvc.__new__(cls,*args,**kwargs)

      @checkKWArgs
      def __init__(self, **kwargs) :
          super(HistogramDefinitionSvc.HistogramDefinitionSvc,self)\
                        .__init__(**_args( kwargs,
                                           name = self.__class__.__name__))

          # special parameters of the default HistogramDefinitionSvc
          self.DefinitionSource="InDetPVMPlotDefRun2.xml"
          self.DefinitionFormat="text/xml"

