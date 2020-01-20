#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
@file PixelAthMonitoringBase.py
@brief Helper functions for Run 3 Pixel monitoring algorithm configuration
'''
# hack to deal with global variables in this module
# check if we are in "old-" or "new-style" configuration
from AthenaConfiguration.AllConfigFlags import ConfigFlags
if ConfigFlags.DQ.isReallyOldStyle:
    from RecExConfig.AutoConfiguration import GetRunNumber
    runtext = ' (Run %d)' % GetRunNumber()
else:
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    runtext = ' (Run %d)' % ConfigFlags.Input.RunNumber[0]


NumLayersDisk = 3
NumLayersDBM  = 3
NumStavesIBL  = 14
NumStavesL0   = 22
NumStavesL1   = 38
NumStavesL2   = 52

NumModulesIBL   = 32
NumModulesIBL2D = 12
NumModulesIBL3D = 8
NumModulesBarrel= 13
NumModulesDisk  = 48
NumModulesDBM   = 4
NumPP0sEC       = 24

btxt     = ";eta index of module"
sbtxt    = ";shifted eta index of module"
ditxt    = ";disk number"
dbmtxt   = ";layer number"
layers   = ["ECA","ECC","B0","B1","B2","IBL","DBMA","DBMC"]
xbinsl   = [    3,    3,  13,  13,  13,   32,     3,     3]
xminsl   = [ -0.5, -0.5,-6.5,-6.5,-6.5,-16.5,  -0.5,  -0.5]
ybinsl   = [   48,   48,  22,  38,  52,   14,     4,     4]
etatxt   = [ditxt,ditxt,btxt,btxt,btxt,sbtxt,dbmtxt,dbmtxt]
phitext  = ";phi index of module"
totcuts  = [   15,   15,  15,  15,  15,    4,     4,     4] 
xbinsem  = [    3,    3,  13,  13,  13,   20,     3,     3]
xminsem  = [  0.5,  0.5,-6.5,-6.5,-6.5,  -10,   0.5,   0.5]
xbinstotz= [    3,    3,  13,  13,  13,   20,     3,     3]
xminstotz= [ -0.5, -0.5,-0.5,-0.5,-0.5, -0.5,  -0.5,  -0.5]
ztotbinsy= [   20,   20,  20,  20,  20,   20,    20,    20]
ztotminsy= [ 19.5, 19.5, 7.5,19.5,19.5, -0.5,  -0.5,  -0.5]

pp0layers= ["ECA","ECC","B0","B1","B2","IBLA","IBLC"]
pp0xbins = [   24,   24,  22,  38,  52,   14,     14]

lumitext  = ";lumi block"
lumibinsx = 3000

bcidbinsx = 3600

#labels
LayersDisk = ["Disk 1", "Disk 2", "Disk 3"] #x EC
#xBarrel
ModulesBarrel = [
    "M6C", "M5C", "M4C", "M3C", "M2C", "M1C", "M0","M1A", "M2A", "M3A", "M4A",
    "M5A", "M6A"]
#xIBL
ModulesIBL = [
    "C8", " ", "C7", " ", "C6", " ", "C5", " ", "C4", " ", "C3", " ", "C2", " ",
    "C1", " ", "A1", " ", "A2", " ", "A3", " ", "A4", " ", "A5", " ", "A6", " ",
    "A7", " ", "A8", " "]
LayersDBM = ["Layer 0", "Layer 1", "Layer 2"] #xDBM

LabelX = [LayersDisk, LayersDisk, ModulesBarrel, ModulesBarrel, ModulesBarrel, ModulesIBL, LayersDBM, LayersDBM]

ModulesECA = [
    "B01_S2_M1", "B01_S2_M6", "B01_S2_M2", "B01_S2_M5", "B01_S2_M3",
    "B01_S2_M4", "B02_S1_M1", "B02_S1_M6", "B02_S1_M2", "B02_S1_M5",
    "B02_S1_M3", "B02_S1_M4", "B02_S2_M1", "B02_S2_M6", "B02_S2_M2",
    "B02_S2_M5", "B02_S2_M3", "B02_S2_M4", "B03_S1_M1", "B03_S1_M6",
    "B03_S1_M2", "B03_S1_M5", "B03_S1_M3", "B03_S1_M4", "B03_S2_M1",
    "B03_S2_M6", "B03_S2_M2", "B03_S2_M5", "B03_S2_M3", "B03_S2_M4",
    "B04_S1_M1", "B04_S1_M6", "B04_S1_M2", "B04_S1_M5", "B04_S1_M3",
    "B04_S1_M4", "B04_S2_M1", "B04_S2_M6", "B04_S2_M2", "B04_S2_M5",
    "B04_S2_M3", "B04_S2_M4", "B01_S1_M1", "B01_S1_M6", "B01_S1_M2",
    "B01_S1_M5", "B01_S1_M3", "B01_S1_M4"]
ModulesECC = [
    "B01_S2_M4", "B01_S2_M3", "B01_S2_M5", "B01_S2_M2", "B01_S2_M6",
    "B01_S2_M1", "B02_S1_M4", "B02_S1_M3", "B02_S1_M5", "B02_S1_M2",
    "B02_S1_M6", "B02_S1_M1", "B02_S2_M4", "B02_S2_M3", "B02_S2_M5",
    "B02_S2_M2", "B02_S2_M6", "B02_S2_M1", "B03_S1_M4", "B03_S1_M3",
    "B03_S1_M5", "B03_S1_M2", "B03_S1_M6", "B03_S1_M1", "B03_S2_M4",
    "B03_S2_M3", "B03_S2_M5", "B03_S2_M2", "B03_S2_M6", "B03_S2_M1",
    "B04_S1_M4", "B04_S1_M3", "B04_S1_M5", "B04_S1_M2", "B04_S1_M6",
    "B04_S1_M1", "B04_S2_M4", "B04_S2_M3", "B04_S2_M5", "B04_S2_M2",
    "B04_S2_M6", "B04_S2_M1", "B01_S1_M4", "B01_S1_M3", "B01_S1_M5",
    "B01_S1_M2", "B01_S1_M6", "B01_S1_M1"]

ModulesDBM = ["M3", "M4", "M1", "M2"] #yDBM


#yBarrel
StavesL0 = [
    "B11_S2", "B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2",
    "B04_S1", "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2", "B07_S1",
    "B07_S2", "B08_S1", "B08_S2", "B09_S1", "B09_S2", "B10_S1", "B10_S2",
    "B11_S1"]


StavesL1 = [
    "B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2", "B04_S1",
    "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2", "B07_S1", "B07_S2",
    "B08_S1", "B08_S2", "B09_S1", "B09_S2", "B10_S1", "B10_S2", "B11_S1",
    "B11_S2", "B12_S1", "B12_S2", "B13_S1", "B13_S2", "B14_S1", "B14_S2",
    "B15_S1", "B15_S2", "B16_S1", "B16_S2", "B17_S1", "B17_S2", "B18_S1",
    "B18_S2", "B19_S1", "B19_S2"]

StavesL2 = [
    "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2", "B04_S1", "B04_S2",
    "B05_S1", "B05_S2", "B06_S1", "B06_S2", "B07_S1", "B07_S2", "B08_S1",
    "B08_S2", "B09_S1", "B09_S2", "B10_S1", "B10_S2", "B11_S1", "B11_S2",
    "B12_S1", "B12_S2", "B13_S1", "B13_S2", "B14_S1", "B14_S2", "B15_S1",
    "B15_S2", "B16_S1", "B16_S2", "B17_S1", "B17_S2", "B18_S1", "B18_S2",
    "B19_S1", "B19_S2", "B20_S1", "B20_S2", "B21_S1", "B21_S2", "B22_S1",
    "B22_S2", "B23_S1", "B23_S2", "B24_S1", "B24_S2", "B25_S1", "B25_S2",
    "B26_S1", "B26_S2", "B01_S1"]
#yIBL
StavesIBL = [
    "S01", "S02", "S03", "S04", "S05", "S06", "S07", "S08", "S09", "S10", "S11",
    "S12", "S13","S14"]

LabelY = [ModulesECA, ModulesECC, StavesL0, StavesL1, StavesL2, StavesIBL, ModulesDBM, ModulesDBM]

#PP0
PP0sEC = [
    "D1_B01_S2", "D1_B02_S1", "D1_B02_S2", "D1_B03_S1", "D1_B03_S2", "D1_B04_S1", "D1_B04_S2", "D1_B01_S1",
    "D2_B01_S2", "D2_B02_S1", "D2_B02_S2", "D2_B03_S1", "D2_B03_S2", "D2_B04_S1", "D2_B04_S2", "D2_B01_S1",
    "D3_B01_S2", "D3_B02_S1", "D3_B02_S2", "D3_B03_S1", "D3_B03_S2", "D3_B04_S1", "D3_B04_S2", "D3_B01_S1"]

PP0LabelX = [PP0sEC, PP0sEC, StavesL0, StavesL1, StavesL2, StavesIBL, StavesIBL]

layergroups = {}
def getLayerGroup(helper, alg, layer):
    if alg not in layergroups:
        layergroups[alg] = {}
    if layer not in layergroups[alg]:
        layergroups[alg][layer] = helper.addGroup(alg, layer)
    return layergroups[alg][layer]


def define2DProfHist(helper, alg, name, title, path, type='TProfile2D', doWeight=False, lifecycle='run', zmin=None, zmax=None, opt='', histname=None):
    '''
    This function configures 2D (Profile) histograms (or maps) for Pixel layers.

    Arguments:
         helper     -- AthMonitorCfgHelper(Old) instance
         alg        -- algorithm - Configurable object returned from addAlgorithm
         name       -- Name of histogram (Name = name_layer)
         title      -- Title of histogram (Title = title +' '+layer)
         path       -- Path in output file for histogram
         type       -- Type of histogram (TH2D, TProfile2D)
         lifecycle  -- global life duration of histograms (run, lowstat [i.e. 20 LB], lumiblock) - APPLIES to MonGroup only
         zmin(zmax) -- fix the displayed range - simply chopping the range!!!
         opt        -- history depth of a histogram e.g. 'kLBNHistoryDepth=10'
         histname   -- another way of naming the histogram(s), useful when multiple histograms are filled from exactly the same variables, but in a different way 
    '''
    if histname is None:
        histname = name
    for i, layer in enumerate(layers):
        fulltitle   = title + ' {0}'.format(layer) + runtext + etatxt[i] + phitext
        layerGroup = getLayerGroup(helper, alg, layer)

        # sequential list of x- and y-axis bin labels (see defineHistogram)
        labels = []
        for label in LabelX[i]:
            labels.append(label)
        for label in LabelY[i]:
            labels.append(label)
        fullvarstring = '{0}_{1},{0}_{2}'.format(name, 'em', 'pm')
        if 'Profile' in type: fullvarstring += ',{0}_{1}'.format(name, 'val')
        if doWeight:
            weightvar = '{0}_{1}'.format(name, 'wgt')
        else :
            weightvar = ''
        fullvarstring += ';' + histname + '_{0}'.format(layer)
        layerGroup.defineHistogram(fullvarstring, 
                                    type=type, path=path, title=fulltitle, weight=weightvar,
                                    xbins=xbinsl[i], xmin=xminsl[i], xmax=xminsl[i]+xbinsl[i], 
                                    ybins=ybinsl[i], ymin=-0.5, ymax=-0.5+ybinsl[i],
                                    zmin=zmin, zmax=zmax,
                                    duration=lifecycle,
                                    opt=opt, labels=labels)

def definePP0Histos(helper, alg, name, title, path, opt=''):
    '''
    This function configures TProfile histograms for Pixel layers per pp0 or ROD.

    Arguments:
         helper  -- AthMonitorCfgHelper(Old) instance
         alg     -- algorithm Configurable object returned from addAlgorithm
         name    -- Name of histogram (Name = name_layer)
         title   -- Title of histogram (Title = title +' '+layer)
         path    -- Path in ouput file for histogram
         opt     -- history depth of a histogram e.g. 'kLBNHistoryDepth=10'
    '''

    for i, layer in enumerate(pp0layers):
        if i<2:
            xatxt = ';sector'
        else :
            xatxt = ';stave'
        yatxt = ';average # clusters per event per '
        if i>4:
            yatxt += 'FE'
        else :
            yatxt += 'module'
        fulltitle   = title + ' {0}'.format(layer) + runtext + xatxt + yatxt
        groupname   = name  + '_{0}'.format(layer)
        layerGroup = getLayerGroup(helper, alg, layer)
        # sequential list of x-axis bin labels (see defineHistogram)
        labels = []
        for label in PP0LabelX[i]:
            labels.append(label)
        fullvarstring = '{0}_{1},{0}_{2}'.format(name, 'pospp0x', 'val')
        fullvarstring += ';' + groupname
        layerGroup.defineHistogram(fullvarstring, 
                                    type='TProfile', path=path, title=fulltitle,
                                    xbins=pp0xbins[i], xmin=-0.5, xmax=-0.5+pp0xbins[i],
                                    opt=opt, labels=labels)



def define1DProfLumiLayers(helper, alg, name, title, path, yaxistext, type='TProfile', histname=None):
    '''
    This function configures 1D (Profile) vs lumi histograms for Pixel layers.

    Arguments:
         helper    -- AthMonitorCfgHelper(Old) instance
         alg       -- algorithm Configurable object returned from addAlgorithm
         name      -- Name of histogram (Name = name_layer)
         title     -- Title of histogram (Title = title +' '+layer)
         path      -- Path in ouput file for histogram
         yaxistext -- Text on the y-axis
         type      -- Type of histogram (TH1D, TProfile)
         histname  -- another way of naming the histogram(s), useful when multiple histograms are filled from exactly the same variables, but in a different way 
    '''

    if histname is None:
        histname = name
    for layer in layers:
        fulltitle   = title + ' {0}'.format(layer) + runtext + lumitext + yaxistext
        layerGroup = getLayerGroup(helper, alg, layer)
        fullvarstring = '{0}_{1}'.format(name,'lb')
        if 'Profile' in type: fullvarstring += ',{0}_{1}'.format(name, 'val')
        fullvarstring += ';' + histname + '_{0}'.format(layer)
        layerGroup.defineHistogram(fullvarstring, 
                                    type=type, path=path, title=fulltitle,
                                    xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx)

def defineMapVsLumiLayers(helper, alg, name, title, path, xaxistext, yaxistext, ybins, ymins, binsizes=[1.0], type='TH2F', histname=None):
    '''
    This function configures 2D histograms vs lumi for Pixel layers.

    Arguments:
         helper  -- AthMonitorCfgHelper(Old) instance
         alg     -- algorithm Configurable object returned from addAlgorithm
         name    -- Name of histogram (Name = name_layer)
         title   -- Title of histogram (Title = title +' '+layer)
         path    -- Path in ouput file for histogram
         ybins, ymin, ymax, yaxistext
                 -- Configure Y-axis
         type    -- Type of TH2 histogram (TH2I, TH2F)
         histname-- alternative root name of the histogram (to be filled with the same variables defined by 'name' above)  
    '''

    if histname is None:
        histname = name
    for idx,layer in enumerate(layers):
        fulltitle   = title + ' {0}'.format(layer) + runtext + lumitext + yaxistext
        layerGroup = getLayerGroup(helper, alg, layer)
        fullvarstring = '{0}_{1}'.format(name,'lb')
        fullvarstring += ',{0}_{1}'.format(name, 'val')
        fullvarstring += ';' + histname  + '_{0}'.format(layer)
        if ( len(ybins)==1 and len(ymins)==1 and len(binsizes)==1):
            layerGroup.defineHistogram(fullvarstring, 
                                       type=type, path=path, title=fulltitle,
                                       xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx,
                                       ybins=ybins[0], ymin=ymins[0], ymax=ymins[0]+binsizes[0]*ybins[0])
        elif (len(ybins)==len(layers) and len(ymins)==len(layers) and len(binsizes)==len(layers)):
            layerGroup.defineHistogram(fullvarstring, 
                                       type=type, path=path, title=fulltitle,
                                       xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx,
                                       ybins=ybins[idx], ymin=ymins[idx], ymax=ymins[idx]+binsizes[idx]*ybins[idx])


def define1DLayers(helper, alg, name, title, path, xaxistext, yaxistext, xbins, xmins, binsizes=[1.0], type='TH1F', histname=None):
    '''
    This function configures 1D (Profile) histograms for Pixel layers.

    Arguments:
         helper     -- AthMonitorCfgHelper(Old) instance
         alg        -- algorithm Configurable object returned from addAlgorithm
         name       -- Root name of variables and histogram (Name = name+'_'+layer)
         title      -- Title of histogram (Title = title +' '+layer)
         path       -- Path in ouput file for histogram
         xaxistext  -- X-axis title
         yaxistext  -- Y-axis title
         type       -- Type of histogram
         histname   -- alternative root name of the histogram (to be filled with the same variables defined by 'name' above)  
    '''

    if histname is None:
        histname = name
    for idx,layer in enumerate(layers):
        fulltitle   = title + ' {0}'.format(layer) + runtext + xaxistext + yaxistext
        layerGroup = getLayerGroup(helper, alg, layer)
        fullvarstring = '{0}_{1}'.format(name,'val')
        fullvarstring += ';' + histname  + '_{0}'.format(layer)
        if ( len(xbins)==1 and len(xmins)==1 and len(binsizes)==1):
            layerGroup.defineHistogram(fullvarstring, 
                                        type=type, path=path, title=fulltitle,
                                        xbins=xbins[0], xmin=xmins[0], xmax=xmins[0]+binsizes[0]*xbins[0])
        elif (len(xbins)==len(layers) and len(xmins)==len(layers) and len(binsizes)==len(layers)):
            layerGroup.defineHistogram(fullvarstring, 
                                        type=type, path=path, title=fulltitle,
                                        xbins=xbins[idx], xmin=xmins[idx], xmax=xmins[idx]+binsizes[idx]*xbins[idx])

def addOnTrackTxt(name, ontrack, wSpace=False):
    if ontrack:
        if wSpace:
            name += ' OnTrack'
        else:
            name += '_OnTrack'
    return name

def addOnTrackToPath(name, ontrack):
    if ontrack:
        name += 'OnTrack/'
    else:
        name += '/'
    return name

def fullDressTitle(name, ontrack, xaxistext, yaxistext):
    if ontrack:
        name += ' OnTrack'
    name += runtext + xaxistext + yaxistext
    return name
