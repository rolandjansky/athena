# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import filecmp
import sys
import os.path


totalLeftOnly  = []
totalRightOnly = []
totalDiffer    = []

def cmpDir(left, right):

    
    dirlist = os.listdir(right)
    dirlist.extend(os.listdir(left))

    
    ignore = [file for file in dirlist if file.find('.pyc') != -1]
    ignore.extend([file for file in dirlist if file.find('~') != -1])
    ignore.append('CVS')
    ignore.append('Config')

    ignoreDict = {}
    for file in ignore:
        ignoreDict.setdefault(file, 1)

    ignore = ignoreDict.keys()

    dc = filecmp.dircmp(left, right, ignore)
    
    print "files in  ",left,' and not in ', right,":"
    print dc.left_only
    totalLeftOnly.extend(dc.left_only)
    
    print "files in  ",right,' and not in ', left,":"
    print dc.right_only
    totalRightOnly.extend(dc.right_only)
    
    print "files in  ",right,' and not in ', left,":"    
    print "files in  both, but which differ:"
    print dc.diff_files
    totalDiffer.extend(dc.diff_files)
    
    print "common dirs:"
    print dc.common_dirs

    for dir in dc.common_dirs:
        newRight= os.path.join(right,dir)
        newLeft = os.path.join(left,dir)

        cmpDir(newRight, newLeft)


if len(sys.argv) != 3:
    print "Give 2 directories as arguments"
    sys.exit(0)

right = sys.argv[1]
if not (os.path.exists(right) or os.path.isdir(right)):
    print right,' is not a directory'
    sys.exit(0)
    
left = sys.argv[2]
if not (os.path.exists(left) or os.path.isdir(left)):
    print right,' is not a directory'
    sys.exit(0)

cmpDir(left, right)



print
print "Total left only:"
print totalLeftOnly
print

print
print "Total right only:"
print totalRightOnly
print

print
print "Total common but differ:"
print totalDiffer
print
