# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Miscellaneous utilities related to COOL.
"""
__author__  = 'Juerg Beringer'
__version__ = 'COOLUtils.py atlas/athena'

import os, time,sys

from PyCool import cool
from CoolConvUtilities import AtlCoolLib

# For resolving tags
sys.path.append('/afs/cern.ch/user/a/atlcond/utils22/CondUtilsLib/')
sys.path.append('/afs/cern.ch/user/a/atlcond/utils22/')


#
# Utility function to open a COOL SQLite beam spot file.
#
def openBeamSpotDbFile(fileName, forceNew=False, folderName='/Indet/Beampos', dbName='BEAMSPOT'):
    """Open a beam spot SQLite COOL file and get a `folderHandle` to the beam spot folder. If the folder
       doesn't exist yet, it is created. If the SQLite file doesn't exist, it is created. If forceNew=True,
       any previously existing output file with the specified name is overwritten."""
    connString =  'sqlite://;schema=%s;dbname=%s' % (fileName,dbName)
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    if forceNew and os.path.exists(fileName):
        os.remove(fileName)
    if os.path.exists(fileName):
        db = dbSvc.openDatabase(connString,False)
    else:
        db = dbSvc.createDatabase(connString)

    # Make sure we have /Indet/Beampos with the correct schema
    spec=cool.RecordSpecification()
    spec.extend("status",cool.StorageType.Int32)
    spec.extend("posX",cool.StorageType.Float)
    spec.extend("posY",cool.StorageType.Float)
    spec.extend("posZ",cool.StorageType.Float)
    spec.extend("sigmaX",cool.StorageType.Float)
    spec.extend("sigmaY",cool.StorageType.Float)
    spec.extend("sigmaZ",cool.StorageType.Float)
    spec.extend("tiltX",cool.StorageType.Float)
    spec.extend("tiltY",cool.StorageType.Float)
    spec.extend("sigmaXY",cool.StorageType.Float)
    spec.extend("posXErr",cool.StorageType.Float)
    spec.extend("posYErr",cool.StorageType.Float)
    spec.extend("posZErr",cool.StorageType.Float)
    spec.extend("sigmaXErr",cool.StorageType.Float)
    spec.extend("sigmaYErr",cool.StorageType.Float)
    spec.extend("sigmaZErr",cool.StorageType.Float)
    spec.extend("tiltXErr",cool.StorageType.Float)
    spec.extend("tiltYErr",cool.StorageType.Float)
    spec.extend("sigmaXYErr",cool.StorageType.Float)

    folder = AtlCoolLib.ensureFolder(db,folderName,spec,AtlCoolLib.athenaDesc(True,'AthenaAttributeList'),cool.FolderVersioning.MULTI_VERSION)

    folderHandle = (db,folder,spec)
    return folderHandle


def writeBeamSpotEntry(folderHandle, tag='nominal',
                       runMin=0, runMax=(1 << 31)-1, lbMin=0, lbMax=(1 << 32)-2,
                       status=0,
                       posX=0., posY=0., posZ=0.,
                       sigmaX=30., sigmaY=30., sigmaZ=500.,
                       tiltX=0., tiltY=0.,
                       sigmaXY=0.,
                       posXErr=0., posYErr=0., posZErr=0.,
                       sigmaXErr=0., sigmaYErr=0., sigmaZErr=0.,
                       tiltXErr=0., tiltYErr=0.,
                       sigmaXYErr=0.):
    """Write a beam spot entry for a given IOV into a beam spot folder whose 'folderHandle' is passsed.
       The IOV is specified in terms of run and LB range. Note that lbMax is inclusive.
       The default parameters for the position and tilt are zero, the ones for the widths are large
       non-constraining widths of 30mm (500mm) transverse (longitudinal)."""
    since = (runMin << 32)+lbMin
    until = (runMax << 32)+lbMax+1
    payload=cool.Record(folderHandle[2])
    payload['status'] = int(status)
    payload['posX'] = float(posX)
    payload['posY'] = float(posY)
    payload['posZ'] = float(posZ)
    payload['sigmaX'] = float(sigmaX)
    payload['sigmaY'] = float(sigmaY)
    payload['sigmaZ'] = float(sigmaZ)
    payload['tiltX'] = float(tiltX)
    payload['tiltY'] = float(tiltY)
    payload['sigmaXY'] = float(sigmaXY)
    payload['posXErr'] = float(posXErr)
    payload['posYErr'] = float(posYErr)
    payload['posZErr'] = float(posZErr)
    payload['sigmaXErr'] = float(sigmaXErr)
    payload['sigmaYErr'] = float(sigmaYErr)
    payload['sigmaZErr'] = float(sigmaZErr)
    payload['tiltXErr'] = float(tiltXErr)
    payload['tiltYErr'] = float(tiltYErr)
    payload['sigmaXYErr'] = float(sigmaXYErr)

    if tag=='HEAD':
        folderHandle[1].storeObject(since,until,payload,0)
    else:
        folderHandle[1].storeObject(since,until,payload,0,tag)



#
# Utility function to convert from COOL time to Unix time (seconds since epoch)
#
def COOLToUnixTime(coolTime):
    if coolTime is not None:
        return int(coolTime/1000000000)
    else:
        return


#
# Class to convert query COOL for various quantities related to runs,
# such as start and end times of runs and LBs, LHC fill number, etc.
#
class COOLQuery:
    """Utility to query COOL to retrieve start and end time of run and LBs."""
    def __init__(self,useOracle=False,debug=True):

        self.tdaqdbname='COOLONL_TDAQ/CONDBR2'
        self.coolpath='/TDAQ/RunCtrl'

        self.trigdbname='COOLONL_TRIGGER/CONDBR2'
        self.coollbpath='/TRIGGER/LUMI/LBLB'

        self.dcsdbname = 'COOLOFL_DCS/CONDBR2'
        self.coollhcpath = '/LHC/DCS/FILLSTATE'

        self.oracle = useOracle
        self.debug = debug

        print ('open cool db' )
        self.cooldb = AtlCoolLib.indirectOpen(self.tdaqdbname, True, self.oracle, self.debug)
        print ('open cooltrig db')
        self.cooltrigdb = AtlCoolLib.indirectOpen(self.trigdbname, True, self.oracle, self.debug)
        print ('open cooldcs db')
        self.cooldcsdb = AtlCoolLib.indirectOpen(self.dcsdbname, True, self.oracle, self.debug)
         
        self.lbDictCache = {'runnr': None, 'lbDict': None}

    def __del__(self):
        try:
          self.cooldb.closeDatabase()
          self.cooltrigdb.closeDatabase()
          self.cooldcsdb.closeDatabase()
        except Exception:
          print ("DB time out -- ignore")

    def getRunStartTime(self,runnr):
        """Get start time of run in Unix time (seconds since epoch)."""
        iov=runnr << 32
        if (iov>cool.ValidityKeyMax): iov=cool.ValidityKeyMax
        folderSOR_Params = self.cooldb.getFolder(self.coolpath+'/SOR_Params')
        itr = folderSOR_Params.browseObjects(iov, iov, cool.ChannelSelection.all())
        try:
            itr.goToNext()
            obj = itr.currentRef()
            sorTime = obj.payload()['SORTime']
            return COOLToUnixTime(sorTime)
        except Exception:
            return


    def getLHCInfo(self,timeSinceEpochInSec):
        """Get LHC fill and other info from COOL. The relevant COOL folder,
           /LHC/DCS/FILLSTATE is time-based, so the iov must be specified in
           ns since the epoch, but the argument to getLHCInfo is s since the
           epoch for convenience."""
        t = timeSinceEpochInSec*1000000000
        lhcfolder = self.cooldcsdb.getFolder(self.coollhcpath)
        itr = lhcfolder.browseObjects(t,t,cool.ChannelSelection.all())
        try:
            info = { 'FillNumber': 0,
                     'StableBeams': False,
                     'BeamEnergyGeV': 0,
                     'NumBunchColl': 0 }
            itr.goToNext()
            obj = itr.currentRef()
            for k in info.keys():
                try:
                    info[k] = obj.payload()[k]
                except Exception:
                    print ('WARNING: Cannot find value for',k)
            return info
        except Exception:
            return None

    def getRunEndTime(self,runnr):
        """Get end time of run in Unix time (seconds since epoch)."""
        iov=runnr << 32
        if (iov>cool.ValidityKeyMax): iov=cool.ValidityKeyMax
        folderEOR_Params = self.cooldb.getFolder(self.coolpath+'/EOR')
        itr = folderEOR_Params.browseObjects(iov, iov, cool.ChannelSelection.all())
        try:
            itr.goToNext()
            obj = itr.currentRef()
            eorTime = obj.payload()['EORTime']
            return COOLToUnixTime(eorTime)
        except Exception:
            return

    def getLbTimes(self,runnr):
        """Get dict of LB start and end times in Unix time (seconds since epoch)."""
        iov1=runnr << 32
        iov2=(runnr+1) << 32
        if (iov2>cool.ValidityKeyMax): iov2=cool.ValidityKeyMax
        folderLB_Params = self.cooltrigdb.getFolder(self.coollbpath)
        itr = folderLB_Params.browseObjects(iov1, iov2, cool.ChannelSelection.all())
        lbDict = { }
        while itr.goToNext():
            obj = itr.currentRef()
            since = obj.since()
            run = since >> 32
            lb = since & 0xFFFFFFFF
            if (run != runnr): continue

            tlo = obj.payload()['StartTime']
            thi = obj.payload()['EndTime']
            lbDict[lb] = (COOLToUnixTime(tlo),COOLToUnixTime(thi))
        return lbDict

    def lbTime(self,runnr,lbnr):
        """Get (startTime,endTime) for a given LB. The LB information is cached
           for the last run, in order make this efficient for querying for the
           times of individual LBs."""
        runnr = int(runnr)
        if self.lbDictCache['runnr']!=runnr:
            # print ('Caching lbDict for run',runnr)
            self.lbDictCache['lbDict'] = self.getLbTimes(runnr)
            self.lbDictCache['runnr'] = runnr
        return self.lbDictCache['lbDict'].get(lbnr,None)

def resolveCurrentAlias(tagtype='ST'):
    "Resolve the current BLK tag alias"

    from AtlCoolBKLib import resolveAlias

    # Will raise exception if alias not defined
    alias = resolveAlias.getCurrent()
    return alias.replace('*', tagtype)

def resolveNextAlias(tagtype='ST'):
    "Resolve the next BLK tag alias"
    
    from AtlCoolBKLib import resolveAlias

    # Returns an empty string if Next not existing or exception thrown
    
    alias = ''
    try:
        alias = resolveAlias.getNext()
    except Exception:
        alias = ''
        
    return alias.replace('*', tagtype)


def resolveCurrentBeamSpotFolder(db = 'COOLOFL_INDET/CONDBR2', folder = '/Indet/Beampos'):
    """
    Resolve the beamspot folder tag for the current BLK tag alisa
    """
    tag = resolveCurrentAlias()
    return resolveBLKTag(tag)

def resolveNextBeamSpotFolder(db = 'COOLOFL_INDET/CONDBR2', folder = '/Indet/Beampos'):
    """
    Resolve the beamspot folder tag for the next BLK tag alias
    """

    tag = resolveNextAlias()
    if tag == '': return ''
    
    return resolveBLKTag(tag)

def resolveBLKTag(blktag, db = 'COOLOFL_INDET/CONDBR2', folder = '/Indet/Beampos'):
    """
    Resolve a global tag into the corresponding tag for given folder in the database specified
    """

    dbSvc = cool.DatabaseSvcFactory.databaseService()
    dbconn = dbSvc.openDatabase(db)
    folder = dbconn.getFolder(folder)
    try:
        tag = folder.resolveTag(blktag)
    finally:
        del dbconn

    return tag

# Test code for modules
if __name__ == '__main__':
    c = COOLQuery()
    print (time.strftime('%c', time.gmtime(c.getRunStartTime(142191)))) # in UTC
    print (time.strftime('%c', time.gmtime(c.getRunStartTime(142193)))) # in UTC
    print (time.strftime('%c', time.gmtime(c.getRunEndTime(142193))))
    lbDict = c.getLbTimes(142193)
    for l in lbDict.keys():
        print (l,time.ctime(lbDict[l][0]),'-',time.ctime(lbDict[l][1]))   # in local time zone

