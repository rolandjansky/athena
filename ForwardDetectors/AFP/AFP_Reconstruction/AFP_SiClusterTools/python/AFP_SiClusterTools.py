# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================
# Job options file for the AFP_SiClusterTools package
#==============================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format
from TrigEDMConfig.TriggerEDMRun3 import recordable

def AFP_SiClusterTools_Cfg(flags, kwargs={}):

        afpGeometryTool = CompFactory.AFP_GeometryTool("AFP_Geometry_tool")

        clusterNeighbour = CompFactory.AFPSiClusterBasicNearestNeighbour("AFPSiClusterBasicNearestNeighbour")
        #it's also possible to get the same clustering tool as in AFPAnalysisToolbox
        #clusterNeighbour = CompFactory.AFPSiClusterAllNeighbours("AFPSiClusterAllNeighbours", neighbourhoodType="X")

        rowColToLocal = CompFactory.AFPSiRowColToLocalCSTool("AFPSiRowColToLocalCSTool", AFP_Geometry=afpGeometryTool)
        
        # prepare Si cluster reco
        clusterTool = CompFactory.AFPSiClusterTool("AFPSiClusterTool", clusterAlgTool=clusterNeighbour, rowColToLocalCSTool = rowColToLocal)
        if flags.Input.Format is Format.POOL:
                if "AFPSiHitContainer" not in flags.Input.Collections:
                        clusterTool.AFPSiHitsContainerName=""
                else:
                        from AthenaCommon.AlgSequence import AlgSequence
                        topSequence = AlgSequence()
                        if hasattr(topSequence,'SGInputLoader'):
                                topSequence.SGInputLoader.Load += [('xAOD::AFPSiHitContainer' , 'StoreGateSvc+AFPSiHitContainer')]
        
        # actually setup the Si cluster reco
        acc = ComponentAccumulator()
        acc.addEventAlgo(CompFactory.AFPSiCluster("AFPSiCluster", clusterRecoTool = clusterTool, **kwargs))
        
        return acc


def AFP_SiClusterTools_HLT(flags):

        acc = AFP_SiClusterTools_Cfg(flags,{"AFPSiHitsClusterContainerKey": recordable("HLT_AFPSiHitsClusterContainer")})
        AFP_SiCl = acc.getEventAlgo("AFPSiCluster")
        
        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

        monTool_AFP_SiClusterTool = GenericMonitoringTool("MonTool_AFP_SiClusterTool")
        monTool_AFP_SiClusterTool.defineHistogram( 'HitsSize', path='EXPERT', type='TH1F', title='SID hits size',xbins=50, xmin=0, xmax=50 )
        monTool_AFP_SiClusterTool.defineHistogram( 'ClusterSize', path='EXPERT', type='TH1F', title='SID cluster size',xbins=50, xmin=0, xmax=50 )
        AFP_SiCl.clusterRecoTool.MonTool = monTool_AFP_SiClusterTool

        return AFP_SiCl
