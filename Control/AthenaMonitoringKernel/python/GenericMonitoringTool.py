#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
from AthenaCommon.Configurable import Configurable
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaMonitoringKernel.AthenaMonitoringKernelConf import GenericMonitoringTool as _GenericMonitoringTool

log = logging.getLogger(__name__)

class GenericMonitoringTool(_GenericMonitoringTool):
    """Configurable of a GenericMonitoringTool"""

    def __init__(self, name=Configurable.DefaultName, *args, **kwargs):
        super(GenericMonitoringTool, self).__init__(name, *args, **kwargs)
        self.convention = ''

    def __new__( cls, *args, **kwargs ):
        # GenericMonitoringTool is always private. To avoid the user having
        # to ensure a unique instance name, always create a new instance.

        b = Configurable.configurableRun3Behavior
        Configurable.configurableRun3Behavior = 1
        try:
            conf = Configurable.__new__( cls, *args, **kwargs )
        finally:
            Configurable.configurableRun3Behavior = b

        return conf

    def defineHistogram(self, *args, **kwargs):
        if 'convention' in kwargs:
            # only if someone really knows what they're doing
            pass
        else:
            if 'duration' in kwargs:
                kwargs['convention'] = self.convention + ':' + kwargs['duration']
                del kwargs['duration']
            elif hasattr(self, 'defaultDuration'):
                kwargs['convention'] = self.convention + ':' + self.defaultDuration
        self.Histograms.append(defineHistogram(*args, **kwargs))

class GenericMonitoringArray:
    '''Array of configurables of GenericMonitoringTool objects'''
    def __init__(self, name, dimensions, **kwargs):
        self.Tools = {}
        for postfix in GenericMonitoringArray._postfixes(dimensions):
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

    def defineHistogram(self, varname, **kwargs):
        '''Propogate defineHistogram to each tool, adding a unique tag.'''
        unAliased = varname.split(';')[0]
        aliasBase = varname.split(';')[1] if ';' in varname else varname.replace(',','')
        for postfix,tool in self.Tools.items():
            aliased = unAliased+';'+aliasBase+postfix
            tool.defineHistogram(aliased,**kwargs)

    @staticmethod
    def _postfixes(dimensions, previous=''):
        '''Generates a list of subscripts to add to the name of each tool.

        Arguments:
        dimensions -- List containing the lengths of each side of the array off tools
        previous -- Strings appended from the other dimensions of the array
        '''
        assert isinstance(dimensions,list) and len(dimensions)>0
        if dimensions==[1]:
            return ['']
        postList = []
        first = dimensions[0]
        if isinstance(first,list):
            iterable = first
        elif isinstance(first,int):
            iterable = range(first)
        for i in iterable:
            if len(dimensions)==1:
                 postList.append(previous+'_'+str(i))
            else:
                postList.extend(GenericMonitoringArray._postfixes(dimensions[1:],previous+'_'+str(i)))
        return postList

## Generate histogram definition string for the `GenericMonitoringTool.Histograms` property
#
#  For full details see the GenericMonitoringTool documentation.
#  @param varname  one (1D) or two (2D) variable names separated by comma
#  @param type     histogram type
#  @param path     top-level histogram directory (e.g. EXPERT, SHIFT, etc.)
#  @param weight   Name of the variable containing the fill weight
#  @param title    Histogram title and optional axis title (same syntax as in TH constructor)
#  @param opt      Histrogram options (see GenericMonitoringTool)
#  @param labels   List of bin labels (for a 2D histogram, sequential list of x- and y-axis labels)

def defineHistogram(varname, type='TH1F', path=None,
                    title=None,weight='',
                    xbins=100, xmin=0, xmax=1,
                    ybins=None, ymin=None, ymax=None,
                    zmin=None, zmax=None,
                    opt='', labels=None, convention=''):

    # Assert argument types
    if not athenaCommonFlags.isOnline():
        if path is None:
            path = ''
    assert path is not None, "path is required"
    assert labels is None or isinstance(labels, (list, tuple) ), "labels must be of type list or tuple"

    if title is None:
        title = varname
    title = title.replace(',','","') # Commas used as delimiters, but "," is ok

    if athenaCommonFlags.isOnline() and type in ['TEfficiency']:
        log.warning('Histogram %s of type %s is not supported for online running and will not be added', varname, type)
        return ""

    coded = "%s, %s, %s, %s, %s, %s, " % (path, type, weight, convention, varname, title)

    if not isinstance(xbins, (list, tuple)):
        coded += '%d, %f, %f' % (xbins, xmin, xmax)
    else:
        # List of :-separated bins, plus two empty spaces for xmin and xmax
        coded += ':'.join(str(xbin) for xbin in xbins)

    if ybins is not None:
        if not isinstance(ybins, (list, tuple)):
            coded += ", %d, %f, %f" % (ybins, ymin, ymax)
        else:
            coded += ', ' + ':'.join(str(ybin) for ybin in ybins)
        if zmin is not None:
            coded += ", %f, %f" % (zmin, zmax)

    if ybins is None and ymin is not None:
        coded += ", %f, %f" % (ymin, ymax)

    if labels is not None and len(labels)>0:
        coded += ', ' + ':'.join(labels) + (':' if len(labels) == 1 else '')    # C++ parser expects at least one ":"

    if len(opt)>0:
        coded += ", %s" % opt

    return coded
