# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
from TriggerMenuPython.TriggerPythonConfig import *
from AthenaCommon.Logging import logging 
from TriggerMenuPython.Lvl1Flags import Lvl1Flags
from TrigDetCalib.TrigDetCalibConfig import *
from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo
from TriggerJobOpts.TriggerFlags import TriggerFlags

log = logging.getLogger('LArCalibMenu')
log.setLevel(logging.DEBUG)

class L1CaloHackSubDetListWriter(TrigSubDetListWriter):
  __slots__ = []
  def __init__(self, name = "L1CaloHackSubDetListWriter"):
    super( L1CaloHackSubDetListWriter, self ).__init__( name )
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig("L1CaloHackSubDetListWriter_Time")
    self.AthenaMonTools = [ time ]

class L1CaloTileHackL2ROBListWriter(L2ROBListWriter):
  __slots__ = []
  def __init__(self, name = "L1CaloTileHackL2ROBListWriter"):
    super( L1CaloTileHackL2ROBListWriter, self ).__init__( name )
    self.MaxRoIsPerEvent = 5
    self.RoIDescriptorLabels = [ "TrigT2CaloEgamma", "initialRoI" ]
    self.extraROBs += [
      7602188, 7602204, 7602220, 7602236, 7602189, 7602205, 7602221, 7602237,
      7471112, 7471144, 7471113, 7471145, 7471114, 7471146, 7471115, 7471147,
      7405574, 7405590, 7405606, 7405622, 7405575, 7405591, 7405607, 7405623,
      7405571, 7405587, 7405603, 7405619, 7405572, 7405588, 7405604, 7405620,
      7405573, 7405589, 7405605, 7405621, 7405568, 7405584, 7405600, 7405616,
      7405569, 7405585, 7405601, 7405617, 7405570, 7405586, 7405602, 7405618,
      ]
  
  def setDefaults(self, handle):
    if hasattr( handle, 'name' ) :
      handle.ROBSelector = TileROBSelector("TileROBSelector_L2")

class CalibChain :
  """ The general (LAr) Calib chain
      
      This takes an existing template chain and adds one more step with calib algos
      Arguments:
        sig_id : sig_id of the new chain
        template_id : sig_id of the template chain
        level : L2 or EF :
           - if L2, the new chain will be an L2 chain, modified from the template one, with no EF part  
           - if EF, the new chain will be an EF chain, modified from the template one, with a deep-copied L2 lower_chain
           in both cases, only one new chain is created
        newCounter : the counter for the new chain
        algos : the (calibration) algos to be executed in the new step
        preAlgos : additional algos to be executed in the previous step 
        newName : the name of the new chain. If None, then use template chain_name + suffix
        suffix : the suffix to be added to the original TE names, and optionally to the original chain name
        streamName : the name of the stream where the new chain will end up (the stream tag is hardcoded to 'calibration')
  """
  
  def __init__(self, sig_id, template_id, level, newCounter, algos, 
               newName = None, newLowerName = None, suffix = '_larcalib', streamName = 'LArCells', preAlgos = []) :
    self.sig_id = sig_id
    self.template_id = template_id
    self.level = level
    self.newCounter = newCounter
    self.algos = algos
    self.newName = newName
    self.newLowerName = newLowerName
    self.suffix = suffix
    self.streamName = streamName
    self.preAlgos = list(preAlgos)
    self.newChain = None
    self.copyL2 = False
    self.templateLevel = self.level
    if self.level == 'NewEF' :
      self.templateLevel = 'L2'
      self.level = 'EF'
    if self.level == 'Both' :
      self.copyL2 = True
      self.level = 'EF'

  def generateMenu(self, triggerPythonConfig) :

    #print 'njpb',  self.templateLevel, self.level, self.template_id
    
    if self.level == 'L2' and not TriggerFlags.doLVL2() : return
    if self.level == 'EF' and not TriggerFlags.doEF() : return

    # Locate the template chain(s) (can be L2, EF or both)
    templateChain = self.findChain(triggerPythonConfig, self.template_id, self.templateLevel)
    if templateChain == None :
      log.error('template chain with sig_id=' + self.template_id + ' is not defined at level ' + self.templateLevel)
      return False
     
    if templateChain.level == 'L2' and self.level == 'EF' :
      #print 'njpb newEF case : ', self.template_id  , templateChain.chain_name
      self.newName = 'EF_' + self.sig_id
      #preTE = self.newName + '_precalib'
      newTE = self.newName
      #preAlgos = [ PESA__dummyAlgo('Dummy_' + self.sig_id) ]
      #preAlgos.extend(self.preAlgos)
      #triggerPythonConfig.addSequence([ templateChain.chain_name ], list(preAlgos), preTE) 
      #triggerPythonConfig.addSequence([ preTE ], list(self.algos), newTE)
      triggerPythonConfig.addSequence([ templateChain.chain_name ], list(self.algos), newTE) 
      self.newChain = HLTChain(self.newName, self.newCounter, templateChain.chain_name, 'EF')
      self.newChain.groups = list(templateChain.groups)
      self.newChain.groups = ['BW:Detector']
      self.newChain.addStreamTag(self.streamName, 'calibration')
      #self.newChain.addHLTSignature([ preTE ])
      self.newChain.addHLTSignature([ newTE ])
      triggerPythonConfig.registerHLTChain(self.sig_id, self.newChain)
      return True

    if self.copyL2 : 
      templateLowerChain = self.findChain(triggerPythonConfig, self.template_id, 'L2')
      if self.copyL2 and templateLowerChain == None :
        log.error('template chain with sig_id=' + self.template_id + ' is not defined at level L2')
        return False

    # Build new names (in case a name wasn't supplied already)
    if self.newName == None : self.newName = templateChain.chain_name + self.suffix
    self.lower_chain_name = templateChain.lower_chain_name

    # In case both levels are needed, perform a deep-copy of the L2 chain (with new name) 
    if self.copyL2 : 
      self.lower_chain_name =  self.newLowerName
      if self.lower_chain_name == None : self.lower_chain_name = templateLowerChain.chain_name + self.suffix
      self.newLowerChain = HLTChain(self.lower_chain_name, self.newCounter, 
                                    templateLowerChain.lower_chain_name, templateLowerChain.level)
      for sig in templateLowerChain.siglist :
        self.newLowerChain.addHLTSignature(list(sig.tes), sig.sigcounter, sig.logic)

      self.newLowerChain.groups = list(templateLowerChain.groups)
      self.newLowerChain.groups += ['BW:Detector']
      self.newLowerChain.addStreamTag(self.streamName, 'physics')

    # Build the new top-level chain (L2 if only L2 requested, EF in case of EF-only or both levels)
    self.newChain = HLTChain(self.newName, self.newCounter, self.lower_chain_name, 
                             templateChain.level, templateChain.prescale, templateChain.pass_through)
    for x in list(templateChain.groups):
      if 'BW' not in x:
        self.newChain.groups.append(x)

    self.newChain.groups += ['BW:Detector']
    self.newChain.addStreamTag(self.streamName, 'calibration')

    # copy all signatures from the template chains, except the last one
    for sig in templateChain.siglist[:-1] :
      self.newChain.addHLTSignature(list(sig.tes), sig.sigcounter, sig.logic)
    
    # Now add an additional step at the end of the chain, modifying also the last step of the template chain
    
    #log.warning('Sig list for ' + templateChain.chain_name)
    #for sig in templateChain.siglist : log.warning(str(sig.tes))
    
    if len(templateChain.siglist) > 0 :
      # Get the last signature of the template chain
      sig = templateChain.siglist[-1]
      if sig.uniqueTEs() > 1 :
        log.warning('Customizing chain with several output TEs (' + str(sig.uniqueTEs()) + ') - is this on purpose ?')
      te1s = []  # TEs for the next-to-last step (last step of template)
      te2s = []  # TEs for the new last step
      # Loop through TEs (should be only 1 normally...)
      for te in sig.tes :
        (newTE1, newTE2) = self.newTEs(triggerPythonConfig, templateChain, te)
        if not newTE1 : return False
        te1s.append(newTE1)
        te2s.append(newTE2)
      #log.warning('TE1s ' + str(te1s))
      #log.warning('TE2s ' + str(te2s))
      self.newChain.addHLTSignature(te1s, sig.sigcounter, sig.logic)
      self.newChain.addHLTSignature(te2s, sig.sigcounter + 1, sig.logic)
      #log.warning('New sig list for ' + self.newChain.chain_name)
      #for sig in self.newChain.siglist : log.warning(str(sig.tes))
    else :
      # template chain has no signatures : we need to pick up the last signature of its lower chain...
      if templateChain.level == 'EF' : 
        lc = self.findChain(triggerPythonConfig, self.template_id, 'L2')
        if lc != None and len(lc.siglist) > 0:
          lastSig = lc.siglist[-1]
          if len(lastSig.tes) > 1 : 
            log.error('Cannot handle unseeded chains whose lower chain has more than 1 TE')
            return False
          teName = lastSig.tes[0]
        else :
          teName = lc.lower_chain_name
      else : # ... or the L1 item. Don know how to navigate item->thresholds, so only for the single-object case
        teName = templateChain.lower_chain_name
      newTE = self.newName
      #log.warning('njpb : adding seq ' + teName + ' ' + str(self.algos) + ' ' + newTE)

      triggerPythonConfig.addSequence([ teName ], list(self.algos), newTE) 
      self.newChain.addHLTSignature([ newTE ])
      #log.warning('njpb : adding sig ' + newTE)

    #log.warning('LArCalib chain : ' + self.sig_id + ' '  + self.newName + ' ' + templateChain.lower_chain_name)
    if self.copyL2 : triggerPythonConfig.registerHLTChain(self.sig_id, self.newLowerChain)
    triggerPythonConfig.registerHLTChain(self.sig_id, self.newChain)

  def findChain(self, triggerPythonConfig, id, level) :
    if not id in triggerPythonConfig.allChains :
      for sig in triggerPythonConfig.allChains : 
        print 'njpb', sig, [ c.chain_name for c in triggerPythonConfig.allChains[sig] ]
      return None
    chains = triggerPythonConfig.allChains[id]
    for chain in chains :
      if chain.level == level :
        return chain
    return None                             


  def newTEs(self, triggerPythonConfig, chain, te) :
    if not te.startswith(chain.level + '_') :
      log.error('TE name ' + te + ' is not in the expected form ' + chain.level + '_xxx')
      return (None, None)
    newTE1 = te + self.suffix + '_precalib'
    newTE2 = te + self.suffix
    seq = None
    alreadyWroteSeq = False
    for s in triggerPythonConfig.theSeqLists :
      if s.output == te :
        seq = s
      if s.output == newTE1 :
        alreadyWriteSeq = True
    if seq == None :
      log.error('Could not find sequence for TE ' + te + ' : original configuration is likely invalid')
      return (None, None)
    if alreadyWroteSeq : return (newTE1, newTE2)

    # HLTSequence is initialized from a list of Configurables (as the one with newTE1->2 below) _or_ a list of names.
    # Internally, the names are what is actually used, so seq.algs is a [ str ... ]. So append strings also. 
    preAlgos = list(seq.algs)
    preAlgos.extend([ alg.getFullName() for alg in self.preAlgos ])
    triggerPythonConfig.addSequence(list(seq.input), preAlgos, newTE1)
    triggerPythonConfig.addSequence(newTE1, list(self.algos), newTE2)
    return (newTE1, newTE2)

 
class LArROBListWriterChain(CalibChain) :
  """ The specific LAr calib chain which adds a LArROBListWriter algorithm"""
  def __init__(self, sig_id, templateName, level, newCounter, newName = None, newLowerName = None, suffix = '_larcalib', etaWidth = 0.1, phiWidth = 0.1) :
    CalibChain.__init__(self, sig_id, templateName, level, newCounter, [], newName, newLowerName, suffix = suffix)
    if level == 'L2' :
      self.algos += [ LArL2ROBListWriter('LArL2ROBListWriter_' + sig_id, addCTPResult = True, addL2Result = True, etaWidth = etaWidth, phiWidth = phiWidth) ]
      #if(self.suffix.count('_rejAll')):
      #  self.algos.rejectAll = True
    else :
      self.algos += [ LArEFROBListWriter('LArEFROBListWriter_' + sig_id, addCTPResult = True, addL2Result = True, addEFResult = True, etaWidth = etaWidth, phiWidth = phiWidth) ]
      #if(self.suffix.count('_rejAll')):
      #  self.algos.rejectAll = True

class LArROBListWriterHECChain(LArROBListWriterChain) :
  """ The specific LAr calib chain which adds a LArROBListWriter algorithm and performs an eta cut to the HEC region"""
  def __init__(self, sig_id, templateName, level, newCounter, newName = None,  newLowerName = None, suffix = '_larcalib') :
    LArROBListWriterChain.__init__(self, sig_id, templateName, level, newCounter, newName, newLowerName, suffix = suffix, etaWidth = 0.2, phiWidth = 0.2)
    self.preAlgos.append( EtaHypo_HEC('EtaHypo_HEC_' + sig_id) )

class LArROBListWriterFCALChain(LArROBListWriterChain) :
  """ The specific LAr calib chain which adds a LArROBListWriter algorithm and performs an eta cut to the FCAL region"""
  def __init__(self, sig_id, templateName, level, newCounter, newName = None,  newLowerName = None, suffix = '_larcalib') :
    LArROBListWriterChain.__init__(self, sig_id, templateName, level, newCounter, newName, newLowerName, suffix = suffix, etaWidth = 0.4, phiWidth = 0.4)
    self.preAlgos.append( EtaHypo_FCAL('EtaHypo_FCAL_' + sig_id) )
        

class LArROBListWriterLvl1Chain :
  """ A special (LAr) Calib chain to perform PEB on a L1 item      
      This takes a Lvl1 lower and creates a one-step LVL2 chain with the PEB algorithm
      Arguments:
        sig_id : sig_id of the new chain
        l1_item : the base l1_item to seed from
        l1_threshold : the Lvl1 threshold to use as inputTE to the algorithm
        newCounter : the counter for the new chain
        newName : the name of the new chain. If None, then use 'L2_' + sig_id
        streamName : the name of the stream where the new chain will end up (the stream tag is hardcoded to 'calibration')
  """
  
  def __init__(self, sig_id, l1_item, l1_threshold, newCounter,
               newName = None, streamName = 'LArCellsEmpty', groups = ['LArCellsEmpty']) :
    self.sig_id = sig_id
    self.l1_item = l1_item
    self.l1_threshold = l1_threshold
    self.newCounter = newCounter
    self.newName = newName
    self.streamName = streamName
    self.groups = groups
    self.algos = [
      LArL2ROBListWriter('LArL2ROBListWriter_' + sig_id, addCTPResult = True, addL2Result = True),
      L1CaloTileHackL2ROBListWriter('L1CaloTileHackL2ROBListWriter_' + sig_id)]
    self.algos1 = [
      CSCSubDetListWriter('CSCSubDetListWriter_' + sig_id)
      ]

  def generateMenu(self, triggerPythonConfig) :    
    if self.newName == None : self.newName = 'L2_' + self.sig_id
    outputTEName = self.newName
    self.newChain = HLTChain(self.newName, self.newCounter, self.l1_item, 'L2', '1', '0')
    self.newChain.addStreamTag(self.streamName, 'calibration')
    self.newChain.groups = ['BW:Detector']
    self.newChain.addHLTSignature([ outputTEName ])
    self.newChain.addHLTSignature([ outputTEName+'_CSC' ])
    triggerPythonConfig.addSequence(self.l1_threshold, self.algos, outputTEName)
    triggerPythonConfig.addSequence(self.l1_threshold, self.algos1, outputTEName+'_CSC')
    triggerPythonConfig.registerHLTChain(self.sig_id, self.newChain)


LArCalibChains = [
 LArROBListWriterChain(		'g3_larcalibV1',	'g3_nocut', 	'EF', '2212'),
 LArROBListWriterChain(		'g3_larcalib',		'g3_loose', 	'EF', '2211'),
 LArROBListWriterChain(		'g5_larcalib',		'g5_loose', 	'EF', '2210'),
 LArROBListWriterChain(		'g10_larcalib',  	'g10_loose',  	'EF', '2213'),
 LArROBListWriterChain(		'g11_larcalib',  	'g11_etcut',  	'EF', '2419'),
 #LArROBListWriterChain(		'g15_larcalib',  	'g15_loose',  	'EF', '2214'),
 LArROBListWriterChain(		'g20_larcalib',		'g20_loose',	'EF', '848'),
 LArROBListWriterChain(		'g20i_larcalib',	'g20i_loose', 	'EF', '2216'),
 LArROBListWriterChain(		'g40_larcalib',	        'g40_loose', 	'EF', '2316'),
 #LArROBListWriterChain(	'g40_larcalib_v1',	'g40_loose_v1', 'EF', '2316'),
 LArROBListWriterChain(		'g50_larcalib',	        'g50_loose', 	'EF', '2416'),
 LArROBListWriterChain(		'g60_larcalib',	        'g60_loose', 	'EF', '2417'),
 LArROBListWriterChain(		'g80_larcalib',	        'g80_loose', 	'EF', '2418'),
 LArROBListWriterChain(		'g25_larcalib',		'g25_tight',	'EF', '2217'),
 LArROBListWriterChain(		'g25i_larcalib',	'g25i_tight', 	'EF', '2218'),
 # chains in Initial Beams, w/o writing a peb
 LArROBListWriterChain(		'g10_larcalib_rejAll',  'g10_loose',  	'EF', '1100', suffix = '_larcalib_rejAll'),
 LArROBListWriterLvl1Chain( 'em3_empty_larcalib', 'L1_EM3_EMPTY',  'EM3',  '2229', streamName = 'LArCellsEmpty'),
 LArROBListWriterLvl1Chain( 'em5_empty_larcalib', 'L1_EM5_EMPTY',  'EM5',  '2300', streamName = 'LArCellsEmpty'),
 LArROBListWriterLvl1Chain( 'em6_empty_larcalib', 'L1_EM6_EMPTY',  'EM6',  '2300', streamName = 'LArCellsEmpty'),
 LArROBListWriterLvl1Chain(  'j5_empty_larcalib', 'L1_J5_EMPTY',   'J5',   '2230', streamName = 'LArCellsEmpty'),
 LArROBListWriterLvl1Chain('tau5_empty_larcalib', 'L1_TAU5_EMPTY', 'HA5', '2231', streamName = 'LArCellsEmpty'),
 LArROBListWriterLvl1Chain('tau8_empty_larcalib', 'L1_TAU8_EMPTY', 'HA8', '2231', streamName = 'LArCellsEmpty'),
 LArROBListWriterLvl1Chain(  'j10_empty_larcalib', 'L1_J10_EMPTY',   'J10',   '2166', streamName = 'LArCellsEmpty'),
 LArROBListWriterLvl1Chain(  'fj0_empty_larcalib', 'L1_FJ0_EMPTY',   'JF0',   '2170', streamName = 'LArCellsEmpty'),
 LArROBListWriterLvl1Chain(  'fj1_empty_larcalib', 'L1_FJ1_EMPTY',   'JF1',   '2165', streamName = 'LArCellsEmpty'),
 LArROBListWriterLvl1Chain(  'fj5_empty_larcalib', 'L1_FJ5_EMPTY',   'JF5',   '2165', streamName = 'LArCellsEmpty'),
 LArROBListWriterLvl1Chain(  'fj10_empty_larcalib', 'L1_FJ10_EMPTY',   'JF10',   '2165', streamName = 'LArCellsEmpty'),
 LArROBListWriterLvl1Chain(  'fj30_empty_larcalib', 'L1_FJ30_EMPTY',   'JF30',   '2165', streamName = 'LArCellsEmpty'),  
]

if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus()\
       or TriggerFlags.triggerMenuSetup() in Lvl1Flags.ALFAv2Menus()):
  LArCalibChains += [
    LArROBListWriterChain( 'g12_larcalib',  	'g12Tvh_loose',	'EF', '969'),
    ]

LArCalibJetChains = [
  LArROBListWriterFCALChain( 'fj75_c4cchad_larcalib',  	'fj80_a4tchad',  	'L2', '2207'),
  LArROBListWriterFCALChain( 'fj140_c4cchad_larcalib', 	'fj145_a4tchad',  	'L2', '2205'),
  LArROBListWriterFCALChain( 'j40_c4cchad_larcalib',  	'j45_a4tchad',  	'L2', '2220'),
  LArROBListWriterFCALChain( 'j165_c4cchad_larcalib',  	'j360_a4tchad',  	'L2', '2221'),  

  ]

LArCalibJetEMScaleChains = [
    # jet thresholds for HEC
    LArROBListWriterHECChain(	'j25_larcalib',  	'j30',  	'L2', '2220'),
    LArROBListWriterHECChain(	'j50_larcalib',  	'j55',  	'L2', '2221'),
    LArROBListWriterHECChain(	'j95_larcalib',  	'j100',  	'L2', '2205'),
    LArROBListWriterHECChain(	'j200_larcalib',  	'j200',  	'L2', '2206'),
    #LArROBListWriterHECChain(	'j20_jetNoCut_larcalib',  	'j20_jetNoCut',  	'Both', '2222'),
    #LArROBListWriterHECChain(	'j50_jetNoCut_larcalib',  	'j50_jetNoCut',  	'Both', '2223'),
    #LArROBListWriterHECChain(	'j95_jetNoCut_larcalib',  	'j95_jetNoCut',  	'Both', '2224'),
    #LArROBListWriterHECChain(	'L1J115_NoAlg_larcalib',  	'L1J115_NoAlg', 	'Both', '2206'),
    #LArROBListWriterHECChain(	'j20_jetNoEF_larcalib',  	'j20_jetNoEF',  	'Both', '2222'),
    #LArROBListWriterHECChain(	'j50_jetNoEF_larcalib',  	'j50_jetNoEF',  	'Both', '2223'),
    #LArROBListWriterHECChain(	'j95_jetNoEF_larcalib',  	'j95_jetNoEF',  	'Both', '2224'),
    # fjet thresholds for FCAL
    LArROBListWriterFCALChain(	'fj25_larcalib',  	'fj30',  	'L2', '2207'),
    LArROBListWriterFCALChain(	'fj50_larcalib',  	'fj55',  	'L2', '2208'),
    #HI
    LArROBListWriterHECChain(	'j15_larcalib_hi',  	'j15_Ecorr',  	'L2', '2302'),
    LArROBListWriterHECChain(	'j20_larcalib_hi',  	'j20_Ecorr',  	'L2', '2303'),
    LArROBListWriterFCALChain(  'fj20_larcalib_hi',     'fj20_Ecorr',   'L2', '2301'),
    #LArROBListWriterHECChain(	'L1FJ95_NoAlg_larcalib',  	'L1FJ95_NoAlg',  	'Both', '2209'),
    #LArROBListWriterHECChain(	'fj30_jetNoCut_larcalib',  	'fj30_jetNoCut',  	'Both', '2225'),
    #LArROBListWriterHECChain(	'fj50_jetNoCut_larcalib',  	'fj50_jetNoCut',  	'Both', '2226'),
    #LArROBListWriterHECChain(	'fj30_jetNoEF_larcalib',  	'fj30_jetNoEF',  	'Both', '2225'),
    #LArROBListWriterHECChain(	'fj50_jetNoEF_larcalib',  	'fj50_jetNoEF',  	'Both', '2226'),
    #introduce until NoAlg chains do not give errors anymore
    #LArROBListWriterHECChain(	'j115_jetNoCut_larcalib',  	'j115_jetNoCut',  	'Both', '2228'),
    #LArROBListWriterHECChain(	'fj95_jetNoCut_larcalib',  	'fj95_jetNoCut',  	'Both', '2227'),
    ]
LArCalibJetHIChains =[
  LArROBListWriterHECChain(     'j25_larcalib',         'j30_a4tcem_L1J15',   'L2', '2220'),
  LArROBListWriterHECChain(     'j50_larcalib',         'j50_a4tcem_L1J30',   'L2', '2221'),
  LArROBListWriterHECChain(     'j95_larcalib',         'j95_a4tcem_L1J75',  'L2', '2205'),
  LArROBListWriterHECChain(     'j200_larcalib',        'j200_a4tcem_L1J175', 'L2', '2206'),
  LArROBListWriterHECChain(	'j15_larcalib_hi',  	'j15_Ecorr',  	'L2', '2302'),
  LArROBListWriterHECChain(	'j20_larcalib_hi',  	'j20_Ecorr',  	'L2', '2303'),

  LArROBListWriterFCALChain( 'fj30_larcalib',  	'fj35_a4tcem',  	'L2', '2208'),


  ]
