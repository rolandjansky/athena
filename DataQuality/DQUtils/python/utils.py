#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Utility module for dataquality specific things
"""

from __future__ import with_statement

from cPickle import loads as unpickle, dumps as pickle

from os.path import exists, dirname, getmtime
from os import makedirs
from time import time, mktime, strptime
from sys import stdout

BUILTIN_NAMES = set(("True", "False"))

def longest_status(iovs, start_time, end_time):
    """
    First intersect the times with the luminosity block, then compute 
    lengths. Then return the longest status.
    """
    if len(iovs) == 1: return iovs[0][2] # Short circuit if there is one iov
    iovs = intersect_with_lumiblock(iovs, start_time, end_time)
    length_statuses = map(make_length_status, iovs)
    # ^ [(-length, status), (-length2, status2), ...]
    lstat = defaultdict(int)
    # Sum together amounts of each status for this block
    for length, status in length_statuses: lstat[status] += length
    length_statuses = [(length, status) for status, length in lstat.iteritems()]
    sorted_by_length_worst = sorted(length_statuses)
    # ^ [Same as above, but the longest length and worst status is first]
    length, status = sorted_by_length_worst[0]
    return status

def worst_status(iovs, *dummy_args):
    """
    Return the worst status of multiple IoVs. (Different from worst_of).
    """
    if len(iovs) == 1: return iovs[0][2]
    statuses = zip(*iovs)[2]
    return worst(statuses)

def worst(iterable):
    states = set(iterable)
    
    if not states: return None
    if  -1 in states: return -1
    elif 1 in states: return 1
    elif 2 in states: return 2
    elif 3 in states: return 3
    elif 0 in states: return 0
    elif None in states: return None
    
    raise RuntimeError("Invalid Code present on database?")

WHITE = None
GREY = 0
    
def first(iterable):
    """
    Return the first filled (and not grey) flag
    """
    result = WHITE
    for element in iterable:
        if element not in [WHITE, GREY]:
            return element
        elif element == GREY:
            result = GREY            
    return result
    
def best(iterable, priorities=[3, 2, 1, -1, 0]):
    """
    a min() function whose priorities can be chosen
    """
    return worst(iterable, [3, 2, 1, -1, 0])

def best_status(iovs, *dummy_args):
    """
    Return the worst status. (Different from worst_of).
    """
    if len(iovs) == 1: return iovs[0][2]
    statuses = zip(*iovs)[2]
    return best(statuses)

def iovs_in_time(wanted_start_time, wanted_end_time):
    """
    Return a function which selects iovs dependent on they occured 
    in the desired time
    """
    
    # TODO: Test that this logic gives the same result as 
    # wanted_start < iov_end and wanted_end > iov_start
    
    def test_one(iov):
        start_time, end_time, status = iov
        # Start or end of IoV lies within desired region
        
        return (    # Desired start or end time lies within block
                       start_time <= wanted_start_time <=        end_time or
                       start_time <=   wanted_end_time <=        end_time or
                    # Block lies within desired start or end time
                wanted_start_time <=        start_time <= wanted_end_time or
                wanted_start_time <=          end_time <= wanted_end_time)
             
    return test_one
    
def get_root_version():
    """
    Return ROOT version tuple
    """
    from ROOT import gROOT
    version_code = gROOT.RootVersionCode()
    return (version_code >> 16, version_code >> 8 & 0xFF, version_code & 0xFF)
    
def fix_iovkey(iovkey):
    """
    If an IoV starts on the 0th lumiblock, then move it forward one.
    """    
    return iovkey if iovkey & 0xFFFFFFFF else iovkey+1

def make_functor(expression, location, global_env={}, input_translation=None):
    """
    Compile an expression, returning the variables used and a functor which can
    be called with the namespace in which the expression is run
    
    `expression` is a single python expression to be evaluated
    `location` is a string describing where the snippet of code came from
               (in case of exceptions, for debugging)
    `global_env` is the global environment in which the expression is executed
    `input_translation` is an optional function which is executed on the 
                        functor's arguments before it is executed.
    """
    compiled = compile(expression, location, "eval")

    provided_vars = set(global_env.keys())
    variables = sorted(set(compiled.co_names) - BUILTIN_NAMES - provided_vars)
    if input_translation:
        def functor(locals_={}):
            return eval(compiled, global_env, input_translation(locals_))
    else:
        def functor(locals_={}):
            return eval(compiled, global_env, locals_)
    return variables, functor
    
def make_floats_pretty(x):
    if isinstance(x, float):
        return "%.3f" % x
    return x
    
def pprint_objects(objects, where=stdout):
    """
    Pretty print a list of IoV-results
    """
    
    for obj in objects:
        since, until = obj[:2]
        result = obj[2:]
        args = since, until, "(%s)" % ", ".join(map(str, map(make_floats_pretty, obj[2:])))
        print >>where, "[%r -> %r) : %s" % args
        
def describe_colour(colour):
    mapping = {
        None: "White",
        -1: "Black",
        0: "Grey",
        1: "Red",
        2: "Yellow",
        3: "Green",
    }
    if colour not in mapping:
        raise RuntimeError, "Unknown colour code: %s" % colour
    return mapping[colour]
