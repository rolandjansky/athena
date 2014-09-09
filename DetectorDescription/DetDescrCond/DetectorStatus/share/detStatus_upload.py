#!/usr/bin/env python
# detStatus_upload.py - tool for uploading detector status in COOL
# conditions DB from a file giving list of runs (and optionally LBs)
# Richard Hawkings, 5/2/07
# execute without arguments for help
# the format of the file is a list of runs on separate lines,
# or a list of <run> <LB1> <LB2> (inclusive) for LB ranges within run

import os,sys
from PyCool import cool
from CoolConvUtilities import AtlCoolLib
from DetectorStatus import DetStatusLib

class setStatus(AtlCoolLib.coolTool):
    def setup(self,args):
        # set values of non-optional parameters
        self.filename=str(args[0])
        self.tag=str(args[1])
        self.detlist=args[2:]
        self.foldername=''
        self.flag=''
        self.newtag=False
        # access to name encoding
        self.namelookup=DetStatusLib.DetStatusNames()
        
    def usage(self):
        """ Define the additional syntax for options """
        self._usage1()
        print 'input_filename COOL_tag det1 {det2 det3 ...}'
        print 'Options are:'
        print '--folder=<folder> : Folder to use (default /GLOBAL/DETSTATUS/LBSUMM)'
        print '--flag=<flag> : Flag value to assume if not in input file (R/Y/G or 1/2/3)'
        print '--newtag:  Allow creation of a new tag, not already existing in folder'
        print 'Provides a way to bulk upload a set of flag values from a file'
        print 'specifying run numbers or run-LB ranges'
        

    def procopts(self,opts):
        "Process the additional non-standard options"
        for o,a in opts:
            if (o=='--folder'):
                self.foldername=str(a)
            if (o=='--flag'):
                self.flag=str(a)
            if (o=='--newtag'):
                self.newtag=True

    def execute(self):
        # check if flag given on command line, if not will get from each file
        # input line
        if (self.flag!=''):
            status=DetStatusLib.colourVal(self.flag)
            if (status is None): sys.exit(1)
        chanlist=self.getchans()
        if len(chanlist)==0: sys.exit(1)

        # folder name depends on choice of run-lumi or timestamp, or explicit
        if (self.foldername==''):
            folder=DetStatusLib.folderName(self.runLumi)
        else:
            folder=self.foldername
        print ">== Store object in folder",folder
        hascomment=(folder=='/GLOBAL/DETSTATUS/SHIFTOFL' or folder=='/GLOBAL/DETSTATUS/SHIFTONL')
        
        # now do update - setup folder specification and create if needed
        spec=cool.RecordSpecification()
        spec.extend("Code",cool.StorageType.Int32)
        spec.extend("deadFrac",cool.StorageType.Float)
        spec.extend("Thrust",cool.StorageType.Float)
        if (hascomment):
            spec.extend("Comment",cool.StorageType.String255)
        
        cfolder=AtlCoolLib.ensureFolder(self.db,folder,spec,AtlCoolLib.athenaDesc(self.runLumi,'CondAttrListCollection')+'<named/>',cool.FolderVersioning.MULTI_VERSION)
        if (cfolder is None): sys.exit(1)
        
        # check if tag exists
        if self.tag!='HEAD':
            taglist=cfolder.listTags()
            if self.tag not in taglist:
                if self.newtag:
                    print ">== Tag %s will be created" % self.tag
                else:
                    print ">== ERROR: Tag %s does not exist (use --newtag if you really want to create it)" % self.tag
                    sys.exit(1)
                    
        # now write data according to list in file
        datafile=open(self.filename,'r')
        # setup storage buffer ready for input
        cfolder.setupStorageBuffer()
        nobj=0
        print "Reading data from file %s to insert in tag %s" % (self.filename,self.tag)
        for line in datafile.readlines():
            tokens=line[:-1].split()
            if (len(tokens)>0 and tokens[0][0]!='#'):
                # if no flag value specified on command line, take from first
                # argument on line
                if (self.flag==''):
                    status=DetStatusLib.colourVal(tokens[0])
                    if (status is None):
                        print "Cannot define status from line %s" % line
                        sys.exit(-1)
                    del tokens[0]
                run1=int(tokens[0])
                run2=run1
                if len(tokens)==2: run2=int(tokens[1])
                if len(tokens)<3:
                    lb1=0
                    lb2=(1 << 32)-1
                else:
                    lb1=int(tokens[1])
                    lb2=int(tokens[2])
                if len(tokens)>3:
                    # take the rest of the line from where the comment starts
                    comment=line[line.find(tokens[3]):-1]
                else:
                    comment=""
                if (comment!="" and not hascomment):
                    print "Comment %s specified for folder type without comments" % comment
                    sys.exit(-1)
                if comment=="":
                    commstr=""
                else:
                    commstr=' comment "%s"' % comment
                print "Update status for [%i,%i] to [%i,%i] to %s%s" % (run1,lb1,run2,lb2,DetStatusLib.colour(status),commstr)
                payload=cool.Record(spec)
                payload['Code']=status
                payload['deadFrac']=0.
                payload['Thrust']=0.
                if (hascomment):
                    payload['Comment']=comment
                since=(run1 << 32)+lb1
                until=(run2 << 32)+lb2+1
                for chan in chanlist:
                    if (self.tag=="HEAD"):
                        cfolder.storeObject(since,until,payload,chan)
                    else:
                        cfolder.storeObject(since,until,payload,chan,self.tag,True)
                    nobj+=1
        datafile.close()
        if (nobj>0):
            chk=raw_input("Execute upload of %i objects (y/n)" % nobj)
            if (chk.upper()=="Y"):
                try:
                    cfolder.flushStorageBuffer()
                    print "Data written to COOL"
                except Exception,e:
                    print "Error during bulk upload",e
            else:
                print "Upload ABORTED - not done"
        print "All done"
    
    def getchans(self):
        chanlist=[]
        for det in self.detlist:
            updet=det.upper()
            if (updet=='ALL'):
                # set chanlist to ONLY all detectors
                chanlist=self.namelookup.allNums()
            else:
                try:
                    chanlist+=self.namelookup.nums(updet)
                except KeyError:
                    print '>== ERROR: Status name',det,'is not defined'
        print "Defined list of %i status channels:" % len(chanlist) ,
        for i in chanlist: print self.namelookup.name(i),
        print
        return chanlist

mytool=setStatus('detStatus_upload.py',False,3,99,['newtag','flag=','folder='])
