import ROOT
import sys

runnum = sys.argv[1].split('_')[0]
f = ROOT.TFile.Open(sys.argv[1], 'READ')
c1 = ROOT.TCanvas()
lumitree = f.lumitree
lumitree.Draw("zrate:lb+10:zratestat", "", "goff")
print 'Selected rows', lumitree.GetSelectedRows()
gr = ROOT.TGraphErrors(lumitree.GetSelectedRows(), lumitree.GetV2(), lumitree.GetV1(), ROOT.nullptr, lumitree.GetV3())
gr.Draw("ap")
gr.GetHistogram().SetXTitle('LB')
gr.GetHistogram().SetYTitle('Fiducial Z yield/second')
gr.SetMarkerStyle(20)
gr.SetTitle('')
c1.Update()
c1.Print('%s_fidyield.eps' % runnum)
# dump CSV
import time
csvout = open('%s_zrate.csv' % runnum, 'w')
lumitree.Draw("zrate:lbwhen[0]:lbwhen[1]:lhcfill:lblive:offlumi", "", "goff,para")
timeformat = '%y/%m/%d %H:%M:%S'
#timeformat = '%m/%d %H:%M:%S'
for i in xrange(lumitree.GetSelectedRows()):
    zrate = lumitree.GetV1()[i]
    instlumi = lumitree.GetVal(5)[i]
    livetime = lumitree.GetVal(4)[i]
    print >>csvout, '%d, %s, %s, %6f, %6f, %4f, %6f' % (lumitree.GetV4()[i],
                                                    time.strftime(timeformat, time.gmtime(lumitree.GetV2()[i])), 
                                                    time.strftime(timeformat, time.gmtime(lumitree.GetV3()[i])), 
                                                    lumitree.GetV1()[i],
                                                    instlumi/1e3,
                                                    instlumi*livetime/1e3,
                                                    lumitree.GetV1()[i]*livetime
                                                    )
csvout.close()

lumitree.Draw("zlumi:lb+10:zlumistat", "", "goff")
gr = ROOT.TGraphErrors(lumitree.GetSelectedRows(), lumitree.GetV2(), lumitree.GetV1(), ROOT.nullptr, lumitree.GetV3())
gr.Draw("ap")
gr.GetHistogram().SetXTitle('LB')
gr.GetHistogram().SetYTitle('Luminosity (x10^{33})')
gr.SetMarkerStyle(20)
gr.SetTitle('')
lumitree.Draw("offlumi:lb+10", "", "goff")
gr2 = ROOT.TGraphErrors(lumitree.GetSelectedRows(), lumitree.GetV2(), lumitree.GetV1())
gr2.SetMarkerStyle(21)
gr2.SetMarkerSize(0.5)
gr2.SetMarkerColor(ROOT.kRed)
gr2.Draw('same,pl')
leg = ROOT.TLegend(0.65, 0.7, 0.89, 0.89)
leg.SetBorderSize(0)
leg.AddEntry(gr, 'Z luminosity', 'pl')
leg.AddEntry(gr2, '-005 calibration', 'pl')
leg.Draw()
c1.Update()
c1.Print('%s_lumicomp.eps' % runnum)
