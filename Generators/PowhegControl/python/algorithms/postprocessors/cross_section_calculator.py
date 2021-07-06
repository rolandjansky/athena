# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
from ...utility import LHE

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


@timed("cross section calculator")
def cross_section_calculator(powheg_LHE_output):
    """! Calculate the cross-section (mean event weight) of pre-generated Powheg events.

    @param powheg_LHE_output  Name of LHE file produced by PowhegBox.

    @author James Robinson <james.robinson@cern.ch>
    """
    logger.info("Born-level suppression is enabled so the cross-section MUST be recalculated!")
    # LHEUtils returns a generator, since the input file may be large
    sum_of_weights, n_events = 0., 0
    for event_weight in LHE.event_weight_iterator(powheg_LHE_output):
        sum_of_weights += event_weight
        n_events += 1
    # import numpy as np
    # import itertools as it
    # sum_of_weights, n_events = reduce(np.add, it.izip(LHEUtils.event_weight_iterator(LHE_file_name), it.repeat(1)))

    # Print statistics to logger
    logger.info("... sum of event weights is:    {}".format(sum_of_weights))
    logger.info("... number of events generated: {}".format(int(n_events)))
    logger.info("... mean event weight:          {}".format((sum_of_weights / n_events if n_events != 0 else 0)))
    logger.info("MetaData: cross-section (nb) = {:.3E}".format((sum_of_weights / (1000 * n_events) if n_events != 0 else 0)))
