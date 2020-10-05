# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import re

_pattern = r"(?P<mult>\d*)(e(?P<threshold1>\d+))(e(?P<threshold2>\d+))*"
_cpattern = re.compile( _pattern )
_possibleSel  = { 'tight':'Tight', 'medium':'Medium', 'loose':'Loose', 'vloose':'VeryLoose',
                  'lhtight':'Tight', 'lhmedium':'Medium', 'lhloose':'Loose', 'lhvloose':'VeryLoose'}
 
from AthenaCommon.SystemOfUnits import GeV
#from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool,defineHistogram
# Just because the TriggerFlags Online and doValidation doen't work
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags 


def _GetPath( cand, sel, basepath = 'RingerSelectorTools/TrigL2_20180903_v9' ):
  
  from AthenaCommon.Logging import logging
  logger = logging.getLogger("TrigMultiVarHypo.GetPath")
  from TriggerMenu.egamma.EgammaSliceFlags import EgammaSliceFlags
  if EgammaSliceFlags.ringerVersion():
    basepath = EgammaSliceFlags.ringerVersion()
  logger.info('TrigMultiVarHypo version: %s', basepath)
  if sel not in _possibleSel.keys():
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


def _IncTool(name, cand, threshold, sel):

  from TrigMultiVarHypo.TrigMultiVarHypoConf import TrigL2CaloRingerHypoToolMT
  
  tool = TrigL2CaloRingerHypoToolMT( name ) 
  tool.AcceptAll = False
  tool.MonTool = ""
  tool.EtCut = (float(threshold)-3.)*GeV

  # monitoring part  
  from TriggerJobOpts.TriggerFlags import TriggerFlags
  if (('Validation' in TriggerFlags.enableMonitoring()) or
      ('Online' in  TriggerFlags.enableMonitoring())    or 
      (athenaCommonFlags.isOnline)):
  
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool,defineHistogram
    monTool = GenericMonitoringTool('MonTool'+name)
    monTool.Histograms = [
      defineHistogram( "TIME_total", path='EXPERT',title="Total Time;time[ms]",xbins=50, xmin=0,xmax=5,type='TH1F'),
      defineHistogram( "TIME_preproc", path='EXPERT',title="Total Time;time[ms]",xbins=50, xmin=0,xmax=5,type='TH1F'),
      defineHistogram( "TIME_propagate",path='EXPERT', title="Total Time;time[ms]",xbins=50, xmin=0,xmax=5,type='TH1F'),
      defineHistogram('Eta', type='TH1F', path='EXPERT',title="#eta of Clusters; #eta; number of RoIs", xbins=50,xmin=-2.5,xmax=2.5),
      defineHistogram('Phi',type='TH1F', path='EXPERT',title="#phi of Clusters; #phi; number of RoIs", xbins=64,xmin=-3.2,xmax=3.2),
      defineHistogram('Et',type='TH1F', path='EXPERT',title="E_{T} of Clusters; E_{T} [MeV]; number of RoIs", xbins=60,xmin=0,xmax=5e4),
      defineHistogram('RnnOut',type='TH1F', path='EXPERT',title="Neural Network output; NN output; number of RoIs", xbins=100,xmin=-10,xmax=10),
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





# Just need to modify the in order to construct the hypo
def _MultTool(name):
  from TrigMultiVarHypo.TrigMultiVarHypoConf import TrigL2CaloRingerHypoToolMTMult
  return TrigL2CaloRingerHypoToolMTMult( name )





def TrigL2CaloRingerHypoToolFromDict( d ):
  """ Use menu decoded chain dictionary to configure the tool """
  cparts = [i for i in d['chainParts'] if ((i['signature']=='Electron') or (i['signature']=='Photon'))]
    
  from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgOnlineDefault
  LuminosityCondAlgOnlineDefault()
    
  def __mult(cpart):
    return int( cpart['multiplicity'] )

  def __th(cpart):
    return cpart['threshold']
    
  def __sel(cpart):
    return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']
    
  def __cand(cpart):
    return cpart['trigType']

  name = d['chainName']

  # do we need to configure high multiplicity selection, either NeX or ex_ey_ez etc...?
  if len(cparts) > 1 or __mult(cparts[0]) > 1:
    tool = _MultTool(name)
    for cpart in cparts:
      for cutNumber in range( __mult( cpart ) ):
        tool.SubTools += [ _IncTool( cpart['chainPartName']+"_"+str(cutNumber), __cand(cpart), __th(cpart), __sel(cpart) ) ]
    # return the tool with all subtools
    return tool
  else:        
    return _IncTool( name, __cand(cparts[0]), __th(cparts[0]), __sel(cparts[0]) )

