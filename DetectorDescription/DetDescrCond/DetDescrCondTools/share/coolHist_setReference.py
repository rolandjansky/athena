#!/bin/env python
# coolHist_setReference.py - tool for setting references to histogram files
# in COOL conditions database, for pickup by Athena CoolHistSvc
# Richard Hawkings, 5/2/07
# execute without arguments for help

import os,sys
from PyCool import cool
from CoolConvUtilities import AtlCoolLib

class setReference(AtlCoolLib.coolTool):
    def setup(self,args):
        # set values of non-optional parameters
        self.folder=str(args[0])
        try:
            self.channel=int(args[1])
            self.channelstr=""
        except ValueError:
            # if it is a string, store that instead
            self.channelstr=str(args[1])
        self.tag=str(args[2])
        self.file=str(args[3])
        
    def usage(self):
        """ Define the additional syntax for options """
        self._usage1()
        print 'folder COOL_channel COOL_tag ROOT_file'
        self._usage2()
        
    def execute(self):
        print '>== Inserting reference to file:',self.file,' - find GUID'
        os.system('coolHist_extractFileIdentifier.sh %s' % self.file)
        guid=open('coolhist_guid.tmp','r').readline()[:-1]
        if (guid=='none'):
            print '>== File has no GUID - aborting'
            sys.exit(1)
        # setup folder specification and create if needed
        spec=cool.RecordSpecification()
        spec.extend("fileGUID",cool.StorageType.String4k)
        cfolder=AtlCoolLib.ensureFolder(self.db,self.folder,spec,AtlCoolLib.athenaDesc(self.runLumi,'CondAttrListCollection')+'<named/>',cool.FolderVersioning.MULTI_VERSION)
        print
        if (cfolder is None): sys.exit(1)
        # check if channel number needs to be looked up
        if (self.channelstr!=""):
            try:
                self.channel=cfolder.channelId(self.channelstr)
                print '>== Channel name',self.channelstr,'is channelId',self.channel
            except Exception:
                print 'Non-existant or invalid channel name:',self.channelstr
                sys.exit(1)
        print '>== Write data on COOL connection:',self.conn
        print '>== To folder:',self.folder,'channel:',self.channel
        print '>== COOL tag:',self.tag
        # now write data
        payload=cool.Record(spec)
        payload['fileGUID']=guid
        print '>== Store object with IOV [',self.since,',',self.until,'] channel',self.channel,'and tag',self.tag
        try:
            cfolder.storeObject(self.since,self.until,payload,self.channel,self.tag)
            return
        except Exception,e:
            print e
            print '>== Storing COOL object FAILED'
            sys.exit(1)

mytool=setReference('coolHist_setReference.py',False,5,5,[])
