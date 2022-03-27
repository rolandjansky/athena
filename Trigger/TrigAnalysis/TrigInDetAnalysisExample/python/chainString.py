#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# take a chain string configuration and decode it into the constituent parts,  
# so that it can be reconstructed without the tags. 

# This is an approximate python implementation of the equivalent c++ 
# class and wouldn't be needed if the  setPath() method on the GenericMonitoringTool
# worked correctly, since the decoding is done in the c++, and the path could then 
# be set correctly at that point. sadly, this doesn;t not seem to work, so we need 
# to decode consistently in the c++ and the python to make sure that things 
# are consistent

class chainString: 

  def __init__(self, input ):
    self.head    = ""
    self.tail    = ""
    self.roi     = ""
    self.vtx     = ""
    self.element = ""
    self.extra   = ""
    self.passed  = ""

    if ":" in input:
        parts = input.split( ":" )
    else:
        parts = [ input ]

    for part in parts :
        if part[-4:]==";DTE":
           self.passed = True
           part = part[0:-4]
        if self.head == "":
            self.head = part
            continue
        if "key=" in part:
            self.tail = part[4:]
        if "roi=" in part:
            self.roi = part[4:]
        if "vtx=" in part:
            self.vtx = part[4:]
        if "te=" in part:
            self.element = part[3:]
        if "extra=" in part:
            self.extra = part[6:]
    
    stuff = [ self.extra, self.roi, self.vtx, self.element ]

    sum = self.head

    if  self.tail != "" :
      if  sum == "" :
        sum = self.tail
      else:
        sum += "_" + self.tail 


    for part in stuff:
        if part != "":
            sum += "_"+part

    if self.passed: 
        sum += "_DTE"

    self.sum = sum

  # provide the summary    
  def summary( self ):
      return self.sum

  # printout if needed
  def printchain( self ):
      print( "  head:   ", self.head   )
      print( "  tail:   ", self.tail   )
      print( "  vtx:    ", self.vtx    )
      print( "  roi:    ", self.roi    )
      print( "  te:     ", self.element)
      print( "  extra:  ", self.extra  )
      print( "  passed: ", self.passed )
      print( "  sum:    ", self.sum    )
      print( "" )


# provide the summary without needing the 
# intermediate class instance

def summarise( input ) :
    return chainString( input ).summary()

