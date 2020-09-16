#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Script to create an AthenaAttributeList with a single attribute "xint".
# Usage example: dmtest_condwriter.py --rs=1 --ls=1 'sqlite://;schema=test.db;dbname=OFLP200' AttrList_noTag 42
#

from __future__ import print_function

import sys,os
os.environ['CLING_STANDARD_PCH'] = 'none' #See bug ROOT-10789
from PyCool import cool
from CoolConvUtilities import AtlCoolLib, AtlCoolTool

class createTestDB(AtlCoolLib.coolTool):

    def setup(self,args):
        # set values of non-optional parameters
        self.tag=str(args[0])
        self.xint=int(args[1])
        
    def usage(self):
        """ Define the additional syntax for options """
        self._usage1()
        print ('TAG xint')
        self._usage2()
        
    def execute(self):

        folder='/DMTest/TestAttrList'

        # do update - setup folder specification and create if needed
        spec = cool.RecordSpecification()
        spec.extend("xint", cool.StorageType.Int32)
        print (">== Store object in folder",folder)
        cfolder = AtlCoolLib.ensureFolder(self.db, folder, spec,
                                          AtlCoolLib.athenaDesc(self.runLumi, 'AthenaAttributeList'),
                                          cool.FolderVersioning.MULTI_VERSION)
        if (cfolder is None): sys.exit(1)
        # now write data
        payload = cool.Record(spec)
        payload['xint'] = self.xint
        print ('>== Store object with IOV [',self.since,',',self.until,'] and tag',self.tag,'xint',self.xint)
        try:
            if (self.tag=="HEAD"):
                cfolder.storeObject(self.since,self.until,payload,0)
            else:
                cfolder.storeObject(self.since,self.until,payload,0,self.tag)
            print (">== Storing COOL object succeeded. Current content:")
        except Exception:
            import traceback
            traceback.print_exc()
            print ('>== Storing COOL object FAILED')
            sys.exit(1)

        # print full content
        act = AtlCoolTool.AtlCoolTool(self.db)
        print (act.more(folder))

mytool = createTestDB('dmtest_condwriter.py',False,3,3,[])
