# Description:
#  This file contains functions used for comparing to monitoring.root files 
# Author:
#  John Alison johnda@hep.upenn.edu

include("InDetAlignmentMonitoring/fittingFunctions.py")

SetStats = 0
ScaleReference = False
thirdFile = False
thirdColor = 1
t_hist = 0    

def procProfile(profileName, path, min = 0, max = 0):
    m_prof = b_file.Get(path+profileName)
    if m_prof:
        m_prof.SetMarkerStyle(20)
        m_prof.SetMarkerColor(monitoredColor)
        m_prof.SetLineColor(monitoredColor)
        m_prof.GetXaxis().SetLabelSize(0.07)
        m_prof.GetYaxis().SetLabelSize(0.07)
        if min != 0 and max != 0:
            m_prof.SetMinimum(min)
            m_prof.SetMaximum(max)
        m_prof.SetStats(SetStats)
        m_prof.Draw()
    else:
        print path+profileName, "not found in",beforeAlignmentFile
    
    r_prof = a_file.Get(path+profileName)
    if r_prof:
        r_prof.SetMarkerStyle(20)
        r_prof.SetMarkerColor(referenceColor)
        r_prof.SetLineColor(referenceColor)
        r_prof.SetStats(SetStats)
        r_prof.Draw("sames")
    else:
        print path+profileName, "not found in",afterAlignmentFile

    if thirdFile: 
        t_prof = t_file.Get(path+profileName)
        if t_prof:
            t_prof.SetMarkerStyle(20)
            t_prof.SetMarkerColor(thirdColor)
            t_prof.SetLineColor(thirdColor)
            t_prof.SetStats(SetStats)
            t_prof.Draw("sames")
        else:
            print path+profileName, "not found."



def procAllProfiles(profileNames, path, file, colors, title, min = 0, max = 0):
    prof = []
    for i in range(len(profileNames)):
        prof.append(file.Get(path+profileNames[i]))
        if prof[i]:
            prof[i].SetMarkerStyle(20)
            prof[i].SetMarkerColor(colors[i])
            prof[i].SetLineColor(colors[i])
            prof[i].SetLineWidth(2)
            prof[i].GetXaxis().SetLabelSize(0.07)
            prof[i].GetYaxis().SetLabelSize(0.07)
            if min != 0 and max != 0:
                prof[i].SetMinimum(min)
                prof[i].SetMaximum(max)
            prof[i].SetStats(SetStats)
            if i==0:
                prof[i].Draw()
            else:
                prof[i].Draw("sames")
        else:
            print path+profileNames[0], "not found."
    

def processAndFitHistogram(histName, path, units):
    print "Called on " ,histName
    r_hist = a_file.Get(path+histName)
    m_hist = b_file.Get(path+histName)
    if thirdFile: 
        t_hist= t_file.Get(path+histName)
    
    if r_hist and m_hist:
        if ScaleReference and r_hist.GetEntries():
            r_hist.Scale(m_hist.GetEntries()/r_hist.GetEntries())
        r_hist.SetMarkerColor(referenceColor)
        r_hist.SetMarkerStyle(20)
        r_hist.SetLineColor(referenceColor)
        r_hist.Draw("")
        #doubleFit(r_hist,units,referenceColor,0.2,0.88,0)
    else:
        print path+histName, "not found."


    if m_hist:
        m_hist.SetMarkerColor(monitoredColor)
        m_hist.SetMarkerStyle(20)
        m_hist.SetLineColor(monitoredColor)
        m_hist.Draw("sames")
        #doubleFit(m_hist,units,monitoredColor,0.2,0.78,1)
    else:
        print path+histName, "not found."

    if thirdFile and t_hist:
        t_hist.SetMarkerColor(thirdColor)
        t_hist.SetMarkerStyle(20)
        t_hist.SetLineColor(thirdColor)
        t_hist.Draw("sames")
        #doubleFit(m_hist,units,monitoredColor,0.2,0.78,1)
    elif thirdFile:
        print path+histName, "not found."



def processAndFitTRTResiduals(histName, path, units="#mum"):
    r_hist = a_file.Get(path+histName)
    m_hist = b_file.Get(path+histName)
    if thirdFile: 
        t_hist= t_file.Get(path+histName)
        
    if r_hist and m_hist:
        if ScaleReference:
            r_hist.Scale(m_hist.GetEntries()/r_hist.GetEntries())
        r_hist.SetMarkerColor(referenceColor)
        r_hist.SetMarkerStyle(7)
        r_hist.SetLineColor(referenceColor)
        r_hist.Draw("")
        TRTResidualFit(r_hist,units,referenceColor,0.2,0.88,0)
    else:
        print path+histName, "not found."

    if m_hist:
        m_hist.SetMarkerColor(monitoredColor)
        m_hist.SetMarkerStyle(7)
        m_hist.SetLineColor(monitoredColor)
        m_hist.Draw("sames")
        TRTResidualFit(m_hist,units,monitoredColor,0.2,0.83,1)
    else:
        print path+histName, "not found."

    if thirdFile and t_hist:
        t_hist.SetMarkerColor(thirdColor)
        t_hist.SetMarkerStyle(7)
        t_hist.SetLineColor(thirdColor)
        t_hist.Draw("sames")
        TRTResidualFit(t_hist,units,thirdColor,0.2,0.78,1)
    elif thirdFile:
        print path+histName, "not found."

def processAndQuoteHistogram(histName, path, units):
    r_hist = a_file.Get(path+histName)
    if r_hist:
        r_hist.SetMarkerColor(referenceColor)
        r_hist.SetLineColor(referenceColor)
        r_hist.Draw("")
        quoteMeanAndRMS(r_hist,units,referenceColor,0.12,0.85,0)
    else:
        print path+histName, "not found in",afterAlignmentFile
        
    m_hist = b_file.Get(path+histName)
    if m_hist:
        m_hist.SetMarkerColor(monitoredColor)
        m_hist.SetLineColor(monitoredColor)
        m_hist.Draw("sames")
        quoteMeanAndRMS(m_hist,units,monitoredColor,0.12,0.8,1)
    else:
        print path+histName, "not found in",beforeAlignmentFile
    

def procHistogram(histName, path, stats=0):
    r_hist = a_file.Get(path+histName)
    m_hist = b_file.Get(path+histName)
    if thirdFile: 
        t_hist= t_file.Get(path+histName)

    if m_hist:
        m_hist.SetMarkerStyle(20)
        m_hist.SetMarkerColor(monitoredColor)
        m_hist.SetLineColor(monitoredColor)
        # m_hist.SetMaximum(0.2)
        # m_hist.SetMinimum(-0.2)
        m_hist.SetStats(stats)
        m_hist.Draw()
    else:
        print path+histName, "not found in",beforeAlignmentFile
    
    if r_hist and m_hist:
        if ScaleReference:
            r_hist.Scale(m_hist.GetEntries()/r_hist.GetEntries())
        r_hist.SetMarkerStyle(20)
        r_hist.SetMarkerColor(referenceColor)
        r_hist.SetLineColor(referenceColor)
        r_hist.SetStats(stats)
        r_hist.Draw("same")
    else:
        print path+histName, "not found in",afterAlignmentFile 

    if thirdFile and t_hist:
        t_hist.SetMarkerStyle(20)
        t_hist.SetMarkerColor(thirdColor)
        t_hist.SetLineColor(thirdColor)
        t_hist.SetStats(stats)
        t_hist.Draw("same")
    elif thirdFile:
        print path+histName, "not found."

def procHitMap(histName,path, after):
    hist = 0
    if after:
        hist = a_file.Get(path+histName)
    else:
        hist = b_file.Get(path+histName)
    if hist:
        hist.SetStats(0)
        gStyle.SetPalette(1)
        hist.Draw("colz")
    else:
        print path+histName, "not found."

    
