###
# Copyright (C) 2021 CERN for the benefit of the ATLAS collaboration
#
# stand-alone script to rename histograms in FTag NTUP_PHYSVAL files produced before ~ April 2021 to the new scheme
# author: judith.hoefer@cern.ch
# Mar 2021
###

from __future__ import print_function
import os
import ROOT

# run this on the unmerged files

# input_file_name = 'NTUP_PHYSVAL_compare_to_standard.root'
# input_file_name = '/afs/cern.ch/user/j/juhofer/WORK/validation/data/run_standard/NTUP_PHYSVAL_standard.root'
input_file_name = '/afs/cern.ch/user/j/juhofer/WORK/validation/data/QSP_on/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.NTUP_PHYSVAL.e4993_s3641_r12287_p4360_p3821_tid24086162_00/NTUP_PHYSVAL.24086162._000001.pool.root.1'


production_process = 'ttbar'

verbose = False

jet_collections = [ 'AntiKt2PV0TrackJets',
                    'AntiKt4EMPFlowJets',
                    'AntiKt4EMTopoJets',
                    'AntiKt4PV0TrackJets',
                    'AntiKtVR30Rmax4Rmin02TrackJets']

## don't change here

if production_process not in ['ttbar', 'Zprime']:
  print("Typo in the given production process: ", production_process, ". Exiting.")
  exit()

var_name_list = [
'IP3D_b_matched_weight ',
'IP3D_b_matched_weight_trackCuts ',
'IP3D_b_50_matched_pt ',
'IP3D_b_70_matched_pt ',
'IP3D_b_80_matched_pt ',
'IP3D_c_matched_weight ',
'IP3D_c_matched_weight_trackCuts ',
'IP3D_c_50_matched_pt ',
'IP3D_c_70_matched_pt ',
'IP3D_c_80_matched_pt ',
'IP3D_tau_matched_weight ',
'IP3D_tau_matched_weight_trackCuts ',
'IP3D_tau_50_matched_pt ',
'IP3D_tau_70_matched_pt ',
'IP3D_tau_80_matched_pt ',
'IP3D_u_matched_weight ',
'IP3D_u_matched_weight_trackCuts ',
'IP3D_u_50_matched_pt ',
'IP3D_u_70_matched_pt ',
'IP3D_u_80_matched_pt ',
'IP2D_b_matched_weight ',
'IP2D_b_matched_weight_trackCuts ',
'IP2D_b_50_matched_pt ',
'IP2D_b_70_matched_pt ',
'IP2D_b_80_matched_pt ',
'IP2D_c_matched_weight ',
'IP2D_c_matched_weight_trackCuts ',
'IP2D_c_50_matched_pt ',
'IP2D_c_70_matched_pt ',
'IP2D_c_80_matched_pt ',
'IP2D_tau_matched_weight ',
'IP2D_tau_matched_weight_trackCuts ',
'IP2D_tau_50_matched_pt ',
'IP2D_tau_70_matched_pt ',
'IP2D_tau_80_matched_pt ',
'IP2D_u_matched_weight ',
'IP2D_u_matched_weight_trackCuts ',
'IP2D_u_50_matched_pt ',
'IP2D_u_70_matched_pt ',
'IP2D_u_80_matched_pt ',
'SV1_b_matched_weight ',
'SV1_b_matched_weight_trackCuts ',
'SV1_b_40_matched_pt ',
'SV1_b_50_matched_pt ',
'SV1_b_60_matched_pt ',
'SV1_c_matched_weight ',
'SV1_c_matched_weight_trackCuts ',
'SV1_c_40_matched_pt ',
'SV1_c_50_matched_pt ',
'SV1_c_60_matched_pt ',
'SV1_tau_matched_weight ',
'SV1_tau_matched_weight_trackCuts ',
'SV1_tau_40_matched_pt ',
'SV1_tau_50_matched_pt ',
'SV1_tau_60_matched_pt ',
'SV1_u_matched_weight ',
'SV1_u_matched_weight_trackCuts ',
'SV1_u_40_matched_pt ',
'SV1_u_50_matched_pt ',
'SV1_u_60_matched_pt ',
'IP3DSV1_b_matched_weight ',
'IP3DSV1_b_matched_weight_trackCuts ',
'IP3DSV1_b_50_matched_pt ',
'IP3DSV1_b_70_matched_pt ',
'IP3DSV1_b_80_matched_pt ',
'IP3DSV1_c_matched_weight ',
'IP3DSV1_c_matched_weight_trackCuts ',
'IP3DSV1_c_50_matched_pt ',
'IP3DSV1_c_70_matched_pt ',
'IP3DSV1_c_80_matched_pt ',
'IP3DSV1_tau_matched_weight ',
'IP3DSV1_tau_matched_weight_trackCuts ',
'IP3DSV1_tau_50_matched_pt ',
'IP3DSV1_tau_70_matched_pt ',
'IP3DSV1_tau_80_matched_pt ',
'IP3DSV1_u_matched_weight ',
'IP3DSV1_u_matched_weight_trackCuts ',
'IP3DSV1_u_50_matched_pt ',
'IP3DSV1_u_70_matched_pt ',
'IP3DSV1_u_80_matched_pt ',
'JetFitter_b_matched_weight ',
'JetFitter_b_matched_weight_trackCuts ',
'JetFitter_b_50_matched_pt ',
'JetFitter_b_70_matched_pt ',
'JetFitter_b_80_matched_pt ',
'JetFitter_c_matched_weight ',
'JetFitter_c_matched_weight_trackCuts ',
'JetFitter_c_50_matched_pt ',
'JetFitter_c_70_matched_pt ',
'JetFitter_c_80_matched_pt ',
'JetFitter_tau_matched_weight ',
'JetFitter_tau_matched_weight_trackCuts ',
'JetFitter_tau_50_matched_pt ',
'JetFitter_tau_70_matched_pt ',
'JetFitter_tau_80_matched_pt ',
'JetFitter_u_matched_weight ',
'JetFitter_u_matched_weight_trackCuts ',
'JetFitter_u_50_matched_pt ',
'JetFitter_u_70_matched_pt ',
'JetFitter_u_80_matched_pt ',
'MV2c10_b_matched_weight ',
'MV2c10_b_matched_weight_trackCuts ',
'MV2c10_c_matched_weight ',
'MV2c10_c_matched_weight_trackCuts ',
'MV2c10_tau_matched_weight ',
'MV2c10_tau_matched_weight_trackCuts ',
'MV2c10_u_matched_weight ',
'MV2c10_u_matched_weight_trackCuts ',
'MV2c10mu_b_matched_weight ',
'MV2c10mu_b_matched_weight_trackCuts ',
'MV2c10mu_c_matched_weight ',
'MV2c10mu_c_matched_weight_trackCuts ',
'MV2c10mu_tau_matched_weight ',
'MV2c10mu_tau_matched_weight_trackCuts ',
'MV2c10mu_u_matched_weight ',
'MV2c10mu_u_matched_weight_trackCuts ',
'MV2c10rnn_b_matched_weight ',
'MV2c10rnn_b_matched_weight_trackCuts ',
'MV2c10rnn_c_matched_weight ',
'MV2c10rnn_c_matched_weight_trackCuts ',
'MV2c10rnn_tau_matched_weight ',
'MV2c10rnn_tau_matched_weight_trackCuts ',
'MV2c10rnn_u_matched_weight ',
'MV2c10rnn_u_matched_weight_trackCuts ',
'truthLabel ',
'TruthBpt ',
'TruthLpt ',
'TruthCpt ',
'TruthTauPt ',
'IP3Dpb ',
'IP3Dpc ',
'IP3Dpu ',
'SV1pb ',
'SV1pc ',
'SV1pu ',
'NTracksIP3D ',
'NTracksIP2D ',
'NGTinSvx1 ',
'NGTinSvx0 ',
'e ',
'pt ',
'eta ',
'phi ',
'd0 ',
'z0 ',
'IP3DgradeOfTracks ',
'IP2DgradeOfTracks ',
'IP3D_valD0wrtPVofTracks ',
'IP3D_valZ0wrtPVofTracks ',
'IP3D_sigD0wrtPVofTracks ',
'IP3D_sigZ0wrtPVofTracks ',
'IP3D_weightBofTracks ',
'IP3D_weightUofTracks ',
'IP2D_weightBofTracks ',
'IP2D_weightUofTracks '
]

def get_name_associations(list):

  taggers = ["_IP3D_", "_SV1_", "_JetFitter_"]
  old_taggers = ["_IP2D_", "_IP3DSV1_", "_MV2c10_", "_MV2c10mu_", "_MV2c10rnn_"]
  oneToOne_associations = {
    '_truthLabel ': '_jet_truthLabel ',
    '_IP3Dpb ': '_tagger_IP3D_pb ',
    '_IP3Dpc ': '_tagger_IP3D_pc ',
    '_IP3Dpu ': '_tagger_IP3D_pu ',
    '_SV1pb ': '_tagger_SV1_pb ',
    '_SV1pc ': '_tagger_SV1_pc ',
    '_SV1pu ': '_tagger_SV1_pu ',
    '_NTracksIP3D ': '_tagger_IP3D_NTracks_incl ',
    '_NTracksIP2D ': '_old_taggers_IP2D_NTracks_incl ',
    '_NGTinSvx1 ': '_tagger_SV1_nGoodTracks_incl ',
    '_eta ': '_jet_jet_eta ',
    '_phi ': '_jet_jet_phi ',
    '_d0 ': '_tracks_d0_b ',
    '_z0 ': '_tracks_z0_b ',
    '_IP3DgradeOfTracks ': '_tagger_IP3D_gradeOfTracks_incl ',
    '_IP2DgradeOfTracks ': '_old_taggers_IP2D_gradeOfTracks_incl ',
  }  
  oneToOne_associations_ttbar = {
    '_TruthBpt ': '_jet_jet_pt_b_ttbar ',
    '_TruthLpt ': '_jet_jet_pt_l_ttbar ',
    '_TruthCpt ': '_jet_jet_pt_c_ttbar ',
    '_e ': '_jet_jet_e ',
    '_pt ': '_jet_jet_pt_ttbar ',
  }
  oneToOne_associations_Zprime = {
    '_TruthBpt ': '_jet_jet_pt_b_Zprime ',
    '_TruthLpt ': '_jet_jet_pt_l_Zprime ',
    '_TruthCpt ': '_jet_jet_pt_c_Zprime ',
    '_e ': '_jet_jet_e_Zprime ',
    '_pt ': '_jet_jet_pt_Zprime ',
  }
  
  name_associations = {}
  
  for name in list:
    new_name = name
  
    for tag in taggers:
      if tag in new_name:
        new_name = new_name.replace(tag, "_tagger" + tag)
    
    for tag in old_taggers:
      if tag in new_name:
        new_name = new_name.replace(tag, "_old_taggers" + tag)
  
    if "_matched_pt" in new_name:
      new_name = new_name.replace("_matched_pt", "_matched_pt_" + production_process)

    if "AntiKt4EMPFlowJets" in new_name:
      new_name = new_name.replace("AntiKt4EMPFlowJets", "AntiKt4EMPFlowJets_")

    if "AntiKtVR30Rmax4Rmin02TrackJets" in new_name:
      new_name = new_name.replace("AntiKtVR30Rmax4Rmin02TrackJets", "AntiKtVR30Rmax4Rmin02TrackJets_")

    for key in oneToOne_associations:
      if key in new_name:
        new_name = new_name.replace(key, oneToOne_associations[key])

    if production_process == 'ttbar':
      for key in oneToOne_associations_ttbar:
        if key in new_name:
          new_name = new_name.replace(key, oneToOne_associations_ttbar[key])

    if production_process == 'Zprime':
      for key in oneToOne_associations_Zprime:
        if key in new_name:
          new_name = new_name.replace(key, oneToOne_associations_Zprime[key])

    # if new_name not in new_list and "tau" not in new_name and "Tau" not in new_name and not "_NGTinSvx0" in new_name:
    #   print(new_name)

    # remove spaces
    name = name.replace(" ", "")
    new_name = new_name.replace(" ", "")
    
    # fill the dict
    name_associations[name] = new_name

  return name_associations
  


# get the name_list for all jet collections
name_list = []
for jet_col in jet_collections:
  for var_name in var_name_list:
    if jet_col == "AntiKt4EMPFlowJets" or jet_col == "AntiKtVR30Rmax4Rmin02TrackJets":
      name_list.append("BTag_" + jet_col + var_name)
    else:
      name_list.append("BTag_" + jet_col + "_" + var_name)

# get the name associations
name_associations = get_name_associations(name_list)
if verbose:
  print(name_associations)

# prepare the output file
if input_file_name.endswith('.root'):
  output_file_name = input_file_name[:-5] + '_renamed.root' 
elif input_file_name.endswith('.pool.root.1'):
  output_file_name = input_file_name[:-12] + '_renamed.pool.root.1' 
else:
  print("Input file ending is not .root and not .pool.root.1, I can't handle the conversion to an output name.")
  exit()

output_file = ROOT.TFile(output_file_name, 'RECREATE')

# make the subdirectories
output_file.mkdir("BTag")
for jet_col in jet_collections:
  path = "BTag/" + jet_col + "/"
  output_file.mkdir(path)

# close the file
output_file.Close()

# loop over all the jet collections
for jet_col in jet_collections:

  # open the input file
  input_file = ROOT.TFile(input_file_name, 'READ')

  if input_file.IsOpen(): #and output_file.IsOpen():

    # go into the directory
    path = "BTag;1/" + jet_col + ";1/"
    ROOT.gDirectory.cd(path)

    if verbose:
      print("\njust cd'ed into: " + path)
      print("\nls")
      input_file.ls()
      
    # get the subdirectories here
    currentdir = ROOT.gDirectory.GetPath()
    dirlist = input_file.Get(currentdir).GetListOfKeys()

    if verbose:
      print("\ndirlist\n", dirlist)

    histograms = []

    # loop over the objects in here 
    for subdir in dirlist:
      obj = subdir.ReadObj()

      # and see if they are histograms and ignore TProfiles
      if obj.IsA().InheritsFrom(ROOT.TH1.Class()) and jet_col in obj.GetName():

        if verbose:
          print("\nobj: ", obj)
          print("'", obj.GetName(), "'," )

        # check if the name is on the list
        if obj.GetName() not in name_associations:
          print("This name is not defined!!!", obj.GetName())
          exit()

        # rename the histo and fill the list with histograms
        new_histo = obj
        new_histo.SetName(name_associations[obj.GetName()])
        histograms.append(new_histo)

    # now open the output file and got to the right dir
    output_file = ROOT.TFile(output_file_name, 'UPDATE')

    if output_file.IsOpen():

      ROOT.gDirectory.cd(path)

      # loop over the histos
      for histo in histograms:
        histo.Write()

      output_file.Close()

    input_file.Close()


print("\n the output was written to:\n  ", output_file_name)