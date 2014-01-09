#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# beamSpot_set.py - tool for setting beamspot positions in COOL
# Richard Hawkings, 5/2/07
# execute without arguments for help
import os,sys
from PyCool import cool
from CoolConvUtilities import AtlCoolLib

class setBeampos(AtlCoolLib.coolTool):
    def setup(self,args):
        # set values of non-optional parameters
        self.tag=str(args[0])
        self.status=int(args[1])
        self.posx=float(args[2])
        self.posy=float(args[3])
        self.posz=float(args[4])
        # parameters with nominal defaults
        self.sigmax=0.015
        self.sigmay=0.015
        self.sigmaz=53.
        self.tiltx=0.
        self.tilty=0.
        self.sigmaxy=0.
        if (len(args)>5): self.sigmax=float(args[5])
        if (len(args)>6): self.sigmay=float(args[6])
        if (len(args)>7): self.sigmaz=float(args[7])
        if (len(args)>8): self.tiltx=float(args[8])
        if (len(args)>9): self.tilty=float(args[9])
        if (len(args)>10): self.sigmaxy=float(args[10])
        
    def usage(self):
        """ Define the additional syntax for options """
        self._usage1()
        print 'COOL_tag status posx pozy posz { sigmax sigmay sigmaz {tiltx tilty tiltz sigmaXY}}'
        self._usage2()
        
    def execute(self):
        # do update - setup folder specification and create if needed
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
        folder='/Indet/Beampos'
        print ">== Store object in folder",folder
        cfolder=AtlCoolLib.ensureFolder(self.db,folder,spec,AtlCoolLib.athenaDesc(self.runLumi,'AthenaAttributeList'),cool.FolderVersioning.MULTI_VERSION)
        if (cfolder is None): sys.exit(1)
        # now write data
        payload=cool.Record(spec)
        payload['status']=self.status
        payload['posX']=self.posx
        payload['posY']=self.posy
        payload['posZ']=self.posz
        payload['sigmaX']=self.sigmax
        payload['sigmaY']=self.sigmay
        payload['sigmaZ']=self.sigmaz
        payload['tiltX']=self.tiltx
        payload['tiltY']=self.tilty
        payload['sigmaXY']=self.sigmaxy
        print '>== Store object with IOV [',self.since,',',self.until,'] and tag',self.tag,'status',self.status
        print '>== Beamspot position (mm):',self.posx,self.posy,self.posz
        print '>== Beamspot sigma    (mm):',self.sigmax,self.sigmay,self.sigmaz
        print '>== Beamspot tilt    (rad):',self.tiltx,self.tilty
        try:
            if (self.tag=="HEAD"):
                cfolder.storeObject(self.since,self.until,payload,0)
            else:
                cfolder.storeObject(self.since,self.until,payload,0,self.tag)
            print ">== Storing COOL object succeeded"
        except Exception,e:
            print e
            print '>== Storing COOL object FAILED'
            sys.exit(1)

mytool=setBeampos('beamPos_set.py',False,6,12,[])
