#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

"""
@file InDetGlobalBeamSpotMonAlgCfg.py
@author Leonid Serkin and Per Johansson
@date April 2020
@brief Configuration for Run 3 based on InDetGlobalBeamSpotMonTool.cxx
"""

def InDetGlobalBeamSpotMonAlgCfg(helper, alg, **kwargs):
    '''Function to configures some algorithms in the monitoring system.'''
    
    # Values set by default here
    m_useBeamspot = True
    expert = True
    m_vxContainerWithBeamConstraint = False
    
    # this creates a "trackGroup" called "alg" which will put its histograms into the subdirectory "BeamSpot"
    bsGroup = helper.addGroup(alg, 'BeamSpot')
    pathbs = '/InDetGlobal/BeamSpot'
    
    # Histograms for track-based beam spot monitoring:
    varName = 'm_trkD0Phi,m_trkD0;trkDPhi'
    bsGroup.defineHistogram(varName,type="TH2F",title="DCA vs Phi; #varphi (rad);d_{0} (#mum)", path=pathbs, xbins=100, xmin=-3.5, xmax=3.5, ybins=100, ymin=-1000, ymax=1000)

    if (expert):
        varName = 'm_trkPt;trkPt'
        bsGroup.defineHistogram(varName, type="TH1F", title="Track Pt;p_{T} (GeV)",path=pathbs,xbins=100,xmin=0,xmax=20)
        varName = 'm_trkNPt;trkNPt'
        bsGroup.defineHistogram(varName, type="TH1F", title="Number of Tracks per event (after p_{T} cut);Number of tracks", path=pathbs, xbins=100, xmin=0, xmax=1000)
    
    #Histograms of assumed beam spot position
    if (m_useBeamspot):
        varName = 'm_trkD0PhiCorr,m_trkD0Corr;trkDPhiCorr'
        bsGroup.defineHistogram(varName,type="TH2F",title="DCA vs Phi wrt Beamspot;#varphi (rad);d_{0} (#mum)",path=pathbs,xbins=100, xmin=-3.5,xmax=3.5, ybins=100, ymin=-500,ymax=500)
        varName = 'm_bsX;bsX'
        bsGroup.defineHistogram(varName, type="TH1F", title="Beam spot position: x;x (mm)",path=pathbs,xbins=100,xmin=-10,xmax=10)
        varName = 'm_bsY;bsY'
        bsGroup.defineHistogram(varName, type="TH1F", title="Beam spot position: y;y (mm)",path=pathbs,xbins=100,xmin=-10,xmax=10)
        varName = 'm_bsZ;bsZ'
        bsGroup.defineHistogram(varName, type="TH1F", title="Beam spot position: z;z (mm)",path=pathbs,xbins=100,xmin=-500,xmax=500)
        varName = 'm_bsTiltX;bsTiltX'
        bsGroup.defineHistogram(varName, type="TH1F", title="Beam spot tile angle: x-z plane; Tilt angle (#murad)",path=pathbs,xbins=100,xmin=-1e3,xmax=1e3)
        varName = 'm_bsTiltY;bsTiltY'
        bsGroup.defineHistogram(varName, type="TH1F", title="Beam spot tile angle: y-z plane; Tilt angle (#murad)",path=pathbs,xbins=100,xmin=-1e3,xmax=1e3)    

    # Histograms for vertex-based beam spot monitoring
    if (not m_vxContainerWithBeamConstraint):
    # The following histograms are made either relative to the current beamspot (from BeamCondSvc), or relative to the nomial beamspot at (0,0,0) without any tilt.
        if (m_useBeamspot):
            varName = 'm_pvXbeam;pvX'
            bsGroup.defineHistogram(varName, type="TH1F", title="Primary vertex: x - x_{beam};x-x_{beam} (#mum)",path=pathbs,xbins=100,xmin=-500,xmax=500)
            varName = 'm_pvYbeam;pvY'
            bsGroup.defineHistogram(varName, type="TH1F", title="Primary vertex: y - y_{beam};y-y_{beam} (#mum)",path=pathbs,xbins=100,xmin=-500,xmax=500)
            varName = 'm_pvZbeam;pvZ'
            bsGroup.defineHistogram(varName, type="TH1F", title="Primary vertex: z - z_{beam};z-z_{beam} (#mum)",path=pathbs,xbins=100,xmin=-500,xmax=500)
        else:
            varName = 'm_pvXbeam;pvX'
            bsGroup.defineHistogram(varName, type="TH1F", title="Primary vertex: x;x (mm)",path=pathbs,xbins=100,xmin=-10,xmax=10)
            varName = 'm_pvXbeam;pvX'
            bsGroup.defineHistogram(varName, type="TH1F", title="Primary vertex: y;y (mm)",path=pathbs,xbins=100,xmin=-10,xmax=10)
            varName = 'm_pvXbeam;pvX'
            bsGroup.defineHistogram(varName, type="TH1F", title="Primary vertex: z;z (mm)",path=pathbs,xbins=100,xmin=-500,xmax=500)
    
    
    #Histograms that are independent of the useBeamSpot parameters
    varName = 'm_pvZ,m_pvX;pvXZ'
    bsGroup.defineHistogram(varName, type="TH2F", title="Primary vertex: x vs z;z (mm);x (mm)", path=pathbs, xbins=100, xmin=-500, xmax=500, ybins=100, ymin=-10, ymax=10)
    varName = 'm_pvZ,m_pvY;pvYZ'
    bsGroup.defineHistogram(varName, type="TH2F", title="Primary vertex: y vs z;z (mm);y (mm)", path=pathbs, xbins=100, xmin=-500, xmax=500, ybins=100, ymin=-10,ymax=10)
    varName = 'm_pvX,m_pvY;pvYX'
    bsGroup.defineHistogram(varName, type="TH2F", title="Primary vertex: y vs x;x (mm);y (mm)", path=pathbs, xbins=100, xmin=-10, xmax=10, ybins=100, ymin=-10, ymax=10)
    varName = 'm_pvChiSqDof;pvChiSqDof'
    bsGroup.defineHistogram(varName, type="TH1F", title="",path=pathbs,xbins=100,xmin=0,xmax=20)

    if (expert):
            varName = 'm_pvN;pvN'
            bsGroup.defineHistogram(varName, type="TH1F", title="Number of primary vertices;Number of vertices", path=pathbs, xbins=100,xmin=0,xmax=100)
            varName = 'm_pvNPriVtx;pvNPriVtx'
            bsGroup.defineHistogram(varName, type="TH1F", title="Number of primary vertices;Number of vertices", path=pathbs, xbins=3,xmin=0,xmax=3)
            varName = 'm_pvNPileupVtx;pvNPileupVtx'
            bsGroup.defineHistogram(varName, type="TH1F", title="Number of pileup vertices;Number of vertices", path=pathbs, xbins=100,xmin=0,xmax=100)
            varName = 'm_pvErrX;pvErrX'
            bsGroup.defineHistogram(varName, type="TH1F", title="Primary vertex: #sigma_{x}; #sigma_{x} (mm)",path=pathbs,xbins=100,xmin=0,xmax=0.5)
            varName = 'm_pvErrY;pvErrY'
            bsGroup.defineHistogram(varName, type="TH1F", title="Primary vertex: #sigma_{y}; #sigma_{y} (mm)",path=pathbs,xbins=100,xmin=0,xmax=0.5)
            varName = 'm_pvErrZ;pvErrZ'
            bsGroup.defineHistogram(varName, type="TH1F", title="Primary vertex: #sigma_{z}; #sigma_{z} (mm)",path=pathbs,xbins=100,xmin=0,xmax=0.5)
            varName = 'm_pvNTracks;pvNTracks'
            bsGroup.defineHistogram(varName, type="TH1F", title="Number of tracks in primary vertex;Number of tracks",path=pathbs,xbins=100,xmin=0,xmax=500)
            varName = 'm_pvTrackPt;pvTrackPt'
            bsGroup.defineHistogram(varName, type="TH1F", title="Primary vertex: original track p_{t};p_{t} (GeV)",path=pathbs,xbins=100,xmin=0,xmax=20)
            varName = 'm_pvTrackEta;pvTrackEta'
            bsGroup.defineHistogram(varName, type="TH1F", title="Primary vertex: original track #eta; #eta",path=pathbs,xbins=100,xmin=-3,xmax=3)

# end histograms
