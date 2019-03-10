# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import re

_pattern = "(?P<mult>\d*)(e(?P<threshold1>\d+))(e(?P<threshold2>\d+))*"
_cpattern = re.compile( _pattern )
_possibleSel  = { 'tight':'Tight', 'medium':'Medium', 'loose':'Loose', 'vloose':'VeryLoose',
                  'lhtight':'Tight', 'lhmedium':'Medium', 'lhloose':'Loose', 'lhvloose':'VeryLoose'}
 
from AthenaCommon.SystemOfUnits import GeV





def _GetPath( cand, sel, basepath = 'RingerSelectorTools/TrigL2_20170221_v6' ):
  
  from AthenaCommon.Logging import logging
  logger = logging.getLogger("TrigMultiVarHypo.GetPath")
  from TriggerMenu.egamma.EgammaSliceFlags import EgammaSliceFlags
  if EgammaSliceFlags.ringerVersion():
    basepath = EgammaSliceFlags.ringerVersion()
  logger.info('TrigMultiVarHypo version: %s', basepath)
  if not sel in _possibleSel.keys():
    raise RuntimeError( "Bad selection name: %s" % sel )
  if 'e' in cand:
    constant = basepath+'/'+ 'TrigL2CaloRingerElectron{SEL}Constants.root'.format(SEL=_possibleSel[sel])
    threshold = basepath+'/'+ 'TrigL2CaloRingerElectron{SEL}Thresholds.root'.format(SEL=_possibleSel[sel])
  elif 'g' in cand:
    constant = basepath+'/'+ 'TrigL2CaloRingerPhoton{SEL}Constants.root'.format(SEL=_possibleSel[sel])
    threshold = basepath+'/'+ 'TrigL2CaloRingerPhoton{SEL}Thresholds.root'.format(SEL=_possibleSel[sel])
  else:
    raise RuntimeError( "Bad signature %s" % cand )
  return constant, threshold










def _HypoTool(name, cand, threshold, sel):

  from TrigMultiVarHypo.TrigMultiVarHypoConf import TrigL2CaloRingerHypoToolMT
  tool = TrigL2CaloRingerHypoToolMT( name ) 
  tool.AcceptAll = False
  tool.MonTool = ""
  tool.EtCut = (float(threshold)-3.)*GeV
  
  from TriggerJobOpts.TriggerFlags import TriggerFlags
  if 'Validation' in TriggerFlags.enableMonitoring() or 'Online' in  TriggerFlags.enableMonitoring():
  
    from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool,defineHistogram
    monTool = GenericMonitoringTool('MonTool'+name)
    monTool.Histograms = [
      defineHistogram( "TIME_total", path='EXPERT',title="Total Time;time[ms]",xbins=50, xmin=0,xmax=5,type='TH1F'),
      defineHistogram( "TIME_preproc", path='EXPERT',title="Total Time;time[ms]",xbins=50, xmin=0,xmax=5,type='TH1F'),
      defineHistogram( "TIME_propagate",path='EXPERT', title="Total Time;time[ms]",xbins=50, xmin=0,xmax=5,type='TH1F'),
      defineHistogram('Eta', type='TH1F', path='EXPERT',title="#eta of Clusters; #eta; number of RoIs", xbins=50,xmin=-2.5,xmax=2.5),
      defineHistogram('Phi',type='TH1F', path='EXPERT',title="#phi of Clusters; #phi; number of RoIs", xbins=64,xmin=-3.2,xmax=3.2),
      defineHistogram('Et',type='TH1F', path='EXPERT',title="E_{T} of Clusters; E_{T} [MeV]; number of RoIs", xbins=60,xmin=0,xmax=5e4),
      defineHistogram('RnnOut',type='TH1F', path='EXPERT',title="E_{T} of Clusters; E_{T} [MeV]; number of RoIs", xbins=100,xmin=-10,xmax=10),
    ]
    
    monTool.HistPath='L2CaloHypo_Ringer/'+monTool.name()
    tool.MonTool=monTool




  if sel == 'nocut':
    tool.AcceptAll = True
  elif sel == "etcut":
    tool.EtCut = (float(threshold)-3.)*GeV
  elif sel in _possibleSel.keys(): # real selection
    pconstants, pthresholds = _GetPath( cand, sel )
    tool.ConstantsCalibPath = pconstants
    tool.ThresholdsCalibPath = pthresholds

  return tool


def _AlgTool(name):
    from TrigMultiVarHypo.TrigMultiVarHypoConf import TrigL2CaloRingerHypoAlgMT
    return TrigL2CaloRingerHypoAlgMT( name )


def decodeThreshold( threshold ):
    """ decodes the thresholds of the form e10, 2e10, e10e15, ... """
    print "TrigL2CaloHypoToolFromName: decoding threshold ", threshold
    if threshold[0].isdigit(): # is if the from NeX, return as list [X,X,X...N times...]
        assert threshold[1] == 'e', "Two digit multiplicity not supported"
        return [ threshold[2:] ] * int( threshold[0] )
    if threshold.count('e') > 1: # is of the form eXeYeZ, return as [X, Y, Z]
        return threshold.strip('e').split('e')
    if threshold.count('g') > 1: # us of the form gXgYgZ, return as [X, Y, Z]
        return threshold.strip('g').split('g')
    # inclusive, return as 1 element list
    return [ threshold[1:] ] ,  threshold[0]





def createRingerDecisions( name, chains, ClustersKey="CaloClusters",RingerKey="CaloRings" ):

    # set the name of the HypoTool (name=chain) and figure out the threshold and selection from conf """
    from AthenaCommon.Constants import INFO
    tool = _AlgTool(name) 
    from AthenaCommon.AppMgr import ToolSvc
    from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
    ToolSvc+=LuminosityToolOnline()
    
    if not type(chains) is list:
      chains=[chains]

    hypotools = []
    for c in chains:
      #print "Configuring ", name
      bname = c.split('_')
      threshold = bname[1]
      sel = bname[2]
      dt, cand = decodeThreshold( threshold )
      hypotools.append(_HypoTool( c, cand, dt[0], sel ))
    
    for t in tool.HypoTools:
       t.OutputLevel=INFO
    
    tool.HypoTools = hypotools
    tool.ClustersKey = ClustersKey
    tool.RingerKey = RingerKey
    return tool









