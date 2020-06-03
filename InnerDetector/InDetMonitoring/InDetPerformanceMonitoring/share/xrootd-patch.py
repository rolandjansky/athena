### for 20.7.7+ athena releases the following
# setting up environmental variables for xrootd patch
# get your local copy from:
# /afs/cern.ch/work/w/wdicleme/public/Alignment/20.7.8/xrootd-4.2.3-install
# and put it in your $TestArea/. path
#

import os
print "xrootd path"
xrootd_path = os.getenv("TestArea") + "/xrootd-4.2.3-install/"
print "TestArea contents"
os.listdir(os.getenv("TestArea"))
print "xrootd_path contents"
os.listdir(xrootd_path)

if os.path.exists(xrootd_path):
    print "Patching xrootd with ",xrootd_path
else:
    print "xrootd patch not found. This will probably fail..."

if not 'LD_LIBRARY_PATH' in os.environ:
    os.environ['LD_LIBRARY_PATH'] = ":" + xrootd_path + "lib64"
else:
    os.environ['LD_LIBRARY_PATH'] += ":" +xrootd_path + "lib64"

if not 'PATH' in os.environ:
    os.environ['PATH'] = ":" +xrootd_path + "bin"
else:
    os.environ['PATH'] += ":" +xrootd_path + "bin"

if not 'ROOT_INCLUDE_PATH' in os.environ:
    os.environ['ROOT_INCLUDE_PATH'] = ":" +xrootd_path + "include"
else:
    os.environ['ROOT_INCLUDE_PATH'] += ":" +xrootd_path + "include"
