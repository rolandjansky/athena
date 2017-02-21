#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import numpy as np
import logging
import os
from itertools import tee, izip, cycle, chain
import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.patches as patches

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
logging.basicConfig(level=logging.INFO)

plt.rcParams['image.cmap'] = 'coolwarm'  # RdBu_r

from matplotlib import rcParams
rcParams['font.family'] = 'sans-serif'
rcParams['mathtext.fontset'] = 'stixsans'
rcParams['mathtext.default'] = 'rm'
rcParams['font.sans-serif'] = 'helvetica, Helvetica, Nimbus Sans L, Mukti Narrow, FreeSans'
rcParams['figure.figsize'] = 8.75, 5.92
#rcParams['figure.facecolor'] = 'white'
rcParams['figure.subplot.bottom'] = 0.16
rcParams['figure.subplot.top'] = 0.95
rcParams['figure.subplot.left'] = 0.16
rcParams['figure.subplot.right'] = 0.95

# axes
rcParams['axes.labelsize'] = 20
rcParams['xtick.minor.visible'] = True
rcParams['ytick.minor.visible'] = True
rcParams['xtick.labelsize'] = 19
rcParams['xtick.major.size'] = 12
rcParams['xtick.minor.size'] = 6
rcParams['ytick.labelsize'] = 19
rcParams['ytick.major.size'] = 14
rcParams['ytick.minor.size'] = 7
rcParams['lines.markersize'] = 8
# rcParams['lines.markeredgewidth'] = 0. # not working, it changes other stuff

# legend
rcParams['legend.numpoints'] = 1
rcParams['legend.fontsize'] = 19
rcParams['legend.labelspacing'] = 0.3
#rcParams['legend.frameon'] = False

extensions = 'pdf', 'png'

def plot_ATLAS(fig, x, y, label='Internal', fontsize=20):
    l = fig.text(x, y, 'ATLAS', fontsize=fontsize, fontstyle='italic', fontweight='bold')
    def on_draw(event):
        x_right = l.get_window_extent().inverse_transformed(fig.transFigure).max[0]
        fig.text(x_right, y, " " + label, fontsize=fontsize)
        fig.canvas.mpl_disconnect(cid)
        return False
    cid = fig.canvas.mpl_connect('draw_event', on_draw)
    fig.canvas.draw()

fout = ROOT.TFile('output_plot.root', 'recreate')

def pairwise(iterable):
    "s -> (s0,s1), (s1,s2), (s2, s3), ..."
    a, b = tee(iterable)
    next(b, None)
    return izip(a, b)

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


def eval_eta_slice(tool, etas, pts, ptype, only_material=False, only_up=True):
    sys_set = ROOT.CP.SystematicSet()
    tool.applySystematicVariation(sys_set)
    nominal = eval_sys_eta_pt(tool, etas, pts, particle=ptype)
    all_syst = tool.recommendedSystematics()

    results = {}
    for sys in all_syst:
        sys_name = sys.name()
        if 'RESOLUTION' in sys_name:
            continue
        if '1down' in sys_name and only_up:
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


def beautify_sysname(sysname):
    d = {"EG_SCALE_PEDESTAL": "Pedestal",
         "EG_SCALE_L2GAIN": "L2 gain",
         "EG_SCALE_L1GAIN": "L1 gain",
         "EG_SCALE_PS": r"$\alpha_{PS}$",
         "EG_SCALE_S12": r"$\alpha_{1/2}$",
         "EG_SCALE_ZEESYST": r"$Z\to ee$ calibration (sys)",
         "PH_SCALE_LEAKAGEUNCONV": "Leakage unconverted",
         "EG_SCALE_MATID": "ID material",
         "EG_SCALE_MATCRYO": "Cryo material",
         "EG_SCALE_CONVEFFICIENCY": "Conversion efficiency",
         "EG_SCALE_MATCALO": "Calo material",
         "EG_SCALE_ZEE_STAT": r"$Z\to ee$ (stat)",
         "EG_SCALE_G4": "Geant4",
         "EG_SCALE_LEAKAGECONV": "Leakage converted",
         "PH_SCALE_CONV_RATIUS": "Conversion radius",
         "PH_SCALE_CONVFAKERATE": "Conversion fake rate",
         "EG_SCALE_LARCALIB": r"$\alpha_{1/2}$ $\mu\to e$ extrap.",
         "EG_SCALE_E4SCINTILLATOR": "Scintillators",
         "EG_SCALE_LARTEMPERATURE_EXTRA2016PRE": r"Temp. 2015 $\to$ 2016",
         "EG_SCALE_LARCALIB_EXTRA2015PRE": r"$\alpha_{1/2}$ Run 1 $\to$ Run 2",
         }
    return d.get(sysname, sysname)

def beautify_particle(particle):
    d = {'electron': 'Electrons',
         'converted': 'Converted photons',
         'unconverted': 'Unconverted photons'}
    return d.get(particle, particle).capitalize()

def plot_all_syst_eta_slice(etabins, netas=3, esmodel='es2012c',
                            decorrelation='FULL_v1', ptype='unconverted',
                            basedir='plot', only_material=False, beautify_sysnames=False,
                            sys_paper_order=False, superimpose_all=False, skip_null_sys=False, yrange=None, only_up=True, debug=False, legend_outside=False, symmetrize_labels=False):
    tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
    tool.setProperty("ESModel", esmodel)
    tool.setProperty("decorrelationModel", decorrelation)
    tool.setProperty("int")("doSmearing", 0)
    if debug:
        tool.msg().setLevel(0)

    tool.initialize()

    if superimpose_all:
        tool_all = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_all")
        tool_all.setProperty("ESModel", esmodel)
        tool_all.setProperty("decorrelationModel", "1NP_v1")
        tool_all.setProperty("int")("doSmearing", 0)
        tool_all.initialize()


    pts = np.logspace(np.log10(5E3), 6, 100)

    # compute the eta-inclusive one, just to sort
    results = eval_eta_slice(tool, np.linspace(-2.5, 2.5, 20),
                             np.linspace(10E3, 200E3, 10),       # use this range to mimic the range in the paper
                             ptype, only_material)
    sorted_sys_name = sorted(list(results), key=lambda k: -np.max(np.abs(results[k])))
    if skip_null_sys:
        sorted_sys_name = [sys_name for sys_name in sorted_sys_name if np.sum(results[sys_name]) != 0]
    if sys_paper_order:
        partitions = [["EG_SCALE_PS__1up", "EG_SCALE_S12__1up", "EG_SCALE_LARCALIB__1up", "EG_SCALE_L2GAIN__1up"],
                      ["EG_SCALE_MATID__1up", "EG_SCALE_MATCRYO__1up", "EG_SCALE_MATCALO__1up", "EG_SCALE_ZEESYST__1up"]]
        if esmodel == "es2016PRE":
            partitions += [["EG_SCALE_PEDESTAL__1up", "EG_SCALE_LARCALIB_EXTRA2015PRE__1up", "EG_SCALE_LARTEMPERATURE_EXTRA2016PRE__1up", "EG_SCALE_E4SCINTILLATOR__1up"]]

        remainers = [item for item in sorted_sys_name if item not in list(np.array(partitions).flatten())]
        partitions = chain(partitions, partition(remainers, 4))
    else:
        partitions = partition(sorted_sys_name, 4)
    partitions = list(partitions)

    for etamin, etamax in etabins:
        etas = np.linspace(etamin, etamax, netas)
        results = eval_eta_slice(tool, etas, pts, ptype, only_material, only_up=only_up)

        if superimpose_all:
            results_all = eval_eta_slice(tool_all, etas, pts, ptype, only_up=False)

        for ip, p in enumerate(partitions):
            f, ax = plt.subplots()
            if superimpose_all:
                #max_up_down = np.max(np.abs([results_all["EG_SCALE_ALL__1down"],
                                             #results_all["EG_SCALE_ALL__1up"]]), axis=0)
                #ax.fill_between(pts / 1E3, -max_up_down * 100., max_up_down * 100,
                                #color='0.8', label='total')
                #ax.plot(pts/1E3, np.sqrt(np.sum([r ** 2 for r in results.values()], axis=0)) * 100., 'r:')
                if only_up:
                    ax.fill_between(pts / 1E3, -results_all["EG_SCALE_ALL__1up"] * 100., results_all["EG_SCALE_ALL__1up"] * 100,
                                    color='0.8', label='Total')
                else:
                    ax.fill_between(pts / 1E3, results_all["EG_SCALE_ALL__1down"] * 100., results_all["EG_SCALE_ALL__1up"] * 100,
                                    color='0.8', label='Total')
            for sys_name in p:
                if sys_name not in results:
                    continue  ## TODO: FIXME
                r = results[sys_name]
                r[np.isnan(r)] = 0
                sys_label = beautify_sysname(sys_name.replace('__1up', '')) if beautify_sysnames else sys_name
                if not only_up:
                    sys_label += " UP"
                ax.semilogx(pts / 1E3, r * 100., label=sys_label)

            if not only_up:
                ax.set_prop_cycle(None)
                for sys_name in p:
                    sys_name = sys_name.replace('up', 'down')
                    if sys_name not in results:
                        continue  ## TODO: FIXME
                    r = results[sys_name]
                    r[np.isnan(r)] = 0
                    sys_label = beautify_sysname(sys_name.replace('__1down', '')) if beautify_sysnames else sys_name
                    sys_label += ' DOWN'
                    ax.semilogx(pts / 1E3, r * 100., label=sys_label, linestyle='--')

            ax.set_xlabel('$E_T$ [GeV]', ha='right', x=1., fontsize=22)
            ax.set_ylabel('Signed uncertainty [%]', ha='right', y=1., fontsize=22)
            ax.tick_params(axis='both', which='major', labelsize=20)

            ax.axis('tight')
            if yrange is None:
                if only_material:
                    vmax = 1.
                else:
                    vmax = max(2, np.max(np.abs(ax.get_ylim())))
                ax.set_ylim(-vmax, vmax)
            else:
                ax.set_ylim(*yrange)

            if legend_outside:
                ax.legend(bbox_to_anchor=(0., 1., 1, 0.2), mode='expand', borderaxespad=0.,
                          loc=3, frameon=True, fontsize=17 if only_up else 14, borderpad=1, ncol=1 if only_up else 2)
                f.subplots_adjust(top=0.65)
                plot_ATLAS(f, 0.2, 0.58, label='Internal')
                f.text(0.2, 0.2, beautify_particle(ptype), transform=ax.transAxes, fontsize=16)
                f.text(0.2, 0.25, r'$%.2f < \eta < %.2f$' % (etamin, etamax), transform=ax.transAxes, fontsize=16)
            else:
                ax.legend(loc=1, frameon=False, fontsize=17 if only_up else 14, borderpad=1, ncol=1 if only_up else 2)

                plot_ATLAS(f, 0.2, 0.85, label='Internal', fontsize=21)
                f.text(0.2, 0.80, beautify_particle(ptype), transform=ax.transAxes, fontsize=16)
                f.text(0.2, 0.75, r'$%.2f < \eta < %.2f$' % (etamin, etamax), transform=ax.transAxes, fontsize=16)

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
        logging.info("plotting sys %s %s", sys_name, ptype)

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


def plot_resolution_error(basedir, **kwargs):
    esmodels = kwargs["esmodels"] or ('es2012c', 'es2015PRE', 'es2015PRE_res_improved', 'es2016PRE')
    if kwargs["eta_bins"] is not None:
        eta_bins = pairwise(kwargs["eta_bins"])
    else:
        eta_bins = (0, 0.4), (0.4, 0.6), (0.6, 1.37), (1.52, 1.8), (1.8, 2.37)
    for esmodel in esmodels:
        logging.debug("plotting resolution error for %s" % esmodel)
        tool = ROOT.AtlasRoot.egammaEnergyCorrectionTool()
        if kwargs['debug']:
            tool.msg().setLevel(0)
        tool.setESModel(getattr(ROOT.egEnergyCorr, esmodel))
        tool.initialize()
        for particle in ('electron', 'converted', 'unconverted'):
            for eta_min, eta_max in eta_bins:
                plot_resolution_error_bin(eta_min, eta_max, particle, esmodel, basedir, tool=tool, **kwargs)


def plot_resolution_error_bin(eta_min, eta_max, particle, esmodel, basedir, tool=None, **kwargs):
    if tool is None:
        tool = ROOT.AtlasRoot.egammaEnergyCorrectionTool()
        tool.setESModel(getattr(ROOT.egEnergyCorr, esmodel))
        tool.initialize()

    ptype = {"electron": ROOT.PATCore.ParticleType.Electron,
             "unconverted": ROOT.PATCore.ParticleType.UnconvertedPhoton,
             "converted": ROOT.PATCore.ParticleType.ConvertedPhoton
             }[particle]

    variations_name_up = {"Zsmearing up": ROOT.egEnergyCorr.Resolution.ZSmearingUp,
                          "sampling up": ROOT.egEnergyCorr.Resolution.SamplingTermUp,
                          "material ID up": ROOT.egEnergyCorr.Resolution.MaterialIDUp,
                          "material calo up": ROOT.egEnergyCorr.Resolution.MaterialCaloUp,
                          "material gap up": ROOT.egEnergyCorr.Resolution.MaterialGapUp,
                          "material cryo up": ROOT.egEnergyCorr.Resolution.MaterialCryoUp,
                          "pileup up": ROOT.egEnergyCorr.Resolution.PileUpUp,
                          "material ibl up": ROOT.egEnergyCorr.Resolution.MaterialIBLUp,
                          "material pp0 up": ROOT.egEnergyCorr.Resolution.MaterialPP0Up,
                          "all up": ROOT.egEnergyCorr.Resolution.AllUp}
    variations_name_down = {"Zsmearing down": ROOT.egEnergyCorr.Resolution.ZSmearingDown,
                            "sampling down": ROOT.egEnergyCorr.Resolution.SamplingTermDown,
                            "material ID down": ROOT.egEnergyCorr.Resolution.MaterialIDDown,
                            "material calo down": ROOT.egEnergyCorr.Resolution.MaterialCaloDown,
                            "material gap down": ROOT.egEnergyCorr.Resolution.MaterialGapDown,
                            "material cryo down": ROOT.egEnergyCorr.Resolution.MaterialCryoDown,
                            "pileup down": ROOT.egEnergyCorr.Resolution.PileUpDown,
                            "material ibl down": ROOT.egEnergyCorr.Resolution.MaterialIBLDown,
                            "material pp0 down": ROOT.egEnergyCorr.Resolution.MaterialPP0Down,

                            "all down": ROOT.egEnergyCorr.Resolution.AllDown}
#                 ^    ^
    all_errors = [{}, {}]  # up and down
#                  ,--,

    pt_range = np.linspace(10E3, 800E3, 100)
    nsamples_eta = kwargs['super_sampling_eta'] or 5
    eta_range = np.linspace(eta_min, eta_max, nsamples_eta + 2)[1:-1]

    for side, errors, variations_name in zip(("up", "down"), all_errors, (variations_name_up, variations_name_down)):
        quadrature_sum = np.zeros((len(pt_range), len(eta_range)))
        for variation_name, variation_id in variations_name.iteritems():
            errors_var_pt_eta = np.zeros((len(pt_range), len(eta_range)))
            for ipt, pt in enumerate(pt_range):
                for ieta, eta in enumerate(eta_range):
                    energy = pt * np.cosh(eta)
                    logging.debug("evaluating systematics %s in eta=%.2f pt=%.2f on resolution" % (variation_name, eta, pt))
                    errors_var_pt_eta[ipt, ieta] = tool.getResolutionError(energy, eta, eta, ptype, variation_id)
            errors[variation_name] = errors_var_pt_eta.mean(axis=1)  # average over different eta points inside the eta-bin

    sorted_keys_up = sorted(variations_name_up.keys(), key=lambda x: np.abs(all_errors[0][x].mean()))
    totals = [np.sqrt(np.sum(e ** 2 for k, e in errors.iteritems() if "all " not in k)) for errors in all_errors]

    fig, ax = plt.subplots()
    ax.fill_between(pt_range / 1E3, all_errors[0]['all up'], all_errors[1]['all down'], color='0.8')
#    ax.fill_between(pt_range / 1E3, totals[0], -totals[-1], color='0.8')
#    totals[1] *= -1

    colors = ['b', 'g', 'r', 'c', 'm', 'y', 'violet', 'pink', 'orange']
    props = mpl.rcParams["axes.prop_cycle"]

    for side, errors, total in zip(('up', 'down'), all_errors, totals):
        #ax.plot(pt_range / 1E3, total, label='sum %s' % side, color='k')
        ax.plot(pt_range / 1E3, errors['all %s' % side], 'k', label='all %s' % side)
        colors_iter = cycle(colors)
        for k in sorted(errors.keys()):
            if 'all' in k:
                continue
            v = errors[k]
            linestyle = "-"
            if "down" in k:
                linestyle = '--'
            if "all" in k:
                linestyle = ":"
            ax.plot(pt_range / 1E3, v, linestyle, label=k, color=next(colors_iter))


    ax.set_title("%s $\eta\in$ [%.2f-%.2f] - %s" % (particle, eta_min, eta_max, esmodel))
    ax.set_ylabel('relative resolution error', ha='right', y=1.)
    ax.set_xlabel('$p_T$ [GeV]', ha='right', x=1.)
    ax.set_ylim(-0.7, 0.7)
    fig.subplots_adjust(bottom=0.35)

    ax.legend(loc=3, bbox_to_anchor=(0., -0.5, 1, 0.2), mode="expand",
              ncol=4, borderaxespad=0., fontsize=10)

    #plot_ATLAS(fig, 0.6, 0.75, 'Internal')

    ax.grid()
    filename = os.path.join(basedir, 'error_relresolution_%s_%s_eta%.2f-%.2f' % (esmodel, particle, eta_min, eta_max))
    for ext in 'png', 'pdf':
        fig.savefig(filename + "." + ext)
    plt.close(fig)


def main():
    from argparse import ArgumentParser, RawDescriptionHelpFormatter
    parser = ArgumentParser(formatter_class=RawDescriptionHelpFormatter,
                            epilog="to produce paper plots for systematics: python plot.py sys_eta_slices --beautify-sysnames --sys-paper-order --superimpose-all --skip-null-sys --esmodels es2016PRE --yrange -1 2.0 --symmetrize-labels")
    parser.add_argument('action', choices=['uA2MeV', 'zee', 'material', 'scale', 'cterm', 'all', 'gain', 'uniformity',
                                           'sys_fixed_pt', 'sys_eta_slices', 'all_sys', 'resolution', 'fast', 'resolution-err'],
                        default='all', help='what to do')
    parser.add_argument('--esmodels', nargs='*', help='esmodel to consider')
    parser.add_argument('--beautify-sysnames', action='store_true')
    parser.add_argument('--eta-bins', nargs='*', type=float, help='edges of the eta-bins')
    parser.add_argument('--pt-bins', nargs='*', type=float, help='edges of the pt-bins')
    parser.add_argument('--super-sampling-eta', type=int, default=5, help='how many point to average inside a bin')
    parser.add_argument('--sys-paper-order', action='store_true', default='use the same order of systeamatics as in run1 paper')
    parser.add_argument('--debug', action='store_true')
    parser.add_argument('--superimpose-all', action='store_true', help='superimpose sum of systematics')
    parser.add_argument('--skip-null-sys', action='store_true', help='do not plot null systematics')
    parser.add_argument('--yrange', nargs=2, type=float)
    parser.add_argument('--add-down', action='store_true', help='plot also the down systematics')
    parser.add_argument('--legend-outside', action='store_true', help='draw the legend outside the plot')
    parser.add_argument('--symmetrize-labels', action='store_true')
    args = parser.parse_args()

    if args.debug:
        logging.basicConfig(level=logging.DEBUG)

    basedir = 'plots'
    if not os.path.exists(basedir):
        os.makedirs(basedir)

    if args.action == 'resolution-err':
        logging.info("plotting resolution error")
        plot_resolution_error(basedir='plots', **vars(args))

    if args.action == 'zee' or args.action == 'all':
        logging.info("plotting scale systematics")
        plot_all_Zee_syst(etas=np.arange(-2.5, 2.5, 0.01), pt=100E3, basedir='plots')

    if args.action == 'all' or args.action == 'uA2MeV':
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

    if args.action == 'all' or args.action == 'scale':
        logging.info("plotting scales")
        plot_all_scales(esmodels=("es2012c", "es2015PRE"),
                        labels=("2012", "new + temp"),
                        basedir=basedir, etas=np.arange(-2.5, 2.5, 0.01),
                        add_patch=True)
    if args.action == 'all' or args.action == 'fast':
        logging.info("plotting full / fast scale")
        check_fast(basedir, "es2015PRE")
        plot_all_scales(esmodels=("es2015PRE", ("es2015PRE", ("useAFII",), (True,), (bool,))),
                        labels=("2015PRE", "2015PRE FAST"),
                        basedir=basedir, etas=np.arange(-2.5, 2.5, 0.01))
    if args.action == 'all' or args.action == 'cterm':
        logging.info("plotting smearings")
        plot_all_cterms(esmodels=("es2012c", "es2012XX", "es2015PRE"), labels=("2012", "new", "new + temp"), basedir=basedir, etas=np.arange(-2.5, 2.5, 0.01))

    if args.action == 'gain' or args.action == "all":
        check_gain(basedir, 'es2015PRE')

    if args.action == 'uniformity' or args.action == 'all':
        check_uniformity(basedir, 'es2012c')
        check_uniformity(basedir, 'es2015PRE')

    if args.action == 'sys_fixed_pt' or args.action == 'all':
        tool_es2015PRE_notemp = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE_notemp")
        tool_es2015PRE_notemp.setProperty("ESModel", "es2015PRE")
        tool_es2015PRE_notemp.setProperty("decorrelationModel", 'FULL_ETACORRELATED_v1')
        tool_es2015PRE_notemp.setProperty("int")("doSmearing", 0)
        tool_es2015PRE_notemp.setProperty("int")("use_temp_correction201215", 0)
        tool_es2015PRE_notemp.initialize()

        compare_all_syst_fixed_pt(basedir, ('es2012c', tool_es2015PRE_notemp, 'es2015PRE'),
                                  names=('es2012c', 'es2015_no_temp', 'es2015PRE'),
                                  labels=('es2012c', '$\oplus$ 7/8 diff $\oplus$ 34/68 diff', '$\oplus$ LAr temperature'))

    if args.action == 'sys_eta_slices' or args.action == 'all':
        etabins = ((0., 0.6), (0.6, 1.45), (1.52, 1.7), (1.7, 1.9), (1.9, 2.5), (1.4, 1.6))
        for ptype in 'electron', 'unconverted', 'converted':
            logging.debug("plot all sys FULL_ETACORRELATED_v1 eta slice %s" % ptype)
            plot_all_syst_eta_slice(etabins, netas=args.super_sampling_eta, esmodel=args.esmodels[0],
                                    decorrelation='FULL_ETACORRELATED_v1', ptype=ptype, basedir=basedir,
                                    beautify_sysnames=args.beautify_sysnames, sys_paper_order=args.sys_paper_order,
                                    superimpose_all=args.superimpose_all, skip_null_sys=args.skip_null_sys, yrange=args.yrange, debug=args.debug,
                                    only_up=not args.add_down, symmetrize_labels=args.symmetrize_labels,
                                    legend_outside=args.legend_outside)
        for ptype in 'electron', 'unconverted', 'converted':
            logging.debug("plot all sys 1NP_v1 eta slice %s" % ptype)
            plot_all_syst_eta_slice(etabins, netas=args.super_sampling_eta, esmodel=args.esmodels[0],
                                    decorrelation='1NP_v1', ptype=ptype, basedir=basedir,
                                    beautify_sysnames=args.beautify_sysnames, sys_paper_order=args.sys_paper_order, yrange=args.yrange, debug=args.debug, only_up=not args.add_down,
                                    legend_outside=args.legend_outside, symmetrize_labels=args.symmetrize_labels)


    if args.action == 'material' or args.action == 'all':
        if not os.path.exists('material'):
            os.makedirs('material')
        etabins = ((0., 0.6), (0.6, 1.45), (1.52, 1.7), (1.7, 1.9), (1.9, 2.5))
        for ptype in 'electron', 'unconverted', 'converted':
            plot_all_syst_eta_slice(etabins, netas=args.super_sampling_eta, esmodel=args.esmodels[0],
                                    decorrelation='FULL_ETACORRELATED_v1', ptype=ptype, basedir="material",
                                    only_material=True, beautify_sysnames=args.beautify_sysnames,
                                    sys_paper_order=args.sys_paper_order, skip_null_sys=args.skip_null_sys, yrange=args.yrange, debug=args.debug, only_up=not args.add_down,
                                    legend_outside=args.legend_outside, symmetrize_labels=args.symmetrize_labels)

    if args.action == 'all_sys' or args.action == 'all':
        for ptype in 'electron', 'unconverted', 'converted':
            plot_all_syst(decorrelation="1NP_v1", ptype=ptype, basedir=basedir)
            plot_all_syst(decorrelation="FULL_ETACORRELATED_v1", ptype=ptype, basedir=basedir)
            plot_all_syst(decorrelation="FULL_v1", ptype=ptype, basedir=basedir)

    if args.action == "resolution" or args.action == 'all':
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
