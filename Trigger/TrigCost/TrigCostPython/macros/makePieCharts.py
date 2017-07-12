#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys, string, math, shutil
from TrigCostPython import TrigCostAnalysis, TrigCostXML
import ROOT

#--------------------------------------------------------------------
#  List of colors for the pie chart
#  WARNING - if the pie chart is larger than the number of colors
#            some colors will be repeated 
#--------------------------------------------------------------------
slice_colors = [1, 2, 3, 4, 5, 6, 7, 8, 9, 46, 14, 0, 38, 28, 49, 30]

def main() :

    #-----------------------------------------------------------------------
    #  Options definitions
    #-----------------------------------------------------------------------
    from optparse import OptionParser
    
    p = OptionParser(usage='<rates xml> -o <output location>')
    
    p.add_option('-f', '--xml-file', type='string', default = None,
                dest = 'file', help = 'path to TriggerRate.xml file'     )
    
    p.add_option('-k', '--group-key', type='string', default='BW', 
                dest = 'group_key', help = 'String indicating set of groups to use (BW, stream)'  )
    
    p.add_option('','--root-file', type='string', default=None,
                dest='root_file', help='Name of output root file, no file created otherwise' )
   
    p.add_option('-o', '--output', type='string', default=None,
                dest='output', help='Location of the output png files, no pngs created otherwise' )

    (options, args) = p.parse_args()
    
    if options.file == None :
        print 'Please provide an input TriggerRate.xml file'
        sys.exit(-1)
    

    MakePieCharts(options.file, options.group_key, options.root_file, options.output)

def MakePieCharts(input, key, root_file = None, output=None) :
    ROOT.gStyle.SetTitleFillColor(10)
    created_files = []
    if key != 'BW' :
        print 'Sorry, only bandwidth groups are currently supported'
        return created_files

    results = TrigCostXML.ReadXmlFile(input)
    L2_pie_groups = []
    EF_pie_groups = []
    for chain in results.GetChains():
        if chain.GetName().find(key) != -1 :
            if chain.GetName().find('L2') != -1 :
                L2_pie_groups.append(results.GetChain(chain.GetName()))
            if chain.GetName().find('EF') != -1 :
                EF_pie_groups.append(results.GetChain(chain.GetName()))

    rfile = None
    if root_file != None :
        rfile = ROOT.TFile(root_file, 'RECREATE')
    
    pies = []

    pies += MakeChart(L2_pie_groups, 'L2') 
    pies += MakeChart(EF_pie_groups, 'EF') 

    if root_file :
        for p in pies :
            p.Write()
    if output :
        for p in pies :
            can = ROOT.TCanvas(p.GetName(), p.GetName())
            can.SetFillColor(10)
            can.SetFrameFillColor(10)
            p.Draw('3d')
            can.SaveAs('%s/%s.png' %(output, p.GetName()))

    return pies

def MakeChart(pie_groups, level) :
    #-----------------------------------------------------
    #  Get the total rate
    #----------------------------------------------------
    
    if len(pie_groups) == 0 :
        print 'MakeChart - group list empty...will not make pie chart'
        return []
    total_rate = 0.0
    for gr in pie_groups :
        total_rate += gr.rate

    if total_rate > 0: total_rate = 1.0/total_rate
    else:              total_rate = 0.0
    
    pie_name = level + '_Bandwidth'    
    

    n_slices = len(pie_groups)
    #don't count small slices
    for gr in pie_groups :
        if gr.rate*total_rate <= 0.05 :
            n_slices -= 1
    #add in "other" slice
    if n_slices < len(pie_groups) :
        n_slices += 1
    #-----------------------------------------------------
    #  Create the pie chart
    #-----------------------------------------------------                
    pie_chart = ROOT.TPie(pie_name,pie_name, n_slices)
    pie_chart_rate = ROOT.TPie(pie_name + "_rates", pie_name + "_rates", n_slices)
    pie_chart_frac = ROOT.TPie(pie_name + "_frac", pie_name + "_frac", n_slices)
    pie_counter = 0
    col_counter = 0
    #-----------------------------------------------------------
    #  Fill the pie chart.  The rates are the values in pie_groups
    #  The labels are the keys in pie_groups, and the colors are
    #  selected in order from the defined list of colors
    #-----------------------------------------------------------
    other_rate = 0.0
    for gr in pie_groups :
        if col_counter == len(slice_colors) :
            col_counter = 0
       
 
        name = gr.name[gr.name.find(':')+1:]
        rate = gr.rate

        if rate*total_rate < 0.05 :
            other_rate += rate
            continue

        pie_chart.SetEntryVal(pie_counter, rate)
        pie_chart.SetEntryLabel(pie_counter, name)
        pie_chart.SetEntryFillColor(pie_counter, slice_colors[col_counter])
        pie_chart.SetLabelsOffset(0.05)
        
        pie_chart_rate.SetEntryVal(pie_counter, rate)
        pie_chart_rate.SetEntryLabel(pie_counter, name)
        pie_chart_rate.SetEntryFillColor(pie_counter, slice_colors[col_counter])
        pie_chart_rate.SetLabelsOffset(0.05)
    
        pie_chart_frac.SetEntryVal(pie_counter, rate*total_rate)
        pie_chart_frac.SetEntryLabel(pie_counter, name)
        pie_chart_frac.SetEntryFillColor(pie_counter, slice_colors[col_counter])
        pie_chart_frac.SetLabelsOffset(0.05)
        pie_counter += 1
        col_counter += 1
    if other_rate > 0.0 :
        pie_chart.SetEntryVal(pie_counter, other_rate)
        pie_chart.SetEntryLabel(pie_counter, 'Other')
        pie_chart.SetEntryFillColor(pie_counter, slice_colors[col_counter])
        pie_chart.SetEntryLineColor(pie_counter, slice_colors[col_counter])

        pie_chart_rate.SetEntryVal(pie_counter, other_rate)
        pie_chart_rate.SetEntryLabel(pie_counter, 'Other')
        pie_chart_rate.SetEntryFillColor(pie_counter, slice_colors[col_counter])
        pie_chart_rate.SetEntryLineColor(pie_counter, slice_colors[col_counter])
    
        pie_chart_frac.SetEntryVal(pie_counter, other_rate*total_rate)
        pie_chart_frac.SetEntryLabel(pie_counter, 'Other')
        pie_chart_frac.SetEntryFillColor(pie_counter, slice_colors[col_counter])
        pie_chart_frac.SetEntryLineColor(pie_counter, slice_colors[col_counter])
        
    #---------------------------------------------------------
    #  Do some configuration, then write
    #---------------------------------------------------------
    pie_chart_rate.SetLabelFormat("#splitline{%txt}{%val Hz}")
    pie_chart_frac.SetLabelFormat("#splitline{%txt}{%val}")
    pie_chart.SetAngle3D(80)
    pie_chart_rate.SetAngle3D(80)
    pie_chart_frac.SetAngle3D(80)
    pie_chart.SetTextSize(0.035)
    pie_chart_rate.SetTextSize(0.035)
    pie_chart_frac.SetTextSize(0.035)
    pie_chart.SetRadius(pie_chart.GetRadius()*0.6)
    pie_chart_rate.SetRadius(pie_chart_rate.GetRadius()*0.6)
    pie_chart_frac.SetRadius(pie_chart_frac.GetRadius()*0.6)
    return [pie_chart, pie_chart_rate, pie_chart_frac]
    
if __name__ == '__main__' :
    main()
