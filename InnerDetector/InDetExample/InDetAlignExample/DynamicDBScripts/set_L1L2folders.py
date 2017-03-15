#!/bin/env python
# set_ibldist.py - tool to generate COOL sqlite DB fragment for IBL distortion
# Richard Hawkings, 21/5/15
# execute without arguments for help
import os,sys
from PyCool import cool
from CoolConvUtilities import AtlCoolLib

class setL1L2(AtlCoolLib.coolTool):
    def setup(self,args):
        "Setup routine called from initialisation"
        # set value of non-optional parameters
        self.tag=str(args[0])
        self.txtfile=str(args[1])
#        self.runmin=281411
#        self.runmax=281411
#        self.lumimin=int(args[2])
#        self.lumimax=int(args[3])

    def usage(self):
        "Define the additional syntax for options"
        self._usage1()
        print "COOL_tag txtfile"
        self._usage2()


    def execute(self):
        "Execute the database creation/update"
        # setup specification 
        spec=cool.RecordSpecification()
        spec.extend("subsystem",cool.StorageType.Int32)
        spec.extend("det",cool.StorageType.Int32)
        spec.extend("bec",cool.StorageType.Int32)
        spec.extend("layer",cool.StorageType.Int32)
        spec.extend("ring",cool.StorageType.Int32)
        spec.extend("sector",cool.StorageType.Int32)
        spec.extend("side",cool.StorageType.Int32)
        spec.extend("Tx",cool.StorageType.Float)
        spec.extend("Ty",cool.StorageType.Float)
        spec.extend("Tz",cool.StorageType.Float)
        spec.extend("Rx",cool.StorageType.Float)
        spec.extend("Ry",cool.StorageType.Float)
        spec.extend("Rz",cool.StorageType.Float)

        # create folder if it does not exist
        folder=''
        
        # read data from the text file
        print "Reading input data from %s" % self.txtfile
        data=[]
        idents=[]
        for iline in open(self.txtfile).readlines():
            # split line into space-separated tokens, ignoring newline
            tokens=iline[:-1].split()
            # skip lines starting with #
            #if tokens[0]=='/': continue
            if "/Indet/" in iline:
                folder=str(iline.rstrip())
                print "The subfolder name is: ", folder
                continue
            # expect format 'above list of 13 entries'
            if len(tokens)!=13:
                print "Skipping malformed line with %i tokens: " % len(tokens), iline 
            else:
                subsystem=int(tokens[0])
                det=int(tokens[1])
                bec=int(tokens[2])
                layer=int(tokens[3])
                ring=int(tokens[4])
                sector=int(tokens[5])
                side=int(tokens[6])
                tx=float(tokens[7])
                ty=float(tokens[8])
                tz=float(tokens[9])
                rx=float(tokens[10])
                ry=float(tokens[11])
                rz=float(tokens[12])
                ident=det*10000+bec*1000+layer*100+ring*10+sector
                if ident not in idents:
                    data+=[(subsystem,det,bec,layer,ring,sector,side,tx,ty,tz,rx,ry,rz)]
                    idents+=[ident]
                else:
                    print "ERROR: read multiple lines for ident %i" % ident
        print "Ready to add data for %i idents" % len(data)
        
        cfolder=AtlCoolLib.ensureFolder(self.db,folder,spec,AtlCoolLib.athenaDesc(self.runLumi,'CondAttrListCollection'),cool.FolderVersioning.MULTI_VERSION)
        if (cfolder is None): sys.exit(1)

        # now print data and write to COOL folder
        print "Ident subsystem det bec layer ring sector side tx ty tz rx ry rz"
        for idata in data:
            (subsystem,det,bec,layer,ring,sector,side,tx,ty,tz,rx,ry,rz)=idata
            # determine channel number 
            channel=det*10000+bec*1000+layer*100+ring*10+sector
            print "%7i" % channel, "%3i %3i  %3i %3i %3i %3i %3i %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f" % idata
            payload=cool.Record(spec)
            payload['subsystem']=subsystem
            payload['det']=det
            payload['bec']=bec
            payload['layer']=layer
            payload['ring']=ring
            payload['sector']=sector
            payload['side']=side
            payload['Tx']=tx
            payload['Ty']=ty
            payload['Tz']=tz
            payload['Rx']=rx
            payload['Ry']=ry
            payload['Rz']=rz
            try:
                cfolder.storeObject(self.since,self.until,payload,channel,self.tag)
            except Exception,e:
                print e
                print "Store to COOL failed for channel %i" % channel
                sys.exit(-1)


smytool=setL1L2('set_L1L2folders.py',False,3,3,[])

