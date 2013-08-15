# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Pull the bunch group structure and format it nicely for the processRules script

usage:
python getBunchGroups.py XXX
   where XXX is the bunch group set ID. If it is available on the
   trigconf web-pages, the script will print out the size of the four
   relevant bunch groups.

"""

import sys
import pickle
import urllib2
import HTMLParser

class Spider:
  """Base class for pulling and formating the bunch group structure"""
  
  params = {}
  
  def __init__(self):
    self.params["bunchGroupID"]              = 0
    self.params["filled_bunches"]             = 214
    self.params["empty_bunches"]              = 4
    self.params["empty_after_filled_bunches"] = 100
    self.params["unp_iso_bunches"]            = 24
    self.params["unp_noniso_bunches"]         = 6
    
  def pullValues(self):
    """Fill the params dictionary with reasonable values"""
    pass

  def printValues(self):
    """Report the bunch group structure to the screen"""
    for key,val in sorted(self.params.items(), key=lambda x: x[0]):
      print "%s = %s" % (key, val)

class DictHTMLParser(HTMLParser.HTMLParser):
  """Parser to query the trigconf pages for the bunch group structure
  of specific bunch group set IDs"""

  def __init__(self, filehandler):
    HTMLParser.HTMLParser.__init__(self)
    
    self.inTable = False
    self.inStrong = False
    self.key = ""
    self.structure = {}

    self.feed(filehandler.read())
    
  def handle_starttag(self, tag, attrs):
    """The bunch group name will appear in a table, after a 'strong' tag"""
    
    #print "tag : %s , %s" % (tag, attrs)

    if tag == "td":
      self.inTable = True
    elif tag == "strong" and self.inTable:
      self.inStrong = True


  def handle_endtag(self, tag):
    """Protect ourselves against random tables starting and stopping"""
    
    if tag == "td":
      self.inTable = False

  def handle_data(self, data):
    """If we are in a table, look for the bunch group name, if we know
    the bunch group name, look for the bunch IDs"""
    
    data = data.strip()

    #print data

    if self.inTable and self.inStrong:
      self.key = data[4:]
      self.inStrong = False
      
    elif len(data) > 0 and self.inTable and len(self.key) > 0:
      #print "Found: %s" % self.key
      self.structure[self.key] = data.split("  ")
      self.key = ""

class TrigConfSpider(Spider):
  """Implementation of the bunch group spider using the TrigConf pages"""

  mapping = {"Filled"              : "filled_bunches",
             "Empty"               : "empty_bunches",
             "EmptyAfterFilled"    : "empty_after_filled_bunches",
             "UnpairedIsolated"    : "unp_iso_bunches",
             "UnpairedNonisolated" : "unp_noniso_bunches",
             }
  
  def __init__(self, id):
    Spider.__init__(self)
    self.params["bunchGroupID"] = id
    self.loc = "http://atlas-trigconf.cern.ch/bunchgroups?key="

  def pullValues(self):
    tcUrl = "%s%s" % (self.loc, self.params["bunchGroupID"])
    req = urllib2.Request(url = tcUrl,
                          headers = {'Accept': 'text/plain',
                                     'Content-Type': 'application/x-www-form-urlencoded'})    
    filehandler = urllib2.urlopen(req)

    d = DictHTMLParser(filehandler)

    if len(set(self.mapping.keys()) & set(d.structure.keys())) != len(self.mapping.keys()):
      print "The TrigConf page at %s did not return the structure of all the expected bunch groups" % tcUrl
      sys.exit(1)

    for key, val in self.mapping.items():
      self.params[val] = len(d.structure[key])

      
if __name__ == "__main__":
  """By defalt read the bunch group structure from the trigconf pages"""
  
  spider = TrigConfSpider(sys.argv[1])
  spider.pullValues()
  spider.printValues()
