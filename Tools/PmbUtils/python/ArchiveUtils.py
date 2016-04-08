#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import glob
import time
import datetime
import os
import PmbUtils.ValuePerDomainCalcUtils

archivebase='/afs/cern.ch/atlas/project/pmb/archive/rtt'

def set_archive_base(d):
    archivebase=d

def getActiveBuilds(packages,daysback=None,firstdate=None,lastdate=None):
    builds=[]
    for daydiff in range(-daysback,1):
        dt=datetime.datetime.now().date()+datetime.timedelta(days=daydiff)
        if firstdate!=None and dt<firstdate: continue
        if lastdate!=None and dt>lastdate: continue
        for p in packages:
            archive_dt=os.path.join(archivebase,str(dt.day).rjust(2,'0'),str(dt.month).rjust(2,'0'),str(dt.year))
            for d in glob.glob(os.path.join(archive_dt,'*/*/*',p)):
                #print d
                b='/'.join(d.split('/')[archive_dt.count('/')+1:-1])
                if not b in builds:
                    builds+=[b]
    return builds

def getJobDirAndLog(builddir,package,jobname,jobgroup):
    d=os.path.join(builddir,package,jobgroup,jobname)
    if not os.path.exists(d): return (None,None)
    l=glob.glob(os.path.join(d,jobname+'*_log.gz'))
    if not len(l)==1 or not os.path.exists(l[0]): return (None,None)
    return (d,l[0])

def getDates(cfg):
    offset=archivebase.count('/')+1
    dates2dir={}
    for d in glob.glob(os.path.join(archivebase,'*/*/*',cfg)):
        (day,month,year)=d.split('/')[offset:offset+3]
        dt=datetime.date(day=int(day),month=int(month),year=int(year))
        dates2dir[dt]=d
    return dates2dir

def collectInfo(cfg,useranalysefct,username,userversion,firstdate=None,lastdate=None):
    import pickle
    import gzip
    dates2data={}
    firstcachedate=datetime.datetime.now().date()+datetime.timedelta(days=-2)
    dates2builddir=getDates(cfg)
    dates=dates2builddir.keys()
    dates.sort()
    for dt in dates:
        builddir=dates2builddir[dt]#calling it many times... fixme
        if firstdate!=None and dt<firstdate: continue
        if lastdate!=None and dt>lastdate: continue
        cancache=(userversion!=None and dt<=firstcachedate)
        cachefile=os.path.join(builddir,'.cache_%s'%username)
        data=None
        needanalysis=True
        if cancache and os.path.exists(cachefile):
            #open and see if the version inside the same as userversion. If so, load the cached data.
            f=gzip.open(cachefile,'r')
            if f:
                assert userversion!=None
                try:
                    v=pickle.load(f)
                except:
                    print "WARNING: Problems loading cachefile",cachefile
                    v=None
                    pass
                if v==userversion:
                    data=pickle.load(f)
                    needanalysis=False
                f.close()
        if not needanalysis:
            if False: print "Taking from cache:",builddir
        else:
            #print "Loading...:",builddir
            data=useranalysefct(builddir,dt)
            #put inside the cachefile along with userversion
            if cancache:
                f=gzip.open(cachefile,'w')
                if f:
                    pickle.dump(userversion,f)
                    pickle.dump(data,f)
                    f.close()
        if data!=None:
            dates2data[dt]=data
    return dates2data

def archivePlots2PlottableResults(cfg,usersetupfct,useranalysefct,firstdate=None,lastdate=None,firstdate_zoom=None):

    otherdivfact=100
    other='other/%i'%otherdivfact
    (username,userversion,usertests)=usersetupfct()
    dates2data=collectInfo(cfg,useranalysefct,username,userversion,firstdate,lastdate)
    dates=dates2data.keys()
    dates.sort()

    colmap={other:(0.5,0.5,0.5)}

    test2setup={}
    test2col2vals={}
    for test,title,unit,cut in usertests:
        if not test in test2setup:
            test2setup[test]={'cols':[],'title':title,'unit':unit}
            test2col2vals[test]={}
        showncolumns=[]
        if cut!=None:
            for d,data in dates2data.items():
                if not test in data.keys(): continue
                for col,val in data[test].items():
                    isnormfluct=('normfluct' in col)
                    isunaccounted=('unaccounted' in col)
                    istotal=col.startswith('total')
                    if (abs(val)>cut or isnormfluct or istotal or isunaccounted) and not col in showncolumns:
                        showncolumns+=[col]
                    if isnormfluct and not col in colmap.keys():
                        colmap[col]=(0.9,0.9,0.9)
                    if istotal and not col in colmap.keys():
                        colmap[col]=(0.0,0.0,0.0)
                    if isunaccounted and not col in colmap.keys():
                        colmap[col]=(0.4,0.4,0.4)
        for d in dates:
            data=dates2data[d]
            if not test in data.keys(): continue
            otherval=0.0
            for col,val in data[test].items():
                if cut!=None and not col in showncolumns:
                    otherval+=val
                    continue
                if not col in test2setup[test]['cols']:
                    test2setup[test]['cols']+=[col]
                if not col in test2col2vals[test].keys():
                    test2col2vals[test][col]={}
                if val==None: continue
                test2col2vals[test][col][d]=val
            if cut!=None:
                if not other in test2setup[test]['cols']: test2setup[test]['cols']+=[other]
                if not other in test2col2vals[test].keys(): test2col2vals[test][other]={}
                test2col2vals[test][other][d]=otherval

    prs=[]
    for test,title,unit,cut in usertests:
        setup=test2setup[test]
        #for test,setup in test2setup.items():
        pr=PmbUtils.ValuePerDomainCalcUtils.PlottableResult(colnames=setup['cols'],colname2colour=colmap,title=setup['title'],unit=setup['unit'])
        pr_short=None
        if firstdate_zoom:
            pr_short=PmbUtils.ValuePerDomainCalcUtils.PlottableResult(colnames=setup['cols'],colname2colour=colmap,title=setup['title'],unit=setup['unit'])

        for col in setup['cols']:
            for dt,val in test2col2vals[test][col].items():
                if col==other:
                    val=val/otherdivfact
                pr.addPoint(col,val,PmbUtils.ValuePerDomainCalcUtils.TimeLabel(date=dt))
                if pr_short and dt>=firstdate_zoom:
                    pr_short.addPoint(col,val,PmbUtils.ValuePerDomainCalcUtils.TimeLabel(date=dt))
        #if other in setup['cols']:
        #    pr.setColumnOffset(other,'auto')
        if pr_short:
            prs+=[(test+'_long',pr)]
            prs+=[(test,pr_short)]
        else:
            prs+=[(test,pr)]
    return prs

def runcmd(cmd):
    import subprocess
    #print "Running",cmd
    output=None
    try:
        p = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE)
        output = p.communicate()[0]
    except:
        output=None
        raise
    #print "Returned",output
    return output

def createThumbnail(imagefile):
    import shutil
    thfile=imagefile.replace('.png','_th.jpg')
    shutil.copy(imagefile,thfile)
    runcmd('/usr/bin/mogrify -geometry 400x99999 -quality 95 %s'%thfile)
    return thfile

def archivePlots(cfg,usersetupfct,useranalysefct,toStdOut=True,doTables=True,doImages=True,htmlFile='html.snippet',
                 outdir=None,firstdate=None,lastdate=None,firstdate_zoom=None,usetmpdir=False):
    if outdir==None:
        outdir=os.getcwd()
    if usetmpdir:
        import random
        import string
        randstr=''.join(random.choice(string.letters) for i in xrange(10))
        outdir_final=outdir
        outdir+='__tmp__'+randstr
    if not os.path.exists(outdir):
        #print "Creating %s"%outdir
        os.mkdir(outdir)
    assert os.path.exists(outdir)
    prs=archivePlots2PlottableResults(cfg,usersetupfct,useranalysefct,firstdate,lastdate,firstdate_zoom)
    updatetime=datetime.datetime.utcnow().isoformat(' ')+' +0000'
    imgsandtables_forhtml=[]
    nplotsproblems=0
    nplotstot=0
    for test,pr in prs:
        testsafe=test.replace('/','__')
        tablefile=None
        imagefile=None
        if doTables:
            tablefile=os.path.join(outdir,'table_%s.txt'%testsafe)
        if doTables or toStdOut:
            PmbUtils.ValuePerDomainCalcUtils.JobListPrinter().printTable(pr,toStdOut=toStdOut,outFile=tablefile)
        if doImages:
            imagefile=os.path.join(outdir,'histplot_%s.png'%testsafe)
            imagefile2=os.path.join(outdir,'histplot_%s.eps'%testsafe)
            ok=PmbUtils.ValuePerDomainCalcUtils.JobListPlotter().produceTimeDevelopmentPlot(pr,imagefile,quietwarn=True,secondoutfile=imagefile2)
            nplotstot+=1
            if not ok:
                nplotsproblems+=1
        if not test.endswith('_long') and (tablefile or imagefile):
            imgsandtables_forhtml+=[(tablefile,imagefile,pr.getTitle())]
    if nplotsproblems:
        dontwarn=['15.6.X.Y','16.7.X.Y','16.2.X','16.2.X.Y','17.1.X']#older or less interesting branches (dont warn on those)
        if not cfg.split('/')[0] in dontwarn:
            print "WARNING: Problems producing %i/%i plots for %s"%(nplotsproblems,nplotstot,str(cfg))

    if htmlFile:
        html=[]
        html+=['<p/><span style="font-size:70%%"><em>Page automatically updated LASTUPDATE[%s]</em></span><br/>'%updatetime]
        html+=['<span style="font-size:70%"><em>Tip: The images are available in eps versions as well as png (just substitute .png to .eps in the links).</em></span><p/>']
        imgsandtext=[]
        for tablefile,imagefile,title in imgsandtables_forhtml:
            i=None
            t=None
            captions=[]
            if imagefile:
                ilong=imagefile.replace('.png','_long.png')
                if os.path.exists(ilong):
                    captions+=['<a href="%s">long-term</a>'%os.path.basename(ilong)]
                imagefile_base=os.path.basename(imagefile)
                thfile=imagefile
                if os.path.exists(imagefile):
                    thfile=os.path.basename(createThumbnail(imagefile))
                    i='<a href="%s"><img alt="%s" width="400" src="%s"/></a>'%(imagefile_base,title,thfile)
            if tablefile:
                tlong=tablefile.replace('.txt','_long.txt')
                captions+=['<a href="%s">table</a>'%os.path.basename(tablefile)]
                if os.path.exists(tlong):
                    captions+=['<a href="%s">table-long-term</a>'%os.path.basename(tlong)]
            if captions:
                title+='<br/> (%s)'%(', '.join(captions))
                #t='%s (<a href="%s">table</a>)'%(pr.getTitle(),tablefile)
            if i or title: imgsandtext+=[(i,title)]
        if imgsandtext:
            n=len(imgsandtext)
            ntables=n/2+n%2
            for itable in range(ntables):
                index=itable*2
                (i1,t1)=imgsandtext[index]
                (i2,t2)=imgsandtext[index+1] if index+1<n else (None,None)
                if not i1: i1='Not Available'
                if not t1: t1=''
                if not i2: i2='Not Available'
                if not t2: t2=''
                html+=['<table style="font-size: 90%; font-style: italic; color: rgb(100, 100, 100);">']
                html+=['  <tbody>']
                html+=['    <tr><th>%s</th><th>%s</th></tr>'%(i1,i2)]
                html+=['    <tr><th>%s</th><th>%s</th></tr>'%(t1,t2)]
                html+=['  </tbody>']
                html+=['</table>']
                #testhtml+=['<p/>']
        if html:
            file = open(os.path.join(outdir,htmlFile), 'w')#fixme: should just write a snippet, not the index.html (or what?)
            for outline in html:
                file.write(outline+'\n')
            file.close()
    if usetmpdir:
        import shutil
        if os.path.exists(outdir_final):
            #I don't really want nasty surprises from shutils.rmtree in a cron job...:
            for f in glob.glob(outdir_final+'/histplot*.png'): os.remove(f)
            for f in glob.glob(outdir_final+'/table*.txt'): os.remove(f)
            for f in glob.glob(outdir_final+'/html*snippet'): os.remove(f)
            os.rmdir(outdir_final)

        if os.path.exists(outdir_final):
            print "ERROR: Problems getting rid of old directory",outdir_final
        else:
            shutil.move(outdir,outdir_final)
            print "Finished ",outdir_final

def addDomainSubtractionValues(domaincalc,jobvals):
    tests=jobvals.keys()
    jobs=domaincalc.defineJobs()
    for test in tests:
        d={}
        for job in jobs:
            val=None
            if job in jobvals[test].keys():
                val=jobvals[test][job]
            d[job]=PmbUtils.ValuePerDomainCalcUtils.Measurement(val)
        domvals={}
        for dom,measval in domaincalc.calcDomains(d).items():
            val=measval.getValue()
            if val!=None:
                domvals[dom]=val
        jobvals[test+'_dom']=domvals
