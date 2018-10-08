#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from os import listdir
from os.path import join as pjoin
import xml.etree.cElementTree as cElementTree

from .db import fetch_iovs
from .events import process_iovs
from .sugar import define_iov_type, IOVSet, RunLumi, RunLumiType

@define_iov_type
def GRL_IOV():
    "Represent a good run region"

def grl_from_dir(xmldir):
    """
    Loads valid IOV ranges if they appear in any grl file whose name ends with ".xml"
    """
    return load_grl_iovs_any(*(pjoin(xmldir, f) 
                             for f in listdir(xmldir) if f.endswith(".xml")))

def load_grl_iovs_any(*files):
    """
    Use IOV ranges from the input `files` xmls if the lumirange is set in any file
    """
    grl_iovsets = [grl_iovs_from_xml(f) for f in files]
    
    assert all(i.ordered for i in grl_iovsets)
    
    result = IOVSet()    
    for since, until, grl_states in process_iovs(*grl_iovsets):
        if any(grl_states):
            result.add(since, until)
    
    return result.solidify(GRL_IOV)

def load_grl(xml_file, IOVSet_class=IOVSet):
    with open(xml_file, "rb") as fd:
        return load_grl_string(fd.read(), IOVSet_class)
    
def load_grl_string(data, IOVSet_class=IOVSet):
    xml = cElementTree.fromstring(data)
    result = []

    for lbc in xml.getiterator('LumiBlockCollection'):
        run = int(lbc.find('Run').text)
        for lbr in lbc.findall('LBRange'):
            since, until = int(lbr.get('Start')), int(lbr.get('End')) + 1
            
            result.append((RunLumi(run, since), RunLumi(run, until)))
    
    return IOVSet_class(map(GRL_IOV._make, sorted(result)))

def make_grl(iovset, name="unknown", version="unknown"):
    assert len(iovset.channels) <= 1
    
    from datetime import datetime
    from textwrap import dedent
    
    result = [dedent("""
    <?xml version="1.0" ?>
    <!DOCTYPE LumiRangeCollection
      SYSTEM 'http://atlas-runquery.cern.ch/LumiRangeCollection.dtd'>
    <!-- Good-runs-list created by DQUtils on {time} -->
    <LumiRangeCollection>
       <NamedLumiRange>
          <Name>{name}</Name>
          <Version>{version}</Version>""".format(
          name=name, version=version, 
          time=datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f"))).strip()]
          
    for run, iovs in sorted(iovset.by_run.iteritems()):
        result.append("      <LumiBlockCollection>")
        result.append("         <Run>%i</Run>" % run)
        for iov in iovs:
            arg = iov.since.lumi, iov.until.lumi-1
            result.append('         <LBRange Start="%i" End="%i"/>' % arg)
        result.append("      </LumiBlockCollection>")
    result.append("   </NamedLumiRange>")
    result.append("</LumiRangeCollection>")
    return "\n".join(result)

def grl_contains_run_lb(grl, runlb):
    # takes IOVSet of GRL_IOV, says whether runlb is in it
    # runlb can be RunLumi type or tuple pair
    if isinstance(runlb, RunLumiType):
        runlb_ = runlb
    else:
        runlb_ = RunLumi(*runlb)
    return any(_.contains_point(runlb_) for _ in grl)

# Deprecated alias
def grl_iovs_from_xml(*args, **kwargs):
    from warnings import warn
    warn("grl_iovs_from_xml was renamed to load_grl", DeprecationWarning)
    return load_grl(*args, **kwargs)

def test():
    path = "/afs/cern.ch/user/b/beate/public/DQAna/StableBeams-periodC1.xml"
    iovs = grl_iovs_from_xml(path)
    
    from pprint import pprint
    pprint(iovs)
            
if __name__ == "__main__":
    test()
