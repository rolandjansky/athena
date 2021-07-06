#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

""" Produce plots about egamma calibration systematics and corrections """

import logging
import os
import time
from fnmatch import fnmatch
from functools import wraps
from itertools import chain, cycle, product, tee

import colorlog
import matplotlib as mpl
mpl.use("Agg")

import matplotlib.lines as mlines
import matplotlib.patches as patches
import matplotlib.pyplot as plt
import numpy as np
import ROOT
import seaborn as sns
import tqdm
from matplotlib.ticker import MaxNLocator
from matplotlib import rcParams


ROOT.PyConfig.IgnoreCommandLineOptions = True

plt.rcParams["image.cmap"] = "coolwarm"  # RdBu_r

rcParams["font.family"] = "sans-serif"
rcParams["mathtext.fontset"] = "stixsans"
rcParams["mathtext.default"] = "rm"
rcParams["font.sans-serif"] = "helvetica, Helvetica, Nimbus Sans L, Mukti Narrow, FreeSans"

# axes
rcParams["axes.labelsize"] = 20
rcParams["xtick.minor.visible"] = True
rcParams["ytick.minor.visible"] = True
rcParams["xtick.direction"] = "in"
rcParams["ytick.direction"] = "in"
rcParams["xtick.labelsize"] = 19
rcParams["xtick.major.size"] = 12
rcParams["xtick.minor.size"] = 6
rcParams["ytick.labelsize"] = 19
rcParams["ytick.major.size"] = 14
rcParams["ytick.minor.size"] = 7
rcParams["lines.markersize"] = 8
rcParams["ytick.right"] = True
rcParams["xtick.top"] = True
# rcParams['lines.markeredgewidth'] = 0. # not working, it changes other stuff

# legend
rcParams["legend.numpoints"] = 1
rcParams["legend.fontsize"] = 19
rcParams["legend.labelspacing"] = 0.3
# rcParams['legend.frameon'] = False

extensions = "pdf", "png"


def check_status_code(func):
    def wrapper(self, *args):
        status = func(self, *args)
        if not status.isSuccess():
            raise ValueError("status is not success")
        return status

    return wrapper


ROOT.CP.EgammaCalibrationAndSmearingTool.initialize = check_status_code(
    ROOT.CP.EgammaCalibrationAndSmearingTool.initialize
)


def timed(method):
    @wraps(method)
    def timed(*args, **kw):
        ts = time.time()
        result = method(*args, **kw)
        te = time.time()

        log.info("function %s run in  %d second", method.__name__, te - ts)
        return result

    return timed


def plot_ATLAS(fig, x, y, label="Internal", fontsize=20):
    label = fig.text(x, y, "ATLAS", fontsize=fontsize, fontstyle="italic", fontweight="bold")

    def on_draw(event):
        x_right = label.get_window_extent().transformed(fig.transFigure.inverted()).max[0]
        fig.text(x_right, y, " " + label, fontsize=fontsize)
        fig.canvas.mpl_disconnect(cid)
        return False

    cid = fig.canvas.mpl_connect("draw_event", on_draw)
    fig.canvas.draw()


fout = ROOT.TFile("output_plot.root", "recreate")


def pairwise(iterable):
    "s -> (s0,s1), (s1,s2), (s2, s3), ..."
    a, b = tee(iterable)
    next(b, None)
    return zip(a, b)


def divide_square(n, horizontal=True):
    """ divide a figure into a square number of subplots in an optimized way """
    if horizontal:
        x = np.ceil(np.sqrt(n))
        y = np.floor(np.sqrt(n))
        if (x * y) < n:
            x += 1
    else:
        x = np.floor(np.sqrt(n))
        y = np.ceil(np.sqrt(n))
        if (x * y) < n:
            y += 1
    return int(x), int(y)


def histo2data(histo):
    xs = []
    ys = []
    exs = []
    eys = []
    for ibin in range(1, histo.GetNbinsX() + 1):
        xs.append(histo.GetBinCenter(ibin))
        ys.append(histo.GetBinContent(ibin))
        eys.append(histo.GetBinError(ibin))
        exs.append(0.5 * histo.GetBinWidth(ibin))
    return np.array(xs), np.array(exs), np.array(ys), np.array(eys)


def systematics_from_tool(tool, only_scale=True, only_resolution=False, only_up=True):
    """ return name of the systematics """
    _ = tool.recommendedSystematics()
    for sys in _:
        sys_name = sys.name()
        if only_scale and "RESOLUTION" in sys_name:
            continue
        if only_resolution and "SCALE" in sys_name:
            continue
        if only_up and "1down" in sys_name:
            continue
        yield sys


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
        for e in np.arange(1e3, 1000e3, 100e3):
            photon = factory.create_photon(eta, 0.1, e, 0)
            yield photon
    store.clear()


def calibrate_eta_pt(tool, etas, pts, simulation=True, particle="unconverted"):
    log.debug("creating TEvent and EgammaFactory")
    event = ROOT.xAOD.TEvent()
    factory = ROOT.EgammaFactory()
    result = np.ones((len(pts), len(etas)))
    ei = factory.create_eventinfo(simulation, 266904)
    assert ei.eventType(ROOT.xAOD.EventInfo.IS_SIMULATION) == simulation
    log.debug("looping")
    for ieta, eta in enumerate(etas):
        for ipt, pt in enumerate(pts):
            if particle == "unconverted":
                p = factory.create_photon(eta, 0.1, pt * np.cosh(eta), 0)
            elif particle == "converted":
                p = factory.create_photon(eta, 0.1, pt * np.cosh(eta), 100)
            elif particle == "electron":
                p = factory.create_electron(eta, 0.1, pt * np.cosh(eta))
            else:
                raise ValueError()
            result[ipt, ieta] = tool.getEnergy(p, ei)
    log.debug("deleting event")
    del event
    log.debug("returning result %s", result)
    return result


def eval_sys_eta_phi(tool, etas, phis, pt, simulation, particle="unconverted"):
    event = ROOT.xAOD.TEvent()
    factory = ROOT.EgammaFactory()
    result = np.ones((len(etas), len(phis)))
    ei = factory.create_eventinfo(simulation, 100000)
    for ieta, eta in enumerate(etas):
        for iphi, phi in enumerate(phis):
            if particle == "unconverted":
                p = factory.create_photon(eta, phi, pt * np.cosh(eta), 0)
            elif particle == "converted":
                p = factory.create_photon(eta, phi, pt * np.cosh(eta), 100)
            elif particle == "electron":
                p = factory.create_electron(eta, phi, pt * np.cosh(eta))
            else:
                raise ValueError()
            result[ieta, iphi] = tool.getEnergy(p, ei)
    del event
    return result


def eval_eta_slice(tool, etas, pts, ptype, only_material=False, only_up=True):
    sys_set = ROOT.CP.SystematicSet()
    tool.applySystematicVariation(sys_set)
    nominal = calibrate_eta_pt(tool, etas, pts, particle=ptype)
    all_syst = tool.recommendedSystematics()

    results = {}
    for sys in all_syst:
        sys_name = sys.name()
        if "RESOLUTION" in sys_name:
            continue
        if "1down" in sys_name and only_up:
            continue
        if only_material:
            if "MAT" not in sys_name:
                continue
        log.info("computing sys %s, %d eta samplings", sys_name, len(etas))
        sys_set = ROOT.CP.SystematicSet()
        sys_set.insert(sys)
        tool.applySystematicVariation(sys_set)

        sys = calibrate_eta_pt(tool, etas, pts, particle=ptype)

        ratio = sys / nominal

        ratio = np.average(ratio - 1, axis=1)  # average the supersampling
        results[sys_name] = ratio
    return results


def beautify_sysname(sysname):
    d = {
        "EG_SCALE_PEDESTAL": "Pedestal",
        "EG_SCALE_L2GAIN": "L2 gain",
        "EG_SCALE_L1GAIN": "L1 gain",
        "EG_SCALE_PS": r"$\alpha_{PS}$",
        "EG_SCALE_S12": r"$\alpha_{1/2}$",
        "EG_SCALE_S12EXTRALASTETABINRUN2": r"$\alpha_{1/2}$ [2.4-2.5]",
        "EG_SCALE_ZEESYST": r"$Z\to ee$ cal. (sys)",
        "EG_SCALE_ZEESTAT": r"$Z\to ee$ cal. (stat)",
        "PH_SCALE_LEAKAGEUNCONV": "Leakage unconverted",
        "EG_SCALE_MATID": "ID material",
        "EG_SCALE_MATCRYO": "Cryo material",
        "EG_SCALE_MATPP0": "PP0 material",
        "EG_SCALE_WTOTS1": "$w_{tots1}$ non-lin.",
        "EG_SCALE_CONVEFFICIENCY": "Conversion efficiency",
        "EG_SCALE_MATCALO": "Calo material",
        "EG_SCALE_ZEE_STAT": r"$Z\to ee$ (stat)",
        "EG_SCALE_G4": "Geant4",
        "PH_SCALE_LEAKAGECONV": "Leakage converted",
        "PH_SCALE_CONVRADIUS": "Conv. radius",
        "PH_SCALE_CONVFAKERATE": "Conv. fake rate",
        "PH_SCALE_CONVEFFICIENCY": "Conv. efficiency",
        "EG_SCALE_LARCALIB": r"$\alpha_{1/2}$ $\mu\to e$ extrap.",
        "EG_SCALE_E4SCINTILLATOR": "Scintillators",
        "EG_SCALE_LARTEMPERATURE_EXTRA2016PRE": r"Temp. 2015 $\to$ 2016",
        "EG_SCALE_LARCALIB_EXTRA2015PRE": r"$\alpha_{1/2}$ Run 1 $\to$ Run 2",
    }
    return d.get(sysname, sysname)


def beautify_particle(particle):
    d = {
        "electron": "Electrons",
        "converted": "Converted photons",
        "unconverted": "Unconverted photons",
    }
    return d.get(particle, particle).capitalize()


def plot_all_syst_eta_slice(
    etabins,
    supersampling_eta=3,
    esmodel="es2012c",
    decorrelation="FULL_v1",
    ptype="unconverted",
    pts=np.logspace(np.log10(5e3), 6, 100),
    basedir="plot",
    only_material=False,
    beautify_sysnames=False,
    sys_order=None,
    superimpose_all=False,
    skip_null_sys=False,
    min_sys=-0.02,
    max_sys=0.02,
    only_up=True,
    debug=False,
    legend_outside=False,
    symmetrize_labels=False,
    log_x=False,
    plot_qsum=False,
    abs_sys=False,
    atlas_label="Internal",
):
    tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
    tool.setProperty("ESModel", esmodel)
    tool.setProperty("decorrelationModel", decorrelation)
    tool.setProperty[bool]("doSmearing", 0)
    log.warning("setting randomRunNumber to 297730")
    tool.setProperty[int]("randomRunNumber", 297730)
    if debug:
        tool.msg().setLevel(0)

    tool.initialize()

    if superimpose_all:
        tool_all = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_all")
        tool_all.setProperty("ESModel", esmodel)
        tool_all.setProperty("decorrelationModel", "1NP_v1")
        tool_all.setProperty[bool]("doSmearing", 0)
        log.warning("setting randomRunNumber to 297730")
        tool_all.setProperty[int]("randomRunNumber", 297730)
        tool_all.initialize()

    # compute the eta-inclusive one, just to sort
    log.info("compute sys inclusively, just to sort sys by importance")
    results = eval_eta_slice(
        tool,
        np.linspace(-2.5, 2.5, 20),
        np.linspace(10e3, 200e3, 10),  # use this range to mimic the range in the paper
        ptype,
        only_material,
    )
    sorted_sys_name = sorted(list(results), key=lambda k: -np.max(np.abs(results[k])))
    if skip_null_sys:
        sorted_sys_name = [
            sys_name for sys_name in sorted_sys_name if np.sum(results[sys_name]) != 0
        ]
    if sys_order is not None:
        if sys_order == "paper_run1":
            partitions = [
                [
                    "EG_SCALE_PS__1up",
                    "EG_SCALE_S12__1up",
                    "EG_SCALE_LARCALIB__1up",
                    "EG_SCALE_L2GAIN__1up",
                ],
                [
                    "EG_SCALE_MATID__1up",
                    "EG_SCALE_MATCRYO__1up",
                    "EG_SCALE_MATCALO__1up",
                    "EG_SCALE_ZEESYST__1up",
                ],
            ]
            if esmodel == "es2016PRE":
                partitions += [
                    [
                        "EG_SCALE_PEDESTAL__1up",
                        "EG_SCALE_LARCALIB_EXTRA2015PRE__1up",
                        "EG_SCALE_LARTEMPERATURE_EXTRA2016PRE__1up",
                        "EG_SCALE_E4SCINTILLATOR__1up",
                    ]
                ]
        elif sys_order == "paper_run2":
            partitions = [
                [
                    "EG_SCALE_PS__1up",
                    "EG_SCALE_S12__1up",
                    "EG_SCALE_LARCALIB__1up",
                    "EG_SCALE_LARCALIB_EXTRA2015PRE__1up",
                    "EG_SCALE_S12EXTRALASTETABINRUN2",
                ],
                [
                    "EG_SCALE_MATID__1up",
                    "EG_SCALE_MATCRYO__1up",
                    "EG_SCALE_MATCALO__1up",
                    "EG_SCALE_MATPP0__1up",
                    "EG_SCALE_ZEESYST__1up",
                ],
                [
                    "EG_SCALE_L2GAIN__1up",
                    "EG_SCALE_WTOTS1__1up",
                    "EG_SCALE_PEDESTAL__1up",
                    "EG_SCALE_E4SCINTILLATOR__1up",
                ],
                [
                    "PH_SCALE_CONVEFFICIENCY__1up",
                    "PH_SCALE_CONVFAKERATE__1up",
                    "PH_SCALE_CONVRADIUS__1up",
                    "PH_SCALE_LEAKAGECONV__1up",
                    "PH_SCALE_LEAKAGEUNCONV__1up",
                ],
            ]
        else:
            raise ValueError("cannot understand sys_order = %s" % sys_order)
        flat_list = [item for sublist in partitions for item in sublist]
        remainers = [item for item in sorted_sys_name if item not in flat_list]
        partitions = chain(partitions, partition(remainers, 4))
    else:
        partitions = partition(sorted_sys_name, 4)
    partitions = list(partitions)

    for etamin, etamax in tqdm.tqdm(etabins):
        log.info("plotting eta range %.2f %.2f", etamin, etamax)
        etas = np.linspace(etamin, etamax, supersampling_eta + 2)[1:-1]
        results = eval_eta_slice(tool, etas, pts, ptype, only_material, only_up=only_up)
        result_values = np.array(list(results.values()))
        qsum = np.sqrt((result_values ** 2).sum(axis=0))

        if superimpose_all:
            results_all = eval_eta_slice(tool_all, etas, pts, ptype, only_up=False)

        for ip, p in enumerate(partitions):
            log.info("plotting %d/%d", ip + 1, len(partitions))

            f, ax = plt.subplots()
            if superimpose_all:
                # max_up_down = np.max(np.abs([results_all["EG_SCALE_ALL__1down"],
                # results_all["EG_SCALE_ALL__1up"]]), axis=0)
                # ax.fill_between(pts / 1E3, -max_up_down * 100., max_up_down * 100,
                # color='0.8', label='total')
                # ax.plot(pts/1E3, np.sqrt(np.sum([r ** 2 for r in results.values()], axis=0)) * 100., 'r:')
                if only_up:
                    if abs_sys:
                        ax.fill_between(
                            pts / 1e3,
                            0,
                            results_all["EG_SCALE_ALL__1up"] * 100,
                            color="0.8",
                            label="Total",
                        )
                    else:
                        ax.fill_between(
                            pts / 1e3,
                            -results_all["EG_SCALE_ALL__1up"] * 100.0,
                            results_all["EG_SCALE_ALL__1up"] * 100,
                            color="0.8",
                            label="Total",
                        )
                else:
                    ax.fill_between(
                        pts / 1e3,
                        results_all["EG_SCALE_ALL__1down"] * 100.0,
                        results_all["EG_SCALE_ALL__1up"] * 100,
                        color="0.8",
                        label="Total",
                    )
            for isys, sys_name in enumerate(p):
                color = "C%d" % isys  # TODO: fix, use cycle
                if sys_name not in results:
                    continue  # TODO: FIXME
                r = results[sys_name]
                r[np.isnan(r)] = 0
                sys_label = (
                    beautify_sysname(sys_name.replace("__1up", ""))
                    if beautify_sysnames
                    else sys_name
                )
                if not only_up:
                    sys_label += " UP"
                if abs_sys:
                    mask_positive = r >= 0
                    r = np.abs(r)
                    ax.plot(
                        pts[mask_positive] / 1e3,
                        r[mask_positive] * 100.0,
                        label=sys_label,
                        color=color,
                    )
                    ax.plot(pts[~mask_positive] / 1e3, r[~mask_positive] * 100.0, "--", color=color)
                else:
                    ax.plot(pts / 1e3, r * 100.0, label=sys_label, color=color)

            if not only_up:
                ax.set_prop_cycle(None)
                for sys_name in p:
                    sys_name = sys_name.replace("up", "down")
                    if sys_name not in results:
                        continue  # TODO: FIXME
                    r = results[sys_name]
                    r[np.isnan(r)] = 0
                    sys_label = (
                        beautify_sysname(sys_name.replace("__1down", ""))
                        if beautify_sysnames
                        else sys_name
                    )
                    sys_label += " DOWN"
                    ax.plot(pts / 1e3, r * 100.0, label=sys_label, linestyle="--")

            if plot_qsum:
                ax.plot(pts / 1e3, qsum * 100, label="quadrature sum", linestyle=":")

            ax.set_xlabel("$E_T$ [GeV]", ha="right", x=1.0, fontsize=19)
            if abs_sys:
                ax.set_ylabel("Uncertainty [%]", ha="right", y=1.0, fontsize=19)
            else:
                ax.set_ylabel("Signed uncertainty [%]", ha="right", y=1.0, fontsize=19)

            ax.tick_params(axis="both", which="major", labelsize=17)

            ax.axis("tight")

            if max_sys is None and min_sys is None:
                max_sys = max(2, np.max(np.abs(ax.get_ylim())))
                min_sys = -max_sys

            ax.set_ylim(min_sys * 100, max_sys * 100)

            if legend_outside:
                ax.legend(
                    bbox_to_anchor=(0.0, 1.0, 1, 0.2),
                    mode="expand",
                    borderaxespad=0.0,
                    loc=3,
                    frameon=True,
                    fontsize=17 if only_up else 14,
                    borderpad=1,
                    ncol=1 if only_up else 2,
                )
                f.subplots_adjust(top=0.65)
                plot_ATLAS(f, 0.2, 0.58, label=atlas_label)
                f.text(0.2, 0.2, beautify_particle(ptype), transform=ax.transAxes, fontsize=16)
                f.text(
                    0.2,
                    0.25,
                    r"$%.2f < \eta < %.2f$" % (etamin, etamax),
                    transform=ax.transAxes,
                    fontsize=16,
                )
            else:
                ax.legend(
                    loc=1,
                    frameon=False,
                    fontsize=13 if only_up else 9,
                    borderpad=1,
                    ncol=1 if only_up else 2,
                )

                plot_ATLAS(f, 0.16, 0.80, label=atlas_label, fontsize=19)
                f.text(0.16, 0.74, beautify_particle(ptype), transform=ax.transAxes, fontsize=16)
                f.text(
                    0.16,
                    0.68,
                    r"$%.2f < \eta < %.2f$" % (etamin, etamax),
                    transform=ax.transAxes,
                    fontsize=16,
                )

            if log_x:
                ax.set_xscale("log")

            for extension in extensions:
                f.savefig(
                    os.path.join(
                        basedir,
                        "%s_%s_%s_%.2f_%.2f_%d.%s"
                        % (ptype, esmodel, decorrelation, etamin, etamax, ip, extension),
                    ),
                    bbox_inches="tight",
                )
            plt.close(f)


def values2histo(name, title, x, y):
    histo = ROOT.TH1F(name, title, len(x) - 1, x)
    for (
        ibin,
        yy,
    ) in enumerate(y, 1):
        histo.SetBinContent(ibin, yy)
    return histo


def plot_all_Zee_syst(etas, pt=100e3, basedir="plots"):

    tool_es2012c = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2012c")
    tool_es2012c.setProperty("ESModel", "es2012c")
    tool_es2012c.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1")
    tool_es2012c.setProperty[int]("doSmearing", 0)
    tool_es2012c.initialize()

    tool_es2015PRE = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE")
    tool_es2015PRE.setProperty("ESModel", "es2015PRE")
    tool_es2015PRE.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1")
    tool_es2015PRE.setProperty[bool]("doSmearing", 0)
    tool_es2015PRE.initialize()

    tool_es2015PRE_notemp = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE_notemp")
    tool_es2015PRE_notemp.setProperty("ESModel", "es2015PRE")
    tool_es2015PRE_notemp.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1")
    tool_es2015PRE_notemp.setProperty[bool]("doSmearing", 0)
    tool_es2015PRE_notemp.setProperty[int]("use_temp_correction201215", 0)
    tool_es2015PRE_notemp.initialize()

    tools = [tool_es2012c, tool_es2015PRE_notemp, tool_es2015PRE]

    nominal_es2012c = calibrate_eta_pt(tool_es2012c, etas, [pt], particle="electron")[0]
    nominal_es2015PRE = calibrate_eta_pt(tool_es2015PRE, etas, [pt], particle="electron")[0]
    nominal_es2015PRE_notemp = calibrate_eta_pt(
        tool_es2015PRE_notemp, etas, [pt], particle="electron"
    )[0]

    # up variations
    sys = ROOT.CP.SystematicVariation("EG_SCALE_ZEESYST", 1)
    sys_set = ROOT.CP.SystematicSet()
    sys_set.insert(sys)

    for tool in tools:
        tool.applySystematicVariation(sys_set)

    sys_up_es2012c = calibrate_eta_pt(tool_es2012c, etas, [pt], particle="electron")[0]
    sys_up_es2015PRE = calibrate_eta_pt(tool_es2015PRE, etas, [pt], particle="electron")[0]
    sys_up_es2015PRE_notemp = calibrate_eta_pt(
        tool_es2015PRE_notemp, etas, [pt], particle="electron"
    )[0]

    ratio_sys_up_es2012c = sys_up_es2012c / nominal_es2012c - 1
    ratio_sys_up_es2015PRE = sys_up_es2015PRE / nominal_es2015PRE - 1
    ratio_sys_up_es2015PRE_notemp = sys_up_es2015PRE_notemp / nominal_es2015PRE_notemp - 1

    # down variations
    sys = ROOT.CP.SystematicVariation("EG_SCALE_ZEESYST", -1)
    sys_set = ROOT.CP.SystematicSet()
    sys_set.insert(sys)

    for tool in tools:
        tool.applySystematicVariation(sys_set)

    sys_down_es2012c = calibrate_eta_pt(tool_es2012c, etas, [pt], particle="electron")[0]
    sys_down_es2015PRE = calibrate_eta_pt(tool_es2015PRE, etas, [pt], particle="electron")[0]
    sys_down_es2015PRE_notemp = calibrate_eta_pt(
        tool_es2015PRE_notemp, etas, [pt], particle="electron"
    )[0]

    ratio_sys_down_es2012c = sys_down_es2012c / nominal_es2012c - 1
    ratio_sys_down_es2015PRE = sys_down_es2015PRE / nominal_es2015PRE - 1
    ratio_sys_down_es2015PRE_notemp = sys_down_es2015PRE_notemp / nominal_es2015PRE_notemp - 1

    # up stat
    sys = ROOT.CP.SystematicVariation("EG_SCALE_ZEESTAT", 1)
    sys_set = ROOT.CP.SystematicSet()
    sys_set.insert(sys)
    tool_es2015PRE.applySystematicVariation(sys_set)
    stat_up_es2015PRE = calibrate_eta_pt(tool_es2015PRE, etas, [pt], particle="electron")[0]
    ratio_stat_up = stat_up_es2015PRE / nominal_es2015PRE - 1

    # down stat
    sys = ROOT.CP.SystematicVariation("EG_SCALE_ZEESTAT", -1)
    sys_set = ROOT.CP.SystematicSet()
    sys_set.insert(sys)
    tool_es2015PRE.applySystematicVariation(sys_set)
    stat_down_es2015PRE = calibrate_eta_pt(tool_es2015PRE, etas, [pt], particle="electron")[0]
    ratio_stat_down = stat_down_es2015PRE / nominal_es2015PRE - 1

    ratio_full_down = -np.sqrt(ratio_stat_down ** 2 + ratio_sys_down_es2015PRE ** 2)
    ratio_full_up = np.sqrt(ratio_stat_up ** 2 + ratio_sys_up_es2015PRE ** 2)

    fig, ax = plt.subplots()

    ax.fill_between(
        etas,
        ratio_full_down,
        ratio_full_up,
        color="#d69e8f",
        interpolate=False,
        label=r"$\cdots\oplus$ stat = 2015PRE (stat $\oplus$ sys)",
        alpha=0.6,
    )
    ax.fill_between(
        etas,
        ratio_sys_down_es2015PRE,
        ratio_sys_up_es2015PRE,
        color="#d7d790",
        interpolate=False,
        label=r"$\cdots\oplus$ temp = 2015PRE sys",
        alpha=0.6,
    )
    ax.fill_between(
        etas,
        ratio_sys_down_es2015PRE_notemp,
        ratio_sys_up_es2015PRE_notemp,
        color="#91da95",
        label=r"$\cdots\oplus$ 7/8 TeV diff $\oplus$ 34/68 bins diff",
        alpha=0.6,
    )
    ax.fill_between(
        etas,
        ratio_sys_down_es2012c,
        ratio_sys_up_es2012c,
        color="#93dcd1",
        interpolate=False,
        label="2012c",
        alpha=0.6,
    )

    p1 = patches.Rectangle((0, 0), 1, 1, color="#d69e8f")
    p2 = patches.Rectangle((0, 0), 1, 1, color="#d7d790")
    p3 = patches.Rectangle((0, 0), 1, 1, color="#91da95")
    p4 = patches.Rectangle((0, 0), 1, 1, color="#93dcd1")

    legend1 = ax.legend(
        [p4, p3, p2, p1],
        [
            "2012c",
            r"$\cdots\oplus$ 7/8 TeV diff $\oplus$ 34/68 bins diff",
            r"$\cdots\oplus$ temp = 2015PRE sys",
            r"$\cdots\oplus$ stat = 2015PRE (stat $\oplus$ sys)",
        ],
        loc="upper right",
        numpoints=1,
        title="errors",
    )

    f = ROOT.TFile("~/Data6_scaledData.root")
    histo_scale = f.Get("alpha")
    x, ex, y, ey = histo2data(histo_scale)

    h1 = ax.errorbar(x, y, yerr=ey, xerr=ex, fmt="o", zorder=11)

    f2 = ROOT.TFile("~/uA2MeV.root")
    histo_uA2MeV = f2.Get("histo_uA2MeV_week12")
    x, ex, y, ey = histo2data(histo_uA2MeV)
    (line_uA2MeV,) = ax.plot(x, y - 1, "k-", zorder=10, label="expected deviation")
    ax.plot(-x, y - 1, "k-", zorder=10)

    ax.set_xlabel(r"$\eta$")
    ax.set_ylim(-0.08, 0.08)

    ax.legend(
        [h1, line_uA2MeV],
        ["scales 13 TeV|es2015 PRE", "expected deviation"],
        loc="lower right",
        numpoints=1,
    )
    ax.add_artist(legend1)

    ax.grid()

    fig.savefig("Zee_sys.png")
    fig.savefig("Zee_sys.svg")
    fig.savefig("Zee_sys.pdf")


def plot_all_syst_fixed_pt(tools, names, labels, pt=100e3, ptype="unconverted", basedir="plots"):
    for tool in tools:
        empty_set = ROOT.CP.SystematicSet()
        tool.applySystematicVariation(empty_set)
    etas_histo = np.linspace(-5, 5, 101, True)
    etas = 0.5 * (etas_histo[1:] + etas_histo[:-1])
    nominals = [calibrate_eta_pt(tool, etas, [pt], particle=ptype)[0] for tool in tools]
    all_syst = tools[0].recommendedSystematics()
    for sys in all_syst:
        sys_name = sys.name()
        sys_set = ROOT.CP.SystematicSet()
        sys_set.insert(sys)
        log.info("plotting sys %s %s", sys_name, ptype)

        f, ax = plt.subplots()

        for tool, nominal, name, label in zip(tools, nominals, names, labels):
            tool.applySystematicVariation(sys_set)
            sys = calibrate_eta_pt(tool, etas, [pt], particle=ptype)[0]
            ratio = sys / nominal - 1
            ax.plot(etas, ratio * 100, label=label)
            histo = values2histo(name + "_" + sys_name, label, etas_histo, ratio * 100)
            fout.WriteTObject(histo)

        ax.grid()
        ax.legend()
        ax.set_ylabel("effect [%]")
        ax.set_title(ptype + " " + sys_name + " at $p_{T}$ = %.2f GeV" % (pt / 1e3))
        ax.set_xlabel(r"$\eta$")
        for extension in extensions:
            f.savefig(
                os.path.join(basedir, "%s_%s_pt_%.2f.%s" % (ptype, sys_name, pt / 1e3, extension))
            )
        plt.close(f)


def compute_or_read_sys(tool, ptypes, eta_edges, pt_edges):
    empty_set = ROOT.CP.SystematicSet()

    eta_edges = np.asarray(eta_edges)
    pt_edges = np.asarray(pt_edges)

    # TODO: replace midpoints with supersampling
    eta_midpoints = 0.5 * (eta_edges[1:] + eta_edges[:-1])
    pt_midpoints = 0.5 * (pt_edges[1:] + pt_edges[:-1])

    log.debug("getting list of systematics")
    all_syst = tool.recommendedSystematics()
    log.debug("%d systematics found", len(all_syst))

    results = {ptype: {} for ptype in ptypes}

    with tqdm.tqdm(total=len(ptypes) * (len(all_syst) + 1), leave=False) as pbar:
        for iptype, ptype in enumerate(ptypes):
            log.debug("computing for particle=%s", ptype)
            tool.applySystematicVariation(empty_set)
            log.debug(
                "computing nominal energies for eta=%s, pt=%s, particle=%s",
                eta_midpoints,
                pt_midpoints,
                ptype,
            )

            values_nominal = calibrate_eta_pt(tool, eta_midpoints, pt_midpoints, particle=ptype)
            pbar.update(1)

            for isys, sys in enumerate(all_syst):
                sys_name = sys.name()
                sys_set = ROOT.CP.SystematicSet()
                sys_set.insert(sys)
                tool.applySystematicVariation(sys_set)
                values_sys = calibrate_eta_pt(tool, eta_midpoints, pt_midpoints, particle=ptype)
                ratio = values_sys / values_nominal - 1

                results[ptype][sys_name] = ratio
                pbar.update(1)

    return results


@timed
def compare_sys(
    esmodels=None,
    ptypes=None,
    decorrelation="FULL_ETACORRELATED_v1",
    eta_edges=None,
    pt_edges=None,
    basedir="plots",
    smearing=False,
    only_up=True,
    abs_sys=False,
    log_x=False,
):
    esmodels = esmodels or ["es2012c"]
    ptypes = ptypes or ["electron"]
    if pt_edges is None:
        pt_edges = np.linspace(0, 100, 5)
    if eta_edges is None:
        eta_edges = np.array([0, 0.6, 1.0, 1.37, 1.55, 1.82, 2.47])
    pt_edges = np.asarray(pt_edges)
    eta_edges = np.asarray(eta_edges)

    pt_midpoints = 0.5 * (pt_edges[1:] + pt_edges[:-1])

    log.info(
        "comparing systematics for esmodels=%s, ptypes=%s, #pt-bins=%d, #eta-bins=%d",
        (esmodels, ptypes, len(pt_edges) - 1, len(eta_edges) - 1)
    )

    effects = {}
    for esmodel in esmodels:
        log.info("creating tool for %s", esmodel)
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool.setProperty("ESModel", esmodel)
        tool.setProperty("decorrelationModel", decorrelation)
        tool.setProperty[int]("randomRunNumber", 297730)
        if not smearing:
            tool.setProperty[bool]("doSmearing", 0)
        tool.initialize()

        log.info("computing systematics for %s", esmodel)

        sys_values = compute_or_read_sys(tool, ptypes, eta_edges, pt_edges)
        effects[esmodel] = sys_values

        log.debug("delete tool")
        del tool

    def sorting_function(sys):
        return max(
            np.abs(effects[esmodel][ptype].get(sys, np.zeros(1))).max()
            for esmodel in esmodels
            for ptype in ptypes
        )

    all_sys = set()
    for effects_esmodel in effects.values():
        for effects_particle in effects_esmodel.values():
            all_sys |= set(effects_particle.keys())

    if only_up:
        all_sys = set(s for s in all_sys if "__1up" in s)
    all_sys = set(s for s in all_sys if "RESOLUTION" not in s)
    sorted_sys = sorted(all_sys, key=sorting_function, reverse=True)

    log.info("plot")
    log.info("sorted sys: %s", sorted_sys)
    colors = sns.color_palette("Set2", len(esmodels))
    line_styles = "--", ":", "-"

    for ptype, (ieta, (eta1, eta2)) in tqdm.tqdm(
        product(ptypes, enumerate(pairwise(eta_edges))),
        total=len(ptypes) * (len(eta_edges) - 1),
        desc="plotting",
        leave=False,
    ):
        nsub_x, nsub_y = divide_square(len(sorted_sys))
        fig, axs = plt.subplots(nsub_x, nsub_y, figsize=(14, 8), sharex=True)
        if hasattr(axs, "flat"):
            axs = axs.flat
        else:
            axs = [axs]
        for ax, sys in zip(axs, sorted_sys):
            for iesmodel, esmodel in enumerate(esmodels):
                if sys in effects[esmodel][ptype]:
                    values = effects[esmodel][ptype][sys][:, ieta]
                    if abs_sys:
                        values = np.abs(values)
                else:
                    values = np.zeros_like(pt_midpoints)
                ax.plot(
                    pt_midpoints / 1e3,
                    values * 100,
                    label=esmodel,
                    ls=line_styles[iesmodel],
                    color=colors[iesmodel],
                )

            ylimits = [0.01, 0.3, 0.7, 2.1, 5]  # possible y-axis maxima
            for ylimit in ylimits:
                if max(np.abs(values)) * 100 < ylimit:
                    if abs_sys:
                        ax.set_ylim(0, ylimit)
                    else:
                        ax.set_ylim(-ylimit, ylimit)
                    break

            title = sys.replace("EG_SCALE_", "").replace("PH_SCALE_", "").replace("__1up", "")
            if len(title) > 17:
                title = title[:17] + "..."
            ax.set_title(title, fontsize=9)
            ax.yaxis.set_major_locator(MaxNLocator(6, prune="both"))
            ax.xaxis.set_major_locator(MaxNLocator(4))
            ax.tick_params(axis="both", which="major", labelsize=8)
            ax.tick_params(axis="y", which="minor", left="off", right="off")

            if log_x:
                ax.set_xscale("log")
            ax.set_ylabel("")

        for ax in axs:
            if ax.is_last_row():
                ax.set_xlabel("$p_{T}$ [GeV]", fontsize=11)
            if ax.is_first_col():
                ax.set_ylabel("effect [%]", fontsize=11)
        fig.subplots_adjust(wspace=0.4, hspace=0.27, bottom=0.15)

        handles = [
            mlines.Line2D([], [], color=colors[i], ls=line_styles[i]) for i in range(len(esmodels))
        ]
        labels = esmodels
        fig.legend(
            handles,
            labels,
            ncol=len(esmodels),
            loc="upper center",
            bbox_to_anchor=(0.1, -0.14, 0.7, 0.2),
            mode="expand",
            borderaxespad=0.0,
        )

        fig.suptitle(r"%s $|\eta|\in [%.2f, %.2f]$" % (ptype, eta1, eta2), fontsize=14)

        figname = "compare_sys_%s_eta_%.2f_%.2f_vs_pT" % (ptype, eta1, eta2)
        log.info("saving %s", figname)
        for extension in extensions:
            fig.savefig(os.path.join(basedir, "%s.%s" % (figname, extension)))
        plt.close(fig)


@timed
def plot_all_syst_eta_pt(
    esmodel="es2012c",
    decorrelation="FULL_v1",
    ptype="unconverted",
    basedir="plots",
    eta_range=None,
    pt_range=None,
    log_pt=False,
    abs_sys=False,
    only_up=False,
    sys_filters=None,
    min_value=None,
    max_value=None,
):
    """
    Plot a 2D map (eta, pT) of the value of the systematic in %
    """
    log.debug("creating tool")
    tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
    tool.setProperty("ESModel", esmodel)
    tool.setProperty("decorrelationModel", decorrelation)
    tool.setProperty[int]("doSmearing", 0)
    log.warning("setting randomRunNumber to 297730")
    tool.setProperty[int]("randomRunNumber", 297730)
    tool.initialize()

    etas = eta_range if eta_range is not None else np.arange(-3, 3, 0.1)
    pts = pt_range if pt_range is not None else np.logspace(4, 7, 100)

    nominal = calibrate_eta_pt(tool, etas, pts, particle=ptype)

    all_syst = tool.recommendedSystematics()

    nplotted = 0

    for sys in tqdm.tqdm(all_syst):
        sys_name = sys.name()
        if "RESOLUTION" in sys_name:
            log.debug("skipping %s", sys_name)
            continue
        if only_up and "1up" not in sys_name:
            log.debug("skipping %s", sys_name)
            continue
        if sys_filters is not None:
            if not any(fnmatch(sys_name, sys_filter) for sys_filter in sys_filters):
                log.debug("skipping %s", sys_name)
                continue
        nplotted += 1
        log.info("computing sys %s", sys_name)

        sys_set = ROOT.CP.SystematicSet()
        sys_set.insert(sys)
        tool.applySystematicVariation(sys_set)

        sys = calibrate_eta_pt(tool, etas, pts, particle=ptype)

        ratio = sys / nominal
        ratio[nominal == 0] = 1
        # TODO: check why nan
        ratio[np.isnan(ratio)] = 1
        ratio = ratio - 1
        vmax = max_value or np.percentile(np.abs(ratio), 95)
        if abs_sys:
            ratio = np.abs(ratio)

        f, ax = plt.subplots()
        p = ax.pcolormesh(etas, pts / 1e3, ratio * 100.0, vmin=-vmax * 100, vmax=vmax * 100)
        ax.set_title("%s\n%s\n%s" % (ptype, esmodel, sys_name), loc="left")
        ax.set_xlabel(r"$\eta$", x=1.0, ha="right")
        ax.set_ylabel(r"$p_T$ [GeV]", y=1.0, ha="right")
        if log_pt:
            ax.set_yscale("log")
        cb = f.colorbar(p)
        cb.ax.set_ylabel("systematic effect [%]")
        for extension in extensions:
            f.savefig(
                os.path.join(
                    basedir, "%s_%s_%s_%s.%s" % (ptype, esmodel, decorrelation, sys_name, extension)
                )
            )
        plt.close(f)

    if nplotted == 0:
        log.warning("no systematic plotted")
    else:
        log.info("%d systematic plotted", nplotted)


@timed
def plot_all_scales(
    esmodels, basedir, labels=None, etas=np.arange(-4.5, 4.5, 0.01), add_patch=False, debug=False
):
    log.info("comparing scale factors %s", esmodels)
    log.warning("<mu> fixed")
    labels = labels or esmodels
    scales = {}

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

        tool_with = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_with")
        tool_without = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_without")

        for tool in tool_with, tool_without:
            if debug:
                tool.msg().setLevel(0)
            tool.setProperty("ESModel", esmodel)
            tool.setProperty[bool]("doSmearing", 0)
            for k, v, t in zip(key_properties, value_properties, type_properties):
                tool.setProperty[t](k, v)

        tool_with.setProperty[bool]("doScaleCorrection", 1)
        tool_without.setProperty[bool]("doScaleCorrection", 0)

        tool_with.initialize()
        tool_without.initialize()

        event = ROOT.xAOD.TEvent()  # noqa: F841
        factory = ROOT.EgammaFactory()
        log.warning("using eveninfo 266904")
        ei = factory.create_eventinfo(False, 266904)

        for eta in etas:
            el = factory.create_electron(eta, 0.1, 40e3)
            en_with = tool_with.getEnergy(el, ei)
            en_without = tool_without.getEnergy(el, ei)
            energy_with.append(en_with)
            energy_without.append(en_without)
        scales[label] = np.array(energy_without) / np.array(energy_with) - 1

    f, ax = plt.subplots()
    for k, v in scales.items():
        ax.plot(etas, v * 100, label=k)
    ax.set_xlabel(r"$\eta$", fontsize=15, x=1, ha="right")
    ax.set_ylabel("energy without scale factors / energy with scale factors - 1 [%]", fontsize=10)
    ax.grid()

    if add_patch:
        ax.add_patch(
            patches.Rectangle(
                (-2.47, -0.013 * 100 - 0.0168 * 100),
                +2.47 - 1.55,
                2 * 0.0168 * 100,
                alpha=0.4,
                label="13 TeV scales 3 bins",
                color="k",
            )
        )
        # ax.add_patch(patches.Rectangle((-1.55, 0.00419 * 100 - 0.001 * 100), +1.55 - 1.37, 2 * 0.001 * 100, alpha=0.4, color='k'))
        ax.add_patch(
            patches.Rectangle(
                (-1.37, -0.0121 * 100 - 0.0052 * 100),
                2 * +1.37,
                2 * 0.0052 * 100,
                alpha=0.4,
                color="k",
            )
        )
        ax.add_patch(
            patches.Rectangle(
                (1.55, -0.013 * 100 - 0.0168 * 100),
                +2.47 - 1.55,
                2 * 0.0168 * 100,
                alpha=0.4,
                color="k",
            )
        )

        ax.add_patch(
            patches.Rectangle(
                (-2.47, -0.00649344 * 100 - 0.00465043 * 100),
                +2.47 * 2,
                2 * 0.00465043 * 100,
                alpha=0.5,
                label="13 TeV scales 1 bin",
                color="orange",
            )
        )

    ax.legend()

    for extension in extensions:
        f.savefig(os.path.join(basedir, "scales.%s" % extension))
    plt.close(f)

    # ratio plot
    reference = labels[0]
    ratio = {label: scales[label] - scales[reference] for label in labels}
    f, ax = plt.subplots()
    for k, v in ratio.items():
        ax.plot(etas, v, label=k)
    ax.set_xlabel(r"$\eta$", fontsize=15, x=1, ha="right")
    ax.set_ylabel("scales - scales (%s)" % reference, fontsize=10)
    ax.grid()
    ax.legend()
    f.tight_layout()

    for extension in extensions:
        f.savefig(os.path.join(basedir, "scales_difference_%s.%s" % (reference, extension)))
    plt.close(f)


def plot_all_cterms(esmodels, basedir, labels=None, etas=np.arange(-4.5, 4.5, 0.01)):
    labels = labels or esmodels
    cterms_all_models = {}
    for esmodel, label in zip(esmodels, labels):

        tool_with = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_with")
        tool_with.setProperty("ESModel", esmodel)
        tool_with.setProperty[bool]("doSmearing", 1)
        tool_with.initialize()

        tool_without = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_without")
        tool_without.setProperty("ESModel", esmodel)
        tool_without.setProperty[bool]("doSmearing", 0)
        tool_without.initialize()

        event = ROOT.xAOD.TEvent()  # noqa: F841
        factory = ROOT.EgammaFactory()
        ei = factory.create_eventinfo(True, 100000)

        cterms = []
        for eta in etas:
            el = factory.create_electron(eta, 0.1, 40e3)
            en_without = tool_without.getEnergy(el, ei)
            ratios = np.zeros(1000)
            for repetition in range(1000):
                en_with = tool_with.getEnergy(el, ei)
                ratios[repetition] = en_with / en_without
            cterms.append(np.std(ratios))
        cterms_all_models[label] = cterms

    f, ax = plt.subplots()
    for k, v in cterms_all_models.items():
        ax.plot(etas, v, label=k)
    ax.set_xlabel(r"$\eta$")
    ax.set_ylabel("std (energy with additional cterm / energy without)")

    ax.add_patch(
        patches.Rectangle(
            (-2.47, 0.028 - 0.027),
            +2.47 - 1.55,
            2 * 0.027,
            alpha=0.4,
            label="13 TeV cterm 3 bins",
            color="k",
        )
    )
    ax.add_patch(
        patches.Rectangle((-1.37, -0.003 - 0.014), +1.37 * 2, 2 * 0.014, alpha=0.4, color="k")
    )
    ax.add_patch(
        patches.Rectangle((1.55, 0.028 - 0.027), +2.47 - 1.55, 2 * 0.027, alpha=0.4, color="k")
    )
    ax.set_ylim(-0.03, 0.06)

    ax.grid()
    ax.legend()
    for extension in extensions:
        f.savefig(os.path.join(basedir, "cterms.%s" % extension))
    plt.close(f)


def compare_two_tools_eta(
    tool1, tool2, pt, simulation, name, basedir, title, particle="unconverted"
):
    etas = np.arange(-4.5, 4.5, 0.01)
    r1 = calibrate_eta_pt(tool1, etas, [pt], simulation, particle)
    r2 = calibrate_eta_pt(tool2, etas, [pt], simulation, particle)
    r = r1 / r2 - 1
    r = r[0]
    print(r)
    f, ax = plt.subplots()
    r = np.nan_to_num(r)
    ax.plot(etas, r * 100)
    ax.set_xlabel(r"$\eta$")
    ax.set_ylabel("effect [%]")
    ax.set_title(title)
    ax.axis("tight")
    ax.grid()
    for extension in extensions:
        f.savefig(os.path.join(basedir, name + "." + extension))
    plt.close(f)


def compare_two_tools_eta_phi(
    tool1, tool2, pt, simulation, name, basedir, title, particle="unconverted"
):
    etas = np.arange(-4.5, 4.5, 0.1)
    phis = np.arange(-np.pi, np.pi, 0.1)
    r1 = eval_sys_eta_phi(tool1, etas, phis, pt, simulation, particle)
    r2 = eval_sys_eta_phi(tool2, etas, phis, pt, simulation, particle)
    r = r1 / r2 - 1
    f, ax = plt.subplots()
    m = np.max(np.abs(r))
    p = ax.pcolormesh(phis, etas, r * 100, vmin=-m * 100, vmax=m * 100)
    ax.set_xlabel(r"$\phi$")
    ax.set_ylabel(r"$\eta$")
    ax.set_title(title)
    ax.axis("tight")
    cb = f.colorbar(p)
    cb.ax.set_ylabel("ratio to nominal [%]")
    for extension in extensions:
        f.savefig(os.path.join(basedir, name + "." + extension))
    plt.close(f)


def compare_two_tools_eta_pt(
    tool1, tool2, simulation, name, basedir, title, particle="unconverted"
):
    etas = np.arange(-4.5, 4.5, 0.05)
    pts = np.logspace(3.2, 6, 50)
    r1 = calibrate_eta_pt(tool1, etas, pts, simulation, particle)
    r2 = calibrate_eta_pt(tool2, etas, pts, simulation, particle)
    r = r1 / r2 - 1
    f, ax = plt.subplots()
    r = np.nan_to_num(r)
    m = np.max(np.abs(r))
    p = ax.pcolormesh(etas, pts / 1e3, r * 100, vmin=-m * 100, vmax=m * 100)
    ax.set_yscale("log")
    ax.set_ylabel("$p_T$ [GeV]")
    ax.set_xlabel(r"$\eta$")
    ax.set_title(title)
    ax.axis("tight")

    cb = f.colorbar(p)
    cb.ax.set_ylabel("ratio to nominal [%]")
    for extension in extensions:
        f.savefig(os.path.join(basedir, name + "." + extension))
    plt.close(f)


def check_gain(basedir, esmodel):
    tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_%s" % esmodel)
    tool.setProperty("ESModel", esmodel)
    tool.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1")
    tool.setProperty[bool]("doSmearing", 0)
    tool.initialize()

    tool_no_gain = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_%s" % esmodel)
    tool_no_gain.setProperty("ESModel", esmodel)
    tool_no_gain.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1")
    tool_no_gain.setProperty[bool]("doSmearing", 0)
    tool_no_gain.setProperty[bool]("useGainCorrection", 0)
    tool_no_gain.initialize()

    for ptype in "electron", "unconverted", "converted":
        compare_two_tools_eta_pt(
            tool_no_gain,
            tool,
            simulation=False,
            name="gain_%s_%s" % (esmodel, ptype),
            basedir=basedir,
            particle=ptype,
            title="gain correction effect - %s - %s" % (esmodel, ptype),
        )

    compare_two_tools_eta(
        tool_no_gain,
        tool,
        40e3,
        simulation=False,
        name="gain_%s_%s_40GeV" % (esmodel, "electron"),
        basedir=basedir,
        title="gain correction effect - %s 40 GeV electron" % esmodel,
        particle="electron",
    )


def check_fast(basedir, esmodel):
    tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_%s" % esmodel)
    tool.setProperty("ESModel", esmodel)
    tool.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1")
    tool.setProperty[bool]("doSmearing", 0)
    tool.initialize()

    tool_fast = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_%s" % esmodel)
    tool_fast.setProperty("ESModel", esmodel)
    tool_fast.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1")
    tool_fast.setProperty[bool]("doSmearing", 0)
    tool_fast.setProperty[bool]("useAFII", True)
    tool_fast.initialize()

    for ptype in "electron", "converted", "unconverted":
        compare_two_tools_eta(
            tool_fast,
            tool,
            pt=100e3,
            simulation=True,
            name="fast_%s_%s" % (esmodel, ptype),
            basedir=basedir,
            particle=ptype,
            title="fast correction effect %s %s 100 GeV" % (esmodel, ptype),
        )


def check_uniformity(basedir, esmodel):
    tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_%s" % esmodel)
    tool.setProperty("ESModel", esmodel)
    tool.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1")
    tool.setProperty[bool]("doSmearing", 0)
    tool.initialize()

    tool_no_uniformity = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_%s" % esmodel)
    tool_no_uniformity.setProperty("ESModel", esmodel)
    tool_no_uniformity.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1")
    tool_no_uniformity.setProperty[bool]("doSmearing", 0)
    tool_no_uniformity.setProperty[bool]("usePhiUniformCorrection", 0)
    tool_no_uniformity.initialize()

    compare_two_tools_eta_phi(
        tool_no_uniformity,
        tool,
        pt=100e3,
        simulation=False,
        name="uniformity_%s" % esmodel,
        basedir=basedir,
        particle="unconverted",
        title="uniformity correction effect - %s - unconverted 100 GeV" % esmodel,
    )


def compare_all_syst_fixed_pt(basedir, esmodels, names=None, labels=None):
    names = names or esmodels
    labels = labels or names
    tools = []
    for esmodel in esmodels:
        if type(esmodel) is str:
            tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_%s" % esmodel)
            tool.setProperty("ESModel", esmodel)
            tool.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1")
            tool.setProperty[bool]("doSmearing", 0)
            tool.initialize()
            tools.append(tool)
        else:
            tools.append(esmodel)
    plot_all_syst_fixed_pt(tools, names=names, labels=labels, basedir=basedir)


def plot_resolution_eta_pt(basedir, tool, pts, etas, ptype, title):
    event = ROOT.xAOD.TEvent()  # noqa: F841
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
    p = ax.pcolormesh(pts / 1e3, etas, result, vmin=vmin, vmax=vmax)
    ax.set_title(title)
    ax.axis("tight")
    ax.set_xlabel(r"$p_T$ [GeV]")
    ax.set_ylabel(r"$\eta$")
    ax.set_xscale("log")
    cb = fig.colorbar(p)
    cb.ax.set_ylabel("resolution")
    for extension in extensions:
        fig.savefig(os.path.join(basedir, "resolution_%s.%s" % (ptype, extension)))


def plot_resolution_error(basedir, **kwargs):
    esmodels = kwargs["esmodels"] or (
        "es2012c",
        "es2015PRE",
        "es2015PRE_res_improved",
        "es2016PRE",
        "es2017",
    )
    if kwargs["eta_bins"] is not None:
        eta_bins = pairwise(kwargs["eta_bins"])
    else:
        eta_bins = (0, 0.4), (0.4, 0.6), (0.6, 1.37), (1.52, 1.8), (1.8, 2.37)
    for esmodel in esmodels:
        log.debug("plotting resolution error for %s", esmodel)
        tool = ROOT.AtlasRoot.egammaEnergyCorrectionTool()
        if kwargs["debug"]:
            tool.msg().setLevel(0)
        tool.setESModel(getattr(ROOT.egEnergyCorr, esmodel))
        tool.initialize()
        for particle in ("electron", "converted", "unconverted"):
            log.info("plotting resolution %s", particle)
            for eta_min, eta_max in tqdm.tqdm(eta_bins):
                plot_resolution_error_bin(
                    eta_min, eta_max, particle, esmodel, basedir, tool=tool, **kwargs
                )


def plot_resolution_error_bin(eta_min, eta_max, particle, esmodel, basedir, tool=None, **kwargs):
    if tool is None:
        tool = ROOT.AtlasRoot.egammaEnergyCorrectionTool()
        tool.setESModel(getattr(ROOT.egEnergyCorr, esmodel))
        tool.initialize()

    ptype = {
        "electron": ROOT.PATCore.ParticleType.Electron,
        "unconverted": ROOT.PATCore.ParticleType.UnconvertedPhoton,
        "converted": ROOT.PATCore.ParticleType.ConvertedPhoton,
    }[particle]

    variations_name_up = {
        "Zsmearing up": ROOT.egEnergyCorr.Resolution.ZSmearingUp,
        "sampling up": ROOT.egEnergyCorr.Resolution.SamplingTermUp,
        "material ID up": ROOT.egEnergyCorr.Resolution.MaterialIDUp,
        "material calo up": ROOT.egEnergyCorr.Resolution.MaterialCaloUp,
        "material gap up": ROOT.egEnergyCorr.Resolution.MaterialGapUp,
        "material cryo up": ROOT.egEnergyCorr.Resolution.MaterialCryoUp,
        "pileup up": ROOT.egEnergyCorr.Resolution.PileUpUp,
        "material ibl up": ROOT.egEnergyCorr.Resolution.MaterialIBLUp,
        "material pp0 up": ROOT.egEnergyCorr.Resolution.MaterialPP0Up,
        "all up": ROOT.egEnergyCorr.Resolution.AllUp,
    }
    variations_name_down = {
        "Zsmearing down": ROOT.egEnergyCorr.Resolution.ZSmearingDown,
        "sampling down": ROOT.egEnergyCorr.Resolution.SamplingTermDown,
        "material ID down": ROOT.egEnergyCorr.Resolution.MaterialIDDown,
        "material calo down": ROOT.egEnergyCorr.Resolution.MaterialCaloDown,
        "material gap down": ROOT.egEnergyCorr.Resolution.MaterialGapDown,
        "material cryo down": ROOT.egEnergyCorr.Resolution.MaterialCryoDown,
        "pileup down": ROOT.egEnergyCorr.Resolution.PileUpDown,
        "material ibl down": ROOT.egEnergyCorr.Resolution.MaterialIBLDown,
        "material pp0 down": ROOT.egEnergyCorr.Resolution.MaterialPP0Down,
        "all down": ROOT.egEnergyCorr.Resolution.AllDown,
    }
    #                 ^    ^
    all_errors = [{}, {}]  # up and down
    #                  ,--,

    pt_range = kwargs.get("pt_bins")
    if pt_range is None:
        pt_range = np.linspace(10e3, 2000e3, 100)

    nsamples_eta = kwargs["super_sampling_eta"] or 5
    eta_range = np.linspace(eta_min, eta_max, nsamples_eta + 2)[1:-1]

    only_up = True

    for side, errors, variations_name in zip(
        ("up", "down"), all_errors, (variations_name_up, variations_name_down)
    ):
        if only_up and side == "down":
            continue

        for variation_name, variation_id in variations_name.items():
            errors_var_pt_eta = np.zeros((len(pt_range), len(eta_range)))
            for ipt, pt in enumerate(pt_range):
                for ieta, eta in enumerate(eta_range):
                    energy = pt * np.cosh(eta)
                    log.debug(
                        "evaluating systematics %s in eta=%.2f pt=%.2f on resolution",
                        (variation_name, eta, pt)
                    )
                    errors_var_pt_eta[ipt, ieta] = tool.getResolutionError(
                        ROOT.PATCore.ParticleDataType.Full, energy, eta, eta, ptype, variation_id
                    )
            errors[variation_name] = errors_var_pt_eta.mean(
                axis=1
            )  # average over different eta points inside the eta-bin
            if kwargs["abs_sys"]:
                errors[variation_name] = np.abs(errors[variation_name])

#    sorted_keys_up = sorted(
#        variations_name_up.keys(), key=lambda x: np.abs(all_errors[0][x].mean())
#    )
    totals = [
        np.sqrt(np.sum([e ** 2 for k, e in errors.items() if "all " not in k]))
        for errors in all_errors
    ]

    fig, ax = plt.subplots()
    if only_up:
        ax.fill_between(pt_range / 1e3, 0, all_errors[0]["all up"], color="0.8")
    else:
        ax.fill_between(
            pt_range / 1e3, all_errors[0]["all up"], all_errors[1]["all down"], color="0.8"
        )
    #    ax.fill_between(pt_range / 1E3, totals[0], -totals[-1], color='0.8')
    #    totals[1] *= -1

    colors = ["b", "g", "r", "c", "m", "y", "violet", "pink", "orange"]
    props = mpl.rcParams["axes.prop_cycle"] # noqa: F841

    for side, errors, total in zip(("up", "down"), all_errors, totals):
        if only_up and side == "down":
            continue
        # ax.plot(pt_range / 1E3, total, label='sum %s' % side, color='k')
        ax.plot(pt_range / 1e3, errors["all %s" % side], "k", label="all %s" % side)
        colors_iter = cycle(colors)
        for k in sorted(errors.keys()):
            if "all" in k:
                continue
            v = errors[k]
            linestyle = "-"
            if "down" in k:
                linestyle = "--"
            if "all" in k:
                linestyle = ":"
            ax.plot(pt_range / 1e3, v, linestyle, label=k, color=next(colors_iter))

    fig.text(0.16, 0.73, beautify_particle(particle), transform=ax.transAxes, fontsize=15)
    fig.text(
        0.16,
        0.67,
        r"$%.2f < \eta < %.2f$" % (eta_min, eta_max),
        transform=ax.transAxes,
        fontsize=15,
    )

    ax.set_title("%s" % esmodel)
    ax.set_ylabel("relative resolution error [%]", ha="right", y=1.0, fontsize=19)
    ax.set_xlabel("$E_T$ [GeV]", ha="right", x=1.0, fontsize=19)
    if kwargs["abs_sys"]:
        ax.set_ylim(0, 0.6)
    else:
        ax.set_ylim(-1.7, 1.7)
    ax.set_xlim(np.min(pt_range) / 1e3, np.max(pt_range) / 1e3)

    fig.subplots_adjust(bottom=0.35)

    ax.legend(
        loc=3,
        bbox_to_anchor=(0.0, -0.5, 1, 0.2),
        mode="expand",
        ncol=4,
        borderaxespad=0.0,
        fontsize=10,
    )

    plot_ATLAS(fig, 0.16, 0.8, "Internal", fontsize=19)

    if kwargs["grid"]:
        ax.grid()
    filename = os.path.join(
        basedir, "error_relresolution_%s_%s_eta%.2f-%.2f" % (esmodel, particle, eta_min, eta_max)
    )
    for ext in "png", "pdf":
        fig.savefig(filename + "." + ext, bbox_inches="tight")
    plt.close(fig)


def list_systematics(esmodels, decorrelation="FULL_ETACORRELATED_v1"):
    if type(esmodels) is str:
        esmodels = [esmodels]
    elif esmodels is None:
        log.warning("no esmodel specified, using es2012c")
        esmodels = ["es2012c"]
    syslist_esmodel = {}
    for esmodel in esmodels:
        log.debug("creating tool for %s", esmodel)
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool.setProperty("ESModel", esmodel)
        tool.setProperty("decorrelationModel", decorrelation)
        tool.initialize()
        syslist_esmodel[esmodel] = [s.name() for s in systematics_from_tool(tool, only_scale=False)]

    for esmodel, sys_list in syslist_esmodel.items():
        print("esmodel: %s %s UP variations" % (esmodel, len(sys_list)))
        for sysname in sys_list:
            print("   {:10s}".format(sysname))

    if len(esmodels) > 1:
        log.info("comparing the %s esmodels", len(esmodels))
        all_sys = set([item for sublist in syslist_esmodel.values() for item in sublist])
        print(" " * 40 + "".join(" [%d] " % i for i in range(len(esmodels))))
        for sysname in sorted(all_sys):
            cross = "".join(
                [
                    "  x  " if sysname in syslist_esmodel[esmodel] else "     "
                    for esmodel in esmodels
                ]
            )
            print(("{:40s}".format(sysname) + cross))
        for iesmodel, esmodel in enumerate(esmodels):
            print("[%d] = %s" % (iesmodel, esmodel))


@timed
def main():
    from argparse import ArgumentParser, RawDescriptionHelpFormatter

    parser = ArgumentParser(
        formatter_class=RawDescriptionHelpFormatter,
        epilog="""
to produce paper plots for systematics:
    python plot.py sys_eta_slices --beautify-sysnames --sys-order paper_run1 --superimpose-all --skip-null-sys --esmodels es2016PRE --min-sys-value -0.01 --max-sys-value 0.02 --symmetrize-labels  --pt-bins-logspace 5E3 1E6 100

    # python plot.py sys_eta_slices --beautify-sysnames --sys-order paper_run1 --superimpose-all --skip-null-sys --esmodels es2016data_mc15c_summer_improved --min-sys-value -0.01 --max-sys-value 0.02 --symmetrize-labels  --pt-bins-logspace 5E3 1E6 100

to produce paper plot for resolution (the name of the esmodels here are the ones used by the internal class)
   python plot.py resolution-err --esmodels es2017 --abs-sys --pt-bins-linspace 5E3 200E3 200

to compare systematics between tools vs pT:
    python plot.py compare_sys --particles electron converted unconverted --esmodels es2016data_mc15c_summer_improved es2016data_mc15c_summer --eta-bins 0 0.6 1.0 1.37 1.55 1.82 2.47 --pt-bins-logspace 5E3 1500E3 50 --abs-sys --log-x

to plot all systematics in a 2d plot (eta, pt):
    python plot.py all_sys --esmodel es2016data_mc15c_summer es2012c --decorrelation=FULL_ETACORRELATED_v1 --eta-bins-linspace -2.5 2.5 50 --pt-bins-linspace 15E3 100E3 50 --particles converted unconverted --sys-filters *LARCALIB* *LARELECUNCONV*

to produce scale factor correction (doScaleCorrection on/off):
    python plot.py scale --esmodels es2015PRE es2016data_mc15c_summer

to list the systematics:
    python plot.py list-systematics --esmodels es2012c es2016data_mc15c_summer es2016data_mc15c_summer_improved es2017_R21_PRE
""",
    )
    parser.add_argument(
        "action",
        choices=[
            "compare_sys",
            "compute_sys",
            "uA2MeV",
            "zee",
            "material",
            "scale",
            "cterm",
            "all",
            "gain",
            "uniformity",
            "sys_fixed_pt",
            "sys_eta_slices",
            "all_sys",
            "resolution",
            "fast",
            "resolution-err",
            "list-systematics",
        ],
        default="all",
        help="what to do",
    )
    parser.add_argument("--esmodels", nargs="*", help="esmodel to consider")
    parser.add_argument("--decorrelation", default="FULL_ETACORRELATED_v1")
    parser.add_argument(
        "--particles",
        nargs="*",
        help="the particle (electron/converted/unconverted)",
        default=["electron"],
    )
    parser.add_argument("--beautify-sysnames", action="store_true")
    parser.add_argument("--eta-bins", nargs="*", type=float, help="edges of the eta-bins")
    parser.add_argument(
        "--eta-bins-linspace", nargs=3, type=float, help="edge of the eta-bins as min max nbins"
    )
    parser.add_argument("--pt-bins", nargs="*", type=float, help="edges of the pt-bins")
    parser.add_argument(
        "--pt-bins-linspace", nargs=3, type=float, help="edge of the eta-bins as min max nbins"
    )
    parser.add_argument(
        "--pt-bins-logspace", nargs=3, type=float, help="edge of the eta-bins as min max nbins"
    )
    parser.add_argument(
        "--super-sampling-eta", type=int, default=5, help="how many point to average inside a bin"
    )
    parser.add_argument(
        "--sys-order",
        choices=["paper_run1", "paper_run2"],
        help="how to order systematics, options: paper_run1, paper_run2",
    )
    parser.add_argument("--debug", action="store_true")
    parser.add_argument(
        "--superimpose-all", action="store_true", help="superimpose sum of systematics"
    )
    parser.add_argument("--skip-null-sys", action="store_true", help="do not plot null systematics")
    parser.add_argument("--yrange", nargs=2, type=float)
    parser.add_argument("--add-down", action="store_true", help="plot also the down systematics")
    parser.add_argument(
        "--legend-outside", action="store_true", help="draw the legend outside the plot"
    )
    parser.add_argument("--symmetrize-labels", action="store_true")
    parser.add_argument(
        "--abs-sys", action="store_true", help="consider the abs value of the systeamatics"
    )
    parser.add_argument("--log-x", action="store_true", help="use log scale")
    parser.add_argument("--log-pt", action="store_true", help="use log scale for pT")
    parser.add_argument(
        "--sys-filters", nargs="*", help="list of wildcard to filter systematic names"
    )
    parser.add_argument("--min-sys-value", type=float, help="min value for the systematic axis")
    parser.add_argument("--max-sys-value", type=float, help="max value for the systematic axis")
    parser.add_argument("--grid", action="store_true", help="show grid")
    parser.add_argument(
        "--plot-qsum",
        action="store_true",
        help="plot the quadrature sum of the systematics for debug (should be equal to 1NP up)",
    )
    parser.add_argument(
        "--atlas-label", default="Internal", help='Internal, Preliminary, ..., use "" for papers'
    )
    args = parser.parse_args()

    if args.debug:
        log.setLevel(logging.DEBUG)
        log.debug("DEBUG activated")

    if args.eta_bins_linspace:
        args.eta_bins = np.linspace(
            args.eta_bins_linspace[0], args.eta_bins_linspace[1], int(args.eta_bins_linspace[2] + 1)
        )
    if args.pt_bins_linspace:
        args.pt_bins = np.linspace(
            args.pt_bins_linspace[0], args.pt_bins_linspace[1], int(args.pt_bins_linspace[2] + 1)
        )
    if args.pt_bins_logspace:
        args.pt_bins = np.logspace(
            np.log10(args.pt_bins_logspace[0]),
            np.log10(args.pt_bins_logspace[1]),
            int(args.pt_bins_logspace[2] + 1),
        )

    basedir = "plots"
    if not os.path.exists(basedir):
        os.makedirs(basedir)

    if args.action == "list-systematics":
        list_systematics(args.esmodels, args.decorrelation)

    if args.action == "compute_sys":
        log.info("computing systematics")
        for esmodel in args.esmodels or ["es2012c"]:
            tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
            tool.setProperty("ESModel", esmodel)
            tool.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1")
            tool.setProperty[bool]("doSmearing", 0)
            tool.initialize()
            result = compute_or_read_sys(tool, ["electron"], args.eta_bins, args.pt_bins)
            print(result)

    if args.action == "compare_sys":
        log.info("comparing systematics")
        compare_sys(
            args.esmodels,
            ptypes=args.particles,
            decorrelation=args.decorrelation,
            eta_edges=args.eta_bins,
            pt_edges=args.pt_bins,
            abs_sys=args.abs_sys,
            log_x=args.log_x,
        )

    if args.action == "resolution-err":
        log.info("plotting resolution error")
        plot_resolution_error(basedir="plots", **vars(args))

    if args.action == "zee" or args.action == "all":
        log.info("plotting scale systematics")
        plot_all_Zee_syst(etas=np.arange(-2.5, 2.5, 0.01), pt=100e3, basedir="plots")

    if args.action == "all" or args.action == "uA2MeV":
        log.info("plotting uA2MeV")
        tool_es2015PRE_nouA2MeV = ROOT.CP.EgammaCalibrationAndSmearingTool(
            "tool_es2015PRE_nouA2MeV"
        )
        tool_es2015PRE_nouA2MeV.setProperty("ESModel", "es2015PRE")
        tool_es2015PRE_nouA2MeV.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1")
        tool_es2015PRE_nouA2MeV.setProperty[bool]("doSmearing", 0)
        tool_es2015PRE_nouA2MeV.setProperty[bool]("use_uA2MeV_2015_first2weeks_correction", 0)
        tool_es2015PRE_nouA2MeV.initialize()

        tool_es2015PRE = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE")
        tool_es2015PRE.setProperty("ESModel", "es2015PRE")
        tool_es2015PRE.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1")
        tool_es2015PRE.setProperty[bool]("doSmearing", 0)
        tool_es2015PRE.initialize()

        compare_two_tools_eta(
            tool_es2015PRE,
            tool_es2015PRE_nouA2MeV,
            100e3,
            simulation=False,
            name="uA2MeV",
            basedir=basedir,
            title="with uA2MeV correction / without",
            particle="electron",
        )

        del tool_es2015PRE_nouA2MeV
        del tool_es2015PRE

    if args.action == "all" or args.action == "scale":
        log.info("plotting scales")
        plot_all_scales(
            esmodels=args.esmodels,
            labels=args.esmodels,
            basedir=basedir,
            etas=np.arange(-2.5, 2.5, 0.01),
            add_patch=False,
            debug=args.debug,
        )
    if args.action == "all" or args.action == "fast":
        log.info("plotting full / fast scale")
        check_fast(basedir, "es2015PRE")
        plot_all_scales(
            esmodels=("es2015PRE", ("es2015PRE", ("useAFII",), (True,), (bool,))),
            labels=("2015PRE", "2015PRE FAST"),
            basedir=basedir,
            etas=np.arange(-2.5, 2.5, 0.01),
        )
    if args.action == "all" or args.action == "cterm":
        log.info("plotting smearings")
        plot_all_cterms(
            esmodels=("es2012c", "es2012XX", "es2015PRE"),
            labels=("2012", "new", "new + temp"),
            basedir=basedir,
            etas=np.arange(-2.5, 2.5, 0.01),
        )

    if args.action == "gain" or args.action == "all":
        check_gain(basedir, "es2015PRE")

    if args.action == "uniformity" or args.action == "all":
        check_uniformity(basedir, "es2012c")
        check_uniformity(basedir, "es2015PRE")

    if args.action == "sys_fixed_pt" or args.action == "all":
        tool_es2015PRE_notemp = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE_notemp")
        tool_es2015PRE_notemp.setProperty("ESModel", "es2015PRE")
        tool_es2015PRE_notemp.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1")
        tool_es2015PRE_notemp.setProperty[bool]("doSmearing", 0)
        tool_es2015PRE_notemp.setProperty[bool]("use_temp_correction201215", 0)
        tool_es2015PRE_notemp.initialize()

        compare_all_syst_fixed_pt(
            basedir,
            ("es2012c", tool_es2015PRE_notemp, "es2015PRE"),
            names=("es2012c", "es2015_no_temp", "es2015PRE"),
            labels=("es2012c", r"$\oplus$ 7/8 diff $\oplus$ 34/68 diff", r"$\oplus$ LAr temperature"),
        )

    if args.action == "sys_eta_slices" or args.action == "all":
        if not args.eta_bins:
            eta_bins = ((0.0, 0.6), (0.6, 1.45), (1.52, 1.7), (1.7, 1.9), (1.9, 2.5), (1.4, 1.6))
            log.warning("no eta-binning specified using %s", eta_bins)
        else:
            eta_bins = list(pairwise(args.eta_bins))
            log.info("eta-bin: %s", eta_bins)
        for ptype in "electron", "unconverted", "converted":
            log.debug("plot all sys FULL_ETACORRELATED_v1 eta slice %s", ptype)
            plot_all_syst_eta_slice(
                eta_bins,
                supersampling_eta=args.super_sampling_eta,
                esmodel=args.esmodels[0],
                decorrelation="FULL_ETACORRELATED_v1",
                ptype=ptype,
                basedir=basedir,
                beautify_sysnames=args.beautify_sysnames,
                sys_order=args.sys_order,
                superimpose_all=args.superimpose_all,
                skip_null_sys=args.skip_null_sys,
                max_sys=args.max_sys_value,
                min_sys=args.min_sys_value,
                debug=args.debug,
                only_up=not args.add_down,
                symmetrize_labels=args.symmetrize_labels,
                legend_outside=args.legend_outside,
                pts=args.pt_bins,
                log_x=args.log_x,
                plot_qsum=args.plot_qsum,
                abs_sys=args.abs_sys,
                atlas_label=args.atlas_label,
            )

    if args.action == "material" or args.action == "all":
        if not os.path.exists("material"):
            os.makedirs("material")
        etabins = ((0.0, 0.6), (0.6, 1.45), (1.52, 1.7), (1.7, 1.9), (1.9, 2.5))
        for ptype in "electron", "unconverted", "converted":
            plot_all_syst_eta_slice(
                etabins,
                supersampling_eta=args.super_sampling_eta,
                esmodel=args.esmodels[0],
                decorrelation="FULL_ETACORRELATED_v1",
                ptype=ptype,
                basedir="material",
                only_material=True,
                beautify_sysnames=args.beautify_sysnames,
                sys_order=args.sys_order,
                skip_null_sys=args.skip_null_sys,
                max_sys=args.max_sys_value,
                min_sys=args.min_sys_value,
                debug=args.debug,
                only_up=not args.add_down,
                legend_outside=args.legend_outside,
                symmetrize_labels=args.symmetrize_labels,
                log_x=args.log_x,
                plot_qsum=args.plot_qsum,
                abs_sys=args.abs_sys,
                atlas_label=args.atlas_label,
            )

    if args.action == "all_sys" or args.action == "all":
        for esmodel in args.esmodels:
            for ptype in args.particles:
                log.info("plotting sys for %s %s", ptype, esmodel)
                plot_all_syst_eta_pt(
                    esmodel=esmodel,
                    decorrelation=args.decorrelation,
                    ptype=ptype,
                    basedir=basedir,
                    eta_range=args.eta_bins,
                    pt_range=args.pt_bins,
                    log_pt=args.log_pt,
                    abs_sys=args.abs_sys,
                    sys_filters=args.sys_filters,
                    min_value=args.min_sys_value,
                    max_value=args.max_sys_value,
                )

    if args.action == "resolution" or args.action == "all":
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool.setProperty("ESModel", "es2015PRE")
        tool.initialize()
        pts = np.logspace(3.2, 6, 50)
        etas = np.linspace(-2.49, 2.49, 100)
        plot_resolution_eta_pt(
            basedir, tool, pts, etas, "electron", title="resolution electron es2015PRE"
        )
        plot_resolution_eta_pt(
            basedir, tool, pts, etas, "converted", title="resolution converted es2015PRE"
        )
        plot_resolution_eta_pt(
            basedir, tool, pts, etas, "unconverted", title="resolution unconverted es2015PRE"
        )


if __name__ == "__main__":

    class TqdmLoggingHandler(logging.Handler):
        def __init__(self, level=logging.NOTSET):
            super(self.__class__, self).__init__(level)

        def emit(self, record):
            try:
                msg = self.format(record)
                tqdm.tqdm.write(msg)
                self.flush()
            except (KeyboardInterrupt, SystemExit):
                raise
            except Exception:
                self.handleError(record)

    log = logging.getLogger(__name__)
    log.setLevel(logging.INFO)
    handler = TqdmLoggingHandler()
    handler.setFormatter(
        colorlog.ColoredFormatter(
            "%(log_color)s %(name)-23s %(levelname)-7s %(message)s",
            log_colors={
                "DEBUG": "cyan",
                "INFO": "blue",
                "SUCCESS:": "green",
                "WARNING": "yellow",
                "ERROR": "red",
                "CRITICAL": "red,bg_white",
            },
        )
    )
    log.addHandler(handler)

    main()
