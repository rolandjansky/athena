# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file TrackD3PDMaker/python/TrackPIDFillerTool.py
# @author scott snyder <snyder@bnl.gov>
# @date Jun, 2010
# @brief Helper to form the job properties for TrackPIDFillerTool.
#


from TrackD3PDMaker import TrackD3PDMakerConf



class TrackPIDFillerTool (TrackD3PDMakerConf.D3PD__TrackPIDFillerTool):
    """Helper to form the job properties for TrackPIDFillerTool.

The TrackPIDFillerTool.PID property is a list of strings, encoding
the PID type and the variable name.  They have to be all strings,
since Gaudi supports only vectors of uniform type; PID is a string
representation of the PID code.  To make it easier to set this property,
this little helper converts every element of PID to a string.
"""
    def __init__ (self,
                  name,
                  PID = [],
                  **kwargs):
        PID = [str(x) for x in PID]
        TrackD3PDMakerConf.D3PD__TrackPIDFillerTool.__init__ (self,
                                                              name,
                                                              PID = PID,
                                                              **kwargs)
        
