# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import datetime

def dateToNightly(date):
    if date==None:
        return None
    today=datetime.date.today()
    if date>today:
        return None
    date_oneweekago = today+datetime.timedelta( -7 )
    if date<=date_oneweekago:
        return None
    return date.isoweekday()%7

def tagDiffs(date1,date2,tags):
    tags=','.join(tags)
    nightly1 = dateToNightly(date1)
    nightly2 = dateToNightly(date2)
    if nightly1==None or nightly2==None:
        return ['<not available>']
    if nightly1==nightly2:
        return ['<same nightly>']
    gettagproblem=False
    try:
        import PyCmt.Cmt
        diffs=PyCmt.Cmt.get_tag_diff(tags+',rel_'+str(nightly1),tags+',rel_'+str(nightly2))
    except:
        try:
            #old-style:
            import PyUtils.Cmt
            diffs=PyUtils.Cmt.get_tag_diff(tags+',rel_'+str(nightly1),tags+',rel_'+str(nightly2))
        except:
            gettagproblem=True
            pass
        pass
    if gettagproblem:
        return ['<problems retrieving tag differences>']
    l=[]

    def formatTag(s): return ('--------' if s=='None-00-00-00' else str(s)).ljust(11)
    name2line={}#To get sorting by pkg name we use this
    for diff in diffs:
        fn=str(diff['full_name'])
        if fn in name2line.keys(): print "WARNING: DUPLICATE TAG ",fn
        name2line[fn]=formatTag(diff['ref'])+' '+formatTag(diff['chk'])+' '+fn
    if not name2line:
        return ['<no differences>']
    names=name2line.keys()
    names.sort()
    for n in names:
        l+=[name2line[n]]
    return l

def formatTagDiffs(date_transitions,tags):
    lines=[]
    for (day1,day2) in date_transitions:
        if lines: lines+=['']
        lines += ["[EXPERIMENTAL] Tag differences for %s -> %s:"%(day1,day2)]
        lines+=['']
        for l in tagDiffs(day1,day2,tags):
            lines +=[str(l)]
    return lines
