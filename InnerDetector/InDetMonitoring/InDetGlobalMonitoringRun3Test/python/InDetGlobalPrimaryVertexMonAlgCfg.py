#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

"""
@file InDetGlobalPrimaryVertexMonAlgCfg.py
@author Leonid Serkin and Per Johansson
@date April 2020
@brief Configuration for Run 3 based on InDetGlobalPrimaryVertexMonTool.cxx
"""

from math import pi as M_PI

def InDetGlobalPrimaryVertexMonAlgCfg(helper, alg, **kwargs):

    # this creates a "pvGroup" called "alg" which will put its histograms into the subdirectory "PrimaryVertex"
    pvGroup = helper.addGroup(alg, 'PrimaryVertex')
    pathpv = '/InDetGlobal/PrimaryVertex'

    # begin histogram definitions
    varName = 'm_PvX;pvX' #done
    title = 'Primary vertex: x;x (mm);Events'
    pvGroup.defineHistogram(varName, type='TH1F', path=pathpv, title=title, xbins=500, xmin=-1.0, xmax=1.0)

    varName = 'm_PvY;pvY' #done
    title = 'Primary vertex: y;y (mm);Events'
    pvGroup.defineHistogram(varName, type='TH1F', path=pathpv, title=title, xbins=500, xmin=-1.0, xmax=1.0)

    varName = 'm_PvZ;pvZ' #done
    title = 'Primary vertex: z;z (mm);Events'
    pvGroup.defineHistogram(varName, type='TH1F', path=pathpv, title=title, xbins=100, xmin=-200., xmax=200.)

    varName = 'm_PvN;pvN' #done
    title = 'Total number of vertices (primary and pile up);Total number of vertices;Events'
    pvGroup.defineHistogram(varName, type='TH1F', path=pathpv, title=title, xbins=50, xmin=-0.5, xmax=49.5)

    varName = 'm_nPriVtx;pvNPriVtx' #done
    title = 'Number of primary vertices;Number of primary vertices;Events'
    pvGroup.defineHistogram(varName, type='TH1F', path=pathpv, title=title, xbins=3, xmin=-0.5, xmax=2.5)

    varName = 'm_nPileupVtx;pvNPileupVtx' #done
    title   = 'Number of pileup vertices;Number of pile up vertices;Events'
    pvGroup.defineHistogram(varName, type='TH1F', path=pathpv, title=title, xbins=40, xmin=-0.5, xmax=39.5)
    
    varName = 'm_PvErrX;pvErrX'  #done
    title   = 'Primary vertex: #sigma_{x}; #sigma_{x} (mm);Events'
    pvGroup.defineHistogram(varName, type='TH1F', path=pathpv, title=title, xbins=100, xmin=0., xmax=0.015)
  
    varName = 'm_PvErrY;pvErrY'  #done
    title   = 'Primary vertex: #sigma_{x}; #sigma_{y} (mm);Events'
    pvGroup.defineHistogram(varName, type='TH1F', path=pathpv, title=title, xbins=100, xmin=0., xmax=0.015)

    varName = 'm_PvErrZ;pvErrZ'   #done
    title   = 'Primary vertex: #sigma_{z}; #sigma_{y} (mm);Events'
    pvGroup.defineHistogram(varName, type='TH1F', path=pathpv, title=title, xbins=100, xmin=0., xmax=0.1)

    varName = 'm_PvChiSqDoF;pvChiSqDof'   #done
    title   = 'Primary vertex: #Chi^{2}/DoF of vertex fit;#Chi^{2}/DoF;Events'
    pvGroup.defineHistogram(varName, type='TH1F', path=pathpv, title=title, xbins=100, xmin=0., xmax=5.)

    varName = 'm_PvN;pvNTracks'  #done
    title = 'Number of tracks in primary vertex;Number of tracks;Events'
    pvGroup.defineHistogram(varName, type='TH1F', path=pathpv, title=title, xbins=300, xmin=0., xmax=300.)

    varName = 'm_PvTrackPt;pvTrackPt'  #done
    title   = 'Primary vertex: original track p_{t};p_{t} (GeV);Events'
    pvGroup.defineHistogram(varName, type='TH1F', path=pathpv, title=title, xbins=100, xmin=0., xmax=20.)

    varName = 'm_PvTrackEta;pvTrackEta'  #done
    title   = 'Primary vertex: original track #eta; #eta;Events'
    pvGroup.defineHistogram(varName, type='TH1F', path=pathpv, title=title, xbins=100, xmin=-3, xmax=3)
 


# end histograms


