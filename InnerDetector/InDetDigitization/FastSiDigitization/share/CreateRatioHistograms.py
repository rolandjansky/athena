from ROOT import *

directories_sct=["barrel","endcap"]
directories_pixel=["barrel","endcap"]

directories={
		"pixel":directories_pixel,
		"sct":directories_sct
		}
list_histograms_pixel_pileup=["IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_pix_barrel_high_pileup","IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_pix_endcap_high_pileup","IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_pix_barrel_med_pileup","IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_pix_endcap_med_pileup"]
list_histograms_pixel_ttbar=["IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_pix_barrel_low_pileup","IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_pix_endcap_low_pileup"]
list_histograms_sct_pileup=["IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_sct_barrel_high_pileup","IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_sct_endcap_high_pileup","IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_sct_barrel_med_pileup","IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_sct_endcap_med_pileup"]
list_histograms_sct_ttbar=["IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_sct_barrel_low_pileup","IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_sct_endcap_low_pileup"]
##############################

def CreateOutputFileStructure(outputfile):


   for key,list in directories.iteritems() :
      directory = outputfile.mkdir(key);
      directory.cd()
      for location in list :
	  directory.mkdir(location);
   
   return outputfile

##############################

def ReadHistograms(filename,histograms,sensors,list_of_histograms):
   
   
   if sensors in histograms :
      print "key found"
   else :
      histograms[sensors] = {}
   
   for h_name in list_of_histograms :
      h = filename.Get(h_name)
      print h_name
      #h.SetDirectory(0)
      histograms[sensors][h_name]=h
      histograms[sensors][h_name].Print("all")
   
   return histograms

###############################

def CreateRatio(histograms_full,histograms_fast,outputfile):
  
   outputfile.cd()
   ratio_collection={}
   
   for key,hist_list in histograms_full.iteritems() :
      if key in histograms_fast :
	ratio_collection[key]={}
	for h_name,hist in hist_list.iteritems() :
	  if h_name in histograms_fast[key] :
	    
             plot_name=""

	     h_out_name=key
	     if "barrel" in h_name:
		h_out_name=h_out_name+"/barrel/"
		plot_name="barrel"
	     elif "endcap" in h_name:
		h_out_name=h_out_name+"/endcap/"
		plot_name="endcap"
	     else :
		continue
	    
             outputfile.cd(h_out_name)
         
	     if "high_pileup" in h_name:
		h_out_name="high_pileup"
	     elif "med_pileup" in h_name:
		h_out_name="med_pileup"
	     else : 
		h_out_name="low_pileup"

	     plot_name=plot_name+"_"+h_out_name
             print "%s %s"%(key,h_name)
    
	     
	     ratio=(histograms_fast[key][h_name]).Clone()
             ratio.SetName(h_out_name)
	     ratio.Divide(hist)
	     ratio.GetYaxis().SetTitle("Fast/Full")
	     print h_out_name
	     ratio.Write()
	     ratio_collection[key][plot_name]=ratio
   
   return  ratio_collection  

##############################
def DrawRatio(ratio_collection):

   c1={}
   legends={}
   
   colors={
      "med_pileup":1,
      "low_pileup":2,
      "high_pileup":4}

   gStyle.SetOptStat(0000)
   for sensor,plots in ratio_collection.iteritems() :
     c1[sensor]={}
     c1[sensor]["barrel"] = TCanvas(sensor+"barrel",sensor+"barrel")
     c1[sensor]["endcap"] = TCanvas(sensor+"endcap",sensor+"barrel")
      
     legends[sensor]={}
     legends[sensor]["barrel"]=TLegend(0.8,0.8,1.0,1.0)
     legends[sensor]["endcap"]=TLegend(0.8,0.8,1.0,1.0)
     
     
     for plot_name,plot in plots.iteritems() :
	 
	region="endcap"
	if "barrel" in plot_name:
	    region="barrel"

	c1[sensor][region].cd()
	plot.GetYaxis().SetRangeUser(0.98,1.3)
	plot.SetLineColor(colors[plot_name.split(region+"_")[-1]])
	plot.Draw("histsame")
	print ("%s %s %s")%(sensor,region,plot_name)
	legends[sensor][region].AddEntry(plot,plot_name.split(region+"_")[-1])
	

   for sensor,plots in c1.iteritems() :
     for region,c in plots.iteritems() :
          c.cd()
	  legends[sensor][region].Draw("same")
	  c.SaveAs(sensor+"_"+region+"_IneffSF.png")

##############################


if __name__ == '__main__':

  gROOT.SetBatch()
  Full_pileup_name="/afs/cern.ch/work/f/ffabbri/FastDigi/Post2018Validation/run/InDetRoots/pileup/Scan_Full_Full_NewPlot_052019/InDetStandardPlots.root"
  Full_pileup=TFile(Full_pileup_name)
  Full_ttbar_name="/afs/cern.ch/work/f/ffabbri/FastDigi/Post2018Validation/run/InDetRoots/ttbar_allhad/Scan_Full_Full_NewPlot_052019/InDetStandardPlots.root"
  Full_ttbar=TFile(Full_ttbar_name)

  Fast_pileup_pixel_name="/afs/cern.ch/work/f/ffabbri/FastDigi/Post2018Validation/run/InDetRoots/pileup/Scan_Pixel_inefficiency0.0_Inefficiency_NewPlot_052019/InDetStandardPlots.root"
  Fast_pileup_pixel=TFile(Fast_pileup_pixel_name)
  Fast_ttbar_pixel_name="/afs/cern.ch/work/f/ffabbri/FastDigi/Post2018Validation/run/InDetRoots/ttbar_allhad/Scan_Pixel_inefficiency0.0_Inefficiency_NewPlot_052019/InDetStandardPlots.root"
  Fast_ttbar_pixel=TFile(Fast_ttbar_pixel_name)

  Fast_pileup_sct_name="/afs/cern.ch/work/f/ffabbri/FastDigi/Post2018Validation/run/InDetRoots/pileup/Scan_barrel4.0_endcap2.0_ineffSF0.0_NewPlot_052019/InDetStandardPlots.root"
  Fast_pileup_sct=TFile(Fast_pileup_sct_name)
  Fast_ttbar_sct_name="/afs/cern.ch/work/f/ffabbri/FastDigi/Post2018Validation/run/InDetRoots/ttbar_allhad/Scan_barrel4.0_endcap2.0_ineff0.0_052019_NewPlot/InDetStandardPlots.root"
  Fast_ttbar_sct=TFile(Fast_ttbar_sct_name)

  outputfile_name="fast_pixel_inefficiency_SF.root"
  outputfile=TFile(outputfile_name,"RECREATE")

  outputfile=CreateOutputFileStructure(outputfile)
  
  histograms_full={}
  histograms_fast={}

  histograms_full=ReadHistograms(Full_ttbar,histograms_full,"pixel",list_histograms_pixel_ttbar)
  histograms_fast=ReadHistograms(Fast_ttbar_pixel,histograms_fast,"pixel",list_histograms_pixel_ttbar)
  

  histograms_full=ReadHistograms(Full_pileup,histograms_full,"pixel",list_histograms_pixel_pileup)
  histograms_fast=ReadHistograms(Fast_pileup_pixel,histograms_fast,"pixel",list_histograms_pixel_pileup)  

  histograms_full=ReadHistograms(Full_ttbar,histograms_full,"sct",list_histograms_sct_ttbar)
  histograms_fast=ReadHistograms(Fast_ttbar_sct,histograms_fast,"sct",list_histograms_sct_ttbar)
  

  histograms_full=ReadHistograms(Full_pileup,histograms_full,"sct",list_histograms_sct_pileup)
  histograms_fast=ReadHistograms(Fast_pileup_sct,histograms_fast,"sct",list_histograms_sct_pileup) 

  ratio_collection= CreateRatio(histograms_full,histograms_fast,outputfile)
  DrawRatio(ratio_collection)

  
