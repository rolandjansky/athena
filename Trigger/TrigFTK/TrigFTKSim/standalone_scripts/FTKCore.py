#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Basic classes for job submission
"""

import re,os,sys,datetime,time,getopt,pickle,math,glob,string,stat
import popen2
import subprocess
from sets import Set

# general settings
GRID_SITES = ['ANALY_MWT2_SL6','ANALY_AGLT2','ANALY_SLAC','ANALY_SLAC_LMEM','ANALY_INFN-FRASCATI','ANALY_INFN-NAPOLI','ANALY_INFN-ROMA1','ANALY_INFN-MILANO-ATLASC','CERN-PROD','ANALY_CERN_XROOTD','ANALY_BNL_LONG','ANALY_CERN_SLC6','LOCAL','MWT2_UC_TRIG-DAQ','ANALY_DESY-HH','AUTO']
CONFIG_SEPARATOR = ' '
DEF_JOB_NAME = 'job.in'
PBS_SCRIPT_NAME = 'ftk.pbs'
CONDOR_EXE_NAME = 'runCondorTemplate.sh'
CONDOR_SUB_NAME = 'runCondorTemplate.submit'
DQ2_CACHE_NAME = 'dq2cache.pickle'
PBS_DEPLIMIT = 32       # maximim length of dependency string
MAX_CMD_LENGTH = 120000  # maximum command length supported by your shell
# job names and output folder labels:
_ROADS = 'roads'
_MERGED = 'merged'
_TRACKS = 'tracks'
_ANALYSIS = 'analysis'
# efficiency curve parameters
# Vertical slice range
#patternPoints = [100000,200000,500000,800000,1000000,1500000,2000000,5000000,8000000,10000000]
# High-pileup range
#patternPoints = [100000,500000,1000000,1500000,2000000,5000000,7500000,10000000,15000000,20000000,23000000,30000000]
patternPoints = [100000,1000000,2000000,4194304,5000000,10000000] #,20000000,30000000,60000000]
patternPoints = [1000000,2000000,4194304,10000000] #,20000000,30000000,60000000]
patternPoints = [1000000,2000000,4194304,6000000] #,20000000,30000000,60000000]
#patternPoints = [10000000,20000000,30000000,60000000]
#11x12x18 pattern points
#patternPoints = [100000,1000000,5000000,10000000,35000000,50000000, 65000000,80000000,100000000]
#patternPoints = [10000000,35000000,50000000, 65000000,80000000,100000000]
#patternPoints = [175000000,250000000]
#patternPoints = [100000,500000,1000000,2000000,3000000,5000000,7500000,10000000,20000000,30000000, 60000000, 85000000]#patternPoints = [100000,200000,300000,400000,500000,600000,700000,800000,1000000]
#patternPoints=[7500000,10000000,20000000,30000000, 60000000, 85000000]
etaBins = [0.0,1.0,1.5,2.0,2.5]
pi=3.14159265358979312
phiBins = [-pi,-pi*3/4,-pi/2,-pi/4,0.0,pi/4,pi/2,pi*3/4,pi]
phifiducialMin=[-0.6,0.2,1.0,1.8,2.6,-2.8,-2.2,-1.4]
phifiducialMax=[-0.2,0.6,1.4,2.2,3.0,-2.4,-1.8,-1.0]
nLayersFinalFit=12
nLayersFinalFitOld=11
CURVLIM = 0.5e-3
CTHETALIM = math.sinh(etaBins[-1])
ZLIM = 110.0
# output text file that keeps the list of all submitted grid jobs
_GRIDLIST = os.path.abspath(os.path.join(__file__,'../','gridjobs.txt'))
# output text files that keep N=_LOGSIZE latest job*.in, stdout, and stderr locations
_LOGSIZE = 3
_LOGLIST = []
_LOGLIST.append( os.path.abspath(os.path.join(__file__,'../','.ftk_jobinp')) )
_LOGLIST.append( os.path.abspath(os.path.join(__file__,'../','.ftk_stdout')) )
_LOGLIST.append( os.path.abspath(os.path.join(__file__,'../','.ftk_stderr')) )

import getpass
USERNAME = getpass.getuser()

class DicBase:
    """ General class that provides a dictionary of settings
    This is a base class both for individual job classes and global settings classes
    """
    def __init__(self,**kwargs):
        self.dic={}  # settings and variables
    def Set(self,settings):
        """ Merges a dictionary of keys/values with main settings """
        for key in settings.keys():
            self.dic[str(key)]=settings[key]
    def Add(self,key,value):
        """ Adds one setting to the dictionary """
        self.dic[str(key)]=value
    def Del(self,key):
        """ Removes one setting from the dictionary """
        if key in self.dic:
            del self.dic[key]
        if str(key) in self.dic:
            del self.dic[str(key)]
    def Get(self,key):
        """ Attempts to retrieve a setting from the dictionary """
        try:
            return str(self.dic[key]) if self.dic[key]!=None else None
        except:
            return None
    def Geti(self,key):
        """ Same as Get(), but returns an integer """
        try:
            return int(self.dic[key])
        except:
            print 'Error: requested a non-assigned integer setting:',key
            sys.exit(1)
    def GetOptions(self):
        return self.dic
    def DumpString(self):
        """ Returns settings dictionary in command-line-friendly format """
        res=[]
        for key in self.dic:
            res.append(str(key)+'='+str(self.dic[key]))
        return ','.join(res)
    def Dump(self):
        """ Dumps contents of settings dictionary """
        for key in self.dic:
            print "%s\t%s"%(key,self.dic[key])
    def msg(selg,msg1,msg2='',msg3=''):
        """ Simple message function """
        print '>> ',msg1,msg2,msg3
    def Timestamp(self):
        """ Format of a generic timestamp used in some file and folder names"""
        return datetime.datetime.now().strftime('%Y_%m_%d_%H_%M_%S')
    def Exec(self,cmds,noexec=False):
        """ Executes a set of commands in subshell """
        for cmd in cmds:
            print '> ',cmd
            if noexec==False:
                os.system(cmd)
    def GenericFileCheck(self,file):
        """ Automatically checks files with known extensions """
        for ext in ['patt','patt.bz2','gcon','gcon.bz2','dat','dat.bz2','rmap','tmap','pmap','ss','list',]:
            if file.endswith(ext):
                if not os.path.isfile(file):
                    print "Can't find the following file:",file
                    sys.exit(2)
                if not os.stat(file).st_size>0:
                    print "The following file has zero size:",file
                    sys.exit(2)


    ##################################################################################
    # generic helpers used to specify output files and folders
    def FileBase(self,fin):
        """ strips away .dat or .dat.bz2 or .dat.gz from filenames """
        # a hack to post-process files generated on the grid
        if self.Get('FILENAME_NOSTRIP')!=None:
            return fin
        localnumtest = fin.split('.')
        try: # Deal with grid-generated outputs like .bz2.1
            endint = int(localnumtest[-1])
        except:
            pass
        else:
            fin = '.'.join(localnumtest[:-1])
        for ext in ('bz2','gz','dat'):
            if fin.endswith(ext):
                fin = fin.split('.')
                fin = '.'.join(fin[:-1])
        return fin
    def LabelMod(self,label,doFilt=True,doMrg=False):
        """ Appends modifications to job label if hit_filter or track_merger were used """
        if self.Get('FILTERED_ROADS') == '1' and doFilt:
            label+='_filt'
        if doMrg:
            label+='_merge'
        return label
    def RunName(self,label,n=1):
        """ run name used for PBS jobs and pickle filenames """
        return self.Get('runstring')+'_'+label+'_'.join(['%s']*n)
    def RunPath(self,label,n=1):
        """ PWD for the jobs and location of its .in and stderr/stdout files """
        res=os.path.join(self.Get('outdir'),'run',self.Get('runstring'),label)
        for i in xrange(n):
            res=os.path.join(res,'%s')
        return res
    def OutputPath(self,label,n=1):
        """ OUTDIR for the job containing principal output """
        res=os.path.join(self.Get('outdir'),self.Get('runstring'),label)
        for i in xrange(n):
            res=os.path.join(res,'%s')
        return res
    def OutputFile(self,label,flabel,fin,n=1):
        """ Name of the output file, for a given input file fin.
        Here, flabel could be 'ftkroads_' or 'ftktracks_'
        """
        fin = os.path.basename(fin)
        return os.path.join(self.OutputPath(label,n),flabel+self.FileBase(fin)+'.root')

    ##################################################################################
    # helpers used to retrieve location of bank files [HARDCODED]
    def SectorFile(self,split=True):
        """ path to sector file (split or unsplit)"""
        m=''
        s=''
        if split==True:
            m = '_%sM'%self.Get('M')
            s='_sub%s'
        res=self.Get('patdir')+'/'+'sectors_raw_'+self.Get('L')+'L'+m+'_reg%s'+s+'.'+self.Get('_PATT')
        return res
    def ConstantFile(self,split=True):
        """ path to constant file (split or unsplit)"""
        m=''
        s=''
        if split==True:
            m = '_%sM'%self.Get('M')
            s='_sub%s'
        res=self.Get('gcondir')+'/'+'corrgen_raw_'+self.Get('L')+'L'+m+'_reg%s'+s+'.'+self.Get('_GCON')            
        return res
    def ConstantFile2(self,NLayers,split=True):
        """ path to constant file (split or unsplit) passing the number of layers"""
        m=''
        s=''
        if split==True:
            m = '_%sM'%self.Get('M')
            s='_sub%s'
        res=self.Get('gcondir')+'/'+'corrgen_raw_'+str(NLayers)+'L'+m+'_reg%s'+s+'.'+self.Get('_GCON')            
        return res
    def ConnectionFile(self,NLayers,split=True):
        """ path to constant file (split or unsplit) passing the number of layers"""
        m=''
        s=''
        if split==True:
            m = '_%sM'%self.Get('M')
            s='_sub%s'
        res=self.Get('patdir')+'/'+'sectors_raw_'+str(NLayers)+'L'+m+'_reg%s'+s+'.conn'            
        return res
    def BadSectorFile(self):
        """ path to bad_sectors file (used during cleanup) """
        res=self.Get('gcondir')+'/'+'bad_sectors_'+self.Get('L')+'L_reg%s'
        return res
    def TotSectorFile(self,split=True):
        """ path to total_sectors file (used during cleanup AND split) """
        m=''
        s=''
        if split==True:
            m = '_%sM'%self.Get('M')
            s='_sub%s'
        res=self.Get('gcondir')+'/'+'tot_sectors_'+self.Get('L')+'L'+m+'_reg%s'+s
        return res
    def SliceFile(self):
        """ path to slices file (always unsplit) """
        res=self.Get('gcondir')+'/'+'slices_'+self.Get('L')+'L_reg%s.root'
        return res
    def PatternFile(self,split=True,run=False):
        """ path to pattern file (split or unsplit)"""
        m=''
        s=''
        rs=''
        if split==True:
            m = '_%sM'%self.Get('M')
            s='_sub%s'
        if run==True:
            rs='_run%s'
        res=self.Get('patdir')+'/'+self.Get('patt_type')+'_raw_'+self.Get('L')+'L'
        res+='_'+self.Get('sslabel')+m+'_reg%s'+s+rs+'.'+self.Get('_PATT')
        return res
    def DummyPatternFile(self):
        try:
            file = os.path.join(self.Get('patdir'),'dummy'+self.Get('L')+'L.patt')
            if not (os.path.isfile(file) and os.stat(file).st_size>0):
                f = open(file,'w')
                #print >>f, 'Version 2'
                print >>f, '%d %d'%(0,self.Geti('L'))
                f.close()
        except IOError:
            file = os.path.join(self.Get('inputdir'),'dummy'+self.Get('L')+'L.patt')
            if not (os.path.isfile(file) and os.stat(file).st_size>0):
                f = open(file,'w')
                #print >>f, 'Version 2'
                print >>f, '%d %d'%(0,self.Geti('L'))
                f.close()            
        return file
    def RegionLookupFile(self,ext='root'):
        """ path to region lookup file (tells the region number of each training event) """
        res=os.path.join(self.Get('lookupdir'),'region_'+self.Get('dataset')+'.'+ext)
        return res
    def SectorLookupFile(self):
        """ path to sector lookup file (tells the sector number of each training event in a given region) """
        res=os.path.join(self.Get('lookupdir'),'sector_'+self.Get('dataset')+'_'+self.Get('L')+'L_reg%s.root')
        return res
    def SectorMapFile(self):
        """ path to final sector lookup map (maps 4L+8L to 11L) """
        res=os.path.join(self.Get('lookupdir'),'sectors_4_8_11_reg%s.dat')
        return res

# some regex patterns used in the code
inline_opts=re.compile(r'\[(\S*?)\]',re.I)
pout = re.compile(r'(\${\S*?})',re.I)
pin = re.compile(r'\${(\S*?)}',re.I)

class BaseJob(DicBase):
    """ Base class for various jobs """
    def __init__(self,**kwargs):
        """ Dummy constructor """
        DicBase.__init__(self,**kwargs)
        self.log=[]
        self.status=0
        self.shell=True    # submit through shell script ftk.pbs?
        self.executable=''
        self.extra_args=''
        self.run_name='run_name'
        self.path='./'
        self.config='./'+DEF_JOB_NAME
        self.delay='0'
        self.stdout=os.path.join('./','stdout.log')
        self.stderr=os.path.join('./','stderr.log')
        self.hookups = []   # list of methods to be executed just before running
        # some variables needed for PBS runs:
        self.ROOTSYS=None
        self.LD_LIBRARY_PATH=None
        self.PATH=None
        self.PYTHONPATH=None
        self.customtpl=''
    def Initialize(self):
        """ Sets critical job parameters. This method must be called AFTER child's Initialize() """
        path=self.Get('run_path')  # set in the child's Initialize()
        if path==None:
            self.WARN(2,"run_path not set. Forgot to call child's Initialize()?")
        if not os.path.exists(path):
            try:
                os.makedirs(path)
            except:
                print 'Cannot create run directory. Will try once more right before starting the job.'
        timestamp=self.Timestamp()
        # set job input file name (for pickled jobs, it is already set, so don't do anything)
        if os.path.basename(self.config) == DEF_JOB_NAME:
            file=os.path.join(path,"%s_%s.in"%('job',timestamp))
            self.config=file
        self.run_name=self.Get('run_name')
        self.path=path
        self.stdout=os.path.join(path,"stdout."+timestamp+".log")
        self.stderr=os.path.join(path,"stderr."+timestamp+".log")
        self.condorlog=os.path.join(path,"condorlog."+timestamp+".log")
        # set a unique pickle job name [for PBS --preloadjob]:
        self.pickle=self.Get('run_name')+'_'+timestamp
        self.condorExe=self.Get('run_name')+'_'+timestamp+'.condor.sh'
        self.condorSub=self.Get('run_name')+'_'+timestamp+'.condor'


    def GetEnvVars(self):
        """ Returns a list of critical environment variables """
        res = []
        res.append('export ROOTSYS='+self.ROOTSYS)
        res.append('export LD_LIBRARY_PATH='+self.LD_LIBRARY_PATH)
        res.append('export PATH='+self.PATH)
        if self.PYTHONPATH:   # not necessary, so may be undefined
            res.append('export PYTHONPATH='+self.PYTHONPATH)
        return res
    def LatestFile(self,path,pattern='stderr*.log'):
        """ Returns the latest-modified file matching pattern in folder path """
        files = os.popen("ls -t %s 2> /dev/null"%os.path.join(path,pattern)).read().splitlines()
        if len(files)==0:
            return None
        return files[0]
    def EarliestFile(self,path,pattern='stderr*.log'):
        """ Returns the earliest-modified file matching pattern in folder path """
        files = os.popen("ls -tr %s 2> /dev/null"%os.path.join(path,pattern)).read().splitlines()
        if len(files)==0:
            return None
        return files[0]
    def WARN(self,type=0,*args):
        """ General warning log / exit function """
        self.log.append('WARNING:')
        for m in args:
            self.log.append(m)
        if(type>0):
            self.status=-1
        if(type>1):
            print 'ERROR: Fatal error discovered.'
            for l in self.log:
                print l
            sys.exit(type)
    def Split(self,line):
        """ Splits a space-separated string, if it's not a comment """
        result=[]
        if len(line)>0 and line[0]!='#':
            line=line.split()
        else:
            line=[line,]
        return line
    def ReadTemplate(self,template):
        """ Reads a template into templateA list """
        self.templateA = []
        f = open(template, "r")
        lines = f.readlines()
        # loop over lines and parse the template
        for line in lines:
            # remove an extra '\n' at the end of each line
            line = line.rstrip('\n')
            # save comments as-is, but split everything else into "words"
            line=self.Split(line)
            self.templateA.append(line)
    def SubLines(self):
        """
        Changes value of all key-setting pairs, if they are overridden in the dictionary
        Saves results in templateB list
        """
        self.templateB = []
        for line in self.templateA:
            result = line
            # only do substitution with KEY-VALUE pairs
            if len(line)==2:
                key=line[0]
                if key in self.dic:
                    result=[key,self.Get(key)]
            self.templateB.append(result)
    def ExpandVars(self):
        """
        Expands variables specified in the ${var} notation
        Saves results into templateC list
        """
        self.templateC = []
        for line in self.templateB:
            result=[]
            for elm in line:
                if len(re.findall(pout,str(elm)))==0:
                    result.append(elm)
                else:
                    def repl(match):
                        """ Replaces stuff inside curly brackets with dictionary values """
                        # first cover special cases ('!i')
                        if match.group(1)=='!i':
                            regions=[str(i) for i in xrange(0,self.Geti('N')) if i!=self.Geti('i')]
                            return ' '.join(regions)
                        return str(self.Get(match.group(1)))
                    result.append(pin.sub(repl,elm))
            self.templateC.append(result)
    def ProcessTemplate(self,template):
        """
        Loads and processes a template
        """
        # read the template: creates templateA
        self.ReadTemplate(template)
        # substitute dictionary settings: creates templateB
        self.SubLines()
        # expand ${variables}: creates templateC
        self.ExpandVars()
        return self.templateC
    def PrintConfig(self,doDump=False):
        """
        Creates a config file to be used by FTK
        Also creates run and log directories
        """
        path=self.path
        if not os.path.exists(path):
            try:
                os.makedirs(path)
            except:
                self.WARN(2,'Cannot create a config file in runpath:',path)
        if doDump:
            file='/dev/stdout'
            f = open(file,'a')
        else:
            file=self.config
            f = open(file,'w')
        for line in self.templateC:
            line=[str(l) for l in line]
            # skip metadata lines:
            if len(line)==1 and (line[0]=='#FTKCHECK' or line[0]=='#FTKNOCHECK'): continue
            print >>f, CONFIG_SEPARATOR.join(line)
        if not doDump:
            if not os.path.isfile(file):
                sys.WARN(2,'Failed to create config file: '+file)
            print 'Created config file:',file
        return file
    def Check(self):
        """ A few hardcoded checks for template files: require presence of certain files
            If a block of template code should not be checked, it can be surrouded with:
            #FTKNOCHECK
              my_code_that_wont_be_checked
            #FTKCHECK
        """
        # for non-shell jobs, there are NO templates, so nothing needs to be checked
        if self.shell==False:
            return -1
        # first create all directories found in template files
        for r in self.templateC:
            if len(r)==2 and r[1].find('/')!=-1 and r[0].lower().find('dir')!=-1:
                if not os.path.exists(r[1]):
                    os.makedirs(r[1])
        # now check presence of all maps and files of known extension
        FTKCHECK=1
        for line in self.templateC:
            for piece in line:
                piece=str(piece)
                if len(piece)==0: continue
                if piece=="#FTKCHECK": FTKCHECK=1
                if piece=="#FTKNOCHECK": FTKCHECK=0
                if piece[0]=='#': continue
                if piece.find('/')!=-1 and FTKCHECK==1:
                    self.GenericFileCheck(piece)
        if(self.status==-1):
            print 'ERROR IN CONFIGURATION:'
            try:
                print '\n'.join(self.log)
            except:
                print 'UNEXPECTED ERROR MESSAGE FORMAT. CLEARTEXT ERROR FOLLOWS...'
                print self.log
            print 'END OF ERROR MESSAGE'
            if self.Get('strict_error_check')=='1':
                sys.exit(2)
        return self.status
    def setNCoords(self):
        """ A hacky function to determine # of coordinates from # of layers"""
        # compute number of coordinates (assuming first three are pix)
        # note that if this was overridden via "-o", we don't do anything here
        if self.Get('ncoords'):
            return
        ncoords=0
        for l in xrange(self.Geti('L')):
            # if only sct cy
            if int(self.Geti('L'))==8 and not self.Get("Lv") : 
                ncoords+=1
                # assuming first three are pix cy
            else: 
                if l<3:
                    ncoords+=2
                else:
                    ncoords+=1
        # hack: 5L arch - one pixel + 4 rphi SCTs
        if int(self.Get('L'))==5:
            ncoords=6
        self.Add('ncoords',ncoords)

class RoadFinderJob(BaseJob):
    """ Specific configuration for road finder """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        self.Add('run_name',self.RunName(_ROADS,2)%(self.Get('i'),self.Get('j')))
        self.Add('run_path',self.RunPath(_ROADS,2)%(self.Get('i'),self.Get('j')))
        # if we intend to skip roadmerger, encode subregion info at roadfinder stage
        if self.Get('MERGED_ROADS')!='1':
            self.Add('ENCODE_SUBREGION',self.Get('j'))
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.OutputPath(_ROADS,2)%(self.Get('i'),self.Get('j')))
        self.executable=self.Get('road_finder')
        BaseJob.Initialize(self)
    def function_AddPatternBanks(self):
        """ Constructs pattern banks """
        result = []
        result.append( "# read clusters or raw hits" )
        result.append( "READ_CLUSTERS "+self.Get("READ_CLUSTERS") )
        result.append("NBANKS %s"%self.Get("N"))
        for i in xrange(self.Geti("N")):
            if i==self.Geti("i"):
                result.append('BANK %s %s'%(i,self.Get('MAXPATTERNS')))
                if self.Get("inject_run")!=None: # special hack to directly use patterns from a certain run
                    result.append(self.PatternFile(run=True)%(self.Get('i'),self.Get('j'),self.Get("inject_run")))
                else:
                    result.append(self.PatternFile()%(self.Get('i'),self.Get('j')))
        result=[r.split() for r in result]
        result.insert(0,['# set the number of AM banks, one per geometrical region',])
        result.insert(1,['# set the banks passing ID and the maximum number of patterns (-1 means all)',])
        return result
    def ProcessTemplate(self,template=''):
        """ Adds pattern bank information to roadfinder configuration """
        if template=='':
            if self.customtpl!="" :
                template=os.path.join(self.Get('scripts_dir'),self.customtpl)
            else :
                template=os.path.join(self.Get('scripts_dir'),'roadfinder.tpl')
                
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddPatternBanks()
        self.templateC = result
        return self.templateC

class CheckRoadsJob(BaseJob):
    """
    A simple job that checks that roadfinders in all subregions (in current region)
    have the same number of files in them (to catch hidden roadfinder crashes).
    This can be called as -a checkroads1_0_1_2 to only check subregions {0,1,2}
    """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label='checkroads'
        self.Add('run_name',self.RunName(label)%self.Get('i'))
        self.Add('run_path',self.RunPath(label)%self.Get('i'))
        if self.Get('OUTDIR')==None:
            self.Add('OUTDIR',self.OutputPath(label)%self.Get('i'))
        self.executable='none'
        self.shell = False
        BaseJob.Initialize(self)
    def function_CheckRoads(self):
        """ Special syntax to specify roads for road_merger jobs and do some cross checks """
        filelists = []
        filedirs = []
        # retrive list of files in each folder
        print 'Checking roads in these subregions:',self.subregions
        for j in self.subregions:
            filedir=self.OutputPath(_ROADS,2)%(self.Get('i'),str(j))
            if not os.path.exists(filedir):
                self.WARN(2,"Can't locate the roads directory:",filedir)
            samplefiles=os.listdir(filedir)
            # remove zero-size files
            files=[f for f in samplefiles if os.stat(os.path.join(filedir,f)).st_size>0]
            if len(files)!=len(samplefiles):
                self.WARN(2,"Found some zero-size road files:",files,samplefiles)
            files.sort()
            filelists.append(files)
            filedirs.append(filedir)
        # check that the first folder contains at least one road file
        if len(filelists[0])==0:
            self.WARN(2,'Cannot find any road files in:',filedirs[0])
        # find a subset of files that are common for all folders (so that each subregion is syncronized)
        common=Set(filelists[0])
        for files in filelists:
            common=common.intersection(Set(files))
        common=list(common)
        common.sort()
        if len(common)==0:
            self.WARN(1,'Cannot find any road files that are common across all subregions:',common)
            self.WARN(2,'DEBUG INFO: filelists = ',filelists)
        # check that the # of files is the same in all subregions as in subregion 0
        if len(filelists[0])!=len(common):
            print "Warning: list of files common to all subregions is smaller than expected (%d vs %d)"%(len(common),len(filelists[0]))
            print "Nfiles in each subfolder:"
            for ifl,fl in enumerate(filelists):
                print ifl,len(fl)
            if self.Get('strict_error_check')=='1':
                print 'ERROR: user required that all subregions contain equal # of files (via -o strict_error_check)' 
                sys.exit(2)
        # check that the # of road files is the same as the number of input wrapper files
        # note that this need not be the case - e.g., if the user only ran over a few events/files from the input list
        if self.Get('run_all_inputs')=='1':
            flist=os.path.join(self.Get('inputdir'),'inputprod_raw_'+self.Get('dataset')+'.list')
            f = open(flist,'r')
            nwraps = len(f.readlines())
            f.close()
            print 'Counting wrapper files in %s: %d'%(flist,nwraps)
            if len(common)!=nwraps:
                print "Warning: number of road files not equal to number of input wrapper files (%d vs %d)"%(len(common),nwraps)
                print 'ERROR: user required that ALL wrapper files are processed by roadfinder (via -o run_all_inputs)'
                print 'If you actually intended to process only SOME of staged wrapper files, please remove this flag'
                sys.exit(2)
    def ProcessTemplate(self,template=''):
        """ Adds roads to the end of generic config information """
        self.function_CheckRoads()
        # set a warning flag so that this job is not processed further
        self.WARN(1,"Checkroad job succeeded")
        return 'Done!'

class RoadMergerJob(BaseJob):
    """ Specific configuration for road merger """
    # TODO: allow to specify a general range of i parameter (so we can merge all-detector roads!)
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        self.Add('run_name',self.RunName(_MERGED)%self.Get('i'))
        self.Add('run_path',self.RunPath(_MERGED)%self.Get('i'))
        if self.Get('OUTDIR')==None:
            self.Add('OUTDIR',self.OutputPath(_MERGED)%self.Get('i'))
        self.executable=self.Get('road_merger')
        BaseJob.Initialize(self)
    def function_AddRoadsToMerge(self):
        """ Special syntax to specify roads for road_merger jobs and do some cross checks """
        filelists = []
        filedirs = []
        filepathlists = []
        patfiles = []
        # retrive list of files in each folder
        for j in xrange(self.Geti('M')):
            filedir=self.OutputPath(_ROADS,2)%(self.Get('i'),str(j))
            patfile=self.DummyPatternFile()
            if self.Get('ROADWARRIOR') and self.Geti('ROADWARRIOR')>=3:
                patfile=self.SectorFile()%(self.Get('i'),j)            
            if not os.path.exists(filedir):
                self.WARN(1,"Can't locate the roads directory:",filedir)
                return [['#Failed to locate the roads directory',],];
            samplefiles=os.listdir(filedir)
            # remove zero-size files
            files=[f for f in samplefiles if os.stat(os.path.join(filedir,f)).st_size>0]
            files.sort()
            filelists.append(files)
            filedirs.append(filedir)
            patfiles.append(patfile)
        # check that the first folder contains at least one roads file
        if len(filelists[0])==0:
            self.WARN(1,'Cannot find any road files in:',filedirs[0])
            return [['#Failed to find any roads to merge',],];
        # find a subset of files that are common for all folders (so that each subregion is syncronized)
        common=Set(filelists[0])
        for files in filelists:
            common=common.intersection(Set(files))
        common=list(common)
        common.sort()
        if len(common)==0:
            self.WARN(1,'Cannot find any road files that are common across all subregions:',common)
            self.WARN(1,'DEBUG INFO: filelists = ',filelists)
            return [['#Failed to find any roads to merge',],];
        if len(filelists[0])!=len(common):
            print "Warning: list of files common to all regions is smaller than expected (%d vs %d)"%(len(common),len(filelists[0]))
            print "Nfiles in each subfolder:"
            for ifl,fl in enumerate(filelists):
                print ifl,len(fl)
            if self.Get('strict_error_check')=='1':
                sys.exit(2)
        for filedir in filedirs:
            filepaths=[os.path.join(filedir,f) for f in common]
            filepathlists.append(filepaths)
        # form the result string that lists all banks and files
        result = []
        result.append("NFILES %d\n"%len(filepathlists[0]))
        for j,filepaths in enumerate(filepathlists):
            result.append('BANK %s %s'%(self.Get('i'),j))
            result.append(patfiles[j])
            for file in filepaths:
                result.append(file)
        result=[r.split() for r in result]
        result.insert(0,['\n# number of output files in each sub-region',])
        return result
    def ProcessTemplate(self,template=''):
        """ Adds roads to the end of generic config information """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'roadmerger.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddRoadsToMerge()
        self.templateC = result
        return self.templateC

class HitFilterJob(BaseJob):
    """ Specific configuration for road and hit filter (SS-shifted architecture) """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        if self.Get('FILTERED_ROADS')!='1':
            print 'hit_filter requires "-o FILTERED_ROADS" option. Setting it now...'
            self.Add('FILTERED_ROADS',1)
        if self.Get('MERGED_ROADS')=='1':
            # using output of road_merger as an input
            label=self.LabelMod(_MERGED)
            label_short=label
            self.Add('run_name',self.RunName(label_short)%self.Get('i'))
            self.Add('run_path',self.RunPath(label)%self.Get('i'))
            if self.Get('OUTDIR')==None:
                self.Add('OUTDIR',self.OutputPath(label)%self.Get('i'))
        else:
            # using output of road_finder as an input
            label=self.LabelMod(_ROADS)
            label_short=label
            self.Add('run_name',self.RunName(label_short,2)%(self.Get('i'),self.Get('j')))
            self.Add('run_path',self.RunPath(label,2)%(self.Get('i'),self.Get('j')))
            if self.Get('OUTDIR')==None:
                self.Add('OUTDIR',self.OutputPath(label)%(self.Get('i'),self.Get('j')))
        self.executable=self.Get('hit_filter')
        BaseJob.Initialize(self)
    def function_AddRoadsToFilter(self):
        """ Special syntax to specify roads for road_merger jobs and do some cross checks """
        filelists = []
        filedirs = []
        filepathlists = []
        # retrive list of files in each folder
        for g in xrange(self.Geti('NGROUPS')):
            if g==0:
                filedir=self.OutputPath(_MERGED)%self.Get('i')
                if self.Get('MERGED_ROADS')!='1':
                    filedir=self.OutputPath(_ROADS,2)%(self.Get('i'),self.Get('i'))
            elif g==1:
                runstring_backup = self.Get('runstring')
                self.Add('runstring',self.Get('SHIFTROADS'))
                filedir=self.OutputPath(_MERGED)%self.Get('i')
                if self.Get('MERGED_ROADS')!='1':
                    filedir=self.OutputPath(_ROADS,2)%(self.Get('i'),self.Get('i'))
                self.Add('runstring',runstring_backup)
            else:
                self.WARN(3,'NGROUPS>2 is not supported. Only ss-1/2 has been implemented.')
            if not os.path.exists(filedir):
                self.WARN(1,"Can't locate the roads directory:",filedir)
                return [['#Failed to locate the roads directory',],];
            samplefiles=os.listdir(filedir)
            # remove zero-size files
            files=[f for f in samplefiles if os.stat(os.path.join(filedir,f)).st_size>0]
            files.sort()
            filelists.append(files)
            filedirs.append(filedir)
        # check that the first folder contains at least one roads file
        if len(filelists[0])==0:
            self.WARN(1,'Cannot find any road files in:',filedirs[0])
            return [['#Failed to find any roads to filter',],];
        # find a subset of files that are common for all folders (so that each subregion is syncronized)
        common=Set(filelists[0])
        for files in filelists:
            common=common.intersection(Set(files))
        common=list(common)
        common.sort()
        if len(common)==0:
            self.WARN(1,'Cannot find any merged road files that are common across all groups:',common)
            self.WARN(1,'DEBUG INFO: filelists = ',filelists)
            return [['#Failed to find any roads to filter',],];
        if len(filelists[0])!=len(common):
            print "Warning: list of files common to all regions is smaller than expected (%d vs %d)"%(len(common),len(filelists[0]))
            print "Nfiles in each subfolder:"
            for ifl,fl in enumerate(filelists):
                print ifl,len(fl)
            if self.Get('strict_error_check')=='1':
                sys.exit(2)
        for filedir in filedirs:
            filepaths=[os.path.join(filedir,f) for f in common]
            filepathlists.append(filepaths)
        # form the result string that lists all banks and files
        result = []
        result.append("NFILES %d\n"%len(filepathlists[0]))
        for j,filepaths in enumerate(filepathlists):
            result.append('BANK %s %s'%(self.Get('i'),j))
            for file in filepaths:
                result.append(file)
        result=[r.split() for r in result]
        result.insert(0,['\n# number of output files in each group',])
        return result
    def ProcessTemplate(self,template=''):
        """ Adds roads to the end of generic config information """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'hitfilter.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddRoadsToFilter()
        self.templateC = result
        return self.templateC

class TrackFitter711Job(BaseJob):
    """ Specific configuration for track fitter """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label=self.LabelMod(_TRACKS)
        label_short=label
        if self.Get('MERGED_ROADS')=='1':
            # using output of road_merger as an input
            self.Add('run_name',self.RunName(label_short)%self.Get('i'))
            self.Add('run_path',self.RunPath(label)%self.Get('i'))
            if self.Get('OUT_DIR')==None:
                self.Add('OUT_DIR',self.OutputPath(label)%self.Get('i'))
        else:
            # using output of road_finder as an input
            self.Add('run_name',self.RunName(label_short,2)%(self.Get('i'),self.Get('j')))
            self.Add('run_path',self.RunPath(label,2)%(self.Get('i'),self.Get('j')))
            if self.Get('OUT_DIR')==None:
                self.Add('OUT_DIR',self.OutputPath(label,2)%(self.Get('i'),self.Get('j')))

        if self.Get("TRACKFITTER_MODE")==None :
            self.Add("TRACKFITTER_MODE",str(3))
        # to ensure compatibility with TSP roadfinder
        if self.Get("amlabel")==None :
            self.Add("amlabel",self.Get("sslabel"))
        self.executable=self.Get('track_fitter711')
        BaseJob.Initialize(self)
    def function_AddMergedRoads(self):
        """ Adds outputs of road merging """
        result = []
        # First, specify corrgen banks
        result.append('# set the number of banks and the number of regions')
        result.append('BANKS_SETUP '+self.Get('N')+' '+self.Get('M'))
        UnsplitSectors = True if self.Get('UNSPLIT_SECTORS') else False
        if self.Geti("TRACKFITTER_MODE")==1 or self.Geti("TRACKFITTER_MODE")==3:
            nsubregions = self.Geti('M')
            subloop = range(nsubregions)
            if self.Get('MERGED_ROADS')!='1':
                subloop = [self.Geti('j')]
            for j in subloop:
                result.append('# for each bank there are 5 arguments: #bank #subreg completebank incomplete_bank connection_db')
                result.append('BANK '+self.Get('i')+' '+str(j))
                if UnsplitSectors :
                  result.append( self.ConstantFile2(8, False)%(self.Get('i')) )
                  result.append( self.ConstantFile2(nLayersFinalFitOld, False)%(self.Get('i')) )
                  result.append( self.ConnectionFile(8, False)%(self.Get('i')) )
                else :
                  result.append( self.ConstantFile2(8, True)%(self.Get('i'),str(j)) )
                  result.append( self.ConstantFile2(nLayersFinalFitOld, True)%(self.Get('i'),str(j)) )
                  result.append( self.ConnectionFile(8, True)%(self.Get('i'),str(j)) )
        else :
            subloop=range(self.Geti('M'))
            if self.Get('MERGED_ROADS')!='1': subloop = [self.Geti('j'),]
            for j in subloop:
                result.append('# for each bank there are 5 arguments: #bank #subreg completebank incomplete_bank connection_db')
                result.append('BANK '+self.Get('i')+' '+str(j))
                result.append( self.ConstantFile2(8, not UnsplitSectors)%(self.Get('i'),str(j)) )
                result.append( self.ConstantFile2(nLayersFinalFitOld, not UnsplitSectors)%(self.Get('i'),str(j)) )
                #result.append( "null" ) # modification
                result.append( self.ConnectionFile(8, True)%(self.Get('i'),str(j)) ) # modifications
        result.append('')
        # Now create a list of merged road files (either filtered or default)
        mergedir=self.OutputPath(self.LabelMod(_MERGED))%self.Get('i')
        if self.Get('MERGED_ROADS')!='1':
            mergedir=self.OutputPath(self.LabelMod(_ROADS),2)%(self.Get('i'),self.Get('j'))
        if not os.path.exists(mergedir):
            self.WARN(1,"Can't locate the directory with merged roads:",mergedir)
            return [['#Failed to locate the merged roads directory',],]
        files=os.listdir(mergedir)
        files = [os.path.join(mergedir,f) for f in files if f.endswith('root')]
        if len(files)==0:
            self.WARN(1,"Can't find anything in the merged roads folder:",mergedir)
            return [["#Can't find any roads in the merged roads directory",],]
        files.sort()
        result.append('# paths to merged roads')
        for f in files:
            result.append('INPUTFILE %s'%f)
        result=[self.Split(r) for r in result]
        return result
    def ProcessTemplate(self,template=''):
        """ Adds merged road information to the template """
        if template=='':
            template = os.path.join(self.Get('scripts_dir'),'trackfitter711.tpl')
        self.setNCoords()
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddMergedRoads()
        self.templateC = result
        return self.templateC

class TrackFitterJob(BaseJob):
    """ Specific configuration for track fitter """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label=self.LabelMod(_TRACKS)
        label_short=label
        if self.Get('MERGED_ROADS')=='1':
            # using output of road_merger as an input
            self.Add('run_name',self.RunName(label_short)%self.Get('i'))
            self.Add('run_path',self.RunPath(label)%self.Get('i'))
            if self.Get('OUT_DIR')==None:
                self.Add('OUT_DIR',self.OutputPath(label)%self.Get('i'))
        else:
            # using output of road_finder as an input
            self.Add('run_name',self.RunName(label_short,2)%(self.Get('i'),self.Get('j')))
            self.Add('run_path',self.RunPath(label,2)%(self.Get('i'),self.Get('j')))
            if self.Get('OUT_DIR')==None:
                self.Add('OUT_DIR',self.OutputPath(label,2)%(self.Get('i'),self.Get('j')))
        self.executable=self.Get('track_fitter')
        BaseJob.Initialize(self)
    def function_AddMergedRoads(self):
        """ Adds outputs of road merging """
        result = []
        # First, specify corrgen banks
        result.append('# set the number of banks and the number of regions')
        result.append('BANKS_SETUP '+self.Get('N')+' '+self.Get('M'))
        result.append('# for each bank there are 3 arguments: #bank #subreg bank_patt')
        subloop=range(self.Geti('M'))
        if self.Get('MERGED_ROADS')!='1': subloop = [self.Geti('j'),]
        UnsplitSectors = True if self.Get('UNSPLIT_SECTORS') else False
        for j in subloop:
            result.append('BANK '+self.Get('i')+' '+str(j))
            if UnsplitSectors :
              result.append( self.ConstantFile(False) % self.Get('i') )
            else :
              result.append( self.ConstantFile(True)%(self.Get('i'),str(j)) )
        result.append('')
        # Now create a list of merged road files (either filtered or default)
        mergedir=self.OutputPath(self.LabelMod(_MERGED))%self.Get('i')
        if self.Get('MERGED_ROADS')!='1':
            mergedir=self.OutputPath(self.LabelMod(_ROADS),2)%(self.Get('i'),self.Get('j'))
        if not os.path.exists(mergedir):
            self.WARN(1,"Can't locate the directory with merged roads:",mergedir)
            return [['#Failed to locate the merged roads directory',],]
        files=os.listdir(mergedir)
        files = [os.path.join(mergedir,f) for f in files if f.endswith('root')]
        if len(files)==0:
            self.WARN(1,"Can't find anything in the merged roads folder:",mergedir)
            return [["#Can't find any roads in the merged roads directory",],]
        files.sort()
        result.append('# paths to merged roads')
        for f in files:
            result.append('INPUTFILE %s'%f)
        result.append( '# number of sectors to include in firmware testing output' )
        result.append( 'FWO_NSECTORS '+self.Get('FWO_NSECTORS') )
        result=[self.Split(r) for r in result]
        return result
    def ProcessTemplate(self,template=''):
        """ Adds merged road information to the template """
        if template=='':
            template = os.path.join(self.Get('scripts_dir'),'trackfitter.tpl')
        self.setNCoords()
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddMergedRoads()
        self.templateC = result
        return self.templateC

class CheckTracksJob(BaseJob):
    """
    A simple job that checks that trackfitters in all subregions (in current region)
    have the same number of files in them (to catch hidden trackfitter crashes).
    This can be called as -a checktracks1_0_1_2 to only check subregions {0,1,2}
    PS - this also works if trackfitter was run on merged roads (i.e. in-region)
    """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label='checktracks'
        self.Add('run_name',self.RunName(label)%self.Get('i'))
        self.Add('run_path',self.RunPath(label)%self.Get('i'))
        if self.Get('OUTDIR')==None:
            self.Add('OUTDIR',self.OutputPath(label)%self.Get('i'))
        self.executable='none'
        self.shell = False
        BaseJob.Initialize(self)
    def function_CheckTracks(self):
        """ Special syntax to specify tracks and do some cross checks """
        i = self.Geti('i')
        filelists = []
        filedirs = []
        # retrive list of track files in each folder
        if len(self.subregions)==1 and self.subregions[0]==None:
            print 'Checking tracks in region:',i
        else:
            print 'Checking tracks in these subregions:',self.subregions
        for j in self.subregions:
            if len(self.subregions)==1 and self.subregions[0]==None:
                filedir=self.OutputPath(self.LabelMod(_TRACKS,doMrg=False))%i
            else:
                filedir=self.OutputPath(self.LabelMod(_TRACKS),2)%(i,j)
            if not os.path.exists(filedir):
                self.WARN(3,"Can't locate the tracks directory:",filedir)
            samplefiles=os.listdir(filedir)
            # remove zero-size files
            files=[f for f in samplefiles if os.stat(os.path.join(filedir,f)).st_size>0]
            if len(files)!=len(samplefiles):
                self.WARN(3,"Found some zero-size track files:",files,samplefiles)
            files.sort()
            filelists.append(files)
            filedirs.append(filedir)
        # check that the first folder contains at least one track file
        if len(filelists[0])==0:
            self.WARN(3,"Cannot find any track files in:",filedirs[0])
        # find a subset of files that are common for all folders (so that each subregion is syncronized)
        common=Set(filelists[0])
        for files in filelists:
            common=common.intersection(Set(files))
        common=list(common)
        common.sort()
        if len(common)==0:
            self.WARN(1,'Cannot find any track files that are common across all subregions:',common)
            self.WARN(3,'DEBUG INFO: filelists = ',filelists)
        # check that the # of files is the same in all subregions as in subregion 0
        if len(filelists[0])!=len(common):
            print "Warning: list of files common to all subregions is smaller than expected (%d vs %d)"%(len(common),len(filelists[0]))
            print "Nfiles in each subfolder:"
            for ifl,fl in enumerate(filelists):
                print ifl,len(fl)
            if self.Get('strict_error_check')=='1':
                print 'ERROR: user required that all subregions contain equal # of files (via -o strict_error_check)' 
                sys.exit(3)
        # check that the # of track files is the same as the number of input wrapper files
        # note that this need not be the case - e.g., if the user only ran over a few events/files from the input list
        if self.Get('run_all_inputs')=='1':
            flist=os.path.join(self.Get('inputdir'),'inputprod_raw_'+self.Get('dataset')+'.list')
            f = open(flist,'r')
            nwraps = len(f.readlines())
            f.close()
            print 'Counting wrapper files in %s: %d'%(flist,nwraps)
            if len(common)!=nwraps:
                print "Warning: number of track files not equal to number of input wrapper files (%d vs %d)"%(len(common),nwraps)
                print 'ERROR: user required that ALL wrapper files are processed by trackfitter (via -o run_all_inputs)'
                print 'If you actually intended to process only SOME of staged wrapper files, please remove this flag'
                sys.exit(3)
    def ProcessTemplate(self,template=''):
        """ Performs checktracks action """
        self.function_CheckTracks()
        # set a warning flag so that this job is not processed further
        self.WARN(1,"Checktrack job succeeded")
        return 'Done!'

class TrackMergerJob(BaseJob):
    """ Specific configuration for track merger + global trackmerger """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        # we keep the same base folder, but add modifiers to the label:
        label=self.LabelMod(_TRACKS,doMrg=True)
        label_short=label
        self.Add('run_name',self.RunName(label_short)%self.Get('i'))
        self.Add('run_path',self.RunPath(label)%self.Get('i'))
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.OutputPath(label)%self.Get('i'))
        self.executable=self.Get('track_merger')
        BaseJob.Initialize(self)
    def function_AddTracks(self):
        """ Special syntax to specify roads for road_merger jobs and do some cross checks """
        filelists = []
        filedirs = []
        bankinfo = []
        subloop=[None,] # dummy loop if trackfitter ran on complete regions
        if self.Get('MERGED_ROADS')!='1': subloop = self.subregions
        # retrive list of files in each folder
        for i in self.regions:
            for j in subloop:
                if j==None:
                    filedir=self.OutputPath(self.LabelMod(_TRACKS,doMrg=(self.Get("INPUT_TREE_MERGED")=="1" or self.Get("MERGED_TRACKS")=="0")))%i
                else:
                    filedir=self.OutputPath(self.LabelMod(_TRACKS),2)%(i,j)
                if not os.path.exists(filedir):
                    self.WARN(1,"Can't locate the tracks directory:",filedir)
                    return [['#Failed to locate the tracks directory',],];
                #samplefiles=os.listdir(filedir)
                samplefiles=glob.glob(os.path.join(filedir,'*root*'))
                # remove zero-size files
                #files=[f for f in samplefiles if os.stat(os.path.join(filedir,f)).st_size>0]
                files=[os.path.basename(f) for f in samplefiles if os.stat(f).st_size>0]
                files.sort()
                filelists.append(files)
                filedirs.append(filedir)
                bankinfo.append((i,j))
        # check that the first folder contains at least one tracks file
        if len(filelists[0])==0:
            self.WARN(1,'Cannot find any track files in:',filedirs[0])
            return [['#Failed to find any tracks to merge',],];
        # find a subset of files that are common for all folders (so that each subregion is syncronized)
        common=Set(filelists[0])
        for files in filelists:
            common=common.intersection(Set(files))
        common=list(common)
        common.sort()
        if len(common)==0:
            self.WARN(1,'Cannot find any track files that are common across all regions:',common)
            self.WARN(1,'DEBUG INFO: filelists = ',filelists)
            return [['#Failed to find any tracks to merge',],];
        if len(filelists[0])!=len(common):
            print "Warning: list of files common to all regions is smaller than expected (%d vs %d)"%(len(common),len(filelists[0]))
            print "Nfiles in each subfolder:"
            for ifl,fl in enumerate(filelists):
                print ifl,len(fl)
            if self.Get('strict_error_check')=='1':
                sys.exit(2)
        # form the result string that lists all banks and files
        result = []
        result.append("NFILES %d\n"%len(common))
        for i in self.regions:
            for j in subloop:
                if j == None:
                    result.append("BANK %d %d"%(i,0))
                else:
                    result.append("BANK %d %d"%(i,j))
                fns=self.Get('FILENAME_NOSTRIP')
                self.Add('FILENAME_NOSTRIP',1)
                for fin in common:
                    fin='.'.join(fin.split('.')[:-1])
                    #fin=''.join(fin.split('.root')[:-1])
                    if j == None:
                        file=self.OutputFile(self.LabelMod(_TRACKS,doMrg=(self.Get("INPUT_TREE_MERGED")=="1" or self.Get("MERGED_TRACKS")=="0")),'',fin)%i
                    else:
                        file=self.OutputFile(self.LabelMod(_TRACKS),'',fin,2)%(i,j)
                    if os.path.isfile(file):
                        result.append(file)
                    else:
                        self.WARN(1,'Cannot find track file:',file)
                        return [['#Failed to find single-region track files',],];
                self.Add('FILENAME_NOSTRIP',fns)
        result=[r.split() for r in result]
        return result
    def ProcessTemplate(self,template=''):
        """ Adds merged road information to the template """
        if template=='':
            template = os.path.join(self.Get('scripts_dir'),'trackmerger.tpl')
        self.setNCoords()
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddTracks()
        self.templateC = result
        return self.templateC

def _hookup_restore_config(job):
    """ Used by AnaJob
    makes a basename of job config file path (as required by ana script)
    backs up root output from previous runs
    """
    job.CleanseOutputDir()
    job.config=os.path.basename(job.config)

class AnaJob(BaseJob):
    """ A job that constructs an input file for TrigFTKAna (and executes it)
    """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label=self.LabelMod(_ANALYSIS,doMrg=(self.Get('MERGED_TRACKS')=='1'))
        label_short=label
        self.Add('run_name',self.RunName(label_short)%self.Get('i'))
        self.Add('run_path',self.RunPath(label)%self.Get('i'))
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.OutputPath(label)%self.Get('i'))
        self.executable=self.Get(self.Get('anaexe'))
        BaseJob.Initialize(self)
        # request that the input file is specified without preceeding path [that's the only way to specify input file]
        self.hookups.append(_hookup_restore_config)
        extras=''
        # restrict to region 0 if running -a ana0
        if self.Geti('i')==0:
            extras='--region 0'
        # check if user redefined any of the analysis cuts:
        extras += ' --mcut %s'%self.Get('mcut') if self.Get('mcut') else ''
        extras += ' --absetacut %s'%self.Get('absetacut') if self.Get('absetacut') else ''
        extras += ' --pt %s'%self.Get('pt') if self.Get('pt') else ''
        extras += ' --absd0 %s'%self.Get('absd0') if self.Get('absd0') else ''
        # output root file name (.root will be appended automatically)
        anaout = self.Get('anaout') or 'ana'
        # cmd line arguments to Antonio's analysis code
        self.extra_args='--offline-read wrap '+extras+' --noROOT --sample-def-dir '+self.path+' --force-output-filename '+os.path.join(self.Get('OUT_DIR'),anaout)
    def GetNentriesFromFile(self,file):
        """ Guesses # of events in the 'ftkdata' TTree """
        command = "echo 'TTree *t = (TTree*)gFile->Get(\"ftkdata\"); std::cout << \"ENTRIES \" << t->GetEntries() << std::endl; gApplication->ProcessLine(\".q\");' | root -b -l "+file+' 2>/dev/null'
        pipe=os.popen(command,"r")
        nevents = 0
        for line in pipe.readlines():
            line = line.strip()
            line = line.split('ENTRIES')
            if len(line)==2:
                try:
                    nevents=int(line[1])
                    break
                except:
                    pass
        pipe.close()
        return nevents
    def CleanseOutputDir(self):
        """ Shortcut: process the job """
        # backup old results (if any)
        for i in [self.Geti('i'),]:
            destroot = self.OutputFile(self.LabelMod(_ANALYSIS,doMrg=(self.Get('MERGED_TRACKS')=='1')),'','ana')%i
            if os.path.isfile(destroot):
                tlong=os.path.getmtime(destroot)
                t=datetime.datetime.fromtimestamp(tlong)
                timestamp=t.strftime('%Y_%m_%d__%H_%M_%S')
                newroot=os.path.splitext(destroot)[0]+'.'+timestamp+os.path.splitext(destroot)[1]
                os.rename(destroot,newroot)
                print 'Renaming existing root file',os.path.basename(destroot),'into',os.path.basename(newroot)
    def ProcessTemplate(self,template=''):
        """ Makes a config file for ana jobs.
        Note that it guesses # of events from the ROOT track files """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'ana_standard_overview.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        flist=os.path.join(self.Get('inputdir'),'inputprod_raw_'+self.Get('dataset')+'.list')
        if self.Get('FTKDAT_LIST')!=None:
            flist = self.Get('FTKDAT_LIST')
        f = open(flist,'r')
        maxevts = self.Geti('nevents')
        totevts = 0
        track_regions = self.regions
        for l in f:
            W=l.strip()
            if not os.path.isfile(W):
                continue
            N = 0
            for i in track_regions:
                T = self.OutputFile(self.LabelMod(_TRACKS,doMrg=(self.Get('MERGED_TRACKS')=='1')),'ftktracks_',W)%i
                if os.path.isfile(T):
                    N = self.GetNentriesFromFile(T)
                    print 'Counting # of events in region',i,'-',N
                    if N>0: break
            if totevts+N>maxevts: N = maxevts-totevts
            if N==0: continue
            result.append(['W',W])
            result.append(['N',str(N)])
            totevts += N
            # add roads to configuration file (if requested) - this makes ana jobs SLOW!
            if self.Get('ANA_IGNORE_ROADS')!='1':
                regloop=self.regions
                # special case: if regions=[8,], we need ALL regions' roads
                if len(self.regions)==1 and self.regions[0]==self.Geti('N'):
                    regloop=range(self.Geti('N'))
                for i in regloop:
                    R = self.OutputFile(self.LabelMod(_MERGED),'ftkroads_',W)%i
                    if os.path.isfile(R):
                        result.append(['R',R,i])
            for i in track_regions:
                T = self.OutputFile(self.LabelMod(_TRACKS,doMrg=(self.Get('MERGED_TRACKS')=='1')),'ftktracks_',W)%i
                if os.path.isfile(T):
                    result.append(['T',T,'M'])
            result.append(['E',])
            if totevts>=maxevts: break
        f.close()
        self.templateC = result
        if self.executable == None:
            self.WARN(1,"Can't find TrigFTKAna executables.")
        return self.templateC

class EffstudyJob(BaseJob):
    """ A job that constructs an input file for TrigFTKAna (and executes it)
    """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label=self.LabelMod("effstudy",doMrg=(self.Get('MERGED_TRACKS')=='1'))
        label_short=label
        self.Add('run_name',self.RunName(label_short)%self.Get('i'))
        self.Add('run_path',self.RunPath(label)%self.Get('i'))
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.OutputPath(label)%self.Get('i'))
        self.executable=self.Get('ana_ftkeffchain')
        BaseJob.Initialize(self)
        # request that the input file is specified without preceeding path [that's the only way to specify input file]
        self.hookups.append(_hookup_restore_config)
        extras=''
        # restrict to region 0 if running -a ana0
        if self.Geti('i')==0:
            extras='--region 0'
        # check if user redefined any of the analysis cuts:
        extras += ' --mcut %s'%self.Get('mcut') if self.Get('mcut') else ''
        extras += ' --absetacut %s'%self.Get('absetacut') if self.Get('absetacut') else ''
        extras += ' --pt %s'%self.Get('pt') if self.Get('pt') else ''
        extras += ' --absd0 %s'%self.Get('absd0') if self.Get('absd0') else ''
        extras += ' --read-offline-si-channels=1 '
        if self.Get('Lv') == None :
            NL = self.Get('L')+'L'
        else :
            NL = self.Get('L')+'L'+self.Get('Lv')
        if self.Get("PMAP_FILE") : # if an exicit PMAP file is specified this is used
            mapdir_string_index = self.Get("PMAP_FILE").find("${mapdir}")
            if mapdir_string_index>=0 :
                extras += ' --ftk-pmap '+self.Get('mapdir')+self.Get('PMAP_FILE')[mapdir_string_index+9:]
            else:
                extras += ' --ftk-pmap '+self.Get('PMAP_FILE')
        else :
            extras += ' --ftk-pmap '+self.Get('mapdir')+"/"+self.Get("raw")+"_"+NL+".pmap "
        # output root file name (.root will be appended automatically)
        anaout = self.Get('anaeff') or 'anaeff'
        # cmd line arguments to Antonio's analysis code
        self.extra_args='--offline-read wrap '+extras+' --noROOT --sample-def-dir '+self.path+' --force-output-filename '+os.path.join(self.Get('OUT_DIR'),anaout)
    def GetNentriesFromFile(self,file):
        """ Guesses # of events in the 'ftkdata' TTree """
        command = "echo 'TTree *t = (TTree*)gFile->Get(\"ftkdata\"); std::cout << \"ENTRIES \" << t->GetEntries() << std::endl; gApplication->ProcessLine(\".q\");' | root -b -l "+file+' 2>/dev/null'
        pipe=os.popen(command,"r")
        nevents = 0
        for line in pipe.readlines():
            line = line.strip()
            line = line.split('ENTRIES')
            if len(line)==2:
                try:
                    nevents=int(line[1])
                    break
                except:
                    pass
        pipe.close()
        return nevents
    def CleanseOutputDir(self):
        """ Shortcut: process the job """
        # backup old results (if any)
        for i in [self.Geti('i'),]:
            destroot = self.OutputFile(self.LabelMod(_ANALYSIS,doMrg=(self.Get('MERGED_TRACKS')=='1')),'','ana')%i
            if os.path.isfile(destroot):
                tlong=os.path.getmtime(destroot)
                t=datetime.datetime.fromtimestamp(tlong)
                timestamp=t.strftime('%Y_%m_%d__%H_%M_%S')
                newroot=os.path.splitext(destroot)[0]+'.'+timestamp+os.path.splitext(destroot)[1]
                os.rename(destroot,newroot)
                print 'Renaming existing root file',os.path.basename(destroot),'into',os.path.basename(newroot)
    def ProcessTemplate(self,template=''):
        """ Makes a config file for ana jobs.
        Note that it guesses # of events from the ROOT track files """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'ana_standard_overview.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        flist=os.path.join(self.Get('inputdir'),'inputprod_raw_'+self.Get('dataset')+'.list')
        if self.Get('FTKDAT_LIST')!=None:
            flist = self.Get('FTKDAT_LIST')
        f = open(flist,'r')
        maxevts = self.Geti('nevents')
        totevts = 0
        track_regions = self.regions
        for l in f:
            W=l.strip()
            if not os.path.isfile(W):
                continue
            N = 0
            for i in track_regions:
                T = self.OutputFile(self.LabelMod(_TRACKS,doMrg=(self.Get('MERGED_TRACKS')=='1')),'ftktracks_',W)%i
                if os.path.isfile(T):
                    N = self.GetNentriesFromFile(T)
                    print 'Counting # of events in region',i,'-',N
                    if N>0: break
            if totevts+N>maxevts: N = maxevts-totevts
            if N==0: continue
            result.append(['W',W])
            result.append(['N',str(N)])
            totevts += N
            # add roads to configuration file (if requested) - this makes ana jobs SLOW!
            if self.Get('ANA_IGNORE_ROADS')!='1':
                regloop=self.regions
                # special case: if regions=[8,], we need ALL regions' roads
                if len(self.regions)==1 and self.regions[0]==self.Geti('N'):
                    regloop=range(self.Geti('N'))
                for i in regloop:
                    R = self.OutputFile(self.LabelMod(_MERGED),'ftkroads_',W)%i
                    if os.path.isfile(R):
                        result.append(['R',R,i])
            else :
                self.WARN(2,"ANA_IGNORE_ROADS=0 is required to run the efficiency study")
            for i in track_regions:
                T = self.OutputFile(self.LabelMod(_TRACKS,doMrg=(self.Get('MERGED_TRACKS')=='1')),'ftktracks_',W)%i
                if os.path.isfile(T):
                    result.append(['T',T,'M'])
            result.append(['E',])
            if totevts>=maxevts: break
        f.close()
        self.templateC = result
        if self.executable == None:
            self.WARN(1,"Can't find TrigFTKAna executables.")
        return self.templateC

class PlotterJob(BaseJob):
    """ Interface to simple even looper/plotter """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label=self.LabelMod('plots')
        label_short=label
        self.Add('run_name',self.RunName(label_short,0))
        self.Add('run_path',self.RunPath(label,0))
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.OutputPath(label,0))
        self.executable='none'
        self.shell = False
        BaseJob.Initialize(self)
    def ProcessTemplate(self,template='irrelevant'):
        """ runs plotting script """
        import plots
        import ROOT
        #LT ADD 1/22/2012
        ROOT.PyConfig.IgnoreCommandLineOptions = True
        mergefiles=os.path.join(self.OutputPath(self.LabelMod(_MERGED)),'*.root')
        if self.Get('MERGED_TRACKS')=='1':
            # use files under track_merge/2/... - useful for grid-processed jobs that don't have tracks/2/ folder
            trackfiles=os.path.join(self.OutputPath(self.LabelMod(_TRACKS,doMrg=True)),'*.root')
        elif self.Get('MERGED_ROADS')=='1':
            # OR: use files under tracks/2/... (only works if trackfitter ran on MERGED roads!)
            trackfiles=os.path.join(self.OutputPath(self.LabelMod(_TRACKS)),'*.root')
        else:
            print 'ERROR: at least one of MERGED_ROADS/MERGED_TRACKS must be enabled to run -a plot action!'
            sys.exit(3)
        if self.Get('batch_mode')!=None: ROOT.gROOT.SetBatch(1)
        plots=plots.RootPlotter(self.Get('libftk'),self.regions,mergefiles,trackfiles,self.Geti('L'))
        plots.SetPlotDir(self.Get('OUT_DIR'))
        try:
            os.makedirs(plots.plotdir)
        except:
            pass
        plots.SetDump(self.Get('dump_events'))
        if plots.Load(self.Get("MERGED_TRACKS"))!=-1:
            plots.Loop(self.Geti('nevents'))
            # plots.PlotHistos()
            plots.PrintCounters(self.stdout)
        # set a warning flag so that this job is not processed further
        self.WARN(1,"Plotting job succeeded")
        return 'Done!'

class NewEffCurveJob(BaseJob):
    """ Interface to Efficiency turn-on curve plotter """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        r = self.regions[0]
        label=self.LabelMod('neweffcurve')
        label_short=label
        self.Add('run_name',self.RunName(label_short)%r)
        self.Add('run_path',self.RunPath(label)%r)
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.OutputPath(label)%r)
        if not os.path.exists(self.Get('OUT_DIR')):
            try:
                os.makedirs(self.Get('OUT_DIR'))
            except:
                print 'Failed to create output directory in',self.Get('OUT_DIR')
                sys.exit(3)
        self.executable='none'
        self.shell = False
        BaseJob.Initialize(self)
    def ProcessTemplate(self,template='irrelevant'):
        """ runs efficiency turn-on curve builder script """
        # set a warning flag so that this job is not processed further
        self.WARN(1,"EffCurve job succeeded")
        return 'Done!'

class EffCurveJob(BaseJob):
    """ Interface to Efficiency turn-on curve plotter """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        r = self.regions[0]
        label=self.LabelMod('effcurve')
        label_short=label
        self.Add('run_name',self.RunName(label_short)%r)
        self.Add('run_path',self.RunPath(label)%r)
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.OutputPath(label)%r)
        if not os.path.exists(self.Get('OUT_DIR')):
            try:
                os.makedirs(self.Get('OUT_DIR'))
            except:
                print 'Failed to create output directory in',self.Get('OUT_DIR')
                sys.exit(3)
        self.executable='none'
        self.shell = False
        BaseJob.Initialize(self)
    def ProcessTemplate(self,template='irrelevant'):
        """ runs efficiency turn-on curve builder script """
        # setup root
        try:
            import ROOT
            if self.Get('batch_mode')!=None:
                ROOT.gROOT.SetBatch(1)
        except:
            print "Couldn't import ROOT module"
            print "Try using the version of python that comes with Athena"
            print ">>   XX.X.X/sw/lcg/external/Python/"
            sys.exit(3)
        libftk = self.Get('libftk')
        try:
            ROOT.gSystem.Load(libftk)
            roads=ROOT.FTKRoadStream()
            tracks=ROOT.FTKTrackStream()
        except:
            print "Couldn't import libftk module, or wrong libftk version"
            print "Check location and compilation settings:",libftk
            sys.exit(3) 
        # setup wrapper truth access
        import truth
        flist=os.path.join(self.Get('inputdir'),'inputprod_raw_'+self.Get('dataset')+'.list')
        if self.Get('FTKDAT_LIST')!=None:
            flist = self.Get('FTKDAT_LIST')
        wraplist = os.path.join(flist)
        wrap = truth.WrapperTruth(wraplist,True)
        # temporarily rename runstring to get the right tracks folder
        runstring=self.Get('runstring')
        newrunname=runstring+'_'+'np%s'
        self.Add('runstring',newrunname)
        # set up track TTree's
        r = self.regions[0]
        roadTree = []
        trackTree = []
        nentries = []
        effcurvePatternPoints=[]
        skipPointList=[]
        useRoads=True
        for i,maxpat in enumerate(patternPoints):
            # Set up the TChain
            trackTree.append( ROOT.TChain("ftkdata","Tracks") )
            ROOT.SetOwnership(trackTree[i], False )
            roadTree.append( ROOT.TChain("ftkdata","Roads") )
            ROOT.SetOwnership(roadTree[i], False )
            # Chain the ROOT files
            for f in wrap.wrapperList:
                if not os.path.isfile(f):
                    continue
                R = self.OutputFile(self.LabelMod(_MERGED),'ftkroads_',f)%(maxpat,r)
                # if roadmerger was run, use tracks from tracks/$reg/*.root
                if self.Get('MERGED_ROADS')=='1':
                    T = self.OutputFile(self.LabelMod(_TRACKS),'ftktracks_',f)%(maxpat,r)
                # if roadmerger was NOT run, use tracks from tracks_merge/$reg/*.root
                elif self.Get('MERGED_TRACKS')=='1':
                    T = self.OutputFile(self.LabelMod(_TRACKS,doMrg=True),'ftktracks_',f)%(maxpat,r)
                    #T = self.OutputFile(self.LabelMod(_TRACKS,doMrg=True),'ftktracks_',f)%(maxpat,8)
                else:
                    print "ERROR: at least one of MERGED_ROADS/MERGED_TRACKS needs to be enabled for effcurve jobs!"
                    sys.exit(3)
                if not os.path.isfile(T):
                    continue
                print T
                trackTree[i].Add(T)
                if not os.path.isfile(R):
                    continue                
                print R
                roadTree[i].Add(R)
            _nentries=int(trackTree[i].GetEntries())
            _nentries_rd=int(roadTree[i].GetEntries())
            print "self.Geti('MAXPATTERNS') =",self.Geti('MAXPATTERNS')
            print "maxpat =",maxpat,"     nentries =",_nentries,"     nentries_r =",_nentries_rd,"     file =",T
            if (_nentries>0) and (self.Geti('MAXPATTERNS')==-1 or maxpat < self.Geti('MAXPATTERNS')):
                nentries.append(_nentries)
                effcurvePatternPoints.append(maxpat)
                if _nentries != _nentries_rd:
                    useRoads=False
                    if _nentries_rd>0:
                        print 'Warning: unequal number of events in track (%d) and road (%d) trees'%(_nentries,_nentries_rd)
            else:
                skipPointList.append(maxpat)
        if not useRoads:
            print 'Warning: will only plot efficiency.'
        for i,dummy in enumerate(effcurvePatternPoints):
            # roads must be in merged/ folder for this:
            if useRoads:
                roadTree[i].SetBranchAddress("FTKBank%d."%r,ROOT.AddressOf(roads))
            # tracks can be either in tracks/, or in tracks_merge/
            if self.Get('MERGED_ROADS')=='1':
                trackTree[i].SetBranchAddress("FTKBank%d."%r,ROOT.AddressOf(tracks))
            else:
                trackTree[i].SetBranchAddress("FTKBankMerged",ROOT.AddressOf(tracks))
        print '# of entries:',nentries
        print 'skipped points with 0 entries',skipPointList
        self.Add('runstring',runstring)  # restore

        # nominator in efficiency calculation in various phi and eta bins [TRACKS]
        foundCounts = []
        foundCountsPhi = []
        for i in etaBins:
            foundCounts.append([0]*len(effcurvePatternPoints))
        for i in phiBins:
            foundCountsPhi.append([0]*len(effcurvePatternPoints))
        foundCountsPhi0fiducial = [0]*len(effcurvePatternPoints)
        foundCountsEta1Phi0fiducial = [0]*len(effcurvePatternPoints)
        foundCountsEta1p5Phi0fiducial = [0]*len(effcurvePatternPoints)

        # nominator in efficiency calculation in various phi and eta bins [ROADS 6/7 || 7/7]
        foundCounts67 = []
        foundCountsPhi67 = []
        for i in etaBins:
            foundCounts67.append([0]*len(effcurvePatternPoints))
        for i in phiBins:
            foundCountsPhi67.append([0]*len(effcurvePatternPoints))
        foundCountsPhi0fiducial67 = [0]*len(effcurvePatternPoints)
        foundCountsEta1Phi0fiducial67 = [0]*len(effcurvePatternPoints)
        foundCountsEta1p5Phi0fiducial67 = [0]*len(effcurvePatternPoints)

        # nominator in efficiency calculation in various phi and eta bins [ROADS 7/7 ONLY]
        foundCounts77 = []
        foundCountsPhi77 = []
        for i in etaBins:
            foundCounts77.append([0]*len(effcurvePatternPoints))
        for i in phiBins:
            foundCountsPhi77.append([0]*len(effcurvePatternPoints))
        foundCountsPhi0fiducial77 = [0]*len(effcurvePatternPoints)
        foundCountsEta1Phi0fiducial77 = [0]*len(effcurvePatternPoints)
        foundCountsEta1p5Phi0fiducial77 = [0]*len(effcurvePatternPoints)

        # denominator in efficiency calculation in various phi and eta bins
        normalization = [0.0]*len(etaBins)
        normalizationPhi = [0.0]*len(phiBins)
        normalizationPhi0fiducial = 0.0
        normalizationEta1Phi0fiducial = 0.0
        normalizationEta1p5Phi0fiducial = 0.0
        bankSizePerRegion = []
        for i in effcurvePatternPoints:
            bankSizePerRegion.append(i*self.Geti('M'))

        # identify phi bounds for region X:
        phi0fiducialMin=phifiducialMin[r]
        phi0fiducialMax=phifiducialMax[r]

           
        ievt = 0
        while wrap.nextevent():
            if ievt>=self.Geti('nevents'):
                print 'Reached',ievt,'events. Done!'
                break
            if int(ievt)>=int(min(nentries)):
                print 'Ran out of tracks on event:',ievt
                break
            # Assume first true event is the one I want
            if wrap.nexttruth() < 0:
                ievt = ievt+1
                continue
            #print ievt,wrap

            # Check that we're in the fiducial region
            
            if (math.fabs(wrap.curv) > CURVLIM or 
                math.fabs(wrap.z0) > ZLIM or math.fabs(wrap.ctheta) > CTHETALIM or
                wrap.barcode == -1):
                ievt = ievt+1
                continue

            # whether the truth track lies in fiducial region
            hasTruthEta = [False]*len(etaBins)
            hasTruthPhi = [False]*len(phiBins)
            hasTruthPhi0Fiducial = False
            hasTruthEta1Phi0Fiducial = False
            hasTruthEta1p5Phi0Fiducial = False
            
            for k in xrange(len(etaBins)):
                if (k == 0 or  #Special case - full range
                    (math.fabs(wrap.ctheta) > math.sinh(etaBins[k-1]) and
                     math.fabs(wrap.ctheta) < math.sinh(etaBins[k])) ):
                    normalization[k] = normalization[k] + 1
                    hasTruthEta[k] = True
            for k in xrange(len(phiBins)):
                if (k == 0 or  #Special case - full range
                    (wrap.phi > phiBins[k-1] and
                     wrap.phi < phiBins[k]) ):
                    normalizationPhi[k] = normalizationPhi[k] + 1
                    hasTruthPhi[k] = True
            if (wrap.phi > phi0fiducialMin and wrap.phi < phi0fiducialMax):
                normalizationPhi0fiducial = normalizationPhi0fiducial + 1
                hasTruthPhi0Fiducial = True
                if (math.fabs(wrap.ctheta) < math.sinh(1.0)):
                    normalizationEta1Phi0fiducial += 1
                    hasTruthEta1Phi0Fiducial = True
                if (math.fabs(wrap.ctheta) > math.sinh(1.5)) and math.fabs(wrap.ctheta) < math.sinh(2.3):
                    normalizationEta1p5Phi0fiducial += 1
                    hasTruthEta1p5Phi0Fiducial = True

            # have we found a track?
            foundOne = []
            foundOnePhi = []
            for i in xrange(len(etaBins)):
                foundOne.append([False]*len(effcurvePatternPoints))
            for i in xrange(len(phiBins)):
                foundOnePhi.append([False]*len(effcurvePatternPoints))
            foundOnePhi0fiducial = [False]*len(effcurvePatternPoints)
            foundOneEta1Phi0fiducial = [False]*len(effcurvePatternPoints)
            foundOneEta1p5Phi0fiducial = [False]*len(effcurvePatternPoints)

            # have we found a 6/7 OR 7/7 road?
            foundOne67 = []
            foundOnePhi67 = []
            for i in xrange(len(etaBins)):
                foundOne67.append([False]*len(effcurvePatternPoints))
            for i in xrange(len(phiBins)):
                foundOnePhi67.append([False]*len(effcurvePatternPoints))
            foundOnePhi0fiducial67 = [False]*len(effcurvePatternPoints)
            foundOneEta1Phi0fiducial67 = [False]*len(effcurvePatternPoints)
            foundOneEta1p5Phi0fiducial67 = [False]*len(effcurvePatternPoints)

            # have we found a 7/7 road?
            foundOne77 = []
            foundOnePhi77 = []
            for i in xrange(len(etaBins)):
                foundOne77.append([False]*len(effcurvePatternPoints))
            for i in xrange(len(phiBins)):
                foundOnePhi77.append([False]*len(effcurvePatternPoints))
            foundOnePhi0fiducial77 = [False]*len(effcurvePatternPoints)
            foundOneEta1Phi0fiducial77 = [False]*len(effcurvePatternPoints)
            foundOneEta1p5Phi0fiducial77 = [False]*len(effcurvePatternPoints)

            for i,dummy in enumerate(effcurvePatternPoints):
                trackTree[i].GetEntry(ievt)
                # if road files are present, compute geometric pattern coverage
                if useRoads:
                    roadTree[i].GetEntry(ievt)
                    foundRoad67 = False
                    foundRoad77 = False
                    #nroads = roads.getNRoads()
                    for j in xrange(roads.getNRoads()):
                        rd = roads.getRoad(j)
                        nhits = rd.getNHits()
                        # coverage with only 7/7 (full) roads:
                        if nhits==self.Geti('L'):
                            foundRoad67 = True
                            foundRoad77 = True
                            break
                        # coverage with 6/7 || 7/7 roads
                        if nhits==self.Geti('L')-1:
                            foundRoad67 = True
                    if foundRoad67:
                        # Note that in coverage mode, we use truth phi/eta to determine the bins
                        for k in xrange(len(etaBins)):
                            if hasTruthEta[k]:
                                foundCounts67[k][i] += 1
                        for k in xrange(len(phiBins)):
                            if hasTruthPhi[k]:
                                foundCountsPhi67[k][i] += 1
                        if hasTruthPhi0Fiducial:
                            foundCountsPhi0fiducial67[i] += 1
                        if hasTruthEta1Phi0Fiducial:
                            foundCountsEta1Phi0fiducial67[i] += 1
                        if hasTruthEta1p5Phi0Fiducial:
                            foundCountsEta1p5Phi0fiducial67[i] += 1
                    if foundRoad77:
                        # Note that in coverage mode, we use truth phi/eta to determine the bins
                        for k in xrange(len(etaBins)):
                            if hasTruthEta[k]:
                                foundCounts77[k][i] += 1
                        for k in xrange(len(phiBins)):
                            if hasTruthPhi[k]:
                                foundCountsPhi77[k][i] += 1
                        if hasTruthPhi0Fiducial:
                            foundCountsPhi0fiducial77[i] += 1
                        if hasTruthEta1Phi0Fiducial:
                            foundCountsEta1Phi0fiducial77[i] += 1
                        if hasTruthEta1p5Phi0Fiducial:
                            foundCountsEta1p5Phi0fiducial77[i] += 1
                    # end loop over roads
                # if tracks are present (aka always), compute full efficiency for final tracks
                if True:
                    for j in xrange(tracks.getNTracks()):
                        aTrk = tracks.getTrack(j)
                        # Make sure the rtt barcode matches and that the
                        # fraction is above 0.7
                        if (aTrk.getBarcode() != wrap.barcode or aTrk.getBarcodeFrac() < 0.7):
                            continue
                        for k in xrange(len(etaBins)):
                            if (k == 0 or  #Special case - full range
                                (math.fabs(aTrk.getCotTheta()) > math.sinh(etaBins[k-1]) and
                                 math.fabs(aTrk.getCotTheta()) < math.sinh(etaBins[k])) ):
                                if not foundOne[k][i]:
                                    foundCounts[k][i] += 1
                                    foundOne[k][i] = True
                        for k in xrange(len(phiBins)):
                            if (k == 0 or  #Special case - full range
                                (aTrk.getPhi() > phiBins[k-1] and
                                 aTrk.getPhi() < phiBins[k]) ):
                                if not foundOnePhi[k][i]:
                                    foundCountsPhi[k][i] += 1
                                    foundOnePhi[k][i] = True
                        if (aTrk.getPhi() > phi0fiducialMin and aTrk.getPhi() < phi0fiducialMax):
                            if not foundOnePhi0fiducial[i]:
                                foundCountsPhi0fiducial[i] += 1
                                foundOnePhi0fiducial[i] = True                        
                            if (math.fabs(aTrk.getCotTheta()) < math.sinh(1.00)):    
                                if not foundOneEta1Phi0fiducial[i]:
                                    foundCountsEta1Phi0fiducial[i] += 1
                                    foundOneEta1Phi0fiducial[i] = True
                            if math.fabs(aTrk.getCotTheta()) > math.sinh(1.5) and math.fabs(aTrk.getCotTheta()) < math.sinh(2.3):
                                if not foundOneEta1p5Phi0fiducial[i]:
                                    foundCountsEta1p5Phi0fiducial[i] += 1
                                    foundOneEta1p5Phi0fiducial[i] = True                        
                    # End loop over tracks
            # End loop over npoints
            ievt = ievt+1
        # End loop over true events

        ###############################################################################
        # Print the summary
        ###############################################################################
        try:
            fout=open(self.stdout,"w")
        except:
            fout=open("/dev/stdout","a")
        print 'Saving effcurve summary under',fout.name
        print >>fout,"Efficiency summary, %d events:"%ievt
        print >>fout,"npatt", effcurvePatternPoints
        for i in xrange(len(etaBins)):
            if i == 0:
                print >>fout,"|eta| < %.1f: "%etaBins[len(etaBins)-1],
            else:
                print >>fout,"%.1f < |eta| < %.1f: "%(etaBins[i-1],etaBins[i]),
            print >>fout,"\t",
            for f in foundCounts[i] :
                print >>fout, f,
            print
            if useRoads:
                print >>fout,"\t",
                for f in foundCounts67[i] :
                    print >>fout, f,
                print 
                print >>fout,"\t",
                for f in foundCounts77[i]:
                    print >>fout, f,
                print
            print >>fout,"\tNormalization",normalization[i]
            if normalization[i] > 0.0:
                for j in xrange(len(effcurvePatternPoints)):
                    foundCounts[i][j] = float(foundCounts[i][j])/normalization[i]
                    if useRoads:
                        foundCounts67[i][j] = float(foundCounts67[i][j])/normalization[i]
                        foundCounts77[i][j] = float(foundCounts77[i][j])/normalization[i]
                print >>fout,"\tEff:",
                for f in foundCounts[i] :
                    print >>fout, "%.2f"%f,
                print
                if useRoads:
                    print >>fout,"\tCov67:",
                    for f in foundCounts67[i] :
                        print >>fout, "%.2f"%f,
                    print
                    print >>fout,"\tCov77:",
                    for f in foundCounts77[i] :
                        print >>fout, "%.2f"%f,
                    print
        for i in xrange(len(phiBins)):
            if i == 0:
                print >>fout,"%.1f < phi < %.1f: "%(phiBins[0],phiBins[len(phiBins)-1])
            else:
                print >>fout,"%.1f < phi < %.1f: "%(phiBins[i-1],phiBins[i]),
            print >>fout,"\t",foundCountsPhi[i]
            if useRoads:
                print >>fout,"\t",foundCountsPhi67[i]
                print >>fout,"\t",foundCountsPhi77[i]
            print >>fout,"\tNormalization",normalizationPhi[i]
            if normalizationPhi[i] > 0.0:
                for j in xrange(len(effcurvePatternPoints)):
                    foundCountsPhi[i][j] = float(foundCountsPhi[i][j])/normalizationPhi[i]
                    if useRoads:
                        foundCountsPhi67[i][j] = float(foundCountsPhi67[i][j])/normalizationPhi[i]
                        foundCountsPhi77[i][j] = float(foundCountsPhi77[i][j])/normalizationPhi[i]
                print >>fout,"\tEff ",
                for f in foundCountsPhi[i] :
                    print >>fout, "%.2f, " % f,
                print >>fout
                if useRoads:
                    print >>fout,"\tCov67 ",
                    for f in foundCountsPhi67[i] :
                        print >>fout, "%.2f, " % f,
                    print >>fout
                    print >>fout,"\tCov77 ", 
                    for f in foundCountsPhi77[i] :
                        print >>fout, "%.2f, " % f,
                    print >>fout

        print >>fout,"Phi%s fiducial in region%s %.1f < phi < %.1f: "%(r,r,phi0fiducialMin,phi0fiducialMax)
        print >>fout,"\t",foundCountsPhi0fiducial
        if useRoads:
            print >>fout,"\t",foundCountsPhi0fiducial67
            print >>fout,"\t",foundCountsPhi0fiducial77
        print >>fout,"\tNormalization",normalizationPhi0fiducial
        if normalizationPhi0fiducial > 0.0:
            for j in xrange(len(effcurvePatternPoints)):
                foundCountsPhi0fiducial[j] = float(foundCountsPhi0fiducial[j])/normalizationPhi0fiducial
                if useRoads:
                    foundCountsPhi0fiducial67[j] = float(foundCountsPhi0fiducial67[j])/normalizationPhi0fiducial
                    foundCountsPhi0fiducial77[j] = float(foundCountsPhi0fiducial77[j])/normalizationPhi0fiducial
            print >>fout,"\tEff ",
            for f in foundCountsPhi0fiducial :
                print >>fout, "%.3f, " % f,
            print >>fout
            if useRoads:
                print >>fout,"\tCov67 ",
                for f in foundCountsPhi0fiducial67 :
                    print >>fout, "%.3f" % f,
                print >>fout
                print >>fout,"\tCov77 ",
                for f in foundCountsPhi0fiducial77 :
                    print >>fout, "%.3f" % f,
                print >>fout                    

        print >>fout,"Eta1Phi%s fiducial in region%s %.1f < phi < %.1f (|eta|<1): "%(r,r,phi0fiducialMin,phi0fiducialMax)
        print >>fout,"\t",foundCountsEta1Phi0fiducial
        if useRoads:
            print >>fout,"\t",foundCountsEta1Phi0fiducial67
            print >>fout,"\t",foundCountsEta1Phi0fiducial77
        print >>fout,"\tNormalization",normalizationEta1Phi0fiducial
        if normalizationEta1Phi0fiducial > 0.0:
            for j in xrange(len(effcurvePatternPoints)):
                foundCountsEta1Phi0fiducial[j] = float(foundCountsEta1Phi0fiducial[j])/normalizationEta1Phi0fiducial
                if useRoads:
                    foundCountsEta1Phi0fiducial67[j] = float(foundCountsEta1Phi0fiducial67[j])/normalizationEta1Phi0fiducial
                    foundCountsEta1Phi0fiducial77[j] = float(foundCountsEta1Phi0fiducial77[j])/normalizationEta1Phi0fiducial
            print >>fout,"\tEff ",
            for f in foundCountsEta1Phi0fiducial :
                print >>fout, "%.3f, " % f,
            print >>fout                    
            if useRoads:
                print >>fout,"\tCov67 ",
                for f in foundCountsEta1Phi0fiducial67 :
                    print >>fout, "%.3f, " % f,
                print >>fout                    
                print >>fout,"\tCov77 ",
                for f in foundCountsEta1Phi0fiducial77 :
                    print >>fout, "%.3f, " % f,
                print >>fout                    


        print >>fout,"Eta1p5Phi%s fiducial in region%s %.1f < phi < %.1f (|eta|>1.5): "%(r,r,phi0fiducialMin,phi0fiducialMax)
        print >>fout,"\t",foundCountsEta1p5Phi0fiducial
        if useRoads:
            print >>fout,"\t",foundCountsEta1p5Phi0fiducial67
            print >>fout,"\t",foundCountsEta1p5Phi0fiducial77
        print >>fout,"\tNormalization",normalizationEta1p5Phi0fiducial
        if normalizationEta1p5Phi0fiducial > 0.0:
            for j in xrange(len(effcurvePatternPoints)):
                foundCountsEta1p5Phi0fiducial[j] = float(foundCountsEta1p5Phi0fiducial[j])/normalizationEta1p5Phi0fiducial
                if useRoads:
                    foundCountsEta1p5Phi0fiducial67[j] = float(foundCountsEta1p5Phi0fiducial67[j])/normalizationEta1p5Phi0fiducial
                    foundCountsEta1p5Phi0fiducial77[j] = float(foundCountsEta1p5Phi0fiducial77[j])/normalizationEta1p5Phi0fiducial
            print >>fout,"\tEff ",
            for f in foundCountsEta1p5Phi0fiducial :
                print >>fout, "%.3f, " % f,
            print >>fout                    
            if useRoads:
                print >>fout,"\tCov67 ",
                for f in foundCountsEta1p5Phi0fiducial67 :
                    print >>fout, "%.3f, " % f,
                print >>fout                                        
                print >>fout,"\tCov77 ",
                for f in foundCountsEta1p5Phi0fiducial77 :
                    print >>fout, "%.3f, " % f,
                print >>fout                                        
        fout.close()
        ###############################################################################
        # Draw the plots, and make them reasonably pretty
        ###############################################################################
        def makeCanvas(label):
            ROOT.gStyle.SetCanvasColor(0);
            ROOT.gStyle.SetCanvasBorderMode(0);
            ROOT.gStyle.SetPadColor(0);
            ROOT.gStyle.SetPadBorderMode(0);
            ROOT.gStyle.SetStatColor(0);
            
            ROOT.gStyle.SetOptStat(111111);
            ROOT.gStyle.SetOptFit(1111);
            ROOT.gStyle.SetHistFillColor(0);
            ROOT.gStyle.SetMarkerStyle(20);
            ROOT.gStyle.SetMarkerSize(.4);
            ROOT.gStyle.SetHistLineWidth(2);
            ROOT.gStyle.SetErrorX(0);
            
            ROOT.gStyle.SetTitleStyle(0);
            
            ROOT.gStyle.SetStatBorderSize(1);
            ROOT.gStyle.SetFrameFillColor(10);
            ROOT.gStyle.SetTitleFillColor(0);
            
            c = ROOT.TCanvas(label,label,1024,768);
            c.cd();
            ROOT.gPad.SetGridx();  ROOT.gPad.SetGridy();
            return c
        def makeGraph(title,mode,n,x,y):
            "mode=0 is efficiency, otherwise coverage"
            color=ROOT.kBlue
            if mode != 0:
                color = ROOT.kRed
            g = ROOT.TGraph(n,x,y);
            g.SetLineColor(color);
            g.SetLineWidth(3);
            g.SetMarkerColor(color);
            g.SetMarkerSize(2.0);
            g.SetTitle(title);
            if mode==0:
                g.Draw("CA*")
            else:
                g.Draw("C*sames")
            g.GetHistogram().SetXTitle("Bank size (per region)");
            g.GetYaxis().SetRangeUser(0.0,1.0);
            return g
        def makeLegend(g_cov,g_eff):
            leg = ROOT.TLegend(0.3,0.3,0.6,0.45);
            leg.SetFillColor(0);
            leg.AddEntry(g_cov,"Coverage","l");
            leg.AddEntry(g_eff,"Efficiency","l");
            leg.Draw();
            return leg
        import array
        for i in xrange(len(etaBins)):
            if normalization[i] > 0.0:
                if i == 0:
                    label="|#eta| < %.1f: "%etaBins[len(etaBins)-1]
                    flabel="reg%seta_%.1f"%(r,etaBins[len(etaBins)-1])
                else:
                    label="%.1f < |#eta| < %.1f: "%(etaBins[i-1],etaBins[i])
                    flabel="reg%seta_%.1f_%.1f"%(r,etaBins[i-1],etaBins[i])
                c = makeCanvas(label)
                x = array.array('f',bankSizePerRegion)
                y = array.array('f',foundCounts[i])
                y67 = array.array('f',foundCounts67[i])
                g1 = makeGraph("Coverage and Efficiency (%s)"%label,0,len(x),x,y)
                if useRoads:
                    g2 = makeGraph("Coverage and Efficiency (%s)"%label,1,len(x),x,y67)
                    l = makeLegend(g2,g1)
                c.SaveAs(os.path.join(self.Get('OUT_DIR'),"eff%s.png"%flabel))
                c.SaveAs(os.path.join(self.Get('OUT_DIR'),"eff%s.ps"%flabel))
        for i in xrange(len(phiBins)):
            if normalizationPhi[i] > 0.0:
                if i == 0:
                    label="%.1f < #phi < %.1f: "%(phiBins[0],phiBins[len(phiBins)-1])
                    flabel="reg%sphi_%.1f_%.1f"%(r,phiBins[0],phiBins[len(phiBins)-1])
                else:
                    label="%.1f < #phi < %.1f: "%(phiBins[i-1],phiBins[i])
                    flabel="reg%sphi_%.1f_%.1f"%(r,phiBins[i-1],phiBins[i])
                c = makeCanvas(label)
                x = array.array('f',bankSizePerRegion)
                y = array.array('f',foundCountsPhi[i])
                y67 = array.array('f',foundCountsPhi67[i])
                g1 = makeGraph("Coverage and Efficiency (%s)"%label,0,len(x),x,y)
                if useRoads:
                    g2 = makeGraph("Coverage and Efficiency (%s)"%label,1,len(x),x,y67)
                    l = makeLegend(g2,g1)
                c.SaveAs(os.path.join(self.Get('OUT_DIR'),"eff%s.png"%flabel))
                c.SaveAs(os.path.join(self.Get('OUT_DIR'),"eff%s.ps"%flabel))

        if normalizationPhi0fiducial > 0.0:
            label="Phi%s fiducial in region%s %.1f < #phi < %.1f: "%(r,r,phi0fiducialMin,phi0fiducialMax)
            flabel="phi%sfid_%.1f_%.1f"%(r,phi0fiducialMin,phi0fiducialMax)
            c = makeCanvas(label)
            x = array.array('f',bankSizePerRegion)
            y = array.array('f',foundCountsPhi0fiducial)
            y67 = array.array('f',foundCountsPhi0fiducial67)
            g1 = makeGraph("Coverage and Efficiency (|#eta|<2.5)",0,len(x),x,y)
            if useRoads:
                g2 = makeGraph("Coverage and Efficiency (|#eta|<2.5)",1,len(x),x,y67)
                l = makeLegend(g2,g1)
            c.SaveAs(os.path.join(self.Get('OUT_DIR'),"eff%s.png"%flabel))
            c.SaveAs(os.path.join(self.Get('OUT_DIR'),"eff%s.ps"%flabel))
        if normalizationEta1Phi0fiducial > 0.0:
            label="Central eta; Phi%s fiducial in region%s %.1f < #phi < %.1f: "%(r,r,phi0fiducialMin,phi0fiducialMax)
            flabel="CentralEta1p0Phi%sfid_%.1f_%.1f"%(r,phi0fiducialMin,phi0fiducialMax)
            c = makeCanvas(label)
            x = array.array('f',bankSizePerRegion)
            y = array.array('f',foundCountsEta1Phi0fiducial)
            y67 = array.array('f',foundCountsEta1Phi0fiducial67)
            g1 = makeGraph("Coverage and Efficiency (|#eta|<1)",0,len(x),x,y)
            if useRoads:
                g2 = makeGraph("Coverage and Efficiency (|#eta|<1)",1,len(x),x,y67)
                l = makeLegend(g2,g1)
            c.SaveAs(os.path.join(self.Get('OUT_DIR'),"eff%s.png"%flabel))
            c.SaveAs(os.path.join(self.Get('OUT_DIR'),"eff%s.ps"%flabel))
        if normalizationEta1p5Phi0fiducial > 0.0:
            label="Forward eta; Phi%s fiducial in region%s %.1f < #phi < %.1f: "%(r,r,phi0fiducialMin,phi0fiducialMax)
            flabel="ForwardEta1p5to2p3Phi%sfid_%.1f_%.1f"%(r,phi0fiducialMin,phi0fiducialMax)
            c = makeCanvas(label)
            x = array.array('f',bankSizePerRegion)
            y = array.array('f',foundCountsEta1p5Phi0fiducial)
            y67 = array.array('f',foundCountsEta1p5Phi0fiducial67)
            g1 = makeGraph("Coverage and Efficiency (1.5<|#eta|<2.3)",0,len(x),x,y)
            if useRoads:
                g2 = makeGraph("Coverage and Efficiency (1.5<|#eta|<2.3)",1,len(x),x,y67)
                l = makeLegend(g2,g1)
            c.SaveAs(os.path.join(self.Get('OUT_DIR'),"eff%s.png"%flabel))
            c.SaveAs(os.path.join(self.Get('OUT_DIR'),"eff%s.ps"%flabel))
        print 'Saved plots under',self.Get('OUT_DIR')
        # set a warning flag so that this job is not processed further
        self.WARN(1,"EffCurve job succeeded")
        return 'Done!'

class SummaryJob(BaseJob):
    """ A job that summarizes the status of current run """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label=self.LabelMod('summary')
        label_short=label
        self.Add('run_name',self.RunName(label_short,0))
        self.Add('run_path',self.RunPath(label,0))
        self.executable='none'
        self.shell = False
        BaseJob.Initialize(self)
    def RunTimeFromLog(self,fin):
        """ Parses the log file and returns # of minutes it took the jobs """
        try:
            f = open(fin,"r")
        except:
            return -1
        ret=0
        for l in f:
            l.strip()
            if l.find('real')!=-1 and l.find('m')!=-1:
                try:
                    l=str(l.split('real')[1])
                    mins=str(l.split('m')[0])
                    secs=l.split('m')[1].split('s')[0]
                    ret=int(mins)+float(secs)/60.0
                except:
                    pass
        return ret
    def ProcessTemplate(self,template='irrelevant'):
        """ summary of the run """
        for i in xrange(self.Geti('N')):
            print 'Region %d:'%i
            # roads:
            print '-- Road files:',
            totruntime=0
            for j in xrange(self.Geti('M')):
                roadfiles=glob.glob(os.path.join(self.OutputPath(_ROADS,2)%(i,j),'*.root'))
                logfile=self.LatestFile(self.RunPath(_ROADS,2)%(i,j))
                runtime=self.RunTimeFromLog(logfile)
                totruntime+=runtime
                print len(roadfiles),
            if totruntime>=0:
                print '\t\t| TIME = %s*%.2f min'%(self.Get('M'),totruntime*1.0/self.Geti('M')),
            print ''
            # merge:
            if self.Get('MERGED_ROADS')=='1':
                mergefiles=glob.glob(os.path.join(self.OutputPath(self.LabelMod(_MERGED))%i,'*.root'))
                logfile=self.LatestFile(self.RunPath(self.LabelMod(_MERGED))%i)
                runtime=self.RunTimeFromLog(logfile)
                print '-- Merged files:',len(mergefiles),
                if runtime>=0:
                    print '\t\t| TIME =',str(runtime)+'min',
                print ''
            # tracks (if not merged)
            print '-- Track files:',
            totruntime=0
            subloop=range(self.Geti('M'))
            if self.Get('MERGED_ROADS')=='1':
                subloop=[None,]
            for j in subloop:
                if j==None:
                    trackfiles=glob.glob(os.path.join(self.OutputPath(self.LabelMod(_TRACKS))%i,'*.root'))
                    logfile=self.LatestFile(self.RunPath(self.LabelMod(_TRACKS))%i)
                else:
                    trackfiles=glob.glob(os.path.join(self.OutputPath(self.LabelMod(_TRACKS),2)%(i,j),'*.root'))
                    logfile=self.LatestFile(self.RunPath(self.LabelMod(_TRACKS),2)%(i,j))
                runtime=self.RunTimeFromLog(logfile)
                totruntime+=runtime
                print len(trackfiles),
            if totruntime>=0:
                print '\t\t| TIME = %s*%.2f min'%(self.Get('M'),totruntime*1.0/self.Geti('M')),
            print ''
        # tracks (merged)
        if self.Get('MERGED_TRACKS')=='1':
            print 'MERGED TRACKS:'
            mergetrackfiles=glob.glob(os.path.join(self.OutputPath(self.LabelMod(_TRACKS,doMrg=True))%self.Geti('N'),'*.root'))
            logfile=self.LatestFile(self.RunPath(self.LabelMod(_TRACKS,doMrg=True))%self.Geti('N'))
            runtime=self.RunTimeFromLog(logfile)
            print '-- Merged track files:',len(mergetrackfiles),
            if runtime>=0:
                print '\t\t| TIME = %.2f min'%runtime,
            print ''
        print 'ANALYSIS JOB:'
        logfile=self.LatestFile(self.RunPath(self.LabelMod(_ANALYSIS,doMrg=(self.Get('MERGED_TRACKS')=='1')))%self.Geti('N'))
        runtime=self.RunTimeFromLog(logfile)
        print '-- Analysis job time:',
        if runtime>=0:
            print '\t\t| TIME = %.2f min'%runtime,
        print ''
        # set a warning flag so that this job is not processed further
        self.WARN(1,"Summary job succeeded")
        return 'Done!'

class ListJob(BaseJob):
    """ Lists all available runs in the output directory """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label=self.LabelMod('list')
        label_short=label
        self.Add('run_name',self.RunName(label_short,0))
        self.Add('run_path',self.RunPath(label,0))
        self.executable='none'
        self.shell = False
        BaseJob.Initialize(self)
    def ProcessTemplate(self,template='irrelevant'):
        """ summary of all available runs """
        outdir=os.path.join(self.Get('outdir'),'*')
        runs=glob.glob(outdir)
        print 'Available runstrings:'
        for l in runs:
            l=os.path.basename(l)
            if l not in ('run','other','garbage'):
                print '--',l
        # set a warning flag so that this job is not processed further
        self.WARN(1,"List job succeeded")
        return 'Done!'

class ShellJob(BaseJob):
    """ Executes a list of shell commands """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
        self.cmds = []
        self.noexec = False
        self.failed = False
    def append(self, cmd):
        self.cmds.append(cmd)
    def clear(self):
        self.cmds = []
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label=self.LabelMod('shell')
        label_short=label
        self.Add('run_name',self.RunName(label_short,0))
        self.Add('run_path',self.RunPath(label,0))
        self.executable='none'
        self.shell = False
        BaseJob.Initialize(self)
    def ProcessTemplate(self,template='irrelevant'):
        """ execute shell commands """
        self.Exec(self.cmds,self.noexec)
        # set a warning flag so that this job is not processed further
        self.WARN(1,"Shell job succeeded")
        return 'Done!'

class MakelistsJob(BaseJob):
    """ Makes 8L TRACKLIST and ROADLIST for SCTtrk mode """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label='makelists'
        label_short=label
        if self.Get('MERGED_ROADS')=='1':
            self.Add('run_name',self.RunName(label_short)%self.Get('i'))
            self.Add('run_path',self.RunPath(label)%self.Get('i'))
        else:
            self.Add('run_name',self.RunName(label_short,2)%(self.Get('i'),self.Get('j')))
            self.Add('run_path',self.RunPath(label,2)%(self.Get('i'),self.Get('j')))
        self.executable='none'
        self.shell = False
        self.noexec = False
        BaseJob.Initialize(self)
    def ProcessTemplate(self,template='irrelevant'):
        """ actual application logic """
        i=self.Geti('i')
        j=self.Geti('j')
        flist=os.path.join(self.Get('inputdir'),'inputprod_raw_'+self.Get('dataset')+'.list')
        if self.Get('FTKDAT_LIST')!=None:
            flist = self.Get('FTKDAT_LIST')
        f = open(flist,'r')
        maxevts = self.Geti('nevents')
        totevts = 0
        if self.Get('MERGED_ROADS')=='1' and False:
            SCTTRK_TRACKLIST = os.path.join(self.Get('otherdir'),self.Get('dataset')+'_tracks_reg'+self.Get('i')+'.list')
            SCTTRK_ROADLIST = os.path.join(self.Get('otherdir'),self.Get('dataset')+'_roads_reg'+self.Get('i')+'.list')
        else:
            SCTTRK_TRACKLIST = os.path.join(self.Get('otherdir'),self.Get('dataset')+'_tracks_reg'+self.Get('i')+'_sub'+self.Get('j')+'.list')
            SCTTRK_ROADLIST = os.path.join(self.Get('otherdir'),self.Get('dataset')+'_roads_reg'+self.Get('i')+'_sub'+self.Get('j')+'.list')
        try:
            fSCTTRK_TRACKLIST = open(SCTTRK_TRACKLIST,'w')
            fSCTTRK_ROADLIST = open(SCTTRK_ROADLIST,'w')
        except:
            self.WARN(3,"Couldn't open: "+SCTTRK_TRACKLIST+","+SCTTRK_ROADLIST)
        nadded=0
        for l in f:
            W=l.strip()
            if not os.path.isfile(W):
                self.WARN(3,'Failed to find input file: '+W)
            T = self.OutputFile(self.LabelMod(_TRACKS),'ftktracks_',W)%i
            if self.Get('MERGED_ROADS')!='1':
                T = self.OutputFile(self.LabelMod(_TRACKS),'ftktracks_',W,2)%(i,j)
            if not os.path.isfile(T):
                break
            R = self.OutputFile(self.LabelMod(_MERGED),'ftkroads_',W)%i
            if self.Get('MERGED_ROADS')!='1':
                R = self.OutputFile(self.LabelMod(_ROADS),'ftkroads_',W,2)%(i,j)
            if not os.path.isfile(R):
                self.WARN(3,'Failed to find road file: '+R)
            print >>fSCTTRK_TRACKLIST, T
            print >>fSCTTRK_ROADLIST, R
            nadded+=1
        print 'Added',nadded,'roads to:',SCTTRK_ROADLIST
        print 'Added',nadded,'tracks to:',SCTTRK_TRACKLIST
        fSCTTRK_TRACKLIST.close()
        fSCTTRK_ROADLIST.close()
        # set a warning flag so that this job is not processed further
        self.WARN(1,"Shell job succeeded")
        return 'Done!'

############################################################################
# Jobs specific for bank production
############################################################################
class SectorJob(BaseJob):
    """ pattgen in sector mode (produces sectors in a particular region) """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        # !!! modified for new trklookup: only make sectors in particular region
        self.Add('run_name',self.RunName('sectors',1)%self.Get('i'))
        self.Add('run_path',self.RunPath('sectors',1)%self.Get('i'))
        self.Add('SECTOR_MODE',2)
        self.Add('REALDATA',1)
        self.Add('WEDGE_MIN',self.Get('i'))
        self.Add('WEDGE_MAX',self.Get('i'))
        if self.Geti('MAXPATTERNS')==-1: self.Add('MAXPATTERNS',1000000000)
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.Get('patdir'))
        self.executable=self.Get('pattgen')
        BaseJob.Initialize(self)
    def function_AddPatternBanks(self):
        """ Constructs pattern banks """
        result = []
        for i in xrange(self.Geti("N")):
            # new trklookup: only save a bank in current region!
            patfile ="%s/%s_%s_reg%s.%s"%(self.Get('garbagedir'),'sectorbankgarbage',self.Get('run_name'),i,'patt')
            if i==self.Geti('i'):
                patfile = self.SectorFile(False)%i
            if not os.path.exists(os.path.dirname(patfile)): os.makedirs(os.path.dirname(patfile))
            result.append('PATT_FILE%d %s'%(i,patfile) )
        result.append('')
        result.append('PATTERN_banks %d'%self.Geti('N'))
        result.append('%d AM banks'%self.Geti('N'))
        for i in xrange(self.Geti("N")):
            patfile ="%s/%s_%s_reg%s.%s"%(self.Get('garbagedir'),'sectorgarbage',self.Get('run_name'),i,'patt')
            if not os.path.exists(os.path.dirname(patfile)): os.makedirs(os.path.dirname(patfile))
            result.append(patfile)
        result=[r.split() for r in result]
        result.insert(0,['# Filenames for the %d sector files'%self.Geti('N'),])
        result.insert(1,['#FTKNOCHECK'])
        result.append(['#FTKCHECK'])
        return result
    def ProcessTemplate(self,template=''):
        """ Adds pattern bank information to roadfinder configuration """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'pattgen.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddPatternBanks()
        self.templateC = result
        return self.templateC

class AllSectorJob(BaseJob):
    """ pattgen in sector mode (produces sectors in all regions in one job) """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        self.Add('run_name',self.RunName('sectors',0))
        self.Add('run_path',self.RunPath('sectors',0))
        self.Add('SECTOR_MODE',2)
        self.Add('REALDATA',1)
        self.Add('WEDGE_MIN',0)
        self.Add('WEDGE_MAX',self.Geti('Nm1'))
        if self.Geti('MAXPATTERNS')==-1: self.Add('MAXPATTERNS',1000000000)
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.Get('patdir'))
        self.executable=self.Get('pattgen')
        BaseJob.Initialize(self)
    def function_AddPatternBanks(self):
        """ Constructs pattern banks """
        result = []
        for i in xrange(self.Geti("N")):
            patfile = self.SectorFile(False)%i
            if not os.path.exists(os.path.dirname(patfile)): os.makedirs(os.path.dirname(patfile))
            result.append('PATT_FILE%d %s'%(i,patfile) )
        result.append('')
        result.append('PATTERN_banks %d'%self.Geti('N'))
        result.append('%d AM banks'%self.Geti('N'))
        for i in xrange(self.Geti("N")):
            patfile ="%s/%s_%s_reg%s.%s"%(self.Get('garbagedir'),'sectorgarbage',self.Get('run_name'),i,'patt')
            if not os.path.exists(os.path.dirname(patfile)): os.makedirs(os.path.dirname(patfile))
            result.append(patfile)
        result=[r.split() for r in result]
        result.insert(0,['# Filenames for the %d sector files'%self.Geti('N'),])
        result.insert(1,['#FTKNOCHECK'])
        result.append(['#FTKCHECK'])
        return result
    def ProcessTemplate(self,template=''):
        """ Adds pattern bank information to roadfinder configuration """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'pattgen.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddPatternBanks()
        self.templateC = result
        return self.templateC

class ConvertLookupJob(BaseJob):
    """ Converts root-based region lookup file (made via -o MAKE_REGLOOKUP -a allsectors)
        to a plain-text format. This is needed to split input muons into per-region blocks """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        self.Add('run_name',self.RunName('lookup',0))
        self.Add('run_path',self.RunPath('lookup',0))
        self.executable='none'
        self.shell = False
        self.noexec = False
        BaseJob.Initialize(self)
    def ProcessTemplate(self,template='irrelevant'):
        """ actual application logic """
        cmds = []
        old = self.RegionLookupFile(ext='root')
        new = self.RegionLookupFile(ext='dat')
        if not (os.path.isfile(old) and os.stat(old).st_size>0):
            self.WARN(3,"Can't locate the ROOT region lookup file:",old)
        if os.path.exists(new):
            print 'WARNING: deleting existing converted region lookup (%s)'%new
            cmds.append('rm -f '+new)
        cmds.append('%s %s %s'%(self.Get('convert_lookup'),old,new))
        self.Exec(cmds,self.noexec)
        if not (os.path.isfile(new) and os.stat(new).st_size>0):
            self.WARN(3,"ConvertLookup job failed: output 'dat' region lookup file is missing or empty:",new)
        # set a warning flag so that this job is not processed further
        self.WARN(1,"ConvertLookup job succeeded")
        return 'Done!'

class FilterJob(BaseJob):
    """ Filters low-statistics sectors """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label='filter'
        label_short=label
        self.Add('run_name',self.RunName(label_short,1)%self.Get('i'))
        self.Add('run_path',self.RunPath(label,1)%self.Get('i'))
        self.executable='none'
        self.shell = False
        self.noexec = False
        BaseJob.Initialize(self)
    def ProcessTemplate(self,template='irrelevant'):
        """ actual application logic """
        L=self.Geti('L')
        i=self.Geti('i')
        f = self.SectorFile(False)%i
        latest = self.EarliestFile(f+'.orig.*')
        if latest!=None:
            print 'It appears that filter was already run. Original backup:',latest
        bp = self.SectorFile(False)%i+'.orig.'+self.Timestamp()
        cmds = []
        cmds.append('mv ' + f + ' ' + bp)
        cmds.append("awk 'BEGIN{out[0]=0;tot=0;};$"+str(L+3)+">="+str(self.nfilt)+"{out[tot]=$0;tot++;}; END{print tot,"+str(L)+"; for(i=0;i<tot;i++) print out[i];}' "+ bp + ' > ' + f)
        self.Exec(cmds,self.noexec)
        # set a warning flag so that this job is not processed further
        self.WARN(1,"Filter job succeeded")
        return 'Done!'

class SecstatJob(BaseJob):
    """ pattgen in sector mode (computes sector efficiency after filtering) """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        self.Add('run_name',self.RunName('secstat',1)%self.Get('i'))
        self.Add('run_path',self.RunPath('secstat',1)%self.Get('i'))
        self.Add('SECTOR_MODE',2)
        self.Add('eff_FLAG',1) # turn on eff mode!
        self.Add('REALDATA',1)
        self.Add('ntracks',100000)
        if self.Get('BARREL_ONLY')=='1':
            self.Add('etaMAXlim',1)
        self.Add('WEDGE_MIN',0)
        self.Add('WEDGE_MAX',self.Geti('Nm1'))
        if self.Geti('MAXPATTERNS')==-1: self.Add('MAXPATTERNS',10000000)
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.Get('patdir'))
        self.executable=self.Get('pattgen')
        BaseJob.Initialize(self)
    def function_AddPatternBanks(self):
        """ Constructs pattern banks """
        print 'NOT IMPLEMENTED YET'
        sys.exit(3)
        result = []
        for i in xrange(self.Geti("N")):
            patfile = os.path.join(self.Get('garbagedir'),'secstat_reg%s.patt'%self.Get('i'))
            result.append('PATT_FILE%d %s'%(i,patfile) )
        result.append('')
        result.append('PATTERN_banks %d'%self.Geti('N'))
        result.append('%d AM banks'%self.Geti('N'))
        for i in xrange(self.Geti("N")):
            patfile ="%s/%s_%s.%s"%(self.Get('garbagedir'),'sectorgarbage',self.Get('run_name'),'patt')
            if not os.path.exists(os.path.dirname(patfile)): os.makedirs(os.path.dirname(patfile))
            result.append(patfile)
        result=[r.split() for r in result]
        result.insert(0,['# Filenames for the %d sector files'%self.Geti('N'),])
        result.insert(1,['#FTKNOCHECK'])
        result.append(['#FTKCHECK'])
        return result
    def ProcessTemplate(self,template=''):
        """ Adds pattern bank information to roadfinder configuration """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'pattgen.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddPatternBanks()
        self.templateC = result
        return self.templateC

class CorrgenJob(BaseJob):
    """ corrgen jobs (produces gcon constants) """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        self.Add('run_name',self.RunName('corrgen',1)%self.Get('i'))
        self.Add('run_path',self.RunPath('corrgen',1)%self.Get('i'))
        self.Add('PE2S_FILE',self.SectorFile(False)%self.Get('i'))
        self.Add('GCON_FILE',self.ConstantFile(False)%self.Get('i'))
        self.Add('SLICE_FILE',self.SliceFile()%self.Get('i'))
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.Get('gcondir'))
        self.executable=self.Get('corrgen')
        BaseJob.Initialize(self)
    def ProcessTemplate(self,template=''):
        """ Adds pattern bank information to roadfinder configuration """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'corrgen.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        self.templateC = result
        return self.templateC

class CleanupJob(BaseJob):
    """ Cleanup sectors,constants,slices after a corrgen run """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label='cleanup'
        label_short=label
        self.Add('run_name',self.RunName(label_short,1)%self.Get('i'))
        self.Add('run_path',self.RunPath(label,1)%self.Get('i'))
        self.Add('patt_type','pattgen') # only cleanup pattgen patterns
        self.executable='none'
        self.shell = False
        self.noexec = False
        BaseJob.Initialize(self)
    def ProcessTemplate(self,template='irrelevant'):
        """ actual application logic """
        L=self.Geti('L')
        i=self.Geti('i')
        sectors = self.SectorFile(False)%i
        badsectors = self.BadSectorFile()%i
        totsectors = self.TotSectorFile(False)%i
        constants = self.ConstantFile(False)%i
        slices = self.SliceFile()%i
        timestamp = self.Timestamp()
        def check_latest(name):
            latest = self.EarliestFile(name+'.uncleaned.*')
            if latest!=None:
                print 'It appears that cleanup was already run. Keeping original backup:',latest
        for f in (sectors,constants,slices): check_latest(f)
        constants_bp = constants+'.uncleaned.'+timestamp
        sectors_bp = sectors+'.uncleaned.'+timestamp
        slices_bp = slices+'.uncleaned.'+timestamp+'.root' # need to maintain root extension
        # First, clean up constants (if they exist):
        if os.path.isfile(constants) and os.stat(constants).st_size>0:
            cmds = []
            cmds.append('mv ' + constants + ' ' + constants_bp)
            cmds.append('rm -f '+badsectors)
            cmds.append('rm -f '+totsectors)
            cmd=self.Get('clean_const.awk')+' -v block=`'+self.Get('block_size.awk')+' '+constants_bp+'`'+' '
            cmd+='-v badname='+badsectors+' -v totname='+totsectors+' '
            cmd+=constants_bp+' > '+constants
            cmds.append(cmd)
            cmds.append('sed -i -e "s#REPLACEME#`head -n1 '+totsectors+'`#" '+constants)
            self.Exec(cmds,self.noexec)
            if os.path.isfile(badsectors) and os.stat(badsectors).st_size>0:
                # Second, clean up slices
                if os.path.isfile(slices) and os.stat(slices).st_size>0:
                    cmds = []
                    cmds.append('mv ' + slices + ' ' + slices_bp)
                    cmd = "root -l -b -q '"+self.Get('clean_slices.C')+'("'
                    cmd+=slices_bp+'","'+slices+'","'+badsectors+'"'
                    cmd+=")'"
                    cmds.append(cmd)
                    self.Exec(cmds,self.noexec)
                else:
                    print 'Failed to find slices file:',slices
                # Third, clean up sectors
                if os.path.isfile(sectors) and os.stat(sectors).st_size>0:
                    cmds = []
                    cmds.append('mv ' + sectors + ' ' + sectors_bp)
                    cmd = self.Get('clean_sectors.awk')+' -v badname='+badsectors+' '+sectors_bp+' > '+sectors
                    cmds.append(cmd)
                    self.Exec(cmds,self.noexec)
                else:
                    print 'Failed to find sectors file:',sectors
            else:
                print 'Region',i,"has no bad sectors; skipping cleanup"
        else:
            self.WARN(1,'Failed to find constants: '+constants)
            return
        # set a warning flag so that this job is not processed further
        self.WARN(1,"Cleanup job succeeded")
        return 'Done!'

class PattgenJob(BaseJob):
    """ pattgen in pattern mode (produces patterns from athena) """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        self.Add('run_name',self.RunName('pattgen',1)%self.Get('i'))
        self.Add('run_path',self.RunPath('pattgen',1)%self.Get('i'))
        self.Add('SECTOR_MODE',0)
        self.Add('REALDATA',1)
        self.Add('WEDGE_MIN',self.Get('i'))
        self.Add('WEDGE_MAX',self.Get('i'))
        self.Add('PE2S_FILE',self.SectorFile(False)%self.Get('i'))
        self.Add('patt_type','pattgen')
        if self.Geti('MAXPATTERNS')==-1: self.Add('MAXPATTERNS',30000000)
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.Get('patdir'))
        self.executable=self.Get('pattgen')
        BaseJob.Initialize(self)
    def function_AddPatternBanks(self):
        """ Constructs pattern banks """
        result = []
        result.append('PE2S_FILE %s'%self.Get('PE2S_FILE'))
        result.append('')
        for i in xrange(self.Geti("N")):
            patfile=os.path.join(self.Get('garbagedir'),'garbage_'+self.Get('run_name')+'.patt')
            if i==self.Geti('i'):
                patfile = self.PatternFile(False)%i
            if not os.path.exists(os.path.dirname(patfile)): os.makedirs(os.path.dirname(patfile))
            result.append('PATT_FILE%d %s'%(i,patfile) )
        result=[r.split() for r in result]
        result.insert(1,['# Filenames for the %d sector files'%self.Geti('N'),])
        result.insert(2,['#FTKNOCHECK'])
        result.append(['#FTKCHECK'])
        return result
    def ProcessTemplate(self,template=''):
        """ Adds pattern bank information to roadfinder configuration """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'pattgen.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddPatternBanks()
        self.templateC = result
        return self.templateC

class PatternsJob(BaseJob):
    """ pattgen in pattern-from-const mode (produces patterns from constants) """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        self.Add('run_name',self.RunName('patterns',3)%(self.Get('i'),self.Get('j'),self.Get('r')) )
        self.Add('run_path',self.RunPath('patterns',3)%(self.Get('i'),self.Get('j'),self.Get('r')) )
        self.Add('SECTOR_MODE',0)
        self.Add('REALDATA',0)
        self.Add('WEDGE_MIN',self.Get('i'))
        self.Add('WEDGE_MAX',self.Get('i'))
        self.Add('PE2S_FILE',self.SectorFile(False)%self.Get('i'))
        self.Add('patt_type','patterns')
        if self.Geti('MAXPATTERNS')==-1: self.Add('MAXPATTERNS',30000000)
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.Get('patdir'))
        self.executable=self.Get('pattgen')
        BaseJob.Initialize(self)
    def function_AddPatternBanks(self):
        """ Constructs pattern banks """
        result = []
        result.append('PE2S_FILE %s'%self.Get('PE2S_FILE'))
        result.append('GCON_FILE %s'%self.ConstantFile(False)%self.Get('i')) # un-split constants!
        result.append('')
        for i in xrange(self.Geti("N")):
            patfile=os.path.join(self.Get('garbagedir'),'garbage_'+self.Get('run_name')+'.patt')
            if i==self.Geti('i'):
                patfile = self.PatternFile(True,True)%(self.Get('i'),self.Get('j'),self.Get('r'))
            if not os.path.exists(os.path.dirname(patfile)): os.makedirs(os.path.dirname(patfile))
            result.append('PATT_FILE%d %s'%(i,patfile) )
        result=[r.split() for r in result]
        result.insert(2,['# Filenames for the %d sector files'%self.Geti('N'),])
        result.insert(3,['#FTKNOCHECK'])
        result.append(['#FTKCHECK'])
        return result
    def ProcessTemplate(self,template=''):
        """ Adds pattern bank information to roadfinder configuration """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'pattFromConst.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddPatternBanks()
        self.templateC = result
        return self.templateC

class PromotePattgenJob(BaseJob):
    """ Bumps up multiplicity of pattgen patterns and saves them as a patterns*_runY.patt file """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label='promote'
        label_short=label
        self.Add('run_name',self.RunName(label_short,2)%(self.Get('i'),self.Get('j')) )
        self.Add('run_path',self.RunPath(label,2)%(self.Get('i'),self.Get('j')) )
        self.executable='none'
        self.shell = False
        self.noexec = False
        BaseJob.Initialize(self)
    def ProcessTemplate(self,template='irrelevant'):
        """ actual application logic """
        self.Add('patt_type','pattgen')
        patterns_old = self.PatternFile(True)%(self.Get('i'),self.Get('j'))
        self.Add('patt_type','patterns')
        patterns_new = self.PatternFile(True,True)%(self.Get('i'),self.Get('j'),self.Get('r'))
        if os.path.isfile(patterns_old) and os.stat(patterns_old).st_size>0:
            if not os.path.isfile(patterns_new):
                cmds = []
                cmd=self.Get('promote_pattgen.awk')+' -v L='+self.Get('L')+' -v offset=10'+' '
                cmd+=patterns_old+' > '+patterns_new
                cmds.append(cmd)
                self.Exec(cmds,self.noexec)
            else:
                self.WARN(1,'The requested destination pattern file already exists: '+patterns_new)
                return
        else:
            self.WARN(1,'Failed to find pattgen patterns: '+patterns_old)
            return
        # set a warning flag so that this job is not processed further
        self.WARN(1,"Pattgen promotion succeeded")
        return 'Done!'

class PatmergeJob(BaseJob):
    """ Merges patterns from different runs - using new TrigFTKSim utility! """
    """ Uses new patmergeroot program if option PATMERGEROOT is given. """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        self.Add('run_name',self.RunName('patmerge',2)%(self.Get('i'),self.Get('j')) )
        self.Add('run_path',self.RunPath('patmerge',2)%(self.Get('i'),self.Get('j')) )
        self.Add('patt_type','patterns')
        if self.Geti('MAXPATTERNS')==-1: self.Add('MAXPATTERNS',100000000)
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.Get('patdir'))
        PatMergeRoot = True if self.Get('PATMERGEROOT') else False
        if PatMergeRoot :
            self.executable=self.Get('patmergeroot')
        else : 
            self.executable=self.Get('patmerge')
        BaseJob.Initialize(self)
    def function_AddPatternBanks(self):
        """ Constructs pattern banks """
        result = []
        # output filename
        result.append('#FTKNOCHECK')
        patfile = self.PatternFile(True)%(self.Get('i'),self.Get('j'))
        if not os.path.exists(os.path.dirname(patfile)): os.makedirs(os.path.dirname(patfile))
        patfile += '.root' # prevent patmergeroot from producing ascii files
        result.append('OUT_FILE %s'%patfile)
        result.append('#FTKCHECK')
        # single run files to be merged
        result.append('NINPUTS '+str(len(self.runs)))
        for i,run in enumerate(self.runs):
            patfile = self.PatternFile(True,True)%(self.Get('i'),self.Get('j'),run)
            result.append(patfile)
        result=[r.split() for r in result]
        return result
    def ProcessTemplate(self,template=''):
        """ Adds pattern bank information to roadfinder configuration """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'patmerge.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        if self.Get('MINCOVERAGE') != None :
            result += ["MINCOVERAGE %d" % self.Geti('MINCOVERAGE')]
        result+=self.function_AddPatternBanks()
        self.templateC = result
        return self.templateC

class OldPatmergeJob(BaseJob):
    """ Merges patterns from different runs using an older TrigFTKLib utility
        DEPRECATED; use PatmergeJob instead!
    """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        self.Add('run_name',self.RunName('patmerge',2)%(self.Get('i'),self.Get('j')) )
        self.Add('run_path',self.RunPath('patmerge',2)%(self.Get('i'),self.Get('j')) )
        self.Add('SECTOR_MODE',0)
        self.Add('REALDATA',1)
        self.Add('MERGE',1)
        self.Add('WEDGE_MIN',0)
        self.Add('WEDGE_MAX',len(self.runs)-1)
        self.Add('PE2S_FILE',self.SectorFile(False)%self.Get('i'))
        self.Add('patt_type','patterns')
        if self.Geti('MAXPATTERNS')==-1: self.Add('MAXPATTERNS',30000000)
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.Get('patdir'))
        self.executable=self.Get('pattgen')
        BaseJob.Initialize(self)
    def function_AddPatternBanks(self):
        """ Constructs pattern banks """
        result = []
        result.append('PE2S_FILE %s'%self.Get('PE2S_FILE'))
        result.append('GCON_FILE %s'%self.ConstantFile(True)%(self.Get('i'),self.Get('j')) )
        result.append('SLICE_FILE %s'%self.SliceFile()%self.Get('i'))
        result.append('')
        # output filename
        result.append('#FTKNOCHECK')
        for i in xrange(self.Geti("N")):
            patfile=os.path.join(self.Get('garbagedir'),'garbage_'+self.Get('run_name')+'.patt')
            if i==0:
                patfile = self.PatternFile(True)%(self.Get('i'),self.Get('j'))
            if not os.path.exists(os.path.dirname(patfile)): os.makedirs(os.path.dirname(patfile))
            result.append('PATT_FILE%d %s'%(i,patfile) )
        result.append('')
        result.append('#FTKCHECK')
        # single run files to be merged
        for i,run in enumerate(self.runs):
            patfile = self.PatternFile(True,True)%(self.Get('i'),self.Get('j'),run)
            result.append('BANK_FILE%d %s'%(i,patfile) )
        patfile=self.DummyPatternFile()
        for j in xrange(i+1,self.Geti('N')):
            result.append('BANK_FILE%d %s'%(j,patfile) )
        result=[r.split() for r in result]
        return result
    def ProcessTemplate(self,template=''):
        """ Adds pattern bank information to roadfinder configuration """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'pattgen.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddPatternBanks()
        self.templateC = result
        return self.templateC

class SplitJob(BaseJob):
    """ Splits sectors, constants, and pattgen patterns into subregions  """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        self.Add('run_name',self.RunName('split',1)%self.Get('i'))
        self.Add('run_path',self.RunPath('split',1)%self.Get('i'))
        self.Add('patt_type','pattgen')
        self.executable='none'
        self.shell = False
        self.noexec = False
        BaseJob.Initialize(self)
    def ProcessTemplate(self,template='irrelevant'):
        """ actual application logic """
        i=self.Geti('i')
        sectors = self.SectorFile(False)%i
        patterns = self.PatternFile(False)%i
        constants = self.ConstantFile(False)%i
        # split sectors
        if os.path.isfile(sectors) and os.stat(sectors).st_size>0:
            for j in xrange(self.Geti('M')):
                sectors_new = self.SectorFile(True)%(i,j)
                if not (os.path.isfile(sectors_new) and os.stat(sectors_new).st_size>0):
                    cmds = []
                    cmd = 'awk "BEGIN{i=0;nsub=%s;isec=0;out[0]=0;};NF==2{L=\$2};(NF==%s+3){if(i%%nsub==%s){\$1=isec;out[isec]=\$0;isec++;}i++;}END{print isec,L; for(j=0;j<isec;j++) print out[j];}" %s > %s'%(self.Get('M'),self.Get('L'),j,sectors,sectors_new)
                    cmds.append(cmd)
                    self.Exec(cmds,self.noexec)
                else:
                    print 'Reg/Sub:',i,j,"- sectors are already split; skipping split job"
        else:
            print "Failed to find sectors file:",sectors
        # split pattgen patterns
        if os.path.isfile(patterns) and os.stat(patterns).st_size>0:
            for j in xrange(self.Geti('M')):
                patterns_new = self.PatternFile(True)%(i,j)
                if not (os.path.isfile(patterns_new) and os.stat(patterns_new).st_size>0):
                    cmds = []
                    # this may need too much RAM (if -m parameter is too slow!)
                    cmd = 'awk "BEGIN{i=0;nsub=%s;isec=0;out[0]=0;};NF==2{L=\$2};(NF==%s+3){if((\$`expr %s + 2`)%%nsub==%s){\$1=isec;\$`expr %s + 2`'%(self.Get('M'),self.Get('L'),self.Get('L'),j,self.Get('L'))
                    cmd += '=int(\$`expr %s + 2`/%s);out[isec]=\$0;isec++;}i++;}END{print isec,L; for(j=0;j<isec;j++) print out[j];}" %s > %s'%(self.Get('L'),self.Get('M'),patterns,patterns_new)
                    cmds.append(cmd)
                    self.Exec(cmds,self.noexec)
                else:
                    print 'Reg/Sub:',i,j,"- patterns are already split; skipping split job"
        else:
            print "Failed to find patterns file:",patterns
        # split constants
        if os.path.isfile(constants) and os.stat(constants).st_size>0:
            for j in xrange(self.Geti('M')):
                constants_new = self.ConstantFile(True)%(i,j)
                totsectors = self.TotSectorFile(True)%(i,j)
                if not (os.path.isfile(constants_new) and os.stat(constants_new).st_size>0):
                    cmds = []
                    cmds.append('rm -f '+totsectors)
                    cmd = self.Get('split_const.awk')+' -v nsubs='+self.Get('M')+' -v block=`'+self.Get('block_size.awk')+' '+constants+'`'
                    cmd += ' -v isub='+str(j)+' -v totname='+totsectors+' '+constants+' > '+constants_new
                    cmds.append(cmd)
                    cmds.append('sed -i -e "s#REPLACEME#`head -n1 '+totsectors+'`#" '+constants_new)
                    self.Exec(cmds,self.noexec)
                else:
                    print 'Reg/Sub:',i,j,"- constants are already split; skipping split job"
        else:
            print "Failed to find constants file:",constants
        self.WARN(1,"Cleanup job succeeded")
        return 'Done!'

class MakeLookupJob(BaseJob):
    """ A job that constructs a sector lookup file for the split architecture
    """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        label=self.LabelMod('lookup')
        label_short=label
        self.Add('run_name',self.RunName(label_short)%self.Get('i'))
        self.Add('run_path',self.RunPath(label)%self.Get('i'))
        if self.Get('OUT_DIR')==None:
            self.Add('OUT_DIR',self.OutputPath(label)%self.Get('i'))
        self.executable=self.Get('parsesec')
        self.shell = False
        self.noexec = False
        BaseJob.Initialize(self)
    def ProcessTemplate(self,template=''):
        """ no need to do anything here """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'ana_standard_overview.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        self.templateC = result
        # cmd line arguments to sector lookup maker
        outfile=self.SectorMapFile()%self.Get('i')
        L=self.Get('L')
        self.Add('L',4); sec4=self.SectorLookupFile()%self.Get('i')
        self.Add('L',8); sec8=self.SectorLookupFile()%self.Get('i')
        self.Add('L',nLayersFinalFit); sec11=self.SectorLookupFile()%self.Get('i')
        self.Add('L',L)
        for f in (sec4,sec8,sec11):
            if not (os.path.isfile(f) and os.stat(f).st_size>0):
                self.WARN(1,"Can't find event-sector lookup file: %s"%f)
                return 'ERROR'
        if self.executable == None:
            self.WARN(1,"Can't find parseSec executable")
        else:
            cmds = [self.executable,sec4,sec8,sec11,outfile]
            cmd = ' '.join(cmds)
            self.Exec([cmd,],self.noexec)
            self.WARN(1,"Successfully ran makelookup job in region %s"%self.Get('i'))
        return self.templateC

class MakeDCJob(BaseJob):
    """ Specific configuration for patt.bz2 -> sqlite patt.db conversion  """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        self.Add('run_name',self.RunName("makedc",2)%(self.Get('i'),self.Get('j')))
        self.Add('run_path',self.RunPath("makedc",2)%(self.Get('i'),self.Get('j')))
        self.executable = self.Get('tsp_bank_generator')
        BaseJob.Initialize(self)
    def function_AddPatternBanks(self):
        """ Constructs pattern banks """
        result = []
        result.append(['BANK %s'%(self.Get('MAXPATTERNS'))])
        result.append([self.Get('INBANK')])
        result.append(["OUTBANK"])
        result.append([self.Get("OUTBANK")])
        result.insert(0,['# set the maximum number of TSP patterns (-1 means all)',])
        return result
    def ProcessTemplate(self,template=''):
        """ Adds pattern bank information to roadfinder configuration """
        if template=='':
            if self.customtpl!="" :
                template=os.path.join(self.Get('scripts_dir'),self.customtpl)
            else :
                template=os.path.join(self.Get('scripts_dir'),'makedc.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddPatternBanks()
        self.templateC = result
        return self.templateC

class QuickDumptracksJob(BaseJob):
    """ TrigFTKSim quickfit in single muon mode: dumping 8L tracks """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        self.Add('run_name',self.RunName('dumptrk',1)%self.Get('i'))
        self.Add('run_path',self.RunPath('dumptrk',1)%self.Get('i'))
        self.Add("TRACK_MODE",1)
        self.Add("REGIONS_11L",1)
        self.executable=self.Get('quick_fit')
        BaseJob.Initialize(self)
    def function_AddPatternBanks(self):
        """ list sectors and constants in pairs """
        result = []
        result.append('NBANKS %s'%self.Get('M'))
        for j in xrange(self.Geti("M")):
            secfile = self.SectorFile(True)%(self.Geti('i'),str(j))
            gconfile = self.ConstantFile(True)%(self.Geti('i'),str(j))
            result.append(secfile)
            result.append(gconfile)
        result=[r.split() for r in result]
        return result
    def ProcessTemplate(self,template=''):
        """ Adds pattern bank information to roadfinder configuration """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'quickfit.tpl')
        self.setNCoords()
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddPatternBanks()
        self.templateC = result
        return self.templateC

class QuickSectorJob(BaseJob):
    """ TrigFTKSim quickfit in single muon mode: making sectors
    This is not used yet (not optimized for speed) - just proof of concept
    """
    def __init__(self,**kwargs):
        BaseJob.__init__(self,**kwargs)
    def Initialize(self):
        """ Sets critical job paramaters. The rest is finished in BaseJob::Initialize() """
        self.Add('run_name',self.RunName('sectors',1)%self.Get('i'))
        self.Add('run_path',self.RunPath('sectors',1)%self.Get('i'))
        self.Add("SECTOR_MODE",1)
        self.Add("NSUBS",self.Geti('M')) # represents number of banks
        self.Add("REGIONS_11L",0)
        self.executable=self.Get('quick_fit')
        BaseJob.Initialize(self)
    def function_AddPatternBanks(self):
        """ list sectors and constants in pairs """
        result = []
        result.append('NBANKS %s'%self.Get('N'))
        result.append('#FTKNOCHECK')
        for i in xrange(self.Geti("N")):
            secfile = self.SectorFile(False)%i
            result.append(secfile)
        result.append('#FTKCHECK')
        result=[r.split() for r in result]
        return result
    def ProcessTemplate(self,template=''):
        """ Adds pattern bank information to roadfinder configuration """
        if template=='':
            template=os.path.join(self.Get('scripts_dir'),'quickfit.tpl')
        result=BaseJob.ProcessTemplate(self,template)
        result+=self.function_AddPatternBanks()
        self.templateC = result
        return self.templateC

############################################################################
# Command line parsing routines
############################################################################
class bcolors:
    """ Helper class to output colored text """
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    def disable(self):
        self.HEADER = ''
        self.OKBLUE = ''
        self.OKGREEN = ''
        self.WARNING = ''
        self.FAIL = ''
        self.ENDC = ''

class dq2_cache:
    """ Helper class that keeps a list of files in grid datasets or containers
    To avoid multiple dq2 calls, the results are cached
    """
    _ndays = 30
    def __init__(s):
        s.data = {}   # indexed by dataset name
        s.date = {}   # indexed by dataset name
        s.fname = 'dq2cache.pickle'
    def cleanup(s):
        """ Remove old datasets from the cache """
        for ds in s.date.keys():
            if (datetime.datetime.now()-s.date[ds]).days > s._ndays:
                print 'Cleaning up old dq2_cache entry:',ds
                del s.date[ds]
                del s.data[ds]
    def fill(s,ds,force=False):
        from dq2.clientapi.DQ2 import DQ2
        dq2 = DQ2()
        
        """ retrieves the list of files in dataset ds """
        # trust the cache if it's not older than _ndays days
        if force==False and ds in s.data and (datetime.datetime.now()-s.date[ds]).days < s._ndays:
            return
        s.data[ds] = []
        print 'Retrieving list of files in DS',ds
        filesinfo, data = dq2.listFilesInDataset(ds)
        if len(filesinfo)==0 :
            print "WARNING: problem retrieving files for", ds
            print filesinfo
        
        try :
            for k, v in filesinfo.iteritems() : # each file is a key in filesinfo fictionary            
                s.data[ds].append(v["lfn"])
            s.data[ds].sort()
            assert len(s.data[ds])>0
            s.date[ds] = datetime.datetime.now()
        except:
            print 'ERROR: failed to run dq2-ls on dataset',ds
            sys.exit(3)
        # update pickle cache
        s.cleanup()
        fout = open(s.fname,'w')
        pickle.dump(s,fout)
        fout.close()
        print 'Cached',ds
    def load(s,ds,force=False):
        return s.fill(ds,force)
    def findall(s,ds,pattern,nSkipFiles=0):
        """ finds all occurances of pattern in files within a given dataset """
        res = []
        if pattern[0]!='*': pattern='*'+pattern
        if pattern[len(pattern)-1]!='*': pattern=pattern+'*'
        pattern = pattern.replace('*','.*')
        for file in s.data[ds]:
            if re.search(pattern,file):
                res.append(file)
        return res[nSkipFiles:]
    def findallBlk(s,ds,pattern,blocksize,iblock,nSkipFiles=0):
        """ returns iblock block of files with given blocksize """
        files = s.findall(ds,pattern,nSkipFiles)
        nfiles = len(files)
        return [f for i,f in enumerate(files) if i<blocksize*(iblock+1) and i>=blocksize*iblock]
    def find(s,ds,pattern):
        """ same as find, but requires that exactly one match is found """
        res = s.findall(ds,pattern)
        assert len(res)==1,'%d files matched [%s] in [%s]; expected 1 match'%(len(res),pattern,ds)
        return res[0]
    def findQ(s,ds,pattern):
        """ a version of find() that doesn't do nmatch error detection, but runs much faster """
        if pattern[0]!='*': pattern='*'+pattern
        if pattern[len(pattern)-1]!='*': pattern=pattern+'*'
        pattern = pattern.replace('*','.*')
        for file in s.data[ds]:
            if re.search(pattern,file):
                return file
        assert 0==1,'0 files matched [%s] in [%s]; expected 1 match'%(pattern,ds)
    def maxIdx(s,ds):
        """ Finds maximum index in the dataset """
        assert ds in s.data,'ERROR: you must fill() dataset %s before you can use it'%ds
        vals = [1,]
        for f in s.data[ds]:
            match = re.search('\._(\d+)\.',f)
            assert match, 'ERROR: cannot match panda idx in file %s'%f
            vals.append(int(match.group(1)))
        return max(vals)
    @staticmethod
    def GET_DQ2_CACHE(fname=DQ2_CACHE_NAME):
        """ Main accessor method to retrieve dq2_cache object """
        if os.path.exists(fname):
            f = open(fname,'r')
            fcache = pickle.load(f)
            f.close()
        else:
            fcache = dq2_cache()
        fcache.fname = fname
        return fcache

class SimpleOptions(DicBase):
    """
    A class to hold all processable options
    It recreates the interface of the old bash-based wrapper
    """
    def __init__(self):
        """ Lightweight constructor """
        DicBase.__init__(self)
        self.actions=[]
        self.nthreads = 1
        self.dic['patt_type']='patterns'  # or pattgen
        self.dic['qsub']='qsub'
        self.dic['qsub_opts']=''
        self.dic['sleep_time']=1
        self.dic['run_mode']='normal'
        # defualt programs
        self.dic['anaexe']='ana_standard_overview'
        # file extensions for patterns, constants and lookups
        self.dic['_PATT']='patt'
        self.dic['_PATTDB']='patt.root'
        self.dic['_GCON']='gcon'
        self.dic['_DAT']='dat'
        # empty bad modules map
        self.Add('BMAP','empty')
        self.Add('BMAP_FOR_HIT','empty')
        # some sensible defaults
        self.Add('MERGED_TRACKS',1)       # enable track merger / hitwarrior
        self.Add('MERGED_ROADS',1)        # trackfitter will use output of road_merger
        self.Add('ANA_IGNORE_ROADS',1)    # make analysis script x100 faster
        self.Add('batch_mode',1)
        # split architecture defaults (following TP configuration)
        # this is only used in "-a newsplit" running;
        # runCM.sh still sets these values explicitly on the command line!!!
        self.Add('sp_RW8',0)     # for TP, we assume there is no RW at 8L stage
        self.Add('sp_RW11',0)
        self.Add('sp_HW8',1)     # in-road hitwarrior (easy to implement in hardware)
        self.Add('sp_HW11',2)
        self.Add('sp_HW_NDIFF11',6)
        self.Add('sp_HW_NDIFF8',3)
        self.Add('sp_nsctmiss',1)
        self.Add('sp_nsctmisspair',0)
        self.Add('sp_npixmiss',1)
        self.Add('sp_nscttrkmiss',1)
        self.Add('sp_chi2dofcut',4)
        self.Add('sp_requireb',0)
        
    def help(self):
        print
        print bcolors.HEADER+'Usage:'+bcolors.ENDC,sys.argv[0],'-q runmode -a action -m Nsubregs -d runstring -n #events -o[ther options]'
        print '                -r outdir -t bankdir -p[attgen patts] -f TrigFTKSimDir -e TrigFTKAnaDir -j TrigFTKLibDir'
        print 'Grid help available with: ',sys.argv[0],'-q',bcolors.HEADER+'grid'+bcolors.ENDC
        print bcolors.HEADER+'Possible runmodes'+bcolors.ENDC+' (-q): ',
        print 'config,dumpconfig,debug,valgrind,normal,silent,pbs,grid,threaded_X (where X is number of threads)'
        print bcolors.HEADER+'Possible actions:'+bcolors.ENDC
        print "(specifying 'Y' runs only on region #Y; otherwise runs on all regions)"
        print "(actions can be listed in comma-separated list, with STOP keyword waiting for previous job to finish)"
        print ' -a ' + bcolors.OKBLUE + "fullchainY" + bcolors.ENDC,'        -  Shortcut to run full FTK chain'
        print ' -a ' + bcolors.OKBLUE + "newsplitY" + bcolors.ENDC,'         -  Shortcut to run full FTK Option-B chain'
        print ' -a ' + bcolors.OKBLUE + "anaY" + bcolors.ENDC,"              -  Antonio's TrigFTKAna analysis code"
        print 'ADDITIONAL COMMANDS:'
        print ' -a ' + bcolors.OKBLUE + "roadfinderY[__X_Z]" + bcolors.ENDC,'-  Road Finder in region Y, subregions X,Z (default=all)'
        print ' -a ' + bcolors.OKBLUE + "roadfindertspY    " + bcolors.ENDC,'-  RF with TSP bank, tsplabel option needed, i.e.: -o tsplabel=96x80x72'
        print ' -a ' + bcolors.OKBLUE + "roadmergerY" + bcolors.ENDC,'       -  Road Merger'
        print ' -a ' + bcolors.OKBLUE + "trackfitterY" + bcolors.ENDC,'      -  Track Fitter (add -o FILTERED_ROADS to fit filtered roads)'
        print ' -a ' + bcolors.OKBLUE + "trackfitter711Y" + bcolors.ENDC,'   -  Track Fitter for opxtion A (add -o FILTERED_ROADS to fit filtered roads)'
        print ' -a ' + bcolors.OKBLUE + "trackmergerY" + bcolors.ENDC,'      -  Track Merger that operates on outputs of trackfitterY jobs'
        print ' -a ' + bcolors.OKBLUE + "alltrackmerger" + bcolors.ENDC,'    -  Track Merger that operates on outputs of trackmergerY jobs'
        print ' -a ' + bcolors.OKBLUE + "plot_Y_Y_Y_Y" + bcolors.ENDC,'      -  Basic Plots over specified regions. Also accepts -o dump_events'
        print ' -a ' + bcolors.OKBLUE + "check" + bcolors.ENDC,'             -  Summary of roads and tracks, includig timing information'
        print ' -a ' + bcolors.OKBLUE + "effchainY" + bcolors.ENDC,'         -  Runs several sets of fullchain with different bank sizes'
        print ' -a ' + bcolors.OKBLUE + "effcurveY" + bcolors.ENDC,'         -  Plots efficiency turn-on curve for corresponding effchain run'
        print 'BANK PRODUCTION COMMANDS (make sure to have TrigFTKLib checked out and specified via -j flag):'
        print ' -a ' + bcolors.OKBLUE + "sectorsY" + bcolors.ENDC,'          -  makes sectors in region Y (-a allsectors makes in all regions)'
        print ' -a ' + bcolors.OKBLUE + "filterXX[_Y_Z]" + bcolors.ENDC,'    -  removes sectors with <XX hits (default=15) in regions Y,Z'
        print ' -a ' + bcolors.OKBLUE + "corrgenY" + bcolors.ENDC,'          -  makes corrgen constants and slices'
        print ' -a ' + bcolors.OKBLUE + "cleanupY" + bcolors.ENDC,'          -  cleans up bad constants, sectors, and slices'
        print ' -a ' + bcolors.OKBLUE + "pattgenY" + bcolors.ENDC,'          -  produces pattgen patterns (from athena)'
        print ' -a ' + bcolors.OKBLUE + "splitY" + bcolors.ENDC,'            -  splits sectors, pattgen patts, constants into "-m" subregions'
        print ' -a ' + bcolors.OKBLUE + "patternsY[_X_Z]__R" + bcolors.ENDC,'-  produces "-n" linearized patterns for reg Y; subreg X,Z; run R'
        print ' -a ' + bcolors.OKBLUE + "promoteY[_X_Z]__R" + bcolors.ENDC,' -  bumps up pattgen multiplicity and saves them as runR patterns'
        print ' -a ' + bcolors.OKBLUE + "patmergeY__R1_R2" + bcolors.ENDC,'  -  merges linearized patterns from runs R1,R2'
        print ' -a ' + bcolors.OKBLUE + "convertlookup" + bcolors.ENDC,'     -  converts region_lookup.root to region_lookup.dat'
        print ' -a ' + bcolors.OKBLUE + "makelookupY" + bcolors.ENDC,'       -  makes a sector lookup table for the split mode (4L+8L->11L)'
        print ' -a ' + bcolors.OKBLUE + "dumptracksY" + bcolors.ENDC,'       -  makes TRK_LOOKUP table with 8L FTK tracks using single muons'
        sys.exit(0)
    def grid_help(self):
        print
        print 'The following options are available to steer FTK grid jobs:'
        print bcolors.OKBLUE+'--mode '+bcolors.ENDC+'              - FTK run mode (see grid/prun_job.sh)'
        print bcolors.OKBLUE+'--username nickname'+bcolors.ENDC+'  - Grid-acceptable user nickname (e.g, kapliy)'
        print bcolors.OKBLUE+'--site SITE_NAME'+bcolors.ENDC+'     - ATLAS grid site from this list:'
        print '  ',' '.join(GRID_SITES)
        print 'The following options select input wrapper files (similar to pathena):'
        print bcolors.OKBLUE+'--inDS=dataset'+bcolors.ENDC+"       - dataset containing wrapper input files"
        print bcolors.OKBLUE+'--match'+bcolors.ENDC+"              - file pattern for wrapper files in inDS"
        print bcolors.OKBLUE+'--nFilesPerJob'+bcolors.ENDC+"       - number of inDS files per job"
        print bcolors.OKBLUE+'--nJobs'+bcolors.ENDC+"              - maximum number of inDS jobs"
        print bcolors.OKBLUE+'--maxNJobs'+bcolors.ENDC+"           - total max of grid jobs (inc. reg/sub loops)"
        print bcolors.OKBLUE+'--gridtest'+bcolors.ENDC+'           - shortcut for --maxNJobs=2'
        print 'The following options can override what the grid submission will loop over:'
        print bcolors.OKBLUE+'--regions 0..7'+bcolors.ENDC+"       - regions to process"
        print bcolors.OKBLUE+'--subregions 0..7'+bcolors.ENDC+"    - subregions to process"
        print bcolors.OKBLUE+'--inputs 2..3'+bcolors.ENDC+"        - manually select input groups of nFilesPerJob files in each"
        print bcolors.OKBLUE+'--loops 2,3,4,5'+bcolors.ENDC+"      - pattPoints OR patt-from-const generation loops"
        print 'Finally, here is an INCOMPLETE list of additional options you may find useful:'
        print bcolors.OKBLUE+'-t dataset'+bcolors.ENDC+'           - dataset with pattern files'
        print bcolors.OKBLUE+'-o baseDS=dataset'+bcolors.ENDC+'    - dataset with sectors & constants (if absent in -t dataset)'
        print bcolors.OKBLUE+'-o ftkDS=dataset'+bcolors.ENDC+'     - output dataset of a prior FTK grid job'
        print bcolors.OKBLUE+'-o DO_NOT_MERGE_ROADS'+bcolors.ENDC+'- use with merge modes to only do global trackmerger'
        print bcolors.OKBLUE+'-o strict_error_check'+bcolors.ENDC+'- exit in case of errors (default is to continue with next action)'
        print bcolors.OKBLUE+'-o run_all_inputs'+bcolors.ENDC+'    - exit if no roads found for ALL inputs in wrapper file list'
        print bcolors.OKBLUE+'-o skip_tarball'+bcolors.ENDC+'      - disables regeneration of all TrigFTK* tarballs (faster submits)'
        print bcolors.OKBLUE+'-o skip_ana'+bcolors.ENDC+'          - disables TrigFTKAna in your grid submission'
        print bcolors.OKBLUE+'-o skip_compile'+bcolors.ENDC+"      - do not compile TrigFTK* executables (only for --site=LOCAL)"
        print bcolors.OKBLUE+'-o skip_submit'+bcolors.ENDC+"       - prepare a PandaRun job, but don't actually submit it"
        if False:  # no longer needed as we get banks/inputs from datasets now
            print bcolors.OKBLUE+'-t /pnfs/path/to/banks'+bcolors.ENDC+' - path to the banks on the grid site (published on twiki)'
            print '  V13 BANKS: /pnfs/uchicago.edu/data/tier3/user/user.AntonKapliy/hugeBank/'
            print '  V14 BANKS: /pnfs/uchicago.edu/data/tier3/user/user.AntonKapliy/ftkbanksV14/'
            print bcolors.OKBLUE+"-I '/pnfs/path/to/wrapper/files/*.dat.bz2'"+bcolors.ENDC+' - path to wrapper files on the grid site:'
            print '  1. Find the grid storage cell where wrapper dataset replicas are located'
            print '    > dq2-ls -L ROAMING -r user.HannahDeBerg.FTK.1033.tautaull'
            print '  2. Suppose the above command found a replica at BNLPANDA. Use it with "-L" to find physical file paths:'
            print '    > dq2-ls -L BNLPANDA -fp user.HannahDeBerg.FTK.1033.tautaull'
            print '  3. Note the pattern for wrapper files and specify "-I" parameter using star expansion:'
            print "  -I '/pnfs/uchicago.edu/data/tier3/user/user.AntonKapliy/cscwhbb1034/ftksimwrap_hit_raw_*.dat.bz2'"
        sys.exit(0)
    def printOptions_help(self):
        print
        print 'List of options with current values:'
        print string.join(map(lambda x: "%s\n"%(x), self.DumpString().split(',')), "")  
        sys.exit(0)
    def GetActions(self):
        return self.actions
    def GetThreadingInfo(self):
        """ Determine optimal # of threads """
        th=self.Get('run_mode').split('_')
        ncpus_req=os.popen("cat /proc/cpuinfo | cut -d ' ' -f 1 | grep -c processor")
        try:
            ncpus=int(ncpus_req.readline().strip())
        except:
            print 'Failed to properly calculate # of CPUs. Resetting to 1'
            ncpus=1
        nthreads=2
        if len(th)==2:
            nthreads=int(th[1])
        if nthreads>ncpus:
            print "You requested %d threads, but only seem to have %d CPUs"%(nthreads,ncpus)
            print "Setting # threads to",ncpus
            nthreads=ncpus
        print "Threading info:",nthreads,"threads on",ncpus,"CPUs"
        return (nthreads,ncpus)
    def SetDefaults(self):
        """ Sets some default values """
        self.SetCommonPars()
        self.SetConfigDir('../config')
        self.SetFtksimDir('../')
        self.SetBankDir('~/base')
        self.SetOutputDir('./output')
        self.SetRunstring('raw_11L_50x64x144_mydataset_mytag')
    def SetCommonPars(self,N=8,M=4,L=nLayersFinalFit,i=0,j=0,nevents=1000,MAXPATTERNS=-1):
        self.dic['N']=N
        self.dic['Nm1']=N-1
        self.dic['M']=M
        self.dic['L']=L
        self.dic['i']=i
        self.dic['j']=j
        self.dic['j8L']=j
        self.dic['MAXPATTERNS']=MAXPATTERNS
        self.dic['nevents']=nevents

    def SetConfigDir(self,a):
        """ Shortcut to set all maps and related files from config.tar.bz2 """
        a=os.path.expanduser(a)
        a=os.path.abspath(a)
        self.dic['confdir']=a
        self.dic['ssdir']=os.path.join(a,'ss_file')
        self.dic['mapdir']=os.path.join(a,'map_file')
        self.dic['hwdir']=os.path.join(a,'hwsdev_file')
        # note that inputdir can be overridden via SetInputListDir
        self.dic['inputdir']=os.path.join(a,'input')  # compatibility
        self.msg('Setting up config dir in: ',a)
    def CheckConfigDir(self):
        for d in ('ssdir','mapdir','hwdir'):
            if not os.path.isdir(self.Get(d)):
                print 'Cannot find all required maps:',self.Get(d)
                sys.exit(3)
    def SetInputListDir(self,a):
        """ Allows to override the default location of inputdir """
        a=os.path.expanduser(a)
        a=os.path.abspath(a)
        self.dic['inputdir']=a
        if not os.path.exists(self.dic['inputdir']):
            try:
                os.makedirs(self.dic['inputdir'])
            except:
                print 'Bad inputdir (-i):',self.dic['inputdir']
                sys.exit(3)
    def CheckInputListDir(self):
        for d in ('inputdir',):
            if not os.path.isdir(self.Get(d)):
                print 'Cannot find input directory with wrapper file lists:',self.Get(d)
                sys.exit(3)
    def MakeInputFileList(self,outname,pattern):
        """ Allows on-the-fly creation of input file lists
        If pattern is something like '/home/antonk/data/*.dat', the list of matching files is saved in outname
        """
        outname=os.path.expanduser(outname)
        outname=os.path.abspath(outname)
        inpfiles=glob.glob(pattern)
        inpfiles.sort()
        if len(inpfiles)==0:
            print "No input files found satisfying pattern:",pattern
            sys.exit(3)
        try:
            f = open(outname, 'w')
        except IOError:
            print "Failed to create input file list:",outname
            sys.exit(3)
        [f.write(os.path.abspath(l)+'\n') for l in inpfiles]
        f.close()
        print 'Added',len(inpfiles),'files to the input list:',outname
        print 'First file in the list:',os.path.abspath(inpfiles[0])
    def SetFtksimDir(self,a):
        """ Shortcut to set the root directory of TrigFTKSim distribution """
        a=os.path.expanduser(a)
        a=os.path.abspath(a)
        self.dic['ftksim_dir']=a
        self.dic['standalone_dir']=os.path.join(a,'standalone')
        self.dic['grid_dir']=os.path.join(a,'grid')
        self.dic['grid_script']=os.path.join(a,'grid','prun_job.sh')
        self.dic['scripts_dir']=os.path.join(a,'standalone_scripts')
        self.dic['ftk_script']=os.path.join(a,'standalone_scripts','ftk.py')
        self.dic['libftk']=os.path.join(a,'standalone','libftk_classes.so')
        self.dic['road_finder']=os.path.join(a,'standalone','road_finder')
        self.dic['tsp_bank_generator']=os.path.join(a,'standalone','tsp_bank_generator')
        self.dic['road_merger']=os.path.join(a,'standalone','road_merger')
        self.dic['hit_filter']=os.path.join(a,'standalone','hit_filter')
        self.dic['track_fitter']=os.path.join(a,'standalone','track_fitter')
        self.dic['track_fitter711']=os.path.join(a,'standalone','track_fitter711')
        self.dic['track_merger']=os.path.join(a,'standalone','track_merger')
        self.dic['patmerge']=os.path.join(a,'standalone','patmerge')
        self.dic['quick_fit']=os.path.join(a,'standalone','quick_fit')
        self.dic['convert_lookup']=os.path.join(a,'standalone','convert_lookup')
        self.msg('Setting up FTK software in: ',a)
    def CheckFtksimDir(self):
        for d in ('libftk','road_finder','road_merger','hit_filter','track_fitter','track_fitter711','track_merger','patmerge','quick_fit','grid_script','ftk_script'):
            if not os.path.isfile(self.Get(d)):
                print 'Cannot find ftk executables'
                print 'Particularly, failed to find:',self.Get(d)
                sys.exit(3)
    def SetFtkanaDir(self,a):
        """ Shortcut to set the root directory of TrigFTKAna distribution """
        a=os.path.expanduser(a)
        a=os.path.abspath(a)
        self.dic['ftkana_dir']=a
        self.dic['ana_standard_overview']=os.path.join(a,'ana','ana_standard_overview')
        self.dic['ana_ftkeffchain']=os.path.join(a,'ana','ana_ftkeffchain')
        self.dic['ana_verticalslice']=os.path.join(a,'ana','ana_verticalslice')
        pass
    def CheckFtkanaDir(self):
        """ If TrigFTKAna is not found, it's OK: just unset the variable """
        for d in ('ana_standard_overview',):
            if not os.path.isfile(self.Get(d)):
                self.Del(d)
    def SetFtklibDir(self,a):
        """ Shortcut to add TrigFTKLib directories (used for bank production) """
        a=os.path.expanduser(a)
        a=os.path.abspath(a)
        self.dic['ftklib_dir']=a
        self.dic['pattgen']=os.path.join(a,'src','pattgen','pattgen')
        self.dic['corrgen']=os.path.join(a,'src','corrgen','corrgen')
        self.dic['parsesec']=os.path.join(a,'src','datatools','parseSec')
        self.dic['ftksim']=os.path.join(a,'src','ftksim','ftksim') # old ftksim
        # add some pattern production scripts
        def add(s):
            self.dic[s]=os.path.join(a,'scripts',s)
        add('split_const.awk')
        add('clean_const.awk')
        add('clean_sectors.awk')
        add('clean_slices.C')
        add('block_size.awk')
        add('promote_pattgen.awk')
        self.msg('Setting up TrigFTKLib in: ',a)
    def CheckFtklibDir(self):
        """ Check for TrigFTKLib. This is only called in bank production mode """
        progs=('split_const.awk','clean_const.awk','clean_sectors.awk','block_size.awk')
        progs+=('promote_pattgen.awk','clean_slices.C','pattgen','corrgen')
        for d in progs:
            if not os.path.isfile(self.Get(d)):
                print 'Cannot find some TrigFTKLib executables'
                print 'Particularly, failed to find:',self.Get(d)
                sys.exit(3)
    def SetOutputDir(self,a):
        """ Shortcut to set the directory that will contain all output """
        a=os.path.expanduser(a)
        a=os.path.abspath(a)
        self.dic['outdir']=a
        self.dic['garbagedir']=os.path.join(a,'garbage')
        self.dic['otherdir']=os.path.join(a,'other')
        self.msg('Will direct all output to: ',a)
    def CheckOutputDir(self):
        for d in ('outdir','garbagedir','otherdir'):
            if not os.path.isdir(self.Get(d)):
                try:
                    os.makedirs(self.Get(d))
                except:
                    print 'Failed to create output directory:',d
                    sys.exit(3)
    def SetBankDir(self,a):
        """ Shortcut to set the directory that contains the banks """
        a=os.path.expanduser(a)
        a=os.path.abspath(a)
        self.dic['bankdir']=a
        self.dic['patdir']=os.path.join(a,'patt')
        self.dic['gcondir']=os.path.join(a,'gcon')
        self.dic['lookupdir']=os.path.join(a,'lookup')
        self.msg('Will use banks from: ',a)
    def CheckBankDir(self,create=False):
        for d in ('patdir','gcondir','lookupdir'):
            if not os.path.isdir(self.Get(d)):
                if create==True:
                    os.makedirs(self.Get(d))
                else:
                    if d != 'lookupdir':  # compatibility
                        print 'Failed to find banks:',self.Get(d)
                        sys.exit(3)
    def SetRunstring(self,a,read_nsubs_string=True):
        self.dic['runstring']=a
        runpars=a.split('_')
        if len(runpars)<4:
            print "Wrong format of runstring: ",a
            print "Example runstring: raw_11L_50x64x144_V1213INDEP_mytag_extrainfo"
            print "You can also specify # of subregions after dataset name: raw_11L_50x64x144_V1213INDEP_16M"
            self.help()
        self.dic['raw']=runpars[0]
        nlfield = runpars[1].split('L')
        self.dic['L']=nlfield[0]
	# if the runstring is in the form ..._8La_... the part before
	# L is the number of layers, the last part the version of the map
        if len(nlfield)>1 :
            self.dic['Lv'] = nlfield[1]

        try:
            L=int(self.dic['L'])
        except ValueError:
            print "Runstring doesn't contain an integer value for the # of layers: ",a
            print "Example runstring: raw_11L_50x64x144_V1213INDEP_mytag"
            self.help()
        self.dic['sslabel']=runpars[2]
        self.dic['dataset']=runpars[3]
        if len(runpars)>=5:
            self.dic['tag']='_'.join(runpars[4:])
            # see if nsubs is given after dataset name
            res=re.match('(\d+)M',runpars[4])
            if res and read_nsubs_string:
                try:
                    M=int(res.group(1))
                    self.dic['M']=M
                    self.msg('Will run on',M,'subregions')
                except:
                    print 'Failed bootstrap # of subregions from runstring:',self.dic['runstring']
                    print 'Match results:',res.groups()
        self.dic['dstag']='_'.join(runpars[3:])   # dataset_tag string (for MakeListsJob)
        self.msg('Runstring: ',bcolors.FAIL+a+bcolors.ENDC)
    def ParseA_A_B_C(self,action,template,a=[],b=[]):
        """ Parses an action string, such as roadfinder1_0_1_2 """
        mask=action.split(template)[1]
        if mask:
            elms=mask.split('_')
            if elms[0]:
                a=[int(elms[0]),]
            if elms[1:]:
                b=[int(e) for e in elms[1:]]
        return (a,b)
    def ParseA_A_B_C__A(self,action,template,a=[],b=[],c=[]):
        """ Parses an action string, such as patterns1_0_1_2__0 """
        mask=action.split(template)[1]
        if mask:
            selms=mask.split('__')
            if len(selms)>1:
                mask=selms[0]
                runs = selms[1].split('_')
                if runs:
                    c=[int(e) for e in runs]
            elms=mask.split('_')
            if elms[0]:
                a=[int(elms[0]),]
            if elms[1:]:
                b=[int(e) for e in elms[1:]]
        return (a,b,c)
    def Parse_A_B_C(self,action,template,a=[]):
        """ Parses an action string, such as roadfinder_0_1_2_3_4_5_6 """
        a=[z for z in xrange(self.Geti('N'))]
        mask=action.split(template)[1]
        if mask:
            elms=mask.split('_')
            if elms[1:]:
                a=[int(e) for e in elms[1:]]
        return a
    def ProcessGridXML(self,mode,opts):
        PatMergeRoot = True if self.Get('PATMERGEROOT') else False
        """ Submits a grid job using the new xml interface to prun """
        os.chdir(self.Get('grid_dir'))
        print 'Changed working directory to:',os.getcwd()
        timestamp=self.Timestamp()

        # check consistency for some steps
        if mode=='merge_ftk' :
            if not re.match('^(user)|(group)\..*\.'+self.Get('runstring')+'\.ftk',self.Get("ftkDS")) :
                print "Merge submission failed: the runstring and the ftkDS don't match"
                print "Dataset name uncorrect:", self.Get("ftkDS")
                sys.exit(-1)
        elif mode=='ana_ftk' or mode=="exp_ftk":
            if not re.match('^(user)|(group)\..*\.'+self.Get('runstring')+'\.merge_ftk',self.Get("ftkDS")):
                print 
                print "Analysis submission failed: the runstring and the ftkDS don't match"
                print "Dataset name uncorrect:", self.Get("ftkDS")
                sys.exit(-1)

        
        # add some information to dataset names in pattern production modes
        NLoops=len(self.dic['loops'])
        # change output name to store NLoops into the output DS name
        namebase='user.'+self.Get('username')
        if self.dic['official']=="trig-daq":
            namebase="group.trig-daq"
        
        name=namebase+'.'+self.Get('runstring')+'.'+mode+"."+timestamp
            
        if mode=='pconst':
            name=namebase+'.'+self.Get('runstring')+'_%sNLoops_unmerged'%NLoops+'.'+timestamp
        # change output name to store the number of subregions into the output DS name
        if mode in ('merge_pconst','reduce','resub',):
            name=namebase+'.'+self.Get('runstring')+'_%sNSubs'%self.Get('M')+'.'+timestamp
        if mode in ('merge_pconst_root',):
            name=namebase+'.'+self.Get('runstring')+'_%sNSubs'%self.Get('factor')+'.'+timestamp
        if mode in ('makedc',):
            name=namebase+'.'+self.Get('runstring')+'_%sNSubs'%self.Get('M')+'_AM'+self.Get('amlabel')+'.'+timestamp
        # quote the arguments in the options string
        optstring = []
        for o in opts:
            if o[1]=='':
                optstring.append("%s"%o[0])
            else:
                optstring.append("%s '%s'"%(o[0],o[1]))
        if self.Get('inputDS') and mode != 'merge_ftk': #input for merge
            # add an alias to inputDS for internal use in prun_job.sh
            optstring.append("-o inputDS=%s"%self.Get('inputDS'))
            # point TrigFTKSim to the right folder
            optstring.append("-I '%s/%s'"%(self.Get('inputDS'),self.Get('match')))
        optstring=' '.join(optstring)
        # tar up TrigFTKSim and (optionally) TrigFTKLib/TrigFTKAna
        # the tarball generation can be skipped via -o skip_tarball
        # however, this flag is ignored if an older version of tarball is not found
        doTar = not self.Get('skip_tarball')
        # The tarball generation skip is enforced if an existing libDS
        # is asked.
        doReuseLib = self.Get('libDS') != None
        
        skipAnaTar = self.Get('skip_ana')  # tmp flag to disable TrigFTKAna uploading
        extout = []
        tar_args = []
        excFiles=['\*xml','\*pickle','.svn',]

        if not doReuseLib :
            if True:  # TrigFTKSim is needed always
                extout.append('TrigFTKSim.tar.bz2')
                if doTar or not os.path.exists(extout[-1]):
                    tar_args+=['TrigFTKSim',self.Get('ftksim_dir')]
            if mode in ('pconst',):
                extout.append('TrigFTKLib.tar.bz2')
                if doTar or not os.path.exists(extout[-1]):
                    tar_args+=['TrigFTKLib',self.Get('ftklib_dir')]
            else:
                excFiles.append('TrigFTKLib.tar.bz2')
            if ( re.search('reg',mode) or re.search('ana',mode) ) and not skipAnaTar:
                extout.append('TrigFTKAna.tar.bz2')
                if doTar or not os.path.exists(extout[-1]):
                    tar_args+=['TrigFTKAna',self.Get('ftkana_dir')]
            else:
                excFiles.append('TrigFTKAna.tar.bz2')
            if len(tar_args)>0:
                print './make_tarball.sh '+' '.join(tar_args)
                print 'Creating tarballs for:',' '.join([v for i,v in enumerate(tar_args) if i%2==0])
                print 'Calling aboves command. Please wait...'
                os.system('./make_tarball.sh '+' '.join(tar_args))
        # prepare to submit the jobs
        if self.Get('site')=='LOCAL':
            # special case: run locally, but use grid submission framework:
            local_run_dir='/tmp/'+self.Get('username')+'/ftk_run_local';
            #local_run_dir='/nfs/dust/atlas/user/britzger/testarea/17.3.10.1.17/Trigger/TrigFTK/local_run_dir_test';
            print '[FTKCore.py] Running on local machine.'
            print '             Using directory:'
            print '             ',local_run_dir
            #os.system('rm -Rf '+local_run_dir)
            os.system('mkdir -p '+local_run_dir)
            os.system('cp ./prun_*.sh ./dependencies.sh ./*.tar.bz2 '+local_run_dir)
            os.system('ls -l '+local_run_dir)
            os.chdir(local_run_dir)
            print 'Changed working directory to:',os.getcwd()
            loc_cmds = []
            if not self.Get('skip_compile'):
                print "changing permisions"
                #os.chmod(local_run_dir+"/prun_compile.sh",stat.S_IXUSR)
                #os.chmod(local_run_dir+"/prun_compile.sh",stat.S_IRUSR)
                #os.chmod(local_run_dir+"/prun_compile.sh",stat.S_IWUSR)
                loc_cmds.append('bash ./prun_compile.sh')
                loc_cmds.append('bash ./prun_compile.sh')
            loc_cmds.append('reg=0 sub=0 loop=0 mode=%s bash ./prun_job.sh %s'%(mode,optstring))
            print "RUNNING:",loc_cmds
            for loc_cmd in loc_cmds:
                os.system(loc_cmd)
        else:
            # create a minidom submission descriptor
            # this object will describe input and output files and datasets
            # for each job spawned under this submission

            # Prepare the cache of all dq2 files for all input & bank datasets
            # Note that we wrap dq2_cache object around a local cache, because
            # some files are used in multiple grid subjobs
            class loc_cache:
                """ This is simply a dictionary wrapped around the dq2_cache object """
                def __init__(s):
                    # prepare a lookup map that stores the list of files in each dataset
                    s.fcache = dq2_cache.GET_DQ2_CACHE(os.path.join(self.Get('grid_dir'),DQ2_CACHE_NAME))
                    # initialize local hashmap cache (aka dictionary) - hopefully won't get any collisions
                    s.cache={}
                def fill(s,ds):
                    s.fcache.fill(ds)
                    
                def find(s,ds,pattern):
                    dsp=ds+pattern
                    if dsp not in s.cache:
                        s.cache[dsp]=s.fcache.findQ(ds,pattern)
                    return s.cache[dsp]
                def findall(s,ds,pattern):
                    dsp=ds+pattern
                    if dsp not in s.cache:
                        s.cache[dsp]=s.fcache.findall(ds,pattern)
                    return s.cache[dsp]
                def fullds(self,ds):
                    from dq2.clientapi.DQ2 import DQ2
                    dq2 = DQ2()

                    vals = dq2.listDatasets2({'name': ds},False,False)
                    if len(vals.keys())==1:
                        return vals.keys()[0]
                     
                    for v in vals.keys() :
                        if ds in v :
                            return v
                    return None
            fcache = loc_cache()
             
            import parsexml
            submission = parsexml.dom_parser()
            submission.command = "./prun_job.sh %s"%optstring
            submission.title = submission.outds = name
            # run a few consistency checks
            if re.search('merge',mode):
                assert self.Get('ftkDS'), 'ERROR: --ftkDS must be defined for all merge modes'
            if mode == 'merge_ftk' or mode == "exp_ftk":
                assert self.Get('ftkDS'), 'ERROR: --inDS must be defined for all FTK simulation modes'
                fullds = fcache.fullds(self.Get('ftkDS'))
                submission.inds[fullds] = 'IN'
                submission.primaryds = fullds # inputDS->ftkDS
            elif not (re.search('pconst',mode) or mode in ('reduce','resub','makedc',)):
                assert self.Get('inputDS'), 'ERROR: --inDS must be defined for all FTK simulation modes'
                fullds = fcache.fullds(self.Get('inputDS'))
                submission.inds[fullds] = 'IN'
                submission.primaryds = fullds # inputDS->ftkDS
            # decide which outputs to register
            outputs=["out_files.tbz2"]
            if mode in ('pconst',): # define extra output files and parameters for pconst mode
                 if PatMergeRoot :
                     outputs+=["patterns.patt.root"] # for patmergeroot
                 else:
                     for kk in list(range( int(self.Geti('M')) )):
                         # note that kk is different from the sub parameter in xml file, which is just a dummy
                         outputs+=["patterns_sub%s.patt.bz2"%kk]
            submission.global_outfiles = outputs
            # Decide which tarball to ship off to the grid sites
            extout=['TrigFTKSim.tar.bz2']
            if mode in ('pconst',):
                extout+=['TrigFTKLib.tar.bz2']
            if ( re.search('reg',mode) or re.search('ana',mode)):
                if os.path.exists('./TrigFTKAna.tar.bz2') and os.path.exists(self.Get('ftkana_dir')):
                    extout+=['TrigFTKAna.tar.bz2']
            extout = ','.join(extout)

            UnsplitSectors = True if self.Get('UNSPLIT_SECTORS') else False
            SectorsAsPatterns = True if self.Get('SECTORSASPATTERNS') else False

            # Create individual jobs for minidom parser
            # Note that we select files from datasets using these options:
            # ftkDS/baseDS/baseDS4/baseDS8/-t/bankdir4/bankdir8/
            for inp in sorted(self.dic['inputs'].keys()):
                for reg in self.dic['regions']:
                    for sub in self.dic['subregions']:
                        for loop in self.dic['loops']:
                            print '> inp=%d\treg=%d\tsub=%d\tloop=%d'%(inp,reg,sub,loop)
                            job = parsexml.dom_job()
                            if mode in ("makedc",):
                                job.outfiles+=["patterns_raw_%sL_%s_%sM_reg%s_sub%s.%s"%(self.Get('L'),self.Get('sslabel'),self.Get('M'),reg,sub,self.Get('_PATTDB'))]
                            if mode in ('merge_pconst','reduce','resub',):
                                job.outfiles+=["patterns_raw_%sL_%s_%sM_reg%s_sub%s.patt.bz2"%(self.Get('L'),self.Get('sslabel'),self.Get('M'),reg,sub)]
                            if mode in ('ftk',) and self.Get('MAKECACHE')!=None:
                                job.outfiles+=["patterns_raw_%sL_%s_%sM_reg%s_sub%s.pcache.root"%(self.Get('L'),self.Get('sslabel'),self.Get('M'),reg,sub)]
                            if mode in ('merge_pconst_root',):
                                for f in range(self.Geti('factor')):
                                    job.outfiles+=["patterns_raw_%sL_%s_%sM_reg%s_sub%s.patt.bz2"%(self.Get('L'),self.Get('sslabel'),self.Get('factor'),reg,f)]
                                job.outfiles+=["patterns_raw_%sL_%s_%sM_reg%s.patt.root"%(self.Get('L'),self.Get('sslabel'),self.Get('factor'),reg)]

                            # forward these options to prun_job.sh
                            job.forward.append(('inp',inp))
                            job.forward.append(('reg',reg))
                            job.forward.append(('sub',sub))
                            job.forward.append(('loop',loop))
                            job.forward.append(('mode',mode))
                            # prepend these options to output filenames
                            job.prepend.append(('inp',inp))
                            job.prepend.append(('reg',reg))
                            job.prepend.append(('sub',sub))
                            job.prepend.append(('loop',loop))
                            # bootstrap from --inDS (aka inputDS)
                            if mode !="merge_ftk": # fixing for the 2nd stage merge jobs                                
                             ds = self.Get('inputDS')
                             if ds:
                                fullds = fcache.fullds(ds)
                                if fullds not in job.infiles:
                                    job.infiles[fullds] = []
                                for ff in self.dic['inputs'][inp]:
                                    job.infiles[fullds].append(ff)
                            # bootstrap from baseDS/baseDS4/baseDS8
                            for l in ('','4','8'):       #base label
                                L = l or self.Get('L')   #actual number of layers
                                M=self.Get('M')
                                if l=='4' and self.Get('runstring4'):
                                    try:
                                        M = self.Get('runstring4').split('_')[4].split('M')[0]
                                    except:
                                        print 'ERROR: runstring4 must be in the format raw_11L_50x64x144_datalabel_4M'
                                        sys.exit(3)
                                ds = self.Get('baseDS%s'%l)
                                if ds and (not re.search('merge',mode) or not re.search('ana',mode) ):
                                    fcache.fill(ds)
                                    fullds = fcache.fullds(ds)
                                    #print fullds
                                    if fullds not in job.infiles:
                                        job.infiles[fullds] = []
                                    # pattern production mode (unsplit sectors/constants + slices)
                                    if re.search('pconst',mode):
                                        job.infiles[fullds].append(fcache.find(ds,'sectors_raw_%sL_reg%s.patt'%(L,reg)))
                                        if l in ('','8'):
                                            job.infiles[fullds].append(fcache.find(ds,'corrgen_raw_%sL_reg%s.gcon'%(L,reg)))
                                        if l=='':
                                            job.infiles[fullds].append(fcache.find(ds,'slices_%sL_reg%s.root'%(L,reg)))
                                    # FTK running mode (splitted sectors/constants etc)
                                    elif not mode in ("exp_ftk", "merge_ftk") :
                                        # avoid to ask for configuration files if they are not required
                                        # usually, we need sectors/constants just for current region/subregion
                                        subs = [sub,]
                                        # for per-region runs, we need ALL subregions
                                        if re.search('reg',mode):
                                            subs = range(0,int(M))
                                        for isub in subs:
                                            #job.infiles[ds].append(fcache.find(ds,'sectors_raw_%sL_%sM_reg%s_sub%s.patt'%(L,M,reg,isub)))
                                            if l=='4':
                                              if UnsplitRegions :
                                                job.infiles[fullds].append(fcache.find(ds,"corrgen_raw_%dL_reg%s.gcon"%(nLayersFinalFitOld,reg)))
                                              else :
                                                job.infiles[fullds].append(fcache.find(ds,'corrgen_raw_%sL_%sM_reg%s_sub%s.gcon'%(nLayersFinalFitOld,M,reg,isub)))
                                            else:
                                              if UnsplitSectors :
                                                job.infiles[fullds].append(fcache.find(ds,"corrgen_raw_%sL_reg%s.gcon"%(L,reg)))
                                                # stage also the sector file in case UnsplitSectors is selected along with SECTORSASPATTERNS=1
                                                if SectorsAsPatterns : 
                                                    job.infiles[fullds].append(fcache.find(ds,'sectors_raw_%sL_reg%s.patt'%(L,reg)))
                                              else :
                                                job.infiles[fullds].append(fcache.find(ds,'corrgen_raw_%sL_%sM_reg%s_sub%s.gcon'%(L,M,reg,isub)))
                                                if SectorsAsPatterns : 
                                                    job.infiles[fullds].append(fcache.find(ds,'sectors_raw_%sL_%sM_reg%s_sub%s.patt'%(L,M,reg,isub)))

                                        # get matching 11L sectors/constants for option-A (only for baseDS):
                                        if (re.search('711',mode) or  self.Get("TFmode")=='trackfitter711') and not re.search('merge',mode) and l=='':
                                            #job.infiles[ds].append(fcache.find(ds,'sectors_raw_%sL_%sM_reg%s_sub%s.patt'%(11,M,reg,sub)))
                                          if UnsplitSectors :
                                            job.infiles[fullds].append(fcache.find(ds,'corrgen_raw_%sL_reg%s.gcon'%(nLayersFinalFitOld,reg)))
                                            job.infiles[fullds].append(fcache.find(ds,'sectors_raw_%sL_reg%s.conn'%(8,reg)))
                                          else :
                                            job.infiles[fullds].append(fcache.find(ds,'corrgen_raw_%sL_%sM_reg%s_sub%s.gcon'%(nLayersFinalFitOld,M,reg,sub)))
                                            job.infiles[fullds].append(fcache.find(ds,'sectors_raw_%sL_%sM_reg%s_sub%s.conn'%(8,M,reg,sub))) ## TODO : 7/8 unhard code
                                        # get sector lookup maps for option-B:
                                        if re.search('split',mode) and l=='4':
                                            job.infiles[fullds].append(fcache.find(ds,'sectors_4_8_11_reg%s.dat'%(reg,)))
                            # bootstrap from -t/bankdir4/bankdir8. Note that -t is aliased to bankdirTMP
                            for l in ('TMP','4','8'):                    #bank label
                                L = self.Get('L') if l=='TMP' else l     #actual number of layers
                                M=self.Get('M')
                                if l=='4' and self.Get('runstring4'):
                                    M = self.Get('runstring4').split('_')[4].split('M')[0]
                                ds = self.Get('bankdir%s'%l)
                                if ds and not re.search('pconst',mode) and (not re.search('merge',mode) or not re.search('ana_ftk',mode)):
                                    fcache.fill(ds)
                                    fullds = fcache.fullds(ds)
                                    if fullds not in job.infiles:
                                        job.infiles[fullds] = []
                                    # usually, we need patterns just for current subregion
                                    subs = [sub,]
                                    # for per-region split runs, we need ALL subregions
                                    if re.search('reg',mode):
                                        subs = range(0,int(M))
                                    # we always need all 4L patterns (grid running only split across 8L subregions)
                                    if l=='4':
                                        subs = range(0,int(M))
                                    # special resub mode: reduced number of subregions in a pattern bank
                                    if mode in ('resub',) and l=='TMP':
                                        factor=self.Get('factor') or 2
                                        subs=[sub+i*int(M) for i in range(int(factor)) ]
                                        M = int(M) * int(factor)
                                    for isub in subs:
                                        # if grid-produced dataset (old format), we must match %05d.out_files.tbz2:
                                        if re.search('NSubsPerRegion',ds):
                                            code = reg*int(M) + isub + 1
                                            job.infiles[fullds].append(fcache.find(ds,'%05d.out_files*tbz2'%code))
                                        # otherwise, we get the actual pattern file:
                                        else:
                                            if self.Get('CACHEDBANK')!=None: # use a cached pattern bank
                                                job.infiles[fullds].append(fcache.find(ds,'p*_raw_%sL*_%sM_reg%s_sub%s.pcache.root'%(L,M,reg,isub)))
                                            else:
                                                if SectorsAsPatterns : 
                                                    if UnsplitSectors :
                                                        job.infiles[fullds].append(fcache.find(ds,'sectors_raw_%sL*_reg%s.patt'%(L,reg)))
                                                    else :
                                                        job.infiles[fullds].append(fcache.find(ds,'sectors_raw_%sL*_%sM_reg%s_sub%s.patt'%(L,M,reg,isub)))
                                                else:
                                                    # e.g. files for makeDC step
                                                    if self.Get('PATMERGEROOT') and mode in ('makedc'):
                                                        job.infiles[ds].append(fcache.find(ds,'p*_raw_%sL*_%sM_reg%s.patt.root'%(L,M,reg)))
                                                    else:
                                                        job.infiles[ds].append(fcache.find(ds,'p*_raw_%sL*_%sM_reg%s_sub%s.patt'%(L,M,reg,isub)))
                            # bootstrap from ftkDS (output of a prior FTK job)
                            # recall that this is needed because we usually run everything in two stages:
                            # * bank production is followed by bank merging
                            # * FTK simulation  is followed by region/subregion merging
                            ds = self.Get("ftkDS")
                            if ds:
                                fcache.fill(ds)
                                fullds = fcache.fullds(ds)
                                if fullds not in job.infiles:
                                    job.infiles[fullds] = []
                                if mode=='merge_pconst':
                                    for ff in fcache.findall(ds,'reg%s_*patterns_sub%s.patt.bz2'%(reg,sub)):
                                        job.infiles[fullds].append(ff)
                                elif mode=='merge_pconst_root':
                                    for ff in fcache.findall(ds,'reg%s_*patterns.patt.root'%(reg)):
                                        job.infiles[fullds].append(ff)
                                #elif mode=='merge_ftk_eff':
                                # currently we just do global merge across all regions/subregions:
                                #    for ff in fcache.findall(ds,'inp%s_reg%s_*out_files.tbz2'%(inp,reg)):
                                #        job.infiles[ds].append(ff)
                                elif re.search('merge',str(mode)) or re.search('ana',str(mode)) or str(mode)=="exp_ftk" :
                                    # currently we just do global merge across all regions/subregions:
                                    for ff in fcache.findall(ds,'inp%s_*out_files.tbz2'%inp):
                                        job.infiles[fullds].append(ff)
                                else:
                                    print 'WARNING: found ftkDS, but it is not needed in mode',mode
                                    if self.Get('strict_error_check')=='1':
                                        sys.exit(2)
                            submission.jobs.append(job)
            # Finally, write out prun submission xml file
            xmlfname = 'jobinput.'+name+'.xml'
            xmlfile = open(xmlfname,'w')
            print >>xmlfile, submission.to_dom().toprettyxml()
            xmlfile.close()
            # Form job submission command
            # get the pandatools version
            from pandatools import PandaToolsPkgInfo
            ptvers = PandaToolsPkgInfo.release_version.split('.')
            # pandatools version transformed in X*100000+Y*1000+Z, valied if Z<1000, Y<100
            ptversN = int(ptvers[0])*100000+int(ptvers[1])*1000+int(ptvers[2])
            if ptversN>=3062 :
                print "Pandatools better than 0.3.60, using the default prun"
                prun_cmd = ['prun']
                prun_cmd.append('--loadXML %s'% xmlfname)
            else :
                print "Pandatools not better than 0.3.60, using the custom prun"
                prun_cmd = ['./prunftk.py']
                prun_cmd.append('--load_xml %s'% xmlfname)

            # if a specific Athena version is the same is used for the job submission
            ath_version = os.getenv("AtlasVersion",'16.0.2') # you can alternatively use 15.6.9

            # add option for short lifetimes
            prun_cmd.append('--useShortLivedReplicas')
            # add option for multiple output files
            prun_cmd.append('--unlimitNumOutputs')
                        

            prun_cmd.append('--athenaTag %s'%ath_version)
            prun_cmd.append('--site %s'%self.Get('site'))
            if self.Get('express')!=None:
                prun_cmd.append('--express') # --individualOutDS')
            if self.Get('unlimitNumOutputs')!=None:
                prun_cmd.append('--unlimitNumOutputs')
            if self.Get('maxCpuCount')!=None:
                prun_cmd.append('--maxCpuCount %s'%self.Get('maxCpuCount'))
            if self.Get('memory')!=None:
                prun_cmd.append('--memory %s'%self.Get('memory'))
            if self.Get('site') == "ANALY_SLAC_LMEM":
                prun_cmd.append('--memory=10240')
        
            prun_cmd.append('--maxFileSize=5000000')
            prun_cmd.append(' --buildInLastChunk ')
            if not False :
              print "Using the current CMT configuration" 
            elif 'x86_64' in os.getenv("CMTCONFIG"):
                prun_cmd.append(" --cmtConfig=x86_64-slc5-gcc43-opt")
                print "USING 64 BIT SLC5 RELEASE"
#                prun_cmd.append(" --cmtConfig=x86_64-slc6-gcc47-opt")
#                print "USING 64 BIT SLC6 RELEASE"
            elif 'i686' in os.getenv("CMTCONFIG"):
                prun_cmd.append(" --cmtConfig=i686-slc5-gcc43-opt")
                print "USING 32 BIT RELEASE"
            # which files to upload?
            if self.dic['official']=="trig-daq":
                prun_cmd.append(" --official --voms atlas:/atlas/trig-daq/Role=production ")
                
                
            if self.Get('libDS') :
                prun_cmd.append('--libDS %s' % self.Get('libDS'))
            else :
                prun_cmd.append('--excludeFile %s' % ','.join(excFiles))
                prun_cmd.append('--extFile %s'%extout)
            prun_cmd.append('--maxNFilesPerJob 2500 --writeInputToTxt %s'%submission.writeInputToTxt())
            prun_cmd.append('--nJobs %s'%self.Get('maxNJobs'))
            # compilation is now separated into a separate build job
            prun_cmd.append('--bexec "./prun_compile.sh"')
            # if any inputs are not registered in LFC, fail the submission
            prun_cmd.append('--requireLFC')
            # force files to be staged directly, even on direct-access sites
            if self.Get('COPY_WRAPS_FROM_DCACHE')=='1':
                prun_cmd.append('--forceStaged')
            if self.Get('COPY_BANKS_FROM_DCACHE')=='1':
                prun_cmd.append('--forceStagedSecondary')
            if self.Get('skip_submit'):
                prun_cmd.append('--noSubmit')
            prun_cmd = ' '.join(prun_cmd)
            print prun_cmd
            # save the name of the dataset and actual submission command
            gridfile = open(_GRIDLIST,'a')
            print >>gridfile, "\nSubmitting to the grid with this command:"
            print >>gridfile, string.join(map(lambda x: "%s "%(x), sys.argv), "")
            print >>gridfile, name
            gridfile.close()
            if not self.Get('skip_submit') or True:
                os.system(prun_cmd)
        sys.exit(0)
    def CheckIfGrid(self,opts):
        """ Checks if it is a grid job. If it is, submits it and exits. """
        def get(v,op):
            """ extracts the value of an option from command line """
            res = [o[1] for o in op if o[0]==v]
            if len(res)==1:
                return res[0]
            else:
                return None
        def get_intlist(v,op):
            """ similar to get(), but reads a comma-separated or ranged list of integer values.
            This function also removes duplicates from the final list
            For example: --regions 0,1..4,7
            """
            res = set()
            g = get(v,op)
            if not g:
                return list(res)
            try:
                for rblock in g.split(','):
                    if re.search('\.\.',rblock):
                        spl = rblock.split('..')
                        [res.add(v) for v in range(int(spl[0]),int(spl[1])+1)]
                    else:
                        res.add(int(rblock))
            except:
                print 'ERROR: wrong format for option',v
                print 'Option %s supports a range or comma-separated list: 0..4 or 0,1,2,3,4'%v
                sys.exit(0)
            return list(res)
        def readOptions(a):
            """ Set all options specified via -o opt1=bla1,opt2,opt3=bla3"""
            extras = a.split(',')
            for extra in extras:
                pair = extra.split('=')
                if len(pair)==2:
                    self.dic[pair[0]]=pair[1]
                else:
                    self.dic[pair[0]]=1
        [readOptions(o[1]) for o in opts if o[0]=='-o']
        # if it's not a grid job, proceed with local jobs
        isgrid = get('-q',opts)
        if isgrid!='grid':
            return
        # grab site and username
        self.dic['site'] = get('--site',opts)
        self.dic['username'] = get('--username',opts)
        self.dic['official']= get('--official',opts)

        if self.dic['username']==None or (self.dic['site'] not in GRID_SITES):
            print 'Please specify "--site" and "--username"'
            self.grid_help()
            sys.exit(0)
        # grab grid run mode (see definitions in grid/prun_job.sh)
        mode = get('--mode',opts)
        if mode==None:
            _default_mode = 'ftk'
            print 'Grid run mode is not specified, falling back to default (%s)'%_default_mode
            mode = _default_mode
        # see if TrigFTKSim dir was given in command line (it contains grid script)
        whichftk=get('-f',opts)
        if whichftk:
            self.SetFtksimDir(whichftk)
        self.CheckFtksimDir()
        # see if TrigFTKLib dir was given in command line (only used in patt-from-const mode)
        if mode in ('pconst',):
            ftklib=get('-j',opts)
            if ftklib:
                self.SetFtklibDir(ftklib)
            self.CheckFtklibDir()
        # see if TrigFTKAna dir was given in command line
        if re.search('merge',mode) or re.search('reg',mode) or re.search('ana',str(mode)):
            ftkana=get('-e',opts)
            if ftkana:
                self.SetFtkanaDir(ftkana)
            self.CheckFtkanaDir()
        # see if number of subregions was overridden (it determines # of grid jobs)
        self.dic['M'] = get('-m',opts) or self.dic['M']
        # parse -d string: we use it to give the name to the job
        rstring = get('-d',opts)
        if rstring:
            self.SetRunstring(rstring,False)
        SSfileName="../config/ss_file/raw_%s.ss"%self.Get('sslabel')
        self.GenericFileCheck(SSfileName)
        self.dic['bankdirTMP'] = get('-t',opts)  # save a reference for grid bootstrapping
        # FTK parameters to determine the number of jobs
        # input (wrapper) dataset specification
        self.dic['inputDS'] = get('--inDS',opts)
        self.dic['express'] = get('--express',opts)
        self.dic['unlimitNumOutputs'] = get('--unlimitNumOutputs',opts)
        self.dic['maxCpuCount'] = get('--maxCpuCount',opts)
        self.dic['memory'] = get('--memory',opts)        
        self.dic['match'] = get('--match',opts) or '*dat.bz2*'
        self.dic['maxNJobs'] = get('--maxNJobs',opts) or -1    # actual number of grid jobs (excl. build job)
        # grab test flag
        if get('--gridtest',opts):
            self.dic['maxNJobs']=2
        # Define the default values for all modes
        def default_inputs(mode):
            """ for wrapper-processing modes, this lists groups of wrapper input files via:
            { 0:[files], 1:[files] ... }
            Note that the user can manually select a particular group via --inputs flag.
            By default, it breaks ALL input files into groups with nFilesPerJob files in each.
            """
            if re.search('pconst',mode) or mode in ('reduce','resub',"makedc",):
                return { 0 : []}
            # get the user-supplied variables
            assert 'inputDS' in self.dic, 'ERROR: missing --inDS'
            inputDS = self.dic['inputDS']
            match = self.dic['match']
            nJobs = int(get('--nJobs',opts)) if get('--nJobs',opts) else -1
            nSkipFiles = int(get('--nSkipFiles',opts)) if get('--nSkipFiles',opts) else 0
            if get('--nFilesPerJob',opts):
                nFilesPerJob = int(get('--nFilesPerJob',opts)) 
            else:
                print "WARNING: --nFilesPerJob not found, setting --nFilesPerJob=10 by default."
                print "WARNING: It might crash the job in case the number of events corresponds to less than 10 input files."
                nFilesPerJob = 10
            # get the list of files
            fcache = dq2_cache.GET_DQ2_CACHE(os.path.join(self.Get('grid_dir'),DQ2_CACHE_NAME))
            fcache.load(inputDS)
            files = fcache.findall(inputDS,match,nSkipFiles)
            nfiles = len(files)
            # split input files across job groups (up to max nJobs job groups)
            out = {}
            ngrps=int(math.ceil(1.0*nfiles/nFilesPerJob))
            print 'INFO: nJobGroups=%s, nfiles=%s, nFilesPerJob=%s'%(ngrps,nfiles,nFilesPerJob)
            for igrp in xrange(0,ngrps):
                if igrp>=int(nJobs) and int(nJobs)>0:
                    break
                ff=fcache.findallBlk(inputDS,match,nFilesPerJob,igrp,nSkipFiles)
                assert(ff),'ERROR: no files found for file group %d (nfiles=%s,nFilesPerJob=%s)'%(igrp,nfiles,nFilesPerJob)
                out[igrp] = ff
            return out
        def default_regions(mode):
            #if mode=='merge_ftk_eff':
            #return range(self.Geti('N'))
            #return [0,]
            if re.search('merge',mode) or re.search('ana',mode) or mode=="exp_ftk":
                return [0,]
            return range(self.Geti('N'))
        def default_subregions(mode):
            if re.search('split_reg',mode) or mode in ('pconst',) or (re.search('merge',mode) and mode not in ('merge_pconst',)) or re.search('ana',mode) or mode=='exp_ftk':
                return [0,]
            return range(self.Geti('M'))
        def default_loops(mode):
            """ Any additional loops, such as:
            * pconst submissions (in patt-from-const generation)
            * patternPoints (in effchain modes)
            """
            if mode in ('pconst',):
                return range(0,100)
            if re.search('eff',mode) and not re.search('merge',mode):
                return [i for i,pp in enumerate(patternPoints) if pp <= self.Geti('MAXPATTERNS') or self.Geti('MAXPATTERNS')==-1]
            return [0,]
        self.dic['inputs'] = default_inputs(mode)
        inplist = get_intlist('--inputs',opts)
        if inplist:
            for k in self.dic['inputs'].keys():
                if k not in inplist:
                    del self.dic['inputs'][k]
        self.dic['regions'] = get_intlist('--regions',opts) or default_regions(mode)
        self.dic['subregions'] = get_intlist('--subregions',opts) or default_subregions(mode)
        self.dic['loops'] = get_intlist('--loops',opts) or default_loops(mode)
        # only forward a subset of options to the grid job
        forward_list = ['-s','-c','-b','-h','-t','-I','-p','-n','-d','-o','-m']
        forw_opts = [o for o in opts if o[0] in forward_list]
        self.ProcessGridXML(mode,forw_opts)
    def PostProcessOptions(self):
        """ Expand composite action shortcuts """
        STOP='STOP'
        def xflatten(seq):
            """a generator to flatten a nested list"""
            for x in seq:
                if type(x) is list:
                    for y in xflatten(x):
                        yield y
                else:
                    yield x
        ##########################################################
        # shortcuts for split arch processing
        ##########################################################
        dstag=self.Get('dstag')
        def check_split_options():
            """ Makes sure the options required for split running have been set """
            opts = ['sp_base8','sp_ss8L','sp_nsubs8','sp_maxpat8']
            opts += ['sp_base4','sp_ss4L','sp_nsubs4','sp_maxpat4']
            for opt in opts:
                val = self.Get(opt)
                if not val:
                    print 'ERROR: local split run requires that you set: -o %s=value:'%opt
                    sys.exit(0)
        def run8common():
            """ set up 8L part """
            return '[-t=%s:-m=%s:HITWARRIOR=%s:HW_NDIFF=%s:SCTTRK_MODE=0:ROADWARRIOR=%s:REQUIRE_FIRST=0:MAX_MISSING_PLANES=%s:MAX_MISSING_SCT_PAIRS=%s]'%(self.Get('sp_base8'),self.Get('sp_nsubs8'),self.Get('sp_HW8'),self.Get('sp_HW_NDIFF8'),self.Get('sp_RW8'),self.Get('sp_nsctmiss'),self.Get('sp_nsctmisspair'))
        def run4common():
            """ set up 4L part """
            return '[-t=%s:-m=%s:SCTTRK_MODE=1:SCTTRK_NSUBS=%s:ROADWARRIOR=0:REQUIRE_FIRST=%s:MAX_MISSING_PLANES=%s:MAX_MISSING_SCT_PAIRS=%s:SCTTRK_MAX_MISSING_PLANES=%s:HW_NDIFF=%s:HITWARRIOR=%s:ROADWARRIOR=%s]'%(self.Get('sp_base4'),self.Get('sp_nsubs4'),self.Get('sp_nsubs8'),self.Get('sp_requireb'),self.Get('sp_npixmiss'),self.Get('sp_nsctmisspair'),self.Get('sp_nscttrkmiss'),self.Get('sp_HW_NDIFF11'),self.Get('sp_HW11'),self.Get('sp_RW11'))
        # newsplit shortcut (across all regions/subregions)
        for i,action in enumerate(self.actions):
            if action.find('newsplit')!=-1:
                regions,subs = self.ParseA_A_B_C(action,'newsplit',range(self.Geti('N')),range(self.Geti('M')))
                if len(regions)>1 or len(subs)>1:
                    chain = []
                    fch_offset=10    # offset due to runsplit$reg_$sub
                    findeps = []
                    for w,r in enumerate(regions):
                        deps = []
                        for z,s in enumerate(subs):
                            deps += ['-%d'%(2+z*fch_offset)]
                            chain += ['newsplit%s_%s'%(r,s),STOP,STOP]
                        if len(subs)==self.Geti('M'):
                            chain += ['[INPUT_TREE_MERGED=1:MERGED_TRACKS=0]']
                            chain += ['[%s]'%':'.join(deps)]  # dependencies
                            chain += ['roadmerger%s'%r,'[MERGED_ROADS=0]','trackmerger%s'%r,STOP]
                            chain += ['[INPUT_TREE_MERGED=0:MERGED_TRACKS=1]']
                            chain += ['ana%s'%r,STOP,STOP]
                            findeps += ['-%d'%(3+w*(3+fch_offset*self.Geti('M')))]
                    if len(subs)==self.Geti('M') and len(regions)==self.Geti('N'):
                        chain += ['[%s]'%':'.join(findeps)]  # dependencies
                        chain += ['[INPUT_TREE_MERGED=1:MERGED_TRACKS=1]']
                        chain += ['alltrackmerger',STOP,'ana']
                    self.actions[i] = chain
        self.actions = list(xflatten(self.actions))
        # newsplit shortcut (running on particular region/subregion)
        for i,action in enumerate(self.actions):
            if action.find('newsplit')!=-1:
                regions,subs = self.ParseA_A_B_C(action,'newsplit',range(self.Geti('N')),range(self.Geti('M')))
                if len(regions)==1 and regions[0]>=0 and regions[0]<self.Geti('N') and len(subs)==1 and subs[0]>=0 and subs[0]<self.Geti('M'):
                    check_split_options()
                    r=regions[0]
                    s=subs[0]
                    chain = [run8common()]
                    chain += ['[-d=raw_8L_%s_%s:MAXPATTERNS=%s:MERGED_ROADS=0]'%(self.Get('sp_ss8L'),dstag,self.Get('sp_maxpat8'))]
                    chain += ['[ENCODE_SUBREGION=%s]'%s,'roadfinder%s_%s'%(r,s),'[ENCODE_SUBREGION=0]',STOP]
                    chain += ['trackfitter%s_%s'%(r,s),STOP,'makelists%s_%s'%(r,s),STOP]
                    chain += [run4common()]
                    chain += ['[-d=raw_4L_%s_%s_tag%s:MAXPATTERNS=%s:MERGED_ROADS=1:j8L=%s]'%(self.Get('sp_ss4L'),dstag,s,self.Get('sp_maxpat4'),s)]
                    chain += ['[ENCODE_SUBREGION=0]','roadfinder%s'%r,STOP,'[ENCODE_SUBREGION=1]','roadmerger%s'%r,STOP,'makelinks',STOP]
                    chain += ['[-d=raw_11L_%s_%s_tag%s]'%(self.Get('sp_ss4L'),dstag,s)]
                    chain += ['trackfitter%s'%r,STOP,'trackmerger%s'%r,STOP]
                    chain += ['[-d=raw_11L_%s_%s:-m=%s]'%(self.Get('sp_ss4L'),dstag,self.Get('sp_nsubs8'))]
                    chain += ['relabelsplit%s_%s'%(r,s)]
                    self.actions[i] = chain
        self.actions = list(xflatten(self.actions))
        ##########################################################
        # shortcuts for effchain and fullchain
        ##########################################################
        # effchain shortcut
        runstring=self.Get('runstring')
        for i,action in enumerate(self.actions):
            if action.find('effchain')!=-1:
                if action.find('effchain711')!=-1:
                    regions,subs = self.ParseA_A_B_C(action,'effchain711',range(self.Geti('N')),range(self.Geti('M')))
                else:
                    regions,subs = self.ParseA_A_B_C(action,'effchain',range(self.Geti('N')),range(self.Geti('M')))
                if len(regions)==self.Geti('N'):
                    r=''
                elif len(regions)==1 and regions[0]>=0 and regions[0]<self.Geti('N'):
                    r=regions[0]
                else:
                    print 'Usage: effchain or effchainN, where N is the region number'
                    sys.exit(0)
                self.actions[i]=[]
                if len(subs)==self.Geti('M'):
                    s=''
                else:
                    s='_'+'_'.join([str(z) for z in subs])
                for p in patternPoints:
                    if self.Geti('MAXPATTERNS')==-1 or p < self.Geti('MAXPATTERNS'):
                        if action.find('effchain711')!=-1:
                            newrunname=runstring+'_'+'np%s'%p
                            self.actions[i].append(STOP)
                            self.actions[i].append(STOP) # do it twice to decouple from previous jobgroup
                            self.actions[i].append('[MAXPATTERNS=%s:-d=%s]'%(p,newrunname))
                            self.actions[i].append('roadfinder711%s%s'%(r,s))
                            self.actions[i].append(STOP)
                            self.actions[i].append('trackfitter711%s%s'%(r,s))
                            self.actions[i].append('[-d=%s]'%runstring) # restore original runstring
                        else:
                            newrunname=runstring+'_'+'np%s'%p
                            self.actions[i].append(STOP)
                            self.actions[i].append(STOP) # do it twice to decouple from previous jobgroup
                            self.actions[i].append('[MAXPATTERNS=%s:-d=%s]'%(p,newrunname))
                            self.actions[i].append('fullchain%s%s'%(r,s))
                            self.actions[i].append('[-d=%s]'%runstring) # restore original runstring
                    else:
                        print "WARNING: Skipping patternPoint=%d because MAXPATTERNS=%s."%(p,self.Get('MAXPATTERNS'))
                        print "WARNING: Increase max patterns if you want to run more pattern points."
        self.actions = list(xflatten(self.actions))
        # effcurve shortcut
        runstring=self.Get('runstring')
        for i,action in enumerate(self.actions):
            if action.find('effcurve')!=-1:
                regions,d = self.ParseA_A_B_C(action,'effcurve',range(self.Geti('N')))
                if len(regions)==self.Geti('N'):
                    self.actions[i]=[]
                    # expand into per-region effcurve
                    for z in xrange(self.Geti('N')):
                        self.actions[i].append('effcurve%d'%z)
                        self.actions[i].append(STOP)                        
                        self.actions[i].append(STOP)
                elif len(regions)==1 and regions[0]>=0 and regions[0]<self.Geti('N'):
                    pass
                else:
                    print 'Usage: effcurve or effcurveN, where N is the region number'
                    sys.exit(0)                         
        self.actions = list(xflatten(self.actions))
        # fullchain shortcut [ across ALL regions - includes track_merger ]
        for i,action in enumerate(self.actions):
            if action.find('fullchain')!=-1:
                regions,d = self.ParseA_A_B_C(action,'fullchain',range(self.Geti('N')))
                if len(regions)==self.Geti('N'):
                    self.actions[i] = []
                    # expand into per-region fullchains
                    for z in xrange(self.Geti('N')):
                        self.actions[i].append('fullchain%d'%z)
                        self.actions[i].append(STOP)
                        self.actions[i].append(STOP)
                    # add trackmerger and dependencies on all trackfitters
                    self.actions[i].append('trackmerger')
                    fch_offset=4
                    if self.Get('MERGED_ROADS')=='0':
                        fch_offset=3
                    deps = ['-%d'%(d*fch_offset+2) for d in xrange(self.Geti('N'))]
                    self.actions[i].append('[%s]'%(':'.join(deps)))
                elif len(regions)==1 and regions[0]>=0 and regions[0]<self.Geti('N'):
                    pass
                else:
                    print 'Usage: fullchain or fullchainN, where N is the region number'
                    sys.exit(0)
        self.actions = list(xflatten(self.actions))
        # fullchain shortcut [ for ONE region ]
        for i,action in enumerate(self.actions):
            if action.find('fullchain')!=-1:
                regions,subs = self.ParseA_A_B_C(action,'fullchain',range(self.Geti('N')),range(self.Geti('M')))
                if len(regions)==self.Geti('N'):
                    pass
                elif len(regions)==1 and regions[0]>=0 and regions[0]<self.Geti('N'):
                    r=regions[0]
                    s='_'.join([str(z) for z in subs])
                    if self.Get('MERGED_ROADS')=='1':
                        self.actions[i] = ['roadfinder%s_%s'%(r,s),STOP,'roadmerger%s_%s'%(r,s),STOP,'trackfitter%s_%s'%(r,s)]
                    else:
                        self.actions[i] = ['roadfinder%s_%s'%(r,s),STOP,'trackfitter%s_%s'%(r,s)]
                else:
                    print 'Usage: fullchain or fullchainN, where N is the region number'
                    sys.exit(0)
        self.actions = list(xflatten(self.actions))
    def QueueOpts(self,a):
        """ -a string supports PBS dependencies specification in the form of -a [-1:-2:0] """
        opts=re.findall(inline_opts,a)
        result = []
        if len(opts)==1:
            opts=opts[0]
            for opt in opts.split(':'):
                try:
                    d = int(opt)
                    result.append(d)
                except ValueError:
                    return False
        return result
    def InlineOpts(self,a):
        """ -a string supports inline option modifiers in the form -a [-d=value:HITWARRIOR=4] """
        opts=re.findall(inline_opts,a)
        result = False
        if len(opts)==1:
            result = True
            opts=opts[0]
            for opt in opts.split(':'):
                pair = opt.split('=')
                self.msg('INLINE MOD:',pair)
                if len(pair)==2 and len(pair[0])>0:
                    if pair[0][0]=='-':
                        self.ParseOptions([(pair[0],pair[1]),],False)
                    else:
                        self.dic[pair[0]]=pair[1]
                elif len(pair)==1 and len(pair[0])>0:
                    self.dic[pair[0]]=1
        return result
    def UnPickleJob(self):
        fname = self.Get('preloadjob')
        if fname != None:
            if os.path.isfile(fname):
                print 'Loading a job from pickle file:',fname
                f = open(fname,'r')
                job = pickle.load(f)
                f.close()
                self.Set(job.GetOptions())
                # erase all other actions!
                self.actions = []
                return job
        return None;
    def ParseOptions(self,opts,throwErrors=True):
        """ Parses command line options """
        if len(opts)==0:
            self.help()
        for o, a in opts:
            if o == "-d":
                self.SetRunstring(a)
            elif o == '-h':
                self.dic['HITWARRIOR']=1
                self.msg('Enabled hitwarrior')
            elif o == '-a':
                self.actions = a.split(',')
                self.msg('Requested actions: ',a)
            elif o == '-r':
                self.SetOutputDir(a)
            elif o == '-t':
                self.SetBankDir(a)
            elif o == '-s':
                try:
                    self.dic['sleep_time']=int(a)
                except:
                    print '-s flag must give an integer number of seconds to sleep'
                    sys.exit(2)
                self.msg('Sleep time between batch jobs: ',a)
            elif o == '-e':
                self.SetFtkanaDir(a)
            elif o == '-f':
                self.SetFtksimDir(a)
            elif o == '-j':
                self.SetFtklibDir(a)
            elif o == '-g':
                self.SetConfigDir(a)
            elif o == '-i':
                pars=[opt[0] for opt in opts]
                try:
                    confIdx=pars.index('-g')
                    inpIdx = pars.index('-i')
                    if confIdx>inpIdx:
                        print '-i inputdir/ must come after -g confdir/'
                        sys.exit(2)
                except ValueError:
                    pass
                self.SetInputListDir(a)
            elif o == '-n':
                try:
                    self.dic['nevents']=int(a)
                except:
                    print '-n takes an integer # of events as its argument'
                    sys.exit(2)
                self.msg('Will run on',a,'events')
            elif o == '-b':
                self.dic['batch_mode']=1
                self.msg('Will run in batch mode')
            elif o == '-q':
                self.dic['run_mode']=a
                self.msg('Will run in '+a+' mode')
            elif o == '-p':
                self.dic['pattgen_mode']=1
                self.dic['patt_type']='pattgen'
                self.msg('Will use pattgen patterns, rather than pat-from-const')
            elif o == '-m':
                print "Setting m %s " %a
                try:
                    self.dic['M']=int(a)
                except:
                    print '-m takes an integer # of subregions'
                    sys.exit(2)
                self.msg('Will run on',a,'subregions')
            elif o == '-o':
                extras = a.split(',')
                for extra in extras:
                    pair = extra.split('=')
                    if len(pair)==2:
                        self.dic[pair[0]]=pair[1]
                        # special cases:
                        if pair[0]=='N':
                            self.dic['Nm1'] = int(self.dic['N'])-1
                    else:
                        self.dic[pair[0]]=1
                self.msg('Additional options: ',a)
            elif o == '--preloadjob':
                # special non-interactive flag used in PBS jobs
                self.dic['preloadjob']=a
            elif o in ('--site','--username'):
                print 'You specified %s, used only for grid jobs'%o
                print 'To submit grid jobs, use "-q grid"'
                sys.exit(0)
            elif o in ('-I',):
                # these options are processed in the second stage
                pass
            else:
                print "Unhandled option",o
                self.help()
        # second stage of options parsing [must come after -g,-i,-d]
        for o, a in opts:
            if o == '-I':
                pattern = a
                inputdir = self.dic['inputdir']
                dataset = self.dic['dataset']
                if inputdir == None or dataset == None:
                    print 'Please set -g and -d parameters'
                    sys.exit(0)
                outname=os.path.join(inputdir,'inputprod_raw_'+dataset+'.list')
                self.MakeInputFileList(outname,a)
        if len(self.actions)==0 and self.Get('preloadjob')==None and throwErrors==True:
            print "You didn't request any actions (use '-a' flag)"
            self.help()

###############################################################################
#  JOB SUBMISSION STUFF
###############################################################################
from threadpool import *

def SaveLine(fout,line):
    """ Saves one line into a circular buffer saved in a file.
    The file contains at most m records at any given time.  """
    content = []
    if os.path.exists(fout):
        f = open(fout,'r')
        content = [l.strip() for l in f.readlines()]
        content.append(line)
        content = content[-_LOGSIZE:]
        f.close()
    f = open(fout,'w')
    for l in content:
        print >>f, l
    f.close()

def FormJobCommand(job,verbose=False,debug=0):
    """ Forms a shell command to run ftk.pbs """
    ftkpbs=os.path.join(job.Get('scripts_dir'),PBS_SCRIPT_NAME)
    # extra_args are quoted (so as not to be confused with subsequent add'l args)
    if debug==1 :
        cmd=['sh',ftkpbs,job.path,job.config,"gdb\\ --args\\ "+job.executable,job.delay,'"'+job.extra_args+'"']
    elif debug==2 :
        cmd=['sh',ftkpbs,job.path,job.config,"valgrind\\ --leak-check=full\\ "+job.executable,job.delay,'"'+job.extra_args+'"']
    else :
        cmd=['sh',ftkpbs,job.path,job.config,job.executable,job.delay,'"'+job.extra_args+'"']
    if verbose==False:
        cmd += ['1>',job.stdout,'2>',job.stderr]
    return ' '.join(cmd)
    
def RunJob(job,verbose=False,debug=0):
    """ Runs a job using ftk.pbs wrapper script """
    for f in job.hookups:
        f(job)
    if os.getenv('ROOTSYS')!=job.ROOTSYS:
        os.putenv('mROOTSYS',job.ROOTSYS)
    if os.getenv('LD_LIBRARY_PATH')!=job.LD_LIBRARY_PATH:
        os.putenv('mLD_LIBRARY_PATH',job.LD_LIBRARY_PATH)
    if os.getenv('PATH')!=job.PATH:
        os.putenv('mPATH',job.PATH)
    if os.getenv('PYTHONPATH')!=job.PYTHONPATH and job.PYTHONPATH!=None:
        os.putenv('mPYTHONPATH',job.PYTHONPATH)
    cmd=FormJobCommand(job,verbose,debug)
    # save the location of latest logs
    SaveLine(_LOGLIST[0],job.config)
    SaveLine(_LOGLIST[1],job.stdout)
    SaveLine(_LOGLIST[2],job.stderr)
    # make sure the command line is not too long (bash limits!)
    if len(cmd) > MAX_CMD_LENGTH:
        print 'Job [',job.run_name,'] exceeds maxumum command length:',len(cmd),'vs',MAX_CMD_LENGTH
        return (job.run_name,-1)
    print 'Running',bcolors.HEADER+job.run_name+bcolors.ENDC
    if verbose==True:
        retcode = os.system(cmd)
    else:
        pipe=popen2.Popen3(cmd,True)
        print 'stdout:',job.stdout
        print 'stderr:',job.stderr
        retcode=pipe.wait()
    return (job.run_name,retcode)

def ThreadRun(job):
    """ Runs threaded jobs in terminal and outputs straight to log file """
    return RunJob(job,verbose=False)

def ThreadFinish(request,result):
    """ Results of a threaded job (callback) """
    print "Finished job %s (request #%s): status=%d" % (result[0],request.requestID, result[1])

class JobManager:
    """ Job management """
    ROOTSYS = os.getenv('ROOTSYS')
    LD_LIBRARY_PATH = os.getenv('LD_LIBRARY_PATH')
    PATH = os.getenv('PATH')
    PYTHONPATH = os.getenv('PYTHONPATH')
    def __init__(self):
        self.jobs=[]
        self.jobIDs=[]  # PBS id's of these jobs
        self.reqIDs=[]  # PBS id's of "prior" jobs that must finish first
        self.deps=[]    # job group id's that will be expanded into reqIDs
        if self.ROOTSYS == None:
            print 'Please set ROOTSYS in your environment'
            sys.exit(0)
    def AddJob(self,job):
        if job.ROOTSYS == None:
            job.ROOTSYS=self.ROOTSYS
        if job.LD_LIBRARY_PATH == None:
            job.LD_LIBRARY_PATH=self.LD_LIBRARY_PATH
        if job.PATH == None:
            job.PATH=self.PATH
        if job.PYTHONPATH == None:
            job.PYTHONPATH=self.PYTHONPATH
        self.jobs.append(job)
    def GetNJobs(self):
        return len(self.jobs)
    def ProcessJobsThreaded(self,nthreads,delay=1):
        """ Multi-threaded job processing """
        goodjobs=[]
        for job in self.jobs:
            job.Initialize()
            sys.stdout.flush()
            job.ProcessTemplate()
            if job.Check()==-1:
                if job.shell==True:
                    print 'Skipping job',job.Get('run_name')
                continue
            job.PrintConfig()
            goodjobs.append(job)
        if len(goodjobs)==0: return
        print 'Submitting',len(goodjobs),'jobs in',nthreads,'threads'
        sys.stdout.flush()
        requests = makeRequests(ThreadRun,self.jobs,ThreadFinish)
        main = ThreadPool(nthreads)
        [main.putRequest(req) for req in requests]
        main.wait()
        time.sleep(delay)
        print 'Done processing',len(goodjobs),'threaded jobs'
    def ProcessJobs(self,delay=0.1):
        """ Process jobs. The delay is in addition to the job waiting delay"""
        if len(self.jobs)==0: return
        doDelay = False
        for i,job in enumerate(self.jobs):
            job.Initialize()
            job.delay = str((i+1)*job.Geti('sleep_time'))
            sys.stdout.flush()
            if job.Get('run_mode')=='qsub' or job.Get('run_mode')=='pbs':
                # save a snapshot of this job in a pickle file (to be unpiclked on compute node)
                job.Add('run_mode','normal')
                self.PickleJob(job)
                self.SubmitJobPBS(job)
                continue

            if job.Get('run_mode')=='condor':
                # save a snapshot of this job in a pickle file (to be unpiclked on compute node)
                job.Add('run_mode','normal')
                self.PickleJob(job)
                self.WriteCondorExecutable(job)
                self.WriteCondorSubmitFile(job)
                continue


            job.ProcessTemplate()
            if job.Check()==-1:
                if job.shell==True:
                    print 'Skipping job',job.Get('run_name')
                continue
            if job.Get('run_mode')=='dumpconfig':
                job.PrintConfig(True)
                continue
            job.PrintConfig()
            if job.Get('run_mode')=='normal':
                self.RunLocal(job)
                doDelay = True
            elif job.Get('run_mode')=='debug':
                self.RunLocalDebug(job,1)
                doDelay = True
            elif job.Get('run_mode')=='valgrind':
                self.RunLocalDebug(job,2)
                doDelay = True
            elif job.Get('run_mode')=='silent':
                self.RunLocalSilent(job)
                doDelay = True
            elif job.Get('run_mode')=='config':
                continue
            elif job.Get('run_mode')=='qsub' or job.Get('run_mode')=='pbs':
                continue
            else:
                print 'Unknown run mode (-q flag):',job.Get('run_mode')
                sys.exit(0)
        if doDelay == True:
            time.sleep(delay)
        print 'Done processing',len(self.jobs),'jobs'
    def PickleJob(self,job):
        """ Saves a binary copy of the job (using pickle serialization module)
        ftk.py can then load and process pickled jobs through --preloadjob=/my/pickle_file
        """
        # verbose summary
        f = open(os.path.join(job.path,job.pickle+'.txt'),'w')
        print >>f, job.DumpString()
        f.close()
        # critical environment variables
        f = open(os.path.join(job.path,job.pickle+'.env'),'w')
        for v in job.GetEnvVars():
            print >>f, v
        f.close()
        # actual pickle object
        f = open(os.path.join(job.path,job.pickle),'w')
        pickle.dump(job,f)
        f.close()
    def RunLocalSilent(self,job):
        """ Runs in terminal and outputs straight to log file """
        os.putenv('EXTRA_ARGS',job.extra_args)
        RunJob(job,verbose=False)
    def RunLocal(self,job):
        """ Runs in terminal and outputs to screen """
        (dummy,ret) = RunJob(job,verbose=True)
        print 'Job return code: ',ret
    def RunLocalDebug(self,job,debug):
        """ Runs in terminal and outputs to screen """
        (dummy,ret) = RunJob(job,verbose=True,debug=debug)
        print 'Job return code: ',ret
    def SubmitDummyPBS(self,qsub,dep,ftkpbs):
        """ Submits a dummy qsub job used to distribute dependency strings across real jobs"""
        jobIDs=[]
        command=[qsub,dep,'-h','-o /dev/null','-e /dev/null','-N dummy',ftkpbs]
        print 'Submitting dummy filler job:'
        print '[','dependencies:',dep,' ]'
        pipe=os.popen(' '.join(command),"r")
        for line in pipe.readlines():
            jobIDs.append(line.strip())
        print 'Submitted to PBS:'
        print jobIDs[0]
        self.jobIDs+=jobIDs
        return jobIDs
    def SubmitJobPBS(self,job):
        """ PBS (qsub) submission
        NOTE: this version specifies environment in a separate file (that comes along with the pickle object)
        """
        ftkpbs=os.path.join(job.Get('scripts_dir'),PBS_SCRIPT_NAME)
        # specify prior jobs that must finish first (if any)
        dep=' '
#         if len(self.reqIDs)>0:
#             dep=' -W depend=afterany:'
#             dep = dep+':'.join([str(i) for i in self.reqIDs])
        # splicing dependency string into 32-element blocks
        z=0
        extra=[]
        while z<len(self.reqIDs):
            dep = ' -W depend=afterany:'
            dep += ':'.join([str(i) for i in self.reqIDs[z:z+PBS_DEPLIMIT]]+extra)
            z+=PBS_DEPLIMIT
            # if there are more deps in the pipeline, create an intermediate dummy job
            if z<len(self.reqIDs):
                ids = self.SubmitDummyPBS(job.Get('qsub'),dep,ftkpbs)
                assert len(ids)==1,'Expected 1 qsub job ID, but received %d'%len(ids)
                extra=[str(ids[0]),]
        qsub_opts = job.Get('qsub_opts') + dep
        command=[job.Get('qsub'),qsub_opts,'-h','-v','app='+job.Get('ftk_script')+',rundir='+job.path+',fin='+'"--preloadjob='+os.path.join(job.path,job.pickle)+'"'+',delay='+job.delay+',FTK_ENV_SETUP='+os.path.join(job.path,job.pickle+'.env'),'-o',job.stdout,'-e',job.stderr,'-N',job.run_name,ftkpbs]
        if len(' '.join(command)) > MAX_CMD_LENGTH:
            print 'Job [',job.run_name,'] exceeds maxumum command length:',len(' '.join(command)),'vs',MAX_CMD_LENGTH
            return -1
        print 'Submitting',job.run_name
        pipe=os.popen(' '.join(command),"r")
        for line in pipe.readlines():
            self.jobIDs.append(line.strip())
    def StatusPBS(self):
        """ Summarize the jobs submitted to PBS """
        if len(self.jobIDs)>0:
            print 'Submitted to PBS:'
            if len(self.reqIDs)>0:
                print '[','dependencies =',':'.join(self.reqIDs),']'
            for id in self.jobIDs:
                print id
