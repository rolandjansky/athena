#!/usr/bin/env python

"""Utility for reading job info back in from produced job tables"""

__author__ = "thomas.kittelmann@cern.ch"

import os.path
import string

def pathSplit(thepath):
    res=[]
    (head,tail)=os.path.split(thepath)
    while head and tail:
        res+=[tail]
        (head,tail)=os.path.split(head)
    res.reverse()
    return res

def joinListToString(l):
    return ' '.join('%s'*len(l)) % l

class PerJobTable:

    def __init__(self,filename,domaincalclib):
        #Analyse filename to get info about test and build.
        assert os.path.exists(filename)
        ok = self.__analyseFileName(filename)
        assert ok and self.test, "Could not extract all information from filename %s"%filename
        if ok:
            ok = self.__parseFile(filename)
        assert ok, "Could not parse information from %s"%filename
        if ok:
            ok = self.__addUserJobIdAndPackageInfo(domaincalclib)
        assert ok, "Could not find userJobId and package info for %s"%filename

    def __analyseFileName(self,filename):
        path_parts = pathSplit(os.path.normpath(filename))
        if len(path_parts)<3: return False
        path_parts=path_parts[len(path_parts)-3:]
        if path_parts[1]!='results': return False
        if not path_parts[2].startswith('table_jobs_') or not path_parts[2].endswith('.txt'):
            return False

        s=path_parts[0]
        #s should have the format <testname>-<build>-<cache>-<cmtconfig>.

        #both build and cmtconfig can have internal dashes, so we
        #first find cmtconfig by assuming it has a standard beginning:

        self.cmtconfig=None
        for c in ['i686','x86_64','i386','i586']:
            if '-'+c+'-' in s:
                self.cmtconfig=c+s.split(c)[-1]
                break
        if not self.cmtconfig:
            return False
        s=s[0:len(s)-len(self.cmtconfig)-1]
        self.cache = s.split('-')[-1]
        s=s[0:len(s)-len(self.cache)-1]
        (self.test,self.build)=string.split(s,'-',1)
        return True

    def __parseFile(self,filename):
        self.unit=None
        self.measurement=None
        self.njobs=None
        self.jobs=[]
        self.jobdata=[]

        for l in open(filename):
            cols=l.split()
            if not cols: continue
            if l.startswith('-'):
                if self.unit or not '[' in l: continue
                pre=None
                for col in cols:
                    if '[' in col:
                        self.unit=col.replace('[','').replace(']','')
                        self.measurement=pre
                        break
                    pre=col
            elif cols[0]=='date':
                self.njobs=len(cols)-1
                self.jobs=cols[1:]
            else:
                if not self.njobs or self.njobs!=len(cols)-1: return False
                datestr=cols[0]
                cols=cols[1:]
                for i in range(self.njobs):
                    val=cols[i]
                    if val=='-': continue
                    self.jobdata+=[(datestr,self.jobs[i], float(val))]
        return True
    def __addUserJobIdAndPackageInfo(self,domaincalclib):
        assert self.test and self.jobdata and self.jobs
        calc=domaincalclib.getCalcByName(self.test)
        if not calc: return False
        cdjobdefs=calc.getCDJobDefinitions("testkey")
        if not cdjobdefs: return False
        def getJobDef(jobname):
            for cdjobdef in cdjobdefs:
                if cdjobdef.getJobName()==jobname: return cdjobdef

        self.jobrttinfo={}
        for job in self.jobs:
            jobdef=getJobDef(job)
            if not jobdef: return False
            try: userjobid = jobdef.getUserJobId()
            except: userjobid = None
            jobdef.getDBJobName()
            self.jobrttinfo[job]={'userJobId':userjobid,
                                  'package':jobdef.getDBJobPackage(),
                                  'dbjobname':jobdef.getDBJobName()}
        return True

    def toList(self):
        l=[]
        for (datestr,job,val) in self.jobdata:
            info = self.jobrttinfo[job]
            l+=[(datestr,info['package'],info['dbjobname'],
                 self.build,self.cache,self.cmtconfig,
                 self.measurement,self.unit,val)]

        return l

    def toASCII(self):
        out=[]
        for l in self.toList():
            out+=[joinListToString(l)]
        return out

def run(opt):
    filename='/home/tkittel/atlas/cmtwork/work-15.3.0/Tools/PmbUtils/run/pmbdata'\
              '/backup_data/rdotoesd-dev-prod-i686-slc4-gcc34-opt/results/table_jobs_vmem.txt'

    from PmbUtils.DomainCalcLib import DomainCalcLib
    calclib=DomainCalcLib()

    alldata=[]
    for filename in opt.files:
        pjt = PerJobTable(filename,calclib)
        alldata.extend(pjt.toList())
    alldata.sort()

    if opt.output_txt:
        outstr=''
        for l in alldata:
            outstr+=joinListToString(l)+'\n'
        print "Writing %i datapoints to %s"%(len(alldata),opt.output_txt)
        fh = open(opt.output_txt, 'w')
        fh.write(outstr)
        fh.close()

    if opt.output_pickle:
        print "Writing %i datapoints to %s"%(len(alldata),opt.output_pickle)
        import pickle
        fh = open(opt.output_pickle, 'w')
        pickle.dump(alldata,fh)
        fh.close()

    return

def parseArgs():
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("--outtxt", dest="output_txt",
                      help="Output parsed info into FILE as columns in a simple text file", metavar="FILE")
    parser.add_option("--outpickle", dest="output_pickle",
                      help="Output parsed info into pickle FILE as one long list", metavar="FILE")
    (opt, args) = parser.parse_args()

    def fixPath(f): return os.path.expanduser(f) if f else None
    opt.output_txt = fixPath(opt.output_txt)
    opt.output_pickle = fixPath(opt.output_pickle)
    if not opt.output_txt and not opt.output_pickle:
        parser.error('Please specify output with --outtxt or --outpickle')
    opt.files=[]
    for filename in args:
        filename=os.path.expanduser(filename)
        if not os.path.exists(filename):
            parser.error('Unknown file %s'%filename)
        opt.files+=[os.path.abspath(filename)]
    if not opt.files:
        parser.error('Please supply pmb-domain table files to parse for information')
    return opt

if __name__ == "__main__":
    import sys
    sys.exit(run(parseArgs()))
