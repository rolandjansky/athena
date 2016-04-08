#!/usr/bin/env python
import os

bn=os.path.basename(__file__).replace('.py','')

env_cmtpath=os.getenv('CMTPATH')
cmtpath=[c for c in env_cmtpath.split(':') if os.path.exists(c)]
fixpref='/build/atnight/'

def fix(filepath):
    rel='/rel_nightly/' if 'rel_nightly' in filepath else '/rel_0/'
    f=filepath.split(rel,1)
    if len(f)==2:
        for c in cmtpath:
            if os.path.exists(os.path.join(c,f[1])):
                return os.path.join(c,f[1])
    print '%s WARNING Could not fix %s'%(bn,filepath)
    return filepath

def get_first_to_fix(l):
    if l.startswith(fixpref):
        return l.split()[0]
    if fixpref in l:
        return fixpref+l.split(fixpref)[1].split()[0]

def which(cmdname):
    import subprocess
    p=subprocess.Popen(['/usr/bin/which',cmdname], stdout=subprocess.PIPE).communicate()[0]
    return p.strip()

def fix_file(f,outfile=None):

    if outfile and outfile!=f:
        in_place=False
    else:
        in_place=True
        outfile=f+'.tmp_fixing'

    if f.endswith('.bz2'):
        import bz2
        fh=bz2.BZ2File(f)
        outfh=bz2.BZ2File(outfile,'w')
    elif f.endswith('.gz'):
        import gzip
        fh=gzip.open(f)
        outfh=gzip.open(outfile,'w')
    else:
        fh=open(f)
        outfh=open(outfile,'w')

    nfixed=0
    for l in fh:
        tofix=get_first_to_fix(l)
        i=0
        while tofix and i<10:
            i+=1
            if tofix in l:
                nfixed+=1
                l=l.replace(tofix,fix(tofix))
            tofix=get_first_to_fix(l)
            if not l.endswith('\n'): l+='\n'
        outfh.write(l)
    outfh.close()
    fh.close()
    if in_place:
        os.remove(f)
        os.rename(outfile,f)
        outfile=f
    return nfixed
    
if __name__=='__main__':
    import sys
    if len(sys.argv)<=1:
        print '%s ERROR Please supply a list of ascii (potentially bziped or gzipped) files in which you need build paths translated to accessible paths'%bn
        sys.exit(1)
    for f in sys.argv[1:]:
        print '%s INFO Attempting to fixup %s'%(bn,f)
        if not os.path.exists(f):
            print '%s ERROR Could not open file %s'%(bn,f)
            continue
        nfixed=fix_file(f)
        print '%s %s Replaced %i file paths in %s'%(bn,('INFO' if nfixed else 'WARNING'),nfixed,f)
