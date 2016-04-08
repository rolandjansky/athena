# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from ROOT import TGraph, TH1F

def plot(runlist, countsl2,countsef):

    print "Start plotting"

    for run in runlist:

        print "Run",run

        example = countsef[run]


        gr = TGraph( len(example) )
        #gr.SetName ( "graph%i" % (ik+1) )
        #gr.SetTitle( varnames[ik] )

        for i, (lbinfo, lbcounts) in enumerate(example):
            total = lbcounts.Total if lbcounts else 0
            print "%i/%i: %1.1f Hz" % (run, lbinfo.lb, float(total)/lbinfo.duration)

            gr.SetPoint( i, lbinfo.lb, total )



        #gr.Draw("")

        h = TH1F("h","h",10,0,10)
        h.Fill(5)
        h.Draw()
