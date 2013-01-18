# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from PmbUtils.CoolDozerAccessUtils import CDJobDefinitionBase

class PhysMonDozerJobDef(CDJobDefinitionBase):
    def __init__(self,dbjobkey,dbjobpackage,trendid,jobname,dbChannelName):
        self.__dbjobkey=dbjobkey
        if '/' in dbjobpackage:
            from os.path import basename
            self.__dbjobpackage=basename(dbjobpackage)
            self.__dbjobpackagefullpath=dbjobpackage
        else:
            self.__dbjobpackage=dbjobpackage
            self.__dbjobpackagefullpath=None
        self.__trendid=trendid
        self.__dbjobname=trendid+"_MuonSummaryRTTDozerApp"
        self.__jobname=jobname
        self.__dbChannelName=dbChannelName
    def getDBJobKey(self):
        return self.__dbjobkey
    def getDBJobPackage(self):
        return self.__dbjobpackage
    def getDBJobPackageFullPath(self):
        return self.__dbjobpackagefullpath
    def getDBJobName(self):
        return self.__dbjobname
    def getJobName(self):
        return self.__jobname

    def getTrendId(self):
        return self.__trendid
    def getDBChannelName(self):
        return self.__dbChannelName

    def getFields(self):
        return {
          "ScatPerTrk":"Scat/Tk", 
          "Chi2perDOFperTrk":"Ch2/dof/T",
          "TrkPerEvt":"Trk/Evt", 
          "HolesPerTrk":"Hole/Tk", 
          "TrigEtaPerTrk":"TrigEta/T", 
          "NumTracks":"Tracks",
          "EtaPerTrk":"Eta/Trk",
          "PhiPerTrk":"Phi/Trk", 
          "HitPerTrk":"Hit/Trk", 
          "NumEvents":"Events"
        }
    def getJobs(self):
        return ['Zmumu_MboyAndMoreOnly_MuonSummaryRTTDozerApp','MuonDataRec_fromBS_MuonSummaryRTTDozerApp']

