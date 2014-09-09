#!/usr/bin/env python
# detStatus_merge.py - tool for merging detector status info / several folders
# Richard Hawkings, 28/1/08
# execute without arguments for help

from CoolConvUtilities import AtlCoolLib
from DetectorStatus import DetStatusLib
from PyCool import cool

class mergeStatus(AtlCoolLib.coolTool):
    def setup(self,args):
        # set values of non-optional parameters
        self.destdbstr=str(args[0])
        # access to name encoding
        self.namelookup=DetStatusLib.DetStatusNames()
        self.folderinfo=[]
        self.destfolder='/GLOBAL/DETSTATUS/LBSUMM'
        self.tag=''
        self.truncate=False
        self.numbers=False
        
    def usage(self):
        "Define additional syntax for options"
        self._usage1()
        print 'destdbname'
        print '--and=<folder> : set folder name to be Anded'
        print '--override=<folder>:<tag> : set folder and tag name for override'
        print '--destfolder=<folder> : set destinaton folder (default /GLOBAL/DETSTATUS/LBSUMM'
        print '--desttag=<tag> : set destination tag'
        print '--numbers : Transfer NConfig and NWorking from source folders if given'
        print '--truncate : truncate IOVs which exceed given interval'
        self._usage2()

    def procopts(self,opts):
        "Process additional options"
        for o,a in opts:
            if (o=='--and' or o=='--override'):
                fields=str(a).split(':')
                folder=fields[0]
                if len(fields)>1:
                    tag=fields[1]
                else:
                    tag='HEAD'
                self.folderinfo+=[(folder,tag,(o=='--override'))]
            if (o=='--destfolder'):
                self.destfolder=str(a)
            if (o=='--numbers'):
                self.numbers=True
            if (o=='--desttag'):
                self.tag=str(a)
            if (o=='--truncate'):
                self.truncate=True

    def execute(self):
        # print "Name is %s" AtlCoolLib.getHostname()
        # prepare an empty StatusList for each channel
        statuslists={}
        for chan in self.namelookup.allNums():
            statuslists[chan]=DetStatusLib.StatusList()

        # deduce the source database instance name, looking for xyzP200
        idx=self.conn.find('P200')
        if (idx>=3):
            dbinst=self.conn[idx-3:idx+4]
            print "Working with database instance %s" % dbinst
        else:
            print "Cannot deduce database instance name, assume COMP200"
            dbinst='COMP200'
            
        # Extract timestamp information for mapping to run/LB
        tsconvdb=AtlCoolLib.indirectOpen('COOLONL_TRIGGER/'+dbinst,oracle=self.oracle)
        # initialize the converter
        tsToRLB = AtlCoolLib.TimeStampToRLB(tsconvdb,self.since,self.until)
        tsconvdb.closeDatabase()

        StartTime = tsToRLB.StartTime
        EndTime = tsToRLB.EndTime

        # open auxillary databases for online and DCS info
        onlinedb=AtlCoolLib.indirectOpen('COOLONL_GLOBAL/'+dbinst,oracle=self.oracle)
        dcsdb=AtlCoolLib.indirectOpen('COOLOFL_GLOBAL/'+dbinst,oracle=self.oracle)

        # loop over all 'AND' folders and merge information
        ngood=0
        nbad=0
        for (afolder,atag,override) in self.folderinfo:
            print "Process folder %s tag %s override %d" % (afolder,atag, override)
            try:
                atsindexed=0                
                isonline=0
                if (afolder.find("ONL") != -1):
                    isonline=1

                isdcs=0
                if (afolder.find("DCS") != -1):
                    isdcs=1

                tostart=self.since
                toend=self.until
                ifolder=None

                if (isonline):
                    ifolder=onlinedb.getFolder(afolder)
                elif (isdcs):
                    ifolder=dcsdb.getFolder(afolder)
                else:
                    ifolder=self.db.getFolder(afolder)

                descr=ifolder.description()

                if (descr.find("<timeStamp>time") != -1):
                    atsindexed=1                    
                    tostart=StartTime
                    toend=EndTime                                       

                objs=None
                if (isonline | isdcs):
                    objs=ifolder.browseObjects(tostart,toend,cool.ChannelSelection.all())
                else:
                    objs=ifolder.browseObjects(tostart,toend,cool.ChannelSelection.all(),atag)

                while objs.goToNext():
                    obj=objs.currentRef()
                    chan=obj.channelId()
                    if (chan in statuslists.keys()):
                        start=obj.since()
                        stop=obj.until()
                        payload=obj.payload()
                        code=payload['Code']
                        deadfrac=payload['deadFrac']
                        thrust=payload['Thrust']
                        if ('NConfig' in payload.keys()):
                            nconfig=payload['NConfig']
                            nworking=payload['NWorking']
                        else:
                            nconfig=-1
                            nworking=-1

                        if ('Comment' in payload.keys()):
                            comment=payload['Comment']
                        else:
                            comment=''
                        #override=False

                        if (atsindexed):
                            start=tsToRLB.getRLB(start, True)
                            stop=tsToRLB.getRLB(stop, False)+1

                        statuslists[chan].merge(DetStatusLib.StatusObj(start,stop,code,deadfrac,thrust,nconfig,nworking,comment),override)
                        ngood+=1
                    else:
                        print "Data found for unexpected channel %i" % chan
                        nbad+=1
            except Exception,e:
                print e
                print "Problem accessing folder %s" % afolder
                nbad+=1
            
        print "All folders processed with %i good and %i items of bad data" % (ngood,nbad)

        # now compose COOL update
        # establish write connection
        writedb=AtlCoolLib.forceOpen(self.destdbstr)
        if (writedb is None): return
        try:
            cfolder=writedb.getFolder(self.destfolder)
            print "Write data to existing folder %s" % self.destfolder
            spec=cfolder.payloadSpecification()
        except Exception,e:
            print "Creating destination folder %s" % self.destfolder
            spec=cool.RecordSpecification()
            spec.extend("Code",cool.StorageType.Int32)
            spec.extend("deadFrac",cool.StorageType.Float)
            spec.extend("Thrust",cool.StorageType.Float)
            if self.numbers:
                spec.extend("NConfig",cool.StorageType.Int32)
                spec.extend("NWorking",cool.StorageType.Int32)
            spec.extend("Comment",cool.StorageType.String255)
                
            cfolder=AtlCoolLib.ensureFolder(writedb,self.destfolder,spec,AtlCoolLib.athenaDesc(self.runLumi,'CondAttrListCollection'),cool.FolderVersioning.MULTI_VERSION)
        # folder now exists, prepare for bulk update
        nobj=0
        nbad=0
        cfolder.setupStorageBuffer()
        if (self.tag==''):
            print "Storing data to HEAD"
        else:
            print "Storing data to tag %s" % self.tag
        
        for (chan,slist) in statuslists.items():
            size1=slist.size()
            size2=slist.compress()
            print "List for channel %i size %i / %i after compression" % (chan,size1,size2)
            for sobj in slist.list():
                data=cool.Record(spec)
                data['Code']=sobj.code
                data['deadFrac']=sobj.deadfrac
                data['Thrust']=sobj.thrust
                if self.numbers:
                    data['NConfig']=sobj.nconfig
                    data['NWorking']=sobj.nworking
                if spec.exists('Comment'):
                    data['Comment']=sobj.comment
                since=sobj.start
                until=sobj.stop
                # truncate IOV if needed
                if (self.truncate and (sobj.start<self.since or sobj.stop>self.until)):
                    since=max(sobj.start,self.since)
                    until=min(sobj.stop,self.until)
                    print "Channel %i: truncating IOV [%i,%i] to range [%i,%i]" % (chan,sobj.start,sobj.stop,since,until)
                if (since<until):
                    try:
                        cfolder.storeObject(since,until,data,chan,self.tag)
                        nobj+=1
                    except Exception,e:
                        print "Exception when storing data",e
                        nbad+=1
                else:
                    print "IOV removed by truncation"

        cfolder.flushStorageBuffer()
        print "Data stored with %i good and %i bad updates" % (nobj,nbad)

        
        

# run the tool
mytool=mergeStatus("detStatus_merge.py",True,2,2,['and=','override=','destfolder=','desttag=','numbers','truncate'])
