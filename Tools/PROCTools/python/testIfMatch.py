#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import re,sys,os
import six

from future import standard_library
standard_library.install_aliases()
import subprocess


def getOutputDictFromAMI(tag):
    try:
        from pyAMI.pyAMI import AMI
        amires = AMI().execute(["ListConfigurationTag","-configTag=%s" % tag])
        d = amires.getDict()['rowset_%s'%tag][tag]
        outDict = eval(d.get('outputs','None'))
    except Exception as e:
        print ("Failed to get OutputDict from AMI")
        print (e)
        return None
    return outDict


def getTaskNamesFromCastor():
    #cdirs=("/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_7TeV","/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_cos")
    cdirs=("/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_hi","/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_7TeV")

    tasknames=set()
    for d in cdirs:
        proj=d.split("/")[-1]
        cmd="nsls "+d
        (stat,out)=subprocess.getstatusoutput(cmd)
        if stat!=0:
            print ("ERROR, could not get list of streams from castor directory",d)
            print (out)
            return None
        for stream in out.split(os.linesep):
            tasknames.add(proj+".00000000."+stream+".merge.RAW")
    return tasknames



if __name__=='__main__':
    if len(sys.argv)!=2:
        print ("Syntax:")
        print (sys.argv[0],"<ami-tag>")
        sys.exit(-1)

    tag=sys.argv[1]
    print ("ifmatch matrix for AMI configuration tag",tag)

    outDict=getOutputDictFromAMI(tag) 
    if outDict is None:
        sys.exit(-1)

    #print (outDict)

    taskNames=getTaskNamesFromCastor()
    if taskNames is None:
        sys.exit(-1)

    #print (streamList)

    matrix=dict()
    for taskname in taskNames:
        matrix[taskname]=dict()

    for name,outDef in six.iteritems (outDict):
        if 'ifMatch' in outDef:
            print ("\n"+name+" ["+ outDef['ifMatch'] +"]")
            for taskname in taskNames:
                if re.match(outDef['ifMatch'],taskname):
                    print ("\t",taskname," MATCH")
                    matrix[taskname][name]=True
                else:
                    matrix[taskname][name]=False


 
    print ("Inverse Matrix:")
    for tn,ops in six.iteritems (matrix):
        print (tn[:-9]+": ", end='')
        #print (" %30s :" % tn[:-9], end='')
        for nm,s in six.iteritems (ops):
            if s:
                print (nm[6:-4]+" ", end='')
                #print ("%8s " % nm[6:-4], end='')
            #else:
            #    print ("        ", end='')
        print ("")
        
