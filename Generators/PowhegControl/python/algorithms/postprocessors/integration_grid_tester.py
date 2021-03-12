# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
import glob
import math
import re

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")

## Regex to extract floats :: from perldoc perlretut
regex_match_floats = r"[+-]? *(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?"
# [+-]? *           # first, match an optional sign (and space)
# (?:               # then match integers or f.p. mantissas:
#   \d+             # start out with a ...
#   (?:
#     \.\d*         # mantissa of the form a.b or a.
#   )?              # ? takes care of integers of the form a
#  |\.\d+           # mantissa of the form .b
# )
# (?:[eE][+-]?\d+)? # finally, optionally match an exponent


@timed("integration grid testing")
def integration_grid_tester():
    """! Calculate and output integration grid quality.

    Read in integration quality metrics from output files.
    Allow any check to fail, since this output is not a critical part of generation.

    @author James Robinson <james.robinson@cern.ch>
    """
    inclusive_xs, inclusive_xs_error, negative_weights, positive_weights, n_events, n_upper_bound_failures_xs, n_upper_bound_failures_radiation = 0, 0, 0, 0, 0, 0, 0
    # Open stat files
    for file_name in sum([glob.glob(_f) for _f in ["pwgstat.dat", "pwgstat-0001.dat", "pwg-stat.dat", "pwg-st3-0001-stat.dat", "pwg-0001-st3-stat.dat"]], []):
        # Inclusive cross-section uncertainty [relative error on (total pos + |total neg|)]
        with open(file_name, "r") as data_file:
            try:
                matched_lines = [line.replace("+-", "") for line in data_file if re.match(r"(.*)(btilde(.*)weights)(.*)[0-9](.*)\+\-(.*)[0-9](.*)", line)]
                matched_lines = [re.sub(" +", " ", _l) for _l in matched_lines] # strip multiple whitespace
                if len(matched_lines) > 0:
                    positive_weight_xs = sum([map(float, re.findall(regex_match_floats, line)) for line in matched_lines if "pos." in line], [])
                    negative_weight_xs = sum([map(float, re.findall(regex_match_floats, line)) for line in matched_lines if "|neg.|" in line], [])
                    inclusive_xs += positive_weight_xs[0] + negative_weight_xs[0]
                    inclusive_xs_error += math.sqrt(positive_weight_xs[1]**2 + negative_weight_xs[1]**2)
            except Exception:  # catch all exceptions
                pass
        # Negative weight test
        with open(file_name, "r") as data_file:
            try:
                matched_lines = [line.replace("+-", "") for line in data_file if re.match(r"(.*)(btilde(.*)weights|Remnant cross section)(.*)[0-9](.*)\+\-(.*)[0-9](.*)", line)]
                matched_lines = [re.sub(" +", " ", _l) for _l in matched_lines] # strip multiple whitespace
                if len(matched_lines) > 0:
                    negative_weights += map(float, re.findall(regex_match_floats, [line for line in matched_lines if "btilde |neg.|" in line][0]))[0]
                    positive_weights += map(float, re.findall(regex_match_floats, [line for line in matched_lines if "btilde pos." in line][0]))[0]
                    positive_weights += map(float, re.findall(regex_match_floats, [line for line in matched_lines if "Remnant cross section" in line][0]))[0]
            except Exception:  # catch all exceptions
                pass
    # Open counter files
    file_names = sum([glob.glob(_f) for _f in ["pwgcounters-st4-*.dat", "pwgcounters0*.dat"]], [])
    if not file_names: file_names = glob.glob("pwgcounters.dat")
    for file_name in file_names:
        # Upper bound violations [in inclusive cross-section and generation of radiation]
        with open(file_name, "r") as data_file:
            try:
                matched_lines = [line.replace("+-", "") for line in data_file if re.match(r"(.*)(btilde event|remnant event|upper bound failure)(.*)[0-9](.*)", line)]
                matched_lines = [re.sub(" +", " ", _l) for _l in matched_lines] # strip multiple whitespace
                if len(matched_lines) > 0:
                    n_events += sum(map(float, [re.findall(regex_match_floats, line)[0] for line in matched_lines if "event" in line]))
                    n_upper_bound_failures_xs += sum(map(float, [re.findall(regex_match_floats, line)[0] for line in matched_lines if "upper bound failure in inclusive" in line]))
                    n_upper_bound_failures_radiation += sum(map(float, [re.findall(regex_match_floats, line)[0] for line in matched_lines if "upper bound failure in generation" in line]))
            except Exception:  # catch all exceptions
                pass

    # Calculate test statistics
    inclusive_xs_test = safe_percentage(inclusive_xs_error, inclusive_xs)
    negative_weight_test = safe_percentage(negative_weights, (negative_weights + positive_weights))
    upper_bound_test = safe_percentage((n_upper_bound_failures_xs + n_upper_bound_failures_radiation), n_events)
    upper_bound_test_xs = safe_percentage(n_upper_bound_failures_xs, n_events)
    upper_bound_test_radiation = safe_percentage(n_upper_bound_failures_radiation, n_events)

    # Write output
    try:
        getattr(logger, ["warning", "info"][0.0 <= inclusive_xs_test < 1.0])("Integration test :: {:>30} : {:.2f}%".format("cross-section uncertainty", inclusive_xs_test))
        getattr(logger, ["warning", "info"][0.0 <= negative_weight_test < 1.0])("Integration test :: {:>30} : {:.2f}%".format("negative weight fraction", negative_weight_test))
        getattr(logger, ["warning", "info"][0.0 <= upper_bound_test < 1.0])("Integration test :: {:>30} : {:.2f}%".format("upper bound violations", upper_bound_test))
        logger.info("                 -> {:>30} : {:.2f}%".format("in inclusive cross-section", upper_bound_test_xs))
        logger.info("                 -> {:>30} : {:.2f}%".format("in generation of radiation", upper_bound_test_radiation))
        if any((inclusive_xs_test < 0.0, inclusive_xs_test >= 1.0, negative_weight_test < 0.0, negative_weight_test >= 1.0, upper_bound_test < 0.0, upper_bound_test >= 1.0)):
            logger.warning("Not all integration tests passed. Please ensure that physics validation is done before using this sample!")
    except Exception:  # catch all exceptions
        pass

def safe_percentage(numerator, denominator):
    try:
        return 100 * numerator / denominator
    except Exception:  # catch all exceptions
        return float("nan")
