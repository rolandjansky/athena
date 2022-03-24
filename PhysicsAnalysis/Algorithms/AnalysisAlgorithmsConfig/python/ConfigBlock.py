# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

class ConfigBlock:
    """the base class for classes implementing individual blocks of
    configuration

    A configuration block is a sequence of one or more algorithms that
    should always be scheduled together, e.g. the muon four momentum
    corrections could be a single block, muon selection could then be
    another block.  The blocks themselves generally have their own
    configuration options/properties specific to the block, and will
    perform a dynamic configuration based on those options as well as
    the overall job.

    The actual configuration of the algorithms in the block will
    depend on what other blocks are scheduled before and afterwards,
    most importantly some algorithms will introduce shallow copies
    that subsequent algorithms will need to run on, and some
    algorithms will add selection decorations that subquent algorithms
    should use as preselections.

    The algorithms get created in a multi-step process (that may be
    extended in the future): As a first step each block retrieves
    references to the containers it uses (essentially marking its spot
    in the processing chain) and also registering any shallow copies
    that will be made.  In the second/last step each block then
    creates the fully configured algorithms.

    One goal is that when the algorithms get created they will have
    their final configuration and there needs to be no
    meta-configuration data attached to the algorithms, essentially an
    inversion of the approach in AnaAlgSequence in which the
    algorithms got created first with associated meta-configuration
    and then get modified in susequent configuration steps.

    For now this is mostly an empty base class, but another goal of
    this approach is to make it easier to build another configuration
    layer on top of this one, and this class will likely be extended
    and get data members at that point.

    The child class needs to implement two methods,
    `collectReferences` and `makeAlgs` which are each given a single
    `ConfigAccumulator` type argument.  The first is for the first
    configuration step, and should only collect references to the
    containers to be used.  The second is for the second configuration
    step, and should create the actual algorithms.

    """

    def __init__ (self) :
        pass
