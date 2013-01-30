#!/usr/bin/env python

import sys,os
from CoolRunQuery.AtlRunQueryLookup import InitDetectorMaskDecoder, DQChannels
from ROOT import TFile, TGraph, TH1I

def execute( fname ):
    file = open( fname, 'r' )

    # first count number of lines in file and find run numbers
    ic = -1
    minRun = 1e20
    maxRun = -1
    for line in file:
        if '--------------------' in line:
            if ic<0: 
                ic = 0
                continue
            else: break
        if ic >= 0: 
            # get run number
            r, sep, line = line.partition(':')
            runno = int(r.strip('run').strip())
            if runno > maxRun: maxRun = runno
            if runno < minRun: minRun = runno
            ic += 1       

    print 'Found: %i runs in file; run range: [%i, %i]' % (ic,minRun, maxRun)
    maxRun += 1
    minRun -= 1

    # --- read detectors ----

    print ' '
    print 'Reading detectors and DQ flags'
    (dName, NotInAll) = InitDetectorMaskDecoder()
    dqchans           = DQChannels()

    # create graphs and histograms
    rootfile = TFile.Open( "queryresults.root", "RECREATE" )        
    detgraphs = {}
    dethists  = {}
    for det in dName:
        detc = det.strip().replace(' ','_')        
        if 'unknown' in detc: continue

        detgraphs[det] = TGraph( ic )
        detgraphs[det].SetName( 'Det_%s_vs_run_graph' % detc )
        dethists[det] = TH1I( 'Det_%s_vs_run_hist' % detc, 'Det_%s_vs_run_hist' % detc, maxRun-minRun+1, minRun, maxRun )

    dqhists   = {}
    for key, dq in dqchans.items():
        dqhists[dq] = TH1I( 'DQ_%s_vs_run_hist' % dq, 'DQ_%s_vs_run_hist' % dq, maxRun-minRun+1, minRun, maxRun )

    # read lines
    ip = -1
    file.seek(0)
    for line in file:
        # search for start of output block
        if '--------------------' in line:
            for line in file:
                if '--------------------' in line: break
                # get run number
                r, sep, line = line.partition(':')
                ip += 1
                runno = int(r.strip('run').strip())

                # check detector
                for det in dName:
                    if 'unknown' in det: continue
                    if det in line: v = 2
                    else:           v = 1
                    detgraphs[det].SetPoint( ip, runno, v )
                    dethists[det].SetBinContent( runno - minRun + 1, v )

                # check DQ flag
                for key, dq in dqchans.items():
                    if not dq in line: v = -1
                    else:           
                        word = line[line.find( dq ):line.find( dq )+len(dq)+2]
                        if '=' in word:
                            w, sep, status = word.partition('=')
                            status = status.strip().lower()
                            if   status == 'u': v = 0
                            elif status == 'g': v = 3
                            elif status == 'y': v = 2
                            elif status == 'r': v = 1
                            else:
                                print 'ERROR: unknown DQ status: "%s" --> abort' % status
                                sys.exit(1)
                            
                            dqhists[dq].SetBinContent( runno - minRun + 1, v )
                        else:
                            print 'ERROR: format error on DQ parsing: "%s" --> abort' % word
                            sys.exit(1)
                            

    print 'End of file reading. Found: %i runs' % (ip+1)

    # write all graphs, and finish
    for key, g in detgraphs.items(): g.Write()
    for key, h in dethists.items() : h.Write()
    for key, h in dqhists.items()  : h.Write()

    print 'Wrote root file: %s' % rootfile.GetName()
    rootfile.Close()


# command line driver for convenience
if __name__=='__main__':

    if len(sys.argv) <= 1:
        print 'ERROR: Need to give input filename as argument --> abort'
        sys.exit(0)

    execute( sys.argv[1] )
