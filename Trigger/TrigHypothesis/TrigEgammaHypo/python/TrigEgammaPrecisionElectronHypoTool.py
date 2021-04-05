#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
 
from AthenaCommon.SystemOfUnits import GeV

def same( val , tool):
  return [val]*( len( tool.EtaBins ) - 1 )

class TrigEgammaPrecisionElectronHypoToolConfig:


  __operation_points  = [  'tight'    ,
                           'medium'   ,
                           'loose'    ,
                           'vloose'   ,
                           'lhtight'  ,
                           'lhmedium' ,
                           'lhloose'  ,
                           'lhvloose' ]

  # isolation cuts
  __isolationCut = {
        None: None,
        'ivarloose': 0.1,
        'ivarmedium': 0.065,
        'ivartight': 0.05
        }

  def __init__(self, name, threshold, sel, iso):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaPrecisionElectronHypoTool')
    self.__name = name
    self.__threshold = float(threshold)
    self.__sel = sel
    self.__iso = iso
    
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionElectronHypoToolInc
    tool = TrigEgammaPrecisionElectronHypoToolInc( name )

    tool.EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    tool.ETthr          = same( self.__threshold*GeV, tool )
    tool.dETACLUSTERthr = 0.1
    tool.dPHICLUSTERthr = 0.1
    tool.vLoose         = False
    tool.Loose          = False
    tool.Medium         = False
    tool.Tight          = False

    self.__tool = tool    

    self.__log.debug( 'Electron_Chain     :%s', name )
    self.__log.debug( 'Electron_Threshold :%s', threshold )
    self.__log.debug( 'Electron_Pidname   :%s', sel )
    self.__log.debug( 'Electron_iso  :%s', iso )

  def chain(self):
    return self.__name

  def pidname( self ):
    return self.__sel

  def etthr(self):
    return self.__threshold

  def isoInfo(self):
    return self.__iso

  def tool(self):
    return self.__tool
  
  def nocut(self):

    self.__log.debug( 'Configure nocut' )
    #self.tool().AcceptAll = True
    self.tool().ETthr          = same( self.etthr()*GeV, self.tool())
    self.tool().dETACLUSTERthr = 9999.
    self.tool().dPHICLUSTERthr = 9999.

  def etcut(self):

    self.__log.debug( 'Configure etcut' )
    self.tool().ETthr          = same( ( self.etthr() -  3 )*GeV )
    # No other cuts applied
    self.tool().dETACLUSTERthr = 9999.
    self.tool().dPHICLUSTERthr = 9999.

  def isoCut(self):

    self.tool().RelPtConeCut = self.__isolationCut[self.isoInfo()]
    self.__setupLHConfiguration()
 
  def nominal(self):
    self.__setupLHConfiguration()

  def compile(self):

    if 'etcut' == self.pidname():
      self.etcut()

    elif 'nocut' == self.pidname():
      self.nocut()

    elif  self.isoInfo()  and self.isoInfo() != '':
      if self.isoInfo() not in self.__isolationCut.keys():
        self.__log.error('Isolation cut %s not defined!', self.isoInfo())
      self.__log.debug('Configuring Isolation cut %s with value %d',self.isoInfo(),self.__isolationCut[self.isoInfo()])
      self.isoCut()

    else:
      self.nominal()

    self.addMonitoring()

  def __setupLHConfiguration(self):

    possibleSel  = { 'tight':'Tight', 'medium':'Medium', 'loose':'Loose', 'vloose':'vLoose',
                         'lhtight':'Tight', 'lhmedium':'Medium', 'lhloose':'Loose', 'lhvloose':'vLoose'}

    possibleTune  = { 'Tight':False, 'Medium':False, 'Loose':False, 'vLoose':False}
    if not self.pidname() in possibleSel.keys():
       raise RuntimeError( "Bad selection name: %s" % self.pidname() )
    possibleTune[possibleSel[self.pidname()]] = True

    self.tool().vLoose = possibleTune['vLoose']
    self.tool().Loose = possibleTune['Loose']
    self.tool().Medium = possibleTune['Medium']
    self.tool().Tight = possibleTune['Tight']


  def addMonitoring(self):

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
    monTool = GenericMonitoringTool("MonTool_"+self.chain())
    monTool.Histograms = [ defineHistogram('dEta', type='TH1F', path='EXPERT', title="PrecisionElectron Hypo #Delta#eta_{EF L1}; #Delta#eta_{EF L1}", xbins=80, xmin=-0.01, xmax=0.01),
                           defineHistogram('dPhi', type='TH1F', path='EXPERT', title="PrecisionElectron Hypo #Delta#phi_{EF L1}; #Delta#phi_{EF L1}", xbins=80, xmin=-0.01, xmax=0.01),
                           defineHistogram('Et_em', type='TH1F', path='EXPERT', title="PrecisionElectron Hypo cluster E_{T}^{EM};E_{T}^{EM} [MeV]", xbins=50, xmin=-2000, xmax=100000),
                           defineHistogram('Eta', type='TH1F', path='EXPERT', title="PrecisionElectron Hypo entries per Eta;Eta", xbins=100, xmin=-2.5, xmax=2.5),
                           defineHistogram('Phi', type='TH1F', path='EXPERT', title="PrecisionElectron Hypo entries per Phi;Phi", xbins=128, xmin=-3.2, xmax=3.2),
                           defineHistogram('EtaBin', type='TH1I', path='EXPERT', title="PrecisionElectron Hypo entries per Eta bin;Eta bin no.", xbins=11, xmin=-0.5, xmax=10.5),
                           defineHistogram('LikelihoodRatio', type='TH1F', path='EXPERT', title="PrecisionElectron Hypo LH", xbins=100, xmin=-5, xmax=5),
                           defineHistogram('mu', type='TH1F', path='EXPERT', title="Average interaction per crossing", xbins=100, xmin=0, xmax=100)]

    cuts=['Input','#Delta #eta EF-L1', '#Delta #phi EF-L1','eta','E_{T}^{EM}']

    monTool.Histograms += [ defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="PrecisionElectron Hypo Passed Cuts;Cut",
                                            xbins=13, xmin=-1.5, xmax=12.5,  opt="kCumulative", xlabels=cuts) ]

    monTool.HistPath = 'PrecisionElectronHypo/'+self.chain()
    self.tool().MonTool = monTool

def _IncTool( name, threshold, sel, iso ):
    config = TrigEgammaPrecisionElectronHypoToolConfig(name, threshold, sel, iso)
    config.compile()
    return config.tool()

def TrigEgammaPrecisionElectronHypoToolFromDict( d ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if ((i['signature']=='Electron') or (i['signature']=='Electron'))]

    def __mult(cpart):
        return int( cpart['multiplicity'] )

    def __th(cpart):
        return cpart['threshold']
    
    def __sel(cpart):
        return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']

    def __iso(cpart):
        return cpart['isoInfo']

    
    name = d['chainName']
        
    return _IncTool( name, __th( cparts[0]),  __sel( cparts[0] ), __iso ( cparts[0])  )
                   
    
def TrigEgammaPrecisionElectronHypoToolFromName(name, conf):
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    decodedDict = dictFromChainName(conf)
    return  TrigEgammaPrecisionElectronHypoToolFromDict( decodedDict )

