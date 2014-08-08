# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Definitions of the monitoring classes

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigCaloCellMakerValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigCaloCellMaker_Validation"):
        super(TrigCaloCellMakerValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('CellContainerSize', type='TH1F', title="TrigCaloCellMaker Cell container size; Size; nevents", xbins=100, xmin=0., xmax=10000.) ]
        self.Histograms += [ defineHistogram('ConversionErrorInLArEM'  , type='TH1F', title="Conversion Errors bit mask (LArEM)   ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInLArHad' , type='TH1F', title="Conversion Errors bit mask (LArHad)  ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInTile'   , type='TH1F', title="Conversion Errors bit mask (Tile)    ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFCalEm' , type='TH1F', title="Conversion Errors bit mask (FCalEm)  ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFCalHad', type='TH1F', title="Conversion Errors bit mask (FCalHad) ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFullCalo',type='TH1F', title="Conversion Errors bit mask (FullCalo); # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInLArEM'  , type='TH2F',
                                             title="Conversion Errors vs Eta (LArEM)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInLArHad'  , type='TH2F',
                                             title="Conversion Errors vs Eta (LArHad)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInTile'  , type='TH2F',
                                             title="Conversion Errors vs Eta (Tile)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInFCalEm'  , type='TH2F',
                                             title="Conversion Errors vs Eta (FCalEm)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInFCalHad'  , type='TH2F',
                                             title="Conversion Errors vs Eta (FCalHad)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInLArEM'  , type='TH2F',
                                             title="Conversion Errors vs Phi (LArEM)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInLArHad'  , type='TH2F',
                                             title="Conversion Errors vs Phi (LArHad)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInTile'  , type='TH2F',
                                             title="Conversion Errors vs Phi (Tile)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInFCalEm'  , type='TH2F',
                                             title="Conversion Errors vs Phi (FCalEm)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInFCalHad'  , type='TH2F',
                                             title="Conversion Errors vs Phi (FCalHad)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]                        
        self.Histograms += [ defineHistogram('TCRec_LArEMTotal', type='TH1F', title="LArEM Calo total unpacking time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_LArEMRegionSelector', type='TH1F', title="LArEM Region Selector time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_LArEMDataAccess', type='TH1F', title="LArEM Data Access time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_LArEMPushBackCells', type='TH1F', title="LArEM Push Back Cells time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_LArHadTotal', type='TH1F', title="LArHad Calo total unpacking time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_LArHadRegionSelector', type='TH1F', title="LArHad Region Selector time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_LArHadDataAccess', type='TH1F', title="LArHad Data Access time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_LArHadPushBackCells', type='TH1F', title="LArHad Push Back Cells time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_TileTotal', type='TH1F', title="Tile Calo total unpacking time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_TileRegionSelector', type='TH1F', title="Tile Region Selector time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_TileDataAccess', type='TH1F', title="Tile Data Access time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_TilePushBackCells', type='TH1F', title="Tile Push Back Cells time", xbins=100, xmin=0., xmax=20.) ]

class TrigCaloCellMakerFCalValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigCaloCellMakerFCal_Validation"):
        super(TrigCaloCellMakerFCalValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('CellContainerSize', type='TH1F', title="TrigCaloCellMaker Cell container size; Size; nevents", xbins=100, xmin=0., xmax=10000.) ]
        self.Histograms += [ defineHistogram('ConversionErrorInLArEM'  , type='TH1F', title="Conversion Errors bit mask (LArEM)   ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInLArHad' , type='TH1F', title="Conversion Errors bit mask (LArHad)  ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInTile'   , type='TH1F', title="Conversion Errors bit mask (Tile)    ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFCalEm' , type='TH1F', title="Conversion Errors bit mask (FCalEm)  ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFCalHad', type='TH1F', title="Conversion Errors bit mask (FCalHad) ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFullCalo',type='TH1F', title="Conversion Errors bit mask (FullCalo); # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInLArEM'  , type='TH2F',
                                             title="Conversion Errors vs Eta (LArEM)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInLArHad'  , type='TH2F',
                                             title="Conversion Errors vs Eta (LArHad)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInTile'  , type='TH2F',
                                             title="Conversion Errors vs Eta (Tile)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInFCalEm'  , type='TH2F',
                                             title="Conversion Errors vs Eta (FCalEm)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInFCalHad'  , type='TH2F',
                                             title="Conversion Errors vs Eta (FCalHad)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInLArEM'  , type='TH2F',
                                             title="Conversion Errors vs Phi (LArEM)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInLArHad'  , type='TH2F',
                                             title="Conversion Errors vs Phi (LArHad)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInTile'  , type='TH2F',
                                             title="Conversion Errors vs Phi (Tile)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInFCalEm'  , type='TH2F',
                                             title="Conversion Errors vs Phi (FCalEm)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInFCalHad'  , type='TH2F',
                                             title="Conversion Errors vs Phi (FCalHad)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]                                
        self.Histograms += [ defineHistogram('TCRec_LArEMTotal', type='TH1F', title="LArEM Calo total unpacking time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_LArEMRegionSelector', type='TH1F', title="LArEM Region Selector time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_LArEMDataAccess', type='TH1F', title="LArEM Data Access time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_LArEMPushBackCells', type='TH1F', title="LArEM Push Back Cells time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_LArHadTotal', type='TH1F', title="LArHad Calo total unpacking time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_LArHadRegionSelector', type='TH1F', title="LArHad Region Selector time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_LArHadDataAccess', type='TH1F', title="LArHad Data Access time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_LArHadPushBackCells', type='TH1F', title="LArHad Push Back Cells time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_TileTotal', type='TH1F', title="Tile Calo total unpacking time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_TileRegionSelector', type='TH1F', title="Tile Region Selector time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_TileDataAccess', type='TH1F', title="Tile Data Access time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_TilePushBackCells', type='TH1F', title="Tile Push Back Cells time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_FCalEmTotal', type='TH1F', title="FCalEm Calo total unpacking time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_FCalEmRegionSelector', type='TH1F', title="FCalEm Region Selector time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_FCalEmDataAccess', type='TH1F', title="FCalEm Data Access time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_FCalEmPushBackCells', type='TH1F', title="FCalEm Push Back Cells time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_FCalHadTotal', type='TH1F', title="FCalHad Calo total unpacking time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_FCalHadRegionSelector', type='TH1F', title="FCalHad Calo total unpacking time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_FCalHadDataAccess', type='TH1F', title="FCalHad Data Access time", xbins=100, xmin=0., xmax=20.) ]
        self.Histograms += [ defineHistogram('TCRec_FCalHadPushBackCells', type='TH1F', title="FCalHad Push Back Cells time", xbins=100, xmin=0., xmax=20.) ]

class TrigCaloCellMakerOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigCaloCellMaker_Online"):
        super(TrigCaloCellMakerOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('CellContainerSize', type='TH1F', title="TrigCaloCellMaker Cell container size; Size; nevents", xbins=100, xmin=0., xmax=10000.) ]
        self.Histograms += [ defineHistogram('ConversionErrorInLArEM'  , type='TH1F', title="Conversion Errors bit mask (LArEM)   ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInLArHad' , type='TH1F', title="Conversion Errors bit mask (LArHad)  ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInTile'   , type='TH1F', title="Conversion Errors bit mask (Tile)    ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFCalEm' , type='TH1F', title="Conversion Errors bit mask (FCalEm)  ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFCalHad', type='TH1F', title="Conversion Errors bit mask (FCalHad) ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFullCalo',type='TH1F', title="Conversion Errors bit mask (FullCalo); # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInLArEM'  , type='TH2F',
                                             title="Conversion Errors vs Eta (LArEM)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInLArHad'  , type='TH2F',
                                             title="Conversion Errors vs Eta (LArHad)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInTile'  , type='TH2F',
                                             title="Conversion Errors vs Eta (Tile)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInFCalEm'  , type='TH2F',
                                             title="Conversion Errors vs Eta (FCalEm)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInFCalHad'  , type='TH2F',
                                             title="Conversion Errors vs Eta (FCalHad)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInLArEM'  , type='TH2F',
                                             title="Conversion Errors vs Phi (LArEM)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInLArHad'  , type='TH2F',
                                             title="Conversion Errors vs Phi (LArHad)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInTile'  , type='TH2F',
                                             title="Conversion Errors vs Phi (Tile)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInFCalEm'  , type='TH2F',
                                             title="Conversion Errors vs Phi (FCalEm)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInFCalHad'  , type='TH2F',
                                             title="Conversion Errors vs Phi (FCalHad)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]                        


class TrigCaloCellMakerCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigCaloCellMaker_Cosmic"):
        super(TrigCaloCellMakerCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        self.Histograms += [ defineHistogram('CellContainerSize', type='TH1F', title="TrigCaloCellMaker Cell container size; Size; nevents", xbins=100, xmin=0., xmax=2000.) ]
        self.Histograms += [ defineHistogram('ConversionErrorInLArEM'  , type='TH1F', title="Conversion Errors bit mask (LArEM)   ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInLArHad' , type='TH1F', title="Conversion Errors bit mask (LArHad)  ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInTile'   , type='TH1F', title="Conversion Errors bit mask (Tile)    ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFCalEm' , type='TH1F', title="Conversion Errors bit mask (FCalEm)  ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFCalHad', type='TH1F', title="Conversion Errors bit mask (FCalHad) ; # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFullCalo',type='TH1F', title="Conversion Errors bit mask (FullCalo); # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInLArEM'  , type='TH2F',
                                             title="Conversion Errors vs Eta (LArEM)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInLArHad'  , type='TH2F',
                                             title="Conversion Errors vs Eta (LArHad)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInTile'  , type='TH2F',
                                             title="Conversion Errors vs Eta (Tile)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInFCalEm'  , type='TH2F',
                                             title="Conversion Errors vs Eta (FCalEm)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('EtaL2, ConversionErrorInFCalHad'  , type='TH2F',
                                             title="Conversion Errors vs Eta (FCalHad)   ; #eta; Conversion Errors",
                                             xbins=92, xmin=-4.9, xmax=4.9, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInLArEM'  , type='TH2F',
                                             title="Conversion Errors vs Phi (LArEM)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInLArHad'  , type='TH2F',
                                             title="Conversion Errors vs Phi (LArHad)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInTile'  , type='TH2F',
                                             title="Conversion Errors vs Phi (Tile)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInFCalEm'  , type='TH2F',
                                             title="Conversion Errors vs Phi (FCalEm)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]
        self.Histograms += [ defineHistogram('PhiL2, ConversionErrorInFCalHad'  , type='TH2F',
                                             title="Conversion Errors vs Phi (FCalHad)   ; #phi; Conversion Errors",
                                             xbins=64, xmin=-3.2, xmax=3.2, ybins=20, ymin=0., ymax=20) ]                        
        
class TrigFullCaloCellMakerValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigFullCaloCellMaker_Validation"):
        super(TrigFullCaloCellMakerValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('CellContainerSize', type='TH1F', title="TrigCaloCellMaker Cell container size; Size; nevents", xbins=100, xmin=0., xmax=200000.) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFullCalo',type='TH1F', title="Conversion Errors bit mask (FullCalo); # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('FullCalo_Total', type='TH1F', title="Full Calo total unpacking time", xbins=100, xmin=0., xmax=1000.) ]

class TrigFullCaloCellMakerOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigFullCaloCellMaker_Online"):
        super(TrigFullCaloCellMakerOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('CellContainerSize', type='TH1F', title="TrigCaloCellMaker Cell container size; Size; nevents", xbins=100, xmin=0., xmax=200000.) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFullCalo',type='TH1F', title="Conversion Errors bit mask (FullCalo); # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('FullCalo_Total', type='TH1F', title="Full Calo total unpacking time", xbins=100, xmin=0., xmax=1000.) ]

class TrigFullCaloCellMakerCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigFullCaloCellMaker_Cosmic"):
        super(TrigFullCaloCellMakerCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        self.Histograms += [ defineHistogram('CellContainerSize', type='TH1F', title="TrigCaloCellMaker Cell container size; Size; nevents", xbins=100, xmin=0., xmax=200000.) ]
        self.Histograms += [ defineHistogram('ConversionErrorInFullCalo',type='TH1F', title="Conversion Errors bit mask (FullCalo); # of errors; ", xbins=20, xmin=0., xmax=20) ]
        self.Histograms += [ defineHistogram('FullCalo_Total', type='TH1F', title="Full Calo total unpacking time", xbins=100, xmin=0., xmax=1000.) ]

