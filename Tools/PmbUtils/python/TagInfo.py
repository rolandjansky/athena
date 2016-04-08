#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#module for getting quick overview (and diffs of) of tag contents of
#nightlies and releases, also more than a week back.
#
#Author: Thomas Kittelmann
#First version: May 2010

import os
import datetime

tagdirbase='/afs/cern.ch/atlas/software/dist/nightlies/nicos_work/tags'
indexbase='/afs/cern.ch/atlas/project/pmb/tags'

def datestr2date(datestr):
    if isinstance(datestr,datetime.date): return datestr
    return datetime.datetime.strptime(datestr.strip(),'%Y-%m-%d').date()

def getOverallBuildIndex(indexfile):
    idx={}
    for l in open(indexfile):
        key,rel,dates_str=l.split(';')
        dates=[]
        for d in dates_str.strip().split(','):
            dates+=[datestr2date(d)]
        if not key in idx:
            idx[key]={}
        assert not rel in idx[key].keys()
        idx[key][rel]=dates
    return idx

def findFile(key,buildidx):
    #key can here be either a release like "15.6.8.1", or a nightly
    #with a date, like "15.5.X-VAL:2010-05-04"
    #
    #Returns (status,string) where status is a boolean indicating success,
    #and string is either the absolute filename or an error message
    if not key.count(':') in [0,1]:
        return (False,"Malformed key "%key)
    key=key.strip()
    build=None
    date=None
    if ':' in key:
        key=key.split(':')
        build=key[0].strip()
        date=datestr2date(key[1])
    else:
        build=key

    dates=None
    for k,info in buildidx.items():
        if build in info.keys():
            dates=info[build]
            break
    if not dates:
        return (False,"Build %s not recognised"%build)
    if date and not date in dates:
        return (False,"Info for date %s not found"%date)
    if not date:
        dates.sort()
        date=dates[-1]#most recent

    #time to open the build index file
    if '/' in build:
        build=build.replace('/','__')
    idxfile=os.path.join(indexbase,'build_%s.txt'%build)
    #look in idxfile to find tagfile:
    searchpattern=str(date)+';'
    for l in open(idxfile):
        l=l.strip()
        if l.startswith(searchpattern):
            l=l.split(';')
            targetfile=os.path.join(tagdirbase,build,l[1],l[2])
            if not os.path.exists(targetfile):
                print "ERROR: Failed to find %s"%targetfile
            else:
                return (True,targetfile)
    return (False,"ERROR: Failed to find file for build=%s, date=%s"%(build,date))

def parseTagFile(tagfile):
    tmp_release=None
    #tmp_nightly=None
    bases=[]
    tags={}
    assert os.path.exists(tagfile),'File not found: %s'%tagfile
    for l in open(tagfile):
        parts=l.split()
        l=' '.join(parts)
        if l.startswith('#'):
            #if tmp_nightly==None and l.startswith('#nightly '):
            #    tmp_nightly = parts[1] if len(parts)>1 else ''
            if tmp_release==None and l.startswith('#release '):
                tmp_release = parts[1] if len(parts)>1 else ''
            if l.startswith('#use AtlasOffline ') or l.startswith('#use AtlasHLT ') and len(parts)==3:
                if not parts[2] in ['rel_0','rel_nightly']:
                    bases+=[parts[2]]
            continue
        if l.startswith('PackDist'):continue
        if len(parts)!=3:
            continue
        tag=parts[1]
        tag='-'.join(tag.split('-',1)[1:])
        if tag.startswith(parts[2]):
            continue
        tags[parts[0]]=tag
    return {'tags_direct':tags,'bases':bases}

__warnings_emitted=[]

def getTags(buildidx,build,datestr,include_baserel_tags=True,silent_if_unknown_build=False):
    global __warnings_emitted
    if datestr:
        key = '%s:%s'%(build,datestr)
    else:
        key=build
    (status,tagfile)=findFile(key,buildidx)
    if not status:
        if not silent_if_unknown_build: print "ERROR:",tagfile
        return None
    taginfo=parseTagFile(tagfile)
    if not include_baserel_tags:
        return taginfo
    bases=taginfo['bases']
    if not bases and datestr and datestr2date(datestr)<=datetime.date(year=2010,month=4,day=30):
        w="WARNING: Can not determine base releases reliable for data before May 2010"
        if not w in __warnings_emitted:
            __warnings_emitted+=[w]
            print w
    for b in taginfo['bases']:
        if b==build:
            #avoid recursive calls without warning (they are not normally a sign of bugs)
            pass
            #w="getTags WARNING: Avoiding recursive call for release %s (file %s)"%(b,tagfile)
            #if not w in __warnings_emitted:
            #    __warnings_emitted+=[w]
            #    print w
        else:
            tagsinbase=getTags(buildidx,b,datestr=None)
            if tagsinbase==None:
                print "ERROR: Could not get information about tags in base release %s"%b
                #if not w in __warnings_emitted:
                #    __warnings_emitted+=[w]
                #    print w
                return
            else:
                for pkg,tg in tagsinbase.items():
                    if not pkg in taginfo['tags_direct'].keys():
                        taginfo['tags_direct'][pkg]=tg
    return taginfo['tags_direct']

def diffTags(tag1,tag2):
    onlyin1=[]
    onlyin2=[]
    diffs=[]
    for p,t in tag1.items():
        if not p in tag2.keys():
            onlyin1+=[(p,t)]
            continue
        if t!=tag2[p]:
            diffs+=[(p,t,tag2[p])]
    for p,t in tag2.items():
        if not p in tag1.keys():
            onlyin2+=[(p,t)]
    diffs.sort()
    onlyin1.sort()
    onlyin2.sort()
    return (diffs,onlyin1,onlyin2)

def formatPkgTag(pkg,tag,tag2=None):
    if tag2:
        return "%s %s %s"%(tag.ljust(11),tag2.ljust(11),pkg)
    else:
        return "%s %s"%(tag.ljust(11),pkg)

def getPackageVersions(buildidx,pkgname,active_only):
    def __find_tag(tags,_pkgname):
        if not tags:
            return (None,None)
        if '/' in _pkgname:
            if _pkgname in tags:
                return (_pkgname,tags[_pkgname])
        else:
            p='/'+_pkgname
            for pkgfullpath,tag in tags.items():
                if pkgfullpath.endswith(p):
                    return (pkgfullpath,tag)
        return (None,None)

    out=[]
    for buildtype,info in buildidx.items():
        if active_only and buildtype.startswith('old'): continue
        for build,dates in info.items():
            dates.sort()
            if not dates:
                continue
            if active_only and (datetime.date.today()-dates[-1]).days > 7:
                continue
            #Find tag-info (try first without loading base release to avoid opening more files needlessly):
            tags=getTags(buildidx,build,str(dates[-1]),include_baserel_tags=False)
            (pkgfullpath,tag)=__find_tag(tags,pkgname)
            if not pkgfullpath:
                tags=getTags(buildidx,build,str(dates[-1]),include_baserel_tags=True)
                (pkgfullpath,tag)=__find_tag(tags,pkgname)
            #result:
            if not pkgfullpath:
                out+=[(build,None,None,None)]
            else:
                out+=[(build,dates[-1],pkgfullpath,tag)]
    out.sort()
    return out
