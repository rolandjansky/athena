# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#by Dan Mori
#config file for CreateMuonValidationSlides.py
#all strings here are for LaTeX

from collections import OrderedDict

texfile = 'muon_validation.tex' #The output pdf file name will be the same

#dataset info
RefVersion = 'ttbar'
TestVersion = 'Zprime'
RefDataset = 'r6059'
TestDataset = 'r6059'
RefDescription = ''
TestDescription = ''

#titlepage info
Title = 'Muon Validation of {0}'.format( TestVersion, RefVersion )
Subtitle = ''
Author = 'Me'
ShortAuthor = 'Me'
Institute = 'CERN'
ShortInstitute = 'CERN'
Date = r'\today'

ShortTitle = r'\textcolor{red}{Test: %s} \textcolor{black}{Ref: %s}'%( TestVersion, RefVersion )  #this will be in the footline of each slide - shows versions of test/ref in red/black

#intro slide
Intro = ''#'Everything looks good'

#summary slide
Summary = []
#Summary = [
#    'item 1',
#    'item 2',
#    'item 3' ]

#############################################################################
#list of plots to go into each page. the dict key is the title for each page
#each plot should be entered as path/to/hist/histname
#The pages will appear in the order given here
#macro is setup to handle 2 or 3 plots per page only

PlotPages = OrderedDict()
PlotComments = {}

#kinematics
PlotPages[ r'Kinematics - $p_T$, $\eta$, $\phi$' ] = [ 
    'Muons/All/reco/AllMuons/kinematics/Muons_All_reco_AllMuons_kinematics_pt',
    'Muons/All/reco/AllMuons/kinematics/Muons_All_reco_AllMuons_kinematics_eta',
    'Muons/All/reco/AllMuons/kinematics/Muons_All_reco_AllMuons_kinematics_phi' ]
#PlotComments[ r'Kinematics - $p_T$, $\eta$, $\phi$' ] = 'PUT COMMENTS HERE IF YOU WANT'

PlotPages[ r'Kinematics - $d_0$, $z_0$' ] = [
    'Muons/All/reco/AllMuons/parameters/Muons_All_reco_AllMuons_parameters_d0',
    'Muons/All/reco/AllMuons/parameters/Muons_All_reco_AllMuons_parameters_z0' ]
#PlotComments[ r'Kinematics - $d_0$, $z_0$' ] = 'PUT COMMENTS HERE IF YOU WANT'

#layers
PlotPages[ 'Precision Layers' ] = [
    'Muons/All/reco/AllMuons/hits/Muons_All_reco_AllMuons_hits_nprecLayers',
    'Muons/All/reco/AllMuons/hits/Muons_All_reco_AllMuons_hits_nphiLayers' ]
#PlotComments[ 'Precision Layers' ] = 'PUT COMMENTS HERE IF YOU WANT'

#hits
PlotPages[ 'Precision Hits - Inner' ] = [
    'Muons/All/reco/AllMuons/hits/Muons_All_reco_AllMuons_hits_innersmallhits',
    'Muons/All/reco/AllMuons/hits/Muons_All_reco_AllMuons_hits_innerlargehits' ]
#PlotComments[ 'Precision Hits - Inner' ] = 'PUT COMMENTS HERE IF YOU WANT'

PlotPages[ 'Precision Hits - Middle' ] = [
    'Muons/All/reco/AllMuons/hits/Muons_All_reco_AllMuons_hits_middlesmallhits',
    'Muons/All/reco/AllMuons/hits/Muons_All_reco_AllMuons_hits_middlelargehits' ]
#PlotComments[ 'Precision Hits - Middle' ] = 'PUT COMMENTS HERE IF YOU WANT'

PlotPages[ 'Precision Hits - Outer' ] = [
    'Muons/All/reco/AllMuons/hits/Muons_All_reco_AllMuons_hits_outersmallhits',
    'Muons/All/reco/AllMuons/hits/Muons_All_reco_AllMuons_hits_outerlargehits' ]
#PlotComments[ 'Precision Hits - Outer' ] = 'PUT COMMENTS HERE IF YOU WANT'

#isolation
PlotPages[ r'Isolation - $p_T$-cone / $p_T$' ] = [
    'Muons/All/reco/AllMuons/isolation/Muons_All_reco_AllMuons_isolation_ptcone20rel',
    'Muons/All/reco/AllMuons/isolation/Muons_All_reco_AllMuons_isolation_ptcone30rel',
    'Muons/All/reco/AllMuons/isolation/Muons_All_reco_AllMuons_isolation_ptcone40rel' ]
#PlotComments[ r'Isolation - $p_T$-cone / $p_T$' ] = 'PUT COMMENTS HERE IF YOU WANT'

PlotPages[ r'Isolation - $E_T$-cone / $p_T$' ] = [
    'Muons/All/reco/AllMuons/isolation/Muons_All_reco_AllMuons_isolation_etcone20rel',
    'Muons/All/reco/AllMuons/isolation/Muons_All_reco_AllMuons_isolation_etcone30rel',
    'Muons/All/reco/AllMuons/isolation/Muons_All_reco_AllMuons_isolation_etcone40rel' ]
#PlotComments[ r'Isolation - $E_T$-cone / $p_T$' ] = 'PUT COMMENTS HERE IF YOU WANT'

#chi^2
PlotPages[ 'Track Fit Quality' ] = [
    'Muons/All/reco/AllMuons/parameters/Muons_All_reco_AllMuons_parameters_tchi2',
    'Muons/All/reco/AllMuons/parameters/Muons_All_reco_AllMuons_parameters_chi2ndof' ]
#PlotComments[ 'Track Fit Quality' ] = 'PUT COMMENTS HERE IF YOU WANT'

#Resolution
PlotPages[ 'Resolution (Combined)' ] = [
   'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_Res_PtResol_vs_pT',
   'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_Res_PtResol_vs_eta',
   'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_Res_PtResol_vs_phi' ]
#PlotComments[ 'Resolution (Combined)' ] = 'PUT COMMENTS HERE IF YOU WANT'

# PlotPages[ 'Resolution (ID)' ] = [
#    'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_ResID_PtResol_vs_pT',
#    'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_ResID_PtResol_vs_eta',
#    'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_ResID_PtResol_vs_phi' ]
# #PlotComments[ 'Resolution (ID)' ] = 'PUT COMMENTS HERE IF YOU WANT'

# PlotPages[ 'Resolution (MS)' ] = [
#    'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_ResMS_PtResol_vs_pT',
#    'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_ResMS_PtResol_vs_eta',
#    'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_ResMS_PtResol_vs_phi' ]
# #PlotComments[ 'Resolution (MS)' ] = 'PUT COMMENTS HERE IF YOU WANT'

#Momentum scale
PlotPages[ r'$p_T$ Scale (Combined)' ] = [
   'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_Res_PtScale_vs_pT',
   'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_Res_PtScale_vs_eta',
   'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_Res_PtScale_vs_phi' ]
#PlotComments[ r'$p_T$ Scale (Combined)' ] = 'PUT COMMENTS HERE IF YOU WANT'

# PlotPages[ r'$p_T$ Scale (ID)' ] = [
#    'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_ResID_PtScale_vs_pT',
#    'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_ResID_PtScale_vs_eta',
#    'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_ResID_PtScale_vs_phi' ]
# #PlotComments[ r'$p_T$ Scale (ID)' ] = 'PUT COMMENTS HERE IF YOU WANT'

# PlotPages[ r'$p_T$ Scale (MS)' ] = [
#    'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_ResMS_PtScale_vs_pT',
#    'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_ResMS_PtScale_vs_eta',
#    'Muons/Prompt/matched/MuidCo/resolution/Muons_Prompt_matched_MuidCo_resolution_ResMS_PtScale_vs_phi' ]
# #PlotComments[ r'$p_T$ Scale (MS)' ] = 'PUT COMMENTS HERE IF YOU WANT'

#Pulls
PlotPages[ r'Track Pull Plots - $q/P$, $\theta$, $\phi$' ] = [
    'Muons/All/matched/AllMuons/Pulls/Muons_All_matched_AllMuons_Pulls_Pull_qOverP',
    'Muons/All/matched/AllMuons/Pulls/Muons_All_matched_AllMuons_Pulls_Pull_theta',
    'Muons/All/matched/AllMuons/Pulls/Muons_All_matched_AllMuons_Pulls_Pull_phi' ]
#PlotComments[ r'Track Pull Plots - $q/P$, $\theta$, $\phi$' ] = 'PUT COMMENTS HERE IF YOU WANT'

PlotPages[ r'Track Pull Plots - $d_0$, $z_0$' ] = [
    'Muons/All/matched/AllMuons/Pulls/Muons_All_matched_AllMuons_Pulls_Pull_d0',
    'Muons/All/matched/AllMuons/Pulls/Muons_All_matched_AllMuons_Pulls_Pull_z0' ]
#PlotComments[ r'Track Pull Plots - $d_0$, $z_0$' ] = 'PUT COMMENTS HERE IF YOU WANT'

# #Efficiency
PlotPages[ 'Efficiency' ] = [
    'Muons/All/matched/AllMuons/efficiency/Muons_All_matched_AllMuons_efficiency_pt',
    'Muons/All/matched/AllMuons/efficiency/Muons_All_matched_AllMuons_efficiency_eta',
    'Muons/All/matched/AllMuons/efficiency/Muons_All_matched_AllMuons_efficiency_phi' ]
#PlotComments[ 'Efficiency' ] = 'PUT COMMENTS HERE IF YOU WANT'

#Purity
PlotPages[ 'Purity' ] = [
    'Muons/All/matched/AllMuons/purity/Muons_All_matched_AllMuons_purity_pt',
    'Muons/All/matched/AllMuons/purity/Muons_All_matched_AllMuons_purity_eta',
    'Muons/All/matched/AllMuons/purity/Muons_All_matched_AllMuons_purity_phi' ]
#PlotComments[ 'Purity' ] = 'PUT COMMENTS HERE IF YOU WANT'

# #Reco fraction
# PlotPages[ 'Reco Fraction - Inflight Muons' ] = [
#     'Muons/InFlight/reco/AllMuons/RecoFraction/Muons_InFlight_reco_AllMuons_RecoFraction_pt',
#     'Muons/InFlight/reco/AllMuons/RecoFraction/Muons_InFlight_reco_AllMuons_RecoFraction_eta',
#     'Muons/InFlight/reco/AllMuons/RecoFraction/Muons_InFlight_reco_AllMuons_RecoFraction_phi' ]
# #PlotComments[ 'Reco Fraction - Inflight Muons' ] = 'PUT COMMENTS HERE IF YOU WANT'

# PlotPages[ 'Reco Fraction - NonIsolated Muons' ] = [
#     'Muons/NonIsolated/reco/AllMuons/RecoFraction/Muons_NonIsolated_reco_AllMuons_RecoFraction_pt',
#     'Muons/NonIsolated/reco/AllMuons/RecoFraction/Muons_NonIsolated_reco_AllMuons_RecoFraction_eta',
#     'Muons/NonIsolated/reco/AllMuons/RecoFraction/Muons_NonIsolated_reco_AllMuons_RecoFraction_phi' ]
# # PlotComments[ 'Reco Fraction - NonIsolated Muons' ] = 'PUT COMMENTS HERE IF YOU WANT'

PlotPages[ 'Reco Fraction - Unmatched All Authors' ] = [
    'Muons/UnmatchedRecoMuons/RecoFraction/Muons_UnmatchedRecoMuons__RecoFraction_pt',
    'Muons/UnmatchedRecoMuons/RecoFraction/Muons_UnmatchedRecoMuons__RecoFraction_eta',
    'Muons/UnmatchedRecoMuons/RecoFraction/Muons_UnmatchedRecoMuons__RecoFraction_phi' ]
#PlotComments[ 'Reco Fraction - Unmatched All Authors' ] = 'PUT COMMENTS HERE IF YOU WANT'
