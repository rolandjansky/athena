# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#file: TileCoolMgr.py
#author: nils.gollub@cern.ch
#modifications: lukas.pribyl@cern.ch

import string

class TileCoolMgr:
    
    #_______________________________________________________________
    def __init__(self):

        #=== initialize logger with INFO level
        from AthenaCommon.Logging import logging
        self.__log = logging.getLogger( 'TileCoolMgr' )
        #        self.__log.setLevel(logging.INFO)
        
        #=== Master dictionary for: condId -> [folder, connStr, tag, folder2, type]
        self.__idDict = {}

        #=== Available folder types
        self.__validTypes = ["SplitMC", "SplitOnline", "OfflineOnly", "Sqlite"]

        #=== do we run on real data (not MC)?
        from AthenaCommon.GlobalFlags import globalflags
        self.__globalflags = globalflags
        self.isData = globalflags.DataSource()=='data'
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        self.__athenaCommonFlags = athenaCommonFlags

        
    #_______________________________________________________________
    def addSource(self, condId, folder, connStr, tag, folder2, type, key = ""):
        if type not in self.__validTypes:
            self.__log.error("Folder type \'%s\' not recognized!" % type)
        if (type != "OfflineOnly") and (folder2 == ""):
            self.__log.error("For folder type \'%s\' second folder name required!" % type)
        self.__idDict[condId] = [folder, connStr, tag, folder2, type, key]

    #_______________________________________________________________
    def getFolder(self, condId):
        idInfo = self.__idDict.get(condId)
        if not idInfo:
            self.__log.error("CondId \'%s\' not recognized!" % condId)
            return None
        else:
            return idInfo[0]
    #_______________________________________________________________
    def getKey(self, condId):
        idInfo = self.__idDict.get(condId)
        if not idInfo:
            self.__log.error("CondId \'%s\' not recognized!" % condId)
            return None
        elif idInfo[5] != "": return idInfo[5]
        elif len(self.getTag(condId)):   return self.getTag(condId)
        else:
            folder = self.getActualFolder(condId)
            if folder: return folder
            else:
                self.__log.error("Cannot find out key for  \'%s\'!" % condId)
                return None

    #_______________________________________________________________
    def getFolderTwo(self, condId):
        idInfo = self.__idDict.get(condId)
        if not idInfo:
            self.__log.error("CondId \'%s\' not recognized!" % condId)
            return None
        else:
            return idInfo[3]

    #_______________________________________________________________
    def isSplitMC(self, condId):
        idInfo = self.__idDict.get(condId)
        if not idInfo:
            self.__log.error("CondId \'%s\' not recognized!" % condId)
            return None
        else:
            if idInfo[4]=="SplitMC": return True
            else: return False

    #_______________________________________________________________
    def isSplitOnline(self, condId):
        idInfo = self.__idDict.get(condId)
        if not idInfo:
            self.__log.error("CondId \'%s\' not recognized!" % condId)
            return None
        else:
            if idInfo[4]=="SplitOnline": return True
            else: return False            

    #_______________________________________________________________
    def isOfflineOnly(self, condId):
        idInfo = self.__idDict.get(condId)
        if not idInfo:
            self.__log.error("CondId \'%s\' not recognized!" % condId)
            return None
        else:
            if idInfo[4]=="OfflineOnly": return True
            else: return False            

    #_______________________________________________________________
    def isSqlite(self, condId):
        idInfo = self.__idDict.get(condId)
        if not idInfo:
            self.__log.error("CondId \'%s\' not recognized!" % condId)
            return None
        else:
            if idInfo[4]=="Sqlite": return True
            else: return False


    #_______________________________________________________________
    def setFolder(self, condId, folder):
        idInfo = self.__idDict.get(condId)
        if not idInfo:
            self.__log.error("CondId \'%s\' not recognized!" % condId)
        else:
            self.__log.info("Setting folder for condId \'%s\' to \'%s\'" % (condId,folder))
            idInfo[0] = folder
            idInfo[3] = folder

    #_______________________________________________________________
    def setDbConn(self, condId, dbConn):
        idInfo = self.__idDict.get(condId)
        if not idInfo:
            self.__log.error("CondId \'%s\' not recognized!" % condId)
        else:
            self.__log.info("Setting dbConn for condId \'%s\' to \'%s\'" % (condId,dbConn))
            idInfo[1] = dbConn
            from string import find
            if find(dbConn,".db")>0:
                self.__log.info("The data source looks like sqlite file (.db), setting folder type to Sqlite")
                self.__idDict[condId][4] = "Sqlite"
                
    #_______________________________________________________________
    def getDbConn(self, condId):
        idInfo = self.__idDict.get(condId)
        if not idInfo:
            self.__log.error("CondId \'%s\' not recognized!" % condId)
        else:
            return idInfo[1]

    #_______________________________________________________________
    def setTag(self, condId, tag):
        idInfo = self.__idDict.get(condId)
        if not idInfo:
            self.__log.error("CondId \'%s\' not recognized!" % condId)
        else:
            if len(tag) > 0:
                if not tag.startswith('Tile'):
                    tag = self.getTagPrefix(self.getActualFolder(condId)) + tag
            self.__log.info("Setting tag for condId \'%s\' to \'%s\'" % (condId,tag))
            idInfo[2] = tag

    #_______________________________________________________________
    def getTag(self, condId):
        idInfo = self.__idDict.get(condId)
        if not idInfo:
            self.__log.error("CondId \'%s\' not recognized!" % condId)
        else:
            return idInfo[2]

    #_______________________________________________________________
    def setGlobalDbConn(self, dbConn):
        for condId in self.__idDict.keys():
            self.setDbConn(condId,dbConn)
            
    #_______________________________________________________________
    def setGlobalTag(self, tag):
        for condId in self.__idDict.keys():
            self.setTag(condId,tag)

    #_______________________________________________________________
    def getTagPrefix(self, coolFolder):
        folderTag=""
        for i in coolFolder.lower().split("/")[1:]:
            folderTag+=i.capitalize()
        folderTag+="-"
        return folderTag
        
    #_______________________________________________________________
    def getConnectionString(self, condId):
        idInfo = self.__idDict.get(condId)
        if not idInfo:
            self.__log.error("CondId \'%s\' not recognized!" % condId)
        else:
            fullStr = idInfo[1]+" "
            if len(idInfo[2]):
                fullStr += idInfo[2]+" "
            fullStr += idInfo[0]
            return fullStr


    def isMC(self):
        return self.__globalflags.DataSource() != 'data'

    def isOnline(self):
        return self.__athenaCommonFlags.isOnline()


    def isSourceAvailable(self, source):
        return source in  self.__idDict


    def getActualFolder(self, condId):
        if self.isOfflineOnly(condId): return self.getFolder(condId)
        elif self.isSplitOnline(condId): 
            if self.isOnline() and not self.isMC(): return self.getFolder(condId)
            else:                                   return self.getFolderTwo(condId)
        elif self.isSplitMC(condId):
            if self.isMC(): return self.getFolderTwo(condId)
            else:           return self.getFolder(condId)
        elif self.isSqlite(condId): return self.getFolder(condId)
        else: return None


#--------------------------------------------------------------------------------------------------------
#=== create object of the user 
#--------------------------------------------------------------------------------------------------------

#=== fill with default folders:
tileCoolMgr = TileCoolMgr()
defConnStr = 'TILE'
oflConnStr = 'TILE_OFL'
#--- energy calibration

tileCoolMgr.addSource('oflLasLin',    '/TILE/ONL01/CALIB/LAS/LIN', defConnStr, "", '/TILE/OFL02/CALIB/LAS/LIN',     'SplitOnline')
tileCoolMgr.addSource('oflCes',       '/TILE/ONL01/CALIB/CES',     defConnStr, "", '/TILE/OFL02/CALIB/CES',         'SplitOnline')
tileCoolMgr.addSource('oflEms',       '/TILE/ONL01/CALIB/EMS',     defConnStr, "", '/TILE/OFL02/CALIB/EMS',         'SplitOnline')

#--- online calib is taken from the folder tag defined in TileOfcCoolPlugin 
#--- the tags are fixed only in the case of real data - in both online and offline schema.
tileCoolMgr.addSource('onlCisLin', '/TILE/ONL01/CALIB/CIS/LIN', defConnStr, "", '/TILE/OFL02/CALIB/CIS/FIT/LIN', 'SplitMC')
tileCoolMgr.addSource('onlLasLin', '/TILE/ONL01/CALIB/LAS/LIN', defConnStr, "", '/TILE/OFL02/CALIB/LAS/LIN',     'SplitMC')
tileCoolMgr.addSource('onlCes',    '/TILE/ONL01/CALIB/CES',     defConnStr, "", '/TILE/OFL02/CALIB/CES',         'SplitMC')
tileCoolMgr.addSource('onlEms',    '/TILE/ONL01/CALIB/EMS',     defConnStr, "", '/TILE/OFL02/CALIB/EMS',         'SplitMC')

#--- MuID
tileCoolMgr.addSource('onlMuID'  , '/TILE/ONL01/MUID',  defConnStr, "", '/TILE/ONL01/MUID',   'SplitMC')

#--- noise
tileCoolMgr.addSource('onlNoise1gOfni', '/TILE/ONL01/NOISE/OFNI', defConnStr, "", '/TILE/ONL01/NOISE/OFNI', 'SplitMC')


#--- status
tileCoolMgr.addSource('onlStatAdc', '/TILE/ONL01/STATUS/ADC', defConnStr, "", '/TILE/ONL01/STATUS/ADC', 'SplitMC')

#--- OFCs OF2
tileCoolMgr.addSource('OfcOf2CisPl100', '/TILE/ONL01/FILTER/OF2/CIS', defConnStr, "", '/TILE/ONL01/FILTER/OF2/CIS', 'SplitMC')
tileCoolMgr.addSource('OfcOf2CisPl5p2', '/TILE/ONL01/FILTER/OF2/CIS', defConnStr, "", '/TILE/ONL01/FILTER/OF2/CIS', 'SplitMC')
tileCoolMgr.addSource('OfcOf2Las',      '/TILE/ONL01/FILTER/OF2/LAS', defConnStr, "", '/TILE/ONL01/FILTER/OF2/LAS', 'SplitMC')

#--- OFCs OF1
tileCoolMgr.addSource('OfcOf1CisPl100', '/TILE/ONL01/FILTER/OF1/CIS', defConnStr, "", '/TILE/ONL01/FILTER/OF1/CIS', 'SplitMC')
tileCoolMgr.addSource('OfcOf1CisPl5p2', '/TILE/ONL01/FILTER/OF1/CIS', defConnStr, "", '/TILE/ONL01/FILTER/OF1/CIS', 'SplitMC')
tileCoolMgr.addSource('OfcOf1Las',      '/TILE/ONL01/FILTER/OF1/LAS', defConnStr, "", '/TILE/ONL01/FILTER/OF1/LAS', 'SplitMC')

def GetTileOfcCoolSource(ofcType, runType = 'PHY'):
    return 'Ofc' + string.capwords(ofcType,'/').replace('/','') + runType.lower().capitalize()

def AddTileOfcCoolSource(ofcType, runType = 'PHY', splitOnline = False):
    ofcSource = GetTileOfcCoolSource(ofcType, runType)
    ofcFullType = ofcType.upper() + '/' + runType.upper()
    ofcFolder = '/TILE/ONL01/FILTER/' + ofcFullType
    if splitOnline:
        ofcOfflineFolder = '/TILE/OFL02/FILTER/' + ofcFullType
        tileCoolMgr.addSource(ofcSource, ofcFolder, defConnStr, "", ofcOfflineFolder, 'SplitOnline')
    else:
        tileCoolMgr.addSource(ofcSource, ofcFolder, defConnStr, "", ofcFolder, 'SplitMC')

from IOVDbSvc.CondDB import conddb
if conddb.GetInstance() == 'CONDBR2':
    
    #--- OFCs
    tileCoolMgr.addSource('OfcOf2Phy',    '/TILE/ONL01/FILTER/OF2/PHY',  defConnStr, "", '/TILE/OFL02/FILTER/OF2/PHY', 'SplitOnline')
    tileCoolMgr.addSource('OfcOf1Phy',    '/TILE/ONL01/FILTER/OF1/PHY',  defConnStr, "", '/TILE/OFL02/FILTER/OF1/PHY', 'SplitOnline')

    #--- energy calibration
    tileCoolMgr.addSource('oflCisFitLin', '/TILE/ONL01/CALIB/CIS/LIN',   defConnStr, "", '/TILE/OFL02/CALIB/CIS/LIN', 'SplitOnline')
    tileCoolMgr.addSource('oflCisFitNln', '/TILE/ONL01/CALIB/CIS/NLN',   defConnStr, "", '/TILE/OFL02/CALIB/CIS/NLN', 'SplitOnline')
    tileCoolMgr.addSource('oflLasNln',    '/TILE/ONL01/CALIB/LAS/NLN',   defConnStr, "", '/TILE/OFL02/CALIB/LAS/NLN', 'SplitOnline')
    tileCoolMgr.addSource('oflLasFib',    '/TILE/OFL02/CALIB/LAS/FIBER', oflConnStr, "", "", 'OfflineOnly')
    
    #--- pulse shape
    tileCoolMgr.addSource('oflPlsPhy',      '/TILE/OFL02/PULSESHAPE/PHY',          oflConnStr, "", "", 'OfflineOnly')
    tileCoolMgr.addSource('oflPlsLas',      '/TILE/OFL02/PULSESHAPE/LAS',          oflConnStr, "", "", 'OfflineOnly')
    tileCoolMgr.addSource('oflPlsCisPl100', '/TILE/OFL02/PULSESHAPE/CIS/PULSE100', oflConnStr, "", "", 'OfflineOnly')
    tileCoolMgr.addSource('oflPlsCisPl5p2', '/TILE/OFL02/PULSESHAPE/CIS/PULSE5P2', oflConnStr, "", "", 'OfflineOnly')
    tileCoolMgr.addSource('oflPlsCisLk100', '/TILE/OFL02/PULSESHAPE/CIS/LEAK100',  oflConnStr, "", "", 'OfflineOnly')
    tileCoolMgr.addSource('oflPlsCisLk5p2', '/TILE/OFL02/PULSESHAPE/CIS/LEAK5P2',  oflConnStr, "", "", 'OfflineOnly')
    
    #--- noise
    tileCoolMgr.addSource('oflNoiseAdc', '/TILE/ONL01/NOISE/SAMPLE', defConnStr, "", '/TILE/OFL02/NOISE/SAMPLE', 'SplitOnline')
    tileCoolMgr.addSource('onlNoiseAdc', '/TILE/ONL01/NOISE/SAMPLE', defConnStr, "", "", 'OfflineOnly')
    tileCoolMgr.addSource('oflNoiseAcr', '/TILE/OFL02/NOISE/AUTOCR', oflConnStr, "", "", 'OfflineOnly')

    #--- timing
    tileCoolMgr.addSource('oflTimeCphy', '/TILE/ONL01/TIME/CHANNELOFFSET/PHY', defConnStr, "", '/TILE/OFL02/TIME/CHANNELOFFSET/PHY', 'SplitOnline')
    tileCoolMgr.addSource('onlTimeCphy', '/TILE/ONL01/TIME/CHANNELOFFSET/PHY', defConnStr, "", "", 'OfflineOnly')
    tileCoolMgr.addSource('oflTimeClas', '/TILE/ONL01/TIME/CHANNELOFFSET/LAS', defConnStr, "", '/TILE/OFL02/TIME/CHANNELOFFSET/LAS', 'SplitOnline')
    tileCoolMgr.addSource('onlTimeClas', '/TILE/ONL01/TIME/CHANNELOFFSET/LAS', defConnStr, "", "", 'OfflineOnly')
    tileCoolMgr.addSource('oflTimeCgapLas', '/TILE/ONL01/TIME/CHANNELOFFSET/GAP/LAS', defConnStr, "", '/TILE/OFL02/TIME/CHANNELOFFSET/GAP/LAS', 'SplitOnline')
    tileCoolMgr.addSource('onlTimeCgapLas', '/TILE/ONL01/TIME/CHANNELOFFSET/GAP/LAS', defConnStr, "", "", 'OfflineOnly')
    tileCoolMgr.addSource('oflTimeCcis', '/TILE/ONL01/TIME/CHANNELOFFSET/CIS', defConnStr, "", '/TILE/OFL02/TIME/CHANNELOFFSET/CIS', 'SplitOnline')
    tileCoolMgr.addSource('onlTimeCcis', '/TILE/ONL01/TIME/CHANNELOFFSET/CIS', defConnStr, "", "", 'OfflineOnly')

    #--- integrator
    tileCoolMgr.addSource('oflIntGain', '/TILE/ONL01/INTEGRATOR',  defConnStr, "", '/TILE/OFL02/INTEGRATOR',   'SplitOnline')

    #--- status
    tileCoolMgr.addSource('oflStatAdc', '/TILE/OFL02/STATUS/ADC', oflConnStr, "", "", 'OfflineOnly')


    #--- TMDB
    tileCoolMgr.addSource('onlTmdbThresholdPhy', '/TILE/ONL01/TMDB/THRESHOLD/PHY', defConnStr, "", '/TILE/ONL01/TMDB/THRESHOLD/PHY', 'SplitMC')
    tileCoolMgr.addSource('onlTmdbThresholdLas', '/TILE/ONL01/TMDB/THRESHOLD/LAS', defConnStr, "", '/TILE/ONL01/TMDB/THRESHOLD/LAS', 'SplitMC')
    tileCoolMgr.addSource('onlTmdbThresholdCis', '/TILE/ONL01/TMDB/THRESHOLD/CIS', defConnStr, "", '/TILE/ONL01/TMDB/THRESHOLD/CIS', 'SplitMC')

    tileCoolMgr.addSource('onlTmdbDelayPhy', '/TILE/ONL01/TMDB/DELAY/PHY', defConnStr, "", '/TILE/ONL01/TMDB/DELAY/PHY', 'SplitMC')
    tileCoolMgr.addSource('onlTmdbDelayLas', '/TILE/ONL01/TMDB/DELAY/LAS', defConnStr, "", '/TILE/ONL01/TMDB/DELAY/LAS', 'SplitMC')
    tileCoolMgr.addSource('onlTmdbDelayCis', '/TILE/ONL01/TMDB/DELAY/CIS', defConnStr, "", '/TILE/ONL01/TMDB/DELAY/CIS', 'SplitMC')

    tileCoolMgr.addSource('onlTmdbTmfPhy', '/TILE/ONL01/TMDB/TMF/PHY', defConnStr, "", '/TILE/ONL01/TMDB/TMF/PHY', 'SplitMC')
    tileCoolMgr.addSource('onlTmdbTmfLas', '/TILE/ONL01/TMDB/TMF/LAS', defConnStr, "", '/TILE/ONL01/TMDB/TMF/LAS', 'SplitMC')
    tileCoolMgr.addSource('onlTmdbTmfCis', '/TILE/ONL01/TMDB/TMF/CIS', defConnStr, "", '/TILE/ONL01/TMDB/TMF/CIS', 'SplitMC')

    tileCoolMgr.addSource('onlTmdbCalibPhy', '/TILE/ONL01/TMDB/CALIB/PHY', defConnStr, "", '/TILE/ONL01/TMDB/CALIB/PHY', 'SplitMC')

    #--- noise
    tileCoolMgr.addSource('oflDspThreshold', '/TILE/ONL01/DSP/THRESHOLD', defConnStr, "", '/TILE/OFL02/DSP/THRESHOLD', 'SplitOnline')

    
else:

    #--- OFCs
    tileCoolMgr.addSource('OfcOf2Phy',      '/TILE/ONL01/FILTER/OF2/PHY', defConnStr, "", '/TILE/ONL01/FILTER/OF2/PHY', 'SplitMC')
    tileCoolMgr.addSource('OfcOf1Phy',      '/TILE/ONL01/FILTER/OF1/PHY', defConnStr, "", '/TILE/ONL01/FILTER/OF1/PHY', 'SplitMC')

    #--- energy calibration
    tileCoolMgr.addSource('oflCisFitLin', '/TILE/ONL01/CALIB/CIS/LIN', defConnStr, "", '/TILE/OFL02/CALIB/CIS/FIT/LIN', 'SplitOnline')
    tileCoolMgr.addSource('oflCisFitNln', '/TILE/OFL01/CALIB/CIS/FIT/NLN', defConnStr, "", '/TILE/OFL02/CALIB/CIS/FIT/NLN', 'SplitOnline')
    tileCoolMgr.addSource('oflLasNln',    '/TILE/OFL01/CALIB/LAS/NLN',     defConnStr, "", '/TILE/OFL02/CALIB/LAS/NLN',     'SplitOnline')
    tileCoolMgr.addSource('oflLasFib',    '/TILE/OFL01/CALIB/LAS/FIBER',   defConnStr, "", '/TILE/OFL02/CALIB/LAS/FIBER',   'SplitOnline')    
    #--- pulse shape
    tileCoolMgr.addSource('oflPlsPhy',      '/TILE/OFL01/PULSESHAPE/PHY',          defConnStr, "", '/TILE/OFL02/PULSESHAPE/PHY',          'SplitOnline')
    tileCoolMgr.addSource('oflPlsLas',      '/TILE/OFL01/PULSESHAPE/LAS',          defConnStr, "", '/TILE/OFL02/PULSESHAPE/LAS',          'SplitOnline')
    tileCoolMgr.addSource('oflPlsCisPl100', '/TILE/OFL01/PULSESHAPE/CIS/PULSE100', defConnStr, "", '/TILE/OFL02/PULSESHAPE/CIS/PULSE100', 'SplitOnline')
    tileCoolMgr.addSource('oflPlsCisPl5p2', '/TILE/OFL01/PULSESHAPE/CIS/PULSE5P2', defConnStr, "", '/TILE/OFL02/PULSESHAPE/CIS/PULSE5P2', 'SplitOnline')
    tileCoolMgr.addSource('oflPlsCisLk100', '/TILE/OFL01/PULSESHAPE/CIS/LEAK100',  defConnStr, "", '/TILE/OFL02/PULSESHAPE/CIS/LEAK100',  'SplitOnline')
    tileCoolMgr.addSource('oflPlsCisLk5p2', '/TILE/OFL01/PULSESHAPE/CIS/LEAK5P2',  defConnStr, "", '/TILE/OFL02/PULSESHAPE/CIS/LEAK5P2',  'SplitOnline')
    
    #--- noise
    tileCoolMgr.addSource('oflNoiseAdc', '/TILE/OFL01/NOISE/SAMPLE', defConnStr, "", '/TILE/OFL02/NOISE/SAMPLE', 'SplitOnline')
    tileCoolMgr.addSource('oflNoiseAcr', '/TILE/OFL01/NOISE/AUTOCR', defConnStr, "", '/TILE/OFL02/NOISE/AUTOCR', 'SplitOnline')

    #--- timing
    tileCoolMgr.addSource('oflTimeCphy', '/TILE/OFL01/TIME/CHANNELOFFSET/PHY', defConnStr, "", '/TILE/OFL02/TIME/CHANNELOFFSET/PHY', 'SplitOnline')
    tileCoolMgr.addSource('oflTimeClas', '/TILE/OFL01/TIME/CHANNELOFFSET/LAS', defConnStr, "", '/TILE/OFL02/TIME/CHANNELOFFSET/LAS', 'SplitOnline')
    tileCoolMgr.addSource('oflTimeCcis', '/TILE/OFL01/TIME/CHANNELOFFSET/CIS', defConnStr, "", '/TILE/OFL02/TIME/CHANNELOFFSET/CIS', 'SplitOnline')

    #--- integrator
    tileCoolMgr.addSource('oflIntGain', '/TILE/OFL01/INTEGRATOR',  defConnStr, "", '/TILE/OFL02/INTEGRATOR',   'SplitOnline')

    #--- status
    tileCoolMgr.addSource('oflStatAdc', '/TILE/OFL01/STATUS/ADC', defConnStr, "", '/TILE/OFL02/STATUS/ADC', 'SplitOnline')

