#!/usr/bin/env python
from __future__ import print_function
import ROOT
import argparse
import math
import os
import sys

# sys.path.append(os.path.dirname(__file__))
from geo_id import GeometryID


def style(h):
    h.SetFillColor(ROOT.kBlue)

def mkdir(path, recursive=True):
    cmd = ["mkdir"]
    if recursive: cmd += ["-p"]
    cmd += [path]
    os.system(" ".join(cmd))

def draw_eta_lines(hist, etas = [0, 1, 1.5, 2, 2.5, 2, 3, 4], sym=True):

    etas = etas + [-eta for eta in etas if eta != 0.]

    xmin = hist.GetXaxis().GetBinLowEdge(0)
    xmax = hist.GetXaxis().GetBinUpEdge(hist.GetXaxis().GetNbins())

    xext = min(map(abs, (xmin, xmax)))
    xmin = -xext
    xmax = xext

    ymax = hist.GetYaxis().GetBinUpEdge(hist.GetYaxis().GetNbins())

    l = ROOT.TLine()
    l.SetLineWidth(1)
    l.SetLineColor(ROOT.kRed)
    tl = ROOT.TLatex()
    tl.SetTextSize(0.025)
    tl.SetTextColor(ROOT.kRed)

    for eta in etas:
        theta = 2 * math.atan(math.exp(-eta))
        y = ymax
        x = y / math.tan(theta)
    
        m = y/x
        if x > xmax:
            y = m*xmax
            x = y/m
        if x < xmin:
            y = m*xmin
            x = y/m

        lab_offset = ymax * tl.GetTextSize()*0.5
        l.DrawLine(0, 0, x, y)
        if eta != 0:
            if x < 0:
                tl.SetTextAlign(11)
            else:
                tl.SetTextAlign(31)
        else:
            tl.SetTextAlign(21)
        tl.DrawLatex(x, y + lab_offset, "#eta = {:.1f}".format(eta))


class HistManager:
    def __init__(self):
        self._hist2D = {}
        self._hist2D_count = {}

    def _key(self, k):
        if type(k) == tuple:
            return "_".join(str(v) for v in k)
        else:
            return k

    def addHist2D(self, name, binx, xmin, xmax, biny, ymin, ymax, xlab="", ylab="", zlab="", average=False):
        key = self._key(name)
        root_name = os.path.basename(name)

        assert key not in self._hist2D, "hist2D with key {} exists".format(key)
        self._hist2D[key] = ROOT.TH2D(root_name, root_name, binx, xmin, xmax, biny, ymin, ymax)
        self._hist2D[key].GetXaxis().SetTitle(xlab)
        self._hist2D[key].GetYaxis().SetTitle(ylab)
        self._hist2D[key].GetZaxis().SetTitle(zlab)
        
        

        if average:
            self._hist2D_count[key] = ROOT.TH2D(root_name+"_count", root_name+"_count", binx, xmin, xmax, biny, ymin, ymax)

    def hist2DExists(self, name):
        key = self._key(name)
        return key in self._hist2D

    def fillHist2D(self, name, x, y, *args):
        key = self._key(name)
        assert key in self._hist2D, "{} not in histograms".format(key)
        self._hist2D[key].Fill(x, y, *args)
        if key in self._hist2D_count:
            self._hist2D_count[key].Fill(x, y)

    def hist2D(self, name):
        key = self._key(name)
        return self._hist2D[key]

    def hist2Diter(self):
        for name, hist in self._hist2D.items():
            if name not in self._hist2D_count:
                yield name, hist
            else:
                hist_divided = hist.Clone()
                hist_divided.Divide(self._hist2D_count[name])
                yield name, hist_divided
    
def axislabels(h, x, y):
    h.GetXaxis().SetTitle(x)
    h.GetYaxis().SetTitle(y)


def main():
    
    p = argparse.ArgumentParser()
    p.add_argument("geant")
    p.add_argument("mapped")
    p.add_argument("--events", "-n", type=int)
    p.add_argument("outdir")

    args = p.parse_args()


    ROOT.gROOT.SetBatch()
    # ROOT.gROOT.SetStyle("ATLAS")
    ROOT.gROOT.LoadMacro("../AtlasStyle.C") 
    ROOT.SetAtlasStyle()


    ROOT.gROOT.LoadMacro(os.path.join(os.path.dirname(__file__), "plot_helper.C"))
    geom_graph = ROOT.fill_geometry_hist_rz("excells_charged.root", 5000)

    c = ROOT.TCanvas("c", "c", 800, 600)

    geom_graph.Draw("ap")
    c.SaveAs(os.path.join(args.outdir, "geom_graph.png"))

    geant_file = ROOT.TFile.Open(args.geant)
    geant_tree = geant_file.Get("MaterialTracks")
    
    mapped_file = ROOT.TFile.Open(args.mapped)
    mapped_tree = mapped_file.Get("MaterialTracks")
    
    
    colors = [
        ROOT.kOrange,
        ROOT.kAzure,
        ROOT.kRed,
        ROOT.kViolet,
        ROOT.kGreen,
        ROOT.kPink,
        ROOT.kBlue,
        ROOT.kMagenta,
        ROOT.kCyan,
        ROOT.kTeal,
        ROOT.kSpring,
    ]
    # colors = [
        # "#e6194b",
        # "#3cb44b",
        # "#ffe119",
        # "#0082c8",
        # "#f58231",
        # "#911eb4",
        # "#46f0f0",
        # "#f032e6",
        # "#d2f53c",
        # "#fabebe",
        # "#008080",
        # "#e6beff",
        # "#aa6e28",
        # "#fffac8",
        # "#800000",
        # "#aaffc3",
        # "#808000",
        # "#ffd8b1",
        # "#000080"
    # ]

    vol_lay_col_map = {}
    vol_6_lay_idxs = range(2, 12+2, 2)
    vol_7_lay_idxs = range(2, 8+2, 2)
    vol_11_lay_idxs = range(2, 18+2, 2)
    vol_12_lay_idxs = range(2, 8+2, 2)

    for i, l in enumerate(vol_6_lay_idxs):
        vol_lay_col_map[(6, l)] = colors[i%len(colors)]

    for l1, l2 in zip(vol_6_lay_idxs, reversed(vol_6_lay_idxs)):
        vol_lay_col_map[(8, l1)] = vol_lay_col_map[(6, l2)]
    
    for i, l in enumerate(vol_11_lay_idxs):
        vol_lay_col_map[(11, l)] = colors[i%len(colors)]

    for l1, l2 in zip(vol_11_lay_idxs, reversed(vol_11_lay_idxs)):
        vol_lay_col_map[(13, l1)] = vol_lay_col_map[(11, l2)]

    for i, l in enumerate(vol_7_lay_idxs):
        vol_lay_col_map[(7, l)] = colors[i%len(colors)]
    for i, l in enumerate(vol_12_lay_idxs):
        vol_lay_col_map[(12, l)] = colors[i%len(colors)]


    # vol_lay_col_map[(6, 2)] = colors[0]
    # vol_lay_col_map[(6, 4)] = colors[1]
    # vol_lay_col_map[(6, 6)] = colors[2]
    # vol_lay_col_map[(6, 8)] = colors[3]
    # vol_lay_col_map[(6, 10)] = colors[4]
    # vol_lay_col_map[(6, 12)] = colors[5]

    # vol_lay_col_map[(8, 2)] = col_lay_col_map[(6, 12)]
    # vol_lay_col_map[(8, 4)] = col_lay_col_map[(6, 10)]
    # vol_lay_col_map[(8, 6)] = col_lay_col_map[(6, 8)]
    # vol_lay_col_map[(8, 8)] = col_lay_col_map[(6, 6)]
    # vol_lay_col_map[(8, 10)] = col_lay_col_map[(6, 4)]
    # vol_lay_col_map[(8, 12)] = col_lay_col_map[(6, 2)]

    def hex2rgb(h):
        h = h[1:]
        return tuple(int(h[i:i+2], 16) for i in (0, 2 ,4))


    layers_geant = do_step(c, geant_tree, os.path.join(args.outdir, "geant"), args.events, geom_graph)
    layers_mapped = do_step(c, mapped_tree, os.path.join(args.outdir, "mapped"), args.events, geom_graph)

    # c.SetWindowSize(1000, 600)
    c_all = ROOT.TCanvas("c_all", "c_all", 1000, 600)

    keys_geant = set(layers_geant.keys())
    keys_mapped = set(layers_mapped.keys())
    keys = keys_geant & keys_mapped

    for idx, key in enumerate(keys):
        geo_id = GeometryID(key)
        print(geo_id)

        h2_geant = layers_geant[key]
        h2_mapped = layers_mapped[key]

        def clip(h, val):
            for bx in range(h.GetNbinsX()):
                for by in range(h.GetNbinsY()):
                    bc = h.GetBinContent(bx, by)
                    if bc > 0:
                        h.SetBinContent(bx, by, val)

        clip(h2_geant, 1)
        clip(h2_mapped, 2)

        # for g in (g_geant, g_mapped):
            # g.SetMarkerSize(0.5)
        
        # color_idx = geo_id.lay_id / 2
        # print("col_idx:", color_idx)

        # color_a = colors[color_idx%len(colors)]

        color_a = vol_lay_col_map[(geo_id.vol_id, geo_id.lay_id)]

        # r, g, b = hex2rgb(colors[idx%len(colors)])
        # color_a = ROOT.TColor(r, g, b, 1.0)
        # color_b = ROOT.TColor(r, g, b, 0.6)

        # basecolnum, _ = colors[idx%len(colors)]
        # basecol = ROOT.gROOT.GetColor(basecolnum)
        # r, g, b = hex2rgb(basecol.AsHexString())



        # color_a = ROOT.TColor.GetColor(r, g, b)
        color_b = ROOT.TColor.GetColorTransparent(color_a, 0.6)

        # color_a = ROOT.TColor(r, g, b, 1.0)
        # color_b = ROOT.TColor(r, g, b, 0.6)



        # h2_geant.SetLineColor(color_b)
        h2_geant.SetLineColor(color_a)
        h2_mapped.SetLineColor(color_a)
        h2_mapped.SetLineStyle(2)
        h2_mapped.SetFillColor(color_a)

        for h in (h2_geant, h2_mapped):
            h.SetLineWidth(1)
        
        opt = "BOX"
        
        c.cd()
        h2_geant.Draw(opt)
        h2_mapped.Draw(opt+" sames")

        c_all.cd()

        if idx == 0:
            h2_geant.Draw(opt)
            h2_mapped.Draw(opt+" sames")
        else:
            opt = "BOX sames"
            h2_geant.Draw(opt)
            h2_mapped.Draw(opt)
        

        lay_str = "vol_{:03d}_lay_{:03d}_app_{:03d}.pdf".format(geo_id.vol_id, geo_id.lay_id, geo_id.app_id)
        path = os.path.join(args.outdir, "assoc", "{}.pdf".format(lay_str))
        mkdir(os.path.dirname(path))
        c.SaveAs(path)

    path = os.path.join(args.outdir, "assoc.pdf")
    mkdir(os.path.dirname(path))
    c_all.SaveAs(path)

   



def do_step(c, tree, outdir, events, geom_graph):
    hm = HistManager()

    hists = {}

    def mkprofile(name, xlab, ylab, *args):
        hists[name] = ROOT.TProfile(name, name, *args)
        axislabels(hists[name], xlab, ylab)

    mkprofile("theta_x0",    '\\theta', "X_{0}",   40, 0, math.pi)
    mkprofile("theta_dInX0", '\\theta', "t/X_{0}", 40, 0, math.pi)
    mkprofile("theta_t",     '\\theta', "t",       40, 0, math.pi)
    mkprofile("theta_tTot",  '\\theta', "t_{tot}", 40, 0, math.pi)

    mkprofile("phi_x0",    "\\phi", "X_{0}",   40, -math.pi, math.pi)
    mkprofile("phi_dInX0", '\\phi', "t/X_{0}", 40, -math.pi, math.pi)
    mkprofile("phi_t",     '\\phi', 't',       40, -math.pi, math.pi)
    mkprofile("phi_tTot",  '\\phi', 't_{tot}', 40, -math.pi, math.pi)

    mkprofile("eta_x0",    '\\eta', 'X_{0}',   40, -4, 4)
    mkprofile("eta_dInX0", '\\eta', 't/X_{0}', 40, -4, 4)
    mkprofile("eta_t",     '\\eta', 't',       40, -4, 4)
    mkprofile("eta_tTot", '\\eta',  't_{tot}', 40, -4, 4)

    nBinsEta = 80
    nBinsPhi = 80
    nBinsR = 200
    nBinsZ = 500


    hm.addHist2D("eta_phi/eta_phi_x0", nBinsEta, -4, 4, nBinsPhi, -math.pi, math.pi, "\\eta", "\\phi", average=True)
    hm.addHist2D("eta_phi/eta_phi_dInX0", nBinsEta, -4, 4, nBinsPhi, -math.pi, math.pi, "\\eta", "\\phi", average=True)
    hm.addHist2D("eta_phi/eta_phi_tTot", nBinsEta, -4, 4, nBinsPhi, -math.pi, math.pi, "\\eta", "\\phi", average=True)

    hm.addHist2D("z_r/z_r_x0", nBinsZ, -3000, 3000, nBinsR, 0, 1200, "z [mm]", "r [mm]", "X_{0}", average=True)

    for k, v in hists.iteritems():
        style(v)

    layers = {}


    for idx, event in enumerate(tree):

        if events:
            if idx > events: break
        if idx % 1000 == 0:
            print("{} / {}".format(idx, tree.GetEntries()))

        # X0 = event.X0
        theta = event.theta
        phi = event.phi

        eta = - math.log(math.tan(theta/2))
        

        tTot = 0

        dInX0_tot = event.dInX0
        
        totalX0 = 0

        zipped = zip(event.step_t, 
                     event.step_X0, 
                     event.step_dInX0, 
                     event.step_z, 
                     event.step_r,
                     event.step_geo_id)
        for t, x0, dInX0, z, r, _geo_id in zipped:
            geo_id = GeometryID(_geo_id)
            # tX0 = t/x0
            tTot += t
            totalX0 += x0

            # if geo_id.value() not in layers:
                # layers.append(geo_id.value())

            lay_str = "vol_{}_lay_{}_app_{}".format(geo_id.vol_id, geo_id.lay_id, geo_id.app_id)
            if _geo_id not in layers:
                hname = lay_str+"_"+outdir+"_z_r"
                layers[_geo_id] = ROOT.TH2D(hname, hname, nBinsZ/2, -3000, 3000, nBinsR/2, 0, 1200)

            h2 = layers[_geo_id]
            # g.SetPoint(g.GetN(), z, r)
            h2.Fill(z, r)

            hm.fillHist2D("z_r/z_r_x0", z, r, x0)
            
            # hname = "z_r/lay/z_r_"+lay_str+"_x0"
            # if not hm.hist2DExist(hname):
                # hm.hist2D

        
        hists["theta_x0"].Fill(theta, totalX0)
        hists["phi_x0"].Fill(phi, totalX0)
        hists["eta_x0"].Fill(eta, totalX0)

        hm.fillHist2D("eta_phi/eta_phi_x0", eta, phi, totalX0)
        hm.fillHist2D("eta_phi/eta_phi_tTot", eta, phi, tTot)
        hm.fillHist2D("eta_phi/eta_phi_dInX0", eta, phi, dInX0_tot)

        hists["theta_dInX0"].Fill(theta, dInX0_tot)
        hists["phi_dInX0"].Fill(phi, dInX0_tot)
        hists["eta_dInX0"].Fill(eta, dInX0_tot)

        hists["theta_tTot"].Fill(theta, tTot)
        hists["phi_tTot"].Fill(phi, tTot)
        hists["eta_tTot"].Fill(eta, tTot)


    hists["phi_dInX0"].GetYaxis().SetRangeUser(0, hists["phi_dInX0"].GetMaximum()*1.2)
    hists["phi_x0"].GetYaxis().SetRangeUser(0, hists["phi_x0"].GetMaximum()*1.2)
    hists["phi_t"].GetYaxis().SetRangeUser(0, hists["phi_t"].GetMaximum()*1.2)
    hists["phi_tTot"].GetYaxis().SetRangeUser(0, hists["phi_tTot"].GetMaximum()*1.2)

    os.system("mkdir -p {}".format(outdir))

    for k, v in hists.iteritems():
        v.Draw("hist")
        c.SaveAs("{}/{}.pdf".format(outdir, k))

    c.SetRightMargin(0.2)
    c.SetTopMargin(0.1)

    for name, h2 in hm.hist2Diter():

        parts = name.split("/")
        print(parts)
        if "z_r" in parts:
            h2.Draw("colz")
            draw_eta_lines(h2)
            filename = "{}.pdf".format(os.path.join(outdir, name))
            mkdir(os.path.dirname(filename))
            c.SaveAs(filename)
            
            h2.Draw("colz")
            geom_graph.Draw("p+same")
            draw_eta_lines(h2)
            filename = "{}_geom.pdf".format(os.path.join(outdir, name))
            mkdir(os.path.dirname(filename))
            c.SaveAs(filename)

        else:
            h2.Draw("colz")
            filename = "{}.pdf".format(os.path.join(outdir, name))
            mkdir(os.path.dirname(filename))
            c.SaveAs(filename)

    # for h2 in hist2D_zr:
        # h2.Divide(hist2D_zr_count)
        # h2.Draw("colz")

        # h2.GetXaxis().SetTitle("z [mm]")
        # h2.GetYaxis().SetTitle("r [mm]")

        # draw_eta_lines(h2)
        # c.SaveAs("{}/{}.pdf".format(args.outdir, h2.GetName()))
        
        # geom_graph.Draw("p+same")
        # c.SaveAs("{}/{}_w_geom.pdf".format(args.outdir, h2.GetName()))

    # for h2 in hist2D_eta_phi:
        # h2.Divide(hist2D_eta_phi_count)
        # h2.Draw("colz")
        # c.SaveAs("{}/{}.pdf".format(args.outdir, h2.GetName()))

    return layers


if "__main__" == __name__:
    main()
