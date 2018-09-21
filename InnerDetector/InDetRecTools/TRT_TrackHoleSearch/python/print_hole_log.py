#!/usr/bin/env python
__author__ = 'Ryan D. Reece  <ryan.reece@cern.ch>'
__date__ = '2009-04-20'
__doc__ = """
NAME
    print_hole_log.py - Creates a log file of TRT holes.

SYNOPSIS
    print_hole_log.py   (no arguments)

DESCRIPTION
    Creates a log file of TRT holes from reading Ryan's
    trt_eff.root ntuple for TRT straw efficiency studies.

AUTHOR
    %(author)s

%(date)s
""" % {'author':__author__, 'date':__date__, 'copyright':__copyright__, 'license':__license__}
#------------------------------------------------------------------------------

import sys
import ROOT, ROOT_vector_vector

#______________________________________________________________________________
def main(argv):
    f = ROOT.TFile('trt_eff.root', 'READ')
    t = f.Get('trt_eff')
    
    o = file('trt_holes.log', 'w')
        
    event_number = 0
    
    # loop over tracks
    nentries = t.GetEntries()
    for i in xrange(nentries):
        t.GetEntry(i)
        if event_number != t.event_number:
            # new event
            event_number = t.event_number
            o.write('======================================================================================\n')
            o.write(' Run: %10s, LB: %10s, Event: %10s\n' % (t.run_number, t.lumi_block, t.event_number) )
            o.write('======================================================================================\n')
        o.write('--------------------------------------------------------------------------------------\n')
        o.write('  Track pT: %4g, eta: %3f, phi: %3f, d0: %4g, z0: %4g\n' % (t.track_pt, t.track_eta, t.track_phi, t.track_d0, t.track_z0) )
        o.write('    # pix hits:  %3i,  # SCT hits:  %3i,  # TRT hits:  %3i\n'  % (t.n_pixel_hits, t.n_sct_hits, t.n_trt_hits) )
        o.write('    # pix holes: %3i,  # SCT holes: %3i,  # TRT holes: %3i\n'  % (t.n_pixel_holes, t.n_sct_holes, t.n_trt_holes) )
        o.write('------ TRT holes -------------------------------\n')
        o.write('  #  bec  phi  layer  strawlayer  straw      locR   locR_error         x         y         z\n')
        # loop over holes
        n_holes = len(t.hole_det)
        for i in xrange(n_holes):
            if t.hole_det[i] == 3: # TRT
                o.write('%(n)3i  %(bec)3i  %(phi)3i    %(layer)3i         %(strawlayer)3i    %(straw)3i  %(locR)8.4g  %(locR_error)11.2g  %(x)8.4g  %(y)8.4g  %(z)8.4g\n' % {
                        'n' :           i+1,
                        'bec' :         t.hole_bec[i],
                        'phi' :         t.hole_phi[i],
                        'layer' :       t.hole_layer[i],
                        'strawlayer' :  t.hole_strawlayer[i],
                        'straw' :       t.hole_straw[i],
                        'locR' :        t.hole_locR[i],
                        'locR_error' :  t.hole_locR_error[i],
                        'x' :           t.hole_x[i],
                        'y' :           t.hole_y[i],
                        'z' :           t.hole_z[i],
                        } )
                
        

#______________________________________________________________________________
if __name__ == '__main__':
    main(sys.argv[1:])

