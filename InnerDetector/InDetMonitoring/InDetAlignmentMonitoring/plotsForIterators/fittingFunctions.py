# Author:
#  John Alison johnda@hep.upenn.edu

from ROOT import TF1,TLatex,TCanvas
writeOut = False

def TRTResidualFit(hist,units,color,xLeft,yLow,same):
    hist.SetAxisRange(-1,1)

    # To get the fit range
    fitRange = TF1("fitRange", "gaus", -0.3,0.3)
    hist.Fit("fitRange", "ORQ","same")
    min = -1.5*fitRange.GetParameter(2);
    max = 1.5*fitRange.GetParameter(2);
    
    #print "Fitting from",min,"to",max

    FZ01 = TF1("FZ01", "gaus", min,max)
    FZ01.SetLineColor(color)
    FZ01.SetLineWidth(1)
    hist.Fit("FZ01", "ORQ","same")
    
    FZ01.Draw("same")
    hist.Draw("same")
    
    # Draw the after mean and sigma
    text = "#mu="
    #text += str((1000*round(FZ01.GetParameter(1),5)))
    text += str(int(1000*FZ01.GetParameter(1)))
    text += " "+units+", #sigma="
    #text += str((1000*round(FZ01.GetParameter(2),4)))
    text += str(int(1000*FZ01.GetParameter(2)))
    text += " "+units
    m_l = TLatex()
    m_l.SetTextSize(0.04)
    m_l.SetLineColor(color)
    m_l.SetTextColor(color)
    m_l.SetTextAlign(12)
    m_l.SetNDC()
    m_l.DrawLatex(xLeft,yLow,text)


def singleGaus(hist,units,color,xLeft,yLow,same,title):
    #print "InSingle gaus"

    can = TCanvas(title,title)
    can.cd()
    hist.Draw()
    
    min = -0.5#hist.GetMinimum()
    max = 0.5#hist.GetMaximum()
    
    print "Fitting from",min,"to",max
    FZ01 = TF1("FZ01", "gaus", min,max)
    FZ01.SetLineColor(color)
    FZ01.SetLineWidth(1)
    hist.Fit("FZ01", "ORQ","same")
    
    FZ01.Draw("same")
    hist.Draw("same")
    
    # Draw the after mean and sigma
    text = "#mu="
    text += str(int(1000*FZ01.GetParameter(1)))
    #text += str(round(FZ01.GetParameter(1),4))
    text += " "+units+", #sigma="
    text += str(int(1000*FZ01.GetParameter(2)))
    #text += str(round(FZ01.GetParameter(2),4))
    text += " "+units

    m_l = TLatex()
    m_l.SetTextSize(0.04)
    m_l.SetTextAlign(12)
    m_l.SetNDC()
    m_l.DrawLatex(xLeft,yLow,text)

    if writeOut:
        can.SaveAs(title+".png")

    return m_l,can,FZ01,hist 

def doubleGaus(hist,units,color,xLeft,yLow,same,fitInput,title):

    can = TCanvas(title,title)
    can.cd()
    hist.Draw()

    #min = hist.GetMean() - 1 * hist.GetRMS()
    #max = hist.GetMean() + 1 * hist.GetRMS()
    min = -0.5
    max = 0.5
    #print "Fitting from",min,"to",max
    FZ01 = TF1("FZ01","[0]*exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-[4])/[5])**2)",min,max)
    FZ01.SetParameter(0,fitInput[0])
    FZ01.SetParameter(1,fitInput[1])
    FZ01.SetParameter(2,fitInput[2])
    FZ01.SetParameter(3,fitInput[3])
    FZ01.SetParameter(4,fitInput[4])
    FZ01.SetParameter(5,fitInput[5])
    
    FZ01.SetLineColor(color)
    FZ01.SetLineWidth(1)
    hist.Fit("FZ01", "ORQ","same")

    
    FZ01.Draw("same")
    hist.Draw("same")
    
    text = "#mu="
    text += str(int(1000*FZ01.GetParameter(1)))
    
    print 1000*FZ01.GetParameter(1)
    print 1000*FZ01.GetParameter(2)
    print 1000*FZ01.GetParameter(4)
    print 1000*FZ01.GetParameter(5)

    #text += str(round(FZ01.GetParameter(1),4))
    text += " "+units+", #sigma="
    text += str(int(1000*FZ01.GetParameter(2)))
    #text += str(round(FZ01.GetParameter(2),4))    
    text += " "+units

    m_l = TLatex()
    m_l.SetTextSize(0.04)
    m_l.SetTextAlign(12)
    m_l.SetNDC()
    m_l.DrawLatex(xLeft,yLow,text)

    if writeOut:
        can.SaveAs(title+".png")
    
    return m_l,can,FZ01,hist 

def doubleFit(hist,units,color,xLeft,yLow,same):
    min_temp = hist.GetBinLowEdge(0)
    max_temp = hist.GetBinLowEdge(hist.GetNbinsX())
    fit_temp = TF1("fit_temp","gaus",min_temp,max_temp)
    if same:
        hist.Fit("fit_temp", "ORQ","same")
    else:
        hist.Fit("fit_temp", "ORQ","")
    fitMean_temp = fit_temp.GetParameter(1)
    fitSig_temp  = fit_temp.GetParameter(2)
    min = fitMean_temp - 1.5* fitSig_temp
    max = fitMean_temp + 1.5 * fitSig_temp
    fit = TF1("fit","gaus",min,max )
    fit.SetLineColor(color)
    if same:
        hist.Fit("fit", "ORQ","same")
    else:
        hist.Fit("fit", "ORQ","")
    fit.Draw("same")

    # Draw the after mean and sigma
    text = "#mu="
    text += str(round(fit.GetParameter(1),5))
    text += " "+units+", #sigma="
    text += str(round(fit.GetParameter(2),4))
    text += " "+units
    m_l = TLatex()
    m_l.SetLineColor(color)
    m_l.SetTextSize(0.03)
    m_l.SetTextAlign(12)
    m_l.SetNDC()
    m_l.SetTextColor(color)
    print text
    m_l.DrawLatex(xLeft,yLow,text)

def quoteMeanAndRMS(hist,units,color,xLeft,yLow,same):
    
    # Draw the after mean and sigma
    text = "mean = "
    text += str(round(hist.GetMean(),5))
    text += " "+units+", RMS = "
    text += str(round(hist.GetRMS(),4))
    text += " "+units
    m_l = TLatex()
    m_l.SetLineColor(color)
    m_l.SetTextSize(0.03)
    m_l.SetTextAlign(12)
    m_l.SetNDC()
    m_l.SetTextColor(color)
    #print text
    m_l.DrawLatex(xLeft,yLow,text)
