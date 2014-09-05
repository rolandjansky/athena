# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Main author: tmhong
#
# MakeRoot() does CostResultCollection -> YOURFILE.root
# FitRoot() in development
#
# --------------------
# Note for FillEvent()
# --------------------
#
# Calculate <mu>
#   Numerator   is # of collisions per second --> xsection*bunchlumi
#   Denominator is # of crossings  per second --> 11245.58 Hz,
#       since it takes 88.924 microsec to go around one LHC ring.
#   NB. Inelastic cross-section is = 60.33 +- 2.10 mb according to
#       ATL-COM-PHYS-2011-361: "The Inelastic Cross-Section
#       and Prospects for Elastic Measurements with ATLAS"
#       AtlDataSummary & AtlRunQuery both use 71.5 mb, which
#       seems to be the number for Schuler & Sjostrand without
#       a cut for detector acceptance,which is is 69.4 mb on
#       Atlas (Apr '11).
#   Units for below: mb (1e-27 cm-2), 1e30 cm-2s-1, Hz

#print 'Hello world TrigCostRoot.py'

import os
import sys
import logging
logging.basicConfig(level=logging.INFO)
log = logging.getLogger('TrigCostRoot')
#print '\tLoaded standard packages'

from array import array                 ; #print '\tLoaded special array package'
from TrigCostPython import TrigCostCool ; #print '\tLoaded special TrigCostPython.TrigCostCool package'
try:
    import ROOT                         ; #print '\tLoaded special ROOT package'
    ROOT.gROOT.SetBatch(True)           ; #print '\tROOT set batch mode'
except:
    print "pyROOT not avaiable: setup the release and then "
    print "export PYTHONPATH=$PYTHONPATH:$ROOTSYS/lib"
    sys.exit(-1)

#print '\tDone prelim import for TrigCostRoot.py'

m_logdir='/afs/cern.ch/user/x/xmon/www/log'

#------------------------------------------------------------
# Verbosity
#------------------------------------------------------------
def SetLogging(options=[]):
    if   "silent" in options:
        logging.basicConfig(level=logging.ERROR)
    elif "quiet"  in options:
        logging.basicConfig(level=logging.WARNING)
    elif "debug"  in options:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)

    return


######################################################################
#
# MakeRoot() is the only function you need to convert
#
# CostResultCollection -> YOURFILE.root
#
######################################################################
def MakeRoot(outpath, ratecoll, lbset, options=[], update=False):
    SetLogging(options)

    if ratecoll==None: return
    if lbset==None: return

    costroot = CostRoot(options)
    costroot.GetFile(outpath)
    lb = costroot.FillRoot(ratecoll, lbset, options)
    costroot.SaveRoot(ratecoll)

    # Save last saved lb info (only if `update`)
    run = ratecoll.GetRun()
    if update:
        SaveLastLB(run,lb)

    return


######################################################################
def SaveLastLB(run,lb,dir=''):
    if len(dir)==0:
        dir = m_logdir
    file = 'lastlb.txt'
    name = os.path.join(dir,file)
    lastfile = open( name, 'w' )
    lastfile.write( '%d/%d' % (run,lb) )
    return


######################################################################
#
# FitRoot() is the only function you need to fit the cross-sections in
#
# YOURFILE.root
#
######################################################################
def FitRoot(outpath, ratecoll, options=[]):
    if "silent"  in options: logging.basicConfig(level=logging.ERROR)
    if "quiet"   in options: logging.basicConfig(level=logging.WARNING)
    if "debug"   in options: logging.basicConfig(level=logging.DEBUG)

    if ratecoll==None: return

    costroot = CostRoot(options)
    costroot.GetFile(outpath, 'READ')
    costroot.Fit(ratecoll)

    return



#---------------------------------------------------------------------
#
# Class to store root variable values
#
#---------------------------------------------------------------------
class RootVar:
    def __init__(self, name='', options=[]):
        self.name = name
        self.options = options
        self.varbank = {} # map varname 'tbp' -> python variable
        if 'debug' in self.options:
            if self.name in ['L1_EM16VH']:
                print 'Booking RootVar(%s)' % name

    def SetName(self, name): self.name = name
    def GetName(self): return self.name

    # Variables
    def InitVar(self, varname, vararray): self.varbank[varname] = vararray
    def GetVar(self, varname):  return self.varbank[varname]
    def GetVarNames(self):      return self.varbank.keys()

    # Values
    def SetValue(self, varname, val):
        self.varbank[varname][0] = val # always an array of length 1
        if 'debug' in self.options:
            if self.name in ['L1_EM16VH']:
                print 'Setting', self.name, varname, '=', val, '=', self.varbank[varname][0]

    def GetValue(self, varname, val):
        if self.varbank.has_key(varname):
            return self.varbank[varname]
        return None


#---------------------------------------------------------------------
#
# Class to make root files
#
#---------------------------------------------------------------------
class CostRoot:
    def __init__(self, mode='NEW', options=[]):
        if "silent"  in options: logging.basicConfig(level=logging.ERROR)
        if "quiet"   in options: logging.basicConfig(level=logging.WARNING)
        if "debug"   in options: logging.basicConfig(level=logging.DEBUG)

        self.options = options
        self.tfile = None
        self.ttree = None
        self.tvars = {} #   map (trigger name, evt) -> RootVar()

        # NB: f=float, i=int
        self.varlist = dict()
        self.varlist['evtvar']  = [\
                                ['fill',       'i'], # Fill number
                                ['run',        'i'], # Run number
                                ['lb',         'i'], # Lb number
                                ['time',       'f'], # UTC sec since 1970-1-1:00:00:00.0.0
                                ['lumi',       'f'], # Inst. lumi per crossing (10^30 cm^-2 s^-1)
                                ['lblength',   'f'], # LB length (seconds)
                                ['lblumi',     'f'], # Int. lumi for the duration of the LB (ub^-1)
                                ['l1livefrac', 'f'], # Frac of time period the CTP was recording (after veto)
                                ['bunches',    'i'], # Number of colliding bunches
                                ['nbunch1',    'i'], # Number of bunches in beam 1
                                ['nbunch2',    'i'], # Number of bunches in beam 2
                                ['bunchlumi',  'f'], # Inst. lumi per bunch (10^30 cm^-2 s^-1)
                                ['mu',         'f'], # Interactions per crossing
                               ]
        self.varlist['trigvar'] = [\
                                ['psv',        'f'], # Trigger pre-scale value
                                ['tbp',        'f'], # Trigger counts before prescale
                                ['tap',        'f'], # Trigger counts after prescale
                                ['tav',        'f'], # Trigger counts after veto
                                ['rbp',        'f'], # Trigger rate before prescale
                                ['rap',        'f'], # Trigger rate after prescale
                                ['rav',        'f'], # Trigger rate after veto
                                ['xbp',        'f'], # Trigger x-section before prescale
                                ['xap',        'f'], # Trigger x-section after prescale
                                ['xav',        'f'], # Trigger x-section after veto
                               ]

        return

    #==========================================================================
    def Cast(self, varname, value):

        vartype = ''
        if   varname in self.varlist['trigvar']: vartype=self.varlist['trigvar'][varname]
        elif varname in self.varlist['evtvar' ]: vartype=self.varlist['evtvar' ][varname]

        if   vartype=='i': return int(value)
        elif vartype=='f': return float(value)

        return value

    #==========================================================================
    def Fit(self, ratecoll):
        return


    #==========================================================================
    def FillRoot(self, ratecoll, lbset, options=[]):

        if ratecoll==None: return
        if lbset==None: return
        if self.ttree==None: return
        if self.tfile==None: return
        if self.tvars==None: return

        chlist = ratecoll.GetChainNames()
        run    = ratecoll.GetRun()
        lblist = ratecoll.LBRange()

#       Now moved to makeRoot.py -- Use COOL to get luminosity etc.
#	    lbset = TrigCostCool.GetLumiblocks(run, ratecoll.FirstLB(), ratecoll.LastLB(),options)

        # Loop over lumiblocks
        for lb in lblist:

            # Fill event info
            self.FillEvent('evt', run, lb, lbset, options)

            # Loop over trigger chains
            for i, chname in enumerate(chlist):

                chain = ratecoll.GetCostChain(lb, chname)

#	                # Debugging
#	                if chname == 'L1_MU10': chain.Print()
#	                if chname == 'EF_mu18': chain.Print()
#	                continue

                if chain==None:
                    continue

                # Fill chain info
                lumi = lbset.GetAvgInstantaneousLumiE30(lb, lb) # Was GetRecordedLumi(), a bug!
                self.FillChain(chname, chain, options, lumi)

#	                # Verbose
#	                if True or 'debug' in self.options and (len(chlist)-i <= 5 or i < 5):
#	                    print '#%-4d [%d %d %d=%d] cntr=%-5d %-40s %10f %10f %10f %10f' % (
#	                        i,
#	                        chain.GetFill(),
#	                        chain.GetRun(),
#	                        chain.GetLb(),
#	                        lb,
#	                        chain.GetCounter(),
#	                        chain.GetName(),
#	                        chain.GetPrescale(),
#	                        chain.GetTBPCnt(),
#	                        chain.GetTAPCnt(),
#	                        chain.GetTAVCnt(),
#	                    )

            # Fill tree once per lb
            self.ttree.Fill()

        return lblist[-1]


    #==========================================================================
    def FillEvent(self, chname, run, lb, lbset, options=[]):

        # Event-wise variables -- lumi, bunchlumi, etc.
        chname = 'evt'
        if self.tvars.has_key(chname) == False:
            self.tvars[chname] = RootVar(chname, options)
            self.SetBranch(chname, 'evtvar')

        # Calculate <mu>
        #   Numerator   is # of collisions per second --> xsection*bunchlumi
        #   Denominator is # of crossings  per second --> 11245.58 Hz,
        #       since it takes 88.924 microsec to go around one LHC ring.
        #   NB. Inelastic cross-section is = 60.33 +- 2.10 mb according to
        #       ATL-COM-PHYS-2011-361: "The Inelastic Cross-Section
        #       and Prospects for Elastic Measurements with ATLAS"
        #       AtlDataSummary & AtlRunQuery both use 71.5 mb, which
        #       seems to be the number for Schuler & Sjostrand without
        #       a cut for detector acceptance,which is is 69.4 mb on
        #       Atlas (Apr '11).
        #   Units for below: mb (1e-27 cm-2), 1e30 cm-2s-1, Hz

        bunchlumi = lbset.GetAvgInstantaneousBunchLumiE30(lb)
        mu = 71.5 * bunchlumi / 11.24558

        # Set branch value
        tvar = self.tvars[chname]
        tvar.SetValue('fill',       self.Cast('fill',       lbset.GetFillNumber(lb)))
        tvar.SetValue('run',        self.Cast('run',        run))
        tvar.SetValue('lb',         self.Cast('lb',         lb))
        tvar.SetValue('time',       self.Cast('time',       lbset.GetStartTime(lb)))
        tvar.SetValue('lumi',       self.Cast('lumi',       lbset.GetAvgInstantaneousLumiE30(lb,lb)))
        tvar.SetValue('lblength',   self.Cast('lblength',   lbset.GetTotalTime(lb,lb)))
        tvar.SetValue('lblumi',     self.Cast('lblumi',     lbset.GetRecordedLumi(lb,lb)))
        tvar.SetValue('l1livefrac', self.Cast('l1livefrac', lbset.GetL1LiveFraction(lb,lb)))
        tvar.SetValue('bunches',    self.Cast('bunches',    lbset.GetBunches(lb)))
        tvar.SetValue('nbunch1',    self.Cast('nbunch1',    lbset.GetNbunch1(lb)))
        tvar.SetValue('nbunch2',    self.Cast('nbunch2',    lbset.GetNbunch2(lb)))
        tvar.SetValue('bunchlumi',  self.Cast('bunchlumi',  bunchlumi))
        tvar.SetValue('mu',         self.Cast('mu',         mu))


    #==========================================================================
    def FillChain(self, chname, chain, options=[], lumi=0):

        # Declare variables on demand
        if self.tvars.has_key(chname) == False:
            self.tvars[chname] = RootVar(chname, options)
            self.SetBranch(chname, 'trigvar')

        # Set branch value
        tvar = self.tvars[chname]
        tvar.SetValue('psv', self.Cast('psv', chain.GetPrescale()) )
        tvar.SetValue('tbp', self.Cast('tbp', chain.GetTBPCnt())   )
        tvar.SetValue('tap', self.Cast('tap', chain.GetTAPCnt())   )
        tvar.SetValue('tav', self.Cast('tav', chain.GetTAVCnt())   )
        tvar.SetValue('rbp', self.Cast('rbp', chain.GetTBPRate())  )
        tvar.SetValue('rap', self.Cast('rap', chain.GetTAPRate())  )
        tvar.SetValue('rav', self.Cast('rav', chain.GetTAVRate())  )

        # Recalculate if not set
        rbp = chain.GetTBPRate()
        rap = chain.GetTAPRate()
        rav = chain.GetTAVRate()
        xbp = chain.GetTBPXsec() / 1e-30 # microbarns
        xap = chain.GetTAPXsec() / 1e-30 # microbarns
        xav = chain.GetTAVXsec() / 1e-30 # microbarns

        if 'debug' in self.options:
            if chname=='L1_EM16VH':
                print 'xbp ',xbp, 'lumi=', lumi
                print 'xap ',xap, 'lumi=', lumi
                print 'xav ',xav, 'lumi=', lumi


        tvar.SetValue('xbp', self.Cast('xbp', xbp))
        tvar.SetValue('xap', self.Cast('xap', xap))
        tvar.SetValue('xav', self.Cast('xav', xav))


    #==========================================================================
    def GetFile(self, outpath, mode='RECREATE'):
        self.CheckDir(outpath)
        self.tfile = ROOT.TFile(outpath, mode)
        self.tfile.cd()
        if   mode=='RECREATE': self.ttree = ROOT.TTree('ntp1', 'ntp1')
        elif mode=='READ':     self.ttree = tfile.Get('ntp1')
        return


    #==========================================================================
    # varcategory = 'trigvar' or 'evtvar'
    #
    def SetBranch(self, chname, varcategory='trigvar'):

        maxn = 1

        # Loop over psv, tbp, etc.
        for varname, vartype in self.varlist[varcategory]:

            # Python variable as a unit-length array
            vararray = None
            if   vartype=='i': vararray = array(vartype, maxn*[0])
            elif vartype=='f': vararray = array(vartype, maxn*[0.0])
            else:
                print 'ERROR - SetBranch() is invalid varname', varname, 'vartype', vartype
                sys.exit(-1)

            # Store it globally
            tvar = self.tvars[chname]
            tvar.InitVar(varname, vararray)

            # Name (ex: tbp__L1_EM3 or evt__time)
            namelist = [varname, chname]
            if varcategory != 'trigvar':    # tbp__L1_EM2   (var, then chname)
                namelist.reverse()          # evt__time     (chname, then var)
            leafname = '__'.join(namelist)

            # Declare TBranches (ex: tbp__L1_EM3/F)
            leaflist = '/'.join([leafname, vartype.upper()])
            self.ttree.Branch(leafname, tvar.GetVar(varname), leaflist)

            # Debug
            if 'debug' in self.options:
                print 'VERBOSE - SetBranch()', 0, '=', leaflist

        return


    #==========================================================================
    def CheckDir(self, outpath, filesuffix='.root'):

        # Output file directory
        outdir = os.path.dirname(outpath)       # /afs/blah/test.root --> /afs/blah
        if len(outdir)==0:
            outdir = '.'

        # Check if work directory exists, otherwise make
        if not os.access(outdir, os.F_OK):
            os.mkdir(outdir)

        # Output file name
        outname = os.path.basename(outpath)     # /afs/blah/test.root --> test.root
        outlist = outname.split(filesuffix)     # test.root --> ['test','']
        if len(outlist[0])==0 or len(outlist)!=2:
            print 'ERROR - CheckDir() has invalid file name', outpath, ', exiting'
            sys.exit(-1)

        return


    #==========================================================================
    def SaveRoot(self, ratecoll):

        if self.tfile == None: return
        if self.ttree == None: return
        if ratecoll == None: return

        # Write Tree
        self.tfile.cd()
        self.tfile.Write()

        # Verbose
        if 'debug' in self.options:
            chlist = ratecoll.GetChainNames()
            newlist = chlist[:5] # first 5
            newlist.extend(chlist[-5:]) # last 5
            varlist = []
            for chname in newlist:
                varlist.append( '__'.join(['tbp',chname]) )

            # Scan 10 chains for 30 lb
            ttree = self.tfile.Get('ntp1')
            ttree.SetScanField(0)
            ttree.Scan( ':'.join(varlist), '', '', 30)

        log.info("Writing %s --> %s", self.ttree.GetName(), self.tfile.GetName())

        # Close ROOT
        self.tfile.Close()

        return
#eof
