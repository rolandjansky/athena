#!/usr/bin/env python

#Script for getting quick overview (and diffs of) of tag contents of
#nightlies and releases, also more than a week back.
#
#Author: Thomas Kittelmann
#First version: May 2010

import os
import datetime
import PmbUtils.TagInfo

def parseOptions():
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("--diff", dest="diffstring",metavar='BUILDS',
                      help="diff tags in two BUILDS Accepted formats are BUILD:DATE1:DATE2 or BUILD1:DATE1:BUILD2:DATE2. Also possible if at least one build is a fixed build rather than a nightly are BUILD1:BUILD2, BUILD1:DATE1:BUILD2 or BUILD1:BUILD2:DATE2."
                      +" Example1: --diff=15.6.X.Y-Prod:2010-05-20:2010-05-15"
                      +" Example2: --diff=17.X.0-VAL:2012-02-09:17.5.0")
    parser.add_option("--showreleases",action="store_true", dest="showreleases", default=False,help="print a list of available releases with recent activity")
    parser.add_option("--showall",action="store_true", dest="showall", default=False,help="use with --showreleases to display all available releases")
    parser.add_option("--showtags",metavar='BUILD',
                      help="show tags used in BUILD. Format should be BUILD:DATE or just BUILD. Example: --showtags=15.6.X.Y-Prod:2010-05-20 or --showtags=15.6.9.9")

    parser.add_option("--pkg", dest="pkg",metavar='PKG',
                      help="Show tags of PKG in main active nightlies and recent releases."
                      +" Example: --pkg=RecExCommon")

    parser.add_option("--pkgall", dest="pkgall",metavar='PKG',
                      help="Show tags of PKG in all nightlies and releases."
                      +" Example: --pkgall=RecExCommon")

    (opt, args) = parser.parse_args()
    if len(args):
        parser.error('Unknown options: '+' '.join(args))
    if not opt.diffstring and not opt.showreleases and not opt.showtags and not opt.pkg and not opt.pkgall:
        parser.print_help()
    if opt.pkg and opt.pkgall:
        parser.print_help()
    return opt

if __name__=='__main__':
    import sys
    opt=parseOptions()
    buildidx=PmbUtils.TagInfo.getOverallBuildIndex(os.path.join(PmbUtils.TagInfo.indexbase,'index.txt'))

    if opt.showreleases:
        for buildtype,info in buildidx.items():
            if not opt.showall and buildtype.startswith('old'): continue
            print 'Builds of type "%s"'%buildtype
            for build in info.keys():
                print "  %s"%build

    if opt.showtags:
        if ':' in opt.showtags:
            (build,date)=opt.showtags.split(':')
        else:
            (build,date)=(opt.showtags,None)
        tags=PmbUtils.TagInfo.getTags(buildidx,build,date)
        if not tags:
            print "ERROR: Could not retrieve tags!"
        else:
            pkgs=tags.keys()
            pkgs.sort()
            for p in pkgs:
                print PmbUtils.TagInfo.formatPkgTag(p,tags[p])

    if opt.pkg or opt.pkgall:
        if opt.pkgall:
            pkgname=opt.pkgall
            active_only=False
        else:
            pkgname=opt.pkg
            active_only=True

        for pkgname_single in pkgname.split(','):
            pkgversions=PmbUtils.TagInfo.getPackageVersions(buildidx,pkgname_single,active_only=active_only)

            def __is_not_main_build(b):
                #a bit hackish list of stuff to ignore here:
                if b.startswith('16.'): return True
                b=b.lower()
                for pattern in ['x.y.z-','mig','old','-dox','-exp','lcg','17.3.x']:
                    if pattern in b:
                        return True
                return False
                
            #dry run for formatting purposes, to figure out width of first column:
            maxbuildwidth=0
            for build,bdate,pkgfullpath,tag in pkgversions:
                if active_only and __is_not_main_build(build): continue
                maxbuildwidth=max(maxbuildwidth,len(str(build)))
    
            #actual output:
            for build,bdate,pkgfullpath,tag in pkgversions:
                if active_only and __is_not_main_build(build): continue
                if not bdate: bdate=''
                else: bdate=str(bdate)
                bstr='%s %s'%(build.ljust(maxbuildwidth),bdate.ljust(10))
                if not tag:
                    print "%s <missing>"%bstr
                else:
                    print "%s %s-%s"%(bstr,pkgfullpath,tag)

    if opt.diffstring:
        import PmbUtils.Misc
        d=opt.diffstring.split(':')
        assert len(d) in [2,3,4], "Badly formatted request"
        build1,date1,build2,date2=None,None,None,None
        if len(d)==2:
            #must be two fixed builds being diffed:
            build1,build2=d[0],d[1]
        elif len(d)==4:
            build1,date1,build2,date2=d[0],d[1],d[2],d[3]
        else:
            assert len(d)==3
            if PmbUtils.Misc.date_decode(d[1]):
                if PmbUtils.Misc.date_decode(d[2]):
                    #build1:date1:date2
                    build1,date1,date2=d[0],d[1],d[2]
                    build2=build1
                else:
                    #build1:date1:build2
                    build1,date1,build2=d[0],d[1],d[2]
            else:
                #build1:build2:date2
                build1,build2,date2=d[0],d[1],d[2]
        assert date1==None or PmbUtils.Misc.date_decode(date1), "Badly formatted request"
        assert date2==None or PmbUtils.Misc.date_decode(date2), "Badly formatted request"
        if date1: s1='%s:%s'%(build1,date1)
        else: s1=build1
        if date2: s2='%s:%s'%(build2,date2)
        else: s2=build2
        print "Finding tag differences between %s and %s:"%(s1,s2)
        tags1=PmbUtils.TagInfo.getTags(buildidx,build1,date1,include_baserel_tags=False)
        tags2=PmbUtils.TagInfo.getTags(buildidx,build2,date2,include_baserel_tags=False)
        bad=False
        if tags1==None:
            print "ERROR: Could not get tags for %s %s"%(build1,date1)
            bad=True
        if tags2==None:
            print "ERROR: Could not get tags for %s %s"%(build2,date2)
            bad=True
        if bad:
            sys.exit(1)
        if tags1['bases']==tags2['bases']:
            #no need to diff the tags in bases since they will all be the same
            if tags2['bases']:
                print "  Same base releases (ignoring): %s"%(' '.join(tags2['bases']))
            tags1=tags1['tags_direct']
            tags2=tags2['tags_direct']
        else:
            #once again, but get all the info about the tags in the base releases:
            tags1=PmbUtils.TagInfo.getTags(buildidx,build1,date1,include_baserel_tags=True)
            tags2=PmbUtils.TagInfo.getTags(buildidx,build2,date2,include_baserel_tags=True)
        bad=False
        if tags1==None:
            print "ERROR: Could not get tags for %s"%s1
            bad=True
        if tags2==None:
            print "ERROR: Could not get tags for %s"%s2
            bad=True
        if bad:
            sys.exit(1)
        (diffs,onlyin1,onlyin2)=PmbUtils.TagInfo.diffTags(tags1,tags2)
        if not diffs and not onlyin1 and not onlyin2:
            print "  No differences found in %i tags!"%(len(tags1.keys()))
        else:
            if onlyin1:
                print "  Packages only in %s:"%s1
                for p,t in onlyin1: print "    %s"%PmbUtils.TagInfo.formatPkgTag(p,t)
            if onlyin2:
                print "  Packages only in %s:"%s2
                for p,t in onlyin2: print "    %s"%PmbUtils.TagInfo.formatPkgTag(p,t)
            if diffs:
                print "  Packages with different tags:"
                for p,t1,t2 in diffs: print "    %s"%PmbUtils.TagInfo.formatPkgTag(p,t1,t2)
