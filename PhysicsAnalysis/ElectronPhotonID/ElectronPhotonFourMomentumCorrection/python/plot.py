#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import numpy as np
import logging
import os
import matplotlib.pyplot as plt
import matplotlib.patches as patches

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
logging.basicConfig(level=logging.INFO)

plt.rcParams['image.cmap'] = 'coolwarm'  # RdBu_r
logging.basicConfig(level=logging.INFO)

from matplotlib import rcParams
rcParams['font.family'] = 'sans-serif'
rcParams['mathtext.fontset'] = 'stixsans'
rcParams['mathtext.default'] = 'rm'
rcParams['font.sans-serif'] = 'helvetica, Helvetica, Nimbus Sans L, Mukti Narrow, FreeSans'
#rcParams['figure.figsize'] = 8.75, 5.92
#rcParams['figure.facecolor'] = 'white'
#rcParams['figure.subplot.bottom'] = 0.16
#rcParams['figure.subplot.top'] = 0.95
#rcParams['figure.subplot.left'] = 0.16
#rcParams['figure.subplot.right'] = 0.95

# axes
#rcParams['axes.labelsize'] = 20
#rcParams['xtick.labelsize'] = 19
#rcParams['xtick.major.size'] = 12
#rcParams['xtick.minor.size'] = 6
#rcParams['ytick.labelsize'] = 19
#rcParams['ytick.major.size'] = 14
#rcParams['ytick.minor.size'] = 7
#rcParams['lines.markersize'] = 8
# rcParams['lines.markeredgewidth'] = 0. # not working, it changes other stuff

# legend
rcParams['legend.numpoints'] = 1
#rcParams['legend.fontsize'] = 19
rcParams['legend.labelspacing'] = 0.3
#rcParams['legend.frameon'] = False

extensions = 'pdf', 'png'

fout = ROOT.TFile('output_plot.root', 'recreate')

def histo2data(histo):
    xs = []
    ys = []
    exs = []
    eys = []
    for ibin in xrange(1, histo.GetNbinsX() + 1):
        xs.append(histo.GetBinCenter(ibin))
        ys.append(histo.GetBinContent(ibin))
        eys.append(histo.GetBinError(ibin))
        exs.append(0.5 * histo.GetBinWidth(ibin))
    return np.array(xs), np.array(exs), np.array(ys), np.array(eys)


def partition(x, n):
    r = []
    for xx in x:
        r.append(xx)
        if len(r) == n:
            yield r
            r = []
            continue
    if len(r) > 0:
        yield r


def generator_photon(self):
    store = ROOT.xAOD.TStore()

    factory = ROOT.EgammaFactory(store)
    for eta in np.arange(-2.5, 2.5, 0.05):
        for e in np.arange(1E3, 1000E3, 100E3):
            photon = factory.create_photon(eta, 0.1, e, 0)
            yield photon
    store.clear()


def eval_sys_eta_pt(tool, etas, pts, simulation=True, particle='unconverted'):
    event = ROOT.xAOD.TEvent()
    factory = ROOT.EgammaFactory()
    result = np.ones((len(pts), len(etas)))
    ei = factory.create_eventinfo(simulation, 266904)
    assert ei.eventType(ROOT.xAOD.EventInfo.IS_SIMULATION) == simulation
    for ieta, eta in enumerate(etas):
        for ipt, pt in enumerate(pts):
            if particle == 'unconverted':
                p = factory.create_photon(eta, 0.1, pt * np.cosh(eta), 0)
            elif particle == 'converted':
                p = factory.create_photon(eta, 0.1, pt * np.cosh(eta), 100)
            elif particle == 'electron':
                p = factory.create_electron(eta, 0.1, pt * np.cosh(eta))
            else:
                raise ValueError()
            result[ipt, ieta] = tool.getEnergy(p, ei)
    del event
    return result


def eval_sys_eta_phi(tool, etas, phis, pt, simulation, particle='unconverted'):
    event = ROOT.xAOD.TEvent()
    factory = ROOT.EgammaFactory()
    result = np.ones((len(etas), len(phis)))
    ei = factory.create_eventinfo(simulation, 100000)
    for ieta, eta in enumerate(etas):
        for iphi, phi in enumerate(phis):
            if particle == 'unconverted':
                p = factory.create_photon(eta, phi, pt * np.cosh(eta), 0)
            elif particle == 'converted':
                p = factory.create_photon(eta, phi, pt * np.cosh(eta), 100)
            elif particle == 'electron':
                p = factory.create_electron(eta, phi, pt * np.cosh(eta))
            else:
                raise ValueError()
            result[ieta, iphi] = tool.getEnergy(p, ei)
    del event
    return result


def eval_eta_slice(tool, etas, pts, ptype, only_material=False):
    sys_set = ROOT.CP.SystematicSet()
    tool.applySystematicVariation(sys_set)
    nominal = eval_sys_eta_pt(tool, etas, pts, particle=ptype)
    all_syst = tool.recommendedSystematics()

    results = {}
    for sys in all_syst:
        sys_name = sys.name()
        if 'RESOLUTION' in sys_name:
            continue
        if '1down' in sys_name:
            continue
        if only_material:
            if 'MAT' not in sys_name:
                continue
        logging.info("plotting sys %s", sys_name)
        sys_set = ROOT.CP.SystematicSet()
        sys_set.insert(sys)
        tool.applySystematicVariation(sys_set)

        sys = eval_sys_eta_pt(tool, etas, pts, particle=ptype)

        ratio = sys / nominal

        ratio = np.average(ratio - 1, axis=1)
        results[sys_name] = ratio
    return results


def plot_all_syst_eta_slice(etabins, netas=3, esmodel='es2012c',
                            decorrelation='FULL_v1', ptype='unconverted',
                            basedir='plot', only_material=False):

    tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
    tool.setProperty("ESModel", esmodel)
    tool.setProperty("decorrelationModel", decorrelation)
    tool.setProperty("int")("doSmearing", 0)
    tool.initialize()

    pts = np.logspace(np.log10(5E3), 7, 100)

    # compute the inclusive one, just to sort
    results = eval_eta_slice(tool, np.linspace(-2.5, 2.5, 20), pts, ptype, only_material)
    sorted_sys_name = sorted(list(results), key=lambda k: -np.max(np.abs(results[k])))

    for etamin, etamax in etabins:
        etas = np.linspace(etamin, etamax, netas)
        results = eval_eta_slice(tool, etas, pts, ptype, only_material)

        for ip, p in enumerate(partition(sorted_sys_name, 4)):
            f, ax = plt.subplots()
            for sys_name in p:
                r = results[sys_name]
                r[np.isnan(r)] = 0
                ax.semilogx(pts / 1E3, r * 100., label=sys_name)
            ax.set_xlabel('$p_T$ [GeV]')
            ax.set_ylabel('ratio to nominal [%]')
            ax.set_title(ptype + ' $\eta\in[%.2f, %.2f]$' % (etamin, etamax))
            if only_material:
                vmax = 1.
            else:
                vmax = np.max(np.abs(ax.get_ylim()))
            ax.axis('tight')
            ax.set_ylim(-vmax, vmax)
            plt.legend(loc=0)
            for extension in extensions:
                f.savefig(os.path.join(basedir, "%s_%s_%.2f_%.2f_%d.%s" % (ptype, decorrelation, etamin, etamax, ip, extension)))
            plt.close(f)


def values2histo(name, title, x, y):
    histo = ROOT.TH1F(name, title, len(x) - 1, x)
    for ibin, yy, in enumerate(y, 1):
        histo.SetBinContent(ibin, yy)
    return histo


def plot_all_Zee_syst(etas, pt=100E3, basedir='plots'):

    tool_es2012c = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2012c")
    tool_es2012c.setProperty("ESModel", "es2012c")
    tool_es2012c.setProperty("decorrelationModel", 'FULL_ETACORRELATED_v1')
    tool_es2012c.setProperty("int")("doSmearing", 0)
    tool_es2012c.initialize()

    tool_es2015PRE = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE")
    tool_es2015PRE.setProperty("ESModel", "es2015PRE")
    tool_es2015PRE.setProperty("decorrelationModel", 'FULL_ETACORRELATED_v1')
    tool_es2015PRE.setProperty("int")("doSmearing", 0)
    tool_es2015PRE.initialize()

    tool_es2015PRE_notemp = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE_notemp")
    tool_es2015PRE_notemp.setProperty("ESModel", "es2015PRE")
    tool_es2015PRE_notemp.setProperty("decorrelationModel", 'FULL_ETACORRELATED_v1')
    tool_es2015PRE_notemp.setProperty("int")("doSmearing", 0)
    tool_es2015PRE_notemp.setProperty("int")("use_temp_correction201215", 0)
    tool_es2015PRE_notemp.initialize()

    tools = [tool_es2012c, tool_es2015PRE_notemp, tool_es2015PRE]

    nominal_es2012c = eval_sys_eta_pt(tool_es2012c, etas, [pt], particle='electron')[0]
    nominal_es2015PRE = eval_sys_eta_pt(tool_es2015PRE, etas, [pt], particle='electron')[0]
    nominal_es2015PRE_notemp = eval_sys_eta_pt(tool_es2015PRE_notemp, etas, [pt], particle='electron')[0]

    # up variations
    sys = ROOT.CP.SystematicVariation('EG_SCALE_ZEESYST', 1)
    sys_set = ROOT.CP.SystematicSet()
    sys_set.insert(sys)

    for tool in tools:
        tool.applySystematicVariation(sys_set)

    sys_up_es2012c = eval_sys_eta_pt(tool_es2012c, etas, [pt], particle='electron')[0]
    sys_up_es2015PRE = eval_sys_eta_pt(tool_es2015PRE, etas, [pt], particle='electron')[0]
    sys_up_es2015PRE_notemp = eval_sys_eta_pt(tool_es2015PRE_notemp, etas, [pt], particle='electron')[0]

    ratio_sys_up_es2012c = sys_up_es2012c / nominal_es2012c - 1
    ratio_sys_up_es2015PRE = sys_up_es2015PRE / nominal_es2015PRE - 1
    ratio_sys_up_es2015PRE_notemp = sys_up_es2015PRE_notemp / nominal_es2015PRE_notemp - 1

    # down variations
    sys = ROOT.CP.SystematicVariation('EG_SCALE_ZEESYST', -1)
    sys_set = ROOT.CP.SystematicSet()
    sys_set.insert(sys)

    for tool in tools:
        tool.applySystematicVariation(sys_set)

    sys_down_es2012c = eval_sys_eta_pt(tool_es2012c, etas, [pt], particle='electron')[0]
    sys_down_es2015PRE = eval_sys_eta_pt(tool_es2015PRE, etas, [pt], particle='electron')[0]
    sys_down_es2015PRE_notemp = eval_sys_eta_pt(tool_es2015PRE_notemp, etas, [pt], particle='electron')[0]

    ratio_sys_down_es2012c = sys_down_es2012c / nominal_es2012c - 1
    ratio_sys_down_es2015PRE = sys_down_es2015PRE / nominal_es2015PRE - 1
    ratio_sys_down_es2015PRE_notemp = sys_down_es2015PRE_notemp / nominal_es2015PRE_notemp - 1

    # up stat
    sys = ROOT.CP.SystematicVariation('EG_SCALE_ZEESTAT', 1)
    sys_set = ROOT.CP.SystematicSet()
    sys_set.insert(sys)
    tool_es2015PRE.applySystematicVariation(sys_set)
    stat_up_es2015PRE = eval_sys_eta_pt(tool_es2015PRE, etas, [pt], particle='electron')[0]
    ratio_stat_up = stat_up_es2015PRE / nominal_es2015PRE - 1


    # down stat
    sys = ROOT.CP.SystematicVariation('EG_SCALE_ZEESTAT', -1)
    sys_set = ROOT.CP.SystematicSet()
    sys_set.insert(sys)
    tool_es2015PRE.applySystematicVariation(sys_set)
    stat_down_es2015PRE = eval_sys_eta_pt(tool_es2015PRE, etas, [pt], particle='electron')[0]
    ratio_stat_down = stat_down_es2015PRE / nominal_es2015PRE - 1

    ratio_full_down = -np.sqrt(ratio_stat_down ** 2 + ratio_sys_down_es2015PRE ** 2)
    ratio_full_up = np.sqrt(ratio_stat_up ** 2 + ratio_sys_up_es2015PRE ** 2)

    fig, ax = plt.subplots()

    ax.fill_between(etas, ratio_full_down, ratio_full_up, color='#d69e8f', interpolate=False, label='$\cdots\oplus$ stat = 2015PRE (stat $\oplus$ sys)', alpha=0.6)
    ax.fill_between(etas, ratio_sys_down_es2015PRE, ratio_sys_up_es2015PRE, color='#d7d790', interpolate=False, label='$\cdots\oplus$ temp = 2015PRE sys', alpha=0.6)
    ax.fill_between(etas, ratio_sys_down_es2015PRE_notemp, ratio_sys_up_es2015PRE_notemp, color='#91da95', label='$\cdots\oplus$ 7/8 TeV diff $\oplus$ 34/68 bins diff', alpha=0.6)
    ax.fill_between(etas, ratio_sys_down_es2012c, ratio_sys_up_es2012c, color='#93dcd1', interpolate=False, label='2012c', alpha=0.6)

    p1 = patches.Rectangle((0, 0), 1, 1, color="#d69e8f")
    p2 = patches.Rectangle((0, 0), 1, 1, color="#d7d790")
    p3 = patches.Rectangle((0, 0), 1, 1, color="#91da95")
    p4 = patches.Rectangle((0, 0), 1, 1, color="#93dcd1")

    legend1 = ax.legend([p4, p3, p2, p1], ['2012c',
                                           '$\cdots\oplus$ 7/8 TeV diff $\oplus$ 34/68 bins diff',
                                           '$\cdots\oplus$ temp = 2015PRE sys',
                                           '$\cdots\oplus$ stat = 2015PRE (stat $\oplus$ sys)',
                                           ], loc='upper right', numpoints=1, title='errors')

    f = ROOT.TFile("~/Data6_scaledData.root")
    histo_scale = f.Get("alpha")
    x, ex, y, ey = histo2data(histo_scale)

    h1 = ax.errorbar(x, y, yerr=ey, xerr=ex, fmt='o', zorder=11)

    f2 = ROOT.TFile("~/uA2MeV.root")
    histo_uA2MeV = f2.Get("histo_uA2MeV_week12")
    x, ex, y, ey = histo2data(histo_uA2MeV)
    line_uA2MeV, = ax.plot(x, y - 1, 'k-', zorder=10, label='expected deviation')
    ax.plot(-x, y - 1, 'k-', zorder=10)

    ax.set_xlabel('$\eta$')
    ax.set_ylim(-0.08, 0.08)


    ax.legend([h1, line_uA2MeV], ['scales 13 TeV|es2015 PRE', 'expected deviation'], loc='lower right', numpoints=1)
    ax.add_artist(legend1)

    ax.grid()

    fig.savefig('Zee_sys.png')
    fig.savefig('Zee_sys.svg')
    fig.savefig('Zee_sys.pdf')


def plot_all_syst_fixed_pt(tools, names, labels, pt=100E3, ptype='unconverted', basedir='plots'):
    for tool in tools:
        empty_set = ROOT.CP.SystematicSet()
        tool.applySystematicVariation(empty_set)
    etas_histo = np.linspace(-5, 5, 101, True)
    etas = 0.5 * (etas_histo[1:] + etas_histo[:-1])
    nominals = [eval_sys_eta_pt(tool, etas, [pt], particle=ptype)[0] for tool in tools]
    all_syst = tools[0].recommendedSystematics()
    for sys in all_syst:
        sys_name = sys.name()
        sys_set = ROOT.CP.SystematicSet()
        sys_set.insert(sys)
        logging.info("plotting sys %s", sys_name)

        f, ax = plt.subplots()

        for tool, nominal, name, label in zip(tools, nominals, names, labels):
            tool.applySystematicVariation(sys_set)
            sys = eval_sys_eta_pt(tool, etas, [pt], particle=ptype)[0]
            ratio = sys / nominal - 1
            ax.plot(etas, ratio * 100, label=label)
            histo = values2histo(name + "_" + sys_name, label, etas_histo, ratio * 100)
            fout.WriteTObject(histo)

        ax.grid()
        ax.legend()
        ax.set_ylabel('effect [%]')
        ax.set_title(ptype + " " + sys_name + " at $p_{T}$ = %.2f GeV" % (pt / 1E3))
        ax.set_xlabel('$\eta$')
        for extension in extensions:
            f.savefig(os.path.join(basedir, "%s_%s_pt_%.2f.%s" % (ptype, sys_name, pt / 1E3, extension)))
        plt.close(f)


def plot_all_syst(esmodel='es2012c', decorrelation='FULL_v1', ptype='unconverted', basedir='plots'):
    tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
    tool.setProperty("ESModel", esmodel)
    tool.setProperty("decorrelationModel", decorrelation)
    tool.setProperty("int")("doSmearing", 0)
    tool.initialize()

    etas = np.arange(-3, 3, 0.1)
    pts = np.logspace(4, 7, 100)
    X, Y = np.meshgrid(etas, pts)
    nominal = eval_sys_eta_pt(tool, etas, pts, particle=ptype)

    all_syst = tool.recommendedSystematics()
    for sys in all_syst:
        sys_name = sys.name()
        if "RESOLUTION" in sys_name:
            continue
        logging.info("plotting sys %s", sys_name)

        sys_set = ROOT.CP.SystematicSet()
        sys_set.insert(sys)
        tool.applySystematicVariation(sys_set)

        sys = eval_sys_eta_pt(tool, etas, pts, particle=ptype)

        ratio = sys / nominal
        f, ax = plt.subplots()
        ratio[nominal == 0] = 1
        # TODO: check why nan
        ratio[np.isnan(ratio)] = 1
        vmax = np.percentile(np.abs(ratio - 1), 90)

        p = ax.pcolormesh(etas, pts / 1E3, (ratio - 1) * 100., vmin=-vmax * 100, vmax=vmax * 100)
        # ax.contour(X, Y / 10E3, ratio - 1, levels=[0], colors='k', linercParamss='dashed')
        ax.set_title(sys_name)
        ax.set_xlabel('$\eta$')
        ax.set_ylabel('$p_T$ [GeV]')
        ax.set_yscale('log')
        cb = f.colorbar(p)
        cb.ax.set_ylabel('ratio to nominal [%]')
        for extension in extensions:
            f.savefig(os.path.join(basedir, "%s_%s_%s.%s" % (ptype, decorrelation, sys_name, extension)))
        plt.close(f)


def plot_all_scales(esmodels, basedir, labels=None, etas=np.arange(-4.5, 4.5, 0.01), add_patch=False):
    labels = labels or esmodels
    scales = {}
    to_legend = []
    for esmodel, label in zip(esmodels, labels):
        energy_with = []
        energy_without = []

        key_properties = []
        value_properties = []
        type_properties = []

        if type(esmodel) != str:
            key_properties = esmodel[1]
            value_properties = esmodel[2]
            type_properties = esmodel[3]
            esmodel = esmodel[0]
        print esmodel, key_properties

        tool_with = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_with")
        tool_without = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_without")
        for tool in tool_with, tool_without:
            tool.setProperty("ESModel", esmodel)
            tool.setProperty("int")("doSmearing", 0)
            for k, v, t in zip(key_properties, value_properties, type_properties):
                tool.setProperty(t)(k, v)

        tool_with.setProperty("int")("doScaleCorrection", 1)
        tool_without.setProperty("int")("doScaleCorrection", 0)

        tool_with.initialize()
        tool_without.initialize()



        event = ROOT.xAOD.TEvent()
        factory = ROOT.EgammaFactory()
        ei = factory.create_eventinfo(False, 266904)

        for eta in etas:
            el = factory.create_electron(eta, 0.1, 40E3)
            en_with = tool_with.getEnergy(el, ei)
            en_without = tool_without.getEnergy(el, ei)
            energy_with.append(en_with)
            energy_without.append(en_without)
        scales[label] = np.array(energy_without) / np.array(energy_with) - 1

    f, ax = plt.subplots()
    for k, v in scales.iteritems():
        ax.plot(etas, v * 100, label=k)
    ax.set_xlabel("$\eta$")
    ax.set_ylabel("energy without scale factors / energy with scale factors - 1 [%]")
    ax.grid()

    if add_patch:
        ax.add_patch(patches.Rectangle((-2.47, -0.013 * 100 - 0.0168 * 100), +2.47-1.55, 2 * 0.0168 * 100, alpha=0.4, label='13 TeV scales 3 bins', color='k'))
        #ax.add_patch(patches.Rectangle((-1.55, 0.00419 * 100 - 0.001 * 100), +1.55 - 1.37, 2 * 0.001 * 100, alpha=0.4, color='k'))
        ax.add_patch(patches.Rectangle((-1.37, -0.0121 * 100 - 0.0052 * 100), 2 * +1.37, 2 * 0.0052 * 100, alpha=0.4, color='k'))
        ax.add_patch(patches.Rectangle((1.55, -0.013 * 100 - 0.0168 * 100), +2.47-1.55, 2 * 0.0168 * 100, alpha=0.4, color='k'))

        ax.add_patch(patches.Rectangle((-2.47, -0.00649344*100 - 0.00465043*100), +2.47 * 2, 2 * 0.00465043 * 100, alpha=0.5, label='13 TeV scales 1 bin', color='orange'))

    ax.legend()

    for extension in extensions:
        f.savefig(os.path.join(basedir, "scales.%s" % extension))
    plt.close(f)


def plot_all_cterms(esmodels, basedir, labels=None, etas=np.arange(-4.5, 4.5, 0.01)):
    labels = labels or esmodels
    cterms_all_models = {}
    for esmodel, label in zip(esmodels, labels):

        tool_with = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_with")
        tool_with.setProperty("ESModel", esmodel)
        tool_with.setProperty("int")("doSmearing", 1)
        tool_with.initialize()

        tool_without = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_without")
        tool_without.setProperty("ESModel", esmodel)
        tool_without.setProperty("int")("doSmearing", 0)
        tool_without.initialize()

        event = ROOT.xAOD.TEvent()
        factory = ROOT.EgammaFactory()
        ei = factory.create_eventinfo(True, 100000)

        cterms = []
        for eta in etas:
            el = factory.create_electron(eta, 0.1, 40E3)
            en_without = tool_without.getEnergy(el, ei)
            ratios = np.zeros(1000)
            for repetition in xrange(1000):
                en_with = tool_with.getEnergy(el, ei)
                ratios[repetition] = en_with / en_without
            cterms.append(np.std(ratios))
        cterms_all_models[label] = cterms

    f, ax = plt.subplots()
    for k, v in cterms_all_models.iteritems():
        ax.plot(etas, v, label=k)
    ax.set_xlabel("$\eta$")
    ax.set_ylabel("std (energy with additional cterm / energy without)")

    ax.add_patch(patches.Rectangle((-2.47, 0.028 - 0.027), +2.47-1.55, 2 * 0.027, alpha=0.4, label='13 TeV cterm 3 bins', color='k'))
    ax.add_patch(patches.Rectangle((-1.37, -0.003 - 0.014), +1.37 * 2, 2 * 0.014, alpha=0.4, color='k'))
    ax.add_patch(patches.Rectangle((1.55, 0.028 - 0.027), +2.47-1.55, 2 * 0.027, alpha=0.4, color='k'))
    ax.set_ylim(-0.03, 0.06)


    ax.grid()
    ax.legend()
    for extension in extensions:
        f.savefig(os.path.join(basedir, "cterms.%s" % extension))
    plt.close(f)


def compare_two_tools_eta(tool1, tool2, pt, simulation, name, basedir, title, particle='unconverted'):
    etas = np.arange(-4.5, 4.5, 0.01)
    r1 = eval_sys_eta_pt(tool1, etas, [pt], simulation, particle)
    r2 = eval_sys_eta_pt(tool2, etas, [pt], simulation, particle)
    r = r1 / r2 - 1
    r = r[0]
    print r
    f, ax = plt.subplots()
    r = np.nan_to_num(r)
    ax.plot(etas, r * 100)
    ax.set_xlabel('$\eta$')
    ax.set_ylabel('effect [%]')
    ax.set_title(title)
    ax.axis('tight')
    ax.grid()
    for extension in extensions:
        f.savefig(os.path.join(basedir, name + "." + extension))
    plt.close(f)


def compare_two_tools_eta_phi(tool1, tool2, pt, simulation, name, basedir, title, particle='unconverted'):
    etas = np.arange(-4.5, 4.5, 0.1)
    phis = np.arange(-np.pi, np.pi, 0.1)
    r1 = eval_sys_eta_phi(tool1, etas, phis, pt, simulation, particle)
    r2 = eval_sys_eta_phi(tool2, etas, phis, pt, simulation, particle)
    r = r1 / r2 - 1
    f, ax = plt.subplots()
    m = np.max(np.abs(r))
    p = ax.pcolormesh(phis, etas, r * 100, vmin=-m * 100, vmax=m * 100)
    ax.set_xlabel('$\phi$')
    ax.set_ylabel('$\eta$')
    ax.set_title(title)
    ax.axis('tight')
    cb = f.colorbar(p)
    cb.ax.set_ylabel('ratio to nominal [%]')
    for extension in extensions:
        f.savefig(os.path.join(basedir, name + "." + extension))
    plt.close(f)


def compare_two_tools_eta_pt(tool1, tool2, simulation, name, basedir, title, particle='unconverted'):
    etas = np.arange(-4.5, 4.5, 0.05)
    pts = np.logspace(3.2, 6, 50)
    r1 = eval_sys_eta_pt(tool1, etas, pts, simulation, particle)
    r2 = eval_sys_eta_pt(tool2, etas, pts, simulation, particle)
    r = r1 / r2 - 1
    f, ax = plt.subplots()
    r = np.nan_to_num(r)
    m = np.max(np.abs(r))
    p = ax.pcolormesh(etas, pts / 1E3, r * 100, vmin=-m * 100, vmax=m * 100)
    ax.set_yscale('log')
    ax.set_ylabel('$p_T$ [GeV]')
    ax.set_xlabel('$\eta$')
    ax.set_title(title)
    ax.axis('tight')

    cb = f.colorbar(p)
    cb.ax.set_ylabel('ratio to nominal [%]')
    for extension in extensions:
        f.savefig(os.path.join(basedir, name + "." + extension))
    plt.close(f)


def check_gain(basedir, esmodel):
    tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_%s" % esmodel)
    tool.setProperty("ESModel", esmodel)
    tool.setProperty("decorrelationModel", 'FULL_ETACORRELATED_v1')
    tool.setProperty("int")("doSmearing", 0)
    tool.initialize()

    tool_no_gain = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_%s" % esmodel)
    tool_no_gain.setProperty("ESModel", esmodel)
    tool_no_gain.setProperty("decorrelationModel", 'FULL_ETACORRELATED_v1')
    tool_no_gain.setProperty("int")("doSmearing", 0)
    tool_no_gain.setProperty("int")("useGainCorrection", 0)
    tool_no_gain.initialize()

    for ptype in 'electron', 'unconverted', 'converted':
        compare_two_tools_eta_pt(tool_no_gain, tool,
                                 simulation=False,
                                 name='gain_%s_%s' % (esmodel, ptype),
                                 basedir=basedir, particle=ptype,
                                 title='gain correction effect - %s - %s' % (esmodel, ptype))

    compare_two_tools_eta(tool_no_gain, tool, 40E3,
                          simulation=False,
                          name='gain_%s_%s_40GeV' % (esmodel, 'electron'),
                          basedir=basedir, title='gain correction effect - %s 40 GeV electron' % esmodel,
                          particle='electron')

def check_fast(basedir, esmodel):
    tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_%s" % esmodel)
    tool.setProperty("ESModel", esmodel)
    tool.setProperty("decorrelationModel", 'FULL_ETACORRELATED_v1')
    tool.setProperty("int")("doSmearing", 0)
    tool.initialize()

    tool_fast = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_%s" % esmodel)
    tool_fast.setProperty("ESModel", esmodel)
    tool_fast.setProperty("decorrelationModel", 'FULL_ETACORRELATED_v1')
    tool_fast.setProperty("int")("doSmearing", 0)
    tool_fast.setProperty("bool")("useAFII", True)
    tool_fast.initialize()

    for ptype in 'electron', 'converted', 'unconverted':
        compare_two_tools_eta(tool_fast, tool,
                              pt=100E3, simulation=True,
                              name='fast_%s_%s' % (esmodel, ptype),
                              basedir=basedir, particle=ptype,
                              title='fast correction effect %s %s 100 GeV' % (esmodel, ptype))


def check_uniformity(basedir, esmodel):
    tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_%s" % esmodel)
    tool.setProperty("ESModel", esmodel)
    tool.setProperty("decorrelationModel", 'FULL_ETACORRELATED_v1')
    tool.setProperty("int")("doSmearing", 0)
    tool.initialize()

    tool_no_uniformity = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_%s" % esmodel)
    tool_no_uniformity.setProperty("ESModel", esmodel)
    tool_no_uniformity.setProperty("decorrelationModel", 'FULL_ETACORRELATED_v1')
    tool_no_uniformity.setProperty("int")("doSmearing", 0)
    tool_no_uniformity.setProperty("int")("usePhiUniformCorrection", 0)
    tool_no_uniformity.initialize()

    compare_two_tools_eta_phi(tool_no_uniformity, tool,
                              pt=100E3, simulation=False,
                              name='uniformity_%s' % esmodel,
                              basedir=basedir, particle='unconverted',
                              title='uniformity correction effect - %s - unconverted 100 GeV' % esmodel)


def compare_all_syst_fixed_pt(basedir, esmodels, names=None, labels=None):
    names = names or esmodels
    labels = labels or names
    tools = []
    for esmodel in esmodels:
        if type(esmodel) is str:
            tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_%s" % esmodel)
            tool.setProperty("ESModel", esmodel)
            tool.setProperty("decorrelationModel", 'FULL_ETACORRELATED_v1')
            tool.setProperty("int")("doSmearing", 0)
            tool.initialize()
            tools.append(tool)
        else:
            tools.append(esmodel)
    plot_all_syst_fixed_pt(tools, names=names, labels=labels, basedir=basedir)


def plot_resolution_eta_pt(basedir, tool, pts, etas, ptype, title):
    event = ROOT.xAOD.TEvent()
    factory = ROOT.EgammaFactory()
    result = np.ones((len(pts), len(etas)))
    eta_centers = 0.5 * (etas[1:] + etas[:-1])
    pt_centers = 0.5 * (pts[1:] + pts[:-1])

    result = np.zeros((len(eta_centers), len(pt_centers)))

    for ieta, eta in enumerate(eta_centers):
        for ipt, pt in enumerate(pt_centers):
            e = pt * np.cosh(eta)
            if ptype == "electron":
                p = factory.create_electron(eta, 0.1, e)
            elif ptype == "converted":
                p = factory.create_converted_photon(eta, 0.1, e)
            elif ptype == "unconverted":
                p = factory.create_unconverted_photon(eta, 0.1, e)
            else:
                raise ValueError()
            res = tool.getResolution(p)
            result[ieta, ipt] = res

    fig, ax = plt.subplots()
    vmin, vmax = np.percentile(result, (2, 98))
    p = ax.pcolormesh(pts / 1E3, etas, result, vmin=vmin, vmax=vmax)
    ax.set_title(title)
    ax.axis('tight')
    ax.set_xlabel('$p_T$ [GeV]')
    ax.set_ylabel('$\eta$')
    ax.set_xscale('log')
    cb = fig.colorbar(p)
    cb.ax.set_ylabel('resolution')
    for extension in extensions:
        fig.savefig(os.path.join(basedir, 'resolution_%s.%s' % (ptype, extension)))


def plot_resolution_error(basedir):
    for esmodel in 'es2012c', 'es2015PRE', 'es2015PRE_res_improved':
        for particle in ('electron', 'converted', 'unconverted'):
            for eta_min, eta_max in (0, 0.4), (0.4, 0.6), (0.6, 1.37), (1.52, 1.8), (1.8, 2.37):
                plot_resolution_error_bin(eta_min, eta_max, particle, esmodel, basedir)


def plot_resolution_error_bin(eta_min, eta_max, particle, esmodel, basedir):
    tool = ROOT.AtlasRoot.egammaEnergyCorrectionTool()
    tool.setESModel(getattr(ROOT.egEnergyCorr, esmodel))
    tool.initialize()
    variations_name = {"Zsmearing": ROOT.egEnergyCorr.Resolution.ZSmearingUp,
                       "samplling": ROOT.egEnergyCorr.Resolution.SamplingTermUp,
                       "material ID": ROOT.egEnergyCorr.Resolution.MaterialIDUp,
                       "material calo": ROOT.egEnergyCorr.Resolution.MaterialCaloUp,
                       "material gap": ROOT.egEnergyCorr.Resolution.MaterialGapUp,
                       "material cryo": ROOT.egEnergyCorr.Resolution.MaterialCryoUp,
                       "pileup": ROOT.egEnergyCorr.Resolution.PileUpUp}

    errors = {}
    ptype = {"electron": ROOT.PATCore.ParticleType.Electron,
             "unconverted": ROOT.PATCore.ParticleType.UnconvertedPhoton,
             "converted": ROOT.PATCore.ParticleType.ConvertedPhoton
             }[particle]
    pt_range = np.linspace(10E3, 800E3, 100)
    for variation_name, variation_id in variations_name.iteritems():
        eta_range = np.linspace(eta_min, eta_max, 5)
        errors_var_pt_eta = np.zeros((len(pt_range), len(eta_range)))
        for ipt, pt in enumerate(pt_range):
            for ieta, eta in enumerate(eta_range):
                energy = pt * np.cosh(eta)
                errors_var_pt_eta[ipt, ieta] = tool.getResolutionError(energy, eta, eta, ptype, variation_id)
        errors[variation_name] = errors_var_pt_eta.mean(axis=1)

    sorted_keys = sorted(variations_name.keys(), key=lambda x: errors[x].mean())
    total = np.sqrt(np.sum(e**2 for e in errors.values()))

    fig, ax = plt.subplots()
    ax.fill_between(pt_range / 1E3, total, 0, label='total', color='0.8')
    ax.plot(pt_range / 1E3, total, label='total', color='k')
    for k in errors:
        ax.plot(pt_range / 1E3, np.abs(errors[k]), label=k)
    ax.set_title("%s $\eta\in$ [%.2f-%.2f] - %s" % (particle, eta_min, eta_max, esmodel))
    ax.set_ylabel('relative resolution error')
    ax.set_xlabel('$p_T$ [GeV]')
    ax.set_ylim(0, 1.5)

    ax.legend(loc=0)
    ax.grid()
    filename = os.path.join(basedir, 'error_relresolution_%s_%s_eta%.2f-%.2f' % (esmodel, particle, eta_min, eta_max))
    for ext in 'png', 'pdf':
        fig.savefig(filename + "." + ext)
    plt.close(fig)


def main():
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option('--action', type='choice', action='store',
                      choices=['uA2MeV', 'zee', 'material', 'scale', 'cterm', 'all', 'gain', 'uniformity',
                               'sys_fixed_pt', 'sys_eta_slices', 'all_sys', 'resolution', 'fast', 'resolution-err'], default='all')
    (options, args) = parser.parse_args()

    basedir = 'plots'
    if not os.path.exists(basedir):
        os.makedirs(basedir)

    if options.action == 'resolution-err':
        logging.info("plotting resolution error")
        plot_resolution_error(basedir='plots')

    if options.action == 'zee' or options.action == 'all':
        logging.info("plotting scale systematics")
        plot_all_Zee_syst(etas=np.arange(-2.5, 2.5, 0.01), pt=100E3, basedir='plots')

    if options.action == 'all' or options.action == 'uA2MeV':
        logging.info("plotting uA2MeV")
        tool_es2015PRE_nouA2MeV = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE_nouA2MeV")
        tool_es2015PRE_nouA2MeV.setProperty("ESModel", "es2015PRE")
        tool_es2015PRE_nouA2MeV.setProperty("decorrelationModel", 'FULL_ETACORRELATED_v1')
        tool_es2015PRE_nouA2MeV.setProperty("int")("doSmearing", 0)
        tool_es2015PRE_nouA2MeV.setProperty("int")("use_uA2MeV_2015_first2weeks_correction", 0)
        tool_es2015PRE_nouA2MeV.initialize()

        tool_es2015PRE = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE")
        tool_es2015PRE.setProperty("ESModel", "es2015PRE")
        tool_es2015PRE.setProperty("decorrelationModel", 'FULL_ETACORRELATED_v1')
        tool_es2015PRE.setProperty("int")("doSmearing", 0)
        tool_es2015PRE.initialize()

        compare_two_tools_eta(tool_es2015PRE, tool_es2015PRE_nouA2MeV,
                              100E3, simulation=False,
                              name="uA2MeV", basedir=basedir,
                              title="with uA2MeV correction / without",
                              particle='electron')

        del tool_es2015PRE_nouA2MeV
        del tool_es2015PRE

    if options.action == 'all' or options.action == 'scale':
        logging.info("plotting scales")
        plot_all_scales(esmodels=("es2012c", "es2015PRE"),
                        labels=("2012", "new + temp"),
                        basedir=basedir, etas=np.arange(-2.5, 2.5, 0.01),
                        add_patch=True)
    if options.action == 'all' or options.action == 'fast':
        logging.info("plotting full / fast scale")
        check_fast(basedir, "es2015PRE")
        plot_all_scales(esmodels=("es2015PRE", ("es2015PRE", ("useAFII",), (True,), (bool,))),
                        labels=("2015PRE", "2015PRE FAST"),
                        basedir=basedir, etas=np.arange(-2.5, 2.5, 0.01))
    if options.action == 'all' or options.action == 'cterm':
        logging.info("plotting smearings")
        plot_all_cterms(esmodels=("es2012c", "es2012XX", "es2015PRE"), labels=("2012", "new", "new + temp"), basedir=basedir, etas=np.arange(-2.5, 2.5, 0.01))

    if options.action == 'gain' or options.action == "all":
        check_gain(basedir, 'es2015PRE')

    if options.action == 'uniformity' or options.action == 'all':
        check_uniformity(basedir, 'es2012c')
        check_uniformity(basedir, 'es2015PRE')

    if options.action == 'sys_fixed_pt' or options.action == 'all':
        tool_es2015PRE_notemp = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE_notemp")
        tool_es2015PRE_notemp.setProperty("ESModel", "es2015PRE")
        tool_es2015PRE_notemp.setProperty("decorrelationModel", 'FULL_ETACORRELATED_v1')
        tool_es2015PRE_notemp.setProperty("int")("doSmearing", 0)
        tool_es2015PRE_notemp.setProperty("int")("use_temp_correction201215", 0)
        tool_es2015PRE_notemp.initialize()

        compare_all_syst_fixed_pt(basedir, ('es2012c', tool_es2015PRE_notemp, 'es2015PRE'),
                                  names=('es2012c', 'es2015_no_temp', 'es2015PRE'),
                                  labels=('es2012c', '$\oplus$ 7/8 diff $\oplus$ 34/68 diff', '$\oplus$ LAr temperature'))

    if options.action == 'sys_eta_slices' or options.action == 'all':
        etabins = ((0., 0.6), (0.6, 1.45), (1.52, 1.7), (1.7, 1.9), (1.9, 2.5))
        for ptype in 'electron', 'unconverted', 'converted':
            plot_all_syst_eta_slice(etabins, netas=3, esmodel='es2012c',
                                    decorrelation='1NP_v1', ptype=ptype, basedir=basedir)
        for ptype in 'electron', 'unconverted', 'converted':
            plot_all_syst_eta_slice(etabins, netas=3, esmodel='es2012c',
                                    decorrelation='FULL_ETACORRELATED_v1', ptype=ptype, basedir=basedir)

    if options.action == 'material' or options.action == 'all':
        if not os.path.exists('material'):
            os.makedirs('material')
        etabins = ((0., 0.6), (0.6, 1.45), (1.52, 1.7), (1.7, 1.9), (1.9, 2.5))
        for ptype in 'electron', 'unconverted', 'converted':
            plot_all_syst_eta_slice(etabins, netas=3, esmodel='es2012c',
                                    decorrelation='FULL_ETACORRELATED_v1', ptype=ptype, basedir="material", only_material=True)

    if options.action == 'all_sys' or options.action == 'all':
        for ptype in 'electron', 'unconverted', 'converted':
            plot_all_syst(decorrelation="1NP_v1", ptype=ptype, basedir=basedir)
            plot_all_syst(decorrelation="FULL_ETACORRELATED_v1", ptype=ptype, basedir=basedir)
            plot_all_syst(decorrelation="FULL_v1", ptype=ptype, basedir=basedir)

    if options.action == "resolution" or options.action == 'all':
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool.setProperty("ESModel", 'es2015PRE')
        tool.initialize()
        pts = np.logspace(3.2, 6, 50)
        etas = np.linspace(-2.49, 2.49, 100)
        plot_resolution_eta_pt(basedir, tool, pts, etas, 'electron', title="resolution electron es2015PRE")
        plot_resolution_eta_pt(basedir, tool, pts, etas, 'converted', title="resolution converted es2015PRE")
        plot_resolution_eta_pt(basedir, tool, pts, etas, 'unconverted', title="resolution unconverted es2015PRE")

if __name__ == '__main__':
    ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
    main()
