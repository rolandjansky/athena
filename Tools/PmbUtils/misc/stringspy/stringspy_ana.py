# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def cppfilt(s):
    if '+0x' in s: s=s.split('+0x')[0]
    if '(' in s: s=s.split('(')[1]
    import subprocess
    p = subprocess.Popen(['/usr/bin/c++filt',s], stdout=subprocess.PIPE)
    return p.communicate()[0].strip()

def analyse(filename):
    symb2waste={}
    symb2total={}
    fullsymb2total={}
    fullsymb2waste={}
    symb2fullsymbs={}
    i=0
    for l in open(filename):
        i+=1
        if not l.startswith('libstringspy::'): continue
        if l.startswith('libstringspy::Wasted') and 'bytes in' in l:
            waste,fullsymb=l.split('bytes in',2)
            waste=float(waste.split()[-1])
            symb=fullsymb.split('(')[0].strip()
            fullsymb=fullsymb.replace(symb,'')
            if not symb in symb2fullsymbs: symb2fullsymbs[symb]=set([fullsymb])
            else:  symb2fullsymbs[symb].add(fullsymb)
            if not symb in symb2waste: symb2waste[symb]=waste
            else: symb2waste[symb]+=waste
            if not fullsymb in fullsymb2waste: fullsymb2waste[fullsymb]=waste
            else: fullsymb2waste[fullsymb]+=waste
        elif l.startswith('libstringspy::Total') and 'bytes in' in l:
            total,fullsymb=l.split('bytes in',2)
            total=float(total.split()[-1])
            symb=fullsymb.split('(')[0].strip()
            fullsymb=fullsymb.replace(symb,'')
            if not symb in symb2fullsymbs: symb2fullsymbs[symb]=set([fullsymb])
            else:  symb2fullsymbs[symb].add(fullsymb)
            if not symb in symb2total: symb2total[symb]=total
            else: symb2total[symb]+=total
            if not fullsymb in fullsymb2total: fullsymb2total[fullsymb]=total
            else: fullsymb2total[fullsymb]+=total

    print i,len(symb2waste),len(symb2total)
    l=[]
    for symb,val in symb2total.items():
        l+=[(val,symb)]
    l.sort(reverse=True)
    b2mb=1.0/(1024.0*1024.0)
    for val,symb in l[0:20]:
        waste=symb2waste.get(symb,0.0)
        print '* %2.1f MB (%.1f MB shareable) %s'%(val*b2mb,waste*b2mb,symb)
        fullsymbs=[(fullsymb2total[fs],fullsymb2waste.get(fs,0),fs) for fs in symb2fullsymbs[symb]]
        fullsymbs.sort(reverse=True)
        for v,w,s in fullsymbs[0:10]:
            if v<val*0.01 and w<0.01*waste: continue
            if v<1024*100: continue
            print '     * %2.1f MB (%.1f MB shareable) %s'%(v*b2mb,w*b2mb,cppfilt(s.strip())[0:100])
if __name__=='__main__':
    import sys
    args=sys.argv[1:]
    assert len(args)==1
    analyse(args[0])
