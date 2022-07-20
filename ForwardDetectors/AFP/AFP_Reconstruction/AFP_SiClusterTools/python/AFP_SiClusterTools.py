# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================
# Job options file for the AFP_SiClusterTools package
#==============================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format
from IOVDbSvc.IOVDbSvcConfig import addFolders
from TrigEDMConfig.TriggerEDMRun3 import recordable


def AFP_SiClusterTools_Cfg(flags, kwargs={}):
        
        # original tool from Athena
        # clusterNeighbour = CompFactory.AFPSiClusterBasicNearestNeighbour("AFPSiClusterBasicNearestNeighbour")
        # it's also possible to get the same clustering tool as in AFPAnalysisToolbox
        clusterNeighbour = CompFactory.AFPSiClusterAllNeighbours("AFPSiClusterAllNeighbours", neighbourhoodType="X")
        
        rowColToLocal = CompFactory.AFPSiRowColToLocalCSTool("AFPSiRowColToLocalCSTool")
        
        acc = ComponentAccumulator()
        acc.addCondAlgo(CompFactory.CondInputLoader(Load=[('CondAttrListCollection', 'ConditionStore+/FWD/AFP/Align/Global'),
                                                          ('CondAttrListCollection', 'ConditionStore+/FWD/AFP/Align/Local')]))
                                                                  
        if flags.Input.isMC:
                # if other tags are desired, they can be set also in postExec, e.g.
                # --postExec 'from IOVDbSvc.CondDB import conddb; conddb.addOverride("/FWD/AFP/Align/Local","AFPMCAlignLoc-329484-02"); conddb.addOverride("/FWD/AFP/Align/Global","AFPMCAlignGlob-331020-01");'

                acc.merge(addFolders(flags, "/FWD/AFP/Align/Local<tag>AFPMCAlignLoc-ideal-01</tag>",  'FWD_OFL', className='CondAttrListCollection', db='OFLP200'))
                acc.merge(addFolders(flags, "/FWD/AFP/Align/Global<tag>AFPMCAlignGlob-ideal-01</tag>",'FWD_OFL', className='CondAttrListCollection', db='OFLP200'))
                
                siLocAlignTool  = CompFactory.getComp("AFP::SiLocAlignDBTool")("SiLocAlignDBTool",  loc_align_key="/FWD/AFP/Align/Local")
                siGlobAlignTool = CompFactory.getComp("AFP::SiGlobAlignDBTool")("SiGlobAlignDBTool",glob_align_key="/FWD/AFP/Align/Global")
        else:
                acc.merge(addFolders(flags, "/FWD/Onl/AFP/Align/Local<tag>AFPAlignLoc-03</tag>",  'FWD_ONL', className='CondAttrListCollection', db='CONDBR2'))
                acc.merge(addFolders(flags, "/FWD/Onl/AFP/Align/Global<tag>AFPAlignGlob-03</tag>",'FWD_ONL', className='CondAttrListCollection', db='CONDBR2'))
                
                siLocAlignTool  = CompFactory.getComp("AFP::SiLocAlignDBTool")("SiLocAlignDBTool",  loc_align_key="/FWD/Onl/AFP/Align/Local")
                siGlobAlignTool = CompFactory.getComp("AFP::SiGlobAlignDBTool")("SiGlobAlignDBTool",glob_align_key="/FWD/Onl/AFP/Align/Global")
        
        # prepare Si cluster reco
        clusterTool = CompFactory.AFPSiClusterTool("AFPSiClusterTool", clusterAlgTool=clusterNeighbour, rowColToLocalCSTool = rowColToLocal, siLocAlignDBTool=siLocAlignTool, siGlobAlignDBTool=siGlobAlignTool)
        
        if flags.Input.Format is Format.POOL:
                if "AFPSiHitContainer" not in flags.Input.Collections:
                        clusterTool.AFPSiHitsContainerName=""
                else:
                        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
                        acc.merge(SGInputLoaderCfg(flags,Load=[('xAOD::AFPSiHitContainer','StoreGateSvc+AFPSiHitContainer')]))
        
        acc.addEventAlgo(CompFactory.AFPSiCluster("AFPSiCluster", clusterRecoTool = clusterTool, **kwargs))
        
        return acc


def AFP_SiClusterTools_HLT(flags):

        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        acc = ComponentAccumulator()
        acc.merge(AFP_SiClusterTools_Cfg(flags,{"AFPSiHitsClusterContainerKey": recordable("HLT_AFPSiHitsClusterContainer")}))
        
        from IOVDbSvc.CondDB import conddb
        if flags.Input.isMC:
                if not conddb.folderRequested('/FWD/AFP/Align/Local'):
                        conddb.addFolder("FWD_OFL","/FWD/AFP/Align/Local<tag>AFPMCAlignLoc-ideal-01</tag>", className="CondAttrListCollection")
                if not conddb.folderRequested('/FWD/AFP/Align/Global'):
                        conddb.addFolder("FWD_OFL","/FWD/AFP/Align/Global<tag>AFPMCAlignGlob-ideal-01</tag>",className="CondAttrListCollection")
        else:
                if not conddb.folderRequested('/FWD/Onl/AFP/Align/Local'):
                        conddb.addFolder("FWD_ONL","/FWD/Onl/AFP/Align/Local<tag>AFPAlignLoc-03</tag>", className="CondAttrListCollection")
                if not conddb.folderRequested('/FWD/Onl/AFP/Align/Global'):
                        conddb.addFolder("FWD_ONL","/FWD/Onl/AFP/Align/Global<tag>AFPAlignGlob-03</tag>",className="CondAttrListCollection")
        
        AFP_SiCl = acc.getEventAlgo("AFPSiCluster")
        
        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

        monTool_AFP_SiClusterTool = GenericMonitoringTool("MonTool_AFP_SiClusterTool")
        monTool_AFP_SiClusterTool.defineHistogram( 'HitsSize', path='EXPERT', type='TH1F', title='SID hits size',xbins=50, xmin=0, xmax=50 )
        monTool_AFP_SiClusterTool.defineHistogram( 'ClusterSize', path='EXPERT', type='TH1F', title='SID cluster size',xbins=50, xmin=0, xmax=50 )
        AFP_SiCl.clusterRecoTool.MonTool = monTool_AFP_SiClusterTool

        return acc

