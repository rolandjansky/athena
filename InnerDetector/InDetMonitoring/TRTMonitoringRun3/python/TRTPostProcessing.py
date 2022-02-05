#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
def hOccupancy(inputs):
    import ROOT
    element = inputs[0][0]['element']
    name = 'hOccupancy' + element
    if element == 'C': title = 'Chip' 
    if element == 'S': title = 'Straw' 
    rh = ROOT.TH1F(name, title + ' Occupancy Distribution', 201, 0, 1.005)
    rh.GetXaxis().SetTitle('Occupancy')
    rh.GetYaxis().SetTitle('Number of ' + title + 's')
    plots = [_[1][0] for _ in inputs]  # all plots passed as first element of list
    for plot in plots:
        nBins = plot.GetNbinsX()
        for j in range(nBins): #range of the input histogram, not the output
            rh.Fill(plot.GetBinContent(j + 1))
    return [rh]

def hHitXonTMap(inputs):
    import ROOT
    hist = inputs[1][0]['histogram'][9:] #removing "unscaled_" prefix
    element = inputs[1][0]['element']
    if element == 'S':
        name  = hist + 'S'
        titleElement = 'Straws'
    if element == 'C':
        name  = hist + 'C'
        titleElement = 'Chips'
    if hist == 'hHitHWonTMap': title = 'HL Hit(In time Window) on Track: '
    if hist == 'hHitWonTMap':  title = 'Leading Edge on Track in Time Window: '
    if hist == 'hHitAonTMap':  title = 'Any LL Bit on Track: '
    if hist == 'hHitAWonTMap': title = 'Any LL Bit on Track in Time Window: '
    if hist == 'hHitHonTMap':  title = 'HL Hit on Track: '
    rh = ROOT.TH1F(name, title + titleElement, 1, 0, 1)
    rh.GetXaxis().SetTitle(titleElement[:-1] + ' Number in Stack')
    rh.GetYaxis().SetTitle('Probability')
    for i in range(len(inputs[0][1])):
        plots = [_[1][i] for _ in inputs]
        if i < 3:
            for m in range(int(len(plots)/2)):
                plot0 = plots[2*m]
                plot1 = plots[2*m + 1]
                nBins = plots[0].GetNbinsX()
                rh.SetBins(nBins, 0, nBins)
                for j in range(nBins): #range of the input histogram, not the output
                    if plot0.GetBinEntries(j + 1) > 0:
                        rh.SetBinContent(j + 1, plot1.GetBinContent(j + 1)*1./plot0.GetBinEntries(j + 1))
                    else:
                        rh.SetBinContent(j + 1, 0)
    return [rh]
    
def hHitOnTrackVsAll(inputs):
    import ROOT
    element = inputs[0][0]['element']
    if element == 'S':
        name  ='S'
        titleElement = 'Straws'
    if element == 'C':
        name  = 'C'
        titleElement = 'Chips'
    rh = ROOT.TH1F('hHitonTrackVsAll' + name, '(Hit on Track) / (Any LL Bit): ' + titleElement, 1, 0, 1)
    rh.GetXaxis().SetTitle(titleElement[:-1] + ' Number in Stack')
    rh.GetYaxis().SetTitle('Ratio')
    for i in range(len(inputs[0][1])):
        plots = [_[1][i] for _ in inputs]
        if i < 4:
            for m in range(int(len(plots)/3)):
                plot0 = plots[3*m]
                plot1 = plots[3*m + 1]
                plot2 = plots[3*m + 2]
                nBins = plots[0].GetNbinsX()
                rh.SetBins(nBins, 0, nBins)
                for j in range(nBins): #range of the input histogram, not the output
                    if (plot0.GetBinContent(j + 1)*plot1.GetBinContent(j + 1)) > 0:
                        rh.SetBinContent(j + 1, plot2.GetBinContent(j + 1)/(plot0.GetBinContent(j + 1)*plot1.GetBinContent(j + 1)))
                    else:
                        rh.SetBinContent(j + 1, 0)
    return [rh]

def hEfficiency(inputs):
    import ROOT
    region = inputs[0][0]['region']
    side = inputs[0][0]['side']
    if region == 'Barrel':
        name  = 'hEfficiencyBarrel' + str(side)
    if region == 'Endcap':
        name  = 'hEfficiencyEndCap' + str(side)
    rh = ROOT.TH1F(name, 'Straw Efficiency (' + name[11:] + ')', 500, -0.01, 1.01)
    rh.GetXaxis().SetTitle('Efficiency')
    rh.GetYaxis().SetTitle('Number of Straws')
    for i in range(len(inputs[0][1])):
        plots = [_[1][i] for _ in inputs]
        for plot in plots:
            for nStraw in range(plot.GetXaxis().GetNbins()):
                rh.Fill(plot.GetBinContent(nStraw + 1))
    return [rh]

def hEfficiencyIntegral(inputs):
    import ROOT
    region = inputs[0][0]['region']
    side = inputs[0][0]['side']
    if region == 'Barrel':
        name  = 'hEfficiencyBarrel' + str(side)
    if region == 'Endcap':
        name  = 'hEfficiencyEndCap' + str(side)
    rh = ROOT.TH1F(name, 'Straw Efficiency (' + name[11:] + ')', 500, -0.01, 1.01)
    rh.GetXaxis().SetTitle('Efficiency')
    rh.GetYaxis().SetTitle('Fraction of Straws')
    for i in range(len(inputs[0][1])):
        plots = [_[1][i] for _ in inputs]
        for plot in plots:
            for nStraw in range(plot.GetXaxis().GetNbins()):
                rh.Fill(plot.GetBinContent(nStraw + 1))
    totalEntries = 0.
    entries = rh.GetEntries()
    for nStraw in range(rh.GetXaxis().GetNbins()):
        totalEntries += rh.GetBinContent(nStraw + 1)
        rh.SetBinContent(nStraw + 1, totalEntries/float(entries))
    return [rh]