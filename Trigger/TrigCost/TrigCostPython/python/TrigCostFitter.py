from ROOT import TFile, TTree, TIter, TGraph, TF1, TH1, TCanvas, TH1F
from ROOT import gROOT
from ROOT import TColor, kBlack, kCyan, kBlue, kRed, kOrange, kYellow, kGreen
from array import array
import logging
logging.basicConfig(level=logging.INFO)
log = logging.getLogger('TrigCostFitter.py')
import numpy as np

class TriggerFitObject(object):
    def __init__(self, name, level = "None", runno = "999999"):
        self.chain_name = name
        self.level = level
        self.runno = runno

        self.xsec_raw = []
        self.mu_raw = []
        self.xsec_filtered = []
        self.mu_filtered = []

    def Read(self, ntp_tree, ps_value=1):
        if self.level == "L1" : prefix = "xbp__"
        if self.level == "HLT" : prefix = "xap__"
        fit_PS = ps_value
        branches = ntp_tree.GetListOfBranches()
        found_trg = False
        found_mu = False
        xsec_read0 = []
        mu_read0 = []
        if (prefix + self.chain_name) in branches: found_trg = True
        if ("evt__mu") in branches: found_mu = True
        if (not found_mu and found_tr):
            self.active = False
            log.warning("Didn't find neccessary information for trigger %s" % self.chain_name)
            return None
        log.info("Now reading data for %s" % self.chain_name)
        count = 0
        nentries = ntp_tree.GetEntries()
        if nentries < 1:
            log.error("Null nentries")
            return None
        for count in range(nentries):
            if (count % 100 == 0) : log.info("Now processing entry == %d" % count)
            if (count % fit_PS != 0) : continue
            ntp_tree.GetEntry(count)
#            if (ntp_tree.__getattr__( "xbp__" + self.chain_name) > 10) : continue # This showed that the fit is very thrown off by extreme points
            xsec_read0.append( ntp_tree.__getattr__( prefix + self.chain_name))
            mu_read0.append(   ntp_tree.__getattr__("evt__" + "mu"))
#            self.xsec_raw.append(ntp_tree.__getattr__( "xbp__" + self.chain_name))
#            self.mu_raw.append(ntp_tree.__getattr__("evt__" + "mu"))
        mean = np.mean(xsec_read0)
        bound = 1.5 * np.std(xsec_read0)
        log.info("****************************************")
        log.info("\tCrude bounds about mean = %2.5f +- %2.5f"%(mean,bound))
        self.bounds = bound

        for key, value in enumerate(xsec_read0):
            if (value >= mean + bound or value <= mean - bound) : continue
            if (mu_read0[key] < 20 or mu_read0[key] > 55) : continue
            self.xsec_raw.append(xsec_read0[key])
            self.mu_raw.append(mu_read0[key])
        log.info("\tPassed Crude Filter = %2.2f %%" % (100 * float(len(self.xsec_raw))/len(xsec_read0)))
        log.info("****************************************")
    def GenerateGraph(self):
        color = kBlack
        ptitle = "Trigger Cross-section vs Pileup for %s" % self.chain_name
        xtitle = 'Pileup [#mu] = <interactions> / <bunch-crossing>'
        ytitle = 'Cross-section [#sigma] = <rate> / <lumi>'
        graph = TGraph(len(self.mu_raw), array('f', self.mu_raw), array('f', self.xsec_raw))
        graph.SetTitle('%s;%s;%s' % (ptitle, xtitle, ytitle) )
        graph.SetMarkerColor(color)
        graph.SetMarkerColor(color)
        graph.SetLineColor(kCyan)
        graph.SetFillColor(color)
        self.graph_raw = graph
    def FitFirst(self):
        p = np.polyfit(self.mu_raw, self.xsec_raw, 1)
        fitfn='[0]*x + [1]'
        fit_x = TF1("f_trial", fitfn, 20, 100)
        fit_x.SetParameter(0, p[0])
        fit_x.SetParameter(1, p[1])
#        fit_x.SetParameter(2, p[2])
        print(fit_x.Eval(40))
        gROOT.SetBatch(True)
        c1 = TCanvas( 'c1', 'The Fit Canvas', 200, 10, 700, 500 )
        self.graph_raw.Draw("AP")
#        self.graph_raw.GetYaxis().SetRangeUser(0,0.3)
        fit_x.Draw("SAME")
        c1.Update()
        c1.SaveAs("./plots/rawfit__%s.pdf" % (self.chain_name))
        self.fit_raw = fit_x
    def FitGaus(self, scale):
        bound = 0.5 * self.bounds
        h_yres = TH1F("h_yres", "Residuals", 100, -bound,bound)
        for key,value in enumerate(self.mu_raw):
            h_yres.Fill(self.xsec_raw[key] - self.fit_raw.Eval(value))
        gROOT.SetBatch(True)
        c1 = TCanvas( 'c2', 'The Residual Canvas', 200, 10, 700, 500 )
        h_yres.Draw()
        fit_x = TF1("f_gaus", "gaus", -bound, bound)
        fit_x.SetLineColor(kBlue-8)
        fit_x.SetLineWidth(3)
        h_yres.Fit("f_gaus", "RWME")
        fit_x.Draw("SAME")
        c1.Update()
        c1.SaveAs("./plots/residuals__%s.pdf" % (self.chain_name))
        log.info("****************************************")
        log.info("\tFitted mean of residuals:   mu_hat = %6.3f +- %5.3f"%(fit_x.GetParameter(1), fit_x.GetParError(1)))
        log.info("\tFitted sigma of residuals:  sig_hat = %6.3f +- %5.3f"%(fit_x.GetParameter(2), fit_x.GetParError(2)))
        upper_bnd = float(fit_x.GetParameter(1) + scale * fit_x.GetParameter(2) )
        lower_bnd = float(fit_x.GetParameter(1) - scale * fit_x.GetParameter(2) )
        self.xsec_filtered = []
        self.mu_filtered = []
        log.info("\tLower Bound:  %3.3f"%(lower_bnd))
        log.info("\tUpper Bound:  %3.3f"%(upper_bnd))
        for key,value in enumerate(self.mu_raw):
            yres = float(self.xsec_raw[key] - self.fit_raw.Eval(value))
            if (yres >= upper_bnd) or (yres <= lower_bnd) : continue
            self.xsec_filtered.append(self.xsec_raw[key])
            self.mu_filtered.append(self.mu_raw[key])
        log.info("\tPassed Residuals Filter = %2.2f %%" % (100 * float(len(self.xsec_filtered))/len(self.xsec_raw)))
        log.info("****************************************")

        # Define bounds of tight, medium, loose, and rejected bound on yres
        upper_bnd_1 = float(fit_x.GetParameter(1) + 0.50 * scale * fit_x.GetParameter(2) )
        lower_bnd_1 = float(fit_x.GetParameter(1) - 0.50 * scale * fit_x.GetParameter(2) )
        upper_bnd_2 = float(fit_x.GetParameter(1) + 0.75 * scale * fit_x.GetParameter(2) )
        lower_bnd_2 = float(fit_x.GetParameter(1) - 0.75 * scale * fit_x.GetParameter(2) )
        upper_bnd_3 = float(fit_x.GetParameter(1) + 1.00 * scale * fit_x.GetParameter(2) )
        lower_bnd_3 = float(fit_x.GetParameter(1) - 1.00 * scale * fit_x.GetParameter(2) )

        # Setup
        gROOT.SetBatch(True)
        c1 = TCanvas( 'c1', 'The Outlier Canvas', 200, 10, 700, 500 )
        ptitle = "Trigger Cross-section vs Pileup for %s" % self.chain_name
        xtitle = 'Pileup [#mu] = <interactions> / <bunch-crossing>'
        ytitle = 'Cross-section [#sigma] = <rate> / <lumi>'
        graph1 = TGraph()
        graph2 = TGraph()
        graph3 = TGraph()
        graph4 = TGraph()

        # Set styles
        graph1.SetTitle('%s;%s;%s' % (ptitle, xtitle, ytitle) )
        graph2.SetTitle('%s;%s;%s' % (ptitle, xtitle, ytitle) )
        graph3.SetTitle('%s;%s;%s' % (ptitle, xtitle, ytitle) )
        graph4.SetTitle('%s;%s;%s' % (ptitle, xtitle, ytitle) )
        graph1.SetMarkerColor(kGreen)
        graph2.SetMarkerColor(kYellow)
        graph3.SetMarkerColor(kOrange)
        graph4.SetMarkerColor(kRed)
        graph1.SetMarkerStyle(24)
        graph2.SetMarkerStyle(24)
        graph3.SetMarkerStyle(25)
        graph4.SetMarkerStyle(25)
        graph1.SetMarkerSize(0.5)
        graph2.SetMarkerSize(0.5)
        graph3.SetMarkerSize(0.5)
        graph4.SetMarkerSize(0.5)

        # Deliniate into separt graphs
        for key,value in enumerate(self.mu_raw):
            yres = float(self.xsec_raw[key] - self.fit_raw.Eval(value))
            if (yres <= upper_bnd_1) and (yres >= lower_bnd_1) :
                graph1.SetPoint(graph1.GetN(), value, self.xsec_raw[key])
            elif (yres <= upper_bnd_2) and (yres >= lower_bnd_2) :
                graph2.SetPoint(graph2.GetN(), value, self.xsec_raw[key])
            elif (yres <= upper_bnd_3) and (yres >= lower_bnd_3) :
                graph3.SetPoint(graph3.GetN(), value, self.xsec_raw[key])
            else :
                graph4.SetPoint(graph4.GetN(), value, self.xsec_raw[key])

        graph1.Draw("AP")
        graph2.Draw("P SAME")
        graph3.Draw("P SAME")
        graph4.Draw("P SAME")
        self.fit_raw.Draw("SAME")
        c1.Update()
        c1.SaveAs("./plots/outliers__%s.pdf" % (self.chain_name))


    def FitFiltered(self):
        color = kBlack
        ptitle = "Filtered Trigger Cross-section vs Pileup for %s" % self.chain_name
        xtitle = 'Pileup [#mu] = <interactions> / <bunch-crossing>'
        ytitle = 'Cross-section [#sigma] = <rate> / <lumi>'
        graph = TGraph(len(self.mu_filtered), array('f', self.mu_filtered), array('f', self.xsec_filtered))
        graph.SetTitle('%s;%s;%s' % (ptitle, xtitle, ytitle) )
        graph.SetMarkerColor(color)
        graph.SetMarkerColor(color)
        graph.SetLineColor(kCyan)
        graph.SetFillColor(color)
        self.graph_filtered= graph

        p1, res1, _, _, _ = np.polyfit(self.mu_filtered, self.xsec_filtered, 1, full=True)
        p2, res2, _, _, _ = np.polyfit(self.mu_filtered, self.xsec_filtered, 2, full=True)
        fitfn='[0] + [1]*x + exp([2] + [3]*(x - 60))'
        self.function_string = fitfn
        fit_x = TF1("f_filter", fitfn, 20, 100)
        self.graph_filtered.Fit("f_filter", "RWEM0")
        fit_pol1 = TF1("f_pol1_filter", '[0] + [1]*x', 20, 100)
        fit_pol2 = TF1("f_pol2_filter", '[0] + [1]*x + [2]*x^2', 20 , 100)
        fit_pol1.SetParameter(0, p1[1])
        fit_pol1.SetParameter(1, p1[0])

        fit_pol2.SetParameter(0, p2[2])
        fit_pol2.SetParameter(1, p2[1])
        fit_pol2.SetParameter(2, p2[0])

        fit_pol1.SetLineColor(2)
        fit_pol2.SetLineColor(3)
        fit_x.SetLineColor(4)


        chi2_spec = fit_x.GetChisquare()
        log.info("***************************************")
        log.info(" -- pol1 fit    = %f" % res1)
        log.info(" -- pol2 fit    = %f" % res2)
        log.info(" -- spec fit    = %f" % chi2_spec)
        log.info(" -- pol2 improvement = %2.1f %%" % (100 * (res2- res1) / res1 ))
        log.info(" -- spec improvement = %2.1f %%" % (100 * (chi2_spec - res2) / res2 ))
        log.info("***************************************")

        gROOT.SetBatch(True)
        c1 = TCanvas( 'c3', 'The ReFit Canvas', 200, 10, 700, 500 )
        self.graph_filtered.Draw("AP")
#        self.graph_raw.GetYaxis().SetRangeUser(0,0.3)
        fit_x.Draw("SAME")
        fit_pol1.Draw("SAME")
        fit_pol2.Draw("SAME")
        c1.Update()
        c1.SaveAs("./plots/filteredfit__%s.pdf" % (self.chain_name))
        self.fit_filtered = fit_x
        pol2_imp = 100.0 * ((res2- res1) / res1 )
        spec_imp = 100.0 * ((chi2_spec - res2) / res2 )
        # Select fit!
        self.fit_selected = fit_pol1
        if pol2_imp < -1.0 : self.fit_selected = fit_pol2
        if spec_imp < -1.0 : self.fit_selected = fit_x
        if (self.chain_name == "L1_MU20") : self.fit_selected = fit_pol1
    def Write (self, F):
        nparam = self.fit_selected.GetNpar()
        par0 = 0.0
        par1 = 0.0
        par2 = 0.0
        par3 = 0.0
        if (nparam >= 1): par0 = self.fit_selected.GetParameter(0)
        if (nparam >= 2): par1 = self.fit_selected.GetParameter(1)
        if (nparam >= 3): par2 = self.fit_selected.GetParameter(2)
        if (nparam >= 4): par3 = self.fit_selected.GetParameter(3)
        writeout = """
    {
        "yaxis":"%s",
        "run":"%s",
        "function":"%s",
        "rms":0.0
        "par":[
            {
                "id":"0",
                "value":"%5.15lf"
            },
            {
                "id":"1",
                "value":"%5.15lf"
            },
            {
                "id":"2",
                "value":"%5.15lf"
            },
            {
                "id":"3",
                "value":"%5.15lf"
            }
        ]
    },\n""" % (self.chain_name, self.runno, self.fit_selected.GetExpFormula(), par0, par1, par2, par3)
        F.write(writeout)
