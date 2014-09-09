#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# detStatus_AsBuilt.py - tool for setting /GLOBAL/DETSTATUS/ASBUILT folder
# Richard Hawkings, 28/7/09
# execute without arguments for help
import os,sys
from PyCool import cool
from CoolConvUtilities import AtlCoolLib
from DetectorStatus import DetStatusLib

class setBuilt(AtlCoolLib.coolTool):
    def setup(self,args):
        # set values of non-optional parameters
        self.isdata=False
        self.det='ALL'
        if len(args)>0:
            self.det=str(args[0])
            self.nbuilt=int(args[1])
            self.isdata=True
        self.foldername='/GLOBAL/DETSTATUS/ASBUILT'
        # access to name encoding
        self.namelookup=DetStatusLib.DetStatusNames()
        
    def usage(self):
        """ Define the additional syntax for options """
        self._usage1()
        print 'detector value'
        self._usage2()

    def procopts(self,opts):
        "Process the additional non-standard options"
        for o,a in opts:
            pass
        
    def execute(self):
        chanlist=self.getchans()
        if len(chanlist)==0: sys.exit(1)
        # now do update - setup folder specification and create if needed
        spec=cool.RecordSpecification()
        spec.extend("NBuilt",cool.StorageType.Int32)
        print ">== Store object in folder",self.foldername
        cfolder=AtlCoolLib.ensureFolder(self.db,self.foldername,spec,AtlCoolLib.athenaDesc(True,'CondAttrListCollection')+'<named/>',cool.FolderVersioning.SINGLE_VERSION)
        if (cfolder is None): sys.exit(1)
        # if we do not have data to add - finish here
        if (not self.isdata): return

        # now write data
        payload=cool.Record(spec)
        payload['NBuilt']=self.nbuilt
        for channel in chanlist:
            print '>== Store object with IOV [',self.since,',',self.until,'] channel',channel,'NBuilt=',self.nbuilt
            try:
                cfolder.storeObject(self.since,self.until,payload,channel)
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

mytool=setBuilt('detStatus_AsBuilt.py',False,1,3,[])
