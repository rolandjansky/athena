#! /usr/bin/env python
# Author: Peter Waller <peter.waller@cern.ch>
# Part of the DataQuality/DQUtils package.
# May 2011
"""
    Usage: dqu_lumi.py [--showiovs] [project tag] [period <period spec>] 
                       [LTAG:<lumi tag>] [GRL:<grl path.xml>].. 
                       [DQD:<defect expression>]..
    
    All arguments are optional, though one or more 
    GRLs and/or periods must be specified.
    
    Some examples:
    
      Show luminosity lost in period to all SCT or PIXEL defects:
        dqu_lumi.py period A DQD:SCT_* or PIXEL_*
    
      Show luminosity accumulated when there were no SCT/PIX defects:
        dqu_lumi.py period A DQD:'not (SCT_* or PIXEL_*)'
     
      Show luminosity in GRL:
        dqu_lumi.py GRL:my_grl.xml
    
      Show luminosity in GRL, totals by periods
        dqu_lumi.py period A-Z GRL:my_grl.xml
    
    Notes:
        * Argument order matters, except for GRL and DQD expressions (although 
          they must be the last parameters).
        * Only primary defects are supported at the moment. Please request 
          virtual defects if you want/need them.
        * Periods and GRLs specified are intersected
        * Wildcarded defect specification means "take the union of all matching"
        * Optional --showiovs will show the ranges for which the defect 
          expression is true.
"""

from sys import stdout
from os import environ
DEBUG = "DEBUG" in environ
if DEBUG:
    print "Python startup finished.",
    stdout.flush()

import fnmatch
import os
import re
import sys

from argparse import ArgumentParser
from os.path import expanduser, expandvars
from pprint import pprint
from textwrap import dedent

from pyparsing import ParseException

from DQDefects import DefectsDB

from DQUtils import fetch_iovs, process_iovs
from DQUtils.grl import load_grl
from DQUtils.lumi import fetch_lumi_inputs, compute_lumi
from DQUtils.periods import fetch_project_period_runs
from DQUtils.sugar import IOVSet, RANGEIOV_VAL, RunLumi, IOVMIN, IOVMAX

if DEBUG:
    from IPython.Shell import IPShellEmbed; ip = IPShellEmbed(["-pdb"])
    print "Imports finished."

def to_int(s, l, t):
    return int(t[0])
def to_upper(s, l, t):
    return t[0].upper()

class ParsedObject(object):
    """
    The base parsed object represents only the tokens captured.
    Subclasses give these tokens functionality, such as representing iovsets.
    """
    def __init__(self, tokens):
        self.tokens = tokens
        
    def __repr__(self):
        return "<%s %s>" % (self.__class__.__name__, self.tokens)
        
    @classmethod
    def make(cls, string, location, tokens):
        """
        Build the subclass `cls` out of a token stream `tokens`. To be used as
        a pyparsing parse action with setParseAction.
        """
        result = cls(tokens.asList())
        result.expression = string
        return result
        
class SinglePeriod(ParsedObject):
    """
    Takes a single period object
    """
    def __init__(self, tokens):
        self.letter, self.number = self.tokens = tokens
        self.period = self.letter
        if self.number:
            self.period += "%i" % self.number

class Periods(ParsedObject):
    ALL = "ALL"
    
    @classmethod
    def load_period_info(cls):
        if hasattr(cls, "period_info"):
            return cls.period_info
        cls.period_info = fetch_project_period_runs()
    
    @classmethod
    def expand(cls, string, location, tokens):
        """
        Expands two SinglePeriods into the list of periods between them.
        
        Not implemented: spanning periods of different letters.
        """
        left, right = tokens
        if left.number or right.number:
            assert left.letter == right.letter
            assert left.number and right.number
            assert left.number < right.number
            span = xrange(left.number, right.number+1)
            parts = [(left.letter, i) for i in span]
        else:
            span = xrange(ord(left.letter), ord(right.letter)+1)
            parts = [(chr(letter), None) for letter in span]
        
        return [SinglePeriod(period) for period in parts]
    
    @property
    def specified(self):
        return self.tokens != ["ALL"]
    
    def load(self, project_tag):
        """
        Populate self.period_iovs
        """
        if not self.specified:
            self.range_iov = RANGEIOV_VAL(IOVMIN, IOVMAX)
            return ["ALL", IOVSet([self.range_iov])]
            
        self.period_iovs, bad_periods, self.all_iovs = [], [], IOVSet()
        
        self.load_period_info()
        
        if project_tag not in self.period_info:
            raise RuntimeError("Unknown period '{0}'. Valid periods: {1}"
                .format(project_tag, ", ".join(sorted(self.period_info))))
        
        ptag_periods = self.period_info[project_tag]
        
        for token in self.tokens:
            if token.period not in ptag_periods:
                bad_periods.append(token.period)
                continue
            
            iovs = IOVSet.from_runs(ptag_periods[token.period])
            self.period_iovs.append((token.period, iovs))
            self.all_iovs |= iovs
            
        if not self.all_iovs:
            # Period selection s empty!
            self.range_iov = RANGEIOV_VAL(RunLumi(0), RunLumi(0))
        else:    
            self.range_iov = self.all_iovs.range_iov
            
        if bad_periods:
            bad_periods = ",".join(bad_periods)
            print "Ignoring the following non-existent periods:", bad_periods
        
class GRL(ParsedObject):
    def __init__(self, tokens):
        (self.name,) = self.tokens = tokens
        self.name = expandvars(expanduser(self.name))

    def load(self):
        return load_grl(self.name)

class SingleDefect(ParsedObject):
    """
    Represents a single defect, or a defect pattern.
    """
    def __init__(self, tokens):
        self.negated = False
        self.tokens = tokens
        self.defect_pattern = tokens[0]
        self.defects = None
    
    def __str__(self):
        """
        Pretty-print the defect expression
        """
        defect_text = self.defect_pattern
        if len(self.defects) > 1:
            defect_text += " [%i defects]" % len(self.defects)
        if self.negated:
            return "(not %s)" % defect_text
        return defect_text
     
    @classmethod
    def negate(self, string, location, tokens):
        """
        Called when the parser encounters a "not" in the expression
        """
        obj = tokens[0][0]
        obj.negated = not obj.negated
        return obj
        
    def query_channels(self, all_defects, typ):
        """
        Returns the set of defects matching self.defect_pattern.
        Raises an error if we reference a defect which doesn't exist.
        """
        if "*" in self.defect_pattern:
            self.defects = fnmatch.filter(all_defects, self.defect_pattern)
            if not self.defects:
                raise RuntimeError("Defect expression doesn't match any "
                                   "defects: %s" % self.defect_pattern)
        else:
            if self.defect_pattern not in all_defects:
                raise RuntimeError("Unknown defect %s" % self.defect_pattern)
            self.defects = [self.defect_pattern]
        return set(self.defects)
        
    def compute(self, iovs_by_defect):
        """
        Compute the logical or of all defects specified by this defect pattern
        """
        iovsets = [iovs_by_defect[d] for d in self.defects]
        result = reduce(IOVSet.logical_or, iovsets)
        if self.negated:
            result = result.logical_not()
        return result

class DefectExpression(SingleDefect):
    """
    Represents an arbitrary defect expression
    """
    def __init__(self, tokens):
        self.negated = False
        if tokens[0] in ("DQD:", "DQDI:"):
            self.typ, tokens = tokens[0], tokens[1:]
        self.tokens = tokens
        self.logic, self.combiner = "or", IOVSet.logical_or
    
    def __str__(self):
        """
        String form of the expression with explicit braces
        """
        result = map(str, self.tokens)
        result = (" %s " % self.logic).join(result)
        if len(self.tokens) == 1:
            return result
        return "not (%s)" % result if self.negated else "(%s)" % result
     
    def query_channels(self, all_defects, typ=None):
        """
        Returns a set of all channels queried by this expression
        """
        #if typ == None:
            #typ = self.type
        query_channels = set()
        for token in self.tokens:
            query_channels.update(token.query_channels(all_defects, typ))
        return query_channels        
    
    def compute(self, iovs_by_defect):
        """
        Given a list of iovs_by_defect, compute a result for this expression
        """
        result_iovsets = [sub.compute(iovs_by_defect) for sub in self.tokens]
        result = reduce(self.combiner, result_iovsets)
        if self.negated:
            result = result.logical_not()
        return result

    @classmethod
    def combine_and(cls, string, location, tokens):
        """
        Make a defect expression whose tokens are combined with "and" logic.
        """
        obj = cls(tokens.asList()[0])
        obj.logic, obj.combiner = "and", IOVSet.logical_and
        return obj
        
    @classmethod
    def combine_or(cls, string, location, tokens):
        obj = cls(tokens.asList()[0])
        obj.logic, obj.combiner = "or", IOVSet.logical_or
        return obj

def get_grammar():
    """
    Returns a grammar to parse the command line query
    """
    from pyparsing import (alphas, nums, Literal, Regex, commaSeparatedList, 
        ZeroOrMore, OneOrMore, operatorPrecedence, opAssoc, alphanums, Optional, 
        Word, Combine, StringStart, StringEnd, Group, QuotedString,
        printables, dblQuotedString)
    
    # The project tag to use when looking up a period
    project_tag = Combine(Regex("data") + Word(alphanums + "_", min=1))
    project_tag = Optional(project_tag, default="data11_7TeV")("project_tag")
    
    # A single period is of the form [a-zA-Z][0-9]{,2}
    single_period = (
        Word(alphas, max=1).setParseAction(to_upper) + 
        Optional(Word(nums, max=2).setParseAction(to_int), default=None)
    ).setParseAction(SinglePeriod.make)
    
    # Multi-period is a two single periods separated by a "-"
    multi_period = (
        single_period + Literal("-").suppress() + single_period
    ).setParseAction(Periods.expand)
    
    # A period is either one or more single periods, or period ranges.
    periods = (Optional(
        Literal("period").suppress() + OneOrMore(multi_period | single_period),
        default=Periods.ALL
    )).setParseAction(Periods.make).setName("periods").setResultsName("periods")
        
    # A defect is an uppercase alphanum with _'s and *'s.
    single_defect = (
        Word(alphanums.upper() + "_*", min=1)
    ).setParseAction(SingleDefect.make)
    
    # Logical defect operations (complete with correct precedence and bracketing
    # with thanks to operatorPrecedence.
    not_op = Literal("not").suppress()
    and_op = Literal("and").suppress()
    or_op  = Literal("or").suppress()
    logical_op = not_op | and_op | or_op
    
    # Bind nots first, then ands, then ors.
    # Don't bind logical ops as defect names
    defect_string = operatorPrecedence((~logical_op) + single_defect,
        [(not_op, 1, opAssoc.RIGHT, SingleDefect.negate),
         (and_op, 2, opAssoc.LEFT,  DefectExpression.combine_and),
         (or_op,  2, opAssoc.LEFT,  DefectExpression.combine_or),]
    ).setName("defect_expression")
    
    def parse_quoted_defects(string, location, tokens):
        """
        If a defect expression is wrapped in quotes, parse it.
        """
        return defect_string.parseString(tokens[0])
    
    # A defect expression is DQD: or DQDI: followed by 
    defect_expression = (Literal("DQD:") | Literal("DQDI:")) + (
        QuotedString('"').setParseAction(parse_quoted_defects) | defect_string
    ).setParseAction(DefectExpression.make).setResultsName("defect_expressions", True)
    
    # GRLS
    path_chars = "".join([c for c in printables if c not in "?"])
    path = Word(path_chars).setName("path")
    grl = (
        Literal("GRL:").suppress() + (QuotedString('"') | path)
    ).setParseAction(GRL.make).setResultsName("grls", True)
    
    # Many defect and GRL expressions can be specified
    operands = ZeroOrMore(defect_expression | grl)
    
    lumi_tag = Optional(
        Combine(Literal("LTAG:").suppress() + Word(alphanums + "-")), 
        default="OflLumi-7TeV-001"
    )("lumi_tag")
    
    showiovs = Optional(
        Literal("--showiovs"),
        default=False
    )("showiovs")
    
    # Final grammar
    expr = showiovs + project_tag + periods + lumi_tag + operands
    expr = StringStart() + expr + StringEnd()
    
    return expr
    
def query_defects(range_iov, defect_expressions, restrict):
    """
    Return the defective IoVs for a given set of defect_expressions.
    """
    ddb = DefectsDB()
    
    # Build a list of channels to query
    channels = set()
    all_defects = ddb.defect_names | ddb.virtual_defect_names
    for de in defect_expressions:
        channels.update(de.query_channels(all_defects))
    
    iovs_by_defect = ddb.retrieve(channels=channels, *range_iov).by_channel
    
    iovs_by_defect = dict((defect, iovs & restrict) 
                          for defect, iovs in sorted(iovs_by_defect.iteritems()))
    
    # Fill in channels for which there are no defects in the requested range
    for defect in channels:
        if defect not in iovs_by_defect:
            iovs_by_defect[defect] = IOVSet()
    
    return [(de, de.compute(iovs_by_defect)) for de in defect_expressions]

def show_info(what, lbs, lumis, iovs, defects, options):
    """
    Show luminosity for a given `what` (e.g., period, total), using lbs and lumis
    iovs are the list of iovs to compute over.
    """
    if not iovs:
        print what, ": No iovs to process in selected range."
        if defects:
            return dict((d, 0) for d in defects)
        return 0, 0
    
    # Just show the total luminosity by this `what`
    lumi = compute_lumi(lbs, lumis, iovs) / 1e6
    print "%s%9.3f pb^-1" % ("--" if defects else "  ", lumi),
        
    first, last = iovs.first.since.run, iovs.last.until.run
    args = what, len(iovs.runs), first, last
    print "period %-3s %3i runs [%6i, %6i] " % args
    
    if defects:
        # Show the luminosity taken by each set of defects
        lumi = {"total":lumi}
        for i, (defect_expr, defectiovs) in enumerate(defects):
            this_lumi = compute_lumi(lbs, lumis, iovs & defectiovs) / 1e6
            lumi[defect_expr] = this_lumi
            print "  %9.3f pb^-1   %-40s" % (this_lumi, defect_expr)
            if options.get("showiovs"):
                (defectiovs & iovs).pprint()
    
    return len(iovs.runs), lumi

def show_by_period(periods, good_iovs, options, defects):
    """
    Sum the luminosity over periods.
    
    `periods`: [(period name, iovs)] list
    `good_iovs`: List of iovs to be considered
    `options`: Parsed options
    `defects`: List of [(defect, expression)] to consider
    """
    total_lumi, total_runs = 0, 0
    total_by_defect = dict((defect_expr, 0) for defect_expr, d in defects)
    total_by_defect["total"] = 0
    for period, period_iovs in periods.period_iovs:
        lbs, lumis = fetch_lumi_inputs(period_iovs.range_iov, options["lumi_tag"])
        period_iovs &= good_iovs
        
        runs, lumi = show_info(period, lbs, lumis, period_iovs, defects, options)
        
        if defects:
            for defect, lumi in sorted(lumi.iteritems()):
                total_by_defect[defect] += lumi
        else:
            total_lumi += lumi
            
        total_runs += runs
        
    if len(periods.period_iovs) > 1:
        print 
        
        if not defects:
            print "  %9.3f pb^-1" % total_lumi,
        else:
            print "  %9.3f pb^-1" % total_by_defect["total"], # All luminosity
        
        print "TOTAL    %3i runs " % total_runs
        
        if defects:
            for defect, defect_iovs in defects:
                print "  %9.3f pb^-1   %s" % (total_by_defect[defect], defect)

def process(periods, good_iovs, options):
    """
    Process one GRL.
    """
    exprs = options.get("defect_expressions", [])
    defects = query_defects(good_iovs.range_iov, exprs, good_iovs)
    
    if periods.specified:
        show_by_period(periods, good_iovs, options, defects)
    else:
        lbs, lumis = fetch_lumi_inputs(good_iovs.range_iov, options["lumi_tag"])
        show_info("TOTAL", lbs, lumis, good_iovs, defects, options)
    
    print

def show_usage():
    print dedent(sys.modules[__name__].__doc__).strip()

def main():
    from sys import argv
    args = argv[1:]
    
    if not args:
        show_usage()
        raise SystemExit(1)
        
    def fixup(a):
        """
        Detect commandline args of the form "ABC: def ghi" (without quotes)
        and put them in the form 'ABC:" def ghi"' (with explicit quotes)
        """
        m = re.match(r"^([A-Z]+)\:(.*)$", a)
        if m:
            return '%s:"%s"' % m.groups()
        return '"%s"' % a
    
    # Fixup commandline arguments with spaces in them so that the parser can
    # deal with them later.
    args = [fixup(a) if " " in a else a for a in args]
    query = " ".join(args)
    
    grammar = get_grammar()
    try:
        options = grammar.parseString(query).asDict()
    except ParseException as e:
        print "Failed to parse input query with message '%s':" % e.msg
        print "Parser broke here: "
        print query
        print " " * e.loc + "^"
        show_usage()
        raise SystemExit(1)
    
    print "[ NOT AN OFFICIAL TOOL - USE AT YOUR OWN RISK ]"
    print "[     No livetime or prescale corrections     ]"
    
    if DEBUG:
        pprint(options)
    
    periods = options["periods"]
    periods.load(options["project_tag"])
    if periods.specified:
        print "Using periods from project tag:", options["project_tag"]
    
    print
    
    grls = options.get("grls", None)
    if grls:
        for grl in grls:
            print "Processing %s:" % repr(grl.name)
            good_iovs = grl.load()
            process(periods, good_iovs, options)
    else:
        if not periods.specified:
            print "Specify either a GRL or at least one period."
            raise SystemExit(1)
        good_iovs = IOVSet([ RANGEIOV_VAL(IOVMIN, IOVMAX)])
        process(periods, good_iovs, options)

if __name__ == "__main__":
    main()
