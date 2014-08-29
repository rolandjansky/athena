#!/usr/bin/env python
# AtlCoolCheckFiles.py <dbname> <topleveltag>

import os,sys,commands,tempfile
from CoolConvUtilities.AtlCoolLib import pathResolver as pathResolver

class FileError:
    "Class to store details of errors in one file"
    def __init__(self,code,guid,count,lfn,pfn,folders):
        self.code=int(code)
        self.guid=str(guid)
        self.count=int(count)
        self.lfn=str(lfn)
        self.pfn=str(pfn)
        self.folders=folders

class checkFile:
    "Wrapper for AtlCoolCopy command which does POOL file checking"
    def __init__(self,dbname,taglist,runsince,rununtil,filename="",verbose=False,oracle=False,openfile=False,nocat=False,poolcats=[],magic=[],parfiles=[],passOpt=""):
        self.dbname=dbname
        self.taglist=taglist
        self.runsince=runsince
        self.rununtil=rununtil
        self.filename=filename
        self.verbose=verbose
        self.oracle=oracle
        self.openfile=openfile
        self.nocat=nocat
        self.poolcats=poolcats
        self.magictags=magic
        self.passOpt=passOpt
        self.parfiles=parfiles
        self.poollist=[]
        self.guidlist=[]
        # setup temporary output file
        self.tfilename=tempfile.mktemp()
        print "Analyse database %s runs from %i to %i" % (self.dbname,self.runsince,self.rununtil)
        for tag in self.taglist: print "Top-level tag %s" % tag
        if (self.openfile):
            print "Files will be physically opened to check GUID consistency"
        # setup list of DBs to check
        self.dblist=[]
        if (self.dbname.find('/')<0):
            # no explict connection string
            schemas=[]
            if (self.oracle):
                # current list of databases needed
                # only include those which are using POOL files
                # this duplicates information in AtlCoolSchema.py
                if self.dbname.find('OFLP')>=0:
                    schemas=['COOLONL_INDET','COOLONL_TRT','COOLONL_LAR',
                             'COOLONL_CALO','COOLONL_GLOBAL','COOLOFL_GLOBAL']
                elif self.dbname.find('CMCP')>=0:
                    schemas=['COOLONL_INDET','COOLONL_TRT','COOLONL_LAR',
                             'COOLONL_CALO','COOLONL_GLOBAL']
                elif self.dbname.find('COMP')>=0:
                    schemas=['COOLONL_INDET','COOLONL_TRT','COOLONL_LAR',
                             'COOLONL_CALO','COOLONL_GLOBAL','COOLOFL_GLOBAL']
                elif self.dbname.find('TMCP')>=0:
                    schemas=['COOLONL_LAR','COOLONL_CALO']
            else:
                # only a single connection is needed if using DBRelease
                schemas=['COOLONL_GLOBAL']
            for s in schemas:
                self.dblist+=['%s/%s' % (s,self.dbname)]
        else:
            # explicit connection string - use it
            self.dblist=[self.dbname]

    def execute(self):
        "Execute the checks, return non-zero code if some files are not OK"
        # construct poolcat options
        poolopts=""
        print "List of POOL file catalogues to be checked:"
        if (len(self.poolcats)==0 and not self.nocat):
            # default POOL file catalogues
            cats=[]
            if self.dbname.find('OFLP')>=0:
                cats=['oflcond']
            elif self.dbname.find('CMCP')>=0:
                cats=['cmccond']
            elif self.dbname.find('COMP')>=0:
                cats=['comcond','oflcond']
            elif self.dbname.find('TBDP')>=0:
                cats=['tbcond']
            elif self.dbname.find('TMCP')>=0:
                cats=['tbcond','oflcond']
            for cat in cats:
                fname=pathResolver('poolcond/PoolCat_%s.xml' % cat,
                                      retFile=False)
                if (fname is not None):
                    poolopts+=' -poolcat %s' % fname
                    print fname
                else:
                    print "Warning: Cannot find catalogue file for %s" % cat
        else:
            for i in self.poolcats:
                poolopts+=' -poolcat %s' % i
                print i

        sumret=0
        nerr=0
        for dbname in self.dblist:
            comm='AtlCoolCopy.exe %s X -nocopy -excludehead -checkrefs -listpfn -hitag -rs %i -ru %i -checkoutput %s%s' % (dbname,self.runsince,self.rununtil,self.tfilename,poolopts)
            for tag in self.taglist: comm+=' -tag %s' % tag
            if (self.oracle): comm+=' -readoracle'
            if (self.openfile):
                comm+=' -checkfiles'
            else:
                comm+=' -checkrefs'
            if (self.passOpt!=""): comm+=" %s" % self.passOpt
            for mtag in self.magictags:
                comm+=" -magic %s" % mtag
                print "Adding magic tag %s" % mtag
            for i in self.parfiles:
                comm+=" -parfile %s" % i
            print "Opening database",dbname
            (s,o)=commands.getstatusoutput(comm)
            if (self.verbose): print o
            if (s==111*256):
                print "Problems detected for files referenced by %s" % dbname
            elif (s!=0):
                print "Problems with DB access, AtlCoolCopy error %i (try --debug to see more)" % (s/256)
                sumret+=s
            # accumulate information from file
            try:
                infile=open(self.tfilename,"r")
                for line in infile.readlines():
                    tokens=line.split()
                    if int(tokens[0])>0: nerr+=1
                    guid=tokens[1]
                    if (guid not in self.guidlist):
                        self.poollist+=[FileError(int(tokens[0]),tokens[1],
                                                  int(tokens[2]),tokens[3],
                                                  tokens[4],tokens[5:])]
                        self.guidlist+=[guid]
                infile.close()
            except IOError:
                # catch file not existing gracefully
                pass
        # end of loop over database instances
        if (self.filename!=""):
            opfile=open(self.filename,"w")
        else:
            opfile=None
        print "Total of %i POOL files with problems" % nerr
        if (len(self.poollist)>0):
            print "ErrCode GUID Count LFN PFN Folders"
            for ifile in self.poollist:
                print '%2i %36s %4i %s %s' % (ifile.code,ifile.guid,ifile.count,ifile.lfn,ifile.pfn) ,
                for j in ifile.folders:
                    print j,' ',
                print
                if (opfile):
                    opfile.write('%2i %36s %4i %s %s ' % (ifile.code,ifile.guid,ifile.count,ifile.lfn,ifile.pfn))
                    for j in ifile.folders:
                        opfile.write(j+' ')
                    opfile.write('\n')
        else:
            print "All files resolved successfully"
        if (opfile): opfile.close()
        # remove temporary file if needed, don't assume it is there
        try:
            os.remove(self.tfilename)
        except OSError:
            pass
        return sumret

    def makeCD(self,filename,ignoremissing=False):
        "Make conditions release with the set of files which were found"
        # first check that all files were found, and if any are in castor
        nbad=0
        prestagelist=[]
        for ifile in self.poollist:
            if (ifile.code!=0 or ifile.pfn=='' or ifile.pfn=='noPFN'):
                nbad+=1
            idx=ifile.pfn.find('/castor/')
            if (idx>=0):
                prestagelist+=[' -M %s' % ifile.pfn[idx:]]
        if (nbad>0):
            print "WARNING: %i files are bad/missing" % nbad
            if (ignoremissing):
                print "Continuing build with missing files"
            else:
                print "Cannot build conditions release - ABORT"
                return 1
        # issue castor pre-stage commands if needed
        if len(prestagelist)>0:
            print "Pre-staging %i files from castor" % len(prestagelist)
            # issue command in groups of 10
            for i in range(0,len(prestagelist),10):
                comm='LD_LIBRARY_PATH="";stager_get'
                for j in range(i,min(i+10,len(prestagelist))):
                    comm+=prestagelist[j]
                print "Staging files %i %s: " % (i,comm)
                os.system(comm)
                
        print "Building conditions release with %i files to output file %s" % (len(self.poollist),filename)
        # create a temporary area to link to the files and zip them
        packdir=tempfile.mktemp()
        os.mkdir(packdir)
        reldir=packdir+'/CDRelease'
        os.mkdir(reldir)
        pooldir=reldir+'/poolcond'
        os.mkdir(pooldir)
        xmldir=reldir+'/XMLConfig'
        os.mkdir(xmldir)
        # determine catalogue name and any dummies required
        dummies=[]
        if (self.dbname.find('OFLP')>=0):
            cat='oflcond'
        elif (self.dbname.find('COMP')>=0):
            cat='comcond'
            dummies+=['oflcond','comcond_castor']
        elif (self.dbname.find('CMCP')>=0):
            cat='cmccond'
        else:
            cat='nocond'
            print "Unknown database instance %s - cannot derive catalogue name" % self.dbname
        # generate any dummy catalogues that are required
        for dummy in dummies:
            print "Making dummy catalogue for %s" % dummy
            dcat=PoolCat('%s/PoolCat_%s.xml' % (pooldir,dummy))
            dcat.close()
        catname='%s/PoolCat_%s.xml' % (pooldir,cat)
        print "POOL file catalogue will be generated at %s" % catname
        catalog=PoolCat(catname)
        print "Using temporary directory %s" % packdir
        # generate copies of current dblookup.xml and authentication.xml
        os.system('cp $CORAL_AUTH_PATH/authentication.xml %s' % xmldir)
        os.system('cp $CORAL_DBLOOKUP_PATH/dblookup.xml %s' % xmldir)
        nbad=0
        for ifile in self.poollist:
            if (ignoremissing and (ifile.pfn=='noPFN' or ifile.code!=0)):
                continue
            idx=ifile.pfn.rfind('/')
            if (idx>0):
                leafname=ifile.pfn[idx+1:]
            else:
                print "Cannot extract leafname from file %s" % ifile.pfn
                leafname='BADFILE'
            idx=ifile.pfn.find('/castor/')
            if (idx>=0):
                # file is from castor - have to rfcp to temp area
                print "Copying from castor: %s" % ifile.pfn[idx:]
                rc=os.system('rfcp %s %s/%s' % (ifile.pfn[idx:],pooldir,leafname))
            else:
              os.symlink(ifile.pfn,'%s/%s' % (pooldir,leafname))
              rc=0
            if (rc!=0):
                print "Problems linking/copying %s" % ifile.pfn
                nbad+=1
            catalog.makeEntry(ifile.guid,ifile.lfn,leafname)
        catalog.close()
        if (nbad>0):
            print "ERRORs detected in linking/copying %i files - ABORT" % nbad
            return 2

        # add the setup file setup.py
        setupfile=open('%s/setup.py' % reldir,'w')
        setupfile.write("""
import os, fileinput

class Setup(object):
    def __init__(self, dbdir = None):
        if dbdir is None:
            self.dbdir = os.getcwd()
        else:
            self.dbdir = dbdir
        print "Setting up CDRelease:"
        self.custom()
        self.env()

    def custom(self):
        fns = ['PoolCat_oflcond.xml', 'PoolCat_tbcond.xml',
               'PoolCat_cmccond.xml', 'PoolCat_comcond.xml']
        fps=[]
        for fn in fns:
            f=os.path.join(self.dbdir,'poolcond',fn)
            if os.path.exists(f):
                fps+=[f]
        print "  Editing pool catalogs ... "
        print fps
        for line in fileinput.input(fps, inplace=1):
            print line.replace('--GENERATED--',
                               os.path.join(self.dbdir, 'poolcond') + os.sep),

    def env(self):
        datapath = os.environ.get('DATAPATH')
        if datapath:
            datapath = datapath.split(os.pathsep)
        else:
            datapath = []
        datapath = [d for d in datapath if 'DBRelease' not in d.split(os.sep)]
        datapath.insert(0, self.dbdir)
        coralpath=os.path.join(self.dbdir,'XMLConfig')
            
        print "  Setting up environment ..."
        self.dbenv = { 'DATAPATH'            : os.pathsep.join(datapath) ,
                       'CDRELEASE'           : os.path.basename(self.dbdir),
                       'CORAL_DBLOOKUP_PATH' : os.path.join(self.dbdir, 'XMLConfig'),
                       'CORAL_AUTH_PATH'     : os.path.join(self.dbdir,'XMLConfig')}
        os.environ.update(self.dbenv)

    def __str__(self):
        return '\\n'.join(['%s=%s' % i for i in self.dbenv.iteritems()])

if __name__ != 'setup':
    print Setup()

        """)
        setupfile.close()
        
        # create the tar file
        print "Creating tar file"
        rc=os.system('cd %s;tar cvf %s.tar CDRelease -h' % (packdir,filename))
        if (rc==0):
            rc=os.system('gzip %s.tar' % filename)
            if (rc!=0):
                print "Non-zero return code %i from gzipping" % rc
                return 3
        else:
            print "Non-zero return code %i from tarfile creation" % rc
            return 2
        # remove the temporary file
        os.system('rm -rf %s' % packdir)
        print "All done"
        return 0

class PoolCat:
    "Class to output POOL file catalogue"
    def __init__(self,catname):
        "Open the catalogue file and write headers"
        self.catfile=open(catname,'w')
        self.catfile.write("""<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<!-- Edited By POOL -->
<!DOCTYPE POOLFILECATALOG SYSTEM "InMemory">
<POOLFILECATALOG>\n""")

    def makeEntry(self,guid,lfn,pfnleaf):
        "Make one entry in the file catalogue"
        self.catfile.write("""
  <File ID="%s">
    <physical>
      <pfn filetype="ROOT_All" name="--GENERATED--%s"/>
    </physical>
    <logical>
      <lfn name="%s"/>
    </logical>
  </File>
""" % (guid,pfnleaf,lfn))

    def close(self):
        "Close the file, writing trailer"
        self.catfile.write('</POOLFILECATALOG>\n')
        self.catfile.close()
    
        

if __name__=='__main__':
    # command line driver
    import getopt
    def usage():
        print"usage: AtlCoolCheckFiles.py {<options>} dbname top-level-tag {top-level-tag}"
        print "  dbname is instance e.g. OFLP200 or full COOL connect-string"
        print "  top-level-tag(s) is global tag in the database instance"
        print "Options are:"
        print "--help :      Display this help"
        print "--debug :     Dump full output from AtlCoolCopy command"
        print "--rs=<run>    First run to consider"
        print "--ru=<run>    Last run to consider (inclusive)"
        print "--r=<run>     Consider single run"
        print "--geo=<geometry tag>: Only include magic tags for given geometry"
        print "--readoracle: Read data from Oracle rather than DBrelease"
        print "--open: Actually open POOL files to check GUID"
        print "--output=<file>: Output POOL catalogue data on <file>"
        print "--poolall: List all POOL files referenced, not only those with errors"
        print "--poolcat=<cat>: Use specific catalogue rather than defaults"
        print "--nocat :     Do not attach any POOL file catalogues"
        print "--makecd <file> : Make CDRelease POOL file tarball with given name"
        print "--ignoremissing : Ignore missing files when making CDRelease"
        print "--passopt=\"<options>\": Pass additional options to AtlCoolCopy"
        print "--parfile <file> : Pass file as additional options to AtlCoolCopy"
    try:
        longopts=['r=','rs=','ru=','readoracle','open','nocat','output=',
                  'poolcat=','debug','passopt=','parfile=','makecd=','geo=',
                  'poolall','ignoremissing','help']
        opts,args=getopt.getopt(sys.argv[1:],'',longopts)
    except getopt.GetoptError,e:
        print e
        usage()
        sys.exit(-1)
    if (len(args)<2):
        usage()
        sys.exit(-1)
    dbname=args[0]
    tags=args[1:]
    # set defaults
    runsince=0
    rununtil=(2 << 31)-1
    filename=""
    oracle=False
    openfile=False
    nocat=False
    verbose=False
    parfiles=[]
    poolcats=[]
    geo=[]
    makecd=""
    passopt=""
    ignoremissing=False
    # process options
    for o,a in opts:
        if (o=='--rs'):
            runsince=int(a)
        if (o=='--ru'):
            rununtil=int(a)
        if (o=='--r'):
            runsince=int(a)
            rununtil=runsince
        if (o=='--open'):
            openfile=True
        if (o=='--nocat'):
            nocat=True
        if (o=='--output'):
            filename=str(a)
        if (o=='--readoracle'):
            oracle=True
        if (o=='--poolcat'):
            poolcats+=[str(a)]
        if (o=='--parfile'):
            parfiles+=[str(a)]
        if (o=='--debug'):
            verbose=True
        if (o=='--passopt'):
            passopt+=str(a)+" "
        if (o=='--poolall'):
            passopt+='-poolall '
        if (o=='--geo'):
            geo+=[str(a)]
        if (o=='--makecd'):
            makecd=str(a)
            passopt+='-poolall -listpfn '
        if (o=='--ignoremissing'):
            ignoremissing=True
        if (o=='--help'):
            usage()
            sys.exit()
    # consider all geometry magic tags if none given
    if len(geo)==0: geo=['ATLAS-']
    
    myCheck=checkFile(dbname,tags,runsince,rununtil,filename,
                      verbose,oracle,openfile,nocat,poolcats,geo,parfiles,passopt)
    rc=myCheck.execute()
    if (makecd!=""): myCheck.makeCD(makecd,ignoremissing)
    
