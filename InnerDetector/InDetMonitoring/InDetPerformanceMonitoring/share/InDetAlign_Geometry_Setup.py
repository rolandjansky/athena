##############################
#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
#############################
########################################################################################
#
#  This script sets up the geometry manager tool and the database tool for
#  the alignment framework based on the alignment requirements.
#
#  As a result three tools are setup which can be used in the definition of
#  Trk::AlignAlg and some supporting tools :
#
#    alignModuleTool
#    trkAlignGeoManagerTool
#    trkAlignDBTool
#
#  Started:  Daniel Kollar <daniel.kollar@cern.ch>  (09/2009)
#
########################################################################################
## common options


newInDetAlignGeo_Options = {
    "outputLevel" : DEBUG                 #  output level for log messages for all tools and algs
    ,"writeTextFiles" :  True            #  whether to write out text files with final constants
    ,"siliconTextFile" :  "OutputSiAlignment.txt"  #  name of the output text file with Si constants
    ,"trtTextFile" :  "OutputTRTAlignment.txt"     #  name of the output text file with TRT constants
    ,"updateConstants" : True            #  whether to update the constants in the DB
    ,"writeOldConstants" : True          #  whether to write out text files with initial constants
    ,"alignInDet" :  True                #  whether to align the full Inner Detector
    ,"alignSilicon" :  True              #  whether to align Si
    ,"alignPixel" :  True                #  whether to align Pixel
    ,"pixelAlignDBM" :  False            #  whether to align DBM instead of actual Pixel detector
    ,"alignSCT" :  True                  #  whether to align SCT
    ,"alignTRT" :  True                  #  whether to align TRT
    ,"doModuleSelection" : False         #  whether to align only selected modules
    ,"moduleSelection" : []              #  identifiers of selected modules
}

print (" == NewInDetAlignGeometrySetup == ** START **")
print (" == NewInDetAlignGeometrySetup == updateConstants: ", newInDetAlignGeo_Options["updateConstants"])
print (" == NewInDetAlignGeometrySetup == doModuleSelection: ", newInDetAlignGeo_Options["doModuleSelection"])
print (" == NewInDetAlignGeometrySetup == writeTextFiles: ", newInDetAlignGeo_Options["writeTextFiles"])

for var in newInDetAlignGeo_Options:
    if var in dir():
        newInDetAlignGeo_Options[var] = eval(var)

########################################################################################
## TRT Alignment Options
trtAlignment_Options = {
    # setup alignment geometry for TRT
    # if level for Barrel or Endcap is set to -1 then it is set to the level
    # set up for the whole TRT
    "trtAlignmentLevelBarrel" : -1       #  alignment level for the TRT barrel
    ,"trtAlignmentLevelEndcaps" : -1     #  alignment level for the TRT endcaps
    ,"trtAlignmentLevel" : 1             #  alignment level for the whole TRT

    # switch on or off of the whole TRT barrel or both end-caps
    ,"trtAlignBarrel" : True             #  whether to align TRT barrel
    ,"trtAlignEndcaps" : True            #  whether to align TRT endcap

    # switch on or off the alignment of the individual degrees-of-freedom
    ,"trtAlignBarrelX" : True            #  whether to align TRT barrel wrt. translations along X
    ,"trtAlignBarrelY" : True            #  whether to align TRT barrel wrt. translations along Y
    ,"trtAlignBarrelZ" : True            #  whether to align TRT barrel wrt. translations along Z
    ,"trtAlignBarrelRotX" : True         #  whether to align TRT barrel wrt. rotations around X
    ,"trtAlignBarrelRotY" : True         #  whether to align TRT barrel wrt. rotations around Y
    ,"trtAlignBarrelRotZ" : True         #  whether to align TRT barrel wrt. rotations around Z
    ,"trtAlignEndcapX" : True            #  whether to align TRT endcaps wrt. translations along X
    ,"trtAlignEndcapY" : True            #  whether to align TRT endcaps wrt. translations along Y
    ,"trtAlignEndcapZ" : True            #  whether to align TRT endcaps wrt. translations along Z
    ,"trtAlignEndcapRotX" : True         #  whether to align TRT endcaps wrt. rotations around X
    ,"trtAlignEndcapRotY" : True         #  whether to align TRT endcaps wrt. rotations around Y
    ,"trtAlignEndcapRotZ" : True         #  whether to align TRT endcaps wrt. rotations around Z

   # sigma for a given DoF is used for scaling for vector and matrix entries
   # for that DoF before solving
    ,"trtSetSigmaBarrelX" : 1            #  scaling constant for translations along X in TRT barrel
    ,"trtSetSigmaBarrelY" : 1            #  scaling constant for translations along Y in TRT barrel
    ,"trtSetSigmaBarrelZ" : 1            #  scaling constant for translations along Z in TRT barrel
    ,"trtSetSigmaBarrelRotX" : 0.001     #  scaling constant for rotations around X in TRT barrel
    ,"trtSetSigmaBarrelRotY" : 0.001     #  scaling constant for rotations around Y in TRT barrel
    ,"trtSetSigmaBarrelRotZ" : 0.001     #  scaling constant for rotations around Z in TRT barrel
    ,"trtSetSigmaEndcapX" : 1            #  scaling constant for translations along X in TRT endcaps
    ,"trtSetSigmaEndcapY" : 1            #  scaling constant for translations along Y in TRT endcaps
    ,"trtSetSigmaEndcapZ" : 1            #  scaling constant for translations along Z in TRT endcaps
    ,"trtSetSigmaEndcapRotX" : 0.001     #  scaling constant for rotations around X in TRT endcaps
    ,"trtSetSigmaEndcapRotY" : 0.001     #  scaling constant for rotations around Y in TRT endcaps
    ,"trtSetSigmaEndcapRotZ" : 0.001     #  scaling constant for rotations around Z in TRT endcaps

    # soft cut for a given DoF is multiplied by the overall soft-mode-cut
    # which is set in the NewInDetAlignAlgs.py and then added to the
    # diagonal element of the matrix for that DoF;
    # the addition happens after the scaling
    ,"trtSetSoftCutBarrelX" : 0.1          #  soft cut for translations along X in TRT barrel
    ,"trtSetSoftCutBarrelY" : 0.1          #  soft cut for translations along Y in TRT barrel
    ,"trtSetSoftCutBarrelZ" : 0.1        #  soft cut for translations along Z in TRT barrel
    ,"trtSetSoftCutBarrelRotX" : 0.05       #  soft cut for rotations around X in TRT barrel
    ,"trtSetSoftCutBarrelRotY" : 0.05       #  soft cut for rotations around Y in TRT barrel
    ,"trtSetSoftCutBarrelRotZ" : 0.05       #  soft cut for rotations around Z in TRT barrel
    ,"trtSetSoftCutEndcapX" : 0.1          #  soft cut for translations along X in TRT endcaps
    ,"trtSetSoftCutEndcapY" : 0.1          #  soft cut for translations along Y in TRT endcaps
    ,"trtSetSoftCutEndcapZ" : 0.001          #  soft cut for translations along Z in TRT endcaps
    ,"trtSetSoftCutEndcapRotX" : 0.05       #  soft cut for rotations around X in TRT endcaps
    ,"trtSetSoftCutEndcapRotY" : 0.05       #  soft cut for rotations around Y in TRT endcaps
    ,"trtSetSoftCutEndcapRotZ" : 0.05       #  soft cut for rotations around Z in TRT endcaps

    #trt module selection
    ,"trtAlignBarrelPhiSectors" : []     #  selection of phi sectors in the TRT barrel
    ,"trtAlignBarrelLayers" : []         #  selection of layers in the TRT barrel
    ,"trtAlignEndcapPhiSectors" : []     #  selection of phi sectors in the TRT endcap
    ,"trtAlignEndcapWheels" : []         #  selection of wheels in the TRT endcap
    ,"trtAlignSingleEndcap" : 0          #  Align single endcap option (-2 is C, 2 is A, 0 is both (default))
    #Level 3 module selection
}

for var in trtAlignment_Options:
  if var in dir():
    trtAlignment_Options[var] = eval(var)

#
# The options for Pixel and SCT are analogical to those for TRT.
#
########################################################################################
## Pixel Alignment Options
pixelAlignment_Options = {
    "pixelAlignmentLevelBarrel" : -1
    ,"pixelAlignmentLevelEndcaps": -1
    ,"pixelAlignmentLevel" : 1
#   ,"pixelAlignDBM" :  False            #  whether to align DBM instead of actual Pixel detector
        ,"pixelAlignBarrel" : True
    ,"pixelAlignEndcaps" : True
    ,"pixelAlignBarrelX" : True
    ,"pixelAlignBarrelY" : True
    ,"pixelAlignBarrelZ" : True
    ,"pixelAlignBarrelRotX" : True
    ,"pixelAlignBarrelRotY" : True
    ,"pixelAlignBarrelRotZ" : True
    ,"pixelAlignBarrelBowX" : False
    ,"pixelAlignEndcapX" : True
    ,"pixelAlignEndcapY" : True
    ,"pixelAlignEndcapZ" : True
    ,"pixelAlignEndcapRotX" : True
    ,"pixelAlignEndcapRotY" : True
    ,"pixelAlignEndcapRotZ" : True
    ,"pixelSetSigmaBarrelX" : 1
    ,"pixelSetSigmaBarrelY" : 1
    ,"pixelSetSigmaBarrelZ" : 1
    ,"pixelSetSigmaBarrelRotX" : 0.001
    ,"pixelSetSigmaBarrelRotY" : 0.001
    ,"pixelSetSigmaBarrelRotZ" : 0.001
    ,"pixelSetSigmaEndcapX" : 1
    ,"pixelSetSigmaEndcapY" : 1
    ,"pixelSetSigmaEndcapZ" : 1
    ,"pixelSetSigmaEndcapRotX" : 0.001
    ,"pixelSetSigmaEndcapRotY" : 0.001
    ,"pixelSetSigmaEndcapRotZ" : 0.001
    ,"pixelSetSoftCutBarrelX" : 0.02
    ,"pixelSetSoftCutBarrelY" : 0.02
    ,"pixelSetSoftCutBarrelZ" : 0.02
    ,"pixelSetSoftCutBarrelRotX" : 0.05
    ,"pixelSetSoftCutBarrelRotY" : 0.05
    ,"pixelSetSoftCutBarrelRotZ" : 0.05
    ,"pixelSetSoftCutEndcapX" : 0.02
    ,"pixelSetSoftCutEndcapY" : 0.02
    ,"pixelSetSoftCutEndcapZ" : 0.02
    ,"pixelSetSoftCutEndcapRotX" : 0.05
    ,"pixelSetSoftCutEndcapRotY" : 0.05
    ,"pixelSetSoftCutEndcapRotZ" : 0.05
}

for var in pixelAlignment_Options:
  if var in dir():
    pixelAlignment_Options[var] = eval(var)

## SCT alignment Options
sctAlignment_Options = {
    "sctAlignmentLevelBarrel" : -1
    ,"sctAlignmentLevelEndcaps" : -1
    ,"sctAlignmentLevel" : 1
    ,"sctAlignBarrel" : False
    ,"sctAlignEndcaps" : True
    ,"sctAlignBarrelX" : True
    ,"sctAlignBarrelY" : True
    ,"sctAlignBarrelZ" : True
    ,"sctAlignBarrelRotX" : True
    ,"sctAlignBarrelRotY" : True
    ,"sctAlignBarrelRotZ" : True
    ,"sctAlignEndcapX" : True
    ,"sctAlignEndcapY" : True
    ,"sctAlignEndcapZ" : True
    ,"sctAlignEndcapRotX" : True
    ,"sctAlignEndcapRotY" : True
    ,"sctAlignEndcapRotZ" : True
    ,"sctSetSigmaBarrelX" : 1
    ,"sctSetSigmaBarrelY" : 1
    ,"sctSetSigmaBarrelZ" : 1
    ,"sctSetSigmaBarrelRotX" : 0.001
    ,"sctSetSigmaBarrelRotY" : 0.001
    ,"sctSetSigmaBarrelRotZ" : 0.001
    ,"sctSetSigmaEndcapX" : 1
    ,"sctSetSigmaEndcapY" : 1
    ,"sctSetSigmaEndcapZ" : 1
    ,"sctSetSigmaEndcapRotX" : 0.001
    ,"sctSetSigmaEndcapRotY" : 0.001
    ,"sctSetSigmaEndcapRotZ" : 0.001
    ,"sctSetSoftCutBarrelX" : 0.05
    ,"sctSetSoftCutBarrelY" : 0.05
    ,"sctSetSoftCutBarrelZ" : 0.05
    ,"sctSetSoftCutBarrelRotX" : 0.05
    ,"sctSetSoftCutBarrelRotY" : 0.05
    ,"sctSetSoftCutBarrelRotZ" : 0.05
    ,"sctSetSoftCutEndcapX" : 0.05
    ,"sctSetSoftCutEndcapY" : 0.05
    ,"sctSetSoftCutEndcapZ" : 0.005
    ,"sctSetSoftCutEndcapRotX" : 0.005
    ,"sctSetSoftCutEndcapRotY" : 0.05
    ,"sctSetSoftCutEndcapRotZ" : 0.05
}

for var in sctAlignment_Options:
  if var in dir():
    sctAlignment_Options[var] = eval(var)

## Silicon alignment Options
siAlignment_Options = {
    "siAlignmentLevel" : -1
    ,"siAlignX" : True
    ,"siAlignY" : True
    ,"siAlignZ" : True
    ,"siAlignRotX" : True
    ,"siAlignRotY" : True
    ,"siAlignRotZ" : True
}

for var in siAlignment_Options:
  if var in dir():
    print (siAlignment_Options[var], " " ,  eval(var))
    siAlignment_Options[var] = eval(var)

## InDet alignment Options
## -1 - use standalone Silicon and TRT setup
##  0 - L0
indetAlignment_Options = {
    "indetAlignmentLevel" : -1
    ,"indetAlignX" : True
    ,"indetAlignY" : True
    ,"indetAlignZ" : True
    ,"indetAlignRotX" : True
    ,"indetAlignRotY" : True
    ,"indetAlignRotZ" : True
}

for var in indetAlignment_Options:
  if var in dir():
    indetAlignment_Options[var] = eval(var)

################################################################################
####
####    Check which geometry managers need to be set up
####
# we need the silicon geometry manager when both Pixel and SCT are to be aligned
# but we might also need it when the whole Inner Detector is to be aligned
# in the later one of Pixel or SCT is in principle enough
if newInDetAlignGeo_Options["alignPixel"] and newInDetAlignGeo_Options["alignSCT"]:
    print ('  == NewInDetAlignGeometrySetup == The silicon is ON')
    newInDetAlignGeo_Options["alignSilicon"] = True
else:
    print (' == NewInDetAlignGeometrySetup == The silicon is OFF')
    newInDetAlignGeo_Options["alignSilicon"] = False

#
# for level 0 we don't need standalone Pixel and SCT geometry managers
if (siAlignment_Options["siAlignmentLevel"]==0):
    print (" == NewInDetAlignGeometrySetup == siAlignmentLevel = 0 !! -> switch off alignPixel and alignSCT")
    newInDetAlignGeo_Options["alignPixel"] = False
    newInDetAlignGeo_Options["alignSCT"] = False
#
# similar to the silicon case above, we need the inner detector geometry manager
# when both Silicon and TRT are to be aligned
# but we might also need it when the whole ATLAS (?) is to be aligned, in the
# later one of Silicon or TRT is in principle enough
if newInDetAlignGeo_Options["alignTRT"] and newInDetAlignGeo_Options["alignSilicon"]:
    newInDetAlignGeo_Options["alignInDet"] = True
else:
    newInDetAlignGeo_Options["alignInDet"] = False
#
# Inner Detector alignment level
# for alignment level -1 the level is set up separately
# for Silicon and TRT according to their setup

#
# for level 0 we don't need standalone Silicon and TRT geometry managers
if (indetAlignment_Options["indetAlignmentLevel"] ==0):
    print (' == NewIndetAlignGeometry> user indetAlignmentLevel = 0 --> NO ALIGNMENT')
    newInDetAlignGeo_Options["alignSilicon"] = False
    newInDetAlignGeo_Options["alignTRT"] = False

if (not newInDetAlignGeo_Options["alignTRT"] or not newInDetAlignGeo_Options["alignSCT"] or not newInDetAlignGeo_Options["alignPixel"]):
    newInDetAlignGeo_Options["alignInDet"] = False
if (not newInDetAlignGeo_Options["alignSCT"] or not newInDetAlignGeo_Options["alignPixel"]):
    newInDetAlignGeo_Options["alignSilicon"] = False



ToolSvc = Service('ToolSvc')

#
# AlignModuleTool
#
# If we're running sct endcap alignment or TRT L3, we need to load
# ID specific AlignModuleTool, but we rather should do it always just in case
# there are changes in the future. It won't hurt anything.
from InDetAlignGeomTools.InDetAlignGeomToolsConf import InDet__InDetAlignModuleTool
alignModuleTool = InDet__InDetAlignModuleTool(name = "InDetAlignModuleTool")
alignModuleTool.OutputLevel = newInDetAlignGeo_Options["outputLevel"]
ToolSvc += alignModuleTool
print (alignModuleTool)

########################################################################################
####
####    Pixel Alignment Geometry Setup
####

#
print (" == NewInDetAlignGeometrySetup> setting up Pixel Alignment_Options --> PixelGeometryManagerTool")
if newInDetAlignGeo_Options["alignPixel"]:
    # Pixel geometry manager tool
    from InDetAlignGeomTools.InDetAlignGeomToolsConf import InDet__PixelGeometryManagerTool
    pixelGeometryManagerTool = InDet__PixelGeometryManagerTool(name = "PixelGeometryManagerTool")
    pixelGeometryManagerTool.OutputLevel = newInDetAlignGeo_Options["outputLevel"]
    pixelGeometryManagerTool.doModuleSelection = newInDetAlignGeo_Options["doModuleSelection"]
    if (pixelGeometryManagerTool.doModuleSelection): print (" == NewInDetAlignGeometrySetup == doModuleSelection True for Pixels ")
    pixelGeometryManagerTool.ModuleSelection = newInDetAlignGeo_Options["moduleSelection"]
    pixelGeometryManagerTool.AlignModuleTool = alignModuleTool
    # Pixel alignment level
    pixelGeometryManagerTool.AlignmentLevel = pixelAlignment_Options["pixelAlignmentLevel"]
    pixelGeometryManagerTool.AlignmentLevelBarrel = pixelAlignment_Options["pixelAlignmentLevelBarrel"]
    pixelGeometryManagerTool.AlignmentLevelEndcaps = pixelAlignment_Options["pixelAlignmentLevelEndcaps"]
    pixelGeometryManagerTool.AlignmentDBM = newInDetAlignGeo_Options["pixelAlignDBM"]
    # Pixel alignment degrees of freedom
    # Barrel
    pixelGeometryManagerTool.AlignBarrel = pixelAlignment_Options["pixelAlignBarrel"]
    if pixelAlignment_Options["pixelAlignBarrel"]:
        pixelGeometryManagerTool.AlignBarrelX = pixelAlignment_Options["pixelAlignBarrelX"]
        pixelGeometryManagerTool.AlignBarrelY = pixelAlignment_Options["pixelAlignBarrelY"]
        pixelGeometryManagerTool.AlignBarrelZ = pixelAlignment_Options["pixelAlignBarrelZ"]
        pixelGeometryManagerTool.AlignBarrelRotX = pixelAlignment_Options["pixelAlignBarrelRotX"]
        pixelGeometryManagerTool.AlignBarrelRotY = pixelAlignment_Options["pixelAlignBarrelRotY"]
        pixelGeometryManagerTool.AlignBarrelRotZ = pixelAlignment_Options["pixelAlignBarrelRotZ"]
        pixelGeometryManagerTool.AlignBarrelBowX = pixelAlignment_Options["pixelAlignBarrelBowX"]
        pixelGeometryManagerTool.SetSigmaBarrelX = pixelAlignment_Options["pixelSetSigmaBarrelX"]
        pixelGeometryManagerTool.SetSigmaBarrelY = pixelAlignment_Options["pixelSetSigmaBarrelY"]
        pixelGeometryManagerTool.SetSigmaBarrelZ = pixelAlignment_Options["pixelSetSigmaBarrelZ"]
        pixelGeometryManagerTool.SetSigmaBarrelRotX = pixelAlignment_Options["pixelSetSigmaBarrelRotX"]
        pixelGeometryManagerTool.SetSigmaBarrelRotY = pixelAlignment_Options["pixelSetSigmaBarrelRotY"]
        pixelGeometryManagerTool.SetSigmaBarrelRotZ = pixelAlignment_Options["pixelSetSigmaBarrelRotZ"]
        pixelGeometryManagerTool.SetSoftCutBarrelX = pixelAlignment_Options["pixelSetSoftCutBarrelX"]
        pixelGeometryManagerTool.SetSoftCutBarrelY = pixelAlignment_Options["pixelSetSoftCutBarrelY"]
        pixelGeometryManagerTool.SetSoftCutBarrelZ = pixelAlignment_Options["pixelSetSoftCutBarrelZ"]
        pixelGeometryManagerTool.SetSoftCutBarrelRotX = pixelAlignment_Options["pixelSetSoftCutBarrelRotX"]
        pixelGeometryManagerTool.SetSoftCutBarrelRotY = pixelAlignment_Options["pixelSetSoftCutBarrelRotY"]
        pixelGeometryManagerTool.SetSoftCutBarrelRotZ = pixelAlignment_Options["pixelSetSoftCutBarrelRotZ"]
    # Endcaps
    pixelGeometryManagerTool.AlignEndcaps = pixelAlignment_Options["pixelAlignEndcaps"]
    if pixelAlignment_Options["pixelAlignEndcaps"]:
        pixelGeometryManagerTool.AlignEndcapX = pixelAlignment_Options["pixelAlignEndcapX"]
        pixelGeometryManagerTool.AlignEndcapY = pixelAlignment_Options["pixelAlignEndcapY"]
        pixelGeometryManagerTool.AlignEndcapZ = pixelAlignment_Options["pixelAlignEndcapZ"]
        pixelGeometryManagerTool.AlignEndcapRotX = pixelAlignment_Options["pixelAlignEndcapRotX"]
        pixelGeometryManagerTool.AlignEndcapRotY = pixelAlignment_Options["pixelAlignEndcapRotY"]
        pixelGeometryManagerTool.AlignEndcapRotZ = pixelAlignment_Options["pixelAlignEndcapRotZ"]
        pixelGeometryManagerTool.SetSigmaEndcapX = pixelAlignment_Options["pixelSetSigmaEndcapX"]
        pixelGeometryManagerTool.SetSigmaEndcapY = pixelAlignment_Options["pixelSetSigmaEndcapY"]
        pixelGeometryManagerTool.SetSigmaEndcapZ = pixelAlignment_Options["pixelSetSigmaEndcapZ"]
        pixelGeometryManagerTool.SetSigmaEndcapRotX = pixelAlignment_Options["pixelSetSigmaEndcapRotX"]
        pixelGeometryManagerTool.SetSigmaEndcapRotY = pixelAlignment_Options["pixelSetSigmaEndcapRotY"]
        pixelGeometryManagerTool.SetSigmaEndcapRotZ = pixelAlignment_Options["pixelSetSigmaEndcapRotZ"]
        pixelGeometryManagerTool.SetSoftCutEndcapX = pixelAlignment_Options["pixelSetSoftCutBarrelX"]
        pixelGeometryManagerTool.SetSoftCutEndcapY = pixelAlignment_Options["pixelSetSoftCutBarrelY"]
        pixelGeometryManagerTool.SetSoftCutEndcapZ = pixelAlignment_Options["pixelSetSoftCutBarrelZ"]
        pixelGeometryManagerTool.SetSoftCutEndcapRotX = pixelAlignment_Options["pixelSetSoftCutBarrelRotX"]
        pixelGeometryManagerTool.SetSoftCutEndcapRotY = pixelAlignment_Options["pixelSetSoftCutBarrelRotY"]
        pixelGeometryManagerTool.SetSoftCutEndcapRotZ = pixelAlignment_Options["pixelSetSoftCutBarrelRotZ"]
    #
    ToolSvc += pixelGeometryManagerTool
    print (pixelGeometryManagerTool)
print (" == NewInDetAlignGeometrySetup == PixelGeometryManagerTool completed")



########################################################################################
####
####    SCT Alignment Geometry Setup
####

#
print (" == NewInDetAlignGeometrySetup == setting up SCT Alignment_Options --> SCTGeometryManagerTool")
if newInDetAlignGeo_Options["alignSCT"]:
    print (" == NewInDetAlignGeometrySetup == setting up SCT Alignment_Options ")
    # SCT geometry manager tool
    from InDetAlignGeomTools.InDetAlignGeomToolsConf import InDet__SCTGeometryManagerTool
    sctGeometryManagerTool = InDet__SCTGeometryManagerTool(name = "SCTGeometryManagerTool")
    sctGeometryManagerTool.OutputLevel = newInDetAlignGeo_Options["outputLevel"]
    sctGeometryManagerTool.doModuleSelection = newInDetAlignGeo_Options["doModuleSelection"]
    if (sctGeometryManagerTool.doModuleSelection): print (" == NewInDetAlignGeometrySetup == doModuleSelection True for SCT ")
    sctGeometryManagerTool.ModuleSelection = newInDetAlignGeo_Options["moduleSelection"]
    sctGeometryManagerTool.AlignModuleTool = alignModuleTool
    # SCT alignment level
    sctGeometryManagerTool.AlignmentLevel = sctAlignment_Options["sctAlignmentLevel"]
    sctGeometryManagerTool.AlignmentLevelBarrel = sctAlignment_Options["sctAlignmentLevelBarrel"]
    sctGeometryManagerTool.AlignmentLevelEndcaps = sctAlignment_Options["sctAlignmentLevelEndcaps"]
    # SCT alignment degrees of freedom
    # Barrel
    sctGeometryManagerTool.AlignBarrel = sctAlignment_Options["sctAlignBarrel"]
    if sctAlignment_Options["sctAlignBarrel"]:
        sctGeometryManagerTool.AlignBarrelX = sctAlignment_Options["sctAlignBarrelX"]
        sctGeometryManagerTool.AlignBarrelY = sctAlignment_Options["sctAlignBarrelY"]
        sctGeometryManagerTool.AlignBarrelZ = sctAlignment_Options["sctAlignBarrelZ"]
        sctGeometryManagerTool.AlignBarrelRotX = sctAlignment_Options["sctAlignBarrelRotX"]
        sctGeometryManagerTool.AlignBarrelRotY = sctAlignment_Options["sctAlignBarrelRotY"]
        sctGeometryManagerTool.AlignBarrelRotZ = sctAlignment_Options["sctAlignBarrelRotZ"]
        sctGeometryManagerTool.SetSigmaBarrelX = sctAlignment_Options["sctSetSigmaBarrelX"]
        sctGeometryManagerTool.SetSigmaBarrelY = sctAlignment_Options["sctSetSigmaBarrelY"]
        sctGeometryManagerTool.SetSigmaBarrelZ = sctAlignment_Options["sctSetSigmaBarrelZ"]
        sctGeometryManagerTool.SetSigmaBarrelRotX = sctAlignment_Options["sctSetSigmaBarrelRotX"]
        sctGeometryManagerTool.SetSigmaBarrelRotY = sctAlignment_Options["sctSetSigmaBarrelRotY"]
        sctGeometryManagerTool.SetSigmaBarrelRotZ = sctAlignment_Options["sctSetSigmaBarrelRotZ"]
        sctGeometryManagerTool.SetSoftCutBarrelX = sctAlignment_Options["sctSetSoftCutBarrelX"]
        sctGeometryManagerTool.SetSoftCutBarrelY = sctAlignment_Options["sctSetSoftCutBarrelY"]
        sctGeometryManagerTool.SetSoftCutBarrelZ = sctAlignment_Options["sctSetSoftCutBarrelZ"]
        sctGeometryManagerTool.SetSoftCutBarrelRotX = sctAlignment_Options["sctSetSoftCutBarrelRotX"]
        sctGeometryManagerTool.SetSoftCutBarrelRotY = sctAlignment_Options["sctSetSoftCutBarrelRotY"]
        sctGeometryManagerTool.SetSoftCutBarrelRotZ = sctAlignment_Options["sctSetSoftCutBarrelRotZ"]
    # Endcaps
    sctGeometryManagerTool.AlignEndcaps = sctAlignment_Options["sctAlignEndcaps"]
    if sctAlignment_Options["sctAlignEndcaps"]:
        sctGeometryManagerTool.AlignEndcapX = sctAlignment_Options["sctAlignEndcapX"]
        sctGeometryManagerTool.AlignEndcapY = sctAlignment_Options["sctAlignEndcapY"]
        sctGeometryManagerTool.AlignEndcapZ = sctAlignment_Options["sctAlignEndcapZ"]
        sctGeometryManagerTool.AlignEndcapRotX = sctAlignment_Options["sctAlignEndcapRotX"]
        sctGeometryManagerTool.AlignEndcapRotY = sctAlignment_Options["sctAlignEndcapRotY"]
        sctGeometryManagerTool.AlignEndcapRotZ = sctAlignment_Options["sctAlignEndcapRotZ"]
        sctGeometryManagerTool.SetSigmaEndcapX = sctAlignment_Options["sctSetSigmaEndcapX"]
        sctGeometryManagerTool.SetSigmaEndcapY = sctAlignment_Options["sctSetSigmaEndcapY"]
        sctGeometryManagerTool.SetSigmaEndcapZ = sctAlignment_Options["sctSetSigmaEndcapZ"]
        sctGeometryManagerTool.SetSigmaEndcapRotX = sctAlignment_Options["sctSetSigmaEndcapRotX"]
        sctGeometryManagerTool.SetSigmaEndcapRotY = sctAlignment_Options["sctSetSigmaEndcapRotY"]
        sctGeometryManagerTool.SetSigmaEndcapRotZ = sctAlignment_Options["sctSetSigmaEndcapRotZ"]
        sctGeometryManagerTool.SetSoftCutEndcapX = sctAlignment_Options["sctSetSoftCutEndcapX"]
        sctGeometryManagerTool.SetSoftCutEndcapY = sctAlignment_Options["sctSetSoftCutEndcapY"]
        sctGeometryManagerTool.SetSoftCutEndcapZ = sctAlignment_Options["sctSetSoftCutEndcapZ"]
        sctGeometryManagerTool.SetSoftCutEndcapRotX = sctAlignment_Options["sctSetSoftCutEndcapRotX"]
        sctGeometryManagerTool.SetSoftCutEndcapRotY = sctAlignment_Options["sctSetSoftCutEndcapRotY"]
        sctGeometryManagerTool.SetSoftCutEndcapRotZ = sctAlignment_Options["sctSetSoftCutEndcapRotZ"]
    #
    ToolSvc += sctGeometryManagerTool
    print (sctGeometryManagerTool)
print (' == NewInDetAlignGeometrySetup == SCTGeometryManagerTool completed')


########################################################################################
####
####    Silicon Alignment Geometry Setup
####
if newInDetAlignGeo_Options["alignSilicon"]:
    # Silicon geometry manager tool
    from InDetAlignGeomTools.InDetAlignGeomToolsConf import InDet__SiGeometryManagerTool
    siGeometryManagerTool = InDet__SiGeometryManagerTool(name = "SiGeometryManagerTool")
    siGeometryManagerTool.OutputLevel = newInDetAlignGeo_Options["outputLevel"]
    siGeometryManagerTool.doModuleSelection = newInDetAlignGeo_Options["doModuleSelection"]
    if (siGeometryManagerTool.doModuleSelection): print (' == NewInDetAlignGeometrySetup == doModuleSelection True for Silicon ')
    siGeometryManagerTool.ModuleSelection = newInDetAlignGeo_Options["moduleSelection"]
    siGeometryManagerTool.AlignModuleTool = alignModuleTool
    #
    # Silicon alignment level
    siGeometryManagerTool.AlignmentLevel = siAlignment_Options["siAlignmentLevel"]
    #
    # we don't need the Pixel and SCT geometry managers if Silicon alignment level is 0
    if not (siAlignment_Options["siAlignmentLevel"]==0):
        if newInDetAlignGeo_Options["alignPixel"]:
            siGeometryManagerTool.PixelGeometryManager = pixelGeometryManagerTool
        if newInDetAlignGeo_Options["alignSCT"]:
            siGeometryManagerTool.SCTGeometryManager = sctGeometryManagerTool
    siGeometryManagerTool.AlignPixel = newInDetAlignGeo_Options["alignPixel"]
    siGeometryManagerTool.AlignSCT = newInDetAlignGeo_Options["alignSCT"]
    #
    # Silicon alignment degrees of freedom
    siGeometryManagerTool.AlignX = siAlignment_Options["siAlignX"]
    siGeometryManagerTool.AlignY = siAlignment_Options["siAlignY"]
    siGeometryManagerTool.AlignZ = siAlignment_Options["siAlignZ"]
    siGeometryManagerTool.AlignRotX = siAlignment_Options["siAlignRotX"]
    siGeometryManagerTool.AlignRotY = siAlignment_Options["siAlignRotY"]
    siGeometryManagerTool.AlignRotZ = siAlignment_Options["siAlignRotZ"]
    #
    ToolSvc += siGeometryManagerTool
    print (siGeometryManagerTool)


########################################################################################
####
####    TRT Alignment Geometry Setup
####

print (" == NewInDetAlignGeometrySetup == setting up TRT Alignment_Options --> TRTGeometryManagerTool")
if newInDetAlignGeo_Options["alignTRT"]:
    # TRT geometry manager tool
    from InDetAlignGeomTools.InDetAlignGeomToolsConf import InDet__TRTGeometryManagerTool
    trtGeometryManagerTool = InDet__TRTGeometryManagerTool(name = "TRTGeometryManagerTool")
    trtGeometryManagerTool.OutputLevel = newInDetAlignGeo_Options["outputLevel"]
    trtGeometryManagerTool.doModuleSelection = newInDetAlignGeo_Options["doModuleSelection"]
    trtGeometryManagerTool.ModuleSelection = newInDetAlignGeo_Options["moduleSelection"]
    trtGeometryManagerTool.AlignBarrelPhiSectors = trtAlignment_Options["trtAlignBarrelPhiSectors"]
    trtGeometryManagerTool.AlignBarrelLayers = trtAlignment_Options["trtAlignBarrelLayers"]
    trtGeometryManagerTool.AlignEndcapPhiSectors = trtAlignment_Options["trtAlignEndcapPhiSectors"]
    trtGeometryManagerTool.AlignEndcapWheels = trtAlignment_Options["trtAlignEndcapWheels"]
    trtGeometryManagerTool.AlignSingleEndcap = trtAlignment_Options["trtAlignSingleEndcap"]
    trtGeometryManagerTool.AlignModuleTool = alignModuleTool
    # TRT alignment level
    trtGeometryManagerTool.AlignmentLevel = trtAlignment_Options["trtAlignmentLevel"]
    trtGeometryManagerTool.AlignmentLevelBarrel = trtAlignment_Options["trtAlignmentLevelBarrel"]
    trtGeometryManagerTool.AlignmentLevelEndcaps = trtAlignment_Options["trtAlignmentLevelEndcaps"]

    # TRT alignment degrees of freedom
    # Barrel
    print (" == NewInDetAlignGeometrySetup == setting up TRT barrel Alignment_Options ")
    trtGeometryManagerTool.AlignBarrel = trtAlignment_Options["trtAlignBarrel"]
    if trtAlignment_Options["trtAlignBarrel"]:
        trtGeometryManagerTool.AlignBarrelX = trtAlignment_Options["trtAlignBarrelX"]
        trtGeometryManagerTool.AlignBarrelY = trtAlignment_Options["trtAlignBarrelY"]
        trtGeometryManagerTool.AlignBarrelZ = trtAlignment_Options["trtAlignBarrelZ"]
        trtGeometryManagerTool.AlignBarrelRotX = trtAlignment_Options["trtAlignBarrelRotX"]
        trtGeometryManagerTool.AlignBarrelRotY = trtAlignment_Options["trtAlignBarrelRotY"]
        trtGeometryManagerTool.AlignBarrelRotZ = trtAlignment_Options["trtAlignBarrelRotZ"]
        trtGeometryManagerTool.SetSigmaBarrelX = trtAlignment_Options["trtSetSigmaBarrelX"]
        trtGeometryManagerTool.SetSigmaBarrelY = trtAlignment_Options["trtSetSigmaBarrelY"]
        trtGeometryManagerTool.SetSigmaBarrelZ = trtAlignment_Options["trtSetSigmaBarrelZ"]
        trtGeometryManagerTool.SetSigmaBarrelRotX = trtAlignment_Options["trtSetSigmaBarrelRotX"]
        trtGeometryManagerTool.SetSigmaBarrelRotY = trtAlignment_Options["trtSetSigmaBarrelRotY"]
        trtGeometryManagerTool.SetSigmaBarrelRotZ = trtAlignment_Options["trtSetSigmaBarrelRotZ"]
        trtGeometryManagerTool.SetSoftCutBarrelX = trtAlignment_Options["trtSetSoftCutBarrelX"]
        trtGeometryManagerTool.SetSoftCutBarrelY = trtAlignment_Options["trtSetSoftCutBarrelY"]
        trtGeometryManagerTool.SetSoftCutBarrelZ = trtAlignment_Options["trtSetSoftCutBarrelZ"]
        trtGeometryManagerTool.SetSoftCutBarrelRotX = trtAlignment_Options["trtSetSoftCutBarrelRotX"]
        trtGeometryManagerTool.SetSoftCutBarrelRotY = trtAlignment_Options["trtSetSoftCutBarrelRotY"]
        trtGeometryManagerTool.SetSoftCutBarrelRotZ = trtAlignment_Options["trtSetSoftCutBarrelRotZ"]
        print (" == InDetAlign_Geometry_Setup == trtAlignment_Options[trtAlignmentLevelBarrel]=", trtAlignment_Options["trtAlignmentLevelBarrel"])
        if (trtAlignment_Options["trtAlignmentLevel"] == 1 or trtAlignment_Options["trtAlignmentLevelBarrel"] == 1):
            trtGeometryManagerTool.AlignBarrelZ = False
                    

    # Endcaps
    print (' <NewInDetAlignGeometrySetup> setting up TRT endcaps Alignment_Options ')
    trtGeometryManagerTool.AlignEndcaps = trtAlignment_Options["trtAlignEndcaps"]
    if trtAlignment_Options["trtAlignEndcaps"]:
        trtGeometryManagerTool.AlignEndcapX = trtAlignment_Options["trtAlignEndcapX"]
        trtGeometryManagerTool.AlignEndcapY = trtAlignment_Options["trtAlignEndcapY"]
        trtGeometryManagerTool.AlignEndcapZ = trtAlignment_Options["trtAlignEndcapZ"]
        trtGeometryManagerTool.AlignEndcapRotX = trtAlignment_Options["trtAlignEndcapRotX"]
        trtGeometryManagerTool.AlignEndcapRotY = trtAlignment_Options["trtAlignEndcapRotY"]
        trtGeometryManagerTool.AlignEndcapRotZ = trtAlignment_Options["trtAlignEndcapRotZ"]
        trtGeometryManagerTool.SetSigmaEndcapX = trtAlignment_Options["trtSetSigmaEndcapX"]
        trtGeometryManagerTool.SetSigmaEndcapY = trtAlignment_Options["trtSetSigmaEndcapY"]
        trtGeometryManagerTool.SetSigmaEndcapZ = trtAlignment_Options["trtSetSigmaEndcapZ"]
        trtGeometryManagerTool.SetSigmaEndcapRotX = trtAlignment_Options["trtSetSigmaEndcapRotX"]
        trtGeometryManagerTool.SetSigmaEndcapRotY = trtAlignment_Options["trtSetSigmaEndcapRotY"]
        trtGeometryManagerTool.SetSigmaEndcapRotZ = trtAlignment_Options["trtSetSigmaEndcapRotZ"]
        trtGeometryManagerTool.SetSoftCutEndcapX = trtAlignment_Options["trtSetSoftCutEndcapX"]
        trtGeometryManagerTool.SetSoftCutEndcapY = trtAlignment_Options["trtSetSoftCutEndcapY"]
        trtGeometryManagerTool.SetSoftCutEndcapZ = trtAlignment_Options["trtSetSoftCutEndcapZ"]
        trtGeometryManagerTool.SetSoftCutEndcapRotX = trtAlignment_Options["trtSetSoftCutEndcapRotX"]
        trtGeometryManagerTool.SetSoftCutEndcapRotY = trtAlignment_Options["trtSetSoftCutEndcapRotY"]
        trtGeometryManagerTool.SetSoftCutEndcapRotZ = trtAlignment_Options["trtSetSoftCutEndcapRotZ"]
        if (trtAlignment_Options["trtAlignmentLevel"] == 1 or trtAlignment_Options["trtAlignmentLevelEndcaps"] == 1):
            trtGeometryManagerTool.AlignEndcapZ = False

    #
    ToolSvc += trtGeometryManagerTool
    print (trtGeometryManagerTool)
print (" == NewInDetAlignGeometrySetup == TRTGeometryManagerTool completed")


########################################################################################
####
####    Inner Detector Alignment Geometry Setup
####
if newInDetAlignGeo_Options["alignInDet"]:
    print (" == NewInDetAlignGeometrySetup == alignInDet = ",newInDetAlignGeo_Options["alignInDet"])
    # Inner Detector geometry manager tool
    from InDetAlignGeomTools.InDetAlignGeomToolsConf import InDet__InDetGeometryManagerTool
    indetGeometryManagerTool = InDet__InDetGeometryManagerTool(name = "InDetGeometryManagerTool")
    indetGeometryManagerTool.OutputLevel = newInDetAlignGeo_Options["outputLevel"]
    indetGeometryManagerTool.doModuleSelection = newInDetAlignGeo_Options["doModuleSelection"]
    indetGeometryManagerTool.ModuleSelection = newInDetAlignGeo_Options["moduleSelection"]
    indetGeometryManagerTool.AlignModuleTool = alignModuleTool
    #
    # Inner Detector alignment level
    indetGeometryManagerTool.AlignmentLevel = indetAlignment_Options["indetAlignmentLevel"]
    #
    # we don't need the Pixel and SCT geometry managers if Inner Detector alignment level is 0
    if not (indetAlignment_Options["indetAlignmentLevel"]==0):
        if newInDetAlignGeo_Options["alignSilicon"]:
            indetGeometryManagerTool.SiGeometryManager = siGeometryManagerTool
        if newInDetAlignGeo_Options["alignTRT"]:
            indetGeometryManagerTool.TRTGeometryManager = trtGeometryManagerTool
    indetGeometryManagerTool.AlignSilicon = newInDetAlignGeo_Options["alignSilicon"]
    indetGeometryManagerTool.AlignTRT = newInDetAlignGeo_Options["alignTRT"]
    print (" == NewInDetAlignGeometrySetup == indetGeometryManagerTool.AlignSilicon = ", indetGeometryManagerTool.AlignSilicon)
    print (" == NewInDetAlignGeometrySetup == indetGeometryManagerTool.AlignTRT = ", indetGeometryManagerTool.AlignTRT)
    #
    # Inner Detector alignment degrees of freedom
    indetGeometryManagerTool.AlignX = indetAlignment_Options["indetAlignX"]
    indetGeometryManagerTool.AlignY = indetAlignment_Options["indetAlignY"]
    indetGeometryManagerTool.AlignZ = indetAlignment_Options["indetAlignZ"]
    indetGeometryManagerTool.AlignRotX = indetAlignment_Options["indetAlignRotX"]
    indetGeometryManagerTool.AlignRotY = indetAlignment_Options["indetAlignRotY"]
    indetGeometryManagerTool.AlignRotZ = indetAlignment_Options["indetAlignRotZ"]
    #
    ToolSvc += indetGeometryManagerTool
    print (indetGeometryManagerTool)


# select geometry manager to be used in the alignment
if newInDetAlignGeo_Options["alignInDet"]:
    trkAlignGeoManagerTool = indetGeometryManagerTool
elif newInDetAlignGeo_Options["alignTRT"]:
    trkAlignGeoManagerTool = trtGeometryManagerTool
elif newInDetAlignGeo_Options["alignSilicon"]:
    trkAlignGeoManagerTool = siGeometryManagerTool
elif newInDetAlignGeo_Options["alignPixel"]:
    trkAlignGeoManagerTool = pixelGeometryManagerTool
elif newInDetAlignGeo_Options["alignSCT"]:
    trkAlignGeoManagerTool = sctGeometryManagerTool




########################################################################################
####
####    Inner Detector DB I/O Setup
####

########################
#### Silicon DB Output
if (newInDetAlignGeo_Options["alignInDet"] or newInDetAlignGeo_Options["alignSilicon"] or newInDetAlignGeo_Options["alignSCT"] or newInDetAlignGeo_Options["alignPixel"]):
    from InDetAlignDBTools.InDetAlignDBToolsConf import InDet__SiTrkAlignDBTool
    siTrkDBTool = InDet__SiTrkAlignDBTool(OutputLevel = newInDetAlignGeo_Options["outputLevel"]);
    siTrkDBTool.AlignModuleTool = alignModuleTool
    if (newInDetAlignGeo_Options["alignPixel"] or newInDetAlignGeo_Options["alignSCT"] and not newInDetAlignGeo_Options["alignSilicon"]):
        if newInDetAlignGeo_Options["alignPixel"]:
            siTrkDBTool.PixelGeometryManager = pixelGeometryManagerTool
        if newInDetAlignGeo_Options["alignSCT"]:
            siTrkDBTool.SCTGeometryManager = sctGeometryManagerTool
    elif newInDetAlignGeo_Options["alignSilicon"]:
        siTrkDBTool.SiGeometryManager = siGeometryManagerTool
    else:
        siTrkDBTool.SiGeometryManager = indetGeometryManagerTool
    siTrkDBTool.UpdateConstants = newInDetAlignGeo_Options["updateConstants"]
    siTrkDBTool.WriteTextFile = newInDetAlignGeo_Options["writeTextFiles"]
    siTrkDBTool.OutputTextFile = newInDetAlignGeo_Options["siliconTextFile"]
    siTrkDBTool.WriteOldConstants = newInDetAlignGeo_Options["writeOldConstants"]
    siTrkDBTool.AlignmentDBM = newInDetAlignGeo_Options["pixelAlignDBM"]
    siTrkDBTool.WriteAsL3 = False
    print (" == NewInDetAlignGeometrySetup == siTrkDBTool.WriteAsL3 ", siTrkDBTool.WriteAsL3)


    ToolSvc += siTrkDBTool
    print     (siTrkDBTool)

########################
#### TRT DB Output
if (newInDetAlignGeo_Options["alignInDet"] or newInDetAlignGeo_Options["alignTRT"]):
    print (' <NewInDetAlignGeometrySetup> defining TRT_AlignDbSvc ')
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_AlignDbSvc
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += TRT_AlignDbSvc(
        name = "TRT_AlignDbSvc",
        #StreamTool = TRTCondStream,
        alignTextFile = "",
        #alignString = "ALold",
        OutputLevel = DEBUG)
        #OutputLevel = newInDetAlignGeo_Options["outputLevel"])

    from InDetAlignDBTools.InDetAlignDBToolsConf import InDet__TRTTrkAlignDBTool
    trtTrkDBTool = InDet__TRTTrkAlignDBTool(OutputLevel = newInDetAlignGeo_Options["outputLevel"]);
    trtTrkDBTool.AlignModuleTool = alignModuleTool
    if newInDetAlignGeo_Options["alignTRT"]:
        trtTrkDBTool.TRTGeometryManager = trtGeometryManagerTool
    else:
        trtTrkDBTool.SiGeometryManager = indetGeometryManagerTool
    trtTrkDBTool.UpdateConstants = newInDetAlignGeo_Options["updateConstants"]
    trtTrkDBTool.WriteTextFile = newInDetAlignGeo_Options["writeTextFiles"]
    trtTrkDBTool.OutputTextFile = newInDetAlignGeo_Options["trtTextFile"]
    trtTrkDBTool.WriteOldConstants = newInDetAlignGeo_Options["writeOldConstants"]
    #trtTrkDBTool.WriteTRTAsL2 = False # R22 --> needs to update the code

    ToolSvc += trtTrkDBTool
    print (' == NewInDetAligGeometrySetup == next lines are for trtDBTool ')
    print     (trtTrkDBTool)

########################
#### Inner Detector DB Output
if newInDetAlignGeo_Options["alignInDet"]:
    from InDetAlignDBTools.InDetAlignDBToolsConf import InDet__InDetTrkAlignDBTool
    indetTrkDBTool = InDet__InDetTrkAlignDBTool(OutputLevel = newInDetAlignGeo_Options["outputLevel"]);
    indetTrkDBTool.SiTrkAlignDBTool = siTrkDBTool
    indetTrkDBTool.TRTTrkAlignDBTool = trtTrkDBTool

    ToolSvc += indetTrkDBTool
    print     (indetTrkDBTool)


# select DB tool to be used in the alignment
if newInDetAlignGeo_Options["alignInDet"]:
    trkAlignDBTool = indetTrkDBTool
elif newInDetAlignGeo_Options["alignTRT"]:
    trkAlignDBTool = trtTrkDBTool
else:
    trkAlignDBTool = siTrkDBTool

print (' == NewInDetAlignGeometrySetup == file ** COMPLETED **')
