# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigGenericMonitoringToolConfig.py')

#
# For now, we use the thread-safe monitoring tool only if needed
# Import the correct one and alias it as "TrigGenericMonitoringTool"
#
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
if jp.ConcurrencyFlags.NumThreads()==0:
    from TrigMonitorBase.TrigMonitorBaseConf import \
        TrigGenericMonitoringTool_NoMutex_IMonitoredAlgo__IGetterp_ \
        as TrigGenericMonitoringTool
    log.info("Using regular TrigGenericMonitoringTool")
else:
    from TrigMonitorBase.TrigMonitorBaseConf import \
        TrigGenericMonitoringTool_std__mutex_ContextGetter_IMonitoredAlgo__IGetter_s_ \
        as TrigGenericMonitoringTool
    log.info("Using thread-safe TrigGenericMonitoringTool")


## Generate histogram definition string for the `TrigGenericMonitoringTool.Histograms` property
#
#  For full details see the TrigGenericMonitoringTool documentation.
#  @param varname  one (1D) or two (2D) variable names separated by comma
#  @param type     histogram type
#  @param path     top-level histrogram directory
#  @param title    Histogram title and optional axis title (same syntax as in TH constructor)
#  @param opt      Histrogram options (see TrigGenericMonitoringTool)
#  @param labels   List of bin labels (for a 2D histogram, sequential list of x- and y-axis labels)
def defineHistogram(varname, type='TH1F', path='EXPERT',
                    title=None,
                    xbins=100, xmin=0, xmax=1,
                    ybins=None, ymin=None, ymax=None, zmin=None, zmax=None, opt='', labels=None):

    if title is None: title=varname
    coded = "%s, %s, %s, %s, %d, %f, %f" % (path, type, varname, title, xbins, xmin, xmax)
    if ybins is not None:
        coded += ",%d, %f, %f" % (ybins, ymin, ymax)
        if zmin is not None:
            coded += ", %f, %f" % (zmin, zmax)
    if ybins is None and ymin is not None:
        coded += ", %f, %f" % (ymin, ymax)

    if isinstance(labels, list) and len(labels)>0:
        coded += ',' + ':'.join(labels) + ':'

    # For backwards compatibility
    elif labels is not None:
        labels = labels.strip()   # remove spurious white-spaces
        if len(labels)>0:
            if labels[-1]!=':': labels += ':'  # C++ parser expects at least one ":"
            coded += ",%s " % labels
    coded += ", %s" % opt

    return coded


class TrigGenericMonitoringToolConfig(TrigGenericMonitoringTool):
    """ Configurable for: TrigGenericMonitoringTool"""

    
    def __init__(self, name):
        """ The contructor for the configurable. """
        super(TrigGenericMonitoringTool,self).__init__(name)
        self.__target = ['Undefined']

    def defineTarget(self, t):
        """ This method is used to define target. Argument can be string or list of strings.


        The intention of the target is as follows. The tools designed for offline (i.e. Validation)
        should not run in online (i.e. they generate to many histograms). So user can define target for
        each of the tool and adds ALL tools to the list of monitoring. Then in global JO the uneccessary
        tols are removed.
        """
        if type(t) is type(""):
            self.__target = [ t ]
        elif type(t) is type([]):
            self.__target = t

    def target(self):
        return self.__target

    def deleteHistogramOfVariable(self, varname):
        """ Undeclares histograms for some varaibles


        It simply looks through the list of histogram defintions and if the name varanem is found in one of this definitions
        it is removed.
        """
        for item in self.Histograms:
            p = item.split(", ")
            if varname in p[2]:
                self.Histograms.remove(item)
        
    def deleteHistogramOfType(self, type):
        """ Undeclares histograms for some histogram types """
        for item in self.Histograms:
            p = item.split(", ")
            if type in p[1]:
                self.Histograms.remove(item)        

    def deleteHistogramInPath(self, path):
        """ Undeclares histograms for some booking path """
        for item in self.Histograms:
            p = item.split(", ")
            if path in p[0]:
                self.Histograms.remove(item)


class ErrorCodeMonitoringToolConfig(TrigGenericMonitoringTool):
    def __init__(self, name):
        """ Constructs generic monitoring tools tool tunned for errors monitoring.

        It simply defines on histogram with bins labeled by error codes.
        """
        super(TrigGenericMonitoringTool,self).__init__(name)
        
        self.__target = ['ErrorCode']
        #actions=['C', 'AC', 'AE', 'AJ']
        #reasons=['', 'missingF', 'gExc', 'efExc', 'stdExc', 'unkExc', 'navErr', 'missingROD', 'corruptROD', 'timeout', 'badSetup', 'user1', 'user2', 'user3', 'user4','']
        #labelsDescription = ''
        #for a in actions:
        #    for r in reasons:
        #        labelsDescription +=  a+'_'+r+':'

        # above lines generate the string below        
        labelsDescription='C_:C_missingF:C_gExc:C_efExc:C_stdExc:C_unkExc:C_navErr:C_missingROD:C_corruptROD:C_timeout:C_badSetup:C_user1:C_user2:C_user3:C_user4:C_:AC_:AC_missingF:AC_gExc:AC_efExc:AC_stdExc:AC_unkExc:AC_navErr:AC_missingROD:AC_corruptROD:AC_timeout:AC_badSetup:AC_user1:AC_user2:AC_user3:AC_user4:AC_:AE_:AE_missingF:AE_gExc:AE_efExc:AE_stdExc:AE_unkExc:AE_navErr:AE_missingROD:AE_corruptROD:AE_timeout:AE_badSetup:AE_user1:AE_user2:AE_user3:AE_user4:AE_:AJ_:AJ_missingF:AJ_gExc:AJ_efExc:AJ_stdExc:AJ_unkExc:AJ_navErr:AJ_missingROD:AJ_corruptROD:AJ_timeout:AJ_badSetup:AJ_user1:AJ_user2:AJ_user3:AJ_user4:AJ_:'
        #print labelsDescription
        
        self.Histograms = [ defineHistogram('ReturnErrorCode', type='TH1I', title="ReturnErrorCodes", xbins=64, xmin=-0.5, xmax=63.5, labels=labelsDescription) ]

    def target(self):
        return ["ErrorCode"]

    def renameUserCodes(self, u1=None, u2=None, u3=None, u4=None):
        from string import replace
        if u1 is not None:
            self.Histograms[0] = self.Histograms[0].replace('user1', u1)
        if u2 is not None:
            self.Histograms[0] = self.Histograms[0].replace('user2', u2)
        if u3 is not None:
            self.Histograms[0] = self.Histograms[0].replace('user3', u3)
        if u4 is not None:
            self.Histograms[0] = self.Histograms[0].replace('user4', u4)

    def monitorUnmappedCode(self):
        hdef = self.Histograms[0]
        self.Histograms += [ hdef.replace('Return', 'Unmapped') ]
        
        
    def monitorActionFixups(self):
        self.Histograms += [defineHistogram('UnmappedErrorCode,ReturnErrorCode', type='TH2I',
                                            title='EC action fixup diagonal means no fixups;original action;mapped action',
                                            xbins=4, xmin=0., xmax=64., ybins=4, ymin=0., ymax=64.,
                                            labels='from continue: from abort chain:from abort event:from abort job:to continue: to abort chain:to abort event:to abort job')]
