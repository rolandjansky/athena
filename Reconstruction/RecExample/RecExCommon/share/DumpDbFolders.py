#This file is supposed to run as transform postExec
#It creates text files containing the all database folders that are set up
#This should help the creation of custom DB releases

from os import linesep,getpid

allFolders=svcMgr.IOVDbSvc.Folders

substep=""
try:
    from RecExConfig.RecFlags import rec
    substep=rec.OutputFileNameForRecoStep()
except: 
    pass

if len(substep)==0:
    print "Failed to obtain substep, will use PID for file with db folder list" 
    substep=str(getpid())



outputFileName="DatabaseFolders_"+substep+".txt"

dbFolderFile=open(outputFileName,"w")
for fn in allFolders:
    dbFolderFile.write(fn+linesep)

dbFolderFile.close()

del outputFileName
del allFolders
del substep
