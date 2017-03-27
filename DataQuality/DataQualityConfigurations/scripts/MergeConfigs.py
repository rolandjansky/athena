#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
MergeConfigs: merges han text configuration files, warning on name conflicts
@author Peter Onyisi <ponyisi@hep.uchicago.edu>
"""

def list_directories(parent_dir, recurse=True):
    """
    returns a list of directories in parent_dir
    Directory 'common' will appear first if present
    if recurse = True, also includes all subdirectories in the tree
    """
    import os
    rv = []
    if recurse:
        for root, dum1, dum2 in os.walk(parent_dir):
            if root == parent_dir:
                continue
            rv.append(root)
    else:
        import stat
        subobjs = os.listdir(parent_dir)
        rv = [os.join(parent_dir, x) for x in subobjs]
        rv = [x for x in rv if stat.S_ISDIR(os.stat(x))]
    rv.sort()
    truerv = []
    for x in rv:
        if 'common' in x:
            truerv = [x] + truerv
        else:
            truerv.append(x)
    return truerv

def _is_ok(dir, options):
    return not dir.startswith(tuple(options.exclude), 2)

def merge_han_configs(template, parent_dir, out, options):
    import sys, os
    """
    Merge the files in the input list to the output file.
    @param inlist: list of filenames of input files
    @param out: output filename
    """
    import re
    
    rematch = re.compile('(\S*)\s+(\S*)\s+{')
    # keywords we should worry about for name conflicts
    kwlist = set(['algorithm', 'compositeAlgorithm', 'reference', 'thresholds'])
    kwhash = {}
    for kw in kwlist:
        kwhash[kw] = {}

    files = []

    # First pass; find if there are name conflicts
    for dir in list_directories(parent_dir):
        f = os.path.join(dir, template)
        if not os.access(f, os.R_OK):
            continue
        if not _is_ok(dir, options):
            print dir, 'excluded from merge'
            continue
        print 'Processing', f
        files.append(f)
        fobj = open(f, 'r')
        # reclevel = how nested we are; reclist = directories above us; kws = previous keywords
        reclevel = 0; reclist = []; kws = []

        for line in fobj:
            stripline = line.strip()
            if len(stripline) == 0:
                continue
            if stripline[0] == '#':
                continue
            if stripline[0] == '}' and reclevel > 0:
                # in principle a properly parsing file should only close objects it has opened.  Dangerous.
                reclevel -= 1; reclist.pop(); kws.pop()
                continue
            match = rematch.search(line)
            if match == None:
                continue
            kws.append(match.group(1)); reclevel += 1; reclist.append(match.group(2))
            if kws[-1] in kwlist:
                # print line
                # print match.group(1), match.group(2),
                fullname = '/'.join(reclist)
                # print fullname
                if fullname in kwhash[match.group(1)]:
                    # print reclist
                    print 'ERROR: repeated definition of %s %s' % (match.group(1), fullname)
                    print '       Current file is %s' % f
                    print '       Earlier definition was in %s' % kwhash[match.group(1)][fullname]
                    print '       Please fix this.  Merging will now stop.  Output file has not been created.'
                    sys.exit(1)
                else:
                    kwhash[match.group(1)][fullname] = f
        fobj.close()
                    
    # if we got here, we should be ok on the unique names: pass 2
    outobj = open(out, 'w')
    outobj.write('# ****************************\n')
    outobj.write('metadata GitInfo {\n')
    outobj.write('  Hash = %s\n' % (os.environ.get('GIT_COMMIT_HASH', 'unknown')))
    outobj.write('}\n')
    outobj.write('# ****************************\n\n')
    for f in files:
        fobj = open(f, 'r')
        for line in fobj:
            if '$Id:' in line:
                line += '# ' + f + '\n'
                line = line.replace('$', '')
            outobj.write(line)
        outobj.write('\n')

_usage = ('%prog [options] templatename parent_dir outfile\n\n'
          'example: %prog cosmics_run.config . cosmics_run_merged.config')

if __name__ == '__main__':
    import sys
    import optparse
    parser = optparse.OptionParser(usage=_usage)
    parser.add_option('--exclude', action='append', metavar='DIRECTORY',
                      default=[],
                      help='Exclude following directory from merge; can be repeated for multiple directories')
    options, args = parser.parse_args()
    
    if len(args) != 3:
        usage()
    merge_han_configs(sys.argv[1], sys.argv[2], sys.argv[3], options)
