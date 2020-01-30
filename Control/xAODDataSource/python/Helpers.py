# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Necessary import(s).
import ROOT

def MakexAODDataFrame( inputs ):
    """Helper function creating a ROOT::RDataFrame object reading xAOD files

    The function returns an instance of ROOT::RDataFrame that uses
    xAOD::RDataSource for reading its inputs.

    Keyword arguments:
      inputs -- A single string, or a list of string selecting the input file(s)
                Note that the string(s) may contain wildcards and environment
                variables as well.
    """

    # Make sure that the dictionary is loaded.
    ROOT.xAOD.ROOT6_xAODDataSource_WorkAround_Dummy()

    # Use the C++ function for the heavy lifting.
    return ROOT.xAOD.MakeDataFrame( inputs )
