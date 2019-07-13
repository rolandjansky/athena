#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaMonitoring.AthenaMonitoringConf import GenericMonitoringTool as _GenericMonitoringTool

log = logging.getLogger(__name__)

class GenericMonitoringTool(_GenericMonitoringTool):
    """Configurable of a GenericMonitoringTool"""

    def __init__(self, name, **kwargs):
        super(GenericMonitoringTool, self).__init__(name, **kwargs)
        self.convention=''

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
        if path is None: path = ''
    assert path is not None, "path is required"
    assert labels is None or isinstance(labels, list), "labels must be of type list"
    # assert labels is None or !isinstance(labels, list), \
           # "Mixed use of variable bin widths and bin labels."

    if title is None:
        title = varname

    if athenaCommonFlags.isOnline() and type in ['TEfficiency']:
        log.warning('Histogram %s of type %s is not supported for online running and will not be added', varname, type)
        return ""

    coded = "%s, %s, %s, %s, %s, %s, " % (path, type, weight, convention, varname, title)

    if not isinstance(xbins,list):
        coded += '%d, %f, %f' % (xbins, xmin, xmax)
    else:
        # List of :-separated bins, plus two empty spaces for xmin and xmax
        coded += ':'.join([str(xbin) for xbin in xbins])

    if ybins is not None:
        if not isinstance(ybins,list):
            coded += ", %d, %f, %f" % (ybins, ymin, ymax)
        else:
            coded += ', ' + ':'.join([str(ybin) for ybin in ybins])
        if zmin is not None:
            coded += ", %f, %f" % (zmin, zmax)

    if ybins is None and ymin is not None:
        coded += ", %f, %f" % (ymin, ymax)

    if labels is not None and len(labels)>0:
        coded += ', ' + ':'.join(labels) + (':' if len(labels) == 1 else '')    # C++ parser expects at least one ":"

    if len(opt)>0:
        coded += ", %s" % opt

    return coded
