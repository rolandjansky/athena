#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



"""
  --text  - calls
       pprof --text `which python` my.athena.profile  > GProfile.txt
  --callgrind
       pprof --callgrind `which python` my.athena.profile > callgrind.out.GProfile
  
  --sqlite
  --web

TODO:
  separate into different files for the classes
  allow for some options (argparse)

  extend list of timers to be added to the list.

"""
import os
import shutil
import datetime as dt
#from sqlite3 import IntegrityError, OperationalError

from MySQLdb import IntegrityError, OperationalError

def getHost(hostname=None):
    if hostname is None:
        hostname=os.getenv('HOSTNAME')

    hout=os.popen('lshosts '+hostname)
    res=hout.read()
    hout.close()
    res2=[l.split() for l in res.strip().split('\n')]
    res3=[ l[:8]+[' '.join(l[8:])] for l in res2 ]
    return res3

def preparation(pname,tname,cname,doText,doCallgrind):
    #pname='test.athena.profile'
    pypath = os.popen('which python').read().strip()
    
    #tname='athena.profile.txt'
    #cname='callgrind.out'
    if doText and not os.path.exists(tname) and not os.path.exists(cname):
        # omit if one off the outputs already exist (to save time for post mortem runs)
        print 'creating',tname
        tout = os.popen('pprof --text '+pypath+' '+pname)
        f = file(tname,'w')
        f.write(tout.read())
        f.close()
    
    if doCallgrind and not os.path.exists(cname):
        print 'creating',cname
        cout = os.popen('pprof --callgrind '+pypath+' '+pname)
        f = file(cname,'w')
        f.write(cout.read())
        f.close()

def postprocessing():
    "copy a few files to afs"

    basedir="/afs/cern.ch/atlas/groups/Simulation/rtt/backup"
    today=dt.date.today().strftime("%y%m%d")

    #AtlasArea=/afs/cern.ch/atlas/software/builds/nightlies/devval/AtlasProduction/rel_2
    atlasArea=os.getenv('AtlasArea')
    dummy,branch,proj,rel=atlasArea.rsplit('/',3)

    #LS_SUBCWD=/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_2/devval/build/x86_64-slc5-gcc43-opt/offline/SimCoreTests/AtlasG4_muons
    subcwd=os.getenv('LS_SUBCWD')
    dummy,package,job=subcwd.rsplit('/',2)

    # do this only for devval
    if branch != 'devval':
        return


    path='/'.join([basedir,branch,package,job,today])

    if not os.path.exists(path):
        os.makedirs(path)

        files=['callgrind.out','env.log','athena.profile.txt','config.txt',job+'_log',job+'_script_log']

        for f in files:
            if os.path.exists(f):
                print 'copy',f,'to',path
                shutil.copy(f, path)

        # install dynamic db parsers
        f='/afs/cern.ch/atlas/groups/Simulation/rtt/prof_display.php'
        shutil.copy(f,'.')
        f='/afs/cern.ch/atlas/groups/Simulation/rtt/prof_display2.php'
        shutil.copy(f,'.')
    else:
        print "WARNING", path, "alread exists"
        print "WARNING files not copied"

import string
import cgi

class HtmlFormator:
    header="""<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html  xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
  <head>
    <meta http-equiv="Content-type" content="text/html;charset=UTF-8" />
    <meta http-equiv="Content-Script-Type" content="text/javascript" />
    <meta http-equiv="Content-Style-Type" content="text/css" />
    <title>%(title)s</title>
    %(style)s
  </head>
  <body>
"""
    footer="""  </body>
</html>"""
    
    def __init__(self,title):
        self.title=cgi.escape(title)
        self.style=''
        self.content=[]
        self.setStyle()

    def addHeader(self):
        self.content.append(self.header%self.__dict__)

    def addFooter(self):
        self.content.append(self.footer)

    def write(self,fname):
        print 'creating',fname
        f = file(fname,'w')
        for l in self.content:
            f.write(l)

    def setStyle(self):

        self.style="""<style type="text/css">
        td {
        text-align: left;
        padding: 5px;
        }
        tr > td td td td td td {
        text-align: left ;
        padding: 5px;
        }
        tr:nth-of-type(odd) {
          background-color:#ccc;
          }
          
        </style>
        """
    
    def addTable(self,lol):
        self.content.append( '<table>' )
        self.content.append( '<tr><th>'+('</th><th>'.join(lol[0]))+'</th></tr>' )
        for sublist in lol[1:]:
            self.content.append( '<tr><td>'+('</td><td>'.join(sublist))+'</td></tr>' )
        self.content.append( '</table>' )

    @classmethod
    def makeLibAnchor(cls,lib):
        lnk='lib_'+lib+'.html'
        return '<a href="'+lnk+'">'+cgi.escape(lib)+'</a>'

    def addLink(self,lnk,title):
        self.content.append('<a href="'+lnk+'">'+cgi.escape(title)+'</a>')


    def addHeading(self,txt,tag='h3'):
        self.content.append( '<%s>%s</%s>'%(tag,cgi.escape(txt),tag) )

    @classmethod 
    def row_major(cls,alist, sublen):      
        return [alist[i:i+sublen] for i in range(0, len(alist), sublen)]

    @classmethod 
    def col_major(cls,alist, sublen):
        numrows = (len(alist)+sublen-1) // sublen 
        return [alist[i::sublen] for i in range(numrows)]

    """adapted from : http://stackoverflow.com/questions/1475123/easiest-way-to-turn-a-list-into-an-html-table-in-python"""

def test_table():
    
    html=HtmlFormator("Hallo")
    html.addHeader()
    html.addTable(html.row_major(string.letters[26:],6))
    html.addFooter()

class LibraryEstimator:
    def __init__(self):
        pass

    @classmethod
    def getLib(self,fn,fl):
        lib=self.checkFileLocation(fl)
        if lib!='':
            return lib
        lib=self.checkFunctionName(fn)
        if lib!='':
            return lib
        return 'unknown'
        
    @classmethod
    def checkFileLocation(self,fl):
        if fl==0:
            return ''
        dd={'/CLHEP':'CLHEP',
            '/HepMC':'HepMC',
            '/expat':'expat',
            '/frontier':'frontier',
            '/c++':'GCC',
            '/Python':'Python',
            '/xercesc':'xerces'}
        for key in dd.keys():
            if fl.fl.find(key)>=0 :
                return dd[key]
        if fl.fl.startswith('G4') and not self.checkAtlasG4(fl.fl):
            return 'Geant4'
        
        return ''

    @classmethod
    def checkAtlasG4(self,name):
        atlasg4=['G4reconHiggs', 'G4simMuon', 'G4digitHiggs', 'G4mplAtlasIonisation',
                 'G4AtlasTests_entries', 'G4BiasedMuPairProduction',
                 'G4EmStandardPhysics_MuBias', 'G4FastSimulationDict', 'G4TestAlg',
                 'G4AtlasControlDict', 'G4reconMuon', 'G4SMuonPlus', 'G4CosmicFilter',
                 'G4AncillarySDDict', 'G4SimTPCnv', 'G4BiasedMuBremsstrahlungModel',
                 'G4AncillarySimEventDict', 'G4AtlantisDumper_options', 'G4TrackCounter',
                 'G4SElectronPlus', 'G4SimTimer', 'G4EventAnalyzer', 'G4AtlasAlgDict',
                 'G4CosmicAndFilter', 'G4combinedinput_trf', 'G4RPC_Digitizer',
                 'G4AtlasAlg', 'G4SimTPCnvDict', 'G4StepLimitation',
                 'G4AtlasApps_TestConfiguration', 'G4AtlasTests_load', 'G4digitMuon',
                 'G4AtlasAlg_load', 'G4ProcessHelper', 'G4AtlasAlg_entries', 'G4SimMem',
                 'G4SElectronMinus', 'G4STauMinus', 'G4FieldDict',
                 'G4CommandInterface', 'G4BiasedMuBremsstrahlung', 'G4HitFilter',
                 'G4AtlasRunManager', 'G4mplAtlasTransportation', 'G4AtlasUtilitiesDict',
                 'G4AtlasTests_TestConfiguration', 'G4SMuonMinus',
                 'G4mplAtlasIonisationModel', 'G4mplAtlasIonisationWithDeltaModel',
                 'G4simHiggs', 'G4BiasedMuPairProductionModel',
                 'G4CosmicOrFilter', 'G4UIcmdWithStrings', 'G4STauPlus',
                 'G4AtlantisDumper', 'G4mplEqMagElectricField', 'G4StepLimitProcess']
        if name in atlasg4:
            return True
        return False

    @classmethod
    def checkFunctionName(self,fn):
        #LArG4 etc.
        dd=[('HepMcParticleLink::','GeneratorObjects'),
            ('InDetDD::PixelDiodeMap','InDetReadoutGeometry'),
            ('InDetDD::PixelModuleDesign','InDetReadoutGeometry'),
            ('InDetDD::PixelDiodeMatrix','InDetReadoutGeometry'),
            ('InDetDD::SiDetectorElement','InDetReadoutGeometry'),
            ('InDetDD::SiLocalPosition','InDetReadoutGeometry'),
            ('InDetDD::SCT_BarrelModuleSide','InDetReadoutGeometry'),
            ('InDetDD::SCT_ForwardModuleSide','InDetReadoutGeometry'),
            ('InDetDD::SCT_ForwardFrameTransformation','InDetReadoutGeometry'),
            ('SiPropertiesTool','SiPropertiesTool'),
            ('SiliconProperties','SiPropertiesTool'),
            ('dsfmt','AtlasCLHEP_RandomGenerators'),
            ('CLHEP::dSFMTEngine','AtlasCLHEP_RandomGenerators'),
            ('CLHEP::RandGaussZiggurat','AtlasCLHEP_RandomGenerators'),
            ('master.0.gbmagzsb_','BFieldStand'),
            ('CLHEP','CLHEP'),
            ('HepGeom','CLHEP'),
            ('SiCharge::','InDetSimEvent'),
            ('SiTotalCharge','InDetSimEvent'),
            ('SiHit','InDetSimEvent'),
            ('HitIdHelper','HitManagement'),
            ('Trk::LocalPosition','TrkEventPrimitives'),
            ('Trk::MagneticFieldTool','TrkMagFieldTools'),
            ('SiChargedDiode','SiDigitization'),
            ('SiSurfaceCharge::SiSurfaceCharge','SiDigitization'),
            ('DiodeCollectionInserter::','SiDigitization'),
            ('PixelDistortionsTool','PixelConditionsTools'),
            ('PixelD','PixelDigitization'),
            ('Pixel','Pixel'),
            ('SCT_SurfaceChargesGenerator','SCT_Digitization'),
            ('SCT_FrontEnd','SCT_Digitization'),
            ('SCT_Digitization','SCT_Digitization'),
            ('SCT_Amp','SCT_Digitization'),
            ('SCT_DCSConditionsTool','SCT_ConditionsTools'),
            ('SCT_SiliconConditionsTool','SCT_ConditionsTools'),
            ('SCT','SCT'),
            ('TRT_SimDriftTimeTool','TRT_DriftTimeSimUtils'),
            ('TRT_PAI','TRT_PAI_Process'),
            ('TRTTimeCorrection','TRT_Digitization'),
            ('TRTElectronics','TRT_Digitization'),
            ('TRTProcessingOfStraw','TRT_Digitization'),
            ('TRTDig','TRT_Digitization'),
            ('LArHitContainerCnv','LArAthenaPool'),
            ('CaloTriggerTowerSvc','CaloTriggerTool'),
            ('bipfunc','CscCalibTools'),
            ('MDT_Response','MDT_Response'),
            ('Amplifier::AddCluster','MDT_Response'),
            ('Muon','MuonSpectrometer'),
            ('boost::','boost'),
            ('SG::','StoreGate'),
            ('StoreGateSvc','StoreGate'),
            ('__ieee754_','libm'),
            ('__exp1','libm'),
            ('LoggedMessageSvc','AthenaServices'),
            ('ServiceManager','GaudiSvc'),
            ('IdDictDictionary','IdDict'),
            ('TRT','TRT'),
            ('LArG4','LArG4'),
            ('LAr','LAr'),
            ('FADS','FADS'),
            ('std::','std-lib'),
            ('Tile','Tile'),
            ('DataPool','DataModel'),
            ('Athena::getMessageSvc','AthenaKernel'),
            ('Gaudi','GaudiSvc'),
            ('pool::','POOL'),
            ('Xrd','ROOT'),
            ('TROOT','ROOT')]
        for key,lib in dd:
            if fn.fn.startswith(key) :
                return lib
        #G4 ... but        
        if fn.fn.startswith('G4') and not self.checkAtlasG4(fn.fn):
            return 'GEANT4'
        if fn.fn[0]=='T' and fn.fn[1] in string.ascii_uppercase and fn.fn[2] in string.ascii_lowercase:
            return 'ROOT'
        return ''
    
        

class FileLocation:
    def __init__(self,fl_id=0,fl_name=''):
        self.id=fl_id
        self.fl=fl_name
        self.lib=''

        self.dbid=None

    def __repr__(self):
        return 'FL(%s; %d)'%(self.fl,self.id)


class FunctionCall:
    def __init__(self,fn_id=0,fn='',fl_id=0):
        self.id=fn_id
        self.fn=fn
        self.fl_id=fl_id  # file id
        #calls to
        self.calls={} # id : hits
        self.parents=[]
        self.children=[]
        self.selfcost=0
        self.totalcost=0
        self.loopcall=0
        self.lib=''

        self.isHtml=False
        self.htmlName=''

        self.issue=0
        self.issueMax=0
        self.dbid=None

    def add(self,id,calls):
        if not self.calls.has_key(id):
            self.calls[id]=calls
        else:
            self.calls[id]+=calls
        self.totalcost+=calls
        if id==0:
            self.selfcost+=calls

    def __repr__(self):
        return 'sc=%d tc=%d loop=%d name=%s (id=%d)'%(self.selfcost,self.totalcost,self.loopcall,self.fn,self.id)

    def makeHtmlName(self):
        print self.fn
        allowed=string.ascii_letters+string.digits
        f = lambda c: c in allowed and c or '_'
        htmlName = ''.join([ f(c) for c in self.fn ])
        if self.issueMax!=0:
            htmlName+='_%03d'%self.issue
        self.htmlName='fn_'+htmlName+'.html'
        print self.htmlName
        return self.htmlName

    def makeAnchor(self):
        if self.htmlName!='':
            return '<a href="'+self.htmlName+'">'+cgi.escape(self.fn)+'</a>'
        return cgi.escape(self.fn)

class CEntry:
    def __init__(self):
        self.fn=None
        self.fl=None
        self.cfl=None
        self.cfn=None
        self.calls=0



class Parser:
    def __init__(self):
        self.filelist={}
        self.functions={}
        self.libraries={}
        self.ignore={}
        self.efn=0
        self.setIgnore()

        self.total=0

        self.featured=[]
        #'LArWheelSolid::DistanceToIn','LArWheelCalculator::DistanceToTheNeutralFibre','LArWheelCalculator::DistanceToTheNearestFan','LArWheelSolid::search_for_nearest_point']

        self.htmlGlobal='athena.profile.html'
        self.htmlPath='prof/'

        self.hostname=None
        self.atlasArea=None
        self.subcwd=None
        #self.hostinfo=getHost()
        self.hostinfo=None
        self.rdatetime=dt.datetime.now()

    def readEnvironment(self,readFile=False,filename='env.log'):
        if readFile:
            f=file(filename)
            d={}
            for l in f:
                l=l.strip()
                if l.find('=')>0:
                    k,v=l.split('=',1)
                    d[k]=v
            self.hostname=d['HOSTNAME']
            self.atlasArea=d['AtlasArea']
            self.subcwd=d['LS_SUBCWD']
            print 'read',self.hostname
            print 'atlasArea=',d['AtlasArea']
            print 'subcwd=',d['LS_SUBCWD']

            lsbfile=d['LSB_OUTPUTFILE']
            f=file(lsbfile)
            pattern='Results reported at '
            for l in f:
                if l.startswith(pattern):
                    d=l.strip(pattern).strip()
                    print d
                    self.rdatetime=dt.datetime.strptime(d,'%a %b %d %H:%M:%S %Y')
                    print 'found',  self.rdatetime
        else:
            self.hostname=os.getenv('HOSTNAME')
            self.atlasArea=os.getenv('AtlasArea')
            self.subcwd=os.getenv('LS_SUBCWD')

    def linkDown(self):
        #add missing keys
        for fn in self.functions.values():
            if fn.id<0:
                self.functions[fn.id]=fn
                self.functions.pop(fn.fn)
        for fn in self.functions.values():
            fn.children=[]
            for i,c in fn.calls.iteritems():
                if i!=0:
                    fn.children.append(self.functions[i])

    def linkUp(self):
        for fn in self.functions.values():
            for child in fn.children:
                child.parents.append(fn)


    def createDict(self):
        self.fn_dict={}
        for fn in self.functions.values():
            if self.fn_dict.has_key(fn.fn):
                afn=self.fn_dict[fn.fn]
                print afn
                afn.issueMax+=1
                print afn.issueMax
                
                fn.issue=afn.issueMax
                print 'created an issue of ',fn
            else:
                self.fn_dict[fn.fn]=fn
        
    def setIgnore(self):
        self.ignore['operator']='google-perftools'

    def setFeatured(self,rel=0.0025,tot=0.005):
        total=float(self.total)
        relHit=int(total*rel)
        totHit=int(total*tot)
        for fn in self.functions.values():
            if fn.selfcost>=relHit or fn.totalcost>=totHit:
                if fn.fn not in self.featured:
                    self.featured.append(fn.fn)
            

    def parse(self,cfilename='callgrind.out'):
        f=file(cfilename)
        cg=CEntry()
        for l in f:
            if l.startswith("events"):
                pass
            # Blank line signifies new entry
            elif len(l.strip())==0:
                cg=CEntry()
            elif l.startswith('fl='):
                cg.fl=self.getFl(l)
            elif l.startswith('fn='):
                cg.fn=self.getFn(l,cg.fl)
            elif l.startswith('cfl='):
                cg.cfl=self.getFl(l)
            elif l.startswith('cfn='):
                cg.cfn=self.getFn(l,cg.cfl)
            elif l.startswith('calls='):
                cg.calls=int(l.split('=')[1].split(' ')[0])
                if cg.cfn is not None:
                    if cg.cfn.id==cg.fn.id:
                        cg.fn.loopcall+=cg.calls
                    else:
                        cg.fn.add(cg.cfn.id,cg.calls)
                else:
                    print 'Error in calls parsing'
            else:
                c=int(l.split(' ')[1])
                if c!=cg.calls and cg.cfn!=None:
                    print 'Error mismatch',c,cg.calls
                    print ' in fn=',fn.fn
                    print ' line=',l
                if cg.cfn==None:
                    # selfcost
                    cg.fn.add(0,c)
        self.getTotalCounts()
        self.setFeatured()
        self.guessLibraries()

    def guessLibraries(self):
        for fn in self.functions.values():
            fl=0
            if fn.fl_id!=0:
                fl=self.filelist[fn.fl_id]
            fn.lib=LibraryEstimator.getLib(fn,fl)
            if self.libraries.has_key(fn.lib):
                self.libraries[fn.lib]+=[fn]
            else:
                self.libraries[fn.lib]=[fn]
        

    def ignoreFn(self,fn):
        if fn.fn in self.ignore.keys():
            fl=self.filelist[fn.fl_id].fl
            if len(fl)>2 and len(self.ignore[fn.fn])>2 and fl.find(self.ignore[fn.fn])>=0:
                return True
        if self.filelist.has_key(fn.fl_id):
            fl=self.filelist[fn.fl_id].fl
            if len(fl)>2 and fl.find(self.ignore.values()[0])>=0:
                self.ignore[fn.fn]=self.ignore.values()[0]
                return True
        return False

    def getTotalCounts(self):
        if self.total!=0 :
            return self.total
        total=0
        ig=0
        #print 'len=',len(self.functions)
        for k,v in self.functions.iteritems():
            if self.ignoreFn(v):
                #print 'found ',v.fn,self.ignore[v.fn]
                ig+=v.selfcost
                continue
            total+=v.selfcost
        print 'total=',total,' ig=',ig
        self.total=total
        return total

    def getFn(self,line,fl_id):
        line=line.strip()
        pre,val=line.split('=',1)
        parts=val.split(' ',1)
        fn_sid = parts[0].strip('()')
        if len(parts)>1:
            fn_name=parts[1][:255]
            if fn_name.startswith('0x'):
                fn_name='(??)'
        else:
            fn_name=None
        try:
            fn_id=int(fn_sid)
            if self.functions.has_key(fn_id):
                return self.functions[fn_id]
            else:
                fn= FunctionCall(fn_id,fn_name,fl_id)
                self.functions[fn_id]=fn
                return fn
        except:
            if self.functions.has_key(fn_sid):
                return self.functions[fn_sid]
            else:
                self.efn-=1
                fn=FunctionCall(self.efn,fn_sid,fl_id)
                self.functions[fn_sid]=fn
                return fn
            
    def getFl(self,line):
        line=line.strip()
        pre,val=line.split('=')
        if val=='??':
            fl= FileLocation(0,"??")
            self.filelist[0]=fl
            return 0
        parts=val.split(' ')
        fl_sid = parts[0].strip('()')
        if len(parts)>1:
            fl_name=parts[1]
        else:
            fl_name=None
        # fl_id here is what is given in the flog script, e.g.
        # cfl=(480) /build/atnight/localbuilds/nightlies/18.X.0/GAUDI/rel_nightly/InstallArea/include/GaudiKernel/implements.h
        # Now filelist is a dictionary of the ids from the script to "FileLocation" objects
        fl_id=int(fl_sid)
        if self.filelist.has_key(fl_id):
            return fl_id
        else:
            fl= FileLocation(fl_id,fl_name)
            self.filelist[fl_id]=fl
            return fl_id

    def printSorted(self,fnlist=None):
        if fnlist is None:
            fnlist=self.functions.values()
        total=float(self.total)
        #        total=97828.
        print 'total=',total
        acul=0.
        from operator import attrgetter
        allfn=sorted(fnlist,key=lambda fn: (-fn.selfcost, fn.fn), reverse=False)
        for fn in allfn:
            if self.ignoreFn(fn):
                continue
            frsc=fn.selfcost/total*100.
            acul+=frsc
            frtot=fn.totalcost/total*100.
            print '%8d %5.1f%% %5.1f%% %8d %5.1f%% %s'%(fn.selfcost,frsc,acul,fn.totalcost,frtot,fn.fn)
        self.sortedall=allfn


    def printWebSorted(self,fnlist=None,title='Performance',fname=None,lib=None):
        if not os.path.exists(self.htmlPath):
            os.mkdir(self.htmlPath)
        
        if fname is None:
            fname=self.htmlGlobal
        if fnlist is None:
            fnlist=self.functions.values()
        total=float(self.total)
        acul=0.
        from operator import attrgetter
        allfn=sorted(fnlist,key=lambda fn: (-fn.selfcost, fn.fn), reverse=False)
        tab=[]
        tab.append(['self hits','rel.', 'acc.','total hits','rel.','function name','library'])
        for fn in allfn:
            if lib!=None and lib!=fn.lib:
                continue
            if self.ignoreFn(fn):
                continue
            frsc=fn.selfcost/total*100.
            acul+=frsc
            frtot=fn.totalcost/total*100.
            row='%8d %5.1f%% %5.1f%% %8d %5.1f%%'%(fn.selfcost,frsc,acul,fn.totalcost,frtot)
            tab.append(row.split()+[fn.makeAnchor(),HtmlFormator.makeLibAnchor(fn.lib)])

        html=HtmlFormator(title)
        html.addHeader()
        if lib is not None:
            html.addLink(self.htmlGlobal,'back to global list')
        
        if self.hostinfo is not None:
            html.addTable(self.hostinfo)
        html.addHeading(title)
        html.addTable(tab)
        html.addFooter()
        html.write(self.htmlPath+fname)
        
            
    def printWebSorted2(self,fnlist,fn=None):
        total=float(self.total)
        acul=0.
        aculc=0.
        from operator import attrgetter

        allfn=sorted(fnlist,key=lambda fn: (-fn.totalcost, fn.fn), reverse=False)
        tab=[]
        if fn is None:
            tab.append(['self hits','rel.', 'total hits','rel.','acc.','function name','library'])
        else:
            tab.append(['call hits','rel.','acc.',
                        'self hits','rel.', 'total hits','rel.','acc.','function name','library'])
        for fn in allfn:
            
            
            if self.ignoreFn(fn):
                continue
            frsc=fn.selfcost/total*100.
            frtot=fn.totalcost/total*100.
            acul+=frtot
            row='%8 %5.1f%% %5.1%% %8d %5.1f%% %8d %5.1f%% %5.1f%%'%(fn.selfcost,frsc,fn.totalcost,frtot,acul)
            tab.append(row.split()+[fn.makeAnchor(),''])

        
        html=HtmlFormator("Performance")
        html.addHeader()
        html.addTable(tab)
        html.addFooter()


    def printWebSorted3(self,func=None):
        if func.isHtml:
            return
        
        total=float(self.total)
        acul=0.
        aculc=0.
        from operator import attrgetter

        # children
        tab=[]
        tab.append(['call hits','rel.','acc.',
                    'self hits','rel.', 'total hits','rel.','acc.','function name','library'])
        for i,c in func.calls.iteritems():
            if i != 0:
                if not self.functions.has_key(i):
                    print 'not found ',i
                    continue
                fn=self.functions[i]
            else:
                fn=func
            frc=c/total*100.
            aculc+=frc

            frsc=fn.selfcost/total*100.
            frtot=fn.totalcost/total*100.
            acul+=frtot
            row='%8d %5.1f%% %5.1f%% %8d %5.1f%% %8d %5.1f%% %5.1f%%'%(c,frc,aculc,fn.selfcost,frsc,fn.totalcost,frtot,acul)
            tab.append(row.split()+[fn.makeAnchor(),''])

        # parents
        parTab=[]
        acul=0.
        aculc=0.

        parTab.append(['call hits','rel.','acc.',
                       'self hits','rel.', 'total hits','rel.','acc.','function name','library'])
        for fn in func.parents:
            c=fn.calls[func.id]
            frc=c/total*100.
            aculc+=frc

            frsc=fn.selfcost/total*100.
            frtot=fn.totalcost/total*100.
            acul+=frtot
            row='%8d %5.1f%% %5.1f%% %8d %5.1f%% %8d %5.1f%% %5.1f%%'%(c,frc,aculc,fn.selfcost,frsc,fn.totalcost,frtot,acul)
            parTab.append(row.split()+[fn.makeAnchor(),''])

        
        html=HtmlFormator("Performance")
        html.addHeader()
        html.addLink(self.htmlGlobal,'back to global list')
        html.addHeading(func.fn)
        html.addHeading('calls to other routines','h4')
        html.addTable(tab)
        html.addHeading('callees of this routine','h4')
        html.addTable(parTab)
        html.addFooter()
        if func.htmlName=='':
            func.makeHtmlName()
        html.write(self.htmlPath+func.htmlName)
        func.isHtml=True

class DataBase(object):
    def __init__(self,fname, dryrun=False):
        self.fname=fname
        self.dryrun=dryrun
        self.cnx=None
        self.cur=None
        self.hostname=None
        self.atlasArea=None
        self.subcwd=None
        self.rdatetime=dt.datetime.now()        


    def readEnvironment(self,parser):
        self.hostname=parser.hostname
        self.atlasArea=parser.atlasArea
        self.subcwd=parser.subcwd
        self.rdatetime=parser.rdatetime

    def openConnection(self):
        #import sqlite3
        import MySQLdb
        self.cnx=MySQLdb.connect(db='asimperf',port=5500,host='dbod-asimperf.cern.ch',user='asdb_writer',passwd='db.wr1t3r')
        #self.cnx = sqlite3.connect(self.fname)
        self.cur = self.cnx.cursor()

    def _execute(self,cmd,args=()):
        print 'SQL:',cmd,args
        self.cur.execute(cmd,args)

    def _dumpTable(self,t):
        #t = (symbol,)
        #cur.execute('select * from stocks where symbol=%s', t)
        self._execute('select * from %s'%t)
        header= [ s[0] for s in self.cur.description ]
        format= ' '.join([ '%25s' for s in self.cur.description])
        print format%tuple(header)
        for res in self.cur.fetchall():
            print format%res


    def insert(self,tname='t_run',hargs=[],args=[]):
        "create prepared statement and execute"
        keys=','.join(hargs)
        vals=','.join(('%s',)*len(args))
        cmd="insert into %s (%s) values (%s)"%(tname,keys,vals)
        print cmd,args
        #self.cur.execute(cmd,args)
        self._execute(cmd,args)
        self.rid=self.cur.lastrowid
        return self.rid

    def dummyHostInfo(self):
        hostInfo={'HOST_NAME': 'lxplus443',
                  'RESOURCES': '(intel plus quadcore wan)',
                  'cpuf': '1.9',
                  'maxmem': '48161M',
                  'maxswp': '51207M',
                  'model': 'vi_10_24',
                  'ncpus': '8',
                  'server': 'Yes',
                  'type': 'SLC5_64'}
        return hostInfo

    def addHost(self,hostname):
        print 'check',hostname
        #info = getHost(hostname)
        #print info

        cmd='select id from t_hosts where hostname=%s'
        self._execute(cmd,(hostname,))
        res=self.cur.fetchone()
        if res is None:
            # add to database
            print hostname,"not found, adding it now"

            hargs=['hostname', 'os_type', 'model', 'cpuf', 'ncpus', 'maxmem', 'maxswp', 'server', 'resources']
            bargs=['HOST_NAME', 'type', 'model', 'cpuf', 'ncpus', 'maxmem', 'maxswp', 'server', 'RESOURCES']
            hostInfo=getHost(hostname)
            hostInfo=dict(zip(*hostInfo))
            hostInfo['maxmem']=int(hostInfo['maxmem'].strip('M'))
            hostInfo['maxswp']=int(hostInfo['maxswp'].strip('M'))
            args=[hostInfo[h] for h in bargs]
            
            self.insert('t_hosts',hargs,args)
        if not self.dryrun:
            self.cnx.commit()

    def addRun(self):

        #AtlasArea=/afs/cern.ch/atlas/software/builds/nightlies/devval/AtlasProduction/rel_2
        #atlasArea=os.getenv('AtlasArea')
        dummy,branch,proj,rel=self.atlasArea.rsplit('/',3)

        #LS_SUBCWD=/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_2/devval/build/x86_64-slc5-gcc43-opt/offline/SimCoreTests/AtlasG4_muons
        #subcwd=os.getenv('LS_SUBCWD')
        dummy1,cmt,dummy2,package,job=self.subcwd.rsplit('/',4)

        rdate,rtime=self.rdatetime.strftime('%Y-%m-%d %H:%M:%S').split()

        hostInfo=getHost(self.hostname)
        hostInfo=dict(zip(*hostInfo))

        machine=hostInfo['HOST_NAME']
        KSI2K_fac=float(hostInfo['cpuf'])
        hargs=['branch', 'rel', 'cmt', 'rdate', 'rtime', 'machine', 'KSI2K_fac', 'package', 'job']
        args=[locals()[x] for x in hargs]

        # Check that host exists and insert if not
        self.addHost(machine)

        id=self.insert('t_run',hargs,args)
        if not self.dryrun:
            self.cnx.commit()
        
        return id

    def addFile(self,fl):
        "add file entry to db and return id"

        # check if already registered
        if fl.dbid is not None:
            return fl.dbid

        # check if database
        cmd='select id from t_files where fl_name=%s'
        self._execute(cmd,(fl.fl,))
        res=self.cur.fetchone()

        if res is None:
            print 'INFO - file',fl.fl,'not found in db - adding to database'
            # adding it
            id=self.insert('t_files',['fl_name'],[fl.fl])
        else:
            # receive existing entry
            id=res[0]

        fl.dbid=id
        return id

    def addFunction(self,fn):
        #print "Adding function ", fn.fn
        if self.parser.filelist.has_key(fn.fl_id):
            fl=self.parser.filelist[fn.fl_id]
            fid=self.addFile(fl)
        else:
            fid=0
        cmd='select id from t_functions where fn_name=%s and fid=%s and issue=%s'
        self._execute(cmd,(fn.fn,fid,fn.issue))
        res=self.cur.fetchone()
        if res is None:
            # adding it
            print "INFO Function not found in database, adding now"
            hargs=['fn_name','fid','issue','lib']
            args=[fn.fn,fid,fn.issue,fn.lib]
            id = self.insert('t_functions',hargs,args)
        else:
            id = res[0]
        fn.dbid=id
    
    def addFunctions(self,parser):
        self.parser=parser
        for fn in parser.functions.values():
            self.addFunction(fn)
        if not self.dryrun:
            self.cnx.commit()

    def addCountAndLinks(self,rid,parser=None):
        if parser!=None:
            self.parser=parser

        success=True
        for fn in parser.functions.values():
            if fn.dbid is None:
                print 'ERROR fn=',fn.fn,'not in db yet'
                self.addFunction(fn)

            hargs=['rid','fid','selfcounts','totalcounts']
            args=[rid,fn.dbid,fn.selfcost,fn.totalcost]
            # add counts to db
            try:
                self.insert('t_counts',hargs,args)
            except IntegrityError as detail:
                print 'IntegrityError',detail
                success=False
                print 'fn=',fn
                continue

            # add sub call links to db
            for i,counts in fn.calls.iteritems():
                if i!=0:
                    cfn=self.parser.functions[i]
                    if cfn.dbid is None:
                        print 'ERROR cfn=',cfn.fn,'not in db yet'
                        self.addFunction(cfn)
                    hargs=['rid','fid','cfunid','counts']
                    args=[rid,fn.dbid,cfn.dbid,counts]
                    try:
                        self.insert('t_links',hargs,args)
                    except IntegrityError as detail:
                        print 'IntegrityError',detail
                        success=False
                        print 'fn=',fn
                        print 'cfn=',cfn
                        
        if success:
            if not self.dryrun:
                self.cnx.commit()
        else:
            print 'some ERROR occured rolling back'
            self.cnx.rollback()
        
if __name__=="__main__":

    import argparse

    usage= "parses callgrind.out and creates web pages, and fills database with performance info"

    parser = argparse.ArgumentParser(description=usage)

    # dummy default 
    parser.add_argument('-a','--all', action='store_true', default=False,
                    dest='doAllSteps',
                    help='do all (to be used in RTT)')

    # steps
    parser.add_argument('--fillDB', action='store_true', default=False,
                    dest='fillDB',
                    help='fill results in db file (default for RTT)')
    parser.add_argument('--no-fillDB', action='store_false', default=False,
                    dest='fillDB',
                    help='do not fill results in db file')

    parser.add_argument('--createDB', action='store_true', default=False,
                        dest='createDB',
                        help='create DB if not exists')

    parser.add_argument('--createWebpage', action='store_true', default=False,
                    dest='doWeb',
                    help='create static web page (default for RTT)')
    parser.add_argument('--no-createWebpage', action='store_false', default=False,
                    dest='doWeb',
                    help='do not create static web page')

    parser.add_argument('--copyLogFiles', action='store_true', default=False,
                    dest='copyLogs',
                    help='save selected logfiles in afs (default for RTT)')
    parser.add_argument('--no-copyLogFiles', action='store_false', default=False,
                    dest='copyLogs',
                    help='do not save selected logfiles in afs')

    # set environment
    parser.add_argument('--readEnvironment', action='store_true', default=False,
                    dest='readFile',
                    help='reads env.log to set $HOSTNAME, $AtlasArea, $LS_SUBCWD (for post mortem runs)')


    # output
    parser.add_argument('-t','--text', action='store_true', default=False,
                    dest='doText',
                    help='create text output (if it does not exist)')
    parser.add_argument('--no-text', action='store_false', default=False,
                    dest='doText',
                    help='do not create text output')
    parser.add_argument('-c','--callgrind', action='store_true', default=False,
                    dest='doCallgrind',
                    help='create callgrind output (if it does not exist)')
    parser.add_argument('--no-callgrind', action='store_false', default=False,
                    dest='doCallgrind',
                    help='do not create callgrind output')


    # special options
    parser.add_argument('--special', action='store_true', default=False,
                    dest='special',
                    help='apply special operations on the DB, e.g. checking librarie associations')

    parser.add_argument('--dry-run', action='store_true', default=False,
                    dest='dryrun',
                    help='Don\'t commit any changes to db')

    parser.add_argument('--backup', action='store_true', default=False,
                    dest='backup',
                    help='backup database to file')

    backupfile='rttProfile.'+dt.datetime.now().strftime("%y%m%d")+'.sqlite'
    parser.add_argument('--backupfile', action='store', dest='backupfile',
                      default=backupfile,
                      help='location of the sqlite file ('+backupfile+')')
  
    # file names
    dbfile='/afs/cern.ch/atlas/groups/Simulation/rtt/rttProfile.sqlite'
    parser.add_argument('--dbfile', action='store', dest='dbfile',
                      default=dbfile,
                      help='location of the sqlite file ('+dbfile+')')
    cfile='callgrind.out'
    parser.add_argument('--callgrindfile', action='store', dest='cfile',  default=cfile,
                    help='callgrind output file name [Warning not used]')
    tfile='athena.profile.txt'
    parser.add_argument('--textfile', action='store', dest='tfile',  default=tfile,
                    help='text output file name [Warning not used]')
    pfile='test.athena.profile'
    parser.add_argument('--proffile', action='store', dest='pfile',  default=pfile,
                    help='output of google profiler [Warning not used]')

    args = parser.parse_args()

    if args.doAllSteps:
        for opt in ['fillDB','doWeb','copyLogs','doText','doCallgrind']:
            setattr(args,opt,True)

    if args.doWeb or args.fillDB:
        args.doCallgrind=True;
    
    # create input files (callgrind.out & athena.profile.txt) if not present already
    preparation(args.pfile,args.tfile,args.cfile,
                args.doText,args.doCallgrind)

    # start parsing callgrind.out
    parser=Parser()
    if args.doWeb or args.fillDB:
        parser.parse(cfilename=args.cfile)
        parser.linkDown()
        parser.linkUp()
        parser.createDict()

    parser.readEnvironment(readFile=args.readFile)
    
    if args.doWeb:
        for fid in parser.featured:
            fn=parser.fn_dict[fid]
            fn.makeHtmlName()

        parser.printWebSorted()

        for lib,fnlist in parser.libraries.iteritems():
            parser.printWebSorted(fnlist,fname='lib_'+lib+'.html',lib=lib)

        for fid in parser.featured:
            fn=parser.fn_dict[fid]
            parser.printWebSorted3(fn)


    if args.createDB and not os.path.exists(args.dbfile):
        print 'creating new database'
        print __file__
        schema="""CREATE TABLE t_counts (rid integer, fid integer, selfcounts integer, totalcounts integer, primary key (rid,fid) );
CREATE TABLE t_files (id integer primary key autoincrement, fl_name text);
CREATE TABLE t_functions (id integer primary key autoincrement, fn_name text, fid integer, issue integer, lib text);
CREATE TABLE t_hosts (id integer primary key autoincrement, hostname varchar(32), os_type varchar(32), model varchar(32), cpuf float, ncpus integer , maxmem varchar(32), maxswp varchar(32), server varchar(6), resources text);
CREATE TABLE t_links (id integer primary key autoincrement, rid integer, fid integer, cfunid integer, counts integer);
CREATE TABLE t_run (id integer primary key autoincrement, branch text, rel text, cmt text, rdate date, rtime time, machine text, KSI2K_fac float, package text, job text);
CREATE TABLE t_timer (id integer primary key autoincrement, rid integer, seconds float, timer_type text, timer_scope text);
CREATE INDEX idx_functions_name_fid_issue on t_functions (fn_name, fid, issue);"""
        db=DataBase(args.dbfile)
        db.openConnection()

        for l in schema.split('\n'):
            #print l
            db._execute(l)


    if args.special:
        print 'perform special SQL statements'
        db=DataBase(args.dbfile)
        db.openConnection()


        # update lib info in db using LibraryEstimator class
        cmd='select count(*) from t_functions;'
        db._execute(cmd)
        res=db.cur.fetchone()
        print 'functions in db',res

        cmd="select count(distinct fn_name) from t_functions where fn_name not like '0x%' and fn_name <> '(??)'"
        db._execute(cmd)
        res=db.cur.fetchone()
        print 'distinct named functions in db',res

        cmd="select a.id,fn_name,fl_name,lib from t_functions a left outer join t_files b on a.fid=b.id where fn_name not like '0x%' and fn_name <> '(??)'"
        db._execute(cmd)
        for res in db.cur.fetchall():
            #print res
            id,fn_name,fl_name,oLib=res
            fn=FunctionCall(fn=fn_name)
            if fl_name is not None:
                fl=FileLocation(fl_name=fl_name)
            else:
                fl=0
            lib=LibraryEstimator.getLib(fn,fl)
            #print lib
            if lib!=oLib:
                print res,lib
                cmd='select * from t_functions where id=%s'
                db._execute(cmd,(id,))
                print db.cur.fetchone()
                cmd="update t_functions set lib=%s where id=%s;"
                db._execute(cmd,(lib,id))

        if not self.dryrun:
            db.cnx.commit()

        # remove function names starting with "0x" and replace with "(??)" and issue no.
        # TODO for each run
        # a) find 0x* functions, create issue numbers
        # b) fix t_counts and t_links
        # c) remove from t_function

    if args.fillDB:
        db=DataBase(args.dbfile)
        db.openConnection()
        db.readEnvironment(parser)
        rid=db.addRun()
        db.addFunctions(parser) # and files
        db.addCountAndLinks(rid,parser)

    if args.copyLogs:
        postprocessing()
