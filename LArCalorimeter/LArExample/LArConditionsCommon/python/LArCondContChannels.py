# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# File: LArConditionsCommon/python/LArCondContChannels.py
# Author: RD Schaffer (R.D.Schaffer@cern.ch)

# The LArCondContChannels class provides the translation for between
# LArConditionsContainer group numbering and COOL channel numbering.
#

import operator

class LArCondContChannels(object) :
  def __init__(self) :
    self.Unknown              = 0
    self.SingleGroup          = 1
    self.SubDetectorGrouping  = 2
    self.FeedThroughGrouping  = 3
    self.ExtFeedThroughGrouping  = 4
    self.ExtSubDetectorGrouping  = 5
    
    # Offset values
    #self.offsetCorrs         =   3
    self.offsetFtNegBarrel    =   3
    self.offsetFtPosBarrel    =  35
    self.offsetFtNegEndcap    =  67
    self.offsetFtPosEndCap    =  92
    self.nFT                  = 114
    self.nFTandCorr           = 117
    self.ftPerGain            = self.nFT

    # For extended FT grouping:
    self.nFTAcc               = 117
    self.offsetFtNegBarrelPS  =   3 + self.nFTAcc #= 120
    self.offsetFtPosBarrelPS  =  35 + self.nFTAcc #= 152 
    self.offsetFtNegEndcapPS  =  67 + self.nFTAcc #= 184
    self.offsetFtPosEndCapPS  =  92 + self.nFTAcc #= 209
    self.nFTPS                = 117 + self.nFTAcc #= 231
    
    self.nChannelsPerGain     =   236
    self.nGains               =   3
    self.subDetsOffset        =   3
    self.nSubDets             =   4
    self.subDetGainCorrOffset = self.subDetsOffset + self.nSubDets    
  
  def isUnknown(self, type):
    return type == self.Unknown

  def isSingleGroup(self, type):
    return type == self.SingleGroup

  def isSubDetectorGrouping(self, type):
    return type == self.SubDetectorGrouping
  
  def isExtSubDetectorGrouping(self, type):
    return type == self.ExtSubDetectorGrouping

  def isFeedThroughGrouping(self, type):
    return type == self.FeedThroughGrouping

  def isExtFeedThroughGrouping(self, type):
    return type == self.ExtFeedThroughGrouping

    
  # folderFeedThruGroupChanSel:
  #
  # Return the list of COOL channels given for a subset of
  # feedthroughs. There are four groupings of feedthroughs possible,
  # selected by the input argument:
  #
  #   "negBarrel" - EMB
  #   "posBarrel" - EMB
  #   "negEndcap" - EMEC, HEC, FCAL
  #   "posEndcap" - EMEC, HEC, FCAL
  #
  # The list of channels is of the form:
  #
  # <channelSelection> x:y, z, a:b </channelSelection>
  #
  # This should be provided in the IOVDbSvc.Folders list for each
  # folder.
  #
  # Input arguments:
  #    feedThruGroup - "negBarrel", "posBarrel", "negEndcap", "posEndcap"
  #    gainList      - a list of gains values: 0, 1, and/or 2, default
  #                    value is all gains
  #
  def folderFeedThruGroupChanSel(self, feedThruGroup, gainList=[0,1,2]):
    gains = gainList
    if operator.isNumberType(gainList): gains = [gainList]
    for g in gains:
      if g < 0 or g > 2:
        print("LArCondContChannels.folderFeedThruGroupChanSel: invalid gain value - must be 0-2 - ", g)
        return "<channelSelection>  </channelSelection>"

    if feedThruGroup == "negBarrel":
      feedThruList = []
      posNeg       = -1
      isBarrel     = True
      for ft in range(32):
        feedThruList += [ft]
      chans = self.feedThruChannelSelection(feedThruList, gains, posNeg, isBarrel)
    elif feedThruGroup == "posBarrel":
      feedThruList = []
      posNeg       = 1
      isBarrel     = True
      for ft in range(32):
        feedThruList += [ft]
      chans = self.feedThruChannelSelection(feedThruList, gains, posNeg, isBarrel)
    elif feedThruGroup == "negEndcap":
      feedThruList = []
      posNeg       = -1
      isBarrel     = False
      for ft in range(25):
        feedThruList += [ft]
      chans = self.feedThruChannelSelection(feedThruList, gains, posNeg, isBarrel)
    elif feedThruGroup == "posEndcap":
      feedThruList = []
      posNeg       = 1
      isBarrel     = False
      for ft in range(25):
        feedThruList += [ft]
      chans = self.feedThruChannelSelection(feedThruList, gains, posNeg, isBarrel)
    else:
      print("LArCondContChannels.folderFeedThruGroupChanSel: feedThru grouping, found", feedThruGroup)
      print("Allowed values: \"negBarrel\", \"posBarrel\", \"negEndcap\", \"posEndcap\"")
      return "<channelSelection>  </channelSelection>"


    chanSel = self.channelSelection(chans)
    return chanSel

  # Return a list of COOL channels given a list of feedthrough
  # numbers. This list is of the form:
  #
  # <channelSelection> x:y, z, a:b </channelSelection>
  #
  # This should be provided in the IOVDbSvc.Folders list for each
  # folder.
  #
  # This should be run separately for barrel/endcap and
  # pos/neg, appending the results.
  #
  # Input args:
  #    Four lists of feedThrough numbers (starting from 0)
  #    for pos/neg Barrel/EndCap, e.g. [4, 8, 30]
  #    (may also just be a single number):
  #      negB_FTList  - negative barrel - 0 to 31
  #      posB_FTList  - positive barrel - 0 to 31
  #      negEC_FTList - negative endcap - 0 to 24
  #      posEC_FTList - positive endcap - 0 to 24
  #
  #    gainList     - list of gains (0 to 2), may be a single number or list
  #    isCorrection - False - normal conditions, True - corrections (default is False)
  def folderFeedThruChannelSelection(self,
                                     negB_FTList,
                                     posB_FTList,
                                     negEC_FTList,
                                     posEC_FTList,
                                     gainList,
                                     isCorrection=False):
    chans = []
    # Check that the lists are not empty
    if len(negB_FTList)   == 0 and \
       len(posB_FTList)   == 0 and \
       len(negEC_FTList)  == 0 and \
       len(posEC_FTList)  == 0:

      print("LArCondContChannels.folderFeedThruChannelSelection Warning: FeedThru lists are all empty - no channels selected")

    elif len(gainList) == 0:
      print("LArCondContChannels.folderFeedThruChannelSelection Warning: Gain list is empty - no channels selected")
    else:
      if len(negB_FTList)   > 0:
        chans += self.feedThruChannelSelection(
          negB_FTList,
          gainList,
          -1,
          True,
          isCorrection)
      if len(posB_FTList)   > 0:
        chans += self.feedThruChannelSelection(
          posB_FTList,
          gainList,
          1,
          True,
          isCorrection)
      if len(negEC_FTList)  > 0:
        chans += self.feedThruChannelSelection(
          negEC_FTList,
          gainList,
          -1,
          False,
          isCorrection)
      if len(posEC_FTList)  > 0:
        chans += self.feedThruChannelSelection(
          posEC_FTList,
          gainList,
          1,
          False,
          isCorrection)

    print("chans ",chans)
    chanSel = self.channelSelection(chans)
    return chanSel





  def folderExtFeedThruChannelSelection(self,
                                     negB_FTList,
                                     posB_FTList,
                                     negEC_FTList,
                                     posEC_FTList,
                                     gainList,
                                     isCorrection=False):
    chans = []
    # Check that the lists are not empty
    if len(negB_FTList)   == 0 and \
       len(posB_FTList)   == 0 and \
       len(negEC_FTList)  == 0 and \
       len(posEC_FTList)  == 0:

      print("LArCondContChannels.folderFeedThruChannelSelection Warning: FeedThru lists are all empty - no channels selected")

    elif len(gainList) == 0:
      print("LArCondContChannels.folderFeedThruChannelSelection Warning: Gain list is empty - no channels selected")
    else:
      if len(negB_FTList)   > 0:
        chans += self.extFeedThruChannelSelection(
          negB_FTList,
          gainList,
          -1,
          True,
          isCorrection)
      if len(posB_FTList)   > 0:
        chans += self.extFeedThruChannelSelection(
          posB_FTList,
          gainList,
          1,
          True,
          isCorrection)
      if len(negEC_FTList)  > 0:
        chans += self.extFeedThruChannelSelection(
          negEC_FTList,
          gainList,
          -1,
          False,
          isCorrection)
      if len(posEC_FTList)  > 0:
        chans += self.extFeedThruChannelSelection(
          posEC_FTList,
          gainList,
          1,
          False,
          isCorrection)

    print("chans ",chans)
    chanSel = self.channelSelection(chans)
    return chanSel






  # Return a list of COOL channels given a list of feedthrough
  # numbers. This should be run separately for barrel/endcap and
  # pos/neg, appending the results.
  #
  # Input args:
  #    feedThruList - list of feedthoughs, e.g. [4, 8, 30]
  #                   may be a single number or list
  #    gainList     - list of gains (0 to 2), may be a single number or list
  #    posNeg       - = +1 positive or = -1 negative
  #    isBarrel     - flag for barrel or endcap, default=True
  #    isCorrection - False - normal conditions, True - corrections (default is False)
  def feedThruChannelSelection(self,
                               feedThruList,
                               gainList,
                               posNeg,
                               isBarrel=True,
                               isCorrection=False):
    # allow for single values rather than lists
    fts   = feedThruList
    if operator.isNumberType(feedThruList): fts = [feedThruList]
    gains = gainList
    if operator.isNumberType(gainList): gains = [gainList]
    fts.sort()
    # Check value for posNeg
    if posNeg != -1 and posNeg != +1:
      print(__name__)
      raise RuntimeError (__name__ +"posNeg must be either +/-1, found: " + str(posNeg))

    offset = 0
    # First get offset per gain
    if isBarrel:
      if posNeg == -1:
        offset = self.offsetFtNegBarrel
      else:
        offset = self.offsetFtPosBarrel
    else:
      # Endcap
      if posNeg == -1:
        offset = self.offsetFtNegEndcap
      else:
        offset = self.offsetFtPosEndCap

    # Now get channels
    chans = []
    # Now loop over gains and add in a FT for each gain
    for gain in gains:
      offset1 = offset + gain*self.ftPerGain
      # Calculate channels for FT list
      for ft in fts:
        chans += [ft + offset1]

    # Return channels
    return chans


  
  def extFeedThruChannelSelection(self,
                               feedThruList,
                               gainList,
                               posNeg,
                               isBarrel=True,
                               isCorrection=False):
    # allow for single values rather than lists
    fts   = feedThruList
    if operator.isNumberType(feedThruList): fts = [feedThruList]
    gains = gainList
    if operator.isNumberType(gainList): gains = [gainList]
    fts.sort()
    # Check value for posNeg
    if posNeg != -1 and posNeg != +1:
      print(__name__)
      raise RuntimeError (__name__ +"posNeg must be either +/-1, found: " + str(posNeg))

    offset = 0
    # First get offset per gain
    if isBarrel:
      if posNeg == -1:
        offset   = self.offsetFtNegBarrel
        offsetPS = self.offsetFtNegBarrelPS
      else:
        offset   = self.offsetFtPosBarrel
        offsetPS = self.offsetFtPosBarrelPS
    else:
      # Endcap
      if posNeg == -1:
        offset = self.offsetFtNegEndcap
        offsetPS = self.offsetFtNegEndcapPS
      else:
        offset = self.offsetFtPosEndCap
        offsetPS = self.offsetFtPosEndCapPS

    # Now get channels
    chans = []
    # Now loop over gains and add in a FT for each gain
    for gain in gains:
      offset1 = offset + gain*self.nChannelsPerGain
      offset1PS = offsetPS + gain*self.nChannelsPerGain
      # Calculate channels for FT list
      for ft in fts:
        chans += [ft + offset1] #Standard channel
        chans += [ft + offset1PS] #Corresponding PS channel
        # add the EMEC in HEC channels
        if not isBarrel:
          if (posNeg == -1 and ft==3): # C-Side
            chans+=[228+3+gain*self.nChannelsPerGain]
          if (posNeg == -1 and ft==10):# C-Side
            chans+=[230+3+gain*self.ChannelsPerGain]
          if (posNeg == -1 and ft==16):# C-Side
            chans+=[232+3+gain*self.ChannelsPerGain]
          if (posNeg == -1 and ft==22):# C-Side
            chans+=[234+3+gain*self.ChannelsPerGain]
          if (posNeg == 1 and ft==3): # A-Side
            chans+=[229+3+gain*self.ChannelsPerGain]
          if (posNeg == 1 and ft==10):# A-Side
            chans+=[231+3+gain*self.ChannelsPerGain]
          if (posNeg == 1 and ft==16):# A-Side
            chans+=[233+3+gain*self.ChannelsPerGain]
          if (posNeg == 1 and ft==22):# A-Side
            chans+=[235+3+gain*self.ChannelsPerGain]


    # Return channels
    return chans

  # Return a string with a channel selection specification of the form
  #
  # <channelSelection> x:y, z, a:b </channelSelection>
  #
  # Input: list of channel numbers
  def channelSelection(self, channels):
    chans = channels
    chans.sort()
    chanSel = "<channelSelection>"
    first   = True
    for c in chans:
      if first:
        first = False
      else:
        chanSel += ', '
      chanSel += str(c)
    chanSel += "</channelSelection>"
    return chanSel


  # Return then COOL channel number given the SubDet name and gain
  #
  # Allowed subdet names: EMB, EMEC, HEC, FCAL
  def subDetChannelSelection(self, subDetName, gain):
    if gain >= self.nGains:
      print("invalid gain: range in 0 to",self.nGain-1)
      return -1
    offset = gain*self.nSubDets + self.subDetsOffset
    if subDetName == "Corr":
      return gain
    elif subDetName == "EMB":
      return offset
    elif subDetName == "EMEC":
      return offset + 1
    elif subDetName == "HEC":
      return offset + 2
    elif subDetName == "FCAL":
      return offset + 3

    
  # Given a COOL channel number extract the feedThrough number and as well provide
  # identification as to where it is (isBarrel + posNeg) and provide the gain.
  #
  # return a tuple of { isBarrel(True/False), posNeg (-1,1), FeedThru (0-32), gain }
  #   FeedThru == -1 if this is an invalid feedthru number, or is a correction
  #
  # Example usage:
  #   (isBarrel, posNeg, feedThru, gain) = decoder.decodeFeedThruChannelNumber(chan)
  #
  # Input arg:
  #    channelNumber - COOL channel number

  def decodeFeedThruChannelNumber(self, channelNumber):
    # initial values
    isBarrel = True
    posNeg   = -1
    feedThru = -1
    gain     = channelNumber

    if channelNumber < self.offsetFtNegBarrel:
      # is a correction
      return (isBarrel, posNeg, feedThru, gain)

    chanNum  = channelNumber - self.offsetFtNegBarrel
    gain     = chanNum/self.ftPerGain
    chanNum  = chanNum - gain*self.ftPerGain + self.offsetFtNegBarrel

    if chanNum < self.offsetFtPosBarrel:
      # is negative barrel
      feedThru = chanNum - self.offsetFtNegBarrel
    elif chanNum < self.offsetFtNegEndcap:
      # is positive barrel
      feedThru = chanNum - self.offsetFtPosBarrel
      posNeg   = 1
    elif chanNum < self.offsetFtPosEndCap:
      # is negative endcap
      feedThru = chanNum - self.offsetFtNegEndcap
      isBarrel = False
    else:
      # is positive endcap
      feedThru = chanNum - self.offsetFtPosEndCap
      posNeg   = 1
      isBarrel = False

    return (isBarrel, posNeg, feedThru, gain)

  
  def decodeExtFeedThruChannelNumber(self, channelNumber):
    # initial values
    isBarrel = True
    posNeg   = -1
    feedThru = -1
    gain     = channelNumber

    if channelNumber < self.offsetFtNegBarrel:
      # is a correction
      return (isBarrel, posNeg, feedThru, gain)

    chanNum  = channelNumber - self.offsetFtNegBarrel
    gain     = chanNum/self.nChannelsPerGain
    chanNum  = chanNum - gain*self.nChannelsPerGain + self.offsetFtNegBarrel

    if chanNum < self.offsetFtPosBarrel:
      # is negative barrel
      feedThru = chanNum - self.offsetFtNegBarrel
      posNeg=-1
      isBarrel=True
    elif chanNum < self.offsetFtNegEndcap:
      # is positive barrel
      feedThru = chanNum - self.offsetFtPosBarrel
      posNeg   = 1
      isBarrel= True
    elif chanNum < self.offsetFtPosEndCap:
      # is negative endcap
      feedThru = chanNum - self.offsetFtNegEndcap
      posNeg=-1
      isBarrel = False
    elif chanNum < self.nFTAcc:
      # is positive endcap
      feedThru = chanNum - self.offsetFtPosEndCap
      posNeg   = 1
      isBarrel = False
    elif chanNum < self.offsetFtNegEndcapPS:
      feedThru = chanNum - self.offsetFtNegBarrelPS
      posNeg=-1
      isBarrel=True
    elif chanNum < self.offsetFtNegEndcapPS:
      # is positive barrel
      feedThru = chanNum - self.offsetFtPosBarrelPS
      posNeg   = 1
      isBarrel= True
    elif chanNum < self.offsetFtPosEndCapPS:
      # is negative endcap
      feedThru = chanNum - self.offsetFtNegEndcapPS
      posNeg=-1
      isBarrel = False
    elif chanNum < self.nFTPS:
      # is positive endcap
      feedThru = chanNum - self.offsetFtPosEndCapPS
      posNeg   = 1
      isBarrel = False
    elif chanNum == 228+3:
      feedThru = 3
      posNeg   = -1
      isBarrel = False
    elif chanNum == 229+3:
      feedThru = 3
      posNeg   = 1
      isBarrel = False
    elif chanNum == 230+3:
      feedThru = 10
      posNeg   = -1
      isBarrel = False
    elif chanNum == 231+3:
      feedThru = 10
      posNeg   = 1
      isBarrel = False
    elif chanNum == 232+3:
      feedThru = 16
      posNeg   = -1
      isBarrel = False
    elif chanNum == 233+3:
      feedThru = 16
      posNeg   = 1
      isBarrel = False
    elif chanNum == 234+3:
      feedThru = 22
      posNeg   = -1
      isBarrel = False
    elif chanNum == 235+3:
      feedThru = 22
      posNeg   = 1
      isBarrel = False
    else: #should never happen
      feedThru = -999
      posNeg   = -999
      isBarrel = False

    return (isBarrel, posNeg, feedThru, gain)


  # Given a COOL channel number extract the subdet name and gain 
  #
  # return a tuple of { detName, gain }
  #
  # Example usage:
  #   (detName, gain) = decoder.decodeSubDetChannelNumber(chan)
  #
  # Input arg:
  #    channelNumber - COOL channel number
  #
  def decodeSubDetChannelNumber(self, channelNumber):
    # initial values
    subDet   = "Unknown"
    gain     = -1
    if channelNumber < self.subDetsOffset:
      # is a correction
      subDet   = "Corr"
      gain     = channelNumber
    else:
      chanSD = channelNumber - self.subDetsOffset
      gain   = chanSD/self.nSubDets
      chanSD = chanSD - gain*self.nSubDets
      if chanSD == 0:
        subDet   = "EMB"
      elif chanSD == 1:
        subDet   = "EMEC"
      elif chanSD == 2:
        subDet   = "HEC"
      elif chanSD == 3:
        subDet   = "FCAL"

    return (subDet, gain)

    
##   def channelSelection(self, channelList, groupType):
##     if groupType == self.SingleGroup:
##       pass
##     elif groupType == self.SubDetectorGrouping:
##       pass
##     elif groupType == self.FeedThroughGrouping:
##       pass


if __name__ == '__main__':

  # Test of LArCondContChannels
  decoder = LArCondContChannels()

  print("0 - unknown", decoder.isUnknown(0),"SG", decoder.isSingleGroup(0),"SDG",decoder.isSubDetectorGrouping(0),"FT",decoder.isFeedThroughGrouping(0))
  print("1 - unknown", decoder.isUnknown(1),"SG", decoder.isSingleGroup(1),"SDG",decoder.isSubDetectorGrouping(1),"FT",decoder.isFeedThroughGrouping(1))
  print("2 - unknown", decoder.isUnknown(2),"SG", decoder.isSingleGroup(2),"SDG",decoder.isSubDetectorGrouping(2),"FT",decoder.isFeedThroughGrouping(2))
  print("3 - unknown", decoder.isUnknown(3),"SG", decoder.isSingleGroup(3),"SDG",decoder.isSubDetectorGrouping(3),"FT",decoder.isFeedThroughGrouping(3))

  # Check channels list

  # Barrel:
  ftList_bp  = [1,19,25,31]
  ftList_bn  = [0,18,24,30]
  # EndCap:
  ftList_ecp = [1,19,24]
  ftList_ecn = [0,18,23]


  print(" ====== CHECKS FOR FEEDTHROUGHGROUPING ====== ")
  
  for gain in range(3):
    chans      = []
    gainList = [ gain ]
    chans += decoder.feedThruChannelSelection(ftList_bn,  gainList, -1)
    chans += decoder.feedThruChannelSelection(ftList_bp,  gainList,  1)
    chans += decoder.feedThruChannelSelection(ftList_ecn, gainList, -1, False)
    chans += decoder.feedThruChannelSelection(ftList_ecp, gainList,  1, False)

    print( "ft list b neg: ", ftList_bn)
    print( "ft list b pos: ", ftList_bp)
    print( "ft list ec neg: ", ftList_ecn)
    print( "ft list ec pos: ", ftList_ecp)
    print( "gain, chans: ", gainList, chans)

    chanSel = decoder.folderFeedThruChannelSelection(ftList_bn,
                                                     ftList_bp,
                                                     ftList_ecn,
                                                     ftList_ecp,
                                                     gainList)
    print( chanSel )

    for chan in chans:
      (isBarrel, posNeg, feedThru, gain) = decoder.decodeFeedThruChannelNumber(chan)
      print( "chan, isBarrel, posNeg, feedThru, gain",chan, isBarrel, posNeg, feedThru, gain)


  # Check subdet channel decoding
  for chan in range(15):
    (detName, gain) = decoder.decodeSubDetChannelNumber(chan)
    chan1 = decoder.subDetChannelSelection(detName, gain)
    print( "chan, detName, gain, and chan recalc", chan, detName, gain, chan1)


  print( " ====== CHECKS FOR EXTENDED FEEDTHROUGHGROUPING ====== ")
  for gain in range(3):
    chans      = []
    gainList = [ gain ]
    chans += decoder.extFeedThruChannelSelection(ftList_bn,  gainList, -1)
    chans += decoder.extFeedThruChannelSelection(ftList_bp,  gainList,  1)
    chans += decoder.extFeedThruChannelSelection(ftList_ecn, gainList, -1, False)
    chans += decoder.extFeedThruChannelSelection(ftList_ecp, gainList,  1, False)

    print( "ft list b neg: ", ftList_bn)
    print( "ft list b pos: ", ftList_bp)
    print( "ft list ec neg: ", ftList_ecn)
    print( "ft list ec pos: ", ftList_ecp)
    print( "gain, chans: ", gainList, chans)

    chanSel = decoder.folderExtFeedThruChannelSelection(ftList_bn,
                                                        ftList_bp,
                                                        ftList_ecn,
                                                        ftList_ecp,
                                                        gainList)
    print( chanSel)

    for chan in chans:
      (isBarrel, posNeg, feedThru, gain) = decoder.decodeExtFeedThruChannelNumber(chan)
      print( "chan, isBarrel, posNeg, feedThru, gain",chan, isBarrel, posNeg, feedThru, gain)


