#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# Set up the runtime environment.
import ROOT
ROOT.xAOD.Init().ignore()

# Create a data frame object.
from xAODDataSource.Helpers import MakexAODDataFrame
df = MakexAODDataFrame( "${ASG_TEST_FILE_DATA}" )

# Test its histogramming.
elPt = df.Define( "ElectronsPt",
                  """
                  std::vector< float > result;
                  result.reserve( Electrons.size() );
                  for( const xAOD::IParticle* p : Electrons ) {
                     result.push_back( p->pt() );
                  }
                  return result;
                  """ )
hist = elPt.Histo1D( "ElectronsPt" )
print( "ElectronsPt entries = %i" % hist.GetEntries() )
