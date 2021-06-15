# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV


#
# photon hypo alg
#
def createTrigEgammaPrecisionPhotonHypoAlg(name, sequenceOut):

  from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import createTrigEgammaPrecisionPhotonSelectors
  from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionPhotonHypoAlg
  thePrecisionPhotonHypo = TrigEgammaPrecisionPhotonHypoAlg(name)
  thePrecisionPhotonHypo.IsEMNames = ['tight','medium','loose']
  thePrecisionPhotonHypo.PhotonIsEMSelectorTools = createTrigEgammaPrecisionPhotonSelectors()
  thePrecisionPhotonHypo.Photons = sequenceOut
  return thePrecisionPhotonHypo


def same( val , tool):
  return [val]*( len( tool.EtaBins ) - 1 )
  

#
# For photons
#
class TrigEgammaPrecisionPhotonHypoToolConfig:

  __operation_points  = [  'tight'    , 
                           'medium'   , 
                           'loose'    , 
                           ]

  __caloIsolationCut = {
                          None          : None,
                          'icaloloose'  : 0.1,
                          'icalomedium' : 0.075,
                          'icalotight'  : 0.
                        }

  def __init__(self, name, threshold, sel, isoinfo):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaPrecisionPhotonHypoTool')
    self.__name       = name
    self.__threshold  = float(threshold) 
    self.__sel        = sel
    self.__isoinfo    = isoinfo

    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionPhotonHypoTool    
    tool = TrigEgammaPrecisionPhotonHypoTool( name ) 
    tool.EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    tool.ETthr          = same( float(threshold) , tool)
    tool.dETACLUSTERthr = 0.1
    tool.dPHICLUSTERthr = 0.1
    tool.PidName        = ""

    self.__tool = tool
    self.__log.debug( 'Chain     :%s', name )
    self.__log.debug( 'Threshold :%s', threshold )
    self.__log.debug( 'Pidname   :%s', sel )
    self.__log.debug( 'isoinfo   :%s', isoinfo )


  def chain(self):
    return self.__name
  
  def pidname( self ):
    return self.__sel

  def etthr(self):
    return self.__threshold

  def isoInfo(self):
    return self.__isoinfo

  def tool(self):
    return self.__tool
  

  def etcut(self):
    self.__log.debug( 'Configure etcut' )
    self.tool().ETthr          = same( ( self.etthr() -  3 )*GeV, self.tool() )
    # No other cuts applied
    self.tool().dETACLUSTERthr = 9999.
    self.tool().dPHICLUSTERthr = 9999.


  #
  # Isolation and nominal cut
  #
  def isoCut(self):
    self.tool().RelTopoEtConeCut = self.__caloIsolationCut[self.isoInfo()]
    self.nominal()
 

  def nominal(self):
    if not self.pidname() in self.__operation_points:
      self.__log.fatal("Bad selection name: %s" % self.pidname())
    self.tool().PidName = self.pidname()


  #
  # Compile the chain
  #
  def compile(self):

    if 'etcut' == self.pidname():
      self.etcut()

    elif self.isoInfo() and self.isoInfo() != '':

      if self.isoInfo() not in self.__caloIsolationCut.keys():
        self.__log.error('Isolation cut %s not defined!', self.isoInfo())
      self.__log.debug('Configuring Isolation cut %s with value %d',self.isoInfo(),self.__caloIsolationCut[self.isoInfo()])
      self.isoCut()

    else:
      self.nominal()

    self.addMonitoring()


  #
  # Monitoring code
  #
  def addMonitoring(self):
    
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
    monTool = GenericMonitoringTool("MonTool_"+self.__name)
    monTool.Histograms = [ defineHistogram('dEta', type='TH1F', path='EXPERT', title="PrecisionPhoton Hypo #Delta#eta_{EF L1}; #Delta#eta_{EF L1}", xbins=80, xmin=-0.01, xmax=0.01),
                           defineHistogram('dPhi', type='TH1F', path='EXPERT', title="PrecisionPhoton Hypo #Delta#phi_{EF L1}; #Delta#phi_{EF L1}", xbins=80, xmin=-0.01, xmax=0.01),
                           defineHistogram('Et_em', type='TH1F', path='EXPERT', title="PrecisionPhoton Hypo cluster E_{T}^{EM};E_{T}^{EM} [MeV]", xbins=50, xmin=-2000, xmax=100000),
                           defineHistogram('Eta', type='TH1F', path='EXPERT', title="PrecisionPhoton Hypo entries per Eta;Eta", xbins=100, xmin=-2.5, xmax=2.5),
                           defineHistogram('Phi', type='TH1F', path='EXPERT', title="PrecisionPhoton Hypo entries per Phi;Phi", xbins=128, xmin=-3.2, xmax=3.2),
                           defineHistogram('EtaBin', type='TH1I', path='EXPERT', title="PrecisionPhoton Hypo entries per Eta bin;Eta bin no.", xbins=11, xmin=-0.5, xmax=10.5)]

    cuts=['Input','#Delta #eta EF-L1', '#Delta #phi EF-L1','eta','E_{T}^{EM}']

    monTool.Histograms += [ defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="PrecisionPhoton Hypo Passed Cuts;Cut",
                                            xbins=13, xmin=-1.5, xmax=12.5,  opt="kCumulative", xlabels=cuts) ]

    monTool.HistPath = 'PrecisionPhotonHypo/'+self.__name
    self.tool().MonTool = monTool



def _IncTool( name, threshold, sel, iso ):
    config = TrigEgammaPrecisionPhotonHypoToolConfig(name, threshold, sel, iso)
    config.compile()
    return config.tool()

 

def TrigEgammaPrecisionPhotonHypoToolFromDict( d ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if ((i['signature']=='Electron') or (i['signature']=='Photon'))]

    def __th(cpart):
        return cpart['threshold']
    
    def __sel(cpart):
        return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']
   
    def __iso(cpart):
        return cpart['isoInfo']

    
    name = d['chainName']
        
    return _IncTool( name, __th( cparts[0]),  __sel( cparts[0] ), __iso( cparts[0])  )
                   
    
