# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



## @file:   ConfigureMuonAlignment.py
## @brief:  Creates algorithms with tools necessary 
#           for performing muon alignment.  All setups
#           are done using flags set beforehand, so it
#           is necessary to set the flags first.
## @author: Robert Harrington <roberth@cern.ch>
## @date:   10/19/2009

from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign
from MuonAlignGenTools.MuonAlignmentIOFiles import MuonAlignmentIOFiles
from GaudiKernel.Constants import VERBOSE,INFO,DEBUG

## ConfigureMuonAlignment calls other classes to create AlignAlg and 
#  reconfigure with tools specific to muon alignment.
class ConfigureMuonAlignment() :

    ## constructor
    def __init__ ( self ) :
        
        # set defaults for muon alignment
        align.alignmentType = 'GlobalChi2'
        align.useSLFitter = True
        align.runOutlierRemoval = False

        if not align.fileRootName.is_locked() :
            self.setFileRootName()
        if not align.outputPath.is_locked() :
            align.outputPath = align.fileRootName()
        if not mualign.asztFilePath.is_locked() :
            mualign.asztFilePath = align.fileRootName()

        #self.muonIO = MuonAlignmentIOFiles()
        #self.muonIO.setInitialAsztFilePath(mualign.initialAsztFilePath())
        #self.muonIO.setAsztFilePath(mualign.asztFilePath())

        if align.alignmentLevel() == 1 :
            self.setL1()
        elif align.alignmentLevel() == 23 :
            self.setL23()
        elif align.alignmentLevel() == 3 :
            self.setL3()
        else:
            print "only L1, L2 and L3 implemented"
            exit

    ## sets root filename from aligflags
    def setFileRootName(self) :
        fileroot=str(align.alignmentLevel())+"_"+align.dataType()
        if mualign.jobType()!="":
            fileroot = fileroot + "_" + mualign.jobType()
        align.fileRootName = fileroot
        print "in ConfigMuonAlignment, fileroot=",fileroot

    ## sets AlignmentFlags and MuonAlignmentFlags for L1 alignment
    def setL1 ( self ) :
        #align.derivativeType = 'Numerical'
        align.alignmentLevel = 1        
        align.requireOverlap = False
        #self.setFullReco(True)
        
        level1modules = []
        if mualign.doEndcap() :
            
            if mualign.endcapsToAlign=='A' or mualign.endcapsToAlign=='BOTH' :
                level1modules.append('ECA')                
            if mualign.endcapsToAlign=='C' or mualign.endcapsToAlign=='BOTH' :
                level1modules.append('ECC')

        if mualign.doBarrel() :
            level1modules.append('Bar')
            #mlog.warning("this isn't really implemented yet!!!")

    ## sets AlignmentFlags and MuonAlignmentFlags for L2.3 alignment
    def setL23 ( self ) :
        align.alignmentLevel = 23

    ## sets AlignmentFlags and MuonAlignmentFlags for L3 alignment
    def setL3 ( self ) :
        #if mualign.alignMDT():
        #    align.derivativeType = 'Numerical'
        #elif mualign.alignCSC():
        #    align.derivativeType = 'Analytical'
        #else:
        #    align.derivativeType = 'unknown'
        align.alignmentLevel = 3
        #self.setFullReco(False)


    ## sets AlignmentFlags and MuonAlignmentFlags for full reconstruction
    #def setFullReco ( self, doFullReco = True ) :
    #    mualign.useFullReco = doFullReco
    #    if doFullReco : 
    #        # align.fitterType should be set by Flags.py
    #        if align.fitterType == 'CombMuonTrackFitter' :
    #            align.useSLFitter = False
    #    else:
    #        align.fitterType = 'MCTB'
    
    ## created configured algorithm
    def configureAlignment ( self, useAlternateAscii=False, isMC=True) :
        
        print "in configureAlignment"

        self.setAlignmentDbTool(useAlternateAscii,isMC)
        
        # get AlignAlg without muon-specific tools
        from TrkAlignGenAlgs.AlignAlgGetter import AlignAlgGetter
        alignAlg = AlignAlgGetter().AlignAlgHandle()
        
        # add muon-specific tools using current MuonAlignment flags
        
        # MuonTrackCollectionProvider
        if mualign.trackCollectionProvider()=='MuonTrackCollectionProvider':
            from MuonAlignGenTools.MuonTrackCollectionProviderGetter import MuonTrackCollectionProviderGetter
            alignAlg.TrackCollectionProvider = MuonTrackCollectionProviderGetter().MuonTrackCollectionProviderHandle()

            # create instance of MuonMatchQuality
            #from MuonTrackThroughCalo.ConfMuonTrackThroughCalo import ConfMuonTrackThroughCalo
            #MuonTTC = ConfMuonTrackThroughCalo("MuonTrackThroughCalo")

            from AthenaCommon.AppMgr import ToolSvc
            #ToolSvc += MuonTTC

            from MuonRecExample import MuonRecTools            
            muonCombiTrackMaker = MuonRecTools.getPublicTool('MooTrackSteering')
            #muonCombiTrackMaker.OutputLevel = DEBUG
            muonCombiTrackMaker.OutputLevel = align.OutputLevel()

            atlasExtrapolator=MuonRecTools.getPublicTool('AtlasExtrapolator')
            from MuonCombinedTestTools.MuonCombinedTestToolsConf import MuonCombined__MuonTrackTagTestTool
            combinedMuonTag = MuonCombined__MuonTrackTagTestTool(name = "CombinedMuonTag",
                                                                 ExtrapolatorTool = atlasExtrapolator,
                                                                 #TTCTool          = MuonTTC,
                                                                 Chi2Cut          = 2500.)
            ToolSvc += combinedMuonTag

            alignAlg.TrackCollectionProvider.TagTool = combinedMuonTag

        # create tool instance
        elif mualign.trackCollectionProvider()=='TrackCollectionProvider':
            from TrkAlignGenTools.TrackCollectionProviderGetter import TrackCollectionProviderGetter
            alignAlg.TrackCollectionProvider = TrackCollectionProviderGetter().TrackCollectionProviderGetterHandle()
        else:
            print "Using default TrackCollectionProvider!"

	# MuonTrackPreProcessor
        from MuonAlignGenTools.MuonTrackPreProcessorGetter import MuonTrackPreProcessorGetter
        alignAlg.AlignTrackPreProcessor = MuonTrackPreProcessorGetter().MuonTrackPreProcessorHandle()
        
	# MuonGeometryManagerTool
        from MuonAlignGenTools.MuonGeometryManagerToolGetter import MuonGeometryManagerToolGetter
        alignAlg.GeometryManagerTool = MuonGeometryManagerToolGetter().MuonGeometryManagerToolHandle()
        
	# MuonFillNtupleTool
        from MuonAlignGenTools.MuonFillNtupleToolGetter import MuonFillNtupleToolGetter
        alignAlg.FillNtupleTool = MuonFillNtupleToolGetter().MuonFillNtupleToolHandle()
        
	# MdtAlignDBTool
        if mualign.alignMDT():
            from MuonAlignGenTools.MdtAlignDBToolGetter import MdtAlignDBToolGetter
            alignAlg.AlignDBTool = MdtAlignDBToolGetter().MdtAlignDBToolHandle()
        elif mualign.alignCSC():
            from MuonAlignGenTools.CscAlignDBToolGetter import CscAlignDBToolGetter
            alignAlg.AlignDBTool = CscAlignDBToolGetter().CscAlignDBToolHandle()
        else:
            print '************ At the moment set to MdtAlignDBTool  *************'
            print '*********** but please make sure what you are doing ***********'
            from MuonAlignGenTools.MdtAlignDBToolGetter import MdtAlignDBToolGetter 
            alignAlg.AlignDBTool = MdtAlignDBToolGetter().MdtAlignDBToolHandle()

	# create MuonAlignModuleTool and configure tools
        from MuonAlignGenTools.MuonAlignModuleToolGetter import MuonAlignModuleToolGetter
        alignModTool = MuonAlignModuleToolGetter().MuonAlignModuleToolHandle()
        alignAlg.AlignTool.AlignModuleTool = alignModTool
        alignAlg.AlignTool.MatrixTool.AlignModuleTool = alignModTool
        alignAlg.AlignTrackCreator.AlignModuleTool = alignModTool
        alignAlg.AlignTrackDresser.DerivCalcTool.AlignModuleTool = alignModTool
        
        # set matrix tool output to DEBUG
        alignAlg.AlignTool.MatrixTool.OutputLevel = 2
        
        # configure GlobalChi2AlignTool's output tree
        globalChi2AlignTool = alignAlg.AlignTool
        
	# add RegionSelectionSvc
        self.addRegionSelectionSvc()

    ## configure database tools and MuonDetectorTool for alignment
    def setAlignmentDbTool(self,useAlternateAscii,isMC) :

        from AtlasGeoModel.MuonGM import GeoModelSvc
        MuonDetectorTool = GeoModelSvc.DetectorTools[ "MuonDetectorTool" ]
        MuonDetectorTool.OutputLevel = align.OutputLevel()
        #MuonDetectorTool.OutputLevel = INFO

        print "in setAlignmentDbTool, useAlternateAscii=",useAlternateAscii
        if isMC:

            #altASZTFile = ""
            if mualign.doMultiLevelAlignment():
                MuonDetectorTool.UseAsciiConditionData = 1            
                MuonDetectorTool.UseConditionDb = 0
            from MuonAlignGenTools.MuonAlignmentIOFiles import MuonAlignmentIOFiles
            if useAlternateAscii:
              #altASZTFile = self.muonIO.asztInputFile()
                MuonDetectorTool.AlternateASZTFile = "asztInput.txt"
                MuonDetectorTool.UseAsciiConditionData = 1            

        # get MuonAlignmentDbTool if already configured, create one if not
        from __main__ import ToolSvc
        try:
            muonAlignmentDbTool = ToolSvc.MGM_AlignmentDbTool
        except:
            from MuonCondTool.MuonCondToolConf import MuonAlignmentDbTool
            muonAlignmentDbTool = MuonAlignmentDbTool("MGM_AlignmentDbTool")
            ToolSvc += muonAlignmentDbTool
            MuonDetectorTool.TheMuonAlignmentTool = muonAlignmentDbTool
            
        muonAlignmentDbTool.OutputLevel = align.OutputLevel()
        #muonAlignmentDbTool.OutputLevel = DEBUG
        muonAlignmentDbTool.DumpALines = False
        if isMC and useAlternateAscii and align.alignmentLevel == 3:
            muonAlignmentDbTool.ALinesFile = "asztInput.txt"
        if not isMC:
            muonAlignmentDbTool.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
                                                  "/MUONALIGN/MDT/ENDCAP/SIDEA",
                                                  "/MUONALIGN/MDT/ENDCAP/SIDEC",
                                                  "/MUONALIGN/TGC/SIDEA",
                                                  "/MUONALIGN/TGC/SIDEC"]
        print "doMultiLevelAlignment: ",mualign.doMultiLevelAlignment()

        if mualign.doMultiLevelAlignment() :
            from MuonCondTool.MuonCondToolConf import MultiLevelAlignmentDbTool
            MultiLevelAlignmentDbTool = MultiLevelAlignmentDbTool("MultiLevelAlignmentDbTool")
            MultiLevelAlignmentDbTool.ABLineDbTool = muonAlignmentDbTool
            if align.alignmentLevel == 1:
                MultiLevelAlignmentDbTool.L1AsciiFile = "level1input.txt"
            elif align.alignmentLevel == 23:
                MultiLevelAlignmentDbTool.L23AsciiFile = "level23input.txt"
            elif align.alignmentLevel == 3:
                MultiLevelAlignmentDbTool.L3AsciiFile = "asztInput.txt"
            else:
                print "*** Alignment level: ", align.alignmentLevel,\
                      "not yet implemented in MultiLevelAlignmentDbTool ***"
            MultiLevelAlignmentDbTool.OutputLevel = align.OutputLevel()
            #MultiLevelAlignmentDbTool.OutputLevel = INFO

            #MultiLevelAlignmentDbTool.MuonIdHelperTool = ToolSvc.MuonIdHelperTool
            ToolSvc += MultiLevelAlignmentDbTool
            
            MuonDetectorTool.TheMuonAlignmentTool = MultiLevelAlignmentDbTool

        #from IOVDbSvc.CondDB import conddb
        #conddb.addOverride("/MUONALIGN/MDT/ENDCAP/SIDEA","MuonAlignMDTEndCapAAlign-2010-01-02")
        #conddb.addOverride("/MUONALIGN/MDT/ENDCAP/SIDEC","MuonAlignMDTEndCapCAlign-2010-01-02")

        print ToolSvc

    ## add RegionSelectionSvc
    def addRegionSelectionSvc(self) :
        from MuonCalibStandAloneBase.MuonCalibStandAloneBaseConf import RegionSelectionSvc
        RegionSelectionSvc = RegionSelectionSvc(PrintList = True)
        RegionSelectionSvc.Region = mualign.chamberSelectionString()
        
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr += RegionSelectionSvc
        print RegionSelectionSvc
        
        
