#!/bin/env python
# set_ibldist.py - tool to generate COOL sqlite DB fragment for IBL distortion
# Richard Hawkings, 21/5/15
# execute without arguments for help
import os,sys
from PyCool import cool
from CoolConvUtilities import AtlCoolLib

class setL1(AtlCoolLib.coolTool):
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
        spec.extend("bec",cool.StorageType.Int32)
        spec.extend("layer",cool.StorageType.Int32)
        spec.extend("sector",cool.StorageType.Int32)
        spec.extend("Tx",cool.StorageType.Float)
        spec.extend("Ty",cool.StorageType.Float)
        spec.extend("Tz",cool.StorageType.Float)
        spec.extend("phi",cool.StorageType.Float)
        spec.extend("theta",cool.StorageType.Float)
        spec.extend("psi",cool.StorageType.Float)

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
            if "/TRT/" in iline:
                folder=str(iline.rstrip())
                print "The subfolder name is: ", folder
                continue
            # expect format 'above list of 13 entries'
            if len(tokens)!=9:
                print "Skipping malformed line with %i tokens: " % len(tokens), iline 
            else:
                bec=int(tokens[0])
                layer=int(tokens[1])
                sector=int(tokens[2])
                tx=float(tokens[3])
                ty=float(tokens[4])
                tz=float(tokens[5])
                phi=float(tokens[6])
                theta=float(tokens[7])
                psi=float(tokens[8])
                ident=1000+bec*100+layer*10+sector
                if ident not in idents:
                    data+=[(bec,layer,sector,tx,ty,tz,phi,theta,psi)]
                    idents+=[ident]
                else:
                    print "ERROR: read multiple lines for ident %i" % ident
        print "Ready to add data for %i idents" % len(data)
        
        cfolder=AtlCoolLib.ensureFolder(self.db,folder,spec,AtlCoolLib.athenaDesc(self.runLumi,'CondAttrListCollection'),cool.FolderVersioning.MULTI_VERSION)
        if (cfolder is None): sys.exit(1)

        # now print data and write to COOL folder
        print "Ident bec layer sector tx ty tz phi theta psi"
        for idata in data:
            (bec,layer,sector,tx,ty,tz,phi,theta,psi)=idata
            # determine channel number 
            channel=1000+bec*100+layer*10+sector
            print "%7i" % channel, "%3i %3i  %3i  %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f" % idata
            payload=cool.Record(spec)
            payload['bec']=bec
            payload['layer']=layer
            payload['sector']=sector
            payload['Tx']=tx
            payload['Ty']=ty
            payload['Tz']=tz
            payload['phi']=phi
            payload['theta']=theta
            payload['psi']=psi
            try:
                cfolder.storeObject(self.since,self.until,payload,channel,self.tag)
            except Exception,e:
                print e
                print "Store to COOL failed for channel %i" % channel
                sys.exit(-1)


smytool=setL1('set_L1TRTfolders.py',False,3,3,[])

