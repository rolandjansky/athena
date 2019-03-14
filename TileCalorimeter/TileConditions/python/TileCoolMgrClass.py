# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#file: TileCoolMgr.py
#author: nils.gollub@cern.ch
#modifications: lukas.pribyl@cern.ch

class TileCoolMgr:
    
    #_______________________________________________________________
    def __init__(self,isMC,isOnline,dbInstance):

        #=== initialize logger with INFO level
        from AthenaCommon.Logging import logging
        self.__log = logging.getLogger( 'TileCoolMgr' )
        #        self.__log.setLevel(logging.INFO)
        
        #=== Master dictionary for: condId -> [folder, connStr, tag, folder2, type]
        self.__idDict = {}

        #=== Available folder types
        self.__validTypes = ["SplitMC", "SplitOnline", "OfflineOnly", "Sqlite"]

        #=== do we run on real data (not MC)?
        self._isMC=isMC
        self._isOnline=isOnline
        self._dbInstance=dbInstance

        self.fillMe()


    def isMC(self):
        return self._isMC

    def isOnline(self):
        return self._isOnline

        
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




    def fillMe(self):
        defConnStr = 'TILE'
        oflConnStr = 'TILE_OFL'
        #--- energy calibration

        self.addSource('oflLasLin',    '/TILE/ONL01/CALIB/LAS/LIN', defConnStr, "", '/TILE/OFL02/CALIB/LAS/LIN',     'SplitOnline')
        self.addSource('oflCes',       '/TILE/ONL01/CALIB/CES',     defConnStr, "", '/TILE/OFL02/CALIB/CES',         'SplitOnline')
        self.addSource('oflEms',       '/TILE/ONL01/CALIB/EMS',     defConnStr, "", '/TILE/OFL02/CALIB/EMS',         'SplitOnline')

        #--- online calib is taken from the folder tag defined in TileOfcCoolPlugin 
        #--- the tags are fixed only in the case of real data - in both online and offline schema.
        self.addSource('onlCisLin', '/TILE/ONL01/CALIB/CIS/LIN', defConnStr, "", '/TILE/OFL02/CALIB/CIS/FIT/LIN', 'SplitMC')
        self.addSource('onlLasLin', '/TILE/ONL01/CALIB/LAS/LIN', defConnStr, "", '/TILE/OFL02/CALIB/LAS/LIN',     'SplitMC')
        self.addSource('onlCes',    '/TILE/ONL01/CALIB/CES',     defConnStr, "", '/TILE/OFL02/CALIB/CES',         'SplitMC')
        self.addSource('onlEms',    '/TILE/ONL01/CALIB/EMS',     defConnStr, "", '/TILE/OFL02/CALIB/EMS',         'SplitMC')

        #--- MuID
        self.addSource('onlMuID'  , '/TILE/ONL01/MUID',  defConnStr, "", '/TILE/ONL01/MUID',   'SplitMC')

        #--- noise
        self.addSource('onlNoise1gOfni', '/TILE/ONL01/NOISE/OFNI', defConnStr, "", '/TILE/ONL01/NOISE/OFNI', 'SplitMC')


        #--- status
        self.addSource('onlStatAdc', '/TILE/ONL01/STATUS/ADC', defConnStr, "", '/TILE/ONL01/STATUS/ADC', 'SplitMC')

        #--- OFCs OF2
        self.addSource('OfcOf2CisPl100', '/TILE/ONL01/FILTER/OF2/CIS', defConnStr, "", '/TILE/ONL01/FILTER/OF2/CIS', 'SplitMC')
        self.addSource('OfcOf2CisPl5p2', '/TILE/ONL01/FILTER/OF2/CIS', defConnStr, "", '/TILE/ONL01/FILTER/OF2/CIS', 'SplitMC')
        self.addSource('OfcOf2Las',      '/TILE/ONL01/FILTER/OF2/LAS', defConnStr, "", '/TILE/ONL01/FILTER/OF2/LAS', 'SplitMC')

        #--- OFCs OF1
        self.addSource('OfcOf1CisPl100', '/TILE/ONL01/FILTER/OF1/CIS', defConnStr, "", '/TILE/ONL01/FILTER/OF1/CIS', 'SplitMC')
        self.addSource('OfcOf1CisPl5p2', '/TILE/ONL01/FILTER/OF1/CIS', defConnStr, "", '/TILE/ONL01/FILTER/OF1/CIS', 'SplitMC')
        self.addSource('OfcOf1Las',      '/TILE/ONL01/FILTER/OF1/LAS', defConnStr, "", '/TILE/ONL01/FILTER/OF1/LAS', 'SplitMC')


        if (self._dbInstance == 'CONDBR2'):
            #--- OFCs
            self.addSource('OfcOf2Phy',    '/TILE/ONL01/FILTER/OF2/PHY',  defConnStr, "", '/TILE/OFL02/FILTER/OF2/PHY', 'SplitOnline')
            self.addSource('OfcOf1Phy',    '/TILE/ONL01/FILTER/OF1/PHY',  defConnStr, "", '/TILE/OFL02/FILTER/OF1/PHY', 'SplitOnline')

            #--- energy calibration
            self.addSource('oflCisFitLin', '/TILE/ONL01/CALIB/CIS/LIN',   defConnStr, "", '/TILE/OFL02/CALIB/CIS/LIN', 'SplitOnline')
            self.addSource('oflCisFitNln', '/TILE/ONL01/CALIB/CIS/NLN',   defConnStr, "", '/TILE/OFL02/CALIB/CIS/NLN', 'SplitOnline')
            self.addSource('oflLasNln',    '/TILE/ONL01/CALIB/LAS/NLN',   defConnStr, "", '/TILE/OFL02/CALIB/LAS/NLN', 'SplitOnline')
            self.addSource('oflLasFib',    '/TILE/OFL02/CALIB/LAS/FIBER', oflConnStr, "", "", 'OfflineOnly')
    
            #--- pulse shape
            self.addSource('oflPlsPhy',      '/TILE/OFL02/PULSESHAPE/PHY',          oflConnStr, "", "", 'OfflineOnly')
            self.addSource('oflPlsLas',      '/TILE/OFL02/PULSESHAPE/LAS',          oflConnStr, "", "", 'OfflineOnly')
            self.addSource('oflPlsCisPl100', '/TILE/OFL02/PULSESHAPE/CIS/PULSE100', oflConnStr, "", "", 'OfflineOnly')
            self.addSource('oflPlsCisPl5p2', '/TILE/OFL02/PULSESHAPE/CIS/PULSE5P2', oflConnStr, "", "", 'OfflineOnly')
            self.addSource('oflPlsCisLk100', '/TILE/OFL02/PULSESHAPE/CIS/LEAK100',  oflConnStr, "", "", 'OfflineOnly')
            self.addSource('oflPlsCisLk5p2', '/TILE/OFL02/PULSESHAPE/CIS/LEAK5P2',  oflConnStr, "", "", 'OfflineOnly')
    
            #--- noise
            self.addSource('oflNoiseAdc', '/TILE/ONL01/NOISE/SAMPLE', defConnStr, "", '/TILE/OFL02/NOISE/SAMPLE', 'SplitOnline')
            self.addSource('onlNoiseAdc', '/TILE/ONL01/NOISE/SAMPLE', defConnStr, "", "", 'OfflineOnly')
            self.addSource('oflNoiseAcr', '/TILE/OFL02/NOISE/AUTOCR', oflConnStr, "", "", 'OfflineOnly')

            #--- timing
            self.addSource('oflTimeCphy', '/TILE/ONL01/TIME/CHANNELOFFSET/PHY', defConnStr, "", '/TILE/OFL02/TIME/CHANNELOFFSET/PHY', 'SplitOnline')
            self.addSource('onlTimeCphy', '/TILE/ONL01/TIME/CHANNELOFFSET/PHY', defConnStr, "", "", 'OfflineOnly')
            self.addSource('oflTimeClas', '/TILE/ONL01/TIME/CHANNELOFFSET/LAS', defConnStr, "", '/TILE/OFL02/TIME/CHANNELOFFSET/LAS', 'SplitOnline')
            self.addSource('onlTimeClas', '/TILE/ONL01/TIME/CHANNELOFFSET/LAS', defConnStr, "", "", 'OfflineOnly')
            self.addSource('oflTimeCgapLas', '/TILE/ONL01/TIME/CHANNELOFFSET/GAP/LAS', defConnStr, "", '/TILE/OFL02/TIME/CHANNELOFFSET/GAP/LAS', 'SplitOnline')
            self.addSource('onlTimeCgapLas', '/TILE/ONL01/TIME/CHANNELOFFSET/GAP/LAS', defConnStr, "", "", 'OfflineOnly')
            self.addSource('oflTimeCcis', '/TILE/ONL01/TIME/CHANNELOFFSET/CIS', defConnStr, "", '/TILE/OFL02/TIME/CHANNELOFFSET/CIS', 'SplitOnline')
            self.addSource('onlTimeCcis', '/TILE/ONL01/TIME/CHANNELOFFSET/CIS', defConnStr, "", "", 'OfflineOnly')

            #--- integrator
            self.addSource('oflIntGain', '/TILE/ONL01/INTEGRATOR',  defConnStr, "", '/TILE/OFL02/INTEGRATOR',   'SplitOnline')

            #--- status
            self.addSource('oflStatAdc', '/TILE/OFL02/STATUS/ADC', oflConnStr, "", "", 'OfflineOnly')


            #--- TMDB
            self.addSource('onlTmdbThresholdPhy', '/TILE/ONL01/TMDB/THRESHOLD/PHY', defConnStr, "", '/TILE/ONL01/TMDB/THRESHOLD/PHY', 'SplitMC')
            self.addSource('onlTmdbThresholdLas', '/TILE/ONL01/TMDB/THRESHOLD/LAS', defConnStr, "", '/TILE/ONL01/TMDB/THRESHOLD/LAS', 'SplitMC')
            self.addSource('onlTmdbThresholdCis', '/TILE/ONL01/TMDB/THRESHOLD/CIS', defConnStr, "", '/TILE/ONL01/TMDB/THRESHOLD/CIS', 'SplitMC')
            
            self.addSource('onlTmdbDelayPhy', '/TILE/ONL01/TMDB/DELAY/PHY', defConnStr, "", '/TILE/ONL01/TMDB/DELAY/PHY', 'SplitMC')
            self.addSource('onlTmdbDelayLas', '/TILE/ONL01/TMDB/DELAY/LAS', defConnStr, "", '/TILE/ONL01/TMDB/DELAY/LAS', 'SplitMC')
            self.addSource('onlTmdbDelayCis', '/TILE/ONL01/TMDB/DELAY/CIS', defConnStr, "", '/TILE/ONL01/TMDB/DELAY/CIS', 'SplitMC')
            
            self.addSource('onlTmdbTmfPhy', '/TILE/ONL01/TMDB/TMF/PHY', defConnStr, "", '/TILE/ONL01/TMDB/TMF/PHY', 'SplitMC')
            self.addSource('onlTmdbTmfLas', '/TILE/ONL01/TMDB/TMF/LAS', defConnStr, "", '/TILE/ONL01/TMDB/TMF/LAS', 'SplitMC')
            self.addSource('onlTmdbTmfCis', '/TILE/ONL01/TMDB/TMF/CIS', defConnStr, "", '/TILE/ONL01/TMDB/TMF/CIS', 'SplitMC')
            
            self.addSource('onlTmdbCalibPhy', '/TILE/ONL01/TMDB/CALIB/PHY', defConnStr, "", '/TILE/ONL01/TMDB/CALIB/PHY', 'SplitMC')

            #--- noise
            self.addSource('oflDspThreshold', '/TILE/ONL01/DSP/THRESHOLD', defConnStr, "", '/TILE/OFL02/DSP/THRESHOLD', 'SplitOnline')
        else:
            
            #--- OFCs
            self.addSource('OfcOf2Phy',      '/TILE/ONL01/FILTER/OF2/PHY', defConnStr, "", '/TILE/ONL01/FILTER/OF2/PHY', 'SplitMC')
            self.addSource('OfcOf1Phy',      '/TILE/ONL01/FILTER/OF1/PHY', defConnStr, "", '/TILE/ONL01/FILTER/OF1/PHY', 'SplitMC')

            #--- energy calibration
            self.addSource('oflCisFitLin', '/TILE/ONL01/CALIB/CIS/LIN', defConnStr, "", '/TILE/OFL02/CALIB/CIS/FIT/LIN', 'SplitOnline')
            self.addSource('oflCisFitNln', '/TILE/OFL01/CALIB/CIS/FIT/NLN', defConnStr, "", '/TILE/OFL02/CALIB/CIS/FIT/NLN', 'SplitOnline')
            self.addSource('oflLasNln',    '/TILE/OFL01/CALIB/LAS/NLN',     defConnStr, "", '/TILE/OFL02/CALIB/LAS/NLN',     'SplitOnline')
            self.addSource('oflLasFib',    '/TILE/OFL01/CALIB/LAS/FIBER',   defConnStr, "", '/TILE/OFL02/CALIB/LAS/FIBER',   'SplitOnline')    
            #--- pulse shape
            self.addSource('oflPlsPhy',      '/TILE/OFL01/PULSESHAPE/PHY',          defConnStr, "", '/TILE/OFL02/PULSESHAPE/PHY',          'SplitOnline')
            self.addSource('oflPlsLas',      '/TILE/OFL01/PULSESHAPE/LAS',          defConnStr, "", '/TILE/OFL02/PULSESHAPE/LAS',          'SplitOnline')
            self.addSource('oflPlsCisPl100', '/TILE/OFL01/PULSESHAPE/CIS/PULSE100', defConnStr, "", '/TILE/OFL02/PULSESHAPE/CIS/PULSE100', 'SplitOnline')
            self.addSource('oflPlsCisPl5p2', '/TILE/OFL01/PULSESHAPE/CIS/PULSE5P2', defConnStr, "", '/TILE/OFL02/PULSESHAPE/CIS/PULSE5P2', 'SplitOnline')
            self.addSource('oflPlsCisLk100', '/TILE/OFL01/PULSESHAPE/CIS/LEAK100',  defConnStr, "", '/TILE/OFL02/PULSESHAPE/CIS/LEAK100',  'SplitOnline')
            self.addSource('oflPlsCisLk5p2', '/TILE/OFL01/PULSESHAPE/CIS/LEAK5P2',  defConnStr, "", '/TILE/OFL02/PULSESHAPE/CIS/LEAK5P2',  'SplitOnline')
    
            #--- noise
            self.addSource('oflNoiseAdc', '/TILE/OFL01/NOISE/SAMPLE', defConnStr, "", '/TILE/OFL02/NOISE/SAMPLE', 'SplitOnline')
            self.addSource('oflNoiseAcr', '/TILE/OFL01/NOISE/AUTOCR', defConnStr, "", '/TILE/OFL02/NOISE/AUTOCR', 'SplitOnline')
            
            #--- timing
            self.addSource('oflTimeCphy', '/TILE/OFL01/TIME/CHANNELOFFSET/PHY', defConnStr, "", '/TILE/OFL02/TIME/CHANNELOFFSET/PHY', 'SplitOnline')
            self.addSource('oflTimeClas', '/TILE/OFL01/TIME/CHANNELOFFSET/LAS', defConnStr, "", '/TILE/OFL02/TIME/CHANNELOFFSET/LAS', 'SplitOnline')
            self.addSource('oflTimeCcis', '/TILE/OFL01/TIME/CHANNELOFFSET/CIS', defConnStr, "", '/TILE/OFL02/TIME/CHANNELOFFSET/CIS', 'SplitOnline')

            #--- integrator
            self.addSource('oflIntGain', '/TILE/OFL01/INTEGRATOR',  defConnStr, "", '/TILE/OFL02/INTEGRATOR',   'SplitOnline')

            #--- status
            self.addSource('oflStatAdc', '/TILE/OFL01/STATUS/ADC', defConnStr, "", '/TILE/OFL02/STATUS/ADC', 'SplitOnline')
            pass



def TileCoolMgrConfig(configflags):
    #Instantiate TileCoolMgr based on run3-style config flags
    return TileCoolMgr(isMC=configFlags.Input.isMC, 
                       isOnline=configFlags.common.isOnline,
                       dbInstance=configFlags.IOVDb.DatabaseInstance)
    
