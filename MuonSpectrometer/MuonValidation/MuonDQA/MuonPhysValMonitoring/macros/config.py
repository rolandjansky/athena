# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Dan Mori
#config file for CreateMuonValidationSlides.py

from collections import OrderedDict

texfile = 'muon_validation_run2_pileup_vs_nopileup_14TeV.tex' #The output pdf file name will be the same

#dataset info
RefVersion = '19.1.1.4 no pileup'
TestVersion = '19.1.1.4 pileup'
RefDataset = r'valid1.*Zmumu.recon.AOD.e2658\_s1967\_s1964\_r5749/'
TestDataset = r'valid1.*Zmumu.recon.AOD.e2658\_s1967\_s1964\_r5746/'
RefDescription = 'run 2, no pileup, 14 TeV'
TestDescription = 'run 2, pileup, 14 TeV'

#titlepage info
Title = 'Muon Validation of {0}'.format( TestVersion, RefVersion )
Subtitle = 'Pileup vs No Pileup, Run 2 Config (14 TeV)'
Author = 'Dan Mori'
ShortAuthor = 'Mori'
Institute = 'Simon Fraser University'
ShortInstitute = 'SFU'
Date = '10 Sept 2014'

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
#list of plots to go into each page. the key is the title for each page
#each plot should be entered as path/to/hist/histname
#The pages will appear in the order given here

PlotPages = OrderedDict()
PlotComments = {}

#kinematics
PlotPages[ 'Kinematics - pt, eta, phi' ] = [ 
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_pt',
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_eta',
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_phi' ]
#PlotComments[ 'Kinematics - pt, eta, phi' ] = 'PUT COMMENTS HERE IF YOU WANT'

PlotPages[ 'Kinematics - d0, z0' ] = [
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_d0',
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_z0' ]
#PlotComments[ 'Kinematics - d0 (left), z0 (right)' ] = 'PUT COMMENTS HERE IF YOU WANT'

#layers
PlotPages[ 'Precision Layers' ] = [
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_nprecLayers',
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_nphiLayers' ]
#PlotComments[ 'Precision Layers' ] = 'PUT COMMENTS HERE IF YOU WANT'

#hits
PlotPages[ 'Precision Hits - Inner' ] = [
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_innersmallhits',
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_innerlargehits' ]
#PlotComments[ 'Precision Hits - Inner' ] = 'PUT COMMENTS HERE IF YOU WANT'

PlotPages[ 'Precision Hits - Middle' ] = [
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_middlesmallhits',
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_middlelargehits' ]
#PlotComments[ 'Precision Hits - Middle' ] = 'PUT COMMENTS HERE IF YOU WANT'

PlotPages[ 'Precision Hits - Outer' ] = [
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_outersmallhits',
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_outerlargehits' ]
#PlotComments[ 'Precision Hits - Outer' ] = 'PUT COMMENTS HERE IF YOU WANT'

#isolation
PlotPages[ r'Isolation - $p_T$-cone / $p_T$' ] = [
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_ptcone20rel',
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_ptcone30rel',
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_ptcone40rel' ]
#PlotComments[ r'Isolation - $p_T$-cone / $p_T$' ] = 'PUT COMMENTS HERE IF YOU WANT'

PlotPages[ r'Isolation - $E_T$-cone / $p_T$' ] = [
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_etcone20rel',
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_etcone30rel',
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_etcone40rel' ]
#PlotComments[ r'Isolation - $E_T$-cone / $p_T$' ] = 'PUT COMMENTS HERE IF YOU WANT'

#chi^2
PlotPages[ 'Track Fit Quality' ] = [
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_tchi2',
    'Muons/Prompt/reco/MuidCombined/Muons_Prompt_reco_MuidCombined_chi2ndof' ]
#PlotComments[ 'Track Fit Quality' ] = 'PUT COMMENTS HERE IF YOU WANT'

#Resolution
#PlotPages[ 'Resolution (MS+ID)' ] = [
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolution_PtRes_vs_pt',
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolution_PtRes_vs_eta',
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolution_PtRes_vs_phi' ]
#PlotComments[ 'Resolution (MS+ID)' ] = 'PUT COMMENTS HERE IF YOU WANT'

#PlotPages[ 'Resolution (ID)' ] = [
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolutionID_PtRes_vs_pt',
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolutionID_PtRes_vs_eta',
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolutionID_PtRes_vs_phi' ]
#PlotComments[ 'Resolution (ID)' ] = 'PUT COMMENTS HERE IF YOU WANT'

#PlotPages[ 'Resolution (MS)' ] = [
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolutionMS_PtRes_vs_pt',
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolutionMS_PtRes_vs_eta',
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolutionMS_PtRes_vs_phi' ]
#PlotComments[ 'Resolution (MS)' ] = 'PUT COMMENTS HERE IF YOU WANT'

#Momentum scale
#PlotPages[ r'$p_T$ Scale (MS+ID)' ] = [
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolution_PtScale_vs_pt',
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolution_PtScale_vs_eta',
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolution_PtScale_vs_phi' ]
#PlotComments[ r'$p_T$ Scale (MS+ID)' ] = 'PUT COMMENTS HERE IF YOU WANT'

#PlotPages[ r'$p_T$ Scale (ID)' ] = [
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolutionID_PtScale_vs_pt',
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolutionID_PtScale_vs_eta',
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolutionID_PtScale_vs_phi' ]
#PlotComments[ r'$p_T$ Scale (ID)' ] = 'PUT COMMENTS HERE IF YOU WANT'

#PlotPages[ r'$p_T$ Scale (MS)' ] = [
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolutionMS_PtScale_vs_pt',
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolutionMS_PtScale_vs_eta',
#    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_resolutionMS_PtScale_vs_phi' ]
#PlotComments[ r'$p_T$ Scale (MS)' ] = 'PUT COMMENTS HERE IF YOU WANT'

#Efficiency
PlotPages[ 'Efficiency' ] = [
    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_Eff_pt',
    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_Eff_eta',
    'Muons/Prompt/matched/MuidCombined/Muons_Prompt_matched_MuidCombined_Eff_phi' ]
#PlotComments[ 'Efficiency' ] = 'PUT COMMENTS HERE IF YOU WANT'

#Reco fraction
#PlotPages[ 'Reco Fraction - Inflight Muons' ] = [
#    'Muons/InFlight/reco/MuidCombined/Muons_InFlight_reco_MuidCombined_RecoFrac_pt',
#    'Muons/InFlight/reco/MuidCombined/Muons_InFlight_reco_MuidCombined_RecoFrac_eta',
#    'Muons/InFlight/reco/MuidCombined/Muons_InFlight_reco_MuidCombined_RecoFrac_phi' ]
#PlotComments[ 'Reco Fraction - Inflight Muons' ] = 'PUT COMMENTS HERE IF YOU WANT'

#PlotPages[ 'Reco Fraction - Rest Muons' ] = [
#    'Muons/Rest/reco/MuidCombined/Muons_Rest_reco_MuidCombined_RecoFrac_pt',
#    'Muons/Rest/reco/MuidCombined/Muons_Rest_reco_MuidCombined_RecoFrac_eta',
#    'Muons/Rest/reco/MuidCombined/Muons_Rest_reco_MuidCombined_RecoFrac_phi' ]
#PlotComments[ 'Reco Fraction - Rest Muons' ] = 'PUT COMMENTS HERE IF YOU WANT'

#PlotPages[ 'Reco Fraction - Unmatched All Authors' ] = [
#    'Muons/UnmatchedRecoMuons/Muons_UnmatchedRecoMuons_RecoFrac_pt',
#    'Muons/UnmatchedRecoMuons/Muons_UnmatchedRecoMuons_RecoFrac_eta',
#    'Muons/UnmatchedRecoMuons/Muons_UnmatchedRecoMuons_RecoFrac_phi' ]
#PlotComments[ 'Reco Fraction - Unmatched All Authors' ] = 'PUT COMMENTS HERE IF YOU WANT'
