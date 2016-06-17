#!/usr/bin/env python

import getopt, sys, re, os.path, operator
import xml.dom.minidom
from itertools import ifilterfalse

from TrigDbHltUpload.converterutils import LineIter, pythonize, xmlize

# Dictionairy for fixing unknown type of algorithms
typeLUT = {
    'ServiceManager'                                                                         :'ServiceManager',
    'Streams'                                                                                :'AthSequencer',
    'DataFlowConfig'                                                                         :'DataFlowConfig'
    }


class Property:
    def __init__(self,name,value,section=''):
        # Remap invalid OutputLevels to INFO (bug #77610)
        if name=='OutputLevel' and (value=='0' or value=='7'): value = '3'
        #set the MuonCalBufferName property to "" instead of e.g. _23921
        if name=='MuonCalBufferName': value = ''
        #set the XMLMenuFile property to "" instead of e.g. outputHLTconfig_PID-92163.xml
        if name=='XMLMenuFile': value = ''
        # exception for list-type values: remove extra whitespace,
        # e.g., convert [ 'Value1' , 'Value2' ] to ['Value1', 'Value2']
        if len(value)!=0 and value[0]=='[':
          value = pythonize(value)
        self.name = name
        self.value = value
        self.section=section

    def __call__(self):
        return self.value
        

    def __str__(self):
        return self.value
        
    def hasEqualValue(self,other):
        """ Compares the values of two properties
        """
        (vo,vn) = (self.value,other.value)
        if (isinstance(vo,(str,unicode)) and isinstance(vn,(str,unicode))):
            vo = vo.strip()
            vn = vn.strip()
            try :
                vo = eval(vo)
                vn = eval(vn)
            except:
                pass
        return vo==vn


class Component:
    currentParent = [None]
    componentList = {}
    numberRE = re.compile("[-+]?(\d+\.\d*|\.\d+|\d+)([eE][-+]?\d+)?$")
    ninesRE  = re.compile("([-+]?\d*\.\d*)999999[5-9]")
    zerosRE  = re.compile("([-+]?\d*\.\d*)000000[0-6]")

    @classmethod
    def getComponent(cls, alias, classname='unknown'):
        """return component of given alias

        - if component does not exist, creates a new one with given classname
        - if component exists, check classname and set it if needed
        """
        if not alias in cls.componentList:
            # create a new component of given classname and alias
            component = Component(alias,classname)
            cls.componentList[alias] = component
            return component
        component = cls.componentList[alias]
        # take care of the classname
        if classname=='unknown':
            return component
        if component.classname=='unknown':
            component.classname = classname
            return component
        if component.classname != classname:
            raise RuntimeError("Existing component %s requested with type '%s', which is different from the old one '%s'" %
                               (alias,classname,component.classname))
        return component




    def __init__(self, alias, classname):
        self.classname = classname
        self.alias = alias
        self.parents = set()
        self.parent = None
        self.properties = {}
        self.pyname = self.pypack = ""
        self.isTopAlg = self.isAlg = self.isSteerAlg = self.isSvc = self.isPubTool = self.isPrivTool = self.isAuditor = False
        self.subAlgs = []

    def setProperties(self,properties):
        for p,v in properties.items():
            self.setProperty(p,v)

    def getProperty(self,p):
        if p in self.properties:
            return self.properties[p]
        return None

    def setPropertyManually(self,p,v):
        """Set property of this component
        p - property name
        v - property value
        """
        if not p in self.properties:
            raise RuntimeError("Can not modify the property %s, since it does not exist" % p)
        self.properties[p] = Property(p,v,'MANUAL')



    def setProperty(self,p,v):
        """Set property of this component
        p - property name
        v - property value
        """
        newProp = Property(p,v,Reader.section)
        if not p in self.properties:
            # new property needs to be created
            if self.alias=='ApplicationMgr' and p in ['AuditInitialize', 'AuditFinalize']:
                print "Remove item %s for %s as known not to be in c++ setup \n" % (p,self.alias) 
                return
            self.properties[p] = newProp
            return

        if self.properties[p].hasEqualValue(newProp):
            # property exists and has the same value
            return

        exception = p in ['AuditRestart', 'AuditEndRun', 'AuditBeginRun', 'AuditInitialize', 'AuditFinalize', 'AuditStop', \
                          'AuditExecute', 'AuditReinitialize', 'AuditAlgorithms', 'AuditStart']
                          
        # Test exception for private/publictool reference e.g. 'ToolSvc'=='ToolSvc/ToolSvc'
        if self.properties[p]() =='%s/%s' % (newProp(),newProp()) or \
           newProp() =='%s/%s' % (self.properties[p](),self.properties[p]()): exception = True

        # If the difference in property was found
        if exception:
            return
        
        newPropValue = newProp()
        try: newPropValue = eval(newPropValue)
        except: pass

        currentPropValue = self.properties[p]()
        try: currentPropValue = eval(currentPropValue)
        except: pass

        # treat float rounding errors
        if currentPropValue!=0 and isinstance(currentPropValue,float) and isinstance(newPropValue,float): 
            valueDifferences = (currentPropValue - newPropValue) / currentPropValue
            if abs(valueDifferences) < 0.0000001:
                self.properties[p] = newProp
                print "COMPONENT FPP %s:\n%s: (difference due to floating point precission)\n       %s (%s) - %s\n  !=   %s (%s) - %s" % (self.alias,p,
                                                                                                                                      currentPropValue,self.properties[p].section, type(currentPropValue),
                                                                                                                                      newPropValue, newProp.section, type(newPropValue))
                print "%s will keep more precise python value %s (%s) \n" % (p,self.properties[p](),self.properties[p].section)
                return

        print "COMPONENT DIFF %s:\n%s:\n       %s (%s) - %s\n  !=   %s (%s) - %s" % (self.alias,p,currentPropValue,self.properties[p].section, type(currentPropValue), \
                                                                                     newPropValue,newProp.section, type(newPropValue))

        
        newAmbiguityResolution = False

        # exception is the ApplicationMgr, since this is not set properly in PYTHON
        if self.alias=='ApplicationMgr' and newProp.section=="SETUP":
            newAmbiguityResolution = False

        if newAmbiguityResolution:
            # always use the latest, so priority is PYTHON > GLOBAL > CXX,
            self.properties[p] = newProp
            print "%s will use value %s (%s)" % (p,self.properties[p](),self.properties[p].section)
        else:
            # Exception for overwriting the property by values from SETUP
            if newProp.section!="SETUP" or isinstance(newPropValue,(bool,int,long)):
                self.properties[p] = newProp
                if newProp.section=="SETUP":
                    print "%s will use new PYTHON value %s (%s)" % (p,self.properties[p](),self.properties[p].section)
                else:
                    print "%s will use new value %s (%s)" % (p,self.properties[p](),self.properties[p].section)
            else :
                # SETUP(python) and !isBool/Int/Long (arrays, strings)
                print "%s will keep value %s (%s)" % (p,self.properties[p](),self.properties[p].section)
        print ""
            

    def setPyNameAndPackage(self,pyname,pypack):
        if pyname: self.pyname = pyname.strip()
        if pypack: self.pypack = pypack.strip()

    def addSubAlg(self, subalg):
        self.subAlgs += [subalg]

    def addParent(self, parent):
        if parent: self.parents.add(parent)

    def __cmp__(self,other):
        return self.alias == other.alias


    def _pedigree(self):
        """returns the length of the bloodline"""
        if not self.parent: return 0
        return self.parent._pedigree() + 1

    def __str__(self):
        al = self.alias
        cn = self.classname
        if not cn:
            #raise RuntimeError, "ERROR: component %s has no classname" % al
            cn=''

        indent = self._pedigree()
        parentname = "noparent"
        if self.parent: parentname = self.parent.alias
        #if 'TrigSteer_L2' in self.alias:
        #print self.alias, parentname, indent
        s  = 4*indent*' '+'<component alias="%s" name="%s" topalg="%i" alg="%i" steeralg="%i" svc="%i" pubT="%i" privT="%i" aud="%i" py_name="%s" py_package="%s">\n' % \
             (xmlize(al), xmlize(cn), self.isTopAlg, self.isAlg, self.isSteerAlg, self.isSvc, self.isPubTool, self.isPrivTool, self.isAuditor, self.pyname, self.pypack)

        # write out the properties
        props = self.properties.items()
        props.sort()
        for n,p in props:
            s += 4*indent*' ' + '    <parameter name="%s" value="%s" op="set"/>\n' % (n,xmlize(p.value))
            
        for c in sorted(self.subAlgs,key=operator.attrgetter('alias')): s += str(c)
        s += 4*indent*' '+'</component> <!-- %s -->\n' % al
        return s

    def hierarchy(self,indent=0):
        s  = 4*indent*' '+'  |-%s\n' % self.alias
        for c in self.subAlgs:
            s += "%s" % c.hierarchy(indent+1)
        return s


            
class Reader:
    blockRE  = re.compile('^(GLOBAL|SERVICES|ALGORITHMS|ALGTOOLS|SETUP)$')
    lineRE   = re.compile("([\w\.:\<\>\,\* \@\-]+?)\s*'([^']*)':(.*)")
    section = None
    def __init__(self,lines):
        self.sections = {}
        self._splitSections(lines)
        self._readClasses()
        self._setParenthood()
        self._stripAthSequencer( Component.getComponent("AthMasterSeq"), top=True )
        self._stripSteerAlgs()
        self._fixPublicToolsType()


    def _splitSections(self,lines):
        cursec=''
        for l in lines:
            if Reader.blockRE.match(l):
                cursec = l
                self.sections[cursec] = []
            else:
                self.sections[cursec] += [l]

    def _removeCaloClusterCorrectionSubtools(self):
        self.mastertoolnames = []
        for c in Component.componentList.values():
            if c.classname == 'CaloRunClusterCorrections':
                self.mastertoolnames += [c.alias]
        toBeRemoved = [ca for ca in Component.componentList.keys() if [ca.startswith(mt+'.') for mt in self.mastertoolnames].count(True)>0]
        for c in toBeRemoved:
            Component.componentList.pop(c)

    def _removeUnwantedComponents(self):
        unwantedComponents = ["DataFlowConfig", "ServiceManager", "TrigHistoryAlg"]
        for c in unwantedComponents:
            try: Component.componentList.pop(c)
            except: pass
    
    ## remove services from components and from ApplicationMgr which have only python implementation.
    def _removeUnwantedServices(self):
        # hardcoded list of services to remove
        unwantedServices = [ "PyAthena::Svc/TrigConf2CoolSyncSvc" ]
        for c in unwantedServices:
            svcName = c.split('/')[-1] # get last name part of CLASS/NAME
            if svcName in Component.componentList: Component.componentList.pop(svcName) # rm service from component list
            if "ApplicationMgr" in Component.componentList: # rm service from ApplicationMgr
                # rm from CreateSvc
                orig = eval(Component.componentList["ApplicationMgr"].getProperty('CreateSvc').value) # ret string is a list
                if c in orig: orig.pop(orig.index(c)) # pop from an array
                Component.componentList["ApplicationMgr"].setPropertyManually('CreateSvc',orig) # use "manually" method to overwrite
                # rm from ExtSvc
                orig = eval(Component.componentList["ApplicationMgr"].getProperty('ExtSvc').value) # ret string is a list
                if c in orig: orig.pop(orig.index(c)) # pop from an array
                Component.componentList["ApplicationMgr"].setPropertyManually('ExtSvc',orig) # use "manually" method to overwrite


    ## Disable HistorySvc - set HistorySvc properties
    def _disableHistorySvc(self):
        if "HistorySvc" in Component.componentList:
            Component.componentList["HistorySvc"].setPropertyManually('Dump','False')
            Component.componentList["HistorySvc"].setPropertyManually('Activate','False')
            Component.componentList["HistorySvc"].setPropertyManually('OutputFile','')
        if "ApplicationMgr" in Component.componentList:
            Component.componentList["ApplicationMgr"].setPropertyManually('ActivateHistory','False')
    

          
    def _stripAthSequencer(self, sequencer, top=False):
        athSeqChildren = []

        subAlgs = sequencer.subAlgs
        if subAlgs != None :
            for ind in xrange(len(subAlgs)-1,-1,-1) :
                if subAlgs[ind].classname=='AthSequencer' :
                    athSeqChildren.extend(self._stripAthSequencer(subAlgs[ind]))
                else :
                    athSeqChildren.append(subAlgs.pop(ind)) # algorithms will be removed from the list of sub_algs. AthSequencer.Member property determines the execution.

        if not top:
            return athSeqChildren

        # remove TrigHistoryAlg from AthMasterSeq Members property
        p = re.compile(".*/TrigHistoryAlg")
        k = eval(sequencer.properties["Members"]())
        k = list(ifilterfalse(p.match,k))
        sequencer.properties["Members"].value = str(k)

        # make children topalgs without parents and put into componentlist if they are not there yet
        for alg in athSeqChildren:
            alg.parent = None
            alg.isTopAlg = True
            if alg.alias in Component.componentList:
                print "Existing Component: %s/%s" % (alg.alias,Component.componentList[alg.alias])
                component=Component.getComponent(alg.alias, alg.classname)
                Component.componentList[alg.alias].isTopAlg = True
                for k,v in alg.properties.items() :
                    component.setProperty(k,v.value)
            else:
                Component.componentList[alg.alias]=alg

        return athSeqChildren

          
    ## Fix the public tool type
    def _fixPublicToolsType(self) :
        if "ToolSvc" in Component.componentList:
            for alg in Component.componentList['ToolSvc'].subAlgs :
                alg.isPubTool = True
        
        
    ## Strip the HLT::TrigSteer type of algorithm and set their children as steering algorithms
    #  @param subAlgs list of input algorithms
    #  @return List of agorithms
    def _stripSteerAlgs(self):
        steerAlgs = []
      
        for c in Component.componentList.values():
            if c.classname=='HLT::TrigSteer' and len(c.subAlgs)>0:
                for ind in xrange(len(c.subAlgs)-1,-1,-1) :
                    if c.subAlgs[ind].properties.has_key('AthenaMonTools') and \
                           c.subAlgs[ind].properties.has_key('doTiming') and \
                           c.subAlgs[ind].properties.has_key('doOperationalInfo'):
                        steerAlgs.append(c.subAlgs.pop(ind))
                    else :
                        c.subAlgs[ind].isSteerAlg=False
                      
        for alg in steerAlgs :
            alg.parent = None
            alg.isSteerAlg = True
            if alg.alias in Component.componentList:
                print "Existing Steering algorithm: %s/%s" % (alg.alias,Component.componentList[alg.alias])
                component=Component.getComponent(alg.alias, alg.classname)
                for k,v in alg.properties.items() :
                    component.setProperty(k,v.value)
            else:
                Component.componentList[alg.alias]=alg
    
        return steerAlgs
  

    ## Fix the empty properties for algorithms of type H1WeightToolCSC12Generic and MCNumInversionResponse
    def _fixEmptyPropertiesSpecialTools(self):
        for c in Component.componentList.values():
            if c.classname in ('H1WeightToolCSC12Generic', 'MCNumInversionResponse') :
                for k,v in c.properties.items() :
                    if k not in ('prefix') and v() in ('',None):
                        c.properties[k].value='\"\"'
    
    def _readClasses(self):
        for sec in ['ALGORITHMS', 'SERVICES', 'ALGTOOLS']:
            Reader.section = sec
            it = LineIter(self.sections[sec])
            while it.next():
                line = it()
                self._readSingleComponent(it)

        self._removeCaloClusterCorrectionSubtools()

        for sec in ['GLOBAL']:
            Reader.section = sec
            it = LineIter(self.sections[Reader.section])
            while it.next():
                line = it()
                self._readGlobalLine(line)

        for sec in ['SETUP']:
            if not sec in self.sections: continue
            Reader.section = sec
            it = LineIter(self.sections[sec])
            while it.next():
                line = it()
                self._readSingleComponent(it)

        self._removeCaloClusterCorrectionSubtools()
        self._removeUnwantedComponents()
        self._removeUnwantedServices()
        self._disableHistorySvc()
        self._fixEmptyPropertiesSpecialTools()


    def _readGlobalLine(self,line):
        matchgroups = Reader.lineRE.match(line)
        if matchgroups==None:
            raise RuntimeError("Could not interpret line\n%s" % line)
        alias,prop,val = matchgroups.groups()
        if [alias.startswith(mt+'.') for mt in self.mastertoolnames].count(True)>0: return
        classname = 'unknown'
        if alias in typeLUT.keys() : classname = typeLUT[alias]
        cmp = Component.getComponent(alias, classname)
        if '.' in alias:
            cmp.addParent(alias.rsplit('.',1)[0])
        cmp.setProperty(prop,val)

    def _readSingleComponent(self,it):
        if it().startswith('----') or it().startswith('>> ') or it().strip()=='': return
        if it()=='}':
            Component.currentParent.pop()
            return
        componentlines = [it()]
        while it.next():
            if it()==']' or it()=='': break
            componentlines += [it()]

        properties = {}
        interpretAsProp = None
        pyname = pypack = ""
        for cl in componentlines:
            key,val = cl.split(':',1)
            if   key == 'Type': classname = val.strip()
            elif key == 'Name':        alias = val.strip()
            elif key == 'Properties':  interpretAsProp = True
            elif key == 'Py_Name':     pyname = val
            elif key == 'Py_Package':  pypack = val
            elif key == 'Version' or key=='Parent':   pass
            elif interpretAsProp:
                if "DefaultName" in val: val = val.replace('DefaultName',alias)
                properties[key.strip().strip("'")] = val
            else:
                raise RuntimeError("Don't know how to interpret this '%s'" % cl)
            
        if classname=="":
            print "Warning: type of component %s is unknown. This is temporarily accepted for a dual-use tool" % alias
            classname = 'unknown'
        component = Component.getComponent(alias,classname)
        component.setPyNameAndPackage(pyname,pypack)
        component.setProperties(properties)

        # get parent and flags
        parent = Component.currentParent[-1]
        if self.section == 'ALGTOOLS':
            if alias.count('.')==0:
                raise RuntimeError('Tool should have a dot in the name: %s' % alias)
            if alias.count('.')==1 and alias.startswith('ToolSvc.'):
                parent = 'ToolSvc'
                component.isPubTool = True
            else:
                parent = alias.rsplit('.',1)[0].strip()
                component.isPrivTool = True
        elif self.section == 'SETUP':
            if alias.count('.')>0:
                parent = alias.rsplit('.',1)[0]
        elif self.section=='SERVICES':
            component.isSvc = True
        elif self.section=='ALGORITHMS':
            component.isAlg = True
            if alias=='AthMasterSeq':
                self.isTopAlg=True
        component.addParent(parent)

        # interpret the line after (sub-alg info)
        it.next()
        if it().startswith('Subalg'):
            Component.currentParent += [component.alias]
        return 

    def _setParenthood(self):
        compDict = Component.componentList
        for c in compDict.values():
            if len(c.parents)==0:
              if len(c.alias.split('.')) > 1 :
                # Fix the child parent relation
                parentName = '.'.join(c.alias.split('.')[:-1])
                if compDict.has_key(parentName) :
                  c.addParent(parentName)
              else:
                continue
            if len(c.parents)>1:
                raise RuntimeError, "Component %s has more than 1 parent %r" % (c.alias, list(c.parents))
            c.parent = compDict[c.parents.pop()]
            c.parent.addSubAlg(c)

        # set steering alg flag
        SteeringSubAlgs = []
        if 'TrigSteer_L2' in compDict: SteeringSubAlgs += compDict['TrigSteer_L2'].subAlgs
        if 'TrigSteer_EF' in compDict: SteeringSubAlgs += compDict['TrigSteer_EF'].subAlgs
        if 'TrigSteer_HLT' in compDict: SteeringSubAlgs += compDict['TrigSteer_HLT'].subAlgs
        for c in SteeringSubAlgs:
            c.isSteerAlg = True

        nop = dict([(c.alias,c) for c in compDict.values() if not c.parent])
        Component.componentList = nop

        # set all remaining services
        for cname in Component.componentList["ApplicationMgr"].properties["ExtSvc"].value:
            alias = cname.split('/')[-1]
            if alias in Component.componentList:
                Component.componentList[alias].isSvc = True




def usage(ecode):
    """Prints a help message"""
    print "\nusage:"
    print "%s [options] inputfile.txt [second_input.txt]" % sys.argv[0]
    print "options are:"
    print "  -h|--help        : prints this message"
    print "  --p1             : temporary hack to run at Point 1 (muFast_Muon)\n"
    print "  -o|--outputdir   : output directory (otherwise directory of first input file will be used\n"
    print "Minimum required   : you have to specify the .txt file to be converted.\n"
    sys.exit(ecode)



if __name__ == "__main__":

    # Read the options
    try: opts, args = getopt.getopt(sys.argv[1:], "h?d:", ['help','p1','outputdir:'])
    except getopt.GetoptError:  usage(1)
    ConversionForPointOne = False
    outputdir = None
    for o, a in opts:
        if o in ('-h', '-?', '--help'):
            usage(0)
        if o in ('--p1'):
            ConversionForPointOne = True
        if o in ('-d','--outputdir'):
            outputdir = a
    if len(args)==0:
        print "Specify filename(s)"
        usage(1)
    infiles = args
    outfile = infiles[0].replace(".txt",".xml")

    if outputdir:
        try: outfile = outputdir + "/" + outfile[outfile.rindex('/')+1:]
        except ValueError: outfile = outputdir + "/" + outfile
            
    # Read the setup file(s)
    lines = []
    for f in infiles:
        fin  = open(f, 'r')
        lines += [l.rstrip() for l in fin.readlines()]
        fin.close()
    r = Reader(lines)

    FH = open(outfile, 'w')
    print "writing",outfile

    FH.write('<?xml version="1.0" encoding="UTF-8"?>\n');
    FH.write('<setup version="1.00000" name="setup">\n');

    aliases = Component.componentList.keys()
    aliases.sort()
    for a in aliases:
        c = Component.componentList[a]
        FH.write(str(c))
    #print Component.componentList['AthMasterSeq'].hierarchy()

    FH.write('</setup>\n');
    FH.close()
    
