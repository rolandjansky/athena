#!/usr/bin/env python
# usage: expand_files [options] [files] [options] [files]
# Return a list of filenames with the package name prepended
# Files will be collected from source dirs, and prepended with the
# destination dir.
# Options:
# -r=<rootdir>: Root directory of the package. Is used to search for the
#           files in the correct package.
# -s=<srcdir>: Default source directory. Will be used if no directory is
#          specified before a filename. This option can be inserted
#          in between the filenames.
# -d=<destdir>: destination directory. Will be prepended to each filename.
#          Can be inserted in between the filenames. The last one before
#          a filename will be used.
import sys,os,glob
srcdir = os.curdir
destdir = ''
rootdir=''
files=[]
for args in sys.argv[1:]:
    for arg in args.split():
        if arg.startswith('-s='):
            # get a new default directory
            srcdir=arg[3:]
        elif arg.startswith('-d='):
            destdir=arg[3:]
        elif arg.startswith('-r='):
            rootdir=arg[3:]
        else:
            argdir = os.path.dirname(arg)
            if not argdir: argdir = srcdir
            argname = os.path.basename(arg)
            fullpath = os.path.normpath( os.path.join( rootdir,'cmt',argdir,argname) )
            filelist = glob.glob( fullpath )
            files += [ os.path.join(destdir,os.path.basename(f)) \
                       for f in filelist ]

if files:
    print ' '.join( files )
