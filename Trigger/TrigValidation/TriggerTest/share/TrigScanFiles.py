from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import os,commands
def getCastorDirectoryList(path,fileRange):
    cmd  = 'eos ls %s/ ' % (path)
    files = []
    for i in commands.getoutput(cmd).split('\n'):
        fileNumber = i[i.rfind(".pool.root")-5:i.rfind(".pool.root")]
        print 'fileNumber',fileNumber
        if  int(fileNumber) > fileRange[0]-1 and int(fileNumber) <fileRange[1]+1 :
            collection = 'root://eosatlas/'+path+i
            files += ["root://eosatlas/%s/%s" % (path,i) ]
    print files 
    return files 

def getCastorDirectoryListRepeat(path,fileRange,repeatN):
    cmd  = 'eos ls %s/ ' % (path)
    files = []
    for j in range(repeatN):
        for i in commands.getoutput(cmd).split('\n'):
            fileNumber = i[i.rfind(".pool.root")-5:i.rfind(".pool.root")]
            print 'fileNumber',fileNumber
            if  int(fileNumber) > fileRange[0]-1 and int(fileNumber) <fileRange[1]+1 :
                collection = 'root://eosatlas/'+path+i
                files += ["root://eosatlas/%s/%s" % (path,i) ]
        print files 
        return files 

def getEOSDirectoryList(path,fileRange):
    cmd  = 'eos ls %s/ ' % (path)
    files = []
    print cmd
    for i in commands.getoutput(cmd).split('\n'):
        print i
        if i.find(".pool.root")==-1:
            continue
        fileNumber = i[i.rfind(".pool.root")-5:i.rfind(".pool.root")]
        print 'fileNumber',fileNumber
        if  int(fileNumber) > fileRange[0]-1 and int(fileNumber) <fileRange[1]+1 :
            collection = 'root://eosatlas/'+path+i
            print collection
            files += ["root://eosatlas//%s/%s" % (path,i) ]
    print files
    return files

def getEOSDirectoryListRepeat(path,fileRange,repeatN):
    cmd  = 'eos ls %s/ ' % (path)
    files = []
    for j in range(repeatN):
        for i in commands.getoutput(cmd).split('\n'):
            print i
            if i.find(".pool.root")==-1:
                continue
            fileNumber = i[i.rfind(".pool.root")-5:i.rfind(".pool.root")]
            print 'fileNumber',fileNumber
            if  int(fileNumber) > fileRange[0]-1 and int(fileNumber) <fileRange[1]+1 :
                collection = 'root://eosatlas/'+path+i
                files += ["root://eosatlas//%s/%s" % (path,i) ]
        print files
        return files

if (('dsName' in dir())  and ('fileRange' in dir()) and ( not ('repeatN' in dir()))  ):
    print "looking for EOS files"
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    athenaCommonFlags.FilesInput=getEOSDirectoryList(dsName,fileRange)
    #athenaCommonFlags.FilesInput=getCastorDirectoryList(dsName,fileRange)
elif (('dsName' in dir())  and ('fileRange' in dir()) and ('repeatN' in dir()) ):
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    athenaCommonFlags.FilesInput=getEOSDirectoryListRepeat(dsName,fileRange,repeatN)
    #athenaCommonFlags.FilesInput=getCastorDirectoryListRepeat(dsName,fileRange,repeatN)
