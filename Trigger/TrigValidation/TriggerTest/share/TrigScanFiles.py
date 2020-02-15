from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from future import standard_library
standard_library.install_aliases()

def getCastorDirectoryList(path,fileRange):
    cmd  = 'eos ls %s/ ' % (path)
    files = []
    for i in subprocess.getoutput(cmd).split('\n'):
        fileNumber = i[i.rfind(".pool.root")-5:i.rfind(".pool.root")]
        printfunc ('fileNumber',fileNumber)
        if  int(fileNumber) > fileRange[0]-1 and int(fileNumber) <fileRange[1]+1 :
            collection = 'root://eosatlas.cern.ch/'+path+i
            files += ["root://eosatlas.cern.ch/%s/%s" % (path,i) ]
    printfunc (files )
    return files 

def getCastorDirectoryListRepeat(path,fileRange,repeatN):
    cmd  = 'eos ls %s/ ' % (path)
    files = []
    for j in range(repeatN):
        for i in subprocess.getoutput(cmd).split('\n'):
            fileNumber = i[i.rfind(".pool.root")-5:i.rfind(".pool.root")]
            printfunc ('fileNumber',fileNumber)
            if  int(fileNumber) > fileRange[0]-1 and int(fileNumber) <fileRange[1]+1 :
                collection = 'root://eosatlas.cern.ch/'+path+i
                files += ["root://eosatlas.cern.ch/%s/%s" % (path,i) ]
        printfunc (files )
        return files 

def getEOSDirectoryList(path,fileRange):
    cmd  = 'eos ls %s/ ' % (path)
    files = []
    printfunc (cmd)
    for i in subprocess.getoutput(cmd).split('\n'):
        printfunc (i)
        if i.find(".pool.root")==-1:
            continue
        fileNumber = i[i.rfind(".pool.root")-5:i.rfind(".pool.root")]
        printfunc ('fileNumber',fileNumber)
        if  int(fileNumber) > fileRange[0]-1 and int(fileNumber) <fileRange[1]+1 :
            collection = 'root://eosatlas.cern.ch/'+path+i
            printfunc (collection)
            files += ["root://eosatlas.cern.ch//%s/%s" % (path,i) ]
    printfunc (files)
    return files

def getEOSDirectoryListRepeat(path,fileRange,repeatN):
    cmd  = 'eos ls %s/ ' % (path)
    files = []
    for j in range(repeatN):
        for i in subprocess.getoutput(cmd).split('\n'):
            printfunc (i)
            if i.find(".pool.root")==-1:
                continue
            fileNumber = i[i.rfind(".pool.root")-5:i.rfind(".pool.root")]
            printfunc ('fileNumber',fileNumber)
            if  int(fileNumber) > fileRange[0]-1 and int(fileNumber) <fileRange[1]+1 :
                collection = 'root://eosatlas.cern.ch/'+path+i
                files += ["root://eosatlas.cern.ch//%s/%s" % (path,i) ]
        printfunc (files)
        return files

if (('dsName' in dir())  and ('fileRange' in dir()) and ( not ('repeatN' in dir()))  ):
    printfunc ("looking for EOS files")
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    athenaCommonFlags.FilesInput=getEOSDirectoryList(dsName,fileRange)
    #athenaCommonFlags.FilesInput=getCastorDirectoryList(dsName,fileRange)
elif (('dsName' in dir())  and ('fileRange' in dir()) and ('repeatN' in dir()) ):
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    athenaCommonFlags.FilesInput=getEOSDirectoryListRepeat(dsName,fileRange,repeatN)
    #athenaCommonFlags.FilesInput=getCastorDirectoryListRepeat(dsName,fileRange,repeatN)
