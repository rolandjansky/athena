# coolDumpFolderTags.py
# write out file coolfoldertags.txt with explicit list of COOL folders and tags
# and extra LFNs defined by CondProxyProvider

# first COOL tags and folders
filename='coolfoldertags.txt'
conddb.dumpFolderTags(filename,True)

# now add CondProxyProvider files
from AthenaCommon.AppMgr import ServiceMgr
if 'CondProxyProvider' in dir(ServiceMgr):
    ofile=open(filename,'a')
    for ifile in ServiceMgr.CondProxyProvider.InputCollections:
        # find all the direct LFN: specifications
        ix=ifile.find('LFN:')
        if (ix>=0):
            lfn=ifile[ix+4:]
            ofile.write('--addlfn %s\n' % lfn)
    ofile.close()
