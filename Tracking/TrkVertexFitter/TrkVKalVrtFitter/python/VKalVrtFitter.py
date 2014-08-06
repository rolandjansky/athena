# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from GaudiKernel.GaudiHandles import *
from AthenaCommon.Configurable import *
from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ToolSvc
#
from TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
#
class InDetVKalVrtFitter( ConfigurableAlgTool ) :

    def __init__(self, name="InDetVKalVrtFitter"):        

        from __main__ import ToolSvc
        mlog = logging.getLogger( 'InDetVKalVrtFitter::__init__ ' )
        mlog.info("entering")

        Trk__TrkVKalVrtFitter.__init__( self,name="Trk__TrkVKalVrtFitter",
	                     Extrapolator="Trk::Extrapolator/InDetExtrapolator",
			     MagFieldSvc="Trk::MagneticFieldTool/InDetMagField" )
	   

class VKalFitterForAnalysis( ConfigurableAlgTool ) :

    def __init__(self, name="VKalFitterForAnalys"):        

        mlog = logging.getLogger( 'VKalFitterForAnalysis::__init__ ' )
        mlog.info("entering")

        Trk__TrkVKalVrtFitter.__init__( self,name="Trk__TrkVKalVrtFitter",
	                     Extrapolator="DefaultVKalPropagator",   # Internal VKal propagator
			     MagFieldSvc="DefaultMagneticField"    # Internal VKal field
			     )

class VKalFitterForAnalysisWithField( ConfigurableAlgTool ) :

    def __init__(self, name="VKalFitterForAnalysWithField"):        

        mlog = logging.getLogger( 'VKalFitterForAnalysisWithField::__init__ ' )
        mlog.info("entering")

        Trk__TrkVKalVrtFitter.__init__( self,name="Trk__TrkVKalVrtFitter",
	                     Extrapolator="DefaultVKalPropagator",      # internal VKal propagatorcd 
			     MagFieldSvc="DefaultMagneticField",  
			     MagFieldAtlasService="MagFieldAthenaSvc" # ATLAS mag field 
			     )
