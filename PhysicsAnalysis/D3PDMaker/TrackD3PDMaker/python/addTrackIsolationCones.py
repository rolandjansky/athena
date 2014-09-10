# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: addTrackIsolationCones.py 544562 2013-04-13 04:42:52Z ssnyder $
#
# @file TrackD3PDMaker/python/addTrackIsolationCones.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2013
# @brief Add isolation cone variables to a TrackParticle.
#


from D3PDMakerCoreComps.SimpleAssociation   import IdentityAssociation
from TrackD3PDMaker.trackIsolationTool      import trackIsolationTool
import TrackD3PDMaker


def addTrackIsolationCones (tp,
                            blockname = None,
                            level = 0,
                            sizes = [0.2, 0.3, 0.4],
                            subblocklevel = 0,
                            isolationTool = None,
                            caloIsoVar = '',
                            trackIsoVar = 'ptcone',
                            nTrackIsoVar = 'nucone',
                            suffix = ''):
    """Add isolation cone variables to a TrackParticle.

  TP is the parent D3PDObject, which should represent a TrackParticle.
  BLOCKNAME is the name for the new block; a default will be chosen if None.
  LEVEL is the level-of-detail for the block.
  SIZES is a list of cone sizes to add.
  SUBBLOCKLEVEL is the level-of-detail of the individual sub-blocks
    for each cone size.  Usually should be appropriate to leave this as 0.
  ISOLATIONTOOL is the isolation tool instance to use.
  CALOISOVAR, TRACKISOVAR, and NTRACKISOVAR are variable names
    to use for the isolation variables.  If None, that variable is not written.
  SUFFIX is a suffix to add to variable names.
"""

    if isolationTool == None:
        isolationTool = trackIsolationTool ('ConeTrackIsolationTool')

    if blockname == None:
        blockname = 'IsolationCones' + suffix

    tp.defineBlock (level, blockname,
                    TrackD3PDMaker.TrackIsolationFillerTool,
                    CaloIsoVar = caloIsoVar,
                    TrackIsoVar = trackIsoVar,
                    NTrackIsoVar = nTrackIsoVar,
                    Suffix = suffix,
                    ConeSizes = sizes,
                    TrackIsolationTool = isolationTool)
                                        

