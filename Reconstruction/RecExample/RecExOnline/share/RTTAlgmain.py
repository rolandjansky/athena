import os, sys
import numpy as np
import ROOT

sys.path.append(os.path.join(os.path.dirname(__file__),'HistCompare'))
print(sys.path)
from RecExOnline.power_of_test import power_of_test
from RecExOnline.utils import get_array
import warnings
warnings.filterwarnings("ignore")

from RecExOnline.setupRTTAlg import *
#YJ add for athena
theApp.EvtMax = 1
#YJ add for html generator (5test & 1 plot)* total his 
w = 6*len(histnames) 
h =  7 
h_th =0
Matrix = [0 for x in range(w)]  
from collections import Counter, defaultdict
#YJ for check time for rel

import datetime
currenttime=datetime.datetime.now() 
rel=currenttime.isoweekday()
print "Time for RTT Alg : %d" %currenttime.hour
if currenttime.hour>15:
   rel=rel+1
if rel==7:
   rel=0
print "rel for RTT Alg : %d" %rel

#output format
header = dict([("Object", max(len(h) for h in histnames)),("Test",max(len(t.split("_")[0]) for t in tests)),("p-value",21)])
std_line = ["Object","Test","p-value"]
#check system
if not os.path.isdir(output_directory):
    os.makedirs(output_directory)
files = ROOT.TFile(ref_filename), ROOT.TFile(rtt_filename)
trees = [file.Get(treename) for treename, file in zip([ref_treename, rtt_treename], files)]
hist_pairs = [(histname, tuple(tree.Get(histname) for tree in trees)) for histname in histnames]

output_file = ROOT.TFile(os.path.join(output_directory, "power_of_test.root"), "RECREATE")
output_tree = ROOT.TTree("Tree", "Tree")
output_tree.SetEntries(size)

#main program
print std_line[0].ljust(header["Object"]), std_line[1].ljust(header["Test"]), std_line[2].center(header["p-value"])
print "="*header["Object"], "="*header["Test"], "="*header["p-value"]
for histname, hist_pair in hist_pairs:
    output_tree.Branch("/"+histname.replace("/","."))
    bins = np.intersect1d(*map(lambda hist: [hist.GetBinLowEdge(i) for i in range(1, hist.GetNbinsX()+2)], hist_pair))
    for hist in hist_pair: hist.SetBins(bins.size-1, bins)
    data1, data2 = map(get_array, hist_pair)
    address = np.zeros(1, 'float32')
    htmalhisname="tot_"+rtt_treename+"_"+histname.replace("/", "_")+".png"
    Matrix[h_th]=("<img src=\"https://atlas-rtt.cern.ch/prod/rtt/rel_%d/20.11.X.Y.Z-VAL/build/x86_64-slc6-gcc49-opt/AtlasP1MON/RecExOnline/RecExOnline_User_Offline_test_v1/" +htmalhisname+"\" alt=\""+htmalhisname+"\" height=\"250\">") %rel
    h_th+=1
    for test, p_arr in power_of_test(data1+data2, data2, tests = tests, rvs_key = {'size' : size, 'freeze' : (False, True), 'entries' : data2.sum()}.copy(), parallel = False).items():
        branchname = '.'.join([histname.replace('/','.'), test.split('_')[0]])
        branch = output_tree.Branch(branchname, address, "F")
        p_arr = np.array(p_arr).round(5)
        if test == "anderson_ksamp":
           p_arr[p_arr >= 1.2] = 0.
        print (histname if std_line[0]!=histname else "").ljust(header["Object"]), (test.split("_")[0] if std_line[1]!=test else "").ljust(header["Test"]), "{:>8} +/- {:<8} (%)".format(round(p_arr.mean()*100,1),round(p_arr.std()*100,2))
        Testresult= "%s %s:%s +- %s" % (histname,test ,p_arr.mean(),p_arr.std())
        if test =="ks_2samp" and (p_arr.mean()+p_arr.std()<0.05):
           Testresult= "<font color=\"red\">%s %s:%s +- %s </font>" % (histname,test ,p_arr.mean(),p_arr.std())
        Matrix[h_th]= Testresult
        h_th+=1
        print "matrix index check : %d "%(h_th)
        sys.stdout.flush()
        std_line[:2] = histname, test
        for p in p_arr:
            address[0] = p
            branch.Fill()
output_file.Write("", 2)
for f in files: f.Close()
output_file.Close()
print(Matrix)




f_html = open('RTTAlg.html', 'w')
f_html.write("<table> \n")
for nprint in range(w):
    f_html.write("<tr> \n")
    f_html.write("<td>" + Matrix[nprint] + "</td> \n")
    f_html.write("</tr> \n")
f_html.write("</table> \n")
