# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from listFiles import listFiles
import os
import shutil
import sys
import getopt

def optusage():
    print 'usage: python updateRTTSummaryReferenceInSrcDir -t [rel path to testsuite dir] --live[to really make the copy]'
    
def getParams():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 't:', ['live'])
    except getopt.GetoptError:
        optusage()
        sys.exit(2)

    suite = None
    live = False
    for o,a in opts:
        if o == '-t':     suite = a
        if o == '--live': live = True

    if not suite:
        optusage()
        sys.exit(2)

    return (suite, live)


suite, live = getParams()
suiteDir = os.path.abspath (suite)
assert (os.path.exists(suite))

destDir = os.path.normpath(os.path.join(os.environ['RTTSrcDir'], '../testsuite/testsuites'))
assert(os.path.exists(destDir))
dest = os.path.normpath( os.path.join(destDir, os.path.basename(suiteDir), 'TestCase.py'))

src = os.path.join(suiteDir, 'TestCase.py')
assert(os.path.exists(src))



if live:
    print 'Copier IS live, copying %s to %s' % (src, dest)
    shutil.copyfile(src, dest)
            
else:
    print '\n\nCopier is NOT  live, copying from:\n%s\n' % (src)
    print '\n\nCopier is NOT  live, copying to:\n%s'     % (dest)
    print '\nCopier is NOT  live, NO copying was done'

