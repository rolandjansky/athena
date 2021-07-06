#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
def effcomb(inputs):
    import ROOT
    rv = ROOT.TH1F('SctEffDistribution;Efficiency;Links', 'SCT Efficiency Distribution', 500, 0, 1)
    plots = [_[1][0] for _ in inputs]  # all plots passed as first element of list
    rv.GetXaxis().SetTitle('Efficiency')
    rv.GetYaxis().SetTitle('Links')
    for plot in plots:
        for xbin in range(plot.GetNbinsX()):
            for ybin in range(plot.GetNbinsY()):
                if(plot.GetBinEntries(plot.GetBin(xbin,ybin))==0):
                    continue
                rv.Fill(plot.GetBinContent(xbin,ybin))
    return [rv]

def conf(inputs, label, thr):
    import ROOT
    rh = ROOT.TH1F('SCTConfNew', 'Num of Problematic Modules in All Region', 5, -0.5, 4.5)
    for l in range(len(label)):
        rh.GetXaxis().SetBinLabel(l+1,label[l])
    for i in range(len(inputs[0][1])):
        plots = [_[1][i] for _ in inputs]  # all plots passed as first element of list
        if i < 3:
            #for plot in plots:
            for m in range(int(len(plots)/2)):
                plot0 = plots[2*m]
                plot1 = plots[2*m+1]
                for xbin in range(plot0.GetNbinsX()):
                    for ybin in range(plot0.GetNbinsY()):
                        if (plot0.GetBinContent(xbin+1,ybin+1) > thr[i] or plot1.GetBinContent(xbin+1,ybin+1) > thr[i]):
                             rh.Fill(i)
        else:
        #use only first plot as all others are replicas (only for Masked and flagged links plots)
            rh.Fill(i,plots[0].GetBinContent(4))
    return [rh]
