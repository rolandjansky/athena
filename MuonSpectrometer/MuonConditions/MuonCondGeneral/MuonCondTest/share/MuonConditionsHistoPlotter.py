#!/usr/bin/env python
#
# read a root tree using Pyroot and find muon track
#
#
import os
import time
from ROOT import TTree,TH2F,gStyle,TAxis
from ROOT import TCanvas, TFile


#Settings 
# Style for plotting
gStyle.SetOptStat(0)
gStyle.SetPalette(1)	
gStyle.SetCanvasColor(33)
gStyle.SetFrameFillColor(0)


def labelhisto_dead(histo_deadchamb):   
	histo_deadchamb.GetXaxis().SetTitle("Eta id")
	histo_deadchamb.GetYaxis().SetTitle("Phi id")
	for i in range(1, histo_deadchamb.GetNbinsX()+1):		
		if(i<=histo_deadchamb.GetNbinsX()/2.):	
			histo_deadchamb.GetXaxis().SetBinLabel(i, "A"+str(int(histo_deadchamb.GetNbinsX()/2.+0.5)-i) )
		elif(i==int(histo_deadchamb.GetNbinsX()/2.+0.5)):
			histo_deadchamb.GetXaxis().SetBinLabel(i, "B" )	
		else:	
			histo_deadchamb.GetXaxis().SetBinLabel(i, "C"+str(i-int(histo_deadchamb.GetNbinsX()/2.+0.5)) )	
			
			
def labelhisto_tube(histo_deadtube):   
	histo_deadtube.GetYaxis().SetTitle("tube id")
	histo_deadtube.GetXaxis().SetTitle("mlayer/layer id")
	histo_deadtube.GetYaxis().SetNdivisions(1010)
	for i in range(1, histo_deadtube.GetNbinsX()+1):			
		histo_deadtube.GetXaxis().SetBinLabel(i, "m"+str((i-1)/4+1)+"l"+str((i-1)%4+1))
					




file=TFile("ConditionsSummaryMeta.root")
T_tubes=file.Get("t_deadtube")

deadtubes_barrel_in = TH2F("deadtubes_barrel_in","deadtubes_barrel_in",17,-8.5,8.5,16,0.5,16.5)
deadtubes_endcap_in = TH2F("deadtubes_endcap_in","deadtubes_endcap_in",17,-8.5,8.5,16,0.5,16.5)
deadtubes_barrel_out = TH2F("deadtubes_barrel_out","deadtubes_barrel_out",17,-8.5,8.5,16,0.5,16.5)
deadtubes_endcap_out = TH2F("deadtubes_endcap_out","deadtubes_endcap_out",17,-8.5,8.5,16,0.5,16.5)
deadtubes_barrel_mid = TH2F("deadtubes_barrel_mid","deadtubes_barrel_mid",17,-8.5,8.5,16,0.5,16.5)
deadtubes_endcap_mid = TH2F("deadtubes_endcap_mid","deadtubes_endcap_mid",17,-8.5,8.5,16,0.5,16.5)
deadtubes_endcap_ee = TH2F("deadtubes_endcap_ee","deadtubes_endcap_ee",17,-8.5,8.5,16,0.5,16.5)
temp=0


for iev in T_tubes :
	
	temp+=1
#	if(temp==40): break
	print iev.deadtube
	#print temp, temp%divider
	c_temp_tubes = TCanvas("c_tubes_"+str(iev.deadtube).split("_")[0], "Dead tubes in " + str(iev.deadtube).split("_")[0],500,800)
	c_temp_tubes.SetGrid(1)
	h_temp_tubes=TH2F("h_tubes_"+str(iev.deadtube).split("_")[0],"h_tubes_"+str(iev.deadtube).split("_")[0],8,1,9,72,0.5,72.5)
	temp_str= str(iev.deadtube).split("_")[1].split(",")
	deadtubenumber=len(str(iev.deadtube).split("_")[1].split(","))-1
	print deadtubenumber
	for line in temp_str:
		if line == "" : break
		#print line
		#print len(line)
		#print line,"multilayer : ", line[0]," layer : ", line[1]," tube : ", line[2]
		if(len(line)==3):tube_id=int(line[2])
		elif(len(line)==4): tube_id=int(line[2]+line[3])
		h_temp_tubes.Fill(int(line[1])+(int(line[0])-1.)*4.,tube_id)
#		print   int(line[1]), "+ (", int(line[0]), "-1.) ","*4"
#		print "Filling : " , int(line[1])+(int(line[0])-1.)*4.
	c_temp_tubes.SetFillColor(10)
	labelhisto_tube(h_temp_tubes)
		
	h_temp_tubes.Draw("colz")
	c_temp_tubes.SaveAs("Dead_tubes_"+str(iev.deadtube).split("_")[0]+".jpg")				
	# print float(iev.deadtube[5]+iev.deadtube[6])
#	print iev.deadtube[3] 
	
	if(iev.deadtube[4]=='A'):
		AC_idhelper=-1
	elif(iev.deadtube[4]=='C'):
		AC_idhelper=1
	elif(iev.deadtube[4]=='B'):
		AC_idhelper=0
	else:
		print "ERROr not known chamber id"			
	eta_tubeid = float(iev.deadtube[3])
#	print AC_idhelper," , ", AC_idhelper*eta_tubeid
	
	if(iev.deadtube[0]=='E'):
		if(iev.deadtube[1]=='I'):deadtubes_endcap_in.Fill(AC_idhelper*eta_tubeid,float(iev.deadtube[5]+iev.deadtube[6]),deadtubenumber)
		elif(iev.deadtube[1]=='M'):deadtubes_endcap_mid.Fill(AC_idhelper*eta_tubeid,float(iev.deadtube[5]+iev.deadtube[6]),deadtubenumber)
		elif(iev.deadtube[1]=='O'):deadtubes_endcap_out.Fill(AC_idhelper*eta_tubeid,float(iev.deadtube[5]+iev.deadtube[6]),deadtubenumber)
		elif(iev.deadtube[1]=='E'):deadtubes_endcap_ee.Fill(AC_idhelper*eta_tubeid,float(iev.deadtube[5]+iev.deadtube[6]),deadtubenumber)
		else: print "ERROr chamber does not hit requirements"		
	elif(iev.deadtube[0]=='B'):
		if(iev.deadtube[1]=='I'):deadtubes_barrel_in.Fill(AC_idhelper*eta_tubeid,float(iev.deadtube[5]+iev.deadtube[6]),deadtubenumber)
		elif(iev.deadtube[1]=='M'):deadtubes_barrel_mid.Fill(AC_idhelper*eta_tubeid,float(iev.deadtube[5]+iev.deadtube[6]),deadtubenumber)
		elif(iev.deadtube[1]=='O'):deadtubes_barrel_out.Fill(AC_idhelper*eta_tubeid,float(iev.deadtube[5]+iev.deadtube[6]),deadtubenumber)
		else: print "ERROr chamber does not hit requirements"	
#		print iev.deadtube 
	

	else:
		print "ERROr chamber does not hit requirements"	

labelhisto_dead(deadtubes_barrel_in)
labelhisto_dead(deadtubes_barrel_mid)
labelhisto_dead(deadtubes_barrel_out)
labelhisto_dead(deadtubes_endcap_in)
labelhisto_dead(deadtubes_endcap_mid)
labelhisto_dead(deadtubes_endcap_out)
labelhisto_dead(deadtubes_endcap_ee)

c_tubes_endcap = TCanvas("c_tubes_endcap"," Dead tubes in endcap ",1200,1200)
c_tubes_endcap.SetFillColor(10)

c_tubes_endcap.Divide(2,2)
c_tubes_endcap.cd(1)
deadtubes_endcap_in.Draw("colz")
c_tubes_endcap.cd(2)
deadtubes_endcap_mid.Draw("colz")
c_tubes_endcap.cd(3)
deadtubes_endcap_out.Draw("colz")
c_tubes_endcap.cd(4)
deadtubes_endcap_ee.Draw("colz")
time.sleep(5)
c_tubes_endcap.SaveAs("tubes_endcap.jpg")


c_tubes_barrel = TCanvas("c_tubes_barrel"," Dead tubes in ebarrel ",1200,1200)
c_tubes_barrel.SetFillColor(10)

c_tubes_barrel.Divide(2,2)
c_tubes_barrel.cd(1)
deadtubes_barrel_in.Draw("colz")
c_tubes_barrel.cd(2)
deadtubes_barrel_mid.Draw("colz")
c_tubes_barrel.cd(3)
deadtubes_barrel_out.Draw("colz")
time.sleep(5)
c_tubes_barrel.SaveAs("tubes_barrel.jpg")




# dead chambers
T_chamb=file.Get("t_deadstations")
   
deadchambers_barrel = TH2F("h_deadchamb_barrel","h_deadchamb_barrel",17,-8.5,8.5,16,0.5,16.5)
deadchambers_endcap = TH2F("h_deadchamb_endcap","h_deadchamb_endcap",17,-8.5,8.5,16,0.5,16.5)

for iev in T_chamb :
	print iev.deadstat
# 	print float(iev.deadstat[5]+iev.deadstat[6])
#	print iev.deadstat[3] 
	eta_statid = float(iev.deadstat[3])
	if(iev.deadstat[4]=='A' and  iev.deadstat[0]=='E'):
		deadchambers_endcap.Fill(-eta_statid,float(iev.deadstat[5]+iev.deadstat[6]))
	elif(iev.deadstat[4]=='C' and iev.deadstat[0]=='E'):               
	        deadchambers_endcap.Fill(eta_statid,float(iev.deadstat[5]+iev.deadstat[6]))
	elif(iev.deadstat[4]=='A' and iev.deadstat[0]=='B'):
#                print iev.deadstat
		deadchambers_barrel.Fill(-eta_statid,float(iev.deadstat[5]+iev.deadstat[6]))
	elif(iev.deadstat[4]=='C' and iev.deadstat[0]=='B'):
#		print iev.deadstat
		deadchambers_barrel.Fill(eta_statid,float(iev.deadstat[5]+iev.deadstat[6]))
	elif(iev.deadtube[4]=='B'):
#		print iev.deadstat
		deadchambers_barrel.Fill(eta_statid,float(iev.deadstat[5]+iev.deadstat[6]))	
	else:
		print "ERROr chamber does not hit requirements"	
		

labelhisto_dead(deadchambers_barrel)
labelhisto_dead(deadchambers_endcap)

c_chambers = TCanvas("c_chambers","DeadChambers in endcap/barrel ",1000,500);

c_chambers.SetFillColor(10)
c_chambers.Divide(2,1)
c_chambers.cd(1)
deadchambers_endcap.Draw("colz")
c_chambers.cd(2)
deadchambers_barrel.Draw("colz")
c_chambers.SaveAs("deadchamb.jpg")










