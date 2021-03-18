# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags 
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthOnnxruntimeService.AthOnnxruntimeServiceConf import AthONNX__ONNXRuntimeSvc
from AthenaCommon.AppMgr import ServiceMgr

# add ONNX into app service mgr
ServiceMgr += AthONNX__ONNXRuntimeSvc()


def same( val , tool):
  return [val]*( len( tool.EtaBins ) - 1 )

class TrigEgammaFastCaloHypoToolConfig:

 
  __operation_points  = [  'tight'    , 
                           'medium'   , 
                           'loose'    , 
                           'vloose'   , 
                           'lhtight'  , 
                           'lhmedium' , 
                           'lhloose'  , 
                           'lhvloose' ]



  def __init__(self, name, cand, threshold, sel, trackinfo, noringerinfo):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaFastCaloHypoTool')
     
    self.__useRun3 = False
    self.__name = name
    self.__cand = cand
    self.__threshold = float(threshold)
    self.__sel = sel
    self.__trackinfo = trackinfo
    self.__noringerinfo = noringerinfo

    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.TrigEgammaFastCaloHypoToolInc( name )
    tool.AcceptAll      = False
    tool.UseRinger      = False
    tool.EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    tool.ETthr          = same( self.__threshold*GeV, tool )
    tool.dETACLUSTERthr = 0.1
    tool.dPHICLUSTERthr = 0.1
    tool.F1thr          = same( 0.005 , tool)
    tool.ET2thr         = same( 90.0*GeV, tool )
    tool.HADET2thr      = same( 999.0  , tool)
    tool.HADETthr       = same( 0.058  , tool)
    tool.WETA2thr       = same( 99999. , tool)
    tool.WSTOTthr       = same( 99999. , tool)
    tool.F3thr          = same( 99999. , tool)
    tool.CARCOREthr     = same( -9999. , tool)
    tool.CAERATIOthr    = same( -9999. , tool)
    self.__tool = tool

    self.__log.debug( 'Chain     :%s', name )
    self.__log.debug( 'Signature :%s', cand )
    self.__log.debug( 'Threshold :%s', threshold )
    self.__log.debug( 'Pidname   :%s', sel )
    self.__log.debug( 'trackinfo  :%s', trackinfo )
    self.__log.debug( 'noringerinfo :%s', noringerinfo )

  def chain(self):
    return self.__name
  
  def pidname( self ):
    return self.__sel

  def etthr(self):
    return self.__threshold

  def isElectron(self):
    return 'e' in self.__cand

  def isPhoton(self):
    return 'g' in self.__cand

  def noringerinfo(self):
    return self.__noringerinfo

  def trackinfo(self):
    return self.__trackinfo

  def tool(self):
    return self.__tool
  

  def nocut(self):
    
    self.__log.debug( 'Configure nocut' )
    self.tool().AcceptAll      = True
    self.tool().UseRinger      = False
    self.tool().ETthr          = same( self.etthr()*GeV , self.tool())
    self.tool().dETACLUSTERthr = 9999.
    self.tool().dPHICLUSTERthr = 9999.
    self.tool().F1thr          = same( 0.0    , self.tool())
    self.tool().HADETthr       = same( 9999.  , self.tool())
    self.tool().CARCOREthr     = same( -9999. , self.tool())
    self.tool().CAERATIOthr    = same( -9999. , self.tool())


  def etcut(self):

    self.__log.debug( 'Configure etcut' )
    self.tool().UseRinger      = False
    self.tool().ETthr          = same( ( self.etthr()  -  3 )*GeV, self.tool() )
    self.tool().dETACLUSTERthr = 9999.
    self.tool().dPHICLUSTERthr = 9999.
    self.tool().F1thr          = same( 0.0    ,self.tool())
    self.tool().HADETthr       = same( 9999.  ,self.tool())
    self.tool().CARCOREthr     = same( -9999. ,self.tool())
    self.tool().CAERATIOthr    = same( -9999. ,self.tool())


  def noringer(self):

    self.__log.debug( 'Configure noringer' )
    from TrigEgammaHypo.TrigL2CaloHypoCutDefs import L2CaloCutMaps
    self.tool().UseRinger   = False
    self.tool().ETthr       = same( ( self.etthr()  - 3 )*GeV , self.tool())
    self.tool().HADETthr    = L2CaloCutMaps( self.etthr() ).MapsHADETthr[self.pidname()]
    self.tool().CARCOREthr  = L2CaloCutMaps( self.etthr() ).MapsCARCOREthr[self.pidname()]
    self.tool().CAERATIOthr = L2CaloCutMaps( self.etthr() ).MapsCAERATIOthr[self.pidname()]


  def ringer(self):

    self.__log.debug( 'Configure ringer' )
    self.tool().UseRinger = True
    self.tool().EtCut     = (self.etthr()-3.)*GeV  
    
    if self.__useRun3:
      # Set the hypo to use the ONNX API for inference
      self.tool().UseRun3 = True
      self.tool().ConfigFile = self.__getRingerConfiguration()
    else:
      # Set the hypo to use the Run2 ringer selector code for inference
      pconstants, pthresholds = self.__getRingerConfiguration()
      self.tool().UseRun3 = False
      self.tool().ConstantsCalibPath = pconstants
      self.tool().ThresholdsCalibPath = pthresholds



  def compile(self):

    if 'etcut' == self.pidname():
      self.etcut()

    elif self.pidname() in self.__operation_points and 'noringer' in self.noringerinfo() and self.isElectron():
      self.noringer()

    elif self.pidname() in self.__operation_points and 'noringer' not in self.noringerinfo() and self.isElectron():
      self.ringer()

    elif self.pidname() in self.__operation_points and self.isPhoton():
      self.noringer()

    self.addMonitoring()


  #
  # Add monitoring tool
  #
  def addMonitoring(self):

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool,defineHistogram
    
    if (('Validation' in TriggerFlags.enableMonitoring()) or ('Online' in  TriggerFlags.enableMonitoring()) or (athenaCommonFlags.isOnline)):
      if self.tool().UseRinger:
        monTool = GenericMonitoringTool('MonTool'+self.__name)
        monTool.Histograms = [
            defineHistogram( "TIME_total", path='EXPERT',title="Total Time;time[us]",xbins=50, xmin=0,xmax=100,type='TH1F'),
            defineHistogram( "TIME_preproc", path='EXPERT',title="Total Time;time[us]",xbins=50, xmin=0,xmax=5,type='TH1F'),
            defineHistogram( "TIME_propagate",path='EXPERT', title="Total Time;time[us]",xbins=50, xmin=0,xmax=100,type='TH1F'),
            defineHistogram('Eta', type='TH1F', path='EXPERT',title="#eta of Clusters; #eta; number of RoIs", xbins=50,xmin=-2.5,xmax=2.5),
            defineHistogram('Phi',type='TH1F', path='EXPERT',title="#phi of Clusters; #phi; number of RoIs", xbins=64,xmin=-3.2,xmax=3.2),
            defineHistogram('Et',type='TH1F', path='EXPERT',title="E_{T} of Clusters; E_{T} [MeV]; number of RoIs", xbins=60,xmin=0,xmax=5e4),
            defineHistogram('RnnOut',type='TH1F', path='EXPERT',title="Neural Network output; NN output; number of RoIs", xbins=20,xmin=-10,xmax=10),
        ]
    
        monTool.HistPath='L2CaloHypo_Ringer/MonTool'+self.__name
        self.tool().MonTool=monTool

      else:

        monTool = GenericMonitoringTool("MonTool_"+self.__name)
        monTool.defineHistogram('dEta', type='TH1F', path='EXPERT', title="L2Calo Hypo #Delta#eta_{L2 L1}; #Delta#eta_{L2 L1}", 
                                xbins=80, xmin=-0.01, xmax=0.01)
        monTool.defineHistogram('dPhi', type='TH1F', path='EXPERT', title="L2Calo Hypo #Delta#phi_{L2 L1}; #Delta#phi_{L2 L1}", 
                                xbins=80, xmin=-0.01, xmax=0.01)
        monTool.defineHistogram('Et_em', type='TH1F', path='EXPERT', title="L2Calo Hypo cluster E_{T}^{EM};E_{T}^{EM} [MeV]", 
                                xbins=50, xmin=-2000, xmax=100000)
        monTool.defineHistogram('Eta', type='TH1F', path='EXPERT', title="L2Calo Hypo entries per Eta;Eta", xbins=100, xmin=-2.5, xmax=2.5)
        monTool.defineHistogram('Phi', type='TH1F', path='EXPERT', title="L2Calo Hypo entries per Phi;Phi", xbins=128, xmin=-3.2, xmax=3.2)

        cuts=['Input','has one TrigEMCluster', '#Delta #eta L2-L1', '#Delta #phi L2-L1','eta','rCore',
              'eRatio','E_{T}^{EM}', 'E_{T}^{Had}','f_{1}','Weta2','Wstot','F3']

        monTool.defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="L2Calo Hypo Passed Cuts;Cut",
                                xbins=13, xmin=-1.5, xmax=12.5,  opt="kCumulative", xlabels=cuts)

        if 'Validation' in TriggerFlags.enableMonitoring():
            monTool.defineHistogram('Et_had', type='TH1F', path='EXPERT', title="L2Calo Hypo E_{T}^{had} in first layer;E_{T}^{had} [MeV]", 
                xbins=50, xmin=-2000, xmax=100000)
            monTool.defineHistogram('Rcore', type='TH1F', path='EXPERT', title="L2Calo Hypo R_{core};E^{3x3}/E^{3x7} in sampling 2", 
                xbins=48, xmin=-0.1, xmax=1.1)
            monTool.defineHistogram('Eratio', type='TH1F', path='EXPERT', 
                title="L2Calo Hypo E_{ratio};E^{max1}-E^{max2}/E^{max1}+E^{max2} in sampling 1 (excl.crack)", 
                xbins=64, xmin=-0.1, xmax=1.5)
            monTool.defineHistogram('EtaBin', type='TH1I', path='EXPERT', title="L2Calo Hypo entries per Eta bin;Eta bin no.", 
                xbins=11, xmin=-0.5, xmax=10.5)
            monTool.defineHistogram('F1', type='TH1F', path='EXPERT', title="L2Calo Hypo f_{1};f_{1}", xbins=34, xmin=-0.5, xmax=1.2)
            monTool.defineHistogram('Weta2', type='TH1F', path='EXPERT', title="L2Calo Hypo Weta2; E Width in sampling 2", 
                xbins=96, xmin=-0.1, xmax=0.61)
            monTool.defineHistogram('Wstot', type='TH1F', path='EXPERT', title="L2Calo Hypo Wstot; E Width in sampling 1", 
                xbins=48, xmin=-0.1, xmax=11.)
            monTool.defineHistogram('F3', type='TH1F', path='EXPERT', title="L2Calo Hypo F3; E3/(E0+E1+E2+E3)", 
                xbins=96, xmin=-0.1, xmax=1.1)

        monTool.HistPath = 'L2CaloHypo/MonTool'+self.__name
        self.tool().MonTool = monTool


  def __getRingerConfiguration(self):

    possibleSel  = { 'tight':'Tight', 'medium':'Medium', 'loose':'Loose', 'vloose':'VeryLoose',
                         'lhtight':'Tight', 'lhmedium':'Medium', 'lhloose':'Loose', 'lhvloose':'VeryLoose'}

    
    if not self.pidname() in possibleSel.keys():
       raise RuntimeError( "Bad selection name: %s" % self.pidname() )
    
    if self.__useRun3:
      basepath = 'RingerSelectorTools/TrigL2_20210227_r3'
      self.__log.info('Ringer version: %s', basepath)
      return basepath+ '/ElectronRinger{OP}TriggerConfig.conf'.format(OP=possibleSel[self.pidname()] )

    else:
      basepath = 'RingerSelectorTools/TrigL2_20180903_v9'
      self.__log.info('Ringer version: %s', basepath)
      if self.isElectron():
         constant = basepath+'/'+ 'TrigL2CaloRingerElectron{OP}Constants.root'.format(OP=possibleSel[self.pidname()])
         threshold = basepath+'/'+ 'TrigL2CaloRingerElectron{OP}Thresholds.root'.format(OP=possibleSel[self.pidname()])
      elif self.isPhoton():
         constant = basepath+'/'+ 'TrigL2CaloRingerPhoton{OP}Constants.root'.format(OP=possibleSel[self.pidname()])
         threshold = basepath+'/'+ 'TrigL2CaloRingerPhoton{OP}Thresholds.root'.format(OP=possibleSel[self.pidname()])
      else:
         raise RuntimeError( "Bad signature %s" % self.__cand )
      return constant, threshold


def _IncTool(name, cand, threshold, sel, trackinfo, noringerinfo):
  config = TrigEgammaFastCaloHypoToolConfig(name, cand, threshold, sel, trackinfo, noringerinfo )
  config.compile()
  return config.tool()


def TrigEgammaFastCaloHypoToolFromDict( d ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if ((i['signature']=='Electron') or (i['signature']=='Photon'))]

    def __th(cpart):
        return cpart['threshold']

    def __sel(cpart):
        return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']

    def __cand(cpart):
        return cpart['trigType']

    def __trackinfo(cpart):
        return cpart['trkInfo'] if cpart['trkInfo'] else ''

    def __noringer(cpart):    
        return cpart['L2IDAlg'] if cpart['trigType']=='e' else ''

    name = d['chainName']

    return _IncTool( name, __cand( cparts[0]), __th( cparts[0]),  __sel( cparts[0]), __trackinfo(cparts[0]), __noringer(cparts[0]))


def TrigEgammaFastCaloHypoToolFromName( name, conf ):
    """ To be phased out """
    """ set the name of the HypoTool (name=chain) and figure out the threshold and selection from conf """

    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    decodedDict = dictFromChainName(conf)
    return TrigEgammaFastCaloHypoToolFromDict( decodedDict )



if __name__ == "__main__":
    TriggerFlags.enableMonitoring=['Validation']

    t = TrigEgammaFastCaloHypoToolFromName( "HLT_e10_etcut_L1EM3","HLT_e10_etcut_L1EM3" )
    assert t, "cant configure EtCut"

    t = TrigEgammaFastCaloHypoToolFromName( "HLT_e5_etcut_L1EM3","HLT_e5_etcut_L1EM3" )
    assert t, "cant configure EtCut"

    t = TrigEgammaFastCaloHypoToolFromName( "HLT_e26_lhtight_nod0_ivarloose_L1EM22VH","HLT_e26_lhtight_nod0_ivarloose_L1EM22VH" )
    assert t, "cant configure EtCut"


    tool = TrigEgammaFastCaloHypoToolFromName('HLT_e3_etcut1step_g5_etcut_L12EM3', 'HLT_e3_etcut1step_g5_etcut_L12EM3')
    assert tool, 'cant configure HLT_e3_etcut1step_g5_etcut_L12EM3'

