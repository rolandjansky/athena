#!/usr/bin/env python
# coolStrFileSvc_set.py - setup folder or insert data for CoolStrFileSvc
# Richard Hawkings, 9/6/09, from earlier insert_coolstrfile.py outside release
# execute without arguments for help

import os,sys
from PyCool import cool
from CoolConvUtilities import AtlCoolLib

class setData(AtlCoolLib.coolTool):
    def setup(self,args):
        # set values of non-optional parameters
        self.foldername=str(args[0])
        self.channel=int(args[1])
        self.tag=str(args[2])
        self.file=str(args[3])
        # set optional parameters
        self.tech=0
        
    def usage(self):
        """ Define the additional syntax for options """
        self._usage1()
        print 'folder channel tag filename'
        self._usage2()
        print '--tech 0|1 : Set stoarge technology (0=string, 1=CLOB)'

    def procopts(self,opts):
        "Process the additional non-standard options"
        for o,a in opts:
            if (o=='--tech'):
                self.tech=int(a)
                
    def execute(self):
        "Execute the command"
        # set up folder specification
        spec=cool.RecordSpecification()
        spec.extend('tech',cool.StorageType.Int32)
        spec.extend('file',cool.StorageType.String4k)
        if self.tech==0:
            spec.extend('data',cool.StorageType.String4k)
        elif self.tech==1:
            spec.extend('data',cool.StorageType.String16M)
        else:
            print "Unknown technology %i specified - must be 0 (String4K) or 1 (CLOB 16M)" % self.tech
            sys.exit(1)
        
        # create folder if needed
        cfolder=AtlCoolLib.ensureFolder(self.db,self.foldername,spec,AtlCoolLib.athenaDesc(True,'CondAttrListCollection'),True)
        if (cfolder is None): sys.exit(-1)
        # check/reset technology used in folder, in case already existing
        fstype=(cfolder.payloadSpecification()['data']).storageType()
        tech=0
        if fstype==cool.StorageType.String16M: tech=1
        
        # read file
        try:
            ifile=open(self.file,'r')
            sdata=ifile.read()
            ifile.close()
            # check length
            if len(sdata)>=4000 and tech==0:
                print "Data string too long for non-CLOB technology"
                sys.exit(-1)
        except:
            print "Problem accessing file",self.file
            sys.exit(-1)
        print "Read %i bytes from file %s" % (len(sdata),self.file)

        # now write the data
        payload=cool.Record(spec)
        payload['tech']=tech
        payload['file']=self.file
        payload['data']=sdata
        print '>== Store object with IOV [',self.since,',',self.until,'] channel',self.channel,'and tag',self.tag,'from file',self.file
        try:
            if (self.tag=="HEAD"):
                cfolder.storeObject(self.since,self.until,payload,self.channel)
            else:
                cfolder.storeObject(self.since,self.until,payload,self.channel,self.tag,True)
        except Exception,e:
            print e
            print '>== Storing COOL object FAILED'
            sys.exit(1)
        print ">== Object stored"

mytool=setData('coolStrFileSvc_set.py',False,5,5,['tech='])
