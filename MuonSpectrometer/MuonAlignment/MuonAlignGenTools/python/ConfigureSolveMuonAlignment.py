# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



## @file:   ConfigureSolveMuonAlignment.py
## @brief:  Creates SolveAlign algorithm with tools necessary 
#           for performing muon alignment.  All setups
#           are done using flags set beforehand, so it
#           is necessary to set the flags first.
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/23/2009
###  Modified on Mar-28-2010 -- Prolay Mal (prolay@physics.arizona.edu)
###             For L3 CSC alignment


from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign
from MuonAlignGenTools.MuonAlignmentIOFiles import MuonAlignmentIOFiles

## ConfigureSolveMuonAlignment calls other classes to create SolveAlign and 
#  reconfigure with tools specific to muon alignment.
class ConfigureSolveMuonAlignment() :

    ## initialize
    def __init__ ( self ) :
        
        # set defaults for muon alignment
        align.alignmentType = 'GlobalChi2'
        align.useSLFitter = True
        align.runOutlierRemoval = False
        
        self.setFileRootName()
        self.muonIO = MuonAlignmentIOFiles()

        if align.alignmentLevel() == 1 :
            self.setL1()
        elif align.alignmentLevel() == 3 :
            self.setL3()
        else:
            print "only L1 and L3 implemented"
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
        align.derivativeType = 'Numerical'
        align.alignmentLevel = 1        
        align.requireOverlap = False
        self.setFullReco(True)

        mualign.alignMDT              = True
        level1modules = []
        if mualign.doEndcap() :

            if mualign.endcapsToAlign=='A' or mualign.endcapsToAlign=='BOTH' :
                level1modules.append(0)                
            if mualign.endcapsToAlign=='C' or mualign.endcapsToAlign=='BOTH' :
                level1modules.append(1)

        if mualign.doBarrel() :
            level1modules.append(2)
            mlog.warning("this isn't really implemented yet!!!")

    ## sets AlignmentFlags and MuonAlignmentFlags for L3 alignment
    def setL3 ( self ) :
        if mualign.alignMDT():
            align.derivativeType = 'Numerical'
        elif mualign.alignCSC():
            align.derivativeType = 'Analytical'
        else:
            align.derivativeType = 'unknown'
        align.alignmentLevel = 3
        self.setFullReco(False)

    ## sets AlignmentFlags and MuonAlignmentFlags for full reconstruction
    def setFullReco ( self, doFullReco = True ) :
        mualign.useFullReco = doFullReco
        from AthenaCommon.DetFlags import DetFlags
        DetFlags.Muon_setOn()
        if doFullReco :             
            DetFlags.ID_setOn()
            # align.fitterType should be set by L1MdtEndcapOnlyFlags.py
            if align.fitterType == 'CombMuonTrackFitter' :
                align.useSLFitter = False
        else:
            align.fitterType = 'MCTB'

    ## sets AlignmentFlags and MuonAlignmentFlags for multilevel alignment
    def doMultiLevelAlignment(self) :
        doMultiLevelAlignment = False
        if align.alignmentLevel()!=3 : 
            if align.iteration()>0 or mualign.jobType()=='misal' :
                doMultiLevelAlignment = True
        mualign.doMultiLevelAlignment = doMultiLevelAlignment
        return doMultiLevelAlignment
    
    ## created configured algorithm
    def configureSolveAlignment ( self, useAlternateAscii=False ) :
        
        self.setAlignmentDbTool(useAlternateAscii)
        
        # get AlignAlg without muon-specific tools
        from TrkAlignGenAlgs.AlignSolveGetter import AlignSolveGetter
        solveAlign = AlignSolveGetter().AlignSolveHandle()
        
        # add muon-specific tools using current MuonAlignment flags
        
	# MuonGeometryManagerTool
        from MuonAlignGenTools.MuonGeometryManagerToolGetter import MuonGeometryManagerToolGetter
        solveAlign.GeometryManagerTool = MuonGeometryManagerToolGetter().MuonGeometryManagerToolHandle()
        
        # Csc/MdtAlignDBTool
        if mualign.alignMDT():            
            from MuonAlignGenTools.MdtAlignDBToolGetter import MdtAlignDBToolGetter
            solveAlign.AlignDBTool = MdtAlignDBToolGetter().MdtAlignDBToolHandle()
        elif mualign.alignCSC():
            from MuonAlignGenTools.CscAlignDBToolGetter import CscAlignDBToolGetter
            solveAlign.AlignDBTool = CscAlignDBToolGetter().CscAlignDBToolHandle()
        else:
            print '************ At the moment set to MdtAlignDBTool  *************'
            print '*********** but please make sure what you are doing ***********'
            from MuonAlignGenTools.MdtAlignDBToolGetter import MdtAlignDBToolGetter 
            solveAlign.AlignDBTool = MdtAlignDBToolGetter().MdtAlignDBToolHandle()

	# create MuonAlignModuleTool and configure tools
        from MuonAlignGenTools.MuonAlignModuleToolGetter import MuonAlignModuleToolGetter
        alignModTool = MuonAlignModuleToolGetter().MuonAlignModuleToolHandle()
        solveAlign.AlignTool.AlignModuleTool = alignModTool
        solveAlign.AlignTool.MatrixTool.AlignModuleTool = alignModTool
        
        # set matrix tool output to DEBUG
        solveAlign.AlignTool.MatrixTool.OutputLevel = 2

        # configure GlobalChi2AlignTool's output tree
        globalChi2AlignTool = solveAlign.AlignTool
        globalChi2AlignTool.DoShiftDerivTree = mualign.doShiftDerivTree()

	# add RegionSelectionSvc
        self.addRegionSelectionSvc()

   ## created configured algorithm
    def newConfigureSolveAlignment ( self ) :
        
        self.setAlignmentDbTool()
        
        # setup MCTB fitter (not used but needed for AlignTrackPreProcessor)

        # get AlignAlg without muon-specific tools
        from TrkAlignGenAlgs.AlignAlgGetter import AlignAlgGetter
        solveAlign = AlignAlgGetter().AlignAlgHandle()
        
        # add muon-specific tools using current MuonAlignment flags
        
	# MuonGeometryManagerTool
        from MuonAlignGenTools.MuonGeometryManagerToolGetter import MuonGeometryManagerToolGetter
        solveAlign.GeometryManagerTool = MuonGeometryManagerToolGetter().MuonGeometryManagerToolHandle()
        
        
	# Csc/MdtAlignDBTool
        if mualign.alignMDT():
            from MuonAlignGenTools.MdtAlignDBToolGetter import MdtAlignDBToolGetter
            solveAlign.AlignDBTool = MdtAlignDBToolGetter().MdtAlignDBToolHandle()
        elif mualign.alignCSC():
            from MuonAlignGenTools.CscAlignDBToolGetter import CscAlignDBToolGetter
            solveAlign.AlignDBTool = CscAlignDBToolGetter().CscAlignDBToolHandle()
        else:
            print '************ At the moment set to MdtAlignDBTool  *************'
            print '*********** but please make sure what you are doing ***********'
            from MuonAlignGenTools.MdtAlignDBToolGetter import MdtAlignDBToolGetter 
            solveAlign.AlignDBTool = MdtAlignDBToolGetter().MdtAlignDBToolHandle()
            
	# create MuonAlignModuleTool and configure tools
        from MuonAlignGenTools.MuonAlignModuleToolGetter import MuonAlignModuleToolGetter
        alignModTool = MuonAlignModuleToolGetter().MuonAlignModuleToolHandle()
        solveAlign.AlignTool.AlignModuleTool = alignModTool
        solveAlign.AlignTool.MatrixTool.AlignModuleTool = alignModTool
        
        # set matrix tool output to DEBUG
        solveAlign.AlignTool.MatrixTool.OutputLevel = 2

        # configure GlobalChi2AlignTool's output tree
        globalChi2AlignTool = solveAlign.AlignTool
        globalChi2AlignTool.DoShiftDerivTree = mualign.doShiftDerivTree()

	# add RegionSelectionSvc
        self.addRegionSelectionSvc()

        solveAlign.SolveOnly = True

    ## configure database tools and MuonDetectorTool for alignment
    def setAlignmentDbTool(self,useAlternateAscii) :

        from AtlasGeoModel.MuonGM import GeoModelSvc
        MuonDetectorTool = GeoModelSvc.DetectorTools[ "MuonDetectorTool" ]
        MuonDetectorTool.OutputLevel = align.OutputLevel()
        altASZTFile = ""
        if mualign.doMultiLevelAlignment():
            MuonDetectorTool.UseAsciiConditionData = 1            
            MuonDetectorTool.UseConditionDb = 1
        else :
            print "doMultiLevelAlignment=False"
        from MuonAlignGenTools.MuonAlignmentIOFiles import MuonAlignmentIOFiles
        if useAlternateAscii:
            altASZTFile = self.muonIO.asztInputFile()
            MuonDetectorTool.AlternateASZTFile = altASZTFile
        print MuonDetectorTool

        from MuonCondTool.MuonCondToolConf import MuonAlignmentDbTool
        MuonAlignmentDbTool = MuonAlignmentDbTool("MGM_AlignmentDbTool")
        MuonAlignmentDbTool.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
                                              "/MUONALIGN/MDT/ENDCAP/SIDEA",
                                              "/MUONALIGN/MDT/ENDCAP/SIDEC",
                                              "/MUONALIGN/TGC/SIDEA",
                                              "/MUONALIGN/TGC/SIDEC"]
        MuonAlignmentDbTool.OutputLevel = align.OutputLevel()
        
        from __main__ import ToolSvc
        ToolSvc += MuonAlignmentDbTool
        
        if mualign.doMultiLevelAlignment() :
            from MuonCondTool.MuonCondToolConf import MultiLevelAlignmentDbTool
            MultiLevelAlignmentDbTool = MultiLevelAlignmentDbTool("MultiLevelAlignmentDbTool")
            MultiLevelAlignmentDbTool.ABLineDbTool = ToolSvc.MGM_AlignmentDbTool
            MultiLevelAlignmentDbTool.L1AsciiFile = self.muonIO.inputLevel1File()
            MultiLevelAlignmentDbTool.OutputLevel = align.OutputLevel()
            ToolSvc += MultiLevelAlignmentDbTool
            
            MuonDetectorTool.TheMuonAlignmentTool = MultiLevelAlignmentDbTool
        else:
            MuonDetectorTool.TheMuonAlignmentTool = MuonAlignmentDbTool

    ## add RegionSelectionSvc
    def addRegionSelectionSvc(self) :
        from MuonCalibStandAloneBase.MuonCalibStandAloneBaseConf import RegionSelectionSvc
        RegionSelectionSvc = RegionSelectionSvc(PrintList = True)
        RegionSelectionSvc.Region = mualign.moduleSelectionString()
        
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr += RegionSelectionSvc
        print RegionSelectionSvc
        
        
