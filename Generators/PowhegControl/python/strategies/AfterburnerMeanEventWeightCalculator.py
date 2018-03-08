# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl AfterburnerMeanEventWeightCalculator
#  Calculate the mean event weight of pre-generated Powheg events
#  Apply PHOTOS afterburner to pre-generated Powheg events
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from AthenaCommon.Logging import logging
import itertools as it
import numpy as np
from ..utility import LHEUtils

# Initialise logging handler
logger = logging.getLogger("PowhegControl")

# Run PHOTOS as an afterburner to existing events
def afterburner_mean_event_weight_calculator(LHE_file_name):
    logger.info("Born-level suppression is enabled so the cross-section MUST be recalculated!")
    # LHEUtils returns a generator, since the input file may be large
    sum_of_weights, n_events = reduce(np.add, it.izip(LHEUtils.event_weight_iterator(LHE_file_name), it.repeat(1)))
    # Print statistics to logger
    logger.info("... sum of event weights is:    {}".format(sum_of_weights))
    logger.info("... number of events generated: {}".format(int(n_events)))
    logger.info("... mean event weight:          {}".format((sum_of_weights / n_events if n_events != 0 else 0)))
    logger.info("MetaData: cross-section (nb) = {:.3E}".format((sum_of_weights / (1000 * n_events) if n_events != 0 else 0)))
