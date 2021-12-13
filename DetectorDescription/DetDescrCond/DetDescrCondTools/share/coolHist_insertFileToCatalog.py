#!/bin/env python
# coolHist_insertFileToCatalog.py
# emulate of pool_insertFileToCatalog for ROOT histogram files with fileGUID
# Richard Hawkings, 7/2/07
# coolHist_insertFileToCatalog.py [-u fileCatalog] files

import os,sys,getopt

def usage():
    print("Usage: coolHist_insertFileToCatalog.py {-u fileCatalog} file1 {file2}...")
    print("Default file catalogue is PoolFileCatalog.xml")

# get options (only -u)
destcat='file:PoolFileCatalog.xml'
try:
    opts,args=getopt.getopt(sys.argv[1:],'u:',[])
    for o,a in opts:
        if o=='-u': destcat=a
        
except getopt.GetoptError:
    usage()
    sys.exit(1)
if len(args)<1:
    usage()
    sys.exit(1)
print(f"Destination catalog is {destcat}")

# open fake source catalogue and write header
sourcename='coolhist_tempcat.xml'
sourcecat=open(sourcename,'w')
sourcecat.write("""<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<POOLFILECATALOG>
""")

for myfile in args:
    print(f">= Obtain GUID for file {myfile}")
    rc=os.system('coolHist_extractFileIdentifier.sh %s' % myfile)
    if (rc==0):
        guid=open('coolhist_guid.tmp','r').readline()[:-1]
        if (guid!='none'):
            sourcecat.write("""
    <File ID=\"%s\">
    <physical>
    <pfn filetype=\"ROOT_All\" name=\"%s\"/>
    </physical>
    </File>
""" % (guid,myfile))
        else:
            print(f">= No GUID available for file {myfile}")
    else:
        print(f">= Problems obtaining GUID for file {myfile}")
# write catalogue trailer
sourcecat.write('</POOLFILECATALOG>')
sourcecat.close()

# now merge catalogues using Pool utility
comm='FCpublish -d %s -u file:%s' % (destcat,sourcename)
print(f"Execute {comm}")
rc=os.system(comm)
if (rc!=0):
    print("Error from Pool FCpublish")
else:
    print("Insertion done")
