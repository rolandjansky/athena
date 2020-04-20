# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
# replaces AthenaCommon/AlgSequence.py for GaudiConfig2-based configs

from AthenaConfiguration.ComponentFactory import CompFactory


class AthSequencer(CompFactory.AthSequencer):
    """Sequence of Gaudi algorithms"""

    def __init__( self, name = "AthSequencer", **kwargs ):
        # call base class __init__ to pass new name
        super( AthSequencer, self ).__init__( name, **kwargs )
 
    def __iadd__(self,algo):
       super( AthSequencer, self ).Members.append(algo.__cpp_type__+"/"+algo.name)

    def __len__(self):
        return len(super( AthSequencer, self ).Members)
