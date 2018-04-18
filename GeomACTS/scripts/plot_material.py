#!/usr/bin/env python
from __future__ import print_function
import ROOT
import argparse
import math

def style(h):
    h.SetFillColor(ROOT.kBlue)

ROOT.gROOT.SetBatch()
# ROOT.gROOT.SetStyle("ATLAS")
ROOT.gROOT.LoadMacro("../AtlasStyle.C") 
ROOT.SetAtlasStyle()

p = argparse.ArgumentParser()
p.add_argument("file")

args = p.parse_args()

file = ROOT.TFile.Open(args.file)
tree = file.Get("MaterialTracks")

c = ROOT.TCanvas()

hists = {}

def axislabels(h, x, y):
    h.GetXaxis().SetTitle(x)
    h.GetYaxis().SetTitle(y)

def mkprofile(name, xlab, ylab, *args):
    hists[name] = ROOT.TProfile(name, name, *args)
    axislabels(hists[name], xlab, ylab)


# hist_theta_x0 = ROOT.TProfile("theta", "theta", 40, 0, math.pi)
# hist_phi_x0 = ROOT.TProfile("phi", "phi", 40, -math.pi, math.pi)

mkprofile("theta_x0", '\\theta', "X_{0}", 40, 0, math.pi)
mkprofile("theta_tX0", '\\theta', "t/X_{0}", 40, 0, math.pi)
mkprofile("theta_t", '\\theta', "t", 40, 0, math.pi)
mkprofile("theta_tTot", '\\theta', "t_#text{tot}", 40, 0, math.pi)

mkprofile("phi_x0", "\\phi", "X_{0}", 40, -math.pi, math.pi)
mkprofile("phi_tX0", '\\phi', "t/X_{0}", 40, -math.pi, math.pi)
mkprofile("phi_t", '\\phi', 't', 40, -math.pi, math.pi)
mkprofile("phi_tTot", '\\phi', 't_#text{tot}', 40, -math.pi, math.pi)

mkprofile("eta_x0", '\\eta', 'X_{0}', 40, -4, 4)
mkprofile("eta_tX0", '\\eta', 't/X_{0}', 40, -4, 4)
mkprofile("eta_t", '\\eta', 't', 40, -6, 6)
mkprofile("eta_tTot", '\\eta', 't_#text{tot}', 40, -6, 6)

hist2D_eta_phi_x0 = ROOT.TH2D("eta_phi_x0", "eta_phi_x0", 40, -4, 4, 40, -math.pi, math.pi)
hist2D_eta_phi_x0_count = ROOT.TH2D("eta_phi_x0_count", "eta_phi_x0_count", 40, -4, 4, 40, -math.pi, math.pi)

axislabels(hist2D_eta_phi_x0, '\\eta', '\\phi')

for k, v in hists.iteritems():
    style(v)

ETA_CUT = 2.5

for idx, event in enumerate(tree):

    # if idx > 1000: break
    if idx % 1000 == 0:
        print("{} / {}".format(idx, tree.GetEntries()))

    X0 = event.X0
    theta = event.theta
    phi = event.phi

    eta = - math.log(math.tan(theta/2))
    
    hists["theta_x0"].Fill(theta, X0)
    hists["phi_x0"].Fill(phi, X0)

    tTot = 0

    if abs(eta) < ETA_CUT:
        hists["eta_x0"].Fill(eta, X0)
        hist2D_eta_phi_x0.Fill(eta, phi, X0)
        hist2D_eta_phi_x0_count.Fill(eta, phi)

    for t, x0 in zip(event.step_steplength, event.step_X0):
        tX0 = t/x0
        tTot += t

        hists["theta_tX0"].Fill(theta, tX0)
        hists["theta_t"].Fill(theta, t)

        hists["phi_tX0"].Fill(phi, tX0)
        hists["phi_t"].Fill(phi, t)

        if abs(eta) < ETA_CUT:
            hists["eta_tX0"].Fill(eta, tX0)

        hists["eta_t"].Fill(eta, t)

    hists["theta_tTot"].Fill(theta, tTot)
    hists["phi_tTot"].Fill(phi, tTot)
    if abs(eta) < ETA_CUT:
        hists["eta_tTot"].Fill(eta, tTot)

hist2D_eta_phi_x0.Divide(hist2D_eta_phi_x0_count)

hist2D_eta_phi_x0.Draw("colz")
c.SaveAs("hist/eta_phi_x0.pdf")

hists["phi_tX0"].GetYaxis().SetRangeUser(0, hists["phi_tX0"].GetMaximum()*1.2)
hists["phi_x0"].GetYaxis().SetRangeUser(0, hists["phi_x0"].GetMaximum()*1.2)
hists["phi_t"].GetYaxis().SetRangeUser(0, hists["phi_t"].GetMaximum()*1.2)
hists["phi_tTot"].GetYaxis().SetRangeUser(0, hists["phi_tTot"].GetMaximum()*1.2)

# hist_theta_x0.Draw("hist")
# c.SaveAs("hist_theta_x0.pdf")

# hist_phi_x0.Draw("hist")
# c.SaveAs("hist_phi_x0.pdf")

for k, v in hists.iteritems():
    v.Draw("")
    c.SaveAs("hist/{}.pdf".format(k))
