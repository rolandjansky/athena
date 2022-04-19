# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from os import popen

def GetInputFiles(inputPath,filePattern):
    print("Searching for files with pattern '",filePattern,"' in ",inputPath)
    fileList=[]
    if (inputPath[0:5]=='/eos/'):    
        cmd='/usr/bin/eos ls '
    else:
        cmd='ls -1'

    for f in popen("%(cmd)s %(path)s | grep '%(pattern)s'" \
                   % {'cmd':cmd,'path':inputPath,'pattern':filePattern}):
        if f[len(f)-1:]=='\n':
            f=f[0:len(f)-1]
        if (inputPath[0:11]=='/eos/atlas/'):    
           fileList+=['root://eosatlas.cern.ch/'+inputPath+'/'+f]
        elif (inputPath[0:10]=='/eos/user/'):   
           fileList+=['root://eosuser.cern.ch/'+inputPath+'/'+f]
        elif (inputPath[0:12]=='/eos/project'):   
           fileList+=['root://eosproject.cern.ch/'+inputPath+'/'+f]
        else:   
           fileList+=[inputPath+'/'+f]
    print ("Found ",len(fileList), " files")
    return fileList
    

def GetInputFilesFromTokens(inputPath,runnumber,prefix=".*",trigger=".*"):
    #pattern=prefix+"\.00"+str(runnumber)+"\..*"
    pattern="%(prefix)s.*%(rn)07d.%(trig)s.*" % {'prefix' : prefix ,'rn' : runnumber,'trig' : trigger}
    
    return GetInputFiles(inputPath,pattern)

def GetInputFilesFromPattern(inputPath,prefix=".*",trigger=".*"):
    pattern="%(prefix)s.*%(trig)s.*" % {'prefix' : prefix ,'trig' : trigger}
    
    return GetInputFiles(inputPath,pattern)

def GetInputFilesFromPrefix(inputPath,prefix=".*"):
    pattern="%(prefix)s.*" % {'prefix' : prefix }
    
    return GetInputFiles(inputPath,pattern)
