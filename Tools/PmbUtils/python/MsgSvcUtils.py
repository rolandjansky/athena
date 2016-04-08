# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def parse(fn,collapse_multijobs=True,src_filter=None):
    #returns list of lists of (count,src,lvl) tuples (one list for each athena step).
    #If collapse_multijobs is set, just a single list of
    #(count,src,lvl) is returned directly.
    if isinstance(fn,str):
        if fn.endswith('.gz'):
            import gzip
            fh=gzip.open(fn)#fixme: fastopen!
        else:
            fh=open(fn)
    else:
        fh=fn
    look=False
    look_for_end=False
    outall,outone=[],[]
    for l in fh:
        if not look:
            if 'Listing sources of Unprotected and Unseen messages' in l:
                look=True
            continue
        if l.startswith('======================='):
            if look_for_end:
                outone.sort()
                outall+=[outone]
                outone=[]
                look_for_end=False
                look=False
            else:
                look_for_end=True
            continue
        l=l.split('|')
        if len(l)!=3:
            continue
        src,lvl,count=l[0].strip(),l[1].strip(),l[2].strip()
        try:
            count=int(count)
        except ValueError:
            count=None
            pass
        if count==None:
            continue
        if src_filter and not src_filter(src): continue
        outone+=[(count,src,lvl)]
    if collapse_multijobs:
        d={}
        for o in outall:
            for count,src,lvl in o:
                k=(src,lvl)
                d[k]=d.get(k,0)+count
        outall=[]
        for k,count in d.items():
            outall+=[(count,k[0],k[1])]
    return outall

def __list_to_dict(l):
    src2info={}
    for count,src,lvl in l:
        if not src in src2info.keys():
            src2info[src]=[]
        src2info[src]+=[(count,lvl)]
    return src2info

def list_to_string(l,title,asints=True):
    out=[title]
    tot=0
    src2info=__list_to_dict(l)
    res=[]
    format='%.1f %s'
    if asints: format='%i %s'
    for src,info in src2info.items():
        c=0
        for count,lvl in info: c+=count
        tot+=c
        d={}
        for count,lvl in info:
            if not lvl in d.keys(): d[lvl]=count
            else: d[lvl]+=count
        col=[]
        for lvl,count in d.items(): col+=[(count,lvl)]
        col.sort()
        col.reverse()
        s=[]
        for count,lvl in col:
            s+=[format%(count,lvl)]
        #for count,lvl in info: s+=[format%(count,lvl)]
        res+=[(c,src,', '.join(s))]
    res.sort()
    res.reverse()
    if asints: out+=['Total count: %i'%tot]
    else: out+=['Total count: %.1f'%tot]
    format=' ===> %12.1f %4.1f%% %-70s (%s)'
    if asints:
        format=' ===> %12i %4.1f%% %-70s (%s)'
    for c,src,types in res:
        out+=[format%(c,c*100.0/tot,src,types)]
    return out

def split_in_evtloop_and_overhead(n_short,l_short,n_long,l_long,ignore_negative_counts=True):

    if n_short>n_long:
        n_short,n_long=n_long,n_short
        l_short,l_long=l_long,l_short
    assert n_short>1
    fact=1.0/(n_long-n_short)
    def sub(info1,info2):
        out=[]
        for count1,lvl1 in info1:
            for count2,lvl2 in info2:
                if lvl1==lvl2:
                    count1-=count2
                    break
            out+=[(count1,lvl1)]
        return out
    def lookup(info,lvl):
        for c,l in info:
            if lvl==l: return c
        return 0
    src2info_short=__list_to_dict(l_short)
    src2info_long=__list_to_dict(l_long)
    l_evtloop=[]
    l_overhead=[]
    considered_for_overhead=set()
    for src,info_l in src2info_long.items():
        info_s=src2info_short.get(src,[])
        for count,lvl in sub(info_l,info_s):
            count_e=count*fact
            count_o=lookup(info_s,lvl)-n_short*count_e
            if count_e and (count_e>0 or not ignore_negative_counts):
                l_evtloop+=[(count_e,src,lvl)]
            considered_for_overhead.add((src,lvl))
            if count_o and (count_o>0 or not ignore_negative_counts):
                l_overhead+=[(count_o,src,lvl)]
    for src,info_s in src2info_short.items():
        for count,lvl in info_s:
            if not (src,lvl) in considered_for_overhead:
                print "recovering...",src,lvl
                l_overhead+=[(count,src,lvl)]
    return l_evtloop,l_overhead
