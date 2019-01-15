#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
from AthenaMonitoring.AthenaMonitoringConf import GenericMonitoringTool as _GenericMonitoringTool

log = logging.getLogger(__name__)

class GenericMonitoringTool(_GenericMonitoringTool):
    """Configurable of a GenericMonitoringTool"""

    def __init__(self, name, **kwargs):
        super(GenericMonitoringTool, self).__init__(name, **kwargs)

    def defineHistogram(self, *args, **kwargs):
        self.Histograms.append(defineHistogram(*args, **kwargs))

## Generate histogram definition string for the `GenericMonitoringTool.Histograms` property
#
#  For full details see the GenericMonitoringTool documentation.
#  @param varname  one (1D) or two (2D) variable names separated by comma
#  @param type     histogram type
#  @param path     top-level histogram directory (e.g. EXPERT, SHIFT, etc.)
#  @param title    Histogram title and optional axis title (same syntax as in TH constructor)
#  @param opt      Histrogram options (see GenericMonitoringTool)
#  @param labels   List of bin labels (for a 2D histogram, sequential list of x- and y-axis labels)
def defineHistogram(varname, type='TH1F', path='DEFAULT',
                    title=None,
                    xbins=100, xmin=0, xmax=1,
                    ybins=None, ymin=None, ymax=None, zmin=None, zmax=None, opt='', labels=None):

    if title is None: title=varname
    coded = "%s, %s, %s, %s, %d, %f, %f" % (path, type, varname, title, xbins, xmin, xmax)
    if ybins is not None:
        coded += ", %d, %f, %f" % (ybins, ymin, ymax)
        if zmin is not None:
            coded += ", %f, %f" % (zmin, zmax)
    if ybins is None and ymin is not None:
        coded += ", %f, %f" % (ymin, ymax)

    if isinstance(labels, list) and len(labels)>0:
        coded += ', ' + ':'.join(labels) + ':'    # C++ parser expects at least one ":"

    # For backwards compatibility
    elif labels is not None:
        log.warning("The 'label1:label2' syntax in defineHistogram is deprecated. "
                    "Please use a list instead: ['label1','label2']")
        labels = labels.strip()   # remove spurious white-spaces
        if len(labels)>0:
            if labels[-1]!=':': labels += ':'
            coded += ",%s " % labels

    if len(opt)>0: coded += ", %s" % opt

    return coded
