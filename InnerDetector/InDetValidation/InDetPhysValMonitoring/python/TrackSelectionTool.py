# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ConfigUtils import injectNameArgument,checkKWArgs,_args,serviceFactory
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool

class InDetTrackSelectionTool(object) :
  '''
  Namespace for inner detector hole search tools
  '''
  def __init__(self) :
     raise('must not be instantiated. Only child classes should be instantiated.')
  
  class InDetTrackSelectionToolLoose(InDet__InDetTrackSelectionTool) :
      '''
      Default InDetTrackSelectionTool for InDetTrackParticles
      '''
      @injectNameArgument
      def __new__(cls, *args, **kwargs) :
          return InDet__InDetTrackSelectionTool.__new__(cls,*args,**kwargs)

      @checkKWArgs
      def __init__(self, **kwargs) :
          super(InDetTrackSelectionTool.InDetTrackSelectionToolLoose,self)\
                        .__init__(**_args( kwargs,
                                           name = self.__class__.__name__))

          # special parameters of the InDetTrackSelectionToolLoose
          self.CutLevel = "Loose"

  class InDetTrackSelectionToolTightPrimary(InDet__InDetTrackSelectionTool) :
      '''
      Default InDetTrackSelectionTool for InDetTrackParticles
      '''
      @injectNameArgument
      def __new__(cls, *args, **kwargs) :
          return InDet__InDetTrackSelectionTool.__new__(cls,*args,**kwargs)

      @checkKWArgs
      def __init__(self, **kwargs) :
          super(InDetTrackSelectionTool.InDetTrackSelectionToolTightPrimary,self)\
                        .__init__(**_args( kwargs,
                                           name = self.__class__.__name__))

          # special parameters of the InDetTrackSelectionToolTightPrimary
          self.CutLevel = "TightPrimary"
 

