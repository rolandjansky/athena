from __future__ import print_function
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# #######################################
## JetMonitoringConfig
##  Helper classes to implement a user-friendly Jet Monitoring configuration (hopefuly)
##  
## The principles are as follow :
##   * the full configuration for 1 JetMonitoringAlg is specified as a dictionnary
##   * specialized helper functions can turn the dictionnary into fully configured algs and perform the definition of histograms in
##     the monitoring framework.
##   * entries in the dictionnary are either
##       - simple values used by the helper functions
##       - Other dicitionnaries specifying a sub-tool
##       - an alias (a string) refering to a known standard specification (that is a dictionnary)
##
## 
##  The dictionnaries are actually specialized versions of the python dictionnary.
##  Specializing allows
##    * to write specifications in a very compact and readable way
##    * to write the helper functions as custom methods of the specialized dictionnaries (ex: spec.toAlg(), spec.toTool(), ...)
##
## Specialized dicts are :
##  ToolSpec : generic base for any tool 
##  VarSpec  : Jet variable specification (name, type, index in vector if of vector type, scale factor to GeV if needed )
##  HistoSpec  : Histograms made of 1,2 or 3 variables as specified by VarSpec
##  SelectSpec : Tool applying a jet selection and filling histos with selected jets
##  JetMonAlgSpec : a full JetMonitoringAlg specification
##
## See python/JetMonitoringExample.py for usage of the system

import six
from AthenaCommon import  SystemOfUnits

class ConfigDict(dict):
    """A python dictionnary extended so that each entry in the dict can also be accessed as 
       member attribute.  
    Ex: 
       d = ConfigDict(aKey = 4)
       d.aKey # --> == 4 is the same as d["aKey"]
       d.bKey = 12 # now d["bKey"] is existing and set to 12      

    all other usual methods of dictionnary are working as expected.    
    """
    def __init__(self, **kwargs):
        dict.__init__(self, **kwargs)
        for k,v in six.iteritems (kwargs):
            dict.__setattr__(self, k,  v)
    def __getattr__(self, attr):
        try:
            return self[attr]
        except KeyError:
            dict.__getattribute__(self,attr)
            #raise AttributeError(attr)

    def __setattr__(self, attr, value):
        if attr in ['keys', 'clear', 'update', 'pop', 'iteritems', 'values','setdefault','get','has_key','copy']:
            print('ConfigDict ERROR can not assign attribute ', attr)
            return
        dict.__setitem__(self, attr,  value)
        dict.__setattr__(self, attr,  value)
    def __setitem__(self, attr, value):
        if attr in ['keys', 'clear', 'update', 'pop', 'iteritems', 'values','setdefault','get','has_key','copy']:
            print('ConfigDict ERROR can not assign attribute ', attr)
            return
        dict.__setitem__(self, attr,  value)
        dict.__setattr__(self, attr,  value)

        
    def clone(self, **kwargs):
        from copy import deepcopy
        c = deepcopy(self)
        for k,v in six.iteritems (kwargs):
            setattr(c,k,v)
        return c

    
    def dump(self, out=None):
        """ prints the content of this dict on stdout (default) or in the file 'out' """
        if out is None :
            from sys import stdout
            out = stdout
        _write = out.write
        def write(s, e='\n'): _write(s+e)
        self._dump(write)
        
    def _dump(self, writeFunc):
        def write(s, e='\n'): writeFunc('  '+s,e)
        writeFunc(self.__class__.__name__+'(')
        for k,v in sorted(six.iteritems (self)):
            if isinstance(v, ConfigDict):
                write(k+' = ','')
                v._dump(write)
            else:
                write(k+' = '+str(v))
        writeFunc(')')




# **********************************************************
# **********************************************************
# Helper functions
# **********************************************************


def interpretSelStr(selStr):
    """Interpret a selection string in the form '12.3<var<42.0'
    and returns a tuple.
     '12.3<var<42.0' -> returns (12.3, 'var', 42.)
     'var<42.0' -> returns (None, 'var', 42.)
     '12.3<var' -> returns (12.3, 'var', None)
    """

    if '>' in selStr:
        print("JetMonitoring ERROR interpreting selection string ", selStr)
        print("JetMonitoring ERROR  can not interpret '>', please use only '<' ")
    parts = selStr.split('<')
    cmin, cmax = None, None
    var = selStr
    if len(parts)==2:
        ismin = False
        try :
            var, cut = parts[0] , float(parts[1])
        except ValueError:
            cut, var = float(parts[0]) ,parts[1]
            ismin=True
        if ismin : cmin = cut
        else: cmax = cut
    elif len(parts)==3:
        cmin, var, cmax = parts
        cmin = float(cmin)
        cmax = float(cmax)

    return cmin, var, cmax
    


def findSelectIndex( name):
    """ interprets 'varName[X]' into ('varName',x) """
    try:
        name, index = name.split('[')
    except ValueError:
        name, index = name, ''
    if not index.endswith(']'):
        return name, -1
    index = int(index[:-1])
    return name, index

# **********************************************************


class ToolSpec(ConfigDict):
    """A dictionnary specialized for containing the specification of a Athena tool.    
    """
    def __init__(self, klass, name, **args):
        self.name = name
        self.klass = klass
        ConfigDict.__init__(self, **args)
        
        
    def toTool(self):
        from AthenaConfiguration.ComponentFactory import CompFactory
        conf = self.clone(self.name)
        klass = getattr(CompFactory,conf.pop('klass')) # remove 'klass'
        conf.pop('name')
        conf.pop('topLevelDir',None)
        conf.pop('bottomLevelDir',None)
        conf.pop('defineHistoFunc',None) # not used here.
        for k, v in six.iteritems (conf):
            if isinstance(v,ToolSpec):
                v.topLevelDir = self.topLevelDir
                v.bottomLevelDir = self.bottomLevelDir
                conf[k] = v.toTool()
            if isinstance(v,list):
                if v == []: continue
                if isinstance(v[0],ToolSpec):
                    # assume we have k is a ToolHandleArray
                    toolInstances = []
                    for toolSpec in v:
                      toolSpec.topLevelDir=self.topLevelDir
                      toolSpec.bottomLevelDir=self.bottomLevelDir
                      toolInstances.append( toolSpec.toTool() )
                    conf[k] = toolInstances
        return klass(**conf)
        
    def clone(self, newname,**args):
        return ConfigDict.clone(self, name=newname, **args)

    def defineHisto(self, parentAlg, monhelper , path):
        # Assume a helper function was given :
        defineHistoFunc = self.defineHistoFunc
        # call it :
        defineHistoFunc(self, parentAlg, monhelper , path)


        
class VarSpec(ToolSpec):
    """A dictionary specialized to contain a jet variable specification"""
    def __init__(self, Name , Type='float', Index=-1, Scale=1):
        # by default we allow only the properties of a JetHistoVarTool
        if Name.endswith(':GeV'):
            Scale=1./SystemOfUnits.GeV
            Name = Name[:-4]


        Name , Index = findSelectIndex(Name)
        if Index != -1:
            if Type[:3] != 'vec': Type='vec'+Type
        self.Name = Name
        self.Type = Type
        self.Scale = Scale
        self.Index = Index
        ConfigDict.__init__(self)


    def toTool(self):
        from AthenaConfiguration.ComponentFactory import CompFactory
        self.pop('topLevelDir', None)
        self.pop('bottomLevelDir', None)
        return CompFactory.JetHistoVarTool(self.Name, **self)

    def vname(self):
        if self.Index ==-1: return self.Name
        return self.Name+str(self.Index)


    def __str__(self):
        if self.Index==-1: return 'VarSpec("{n}",{t})'.format(n=self.Name, t=self.Type)
        else: return 'VarSpec("{n}[{i}]",{t})'.format(n=self.Name, t=self.Type, i=self.Index)

    def _dump(self, writeFunc):
        writeFunc( str(self))
        
class HistoSpec(ToolSpec):
    """A dictionary specialized to contain a JetHistoAttributeFiller specification
    Invocation is like : spec = HistoSpec( name, bins, ...optional args... ) where 
      - name : a string, from which the name of the Histogram is build.
               AND also the description of variables to be plotted IF none of xvar, yvar or zvar is in the optional args.
               The final name of histos is given by name.replace(':GeV','').replace(';','_')
           ex: 'eta'        --> histo name : 'eta', variable to plot is 'eta'
               'pt:GeV'     --> histo name : 'pt', variable to plot is 'pt' at GeV scale 
               'pt:GeV;eta' --> histo name : 'pt_eta', 2 variables to plot  'pt' at GeV scale on x axis and 'eta' on y axis
      - bins : the binning of the histogram in the form (nbin, min, max) or (nbinX, minX, maxX, nbinY, minY, maxY )
      - optionnal args are : 
         + xvar, yvar, zvar : strings specifying variables
         + profile : boolean, True if the histo is a TProfile (or TProfile2D)     
         + any other args (ex: title) will be forwarded to the mongroup.defineHistogram() function.
    """
    def __init__(self, name , bins,  xvar=None, yvar=None, zvar=None,  profile=False,**args):
        self.is1D = ';' not in name

        self.nVar = name.count(';')+1
        self.isProfile = profile
        if self.nVar==1:
            self.xvar = retrieveVarToolConf( xvar if xvar is not None else name)
            self.yvar = None
            self.zvar = None
        elif self.nVar==2:
            xv , yv = name.split(';')
            self.xvar = retrieveVarToolConf( xvar if xvar is not None else xv)
            self.yvar = yvar if yvar is not None else yv            
            self.zvar = None
        else:
            xv , yv, zv = name.split(';')
            self.xvar = xvar if xvar is not None else xv
            self.yvar = yvar if yvar is not None else yv
            self.zvar = zvar if zvar is not None else zv
            self.isProfile = True
        self.name = name.replace(':GeV','')
        self.bins = bins
        self.hargs = ConfigDict( **args)
        ConfigDict.__init__(self, **args)

    def groupName(self):
        return self.name if self.nVar==1 else self.name.replace(';','_')


    def clone(self, newname, bins=None,  xvar=None, yvar=None, zvar=None, **hargs):
        c = ConfigDict.clone(self)
        if xvar : c.xvar = xvar
        if yvar : c.yvar = yvar
        if zvar : c.zvar = zvar
        if bins: c.bins = bins
        c.hargs.update(**hargs)
        c.name = newname
        return c

    def to2DSpec(self, yspec, profile=False, zspec=None):
        """ Merge this histo spec and yspec into a new 2D histo spec"""
        tx, ax, _ = self.title.split(';')
        ty, ay, _ = yspec.title.split(';')
        title = ty+' vs '+tx+';'+ax+';'+ay
        name = self.name+';'+yspec.name
        if zspec is not None:
            tz, az, _ = zspec.title.split(';')
            title = tz+' vs '+tx+' and '+ty+';'+ax+';'+ay

            name = name + ';'+zspec.name
        s2d = HistoSpec( name, self.bins+yspec.bins,
                         xvar = self.xvar,
                         yvar = yspec.xvar, title=title, profile=profile
        )
        if zspec is not None:
            s2d.zvar = zspec.xvar
        return s2d

    

    def toTool(self):
        from AthenaConfiguration.ComponentFactory import CompFactory
        vx = retrieveVarToolConf( self.xvar)
        tool = CompFactory.JetHistoAttributeFiller(self.groupName()+"hfiller",
                                       VarX = vx.toTool(),
                                       Group = self.groupName(),
        )
        if self.nVar>1:
            tool.VarY = retrieveVarToolConf( self.yvar).toTool()
            if self.nVar>2:
                tool.VarZ = retrieveVarToolConf( self.zvar).toTool()
            else: tool.VarZ = None
        else : tool.VarY = None

        return tool
    
    
    def defineHisto(self, parentAlg, monhelper , path):
        """ This function performs the standard monitoring config calls for this JetHisto tool.
              - group = monhelper.addGroup( ... )
              - group.defineHistogram( ... )
        """

        # build the arguments to be passed to defineHistogram
        hargs = ConfigDict()
        bins = self.bins
        if self.nVar==1:
            hargs.update(xbins = bins[0],xmin = bins[1], xmax=bins[2],                         
                         type='TH1F',
            )
        elif self.nVar>1:
            hargs.update(xbins = bins[0],xmin = bins[1], xmax=bins[2],
                         ybins = bins[3],ymin = bins[4], ymax=bins[5], 
                         type='TH2F' if not self.isProfile else 'TProfile',                         
            )
            hargs.setdefault('opt', 'colz')
            if self.nVar==3:
                hargs.type = 'TProfile2D'
        hargs.update( **self.hargs) # overwrite by user-given args if any
        
        # we create one group for each histoFiller : self.groupName() are unique within a JetMonitoringAlg
        bottomLevelDir = self.bottomLevelDir if self.bottomLevelDir != '' else parentAlg.JetContainerName
        group = monhelper.addGroup(parentAlg, self.groupName(), self.topLevelDir+bottomLevelDir)

        # define the variables used by this tool
        #  we encode as 'varx,vary;alias' as requested ny standard monitoring config, see
        #  https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/Control/AthenaMonitoring/python/GenericMonitoringTool.py
        name = retrieveVarToolConf( self.xvar).vname()
        if self.nVar>1:
            name = name +"," + retrieveVarToolConf( self.yvar).vname()
        if self.nVar>2:
            name = name +"," + retrieveVarToolConf( self.zvar).vname()
            
        name = name+";"+self.groupName()

        #print ' uuuuuuuuuuu ', self.name , ' --> ', name,  hargs
        # call the standard histo definition routine :
        group.defineHistogram(name, path=path, **hargs)


    def _dump(self, writeFunc):
        def write(s): writeFunc('  '+s)
        writeFunc('HistoSpec("'+self.name+'", '+str(self.bins) )
        for k in [ 'xvar',  'yvar', 'zvar', 'isProfile' ]:
            if self[k] is not None:
                write(' '+k +' = '+str(self[k]))
        write(')')


class EventHistoSpec(ToolSpec):
    """A similar dictionary to HistoSpec above, but specialized to contain a
    JetHistoEventLevelFiller specification.
    Invocation is like : spec = EventHistoSpec( name, bins=(n,xlow,xhigh) )
    """
    def __init__(self, name, bins, **args):
        ToolSpec.__init__(self, klass=None, name=name, **args) # we don't really need to pass a klass because we're specialized for JetHistoEventLevelFiller, see toTool()
        self.bins = bins
        self.hargs = ConfigDict( **args)


    def toTool(self):
        from AthenaConfiguration.ComponentFactory import CompFactory
        from JetMonitoring.JetStandardHistoSpecs import knownEventVar
        # force the property "VarName" to simply be the name of the variable specification:
        v = knownEventVar[self.name]
        v.VarName = v.name
        tool = CompFactory.JetHistoEventLevelFiller( self.name+"hfiller",
                                                Var = v.toTool(),
                                                Group = self.name,
        )
        return tool

    def defineHisto(self, parentAlg, monhelper , path):
        from JetMonitoring.JetStandardHistoSpecs import knownEventVar
        hargs = dict(xbins = self.bins[0],xmin = self.bins[1], xmax=self.bins[2],
                     type='TH1F', )
        hargs.update( **self.hargs)
        # we create one group for each histoFiller : self.name() are unique within a JetMonitoringAlg
        bottomLevelDir = self.bottomLevelDir if self.bottomLevelDir != '' else parentAlg.JetContainerName
        group = monhelper.addGroup(parentAlg, self.name, self.topLevelDir+bottomLevelDir)
        group.defineHistogram(knownEventVar[self.name].VarName, path=path, **hargs)





class SelectSpec(ToolSpec):
    """A dictionary specialized to contain a JetHistoSelectSort specification
    Invocation is like : spec = SelectSpec( name, , expr, path ) where 
     - name : a string naming the selection being applied
     - expr : s string, defining a selection in the form '0.123<var<4.567' where var is an attribute/variable
     - path : optional string, giving the sub-directory into which histos for this selection will be put. if not set, defaults to name.
    """
    def __init__(self, selname, selexpr, path=None, **args):
        path = selname if path is None else path
        if '<' in selexpr:
            # interpret it as min<v<max
            cmin, v , cmax = interpretSelStr(selexpr)
            if args.setdefault('isEventVariable', False) :
              selProp = 'EventSelector'
              selSpec = ToolSpec('JetEventSelector', v+'sel', Var = retrieveEventVarToolConf(v), )
            else:
              selProp = 'Selector'
              selSpec = ToolSpec('JetSelectorAttribute', v+'sel', Var = retrieveVarToolConf(v), )
            if cmin is not None: selSpec['CutMin'] = cmin
            if cmax is not None: selSpec['CutMax'] = cmax
            args[selProp] = selSpec
        elif selexpr != '':
            from JetMonitoring.JetStandardHistoSpecs import  knownSelector
            # assume it's a known pre-defined jet selector
            selSpec = knownSelector.get(selexpr, None)
            if selSpec is None :
                print("ERROR  ", selexpr , " is an unknown JetSelector ")
                # should raise an exception ??
                return
            args['Selector'] = selSpec



        self.name = selname
        self.path = path
        ConfigDict.__init__(self, **args)
        tmpL = self.FillerTools
        self.FillerTools = []
        self.appendHistos(*tmpL)

    def appendHistos(self, *hfillers):
        tmpL = [retrieveHistoToolConf(hspec) for hspec in hfillers]
        self.FillerTools += tmpL
        
    def toTool(self):
        from AthenaConfiguration.ComponentFactory import CompFactory
        # conf = self.clone(self.name)
        # name = conf.pop('name')
        selTool = CompFactory.JetHistoSelectSort(self.name, SelectedIndex=self.get('SelectedIndex',-1), InverseJetSel=self.get('InverseJetSel',False))
        if hasattr(self,'Selector'):
            self.Selector.topLevelDir = self.topLevelDir
            self.Selector.bottomLevelDir = self.bottomLevelDir
            selTool.Selector = self.Selector.toTool()
        if hasattr(self,'EventSelector'):
            self.EventSelector.topLevelDir = self.topLevelDir
            self.EventSelector.bottomLevelDir = self.bottomLevelDir
            selTool.EventSelector = self.EventSelector.toTool()
        if hasattr(self, 'SortVariable'):
            selTool.SortVariable = retrieveVarToolConf(self.SortVariable)
        suffix = '_'+self.name
        for i,tconf in enumerate(self.FillerTools):
            tconf.topLevelDir = self.topLevelDir
            tconf.bottomLevelDir = self.bottomLevelDir
            tconf = tconf.clone(newname=tconf.name+suffix)
            self.FillerTools[i] = tconf # re-assign the modified conf so it's consistently re-used elsewhere 
            selTool.FillerTools += [ tconf.toTool() ] # assign a configured tool to the JetHistoSelectSort instance
        return selTool

    def defineHisto(self, parentAlg, monhelper , path):
        # redefine path for sub-histos if needed :
        path = path if  self.path is None else self.path
        # define histos for each subtools
        for tconf in self.FillerTools:
            tconf.defineHisto(parentAlg, monhelper, path)


    def _dump(self, writeFunc):
        def write(s,e='\n'): writeFunc('  '+s,e)
        def write2(s,e='\n'): writeFunc('    '+s,e)
        writeFunc('SelectSpec("'+self.name+'", path='+self.path+',')
        if hasattr(self, 'Selector' ):
            write('  Selector=','')
            self.Selector._dump( write2)
        if hasattr(self, 'EventSelector' ):
            write('  EventSelector=','')
            self.EventSelector._dump( write2)
        write('FillerTools= [')
        for hspec in self.FillerTools:
            hspec._dump(write2)
        write(']')
        writeFunc(')')
        
class JetMonAlgSpec(ConfigDict):
    """A dictionary specialized to contain a JetMonitoringAlg specification"""

    def __init__(self, name , defaultPath='standardHistos/', TriggerChain='' ,**args):
        
        self.name = name
        args.setdefault('FillerTools',[])
        args.setdefault('topLevelDir', 'Jets/')
        args.setdefault('bottomLevelDir', '')
        args.setdefault('failureOnMissingContainer', True)
        ConfigDict.__init__(self, defaultPath=defaultPath, TriggerChain=TriggerChain, **args)
        tmpL = self.FillerTools
        self.FillerTools = []
        self.appendHistos(*tmpL)

    def appendHistos(self, *hfillers):
        tmpL = [retrieveHistoToolConf(hspec) for hspec in hfillers]
        self.FillerTools += tmpL
            
    def toAlg(self, monhelper):
        from AthenaConfiguration.ComponentFactory import CompFactory
        alg = monhelper.addAlgorithm(CompFactory.JetMonitoringAlg, self.name)
        alg.TriggerChain = self.TriggerChain
        alg.JetContainerName = self.JetContainerName
        alg.FailureOnMissingContainer = self.failureOnMissingContainer
        
        path = self.defaultPath
        tools = []
        for tconf in self.FillerTools:
            tconf.topLevelDir = self.topLevelDir
            tconf.bottomLevelDir = self.bottomLevelDir
            tools.append( tconf.toTool( ))
            tconf.defineHisto(alg, monhelper, path)
        alg.FillerTools = tools
        return alg

    def _dump(self, writeFunc):
        def write(s,e='\n'): writeFunc('  '+s,e)
        def write2(s,e='\n'): writeFunc('    '+s,e)
        writeFunc(self.__class__.__name__+'(')
        for k,v in sorted(six.iteritems (self)):
            if k == 'FillerTools':
                write('FillerTools = [')
                for hspec in v:
                    #print ' uuuu ', hspec
                    hspec._dump(write2)
                    write('')
                write(']')
            else:
                write(k+' = '+str(v))
        writeFunc(')')


# **************************************                

class JetMonSpecException(Exception):
    pass

def retrieveVarToolConf(alias):
    """Return a VarSpec from alias : 
        * if alias is a string look up in JetStandardHistoSpecs.knownVar
          --> if not found build a VarSpec, assuming alias is an attribute of type float.
        * if alias is a VarSpec, returns it directly
    """
    from JetMonitoring.JetStandardHistoSpecs import knownVar
    if isinstance(alias, str):
        conf = knownVar.get(alias,None)
        if conf is None:
            conf=VarSpec( Name=alias, Type='float')
    else: # assume it's a config dict
        conf = alias
    return conf


def retrieveEventVarToolConf(alias):
    """Return a ToolSpec from alias : (now with EventInfo or JetContainer variables) 
        * if alias is a string build a ToolSpec, assuming alias is an attribute of type float.
        * if alias is a ToolSpec, returns it directly
    """
    from JetMonitoring.JetStandardHistoSpecs import knownEventVar
    if isinstance(alias, str): #check for existing event or jetcontainer specs
        conf = knownEventVar.get(alias,None) 
        if conf is None: #assume it's an eventInfo variable
          conf = ToolSpec('EventHistoVarTool', alias, Attribute=alias) 
    else: # assume it's a config dict
        conf = alias
    return conf


def retrieveHistoToolConf(alias):
    """Return a HistoSpec from alias : 
        * if alias is a string look up in JetStandardHistoSpecs.knownHistos
          --> if found, return a full clone (so client can modify it safely)
          --> if not found and contain a ';' build a HistoSpec for a 2D histograms
        * if alias is a ToolSpec, returns it directly
    """
    from JetMonitoring.JetStandardHistoSpecs import knownHistos
    if isinstance(alias, ToolSpec):
        return alias
    elif isinstance(alias,str):        
        # get it from knownHistos
        c = knownHistos.get(alias,None)
        if c :
            # found a knownHistos.
            # we return a *clone* so that it can be modified without changing the standard spec.
            return c.clone(c.name) 
        if ';' not in alias:
            raise JetMonSpecException(" Unknown Histo specification : '{}' ".format(alias))
            # print 'ERROR unknown Histo Filler specification ', alias
            # return None
        # we have a ';' : try to generate a 2D histo
        aliases = alias.split(';')
        aliasX, aliasY = aliases[0], aliases[1]
        cX = knownHistos.get(aliasX,None)
        cY = knownHistos.get(aliasY,None)
        if len(aliases) == 2:
            if None in (cX,cY):
                print("ERROR unknown Histo Filler specification  ", cX if cX is None else cY)
                return None
            # merge the spec            
            return cX.to2DSpec(cY)
        else: # must be 3
            aliasZ = aliases[2]
            cZ = knownHistos.get(aliasZ,None)
            if cZ is None:
                # then we can try to build a dummy HistoSpec from a variable
                #  this is ok, since we're building a TProfile2D and we don't need
                #  binning info on the xAxis
                vZ = retrieveVarToolConf(aliasZ)
                cZ = HistoSpec(vZ.Name, (10,0,1) , title=vZ.Name+';'+vZ.Name+';', xvar=vZ )
            return cX.to2DSpec(cY, zspec=cZ)
        
    else:
        # What is this :
        print('ERROR can not instantiate a tool from ', alias)
        return None


