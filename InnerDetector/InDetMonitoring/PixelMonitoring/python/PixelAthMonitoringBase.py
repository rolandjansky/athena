#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
@file PixelAthMonitoringBase.py
@brief Helper functions for Run 3 Pixel monitoring algorithm configuration
'''
# hack to deal with global variables in this module
# check if we are in "old-" or "new-style" configuration
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from RecExConfig.AutoConfiguration import GetRunNumber
if ConfigFlags.DQ.isReallyOldStyle:
    runtext = ''
    if GetRunNumber() is not None:
        runtext = ' (Run %d)' % GetRunNumber()
else:
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
errbbinsy= [   17,   17,  17,  17,  17,   27,    27,    27]
errbminsy= [    0,    0,   0,   0,   0,    0,     0,     0]
errbbsizy= [    1,    1,   1,   1,   1,    1,     1,     1]
errtbinsy= [    7,    7,   7,   7,   7,    7,     7,     7]

pp0layers= ["ECA","ECC","B0","B1","B2","IBLA","IBLC"]
pp0xbins = [   24,   24,  22,  38,  52,   14,     14]

fei3layers= ["ECA","ECC","B0","B1","B2"]
fei4layers= ["IBL","DBMA","DBMC"]

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

#Errors
ErrStateLabelsFEI3 = [
      ("ROD_Timeout", "ROD Formatter Timeout"),
      ("ROD_Sync_BCID_errors", "ROD BCID Sync"),
      ("ROD_Sync_LVL1ID_errors", "ROD LVL1ID Sync"),
      ("Optical_Errors", "Preamble/Header"),
      ("Mod_Sync_LVL1ID_errors", "FE/MCC LVL1ID Sync"),
      ("Mod_Sync_BCID2_errors", "FE/MCC BCID2 Sync"),
      ("Mod_Sync_BCID1_errors", "FE/MCC BCID1 Sync"),
      ("Mod_Trunc_EoE_Overflow_errors", "FE/MCC EoE Overflow Trunc"),
      ("Mod_Trunc_Hit_Overflow_errors", "FE/MCC Hit Overflow Trunc"),
      ("FE_Warning", "FE Warning"),
      ("SEU_Hit_Parity", "SEU Hit Parity"),
      ("SEU_Register_Parity", "SEU Register Parity"),
      ("SEU_Hamming", "SEU Hamming Code"),
      ("Mod_Trunc_EOC_errors", "FE/MCC EoC Trunc"),
      ("ROD_Trailer_Bit_errors", "ROD Trailer Bit"),
      ("ROD_Trunc_HT_Limit_errors", "ROD H/T Limit Trunc"),
      ("ROD_Trunc_ROD_OF_errors", "ROD Overflow Trunc")
]
ErrStateLabelsFEI4 = [
      ("ROD_Timeout_errors", "ROD Timeout"),
      ("ROD_BCID_errors", "ROD BCID synchronization"),
      ("ROD_LVL1ID_errors", "ROD LVL1ID synchronization"),
      ("ROD_Preamble_errors", "ROD Preamble"),
      ("ROD_Trailer_errors", "ROD Trailer"),
      ("ROD_Row-Column_errors", "ROD row-column"),
      ("ROD_Masked_link", "ROD Masked link"),
      ("ROD_Limit_errors", "ROD Header Trailer limit"),
      ("SR0_BCID_counter_errors", "SR0 BCID counter"),
      ("SR1_Hamming_code_0_errors", "SR1 Hamming code in word 0"),
      ("SR2_Hamming_code_1_errors", "SR2 Hamming code in word 1"),
      ("SR3_Hamming_code_2_errors", "SR3 Hamming code in word 2"),
      ("SR4_L1_in_counter_errors", "SR4 L1 in counter"),
      ("SR5_L1_request_counter_errors", "SR5 L1 request counter"),
      ("SR6_L1_register_errors", "SR6 L1 register"),
      ("SR7_L1_Trigger_ID_errors", "SR7 L1 trigger ID"),
      ("SR8_FE_readout_process_errors", "SR8 FE readout process"),
      ("SR15_Skippped_trig_count_errors", "SR15 Skipped trigger counter"),
      ("SR16_Truncated_event_flag_errors", "SR16 Truncated event"),
      ("SR24_Triple_redundant_errors_CNFGMEM", "SR24 Triple redundant CNFGMEM"),
      ("SR25_Write_reg_data_errors", "SR25 Write register data"),
      ("SR26_Address_errors", "SR26 Address"),
      ("SR27_Other_CMD_decoder_errors", "SR27 CMD decoder"),
      ("SR28_CMD_decoder_bitflip_errors", "SR28 CMD decoder bit flip"),
      ("SR29_Triple_redundant_errors_CMD", "SR29 Triple redundant CMD"),
      ("SR30_Data_bus_address_errors", "SR30 Data bus address"),
      ("SR31_Triple_redundant_errors_EFUSE", "SR31 Triple redundant EFUSE")
]

ErrCatRODModLabels = [
      ("SyncErrors_Mod", "FE/MCC Sync Errors"),
      ("SyncErrors_ROD", "ROD Sync Errors"),
      ("TruncErrors_Mod", "FE/MCC Trunc Errors"),
      ("TruncErrors_ROD", "ROD Trunc Errors"),
      ("OpticalErrors_RODMod", "Preamble/Header Errors"),
      ("SEUErrors_RODMod", "SEU Errors"),
      ("TimeoutErrors_RODMod", "Timeout Errors")
]

ErrCatRODModLabelsNorm = [
      "SyncErrors_Mod_Frac_per_event",
      "SyncErrors_ROD_Frac_per_event",
      "TruncErrors_Mod_Frac_per_event",
      "TruncErrors_ROD_Frac_per_event"
]

ErrCatLabels = [
      ("SyncErrors", "Sync Errors (FE/MCC & ROD)"),
      ("TruncErrors", "Trunc Errors (FE/MCC & ROD)"),
      ("OpticalErrors", "Preamble/Header Errors"),
      ("SEUErrors", "SEU Errors"),
      ("TimeoutErrors", "Timeout Errors")
]

ErrCatLabelsNorm = [
      "SyncErrorsFrac_per_event", 
      "TruncationErrorsFrac_per_event", 
      "OpticalErrorsFrac_per_event", 
      "SEUErrorsFrac_per_event", 
      "TimeoutErrorsFrac_per_event"
]

layergroups = {}
def getLayerGroup(helper, alg, layer):
    if alg not in layergroups:
        layergroups[alg] = {}
    if layer not in layergroups[alg]:
        layergroups[alg][layer] = helper.addGroup(alg, layer)
    return layergroups[alg][layer]


def define2DProfHist(helper, alg, name, title, path, type='TProfile2D', doWeight=False, lifecycle='run', zmin=None, zmax=None, opt='', histname=None, onlylayers=layers):
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
         onlylayers -- sublist of layers 
    '''
    assert(set(onlylayers).issubset(layers))
    if histname is None:
        histname = name
    for i, layer in enumerate(layers):
        if layer not in onlylayers: 
            continue
        fulltitle   = title + ' {0}'.format(layer) + runtext + etatxt[i] + phitext
        layerGroup = getLayerGroup(helper, alg, layer)

        fullvarstring = '{0}_{1},{0}_{2}'.format(name, 'em', 'pm')
        weightvar = ''
        if 'Profile' in type: 
            fullvarstring += ',{0}_{1}'.format(name, 'val')
        elif doWeight:
            weightvar = '{0}_{1}'.format(name, 'val') # re-use the same variable for TH2 for now.
            
        fullvarstring += ';' + histname + '_{0}'.format(layer)
        layerGroup.defineHistogram(fullvarstring, 
                                    type=type, path=path, title=fulltitle, weight=weightvar,
                                    xbins=xbinsl[i], xmin=xminsl[i], xmax=xminsl[i]+xbinsl[i], 
                                    ybins=ybinsl[i], ymin=-0.5, ymax=-0.5+ybinsl[i],
                                    zmin=zmin, zmax=zmax,
                                    duration=lifecycle,
                                    opt=opt, xlabels=LabelX[i], ylabels=LabelY[i])

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
        fullvarstring = '{0}_{1},{0}_{2}'.format(name, 'pospp0x', 'val')
        fullvarstring += ';' + groupname
        layerGroup.defineHistogram(fullvarstring, 
                                    type='TProfile', path=path, title=fulltitle,
                                    xbins=pp0xbins[i], xmin=-0.5, xmax=-0.5+pp0xbins[i],
                                    opt=opt, xlabels=PP0LabelX[i])



def define1DProfLumiLayers(helper, alg, name, title, path, yaxistext, type='TProfile', histname=None, onlylayers=layers):
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
         onlylayers -- sublist of layers
    '''
    assert(set(onlylayers).issubset(layers))
    if histname is None:
        histname = name
    for layer in layers:
        if layer not in onlylayers: 
            continue
        fulltitle   = title + ' {0}'.format(layer) + runtext + lumitext + yaxistext
        layerGroup = getLayerGroup(helper, alg, layer)
        fullvarstring = '{0}_{1}'.format(name,'lb')
        if 'Profile' in type: fullvarstring += ',{0}_{1}'.format(name, 'val')
        fullvarstring += ';' + histname + '_{0}'.format(layer)
        layerGroup.defineHistogram(fullvarstring, 
                                    type=type, path=path, title=fulltitle,
                                    xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx)

def defineMapVsLumiLayers(helper, alg, name, title, path, xaxistext, yaxistext, ybins, ymins, binsizes=[1.0], ylabels=None, type='TH2F', histname=None, onlylayers=layers):
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
         type    -- Type of histogram (TH2I, TH2F, TProfile2D)
         histname-- alternative root name of the histogram (to be filled with the same variables defined by 'name' above)
         onlylayers -- sublist of layers
    '''
    assert(set(onlylayers).issubset(layers))
    if histname is None:
        histname = name
    for idx,layer in enumerate(layers):
        if layer not in onlylayers: 
            continue
        fulltitle   = title + ' {0}'.format(layer) + runtext + lumitext + yaxistext
        layerGroup = getLayerGroup(helper, alg, layer)
        fullvarstring = '{0}_{1}'.format(name,'lb')
        if 'Profile' in type: fullvarstring += ',{0}_{1}'.format(name, 'cat')
        fullvarstring += ',{0}_{1}'.format(name, 'val')
        fullvarstring += ';' + histname  + '_{0}'.format(layer)
        if ( len(ybins)==1 and len(ymins)==1 and len(binsizes)==1):
            layerGroup.defineHistogram(fullvarstring, 
                                       type=type, path=path, title=fulltitle,
                                       xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx,
                                       ybins=ybins[0], ymin=ymins[0], ymax=ymins[0]+binsizes[0]*ybins[0])
        elif (len(ybins)==len(layers) and len(ymins)==len(layers) and len(binsizes)==len(layers) and len(ylabels)==len(layers)):
            layerGroup.defineHistogram(fullvarstring, 
                                       type=type, path=path, title=fulltitle,
                                       xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx,
                                       ybins=ybins[idx], ymin=ymins[idx], ymax=ymins[idx]+binsizes[idx]*ybins[idx], ylabels=ylabels[idx])


def define1DLayers(helper, alg, name, title, path, xaxistext, yaxistext, xbins, xmins, binsizes=[1.0], type='TH1F', histname=None, onlylayers=layers):
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
         onlylayers -- sublist of layers
    '''
    assert(set(onlylayers).issubset(layers))
    if histname is None:
        histname = name
    for idx,layer in enumerate(layers):
        if layer not in onlylayers: 
            continue
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
