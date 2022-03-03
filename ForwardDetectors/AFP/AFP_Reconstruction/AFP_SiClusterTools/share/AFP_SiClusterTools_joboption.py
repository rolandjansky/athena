# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================
# Job options file for the AFP_SiClusterTools package
#==============================================================

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from TrigEDMConfig.TriggerEDMRun3 import recordable

def AFP_SiClusterTools_Cfg(kwargs={}):
	
	afpGeometryTool = CompFactory.AFP_GeometryTool("AFP_Geometry_tool")
	
	clusterNeighbour = CompFactory.AFPSiClusterBasicNearestNeighbour("AFPSiClusterBasicNearestNeighbour")
# 	it's also possible to get the same clustering tool as in AFPAnalysisToolbox
# 	clusterNeighbour = CompFactory.AFPSiClusterAllNeighbours("AFPSiClusterAllNeighbours", neighbourhoodType="X")
	
	rowColToLocal = CompFactory.AFPSiRowColToLocalCSTool("AFPSiRowColToLocalCSTool", AFP_Geometry=afpGeometryTool)
	
	clusterTool = CompFactory.AFPSiClusterTool("AFPSiClusterTool", clusterAlgTool=clusterNeighbour, rowColToLocalCSTool = rowColToLocal)
	if ConfigFlags.Input.Format is Format.POOL and "AFPSiHitContainer" not in ConfigFlags.Input.Collections:
		clusterTool.AFPSiHitsContainerName=""
	
	return CompFactory.AFPSiCluster("AFPSiCluster", clusterRecoTool = clusterTool, **kwargs)


def AFP_SiClusterTools_HLT():
	
	AFP_SiCl = AFP_SiClusterTools_Cfg({"AFPSiHitsClusterContainerKey": recordable("HLT_AFPSiHitsClusterContainer")})
	
	from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
	
	monTool_AFP_SiClusterTool = GenericMonitoringTool("MonTool_AFP_SiClusterTool")
	monTool_AFP_SiClusterTool.defineHistogram( 'HitsSize', path='EXPERT', type='TH1F', title='SID hits size',xbins=50, xmin=0, xmax=50 )
	monTool_AFP_SiClusterTool.defineHistogram( 'ClusterSize', path='EXPERT', type='TH1F', title='SID cluster size',xbins=50, xmin=0, xmax=50 )
	AFP_SiCl.clusterRecoTool.MonTool = monTool_AFP_SiClusterTool
	
	return AFP_SiCl


