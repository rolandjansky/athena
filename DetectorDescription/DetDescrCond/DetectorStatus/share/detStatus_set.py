#!/usr/bin/env python
# detStatus_set.py - tool for setting detector status in COOL conditions DB
# Richard Hawkings, 5/2/07
# execute without arguments for help
import os,sys
from PyCool import cool
from CoolConvUtilities import AtlCoolLib
from DetectorStatus import DetStatusLib

class setStatus(AtlCoolLib.coolTool):
    def setup(self,args):
        # set values of non-optional parameters
        self.isdata=False
        self.comments=False
        self.numbers=False
        self.foldername=''
        self.fversion=cool.FolderVersioning.MULTI_VERSION
        if (len(args)>0):
            self.tag=str(args[0])
            self.det=str(args[1])
            self.light=str(args[2])
            self.isdata=True
        else:
            self.det='ALL'
        self.deadfrac=0.
        self.deadthrust=1.
        self.commstr=''
        self.nconfig=0
        self.nworking=0
        self.newtag=False
        if len(args)>3: self.deadfrac=float(args[3])
        if len(args)>4: self.deadthrust=float(args[4])
        if len(args)==6:
            self.commstr=str(args[5])
            self.comments=True
        if len(args)==7:
            self.nconfig=int(args[5])
            self.nworking=int(args[6])
            self.numbers=True
        # access to name encoding
        self.namelookup=DetStatusLib.DetStatusNames()
        
    def usage(self):
        """ Define the additional syntax for options """
        self._usage1()
        print 'COOL_tag detector traffic_light dead_frac dead_thr {<comm> | NConfig NWorking}'
        self._usage2()
        print '--folder <folder>: Set foldername to be used'
        print '--comm: Include comment field when creating structure'
        print '--newtag:  Allow creation of a new tag, not already existing in folder'
        print '--numbers: Include numbers fields (NConfig, NWorking) when creating structure'
        print '--single: Create folder as single version (default multi)'

    def procopts(self,opts):
        "Process the additional non-standard options"
        for o,a in opts:
            if (o=='--comm'):
                self.comments=True
            if (o=='--numbers'):
                self.numbers=True
            if (o=='--folder'):
                self.foldername=str(a)
            if (o=='--single'):
                self.fversion=cool.FolderVersioning.SINGLE_VERSION
            if (o=='--newtag'):
                self.newtag=True
        
    def execute(self):
        chanlist=self.getchans()
        if len(chanlist)==0: sys.exit(1)
        # now do update - setup folder specification and create if needed
        spec=cool.RecordSpecification()
        spec.extend("Code",cool.StorageType.Int32)
        spec.extend("deadFrac",cool.StorageType.Float)
        spec.extend("Thrust",cool.StorageType.Float)
        if (self.numbers):
            spec.extend("NConfig",cool.StorageType.Int32)
            spec.extend("NWorking",cool.StorageType.Int32)
        if (self.comments):
            spec.extend("Comment",cool.StorageType.String255)
        # folder name depends on choice of run-lumi or timestamp
        if (self.foldername==''):
            folder=DetStatusLib.folderName(self.runLumi)
        else:
            folder=self.foldername
        print ">== Store object in folder",folder
        cfolder=AtlCoolLib.ensureFolder(self.db,folder,spec,AtlCoolLib.athenaDesc(self.runLumi,'CondAttrListCollection')+'<named/>',self.fversion)
        if (cfolder is None): sys.exit(1)
        # if we do not have data to add - finish here
        if (not self.isdata): return

        # translate detector to COOL channel
        status=DetStatusLib.colourVal(self.light)
        if (status is None):
            print"Cannot define status from value",self.light
            sys.exit(1)
        # check if tag exists
        if self.tag!='HEAD':
            taglist=cfolder.listTags()
            if self.tag not in taglist:
                if self.newtag:
                    print ">== Tag %s will be created" % self.tag
                else:
                    print ">== ERROR: Tag %s does not exist (use --newtag if you really want to create it)" % self.tag
                    sys.exit(1)
        # now write data
        payload=cool.Record(spec)
        payload['Code']=status
        payload['deadFrac']=self.deadfrac
        payload['Thrust']=self.deadthrust
        if (self.comments):
            payload['Comment']=self.commstr
        if (self.numbers):
            payload['NConfig']=self.nconfig
            payload['NWorking']=self.nworking
        for channel in chanlist:
            print '>== Store object with IOV [',self.since,',',self.until,'] channel',channel,'and tag',self.tag,'status info',status,self.deadfrac,self.deadthrust
            try:
                if (self.tag=="HEAD"):
                    cfolder.storeObject(self.since,self.until,payload,channel)
                else:
                    cfolder.storeObject(self.since,self.until,payload,channel,self.tag,True)
            except Exception,e:
                print e
                print 'Exception thrown when storing for channel',channel
                print '>== Storing COOL object FAILED'
                sys.exit(1)

    def getchans(self):
        updet=self.det.upper()
        if (updet=='ALL'):
            chanlist=self.namelookup.allNums()
        else:
            try:
                chanlist=[self.namelookup.num(updet)]
            except KeyError:
                print '>== ERROR: Status name',self.det,'is not defined'
                chanlist=[]
        return chanlist

mytool=setStatus('detStatus_set.py',False,1,8,['comm','numbers','single','newtag','folder='])
