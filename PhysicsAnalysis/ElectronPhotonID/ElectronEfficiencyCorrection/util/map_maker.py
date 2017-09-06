# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys

# --- # --- # --- # --- # --- # --- # --- # --- #

def main():

  # --- # --- # --- # --- #
  # --- Configuration --- #
  # --- # --- # --- # --- #

  BaseDir = 'ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1'
  NamesFile = 'list_of_all_files.txt'
  OutputFile = 'map0.txt'

  # --- # --- # --- # --- #

  if not BaseDir.endswith( '/' ):
    BaseDir += '/' 
  Names = []
  with open( NamesFile ) as File:
    Names = [ Name.strip() for Name in File.readlines() ] 
  OutFile = open( OutputFile, 'w' )
  Lines = []
  for Name in Names:
    TheKey = GetKeyCombo( Name )
    TheKeyName, TheFolder = TheKey.GetInfo()
    Lines.append( '%s=%s%s/%s\n' % ( TheKeyName, BaseDir, TheFolder, Name ) )
  for Line in sorted( Lines, key = lambda v: v.upper() ):
    OutFile.write( Line ) 
  OutFile.close()
  os.system( 'cat ' + OutputFile + ' | sort > temp_file.txt ; mv temp_file.txt ' + OutputFile )


# --- # --- # --- # --- # --- # --- # --- # --- #

class Key:
  
  def __init__ ( self, reco_key = '', id_key = '', iso_key = '', trigger_key = '', charge_mis_id_key = '' ):
    self.RecoKey = reco_key
    self.IdKey = id_key
    self.IsoKey = iso_key
    self.TriggerKey = trigger_key
    self.ChargeMisIdKey = charge_mis_id_key
    self.IsCorrelation = False
    self.IsEffOnly = False

  def GetInfo ( self ):
    ResultName = ''
    ResultFolder = 'offline'
    ###
    if self.RecoKey != '':
      ResultName = self.RecoKey
    ###
    if self.IdKey != '':
      if ResultName == '': 
        ResultName = self.IdKey
      else:
        ResultName += '_' + self.IdKey
    ###
    if self.IsoKey != '':
      if ResultName == '': 
        ResultName = self.IsoKey
      else:
        ResultName += '_' + self.IsoKey
        ResultFolder = 'isolation'
    ###
    if self.TriggerKey != '':
      if ResultName == '': 
        ResultName = self.TriggerKey
      else:
        ResultName = self.TriggerKey + '_' + ResultName
      ResultFolder = 'trigger'
    ###
    if self.IsCorrelation:
      ResultName += '_corrID'
    ###
    if self.IsEffOnly:
      ResultName = 'Eff_' + ResultName
    ###
    return ResultName, ResultFolder

# --- # --- # --- # --- # --- # --- # --- # --- #

def GetKeyCombo( Name ):
  ResKey = Key()
  # Sanity Checks
  if not 'efficiency' in Name:
    print 'Sanity Check n.1 not passed'
    return ResKey 
  if 'efficiencySF' in Name:
    pass
  elif 'efficiency' in Name:
    ResKey.IsEffOnly = True
  # Reco Key Retrieval
  if 'RecoTrk' in Name:
    ResKey.RecoKey = 'Reconstruction'
  # Id Key Retrieval (the order is important to make sure the no d0 items are correctly matched)
  KeyPairs = [ 
    ( 'LooseAndBLayerLLH_d0z0', 'LooseBLayer' ),
    ( 'MediumLLH_d0z0', 'Medium' ),
    ( 'TightLLH_d0z0', 'Tight' ),
    ( 'LooseAndBLayerLLH', 'LooseBLayer_Nod0z0' ),
    ( 'MediumLLH', 'Medium_Nod0z0' ),
    ( 'TightLLH', 'Tight_Nod0z0' ),
  ]
  for SearchPattern, KeyName in KeyPairs:
    if SearchPattern in Name:
      ResKey.IdKey = KeyName
      break
  # Iso Key Retrieval (the order is important) 
  KeyPairs = [ 
    ( 'isolFixedCutLoose', 'FixedCutLoose' ),
    ( 'isolFixedCutTightTrackOnly', 'FixedCutTightTrackOnly' ),
    ( 'isolFixedCutTight', 'FixedCutTight' ),
    ( 'isolGradientLoose', 'GradientLoose' ),
    ( 'isolGradient', 'Gradient' ),
    ( 'isolLooseTrackOnly', 'LooseTrackOnly' ),
    ( 'isolLoose', 'Loose' ),
    ( 'isolTight', 'Tight' )
    ( 'isolFixedCutHighPtCaloOnly', 'FixedCutHighPtCaloOnly' )
    ( 'isolFixedCutTrackCone40', 'FixedCutTrackCone40' )
  ]
  for SearchPattern, KeyName in KeyPairs:
    if SearchPattern in Name:
      ResKey.IsoKey = KeyName
      break
  # Correlation
  if 'correlationID' in Name:
    ResKey.IsCorrelation = True
  # Trigger (Oh my!) 
  # 2015 Only
  KeyPairs2015 = [ 
    ( 'e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose', 'e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose' ),
    ( 'e24_lhmedium_L1EM20VHI', 'e24_lhmedium_L1EM20VHI' ),
    ( 'e12_lhloose_L1EM10VH', 'e12_lhloose_L1EM10VH' ),
    ( 'e15_lhloose_L1EM13VH', 'e15_lhloose_L1EM13VH' ),
    ( 'e17_lhloose_L1EM15', 'e17_lhloose_L1EM15' ),
    ( 'e17_lhloose', 'e17_lhloose' ),
    ( 'e9_lhloose', 'e9_lhloose' ),
    ( 'e7_lhmedium', 'e7_lhmedium' ),
    ( 'e17_lhmedium', 'e17_lhmedium' ),
    ( 'e20_lhmedium', 'e20_lhmedium' ),
    ( 'e24_medium_L1EM20VHI', 'e24_medium_L1EM20VHI' ),
    ( 'e24_lhmedium_L1EM15VH', 'e24_lhmedium_L1EM15VH' ),
    ( 'e26_lhtight_nod0_iloose', 'e26_lhtight_nod0_iloose' ),
  ]
  # 2016 Only
  KeyPairs2016 = [ 
    ( 'e7_lhmedium_nod0', 'e7_lhmedium_nod0' ),
    ( 'e9_lhloose_nod0', 'e9_lhloose_nod0' ),
    ( 'e9_lhmedium_nod0', 'e9_lhmedium_nod0' ),
    ( 'e12_lhloose_nod0', 'e12_lhloose_nod0' ),
    ( 'e15_lhvloose_nod0_L1EM13VH', 'e15_lhvloose_nod0_L1EM13VH' ),
    ( 'e17_lhvloose_nod0', 'e17_lhvloose_nod0' ),
    ( 'e17_lhloose_nod0', 'e17_lhloose_nod0' ),
    ( 'e17_lhloose', 'e17_lhloose' ),
    ( 'e17_lhmedium_nod0_ivarloose_L1EM15HI', 'e17_lhmedium_nod0_ivarloose_L1EM15HI' ),
    ( 'e17_lhmedium_nod0_L1EM15HI', 'e17_lhmedium_nod0_L1EM15HI' ),
    ( 'e17_lhmedium_nod0', 'e17_lhmedium_nod0' ),
    ( 'e20_lhmedium_nod0', 'e20_lhmedium_nod0' ),
    ( 'e24_lhmedium_nod0_L1EM15VH', 'e24_lhmedium_nod0_L1EM15VH' ),
    ( 'e24_lhmedium_nod0_L1EM20VHI', 'e24_lhmedium_nod0_L1EM20VHI' ),
    ( 'e24_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0', 'e24_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0' ),
    ( 'e26_lhmedium_nod0_L1EM22VHI', 'e26_lhmedium_nod0_L1EM22VHI' ),
    ( 'e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0', 'e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0' ),
  ]
  # 2015 / 2016 combinations 
  CommonKeyPairs = [
    ( 'SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e24_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0', 'SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e24_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0' ),
    ( 'SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0', 'SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0' ),
    ( 'DI_E_2015_e12_lhloose_L1EM10VH_2016_e15_lhvloose_nod0_L1EM13VH', 'DI_E_2015_e12_lhloose_L1EM10VH_2016_e15_lhvloose_nod0_L1EM13VH' ),
    ( 'DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0', 'DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0' ),
    ( 'DI_E_2015_e17_lhloose_2016_e17_lhloose', 'DI_E_2015_e17_lhloose_2016_e17_lhloose' ),
    ( 'TRI_E_2015_e17_lhloose_2016_e17_lhloose_nod0', 'TRI_E_2015_e17_lhloose_2016_e17_lhloose_nod0' ),
    ( 'TRI_E_2015_e17_lhloose_2016_e17_lhmedium_nod0', 'TRI_E_2015_e17_lhloose_2016_e17_lhmedium_nod0' ),
    ( 'TRI_E_2015_e9_lhloose_2016_e9_lhloose_nod0', 'TRI_E_2015_e9_lhloose_2016_e9_lhloose_nod0' ),
    ( 'TRI_E_2015_e9_lhloose_2016_e9_lhmedium_nod0', 'TRI_E_2015_e9_lhloose_2016_e9_lhmedium_nod0' ),
    ( 'MULTI_L_2015_e17_lhloose_2016_e17_lhloose_nod0', 'MULTI_L_2015_e17_lhloose_2016_e17_lhloose_nod0' ),
    ( 'MULTI_L_2015_e7_lhmedium_2016_e7_lhmedium_nod0', 'MULTI_L_2015_e7_lhmedium_2016_e7_lhmedium_nod0' ),
    ( 'MULTI_L_2015_e12_lhloose_2016_e12_lhloose_nod0', 'MULTI_L_2015_e12_lhloose_2016_e12_lhloose_nod0' ),
    ( 'MULTI_L_2015_e24_lhmedium_L1EM20VHI_2016_e24_lhmedium_nod0_L1EM20VHI', 'MULTI_L_2015_e24_lhmedium_L1EM20VHI_2016_e24_lhmedium_nod0_L1EM20VHI' ),
    ( 'MULTI_L_2015_e24_lhmedium_L1EM20VHI_2016_e26_lhmedium_nod0_L1EM22VHI', 'MULTI_L_2015_e24_lhmedium_L1EM20VHI_2016_e26_lhmedium_nod0_L1EM22VHI' ),
  ]
  FoundTrigger = False
  # Priority to 2015+2016 Triggers
  for SearchPattern, KeyName in CommonKeyPairs:
    if SearchPattern in Name:
      ResKey.TriggerKey = KeyName
      FoundTrigger = True
      break
  if not FoundTrigger:
    # Moving on to 2015 Triggers
    for SearchPattern, KeyName in KeyPairs2015:
      if SearchPattern in Name and '2015' in Name:
        ResKey.TriggerKey = '2015_' + KeyName
        break
    # Moving on to 2016 Triggers
    for SearchPattern, KeyName in KeyPairs2016:
      if SearchPattern in Name and '2016' in Name:
        ResKey.TriggerKey = '2016_' + KeyName
        break

  return ResKey

# --- # --- # --- # --- # --- # --- # --- # --- #

if __name__ == '__main__':
  sys.exit( main() )
