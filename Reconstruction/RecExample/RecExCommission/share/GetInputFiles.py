from os import popen

def GetInputFiles(inputPath,filePattern):
    print("Searching for files with pattern '",filePattern,"' in ",inputPath)
    fileList=[]
    if (inputPath[0:8]=='/castor/'):
        cmd='nsls'
    else:
        cmd='ls -1'

    for f in popen("%(cmd)s %(path)s | grep '%(pattern)s'" \
                   % {'cmd':cmd,'path':inputPath,'pattern':filePattern}):
        if f[len(f)-1:]=='\n':
            f=f[0:len(f)-1]
        fileList+=[inputPath+'/'+f]
    print("Found ",len(fileList), " files")
    return fileList
    

def GetInputFilesFromTokens(inputPath,runnumber,prefix=".*",trigger=".*"):
    #pattern=prefix+"\.00"+str(runnumber)+"\..*"
    pattern="%(prefix)s.*%(rn)07d.%(trig)s.*" % {'prefix' : prefix ,'rn' : runnumber,'trig' : trigger}
    
    return GetInputFiles(inputPath,pattern)
