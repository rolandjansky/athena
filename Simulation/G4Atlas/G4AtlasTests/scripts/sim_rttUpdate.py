#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
check/update references 

for SimCoreTest

update script:
 - check file exists before doing something (dcube)
 - restrict to used branches

fix DCube for devval

add sqlite 
 t_logs ( rel, cmt, branch, rundate, update date&time , comment, jobsalias_id )
--
 t_branches (branch,ref,project)   [ 17.X.0-VAL , 17.X.0 ]
 t_alias (alias, branch)       [ 'devval', '17.X.0-VAL']
 t_tests (testname,pathpiece,project)  ['SimCoreTests','Simulation/Tests','simulation']
 t_jobs (testname, job, file, type)  ['SimCoreTests','AtlasG4_muons'
 t_joblsit (alias, job)  -- except 'all'  ['dcube','AtlasG4_%']
 t_user (admins) ['jchapman','aschaeli','z']
"""

from exceptions import Exception

class SimRttDBError(Exception):
    pass

class SimRttUpdateError(Exception):
    pass


try:
    import sqlite3
except:
    print 'ERROR:'
    print 'please use Python 2.5 or above:'
    print '> source ~aschaeli/public/setup_python26.sh'
    print ' '
    raise SimRttUpdateError()


class SimRttDataBase:
    def __init__(self,fname):
        self.fname=fname
        self.cnx=None
        self.cur=None


    def openConnection(self):
        self.cnx = sqlite3.connect(self.fname)
        self.cur = self.cnx.cursor()

    def _execute(self,cmd,args=()):
        print 'SQL:',cmd,args
        self.cur.execute(cmd,args)
    
    def _createTable(self,tname,*args):
        #print 'creating table',tname
        vals=','.join(('%s text ',)*len(args)) 
        cmd=('create table %s ('+vals+')')% ( (tname,)+args )
        self._execute(cmd)

    def _fillTable(self,tname,*args):
        #print 'updating table',tname
        vals=','.join(('?',)*len(args))
        cmd="insert into %s values (%s)"%(tname,vals)
        self._execute(cmd,args)

        #insert into t_branches values ('17.2.X-VAL','17.2.X','simulation');
        #Error: column branch is not unique

    def _hasKey(self,tname,column,value):
        cmd='select count(*) from %s where %s=?'%(tname,column)
        self._execute(cmd,(value,))
        res=sel.cur.fetchone()
        print res
        print res[0]>0
        return res[0]

    def _updateTable(self,tname,keys,*args):
        cmd='select * from %s limit 1'%tname
        self._execute(cmd)
        header= [ s[0] for s in self.cur.description ]
        res=self.cur.fetchall()

        values=[ k+'=?' for k in header if k not in keys ]
        key_values=tuple([ a for k,a in zip(header,args) if k in keys])
        set_values=tuple([ a for k,a in zip(header,args) if k not in keys])
        vals=','.join(values)
        cmd="update %s set %s where %s=?"%(tname,vals,keys[0])
        for k in keys[1:]:
            cmd+=' and %s=?'%k
        self._execute(cmd, set_values+key_values)
        self.cnx.commit()

    def _dropTable(self,tname):
        cmd='drop table %s'%tname
        try:
            self._execute(cmd)
        except:
            print 'SQL Error:',cmd
    
    def create(self):
        for t in ['t_logs','t_branches','t_alias','t_tests','t_jobs','t_joblist','t_user']:
            self._dropTable(t)
        self._createTable('t_logs','branch','rel','cmt','res_date','up_date')
        #self._createTable('t_branches','branch','ref','project')
        #'alter table t_branches rename to t_branches_old;'
        self._execute('create table t_branches (branch text, ref text, project text, primary key (branch,project) ) ;')
        #'insert into t_branches select * from t_branches_old;'
        self._createTable('t_alias','alias','value')
        self._createTable('t_tests','testname','pathpiece','project')
        self._createTable('t_jobs','testname','job','file','type')
        self._createTable('t_joblist','jalias','job')
        self._createTable('t_user','login','rights')
        self.cnx.commit()
        
    def initialFill(self):
        # dev, devval ??!!
        sim='simulation'
        for br in ['17.X.0-VAL','17.X.0']:
            self._fillTable('t_branches', br, '17.X.0',sim)
        for br in ['17.1.X-VAL','17.1.X','17.1.X.Y-VAL','17.1.X.Y','17.2.X-VAL','17.2.X']:
            self._fillTable('t_branches', br, '17.1.X',sim)
        for br in ['16.6.X.Y-VAL','16.6.X.Y','16.6.X-VAL','16.6.X']:
            self._fillTable('t_branches', br, '16.6.X',sim)
  

        self.djobs={ 'AtlasG4_electrons' : ['truth.root'],
                     'AtlasG4_muons' : ['truth.root'],
                     'WriteCalHitsTest' : ['truth.root'],
                     'AtlasG4_pions' : ['truth.root'] }

        self.jobs={'AtlasGeom19Sim' : ['test.HITS.pool.root'],
                   'CavernBg_EVNT2TR' : ['cavbg.EVNT.pool.root'],
                   'CavernBg_TR2HITS' : ['cavbg.HITS.pool.root'],
                   'CosmicSim' : ['testCosmics.HITS.pool.root','testCosmics.TR.pool.root'],
                   'CosmicSimTR' : ['testCosmics.HITS.pool.root'],
                   'HeavyIonSim': ['test.HITS.pool.root'],
                   'TTbarSim' : ['test.HITS.pool.root'],
                   'FtfpBertTest': ['test.HITS.pool.root'],
                   'LucidSimTest': ['testLucid.HITS.pool.root'],
                   'NeutronCutTest': ['test.HITS.pool.root'],
                   'QgsBicTest': ['test.HITS.pool.root'],
                   'QgspBertChipsTest': ['test.HITS.pool.root'],
                   'SimCheckDBRel': ['test.HITS.pool.root'],
                   'ALFASimTest' : ['testALFA.HITS.pool.root'],
                   'ZDCSimTest' : ['testZDC.HITS.pool.root']}
        
        self.jobs0={'FrozenShowerFCalOnly': ['test.HITS.pool.root']}
        self.jobs1={'FrozenShowerTest1': ['test.HITS.pool.root'],
                    'FrozenShowerTest2': ['test.HITS.pool.root']}
        self.alias={} #{'FrozenShowerFCalOnly':'FrozenShowerTest1'} 

        tn='SimCoreTests'
        self._fillTable('t_tests',tn,'Simulation/Tests','simulation')
        tn1='G4AtlasTests'
        self._fillTable('t_tests',tn1,'Simulation/G4Atlas','simulation')
        for dj,files in self.djobs.iteritems():
            self._fillTable('t_joblist','dcube_jobs',dj)
            for fn in files:
                self._fillTable('t_jobs',tn,dj,fn,'dcube')
                self._fillTable('t_jobs',tn1,dj,fn,'dcube')
                                
        for j,files in self.jobs.iteritems():
            self._fillTable('t_joblist','diffpool_jobs',j)
            for fn in files:
                self._fillTable('t_jobs',tn,j,fn,'diffpool')
                self._fillTable('t_jobs',tn1,j,fn,'diffpool')

        for j,files in self.jobs0.iteritems():
            self._fillTable('t_joblist','diffpool_jobs',j)
            for fn in files:
                self._fillTable('t_jobs',tn,j,fn,'diffpool')

        for j,files in self.jobs1.iteritems():
            self._fillTable('t_joblist','diffpool_jobs',j)
            for fn in files:
                self._fillTable('t_jobs',tn1,j,fn,'diffpool')

        tn2='SimExoticsTests'
        self._fillTable('t_tests',tn2,'Simulation/Tests','simulation')

        tn3='SimPerformanceTests'
        self._fillTable('t_tests',tn3,'Simulation/Tests','simulation')


        self.users=['aschaeli', 'jchapman', 'fmgaray']
        for u in self.users:
            self._fillTable('t_user',u,'a')

        self.cnx.commit()

    def _dumpTable(self,t):
        #t = (symbol,)
        #cur.execute('select * from stocks where symbol=?', t)
        self._execute('select * from %s'%t)
        header= [ s[0] for s in self.cur.description ]
        format= ' '.join([ '%25s' for s in self.cur.description])
        print format%tuple(header)
        for res in self.cur.fetchall():
            print format%res

    def getUsers(self,right='a'):
        cmd='select login from t_user where rights=?'
        self._execute(cmd,(right,))
        return [res[0] for res in self.cur.fetchall()]

    def findPackage(self,package):
        cmd='select pathpiece,project from t_tests where testname=?'
        self._execute(cmd,(package,))
        return self.cur.fetchone()

    def findReference(self,branch,project):
        cmd='select ref from t_branches where branch=? and project=?'
        if branch=='devval' :
            branch='18.X.0-VAL'
        if branch=='dev':
            branch='18.X.0'
        self._execute(cmd,(branch,project))
        res=self.cur.fetchone()
        if res is None:
            print 'ERROR',branch,project,'not found in t_branches'
            raise SimRttDBError
        
        #print res
        return res[0]

    def findJobs(self,testname,jobtype):
        cmd='select job,file from t_jobs where testname=? and type=?'
        self._execute(cmd,(testname,jobtype))
        res=self.cur.fetchall()
        jobs={}
        for r in res:
            key=r[0]
            fname=r[1]
            if jobs.has_key(key):
                jobs[key].append(fname)
            else:
                jobs[key]=[fname]

        return jobs


    def dump(self):
        for t in ['t_user','t_branches','t_tests','t_jobs','t_alias','t_joblist']:
            self._dumpTable(t)


    def addUser(self,username='aschaeli',right='a'):
        self._fillTable('t_user',username,right)
        self.cnx.commit()
        
    def addBranch(self,branch='17.X.0-VAL',refid='17.X.0'):
        self._fillTable('t_branches',branch,refid)
        self.cnx.commit()

    def addTest(self,testname='SimCoreTests',path='Simulation/Tests'):
        self._fillTable('t_tests',testname,path)
        self.cnx.commit()

    def addJob(self,testname='SimCoreTests',jobname='AtlasG4_muons',
               files=['truth.root'],typeid='dcube'):
        for fn in files:
            self._fillTable('t_jobs',testname,jobname,fn,typeid)
        self.cnx.commit()

    def defineJobAlias(self,alias='dcube_jobs', 
                       joblist=['AtlasG4_muons','AtlasG4_electrons','AtlasG4_pions']):
        for job in joblist:
            self._fillTable('t_joblist',alias,job)
        self.cnx.commit()



    # We can also close the cursor if we are done with it
    #cur.close()
    


import os
import datetime as dt
from argparse import ArgumentParser
import pwd


class MyArgumentParser(ArgumentParser):
    def __init__(self,admin=True,user='aschaeli'):
        usage= "updates reference files for the Core Simulation Runtime Tests"

        epilog="""
        Example:
        %prog -n  --branch=devval --rel=rel_3 --package=G4AtlasTests
        """

        """
        update --branch --rel --cmt --jobs --today --rttpath (--suffix)

        dumpDB
        dumpUser [--right]
        dumpTests [--project]
        dumpBranches [--project]
        addUser --user --right
        addBranch --branch --ref --project
        addTest --package --testpath --project
        addJobs --package --jobs  --jobfiles --testtype
        defAlias --jobalias --jobs
        """

        ArgumentParser.__init__(self,description=usage,epilog=epilog)

        self.add_argument('-n', '--dryrun', action='store_const',
                          const=True, dest='dryrun',  default=False,
                          help='dry run  - do nothing')
        group = self.add_mutually_exclusive_group(required=not admin)
        group.add_argument('--branch', action='store', dest='branch',  default='devval', 
                            help='release to use')

        group.add_argument('--logs', '-l', action='store_true', dest='logs',
                           help='print logs but do nothing')


        self.add_argument('--package', action='store', dest='package', default='G4AtlasTests',
                            help='package containing the tests (default G4AtlasTests)')

        self.add_argument('--rel', action='store', dest='rel',  default='rel_X',
                            help='rel identifier (default todays rel_X)')

        self.add_argument('--cmt', action='store', dest='cmt', default='x86_64-slc5-gcc43-opt',
                            help='cmt identifier (default:x86_64-slc5-gcc43-opt)')

        self.add_argument('--jobs', action='store', dest='jobs', default='',
                            help='job names, e.g. CosmicSim,WriteCalHitsTest')

        self.add_argument('--today', action='store', dest='today', default='AUTO',
                            help='store results in specified subdir ')

        self.add_argument('--rttpath', action='store', dest='rttpath', default='default',
                            help='use path for looking for root files')

        self.add_argument('--suffix', action='store', dest='sbranch', default='',
                            help='file suffix to distingish ref files for different releases')

        # admin arguments
        # TODO: only add if admin
        if admin:
            group1=self.add_argument_group('admin arguments', 'commands for changing the configuration')

            group1.add_argument('--dumpDB', action='store_true', dest='dumpDB',
                                help='print all database entries')

            group1.add_argument('--dumpUsers',action='store_true', dest='dumpUsers',  
                                help='print all users authorised to use admin commands')

            group1.add_argument('--dumpTests',action='store_true', dest='dumpTests',
                                help='print all Tests')

            group1.add_argument('--dumpBranches',action='store_true', dest='dumpBranches',
                                help='print all Branches')

            group1.add_argument('--addBranch', action='store_true', dest='addBranch', 
                                help='add new branch to configuration file')

            group1.add_argument('--refpath', action='store', dest='ref', default='17.X.0',
                                help='assign refpath to given branch')

            group1.add_argument('--project', action='store', dest='project', default='simulation',
                                help='assign project e.g. digitization (default: simulation)')
            
            group1.add_argument('--addTest', action='store_true', dest='addTest',
                              help='add new test to configuraiton file')

            group1.add_argument('--testpath', action='store', dest='testpath',
                                default='Simulation/Tests',
                                help='assign path to given test')

            group1.add_argument('--addJobs', action='store_true', dest='addJobs',
                                help='add new job(s) to a given test')

            group1.add_argument('--jobfiles', action='store', dest='jobfiles',
                                default='test.HITS.pool.root',
                                help='comma separated list of files to be updated')

            group1.add_argument('--testtype', action='store', dest='testtype',
                                default='diffpool',
                                help='currently only dcube or diffpool supported')

            group1.add_argument('--jobalias', action='store', dest='jobalias',  default='',
                                help='assign alias to joblist (e.g. cavernbg_tests)')



def get_username():
    return pwd.getpwuid( os.getuid() )[ 0 ]

class RttUpdate :
    def __init__(self,opt,db):
        self.db = db
        self.opt = opt

        # release
        if opt.rel=='rel_X':
            opt.rel='rel_%d'% (dt.date.today().isoweekday() % 7)
        
        # RTT result directory
        if opt.rttpath == 'default':
            opt.rttpath='/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/'+opt.rel+'/'+opt.branch+'/build/'+opt.cmt+'/offline/'+opt.package
            print 'using default rttpath =',opt.rttpath

        # EOS result directory, etc.
        opt.rttcastor='/eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod/rtt/'+opt.rel+'/'+opt.branch+'/'+opt.cmt+'/offline/'+opt.package
        opt.eoscmd='/afs/cern.ch/project/eos/installation/0.1.0-22d/bin/eos.select'
        opt.eosprefix='root://eosatlas.cern.ch/'

        """
        xrdcp root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod/rtt/rel_2/17.1.X.Y/i686-slc5-gcc43-opt/offline/DigitizationTests/mc11_ttbar_LVL1_on/mc11_ttbar.RDO.pool.root root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/ReferenceFiles/MC12/MC11_ttbar_LVL1_ON.RDO.pool.root
"""

        opt.testpath,opt.project=db.findPackage(opt.package)
        opt.ref=db.findReference(opt.branch,opt.project)
        

        opt.refpath='/eos/atlas/atlascerngroupdisk/proj-sit/'+opt.project+'/validation/RTT/referenceFiles/'
        opt.dcubepath='/afs/cern.ch/atlas/project/RTT/DCubeReference/offline/'+opt.testpath+'/'+opt.package+'/'
        if opt.ref!='':
            opt.refpath+=opt.ref+'/'
            opt.dcubepath+=opt.ref+'/'

        #db._dumpTable('t_jobs')
        self.djobs=db.findJobs(opt.package,jobtype='dcube')
        #print 'djobs=',self.djobs

        self.jobs=db.findJobs(opt.package,jobtype='diffpool')
        #print 'jobs=',self.jobs

        if opt.jobs!='':
            print "selected jobs"
            print opt.jobs
            joblist=opt.jobs.split(',')
            print joblist
            
            self.jobs = dict ( [ (k,self.jobs[k])  for k in joblist if self.jobs.has_key(k) ])
            print self.jobs
            self.djobs = dict ( [ (k,self.djobs[k]) for k in joblist if self.djobs.has_key(k) ])
            print self.djobs
            # python 2.7 could use
            # self.djobs = { k:self.djobs[k] for k in joblist if self.djobs.has_key(k) }

        # obsolete for devval
        #self.alias={'AtlasGeom19Sim':'SingleParticleSim'} 
        self.alias={} #{'FrozenShowerFCalOnly':'FrozenShowerTest1'} 
                  
        if self.opt.today=='AUTO':
            self.opt.today=dt.date.today().strftime("%y%m%d")
            #self.opt.today=self.today
        self.opt.user=get_username()
        #self.opt.today='120207-fix'   #*AS* fix !!
        self.logfile='/afs/cern.ch/atlas/groups/Simulation/rtt/rttUpdate.log'

        self.writeLogFile()

    def ls(self):
        os.system('ls '+self.opt.rttpath)

    def printOptions(self):

        print '*** options: ***'
        for i in dir(self.opt):
            if i=='ensure_value' or i=='read_file' or i=='read_module':
                continue
            if i.startswith('_'):
                continue
            print '%12s:'%i,getattr(self.opt,i)

    def writeLogFile(self):
        f=file(self.logfile,'a')
        for i in ['today','dryrun','user','branch','rel','cmt', 'package','jobs','comment']:
            val ='None'
            if hasattr(self.opt,i):
                val =  str(getattr(self.opt,i))
            if val=='':
                val='None'
            if val.count(' ')>0:
                val='"'+val+'"'
            #print i,'=',val
            f.write( val )
            f.write(' ')
        f.write('\n')

    def printLogFile(self):
        f=file(self.logfile)
        for l in f:
            print l,

    def updateDcubeXML(self,tname,path):
        print "updating dcube xml file"
        print tname
        print path

        dxml=path+"dcube-"+tname+".xml"
        dxmln=dxml+".new"
        print dxml

        dfile=file(dxml)
        dnew=file(dxmln,"w")
        for l in dfile:
            if l.count("Results from")>0:
                print l,

                newl= "Results from "
                newl+= dt.date.today().strftime("%d %B %Y")
                newl+= ", "+self.opt.rel+" "
                newl+= self.opt.branch+" "
                newl+= self.opt.cmt
                self.opt.comment=newl
                newl+= " for "+tname
                print newl
                l=newl+"\n"
            dnew.write(l)
        dnew.close()
        dfile.close()

        cmd="mv "+dxmln+" "+dxml
        print cmd
        if not self.opt.dryrun:
            print "..."
            os.system(cmd)
            
            

    def updateDcube(self):
        if len(self.djobs)>0:
            cmd='mkdir '+self.opt.dcubepath+self.opt.today
            print cmd
            if not self.opt.dryrun:
                os.system(cmd)

        # dcube
        for j,files in self.djobs.items():
            epath=self.opt.rttpath+'/'+j
            print '########################################'
            if len(files)==0:
                print 'WARNING, no files given, looking for root files '
                os.system('ls -l '+epath+'/*.root')

            for f in files:
                src=epath+'/'+f
                if not os.path.exists(src):
                    print src,'does not exist'
                    break
                sj = j
                parts=j.split('_')
                dst=self.opt.dcubepath+sj+'.'+f
                s=os.system('ls '+dst)  ### AFS
                if (s==0) :
                    print 'destination exists'
                    cmd='rm '+dst
                    print cmd
                    if not self.opt.dryrun:
                        os.system(cmd)

                cmd='cp -v '+src+' '+self.opt.dcubepath+self.opt.today+'/'+sj+'.'+f
                print cmd
                if not self.opt.dryrun:
                    os.system(cmd)
                cmd='ln -s '+self.opt.today+'/'+sj+'.'+f+' '+dst
                print cmd
                if not self.opt.dryrun:
                    os.system(cmd)

                self.updateDcubeXML(j,self.opt.dcubepath)


        # break
    def updateRefs(self):
        if len(self.jobs)>0:

            # diffPoolFileRunner
            cmd=self.opt.eoscmd+' mkdir '+self.opt.refpath+self.opt.today
            print cmd
            if not self.opt.dryrun:
                os.system(cmd)
    
        for j,files in self.jobs.items():
            epath=self.opt.rttpath+'/'+j
            print '########################################'
            #print epath,':'
            #print files

            if len(files)==0:
                print 'WARNING, no files given, looking for root files '
                os.system('ls -l '+epath+'/*.root')

            for f in files:
                #print '#'
                s=os.system('ls -l '+epath+'/'+f)
                #print '#',s
                if s!=0:
                    epath=self.opt.rttcastor+'/'+j
                    #print epath,':'
                    #s=os.system('nsls -l '+epath+'/'+f)  ### Castor
                    s=os.system(self.opt.eoscmd+' ls '+epath+'/'+f)  ### EOS
                    #print '#',s
                    if s==0:
                        epath= self.opt.eosprefix+epath

                if s==0:
                    # 
                    src=epath+'/'+f
                    sj = j
                    parts=j.split('_')
                    if len(parts)>1 and parts[1] in ['EVNT2TR','TR2HITS']:
                        sj=parts[0]
                    if self.alias.has_key(sj):
                        print 'changed', sj
                        sj=self.alias[sj]
                        print 'to', sj
                        
                    #print parts    
                    pref,suff=f.split('.',1)
                    dst=self.opt.refpath+sj+self.opt.sbranch+'.'+suff
                    #s=os.system('nsls -l '+dst)    ### Castor
                    s=os.system(self.opt.eoscmd+' ls '+dst)  ### EOS
                    #print '@',s
                    if (s==0) :
                        print 'destination exists'
                        # cmd='nsrename '+dst+' '+dst+'.1'
                        # cmd="nsrm "+dst           ### Castor
                        cmd=self.opt.eoscmd+" rm "+dst       ### EOS
                        print cmd
                        if not self.opt.dryrun:
                            os.system(cmd)
                    cmd='xrdcp '+src+' '+self.opt.eosprefix+dst
                    print cmd
                    if not self.opt.dryrun:
                        os.system(cmd)
                    dst=self.opt.refpath+self.opt.today+'/'+sj+self.opt.sbranch+'.'+suff
                    cmd='xrdcp '+src+' '+self.opt.eosprefix+dst
                    print cmd
                    if not self.opt.dryrun:
                        os.system(cmd)
                else:
                    print 'source not found!'


if __name__=='__main__':

    # load configuration
    db=SimRttDataBase('/afs/cern.ch/atlas/groups/Simulation/rtt/rttUpdate.sqlite')
    db.openConnection()
    #db.create()
    #db.initialFill()
    #db.dump()

    # check user rights
    users=db.getUsers()
    #print 'users=', users;
    user=get_username()
    #print 'user=', user
    admin=False
    if user in users:
        #db.addJob(jobname='AtlasG4_minbias',files=['truth.root'],typeid='dcube')
        
        admin=True
        print 'user',user,'authorised for admin commands'

    # parse commandline options
    optP=MyArgumentParser(admin=admin,user=user)
    args = optP.parse_args()
    adminCommand=False

    if args.addBranch:
        print 'addBranch'
        branch=args.branch
        if args.branch=='devval' :
            branch='17.X.0-VAL'
        if args.sbranch=='dev':
            branch='17.X.0'
        print 'branch=',branch
        print 'refpath=',args.ref
        print 'proj=',args.project
        try:
            db._fillTable('t_branches',branch,args.ref,args.project)
            db.cnx.commit()
        except sqlite3.IntegrityError:
            print "key exists, try updating"
            db._updateTable('t_branches',['branch','project'],branch,args.ref,args.project)
        adminCommand=True

    if args.dumpDB:
        db.dump()
        adminCommand=True
    if args.dumpUsers:
        db._dumpTable('t_user')
        adminCommand=True
    if args.dumpTests:
        db._dumpTable('t_tests')
        adminCommand=True
    if args.dumpBranches:
        db._dumpTable('t_branches')
        adminCommand=True

    if not adminCommand:
        rttUpdate=RttUpdate(args,db)
        #rttUpdate.ls()
        rttUpdate.printOptions()
        if args.logs:
            rttUpdate.printLogFile()
        else:
            rttUpdate.updateDcube()
            rttUpdate.updateRefs()
            rttUpdate.writeLogFile()


