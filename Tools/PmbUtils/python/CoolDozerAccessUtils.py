# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#  Initial version: August 2009

import datetime,time

class CDJobDefinitionBase:
    #NB: CD is short for CoolDozer

    #Reimplement the following methods as necessary:

    #Methods defining in which path to look in the database:
    def getDBJobKey(self):
        import os.path
        return os.path.join('dev','build','i686-slc4-gcc34-opt','AtlasProduction')

    def getDBJobPackage(self):
        assert False, "Must reimplement getDBJobPackage"
        #Example: return 'RecExRecoTest'

    def getDBJobPackageFullPath(self):
        return None

    def getXMLFileName(self):
        return self.getDBJobPackage()+'_TestConfiguration.xml'

    def getSVNPathToXML(self,tag=None):
        fp=self.getDBJobPackageFullPath()
        version='trunk' if not tag else 'tags/'+tag
        return fp+'/'+version+'/test/'+self.getXMLFileName() if fp!=None else None

    def __svnurl(self,svnpath):
        return 'https://svnweb.cern.ch/trac/atlasoff/browser/'+svnpath if svnpath else None

    def getSVNURLToXml(self,tag=None):
        return self.__svnurl(self.getSVNPathToXML(tag))

    def getSVNPathToPackage(self,tag=None):
        version='trunk' if not tag else 'tags/'+tag
        fp=self.getDBJobPackageFullPath()
        return fp+'/'+version if fp else None

    def getSVNURLToPackage(self,tag=None):
        return self.__svnurl(self.getSVNPathToPackage(tag))

    def getDBJobName(self):
        assert False, "Must reimplement getDBJobName"
        #Example: return 'mc08_'+getJobName()+'_perfMonDozer'

    def getDBChannelName(self):
        assert False, "Must reimplement getDBChannelName"

    #Method defining the name of the job in the output of the present script
    def getJobName(self):
        assert False, "Must reimplement getJobName"
        #Example: return 'idmuoncalo'

    #Method defining what fields to look for in the database, and what
    #name to associate with their resulting parameters:
    def getFields(self):
        assert False, "Must reimplement getFields"
        #Example: return {'Vmem per evt':'vmem','CPU per evt':'cpu'}

    #Optional method (mainly intended to be used for jobs generating auto-warnings):
    def estimateError(self,fieldname,fieldvalue,fieldunit):
        return None
        #Example : return fieldvalue*0.01 if fieldname=='vmem' else None

#Specialization for PerfMonDozer:
class PerfMonDozerJobDef(CDJobDefinitionBase):
    def __init__(self,dbjobkey,dbjobpackage,userjobid,jobname):
        self.__dbjobkey=dbjobkey
        if '/' in dbjobpackage:
            from os.path import basename
            self.__dbjobpackage=basename(dbjobpackage)
            self.__dbjobpackagefullpath=dbjobpackage
        else:
            self.__dbjobpackage=dbjobpackage
            self.__dbjobpackagefullpath=None
        self.__userjobid=userjobid
        self.__dbjobname=userjobid+'_perfMonDozer'
        self.__jobname=jobname
    def getDBJobKey(self):
        return self.__dbjobkey
    def getDBJobPackage(self):
        return self.__dbjobpackage
    def getDBJobPackageFullPath(self):
        return self.__dbjobpackagefullpath
    def getDBJobName(self):
        return self.__dbjobname
    def getDBChannelName(self):
        return self.getDBJobName()#perfmondozer has same channel and job name.
    def getJobName(self):
        return self.__jobname

    def getUserJobId(self):
        return self.__userjobid

    def getFields(self):
        return {
            'Vmem per evt':'vmem',
            'CPU per evt':'cpu/evt',
            'Malloc per evt':'malloc/evt',
            'MemLeak per evt':'memleak/evt',
            'NormCPU per evt':'si2k'
            }
    def estimateError(self,fieldname,fieldvalue,fieldunit):
        assert fieldvalue!=None
        if fieldname=='vmem':
            assert fieldunit.lower()=='mb'
            return 0.6*(1.0+fieldvalue*0.001)#Just an estimate. 1MB for small jobs -> 2-3MB for big jobs
        return None

class CDTimeHelper:

    #Helper class for constructing the timestamps used when calling
    #CDDataRetrievalHelper.
    #Like in DozerIOV, we discard all the HH:MM:SS from the timestamps:

    @staticmethod
    def now():
        return CDTimeHelper.nDaysAgo(0)

    @staticmethod
    def nDaysAgo(ndays):
        return datetime.date.today()+datetime.timedelta( -ndays )

    @staticmethod
    def nDaysBefore(ndays,date):
        return date+datetime.timedelta( -ndays )

    @staticmethod
    def nDaysAfter(ndays,date):
        return date+datetime.timedelta( ndays )

    @staticmethod
    def fromDate(day,month,year):
        return datetime.date(year=year,month=month,day=day)

    @staticmethod
    #Returns 0 in case of bad input
    def fromDateString(datestr):
        import re
        if len(datestr)!=10 or not re.match('\d\d\d\d\-\d\d\-\d\d',datestr):
            return 0
        import datetime
        try:
            d=datetime.datetime.strptime(datestr,'%Y-%m-%d').date()
        except:
            d=None
        return d

class CDDataRetrievalHelper(object):

    #Static private data. Hands off!:
    __private__dbhandler=None
    connectString='oracle://atlas_coolprod;schema=ATLAS_LTM_RTT;dbname=ATLR;user=ATLAS_LTM_RTT_R;password=%s'% \
                   '4c544d52656164657232303039'.decode('hex_codec')
    #Workhorse is a static method which takes a list of objects
    #implementing the CDJobDefinitionBase above, and returning a list
    #of Job's (from ValuePerDomainCalcUtils).
    @staticmethod
    def getJobsFromDB(cdjobdefinitions,since,until=CDTimeHelper.now(),quiet=False):
        from CoolDozer import DozerIOV
        assert since<=until, "Wrong time interval requested"
        assert cdjobdefinitions != None
        since_extended=CDTimeHelper.nDaysBefore(3,since)
        until_extended=CDTimeHelper.nDaysAfter(3,until)
        if len(cdjobdefinitions)==0:
            return []
        #Make sure we are connected to the database:
        if CDDataRetrievalHelper.__private__dbhandler==None:
            if not quiet:
                print " ==> Loading database modules"
            from CoolDozer import DozerDB,DozerLogger
            if not quiet:
                print " ==> Database modules loaded"

            #Hack to work around several problems in DozerDB:
            class DozerDBHack(DozerDB):
                def __init__(self, package, framework, dry=False, log=None, connect=None):
                    #Avoid CRITICAL message (dry==True), and delay openDB() call
                    super(DozerDBHack, self).__init__(package=package,framework=framework,
                                                      dry=False,db="PROD_READ",log=log)
#                                                      dry=True,log=log)
                    if connect!=None:
                        self._DozerDB__connect=connect
                    self._DozerDB__dry=dry
                    super(DozerDBHack, self).openDB()
                def openDB( self ):
                    return
                def setPackage(self,p):
                    self._DozerDB__package=p

            class CDDBHandler(object):
                def __init__(self,firstpackage):
                    import logging
                    self.log = DozerLogger('/dev/null', 'CDDataRetrievalHelper', toConsole=True).getLogger(self)
                    self.log.setLevel(logging.WARNING)
                    dryRun=False
                    if not quiet:
                        print " ==> Initiating connection to database"
                    self.db=DozerDBHack(firstpackage, "LTM_RTT", dryRun, self.log,connect=CDDataRetrievalHelper.connectString)
                    if not self.db.isOpened():
                        raise Exception('CDDataRetrievalHelper: Could not connect to database')
                    if not quiet:
                        print " ==> Data base connection established"
                def getRecordSet(self,path,since,until,package):
                    self.db.setPackage(package)
                    assert self.db.package() == package,"problems changing package for DozerDB."
                    def date2Cool(thedate):
                        return long( time.mktime(thedate.timetuple()) * long(1000000000) )
                    #Change log-level while retrieving to suppress spurious error messages
                    import logging
                    oldlvl=logging.getLogger(  ).level
                    logging.getLogger(  ).setLevel(logging.CRITICAL)
                    rs=None
                    try:
                        rs=self.db.get(path, since=DozerIOV(date2Cool(since)), until=DozerIOV(date2Cool(until)))
                    except (AttributeError,TypeError):
                        #This happens when looking in path where data was never stored:
                        print " ==> WARNING: Problems retrieving recordset at",path," (probably no such data exist)"
                        rs=None
                        pass
                    logging.getLogger(  ).setLevel(oldlvl)
                    return rs
            CDDataRetrievalHelper.__private__dbhandler = CDDBHandler(cdjobdefinitions[0].getDBJobPackage())
        #Start retrieval:
        import os.path
        from ValuePerDomainCalcUtils import Job,TimeLabel
        import datetime
        gavecooldozerwarning=False
        outputjobs=[]
        for cdjobdefinition in cdjobdefinitions:
            #Attempt to get recordset from database:
            try:
                retrievekey=cdjobdefinition.getDBJobName()
                channelname=cdjobdefinition.getDBChannelName()
                #We extend the interval somewhat white retrieving. Important to work around all the IOV "features":
                #print os.path.join(cdjobdefinition.getDBJobKey(),retrievekey,channelname)
                recordset=CDDataRetrievalHelper.__private__dbhandler.getRecordSet( os.path.join(cdjobdefinition.getDBJobKey(),
                                                                                               retrievekey,channelname),
                                                                                   since_extended, until_extended,
                                                                                   cdjobdefinition.getDBJobPackage())
                iovranges=recordset.keys() if recordset!=None else []
                if recordset==None:
                    print " ==> WARNING: Problems getting jobs with key",retrievekey,"and channel name",channelname
                for iovrange in iovranges:
                    #Timestamp of IOV. Only the lower range of an IOV actually makes sense for us:
                    unixtime = DozerIOV(iovrange[0]).AsUnix() if iovrange[0]!=None else None
                    if unixtime==None:
                        continue
                    thedate=datetime.datetime.fromtimestamp(unixtime).date()
                    if thedate<since or thedate>until:
                        continue
                    if iovrange[1]>9000000000000000000L and thedate>datetime.date(year=2010,month=6,day=5):
                        #ignore old faulty entries with infinite upper
                        #limits masquerading as new entries via iov magic
                        continue

                    #Get channel (protect against the case where for
                    #some __getitem__ returns random channels from
                    #elsewhere in the db):
                    channel=recordset.__getitem__(iovrange)
                    if channel!=None and channel.name()!=channelname:
                        if not gavecooldozerwarning:
                            print " ==> WARNING: Working around CoolDozer retrieval bug"
                            gavecooldozerwarning=True
                        continue
                    meas_name2val={}
                    meas_name2error={}
                    block=False
                    for fieldname_db, fieldname_output in cdjobdefinition.getFields().items():
                        data=channel.__getitem__(fieldname_db) if channel!=None else None
                        #print fieldname_db,data
                        if data==None:
                            continue
                        val = data.value()
                        #Sometimes value() returns an empty string (bloody missing type-safety!)
                        if type(val)==type(""):
                            continue
                        unit = data.unit()
                        #Hacks for malloc:
                        if fieldname_db=='Malloc per evt':
                            if unit=='ms': unit='MB'
                            if val==0.0: val=None
                        #Vmem==0.0 actually indicates a failed job:
                        if val==0.0 and fieldname_db=='Vmem per evt':
                            block=True
                        if val!=None and unit!=None:
                            meas_nameandunit=fieldname_output+':'+unit
                            meas_name2val[meas_nameandunit]=val
                            meas_name2error[meas_nameandunit]=cdjobdefinition.estimateError(fieldname_output,val,unit)
                    if not block:
                        outputjobs+=[Job(cdjobdefinition.getJobName(),meas_name2val,TimeLabel(date=thedate),meas_name2error)]
            except:
                print "Problems while retrieving data for key: %s and channel name: %s" % (retrievekey,channelname)
                raise
        print " ==> Data from",len(outputjobs),"jobs retrieved"
        return outputjobs

    @staticmethod
    def dumpDBContents():
        #Based on code from Ciba:
        from PyCool import cool, coral
        dbSvc = cool.DatabaseSvcFactory.databaseService()
        #print 'using software release', dbSvc.serviceVersion()
        db = dbSvc.openDatabase( CDDataRetrievalHelper.connectString, False )
        print db
        folderlist=db.listAllNodes()
        for fpath in folderlist:
            if ( fpath != "/" ):
                try:
                    ifolder = db.getFolderSet( fpath )
                    print "IFolderSet = %s" % fpath
                except:
                    ifolder = db.getFolder( fpath )
                    print "IFolder    = %s" % fpath
                    channels = None
                    try:
                        channels = ifolder.listChannelsWithNames()
                    except:
                        pass
                    if ( channels ):
                        for item in channels:
                            print "  channel name=%s" % item
