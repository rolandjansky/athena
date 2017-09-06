#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import TopExamples.plot
import ROOT

integratedLuminosity = 1. #fb-1
dilepton = True

stack = [
    TopExamples.plot.Proc('test1/Data15.root', ROOT.kBlack, 'Data', False),
    TopExamples.plot.Proc('test1/13TeV_FS_ttbar_PowhegPythiaEvtGen.root', ROOT.kRed, 'Powheg+Pythia t#bar{t}'),
#    TopExamples.plot.Proc('test1/147770.Sherpa_CT10_Zee.e1434_s1933_s1911_r5591_r5625.22_08_1_output.root', ROOT.kBlue,  'Sherpa Zee'),
#    TopExamples.plot.Proc('test1/147771.Sherpa_CT10_Zmumu.e1434_s1933_s1911_r5591_r5625.22_08_1_output.root', ROOT.kGreen, 'Sherpa Zmumu'),
]

channels = ['ejets', 'mujets']
if dilepton:
    channels = ['ee', 'mumu', 'emu']

p = TopExamples.plot.Plotter('index.html', stack, integratedLuminosity)

if dilepton:
    p.h2('Some control plots')

    p.h3('met in Z window, at least two jets')
    p.plot('ee/control_met')
    p.plot('mumu/control_met')
    p.br()

    p.h3('invariant mass for met < 60 + >= 2 jets')
    p.plot('ee/control_invmass')
    p.plot('mumu/control_invmass')
    p.br()

    p.h3('njet for met < 60 in Z-window')
    p.plot('ee/control_njet')
    p.plot('mumu/control_njet')
    p.br()

    p.h3('dilepton invariant mass for met < 60 GeV, >= 2 jets')
    p.plot('ee/control_invmass')
    p.plot('mumu/control_invmass')
    p.br()

    p.h3('in Z window, met > 30, >= 2 jet')
    p.plot('ee/dy_dphi')
    p.plot('mumu/dy_dphi')
    p.br()

    p.plot('ee/dy_dphi_zpt')
    p.plot('mumu/dy_dphi_zpt')
    p.br()

    p.plot('ee/dy_counter')
    p.plot('mumu/dy_counter')
    p.br()

    p.h2('All cuts except <something> style plots')

    p.h3('all cuts except Z window (then with Z window)')
    p.plot('ee/control_sig_invmass')
    p.plot('mumu/control_sig_invmass')
    p.br()

    p.plot('ee/control_sig_invmass_cut')
    p.plot('mumu/control_sig_invmass_cut')
    p.br()

    p.h3('all cuts except njet')
    p.plot('ee/control_sig_njet')
    p.plot('mumu/control_sig_njet')
    p.br()

    p.h3('all cuts except met')
    p.plot('ee/control_sig_met')
    p.plot('mumu/control_sig_met')
    p.br()

    p.h3('emu channel ht')
    p.plot('emu/ht')
    p.br()

for channel in channels:
    p.h2(channel)

    p.h3('event')
    p.plot(channel + '/mu')
    p.plot(channel + '/mc_weight')
    p.br()

    p.h3('el')
    p.plot(channel + '/el_n')
    p.plot(channel + '/el_pt')
    p.plot(channel + '/el_eta')
    p.plot(channel + '/el_phi')
    p.plot(channel + '/el_e')
    p.plot(channel + '/el_m')
    p.plot(channel + '/el_charge')
    p.br()

    p.h3('mu')
    p.plot(channel + '/mu_n')
    p.plot(channel + '/mu_pt')
    p.plot(channel + '/mu_eta')
    p.plot(channel + '/mu_phi')
    p.plot(channel + '/mu_e')
    p.plot(channel + '/mu_m')
    p.plot(channel + '/mu_charge')
    p.br()

    p.h3('jet')
    p.plot(channel + '/jet_n')
    p.plot(channel + '/jet_pt')
    p.plot(channel + '/jet_eta')
    p.plot(channel + '/jet_phi')
    p.plot(channel + '/jet_e')
    p.plot(channel + '/jet_mv2c20')
    p.br()

    p.h3('leading jet')
    p.plot(channel + '/jet0_pt')
    p.plot(channel + '/jet0_eta')
    p.plot(channel + '/jet0_phi')
    p.plot(channel + '/jet0_e')
    p.br()

    p.h3('sub-leading jet')
    p.plot(channel + '/jet1_pt')
    p.plot(channel + '/jet1_eta')
    p.plot(channel + '/jet1_phi')
    p.plot(channel + '/jet1_e')
    p.br()

    p.h3('sub-sub-leading jet')
    p.plot(channel + '/jet2_pt')
    p.plot(channel + '/jet2_eta')
    p.plot(channel + '/jet2_phi')
    p.plot(channel + '/jet2_e')
    p.br()

    p.h3('sub-sub-sub-leading jet')
    p.plot(channel + '/jet3_pt')
    p.plot(channel + '/jet3_eta')
    p.plot(channel + '/jet3_phi')
    p.plot(channel + '/jet3_e')
    p.br()

    p.h3('met')
    p.plot(channel + '/met_et')
    p.plot(channel + '/met_phi')

p.doneAndDusted()
