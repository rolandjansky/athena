#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
from AthenaCommon.Configurable import Configurable
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

import json
import six

from AthenaConfiguration.ComponentFactory import isRun3Cfg

if isRun3Cfg():
    from GaudiConfig2.Configurables import GenericMonitoringTool as _GenericMonitoringTool
else:
    from AthenaMonitoringKernel.AthenaMonitoringKernelConf import GenericMonitoringTool as _GenericMonitoringTool

log = logging.getLogger(__name__)

class GenericMonitoringTool(_GenericMonitoringTool):
    """Configurable of a GenericMonitoringTool"""

    __slots__ = ['_convention', '_defaultDuration']

    def __init__(self, name=None, *args, **kwargs):
        self._convention = ''
        self._defaultDuration = kwargs.pop('defaultDuration', None)
        super(GenericMonitoringTool, self).__init__(name, *args, **kwargs)

    if not isRun3Cfg():
        def __new__( cls, name=None, *args, **kwargs ):
            if not Configurable.configurableRun3Behavior:
                if name is None: name = cls.__name__

            # GenericMonitoringTool is always private. To avoid the user having
            # to ensure a unique instance name, always create a new instance.
            b = Configurable.configurableRun3Behavior
            Configurable.configurableRun3Behavior = 1
            try:
                conf = super(GenericMonitoringTool, cls).__new__( cls, name, *args, **kwargs )
            finally:
                Configurable.configurableRun3Behavior = b

            return conf

    @property
    def convention(self):
        return self._convention

    @convention.setter
    def convention(self, value):
        self._convention = value

    @property
    def defaultDuration(self):
        return self._defaultDuration

    @defaultDuration.setter
    def defaultDuration(self, value):
        self._defaultDuration = value

    def _coreDefine(self, deffunc, *args, **kwargs):
        if 'convention' in kwargs:
            # only if someone really knows what they're doing
            pass
        else:
            duration = kwargs.pop('duration', self.defaultDuration)
            if duration is not None:
                kwargs['convention'] = self.convention + ':' + duration
        self.Histograms.append(deffunc(*args, **kwargs))

    def defineHistogram(self, *args, **kwargs):
        self._coreDefine(defineHistogram, *args, **kwargs)

    def defineTree(self, *args, **kwargs):
        self._coreDefine(defineTree, *args, **kwargs)

class GenericMonitoringArray:
    '''Array of configurables of GenericMonitoringTool objects'''
    def __init__(self, name, dimensions, **kwargs):
        self.Tools = {}
        self.Postfixes, self.Accessors = GenericMonitoringArray._postfixes(dimensions)
        for postfix in self.Postfixes:
            self.Tools[postfix] = GenericMonitoringTool(name+postfix,**kwargs)

    def __getitem__(self,index):
        '''Forward operator[] on class to the list of tools'''
        return self.toolList()[index]

    def toolList(self):
        return list(self.Tools.values())

    def broadcast(self, member, value):
        '''Allows one to set attributes of every tool simultaneously

        Arguments:
        member -- string which contains the name of the attribute to be set
        value -- value of the attribute to be set
        '''
        for tool in self.toolList():
            setattr(tool,member,value)

    def defineHistogram(self, varname, title=None, path=None, pattern=None, **kwargs):
        '''Propogate defineHistogram to each tool, adding a unique tag.
        
        Arguments:
        pattern -- if specified, list of n-tuples of indices for plots to create
        '''
        unAliased = varname.split(';')[0]
        _, aliasBase = _alias(varname)
        if aliasBase is None:
            return
        if pattern is not None:
            import six
            try:
                iter(pattern)
            except TypeError:
                raise ValueError('Argument to GenericMonitoringArray.defineHistogram must be iterable')
            if not isinstance(pattern, list):
                pattern = list(pattern)
            if len(pattern) == 0: # nothing to do
                return
            if isinstance(pattern[0], six.string_types + six.integer_types):
                # assume we have list of strings or ints; convert to list of 1-element tuples
                pattern = [(_2,) for _2 in pattern]
        for postfix, tool in self.Tools.items():
            aliased = unAliased+';'+aliasBase+postfix

            try:
                accessors = tuple(self.Accessors[postfix])
                if pattern is not None:
                    if accessors not in pattern:
                        continue
                if title is not None:
                    kwargs['title'] = title.format(*accessors)
                if path is not None:
                    kwargs['path'] = path.format(*accessors)
            except IndexError as e:
                log.error('In title or path template of histogram {0}, too many positional '\
                    'arguments were requested. Title and path templates were "{1}" and "{2}", '\
                    'while only {3} fillers were given: {4}.'.format(aliased, title,\
                    path, len(accessors), accessors))
                raise e

            tool.defineHistogram(aliased, **kwargs)

    @staticmethod
    def _postfixes(dimensions, previous=''):
        '''Generates a list of subscripts to add to the name of each tool

        Arguments:
        dimensions -- List containing the lengths of each side of the array off tools
        previous -- Strings appended from the other dimensions of the array
        '''
        import collections
        assert isinstance(dimensions,list) and len(dimensions)>0, \
            'GenericMonitoringArray must have list of dimensions.'
        try:
            if dimensions==[1]:
                return [''], {'': ['']}
        except AttributeError: 
            #Evidently not [1]
            pass
        postList = []
        accessorDict = collections.OrderedDict()
        first = dimensions[0]
        if isinstance(first,list):
            iterable = first
        elif isinstance(first,int):
            iterable = range(first)
        else:
            #Assume GaudiConfig2.semantics._ListHelper
            iterable = list(first)
            #print("Type of first:",type(first))
        for i in iterable:
            if len(dimensions)==1:
                postList.append(previous+'_'+str(i))
                accessorDict[previous+'_'+str(i)]=[str(i)]
            else:
                postfixes, accessors = GenericMonitoringArray._postfixes(dimensions[1:],previous+'_'+str(i))
                postList.extend(postfixes)
                for acckey, accval in accessors.items():
                    accessorDict[acckey] = [str(i)] + accval
        return postList, accessorDict


## Check if name is an allowed histogram/branch name
#
#  Certain characers are best avoided in ROOT histogram names as it makes interactive
#  use awkward. Also there are additional constraints from OH and MDA archiving for
#  online running (ATR-15173).
#
#  @param name string to check
#  @return set of forbidden characters found
def _invalidName(name):
    blacklist = '/\\'
    if athenaCommonFlags.isOnline():
        blacklist += '=,:.()'
    return set(name).intersection(blacklist)

## Generate an alias for a set of variables
#
#  A helper function is useful for this operation, since it is used both by the module
#  function defineHistogram, as well as by the GenericMonitoringArray defineHistogram
#  member function.
#  @param varname unparsed
#  @return varList, alias
def _alias(varname):
    variableAliasSplit = varname.split(';')
    varList = [v.strip() for v in variableAliasSplit[0].split(',')]
    if len(variableAliasSplit)==1:
        return varList, '_vs_'.join(reversed(varList))
    elif len(variableAliasSplit)==2:
        return varList, variableAliasSplit[1]
    else:
        message = 'Invalid variable or alias for {}. Histogram(s) not defined.'
        log.warning(message.format(varname))
        return None, None

## Generate dictionary entries for opt strings
#  @param opt string or dictionary specifying type
#  @return dictionary full of options
def _options(opt):
    # Set the default dictionary of options
    settings = {
        'Sumw2': False,                 # store sum of squares of weights
        'kLBNHistoryDepth': 0,          # length of lumiblock history
        'kAddBinsDynamically': False,   # add new bins if fill is outside axis range
        'kRebinAxes': False,            # increase axis range without adding new bins
        'kCanRebin': False,             # allow all axes to be rebinned
        'kVec': False,                  # add content to each bin from each element of a vector
        'kVecUO': False,                # same as above, but use 0th(last) element for underflow(overflow)
        'kCumulative': False,           # fill bin of monitored object's value, and every bin below it
    }
    if opt is None:
        # If no options are provided, skip any further checks.
        pass
    elif isinstance(opt, dict):
        # If the user provides a partial dictionary, update the default with user's.
        # Check that each provided option is valid
        keyValid = [option in settings for option in opt]
        assert all(keyValid), 'Unknown option provided in opt dictionary. Choices are'+\
            '['+', '.join(settings)+'].'
        typeValid = [isinstance(opt[key], type(val)) for key, val in zip(settings.items())]
        assert all(typeValid), 'An incorrect type was provided in opt dictionary.'
        settings.update(opt)
    elif isinstance(opt, str) and len(opt)>0:
        # If the user provides a comma- or space-separated string of options.
        from argparse import ArgumentParser # a module to parse a string of options
        parser = ArgumentParser()
        for settingName, settingValue in settings.items():
            opt = opt.replace(settingName, '--'+settingName)
            if isinstance(settingValue, bool):
                parser.add_argument('--'+settingName, action='store_true')
            else:
                settingType = type(settingValue)
                parser.add_argument('--'+settingName, default=settingValue, type=settingType)
        known, unknown = parser.parse_known_args(opt.replace(',',' ').split(' '))
        settings = vars(known)
    return settings

## Generate histogram definition string for the `GenericMonitoringTool.Histograms` property
#
#  For full details see the GenericMonitoringTool documentation.
#  @param varname  one (1D) or two (2D) variable names separated by comma
#                  optionally give histogram name by appending ";" plus the name
#  @param type     histogram type
#  @param path     top-level histogram directory (e.g. EXPERT, SHIFT, etc.)
#  @param title    Histogram title and optional axis title (same syntax as in TH constructor)
#  @param weight   Name of the variable containing the fill weight
#  @param cutmask  Name of the boolean-castable variable that determines if the plot is filled
#  @param opt      String or dictionary of histogram options (see _options())
#  @param treedef  Internal use only. Use defineTree() method.
#  @param xlabels  List of x bin labels.
#  @param ylabels  List of y bin labels.
#  @param zlabels  List of x bin labels.
#  @param merge    Merge method to use for object, if not default. Possible algorithms for offline DQM
#                  are given in https://twiki.cern.ch/twiki/bin/view/Atlas/DQMergeAlgs
def defineHistogram(varname, type='TH1F', path=None,
                    title=None, weight=None,
                    xbins=100, xmin=0, xmax=1, xlabels=None,
                    ybins=None, ymin=None, ymax=None, ylabels=None,
                    zmin=None, zmax=None, zlabels=None,
                    opt=None, convention=None, cutmask=None,
                    treedef=None, merge=None):

    # All of these fields default to an empty string
    stringSettingsKeys = ['xvar', 'yvar', 'zvar', 'type', 'path', 'title', 'weight',
                          'cutMask', 'convention', 'alias', 'treeDef', 'merge']
    # All of these fileds default to 0
    numberSettingsKeys = ['xbins', 'xmin', 'xmax', 'ybins', 'ymin', 'ymax', 'zbins',
                          'zmin', 'zmax']
    # All of these fields default to an empty array
    arraySettingsKeys = ['allvars', 'xlabels', 'xarray', 'ylabels', 'yarray', 'zlabels']
    # Initialize a dictionary with all possible fields
    settings = dict((key, '') for key in stringSettingsKeys)
    settings.update(dict((key, 0.0) for key in numberSettingsKeys))
    settings.update(dict((key, []) for key in arraySettingsKeys))

    # Alias
    varList, alias = _alias(varname)
    if alias is None:
        return ''

    invalid = _invalidName(alias)
    if invalid:
        log.warning('%s is not a valid histogram name. Illegal characters: %s',
                    alias, ' '.join(invalid))
        return ''

    settings['alias'] = alias

    # Variable names
    if len(varList)>0:
        settings['xvar'] = varList[0]
    if len(varList)>1:
        settings['yvar'] = varList[1]
    if len(varList)>2:
        settings['zvar'] = varList[2]
    settings['allvars'] = varList
    nVars = len(varList)

    # Type
    if athenaCommonFlags.isOnline() and type in ['TEfficiency', 'TTree']:
        log.warning('Object %s of type %s is not supported for online running and '
                    'will not be added.', varname, type)
        return ''
    # Check that the histogram's dimension matches the number of monitored variables
    # Add TTree to the lists, it can have any number of vars
    hist2D = ['TH2','TProfile','TEfficiency', 'TTree']
    hist3D = ['TProfile2D','TEfficiency', 'TTree']
    if nVars==2:
        assert any([valid2D in type for valid2D in hist2D]),'Attempting to use two '
        'monitored variables with a non-2D histogram.'
    elif nVars==3:
        assert any([valid3D in type for valid3D in hist3D]),'Attempting to use three '
        'monitored variables with a non-3D histogram.'
    settings['type'] = type

    # Path
    if not athenaCommonFlags.isOnline() and path is None:
        path = ''
    assert path is not None, 'path argument in defineHistogram() is required.'
    settings['path'] = path

    # Title
    if title is None:
        title = varname
    settings['title'] = title

    # Weight
    if weight is not None:
        settings['weight'] = weight

    # Cutmask
    if cutmask is not None:
        settings['cutMask'] = cutmask

    # Output path naming convention
    if convention is not None:
        settings['convention'] = convention

    # Bin counts and ranges
    # Possible types allowed for bin counts
    binTypes = six.integer_types + (list, tuple)

    # X axis count and range
    assert isinstance(xbins, binTypes),'xbins argument must be int, list, or tuple'
    if isinstance(xbins, six.integer_types): # equal x bin widths
        settings['xbins'], settings['xarray'] = xbins, []
    else: # x bin edges are set explicitly
        settings['xbins'], settings['xarray'] = len(xbins)-1, xbins
    settings['xmin'] = xmin
    settings['xmax'] = xmax

    # Y axis count and range
    if ybins is not None:
        assert isinstance(ybins, binTypes),'ybins argument must be int, list, or tuple'
        if isinstance(ybins, six.integer_types): # equal y bin widths
            settings['ybins'], settings['yarray'] = ybins, []
        else: # y bin edges are set explicitly
            settings['ybins'], settings['yarray'] = len(ybins)-1, ybins
    if ymin is not None:
        settings['ymin'] = ymin
    if ymax is not None:
        settings['ymax'] = ymax

    # Z axis count and range
    if zmin is not None:
        settings['zmin'] = zmin
    if zmax is not None:
        settings['zmax'] = zmax

    # Then, parse the [xyz]label arguments
    if xlabels is not None and len(xlabels)>0:
        assert isinstance(xlabels, (list, tuple)),'xlabels must be list or tuple'
        settings['xbins'] = len(xlabels)
        settings['xlabels'] = xlabels
    if ylabels is not None and len(ylabels)>0:
        assert isinstance(ylabels, (list, tuple)),'ylabels must be list or tuple'
        settings['ybins'] = len(ylabels)
        settings['ylabels'] = ylabels
    if zlabels is not None and len(zlabels)>0:
        assert isinstance(zlabels, (list, tuple)),'zlabels must be list or tuple'
        settings['zlabels'] = zlabels

    # merge method
    if merge is not None:
        assert type not in ['TEfficiency', 'TTree', 'TGraph'],'only default merge defined for non-histogram objects'
        settings['merge'] = merge

    # Tree branches
    if treedef is not None:
        assert type=='TTree','cannot define tree branches for a non-TTree object'
        settings['treeDef'] = treedef

    # Finally, add all other options
    settings.update(_options(opt))

    return json.dumps(settings)

## Generate tree definition string for the `GenericMonitoringTool.Histograms` property. Convenience tool for 
#
#  For full details see the GenericMonitoringTool documentation.
#  @param varname  at least one variable name (more than one should be separated by comma);
#                  optionally give the name of the tree by appending ";" plus the tree name
#  @param treedef  TTree branch definition string. Looks like the standard TTree definition
#                  (see https://root.cern.ch/doc/master/classTTree.html#addcolumnoffundamentaltypes).
#                  In fact if only scalars are given, it is exactly the same as you would use to
#                  define the TTree directly: "varA/F:varB/I:...".  Vectors can be defined by giving
#                  "vector<int>", etc., instead of "I".
#  @param path     top-level histogram directory (e.g. EXPERT, SHIFT, etc.)
#  @param title    Histogram title and optional axis title (same syntax as in TH constructor)
#  @param cutmask  Name of the boolean-castable variable that determines if the plot is filled
#  @param opt      TTree options (none currently)
#  @param convention Expert option for how the objects are placed in ROOT

def defineTree(varname, treedef, path=None, title=None,
               opt='', convention=None,
               cutmask=None):
    return defineHistogram(varname, type='TTree', path=path, title=title,
                           treedef=treedef, opt=opt, convention=convention,
                           cutmask=cutmask)
