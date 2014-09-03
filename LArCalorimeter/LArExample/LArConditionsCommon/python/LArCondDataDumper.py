# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------
# class LArCondDataDumper
#
#   Used for printing out the contents of LArConditionsContainers
#  
#----------------------------------------------------------------
# python for LArConditionsContainer
from LArRawConditions.LArConditionsContainer import *
# pyKernel access
from PyKernel import PyKernel

class LArCondDataDumper(object) :
  def __init__(self, larCablingSvc, outputLevel) :
    self.larCablingSvc = larCablingSvc
    self.outputLevel   = outputLevel

  # workaround, not needed in 12.2.0 and later. 
  def pyroot_typedef_bug_workaround(self) :
    # delete iterator types to prevent spurious typedeffing
    import libPyROOT

    try :
      del libPyROOT.const_iterator
    except AttributeError:
      pass

    try:
      del libPyROOT.iterator
    except AttributeError:
      pass



  #--------------------------------------------------------------
  # print LAr*P data objects
  #--------------------------------------------------------------
  def printLarCondObject(self, obj):
    t = str(type(obj))

    if -1!=t.find("LArRampP"):
      obj_str = ""
      for obj_i in obj.m_vRamp:
        obj_str = obj_str+str(obj_i)+' '   
      print "ramp = ", obj_str


    elif -1!=t.find("LArShapeP"):
      obj_str = ""
      for obj_i in obj.m_vShape :
        obj_str = obj_str+str(obj_i)+' '   
      print "LArShape = ", obj_str
      obj_str = ""
      for obj_i in obj.m_vShapeDer :
        obj_str = obj_str+str(obj_i)+' '   
      print "LArShapeDer = ", obj_str

    elif -1!=t.find("LArPedestalP"):
      obj_str = ""
      for obj_i in obj.m_vPedestal:
        obj_str = obj_str+str(obj_i)+' '   
      print "Pedestal = ", obj_str

      obj_str = ""
      for obj_i in obj.m_vPedestalRMS:
        obj_str = obj_str+str(obj_i)+' '   
      print "PedestalRMS = ", obj_str

    elif -1!=t.find("LArAutoCorrP"):
      obj_str = ""
      for obj_i in obj.m_vAutoCorr:
        obj_str = obj_str+str(obj_i)+' '   
      print "AutoCorr = ", obj_str

    elif -1!=t.find("LArDAC2uAP"):
      obj_str = ""
      print "DAC2uA = ", obj.m_DAC2uA

    elif -1!=t.find("LArfSamplP"):
      obj_str = ""
      print "fSampl = ", obj.m_fSampl

    elif -1!=t.find("LAruA2MeVP"):
      print "uA2MeV = ", obj.m_uA2MeV

    elif -1!=t.find("LArNoiseP") :
      print "Noise = ", obj.m_Noise

    elif -1!=t.find("LArMinBiasP") :
      print "MinBias = ", obj.m_MinBiasRMS

    elif -1!=t.find("LArMphysOverMcalP") :
      print "MphysOverMcal = ", obj.m_MphysOverMcal

    elif -1!=t.find("LArOFCP") :
      delay=0 
      for obj_i in obj.m_vOFC_a:
        print " delay=",delay

        obj_str = ""
        for obj_j in obj_i: 
          obj_str = obj_str + str(obj_j) + ' '
        print " OFC_a = ", obj_str

        obj_str = ""
        for obj_j in obj.m_vOFC_b[delay]:
          obj_str = obj_str + str(obj_j) + ' '
        print " OFC_b = ", obj_str
        delay +=1 


    elif -1!=t.find("LArCaliWaveVec") :
      for wave in obj:
        print " DAC_0 = ", wave.getDAC(), " Dt=", wave.getDt()

        obj_str = ""
        i = 0
        for obj_i in wave.getWave():
          obj_str = obj_str + str(i) + ' ' + str(obj_i) + ' '   
          i += 1
        print " Wave = ", obj_str


  # 
  #--------------------------------------------------------------
  # print out data for a list of containers
  #--------------------------------------------------------------
  def printAllLArCondData(self, ListofType, ListofKey):

    import sys
    import traceback

    if len(ListofType) != len(ListofKey) : 
      print " ERROR: Type and Key not the same length"
      return 

    first = True

    for i in range(len(ListofType)):
      key = ListofKey[i]
      typ = ListofType[i]

      # Add on extra iterators for the python class
      try:
        TheCont = LArConditionsContainer(typ.__name__)
      except:
        print "Cannot get ",typ.__name__," Container"

      try :
        self.pyroot_typedef_bug_workaround()

        container = PyKernel.retrieveDet(typ,key)
        if first:
          # First time through get the cabling server and id helper
          #self.larCablingSvc = container.larCablingSvc()
          self.onlineID      = container.onlineHelper()
          first = False
          
        # for CaliWave, T=LArCaliWaveVec
        caliWave = -1!=key.find("CaliWave")

        # Loop Over Gains
        for gain in range(container.minGain(), container.minGain()+container.nGains()):
          print "    Accessing ",key," gain =  ", gain
          # Give Conditions
          n=0
          FeedThroughs = {} 
          for obj,id in container.conditionsIter(gain):
            ft = self.onlineID.feedthrough_name(id)  
            FeedThroughs[ft]=1 
            if ( (not caliWave) and (not obj.isEmpty()) ) or (caliWave and len(obj)!=0):
              n=n+1  
              if self.outputLevel <= 2:
                str_id1 = self.onlineID.print_to_string(id)
                print str_id1
                if self.larCablingSvc.isOnlineConnected(id) :
                  off_id = self.larCablingSvc.cnvToIdentifier(id)             
                  str_id2 = self.onlineID.print_to_string(off_id)
                  print str_id2
                else: 
                  print " disconnected channel" 
                self.printLarCondObject(obj)
                #self.getDataMembers(obj, type(obj))

          print n, " ",key," accessed" 
          keys = FeedThroughs.keys()
          keys.sort() 
          print " Feedthroughs ",keys 

      except:
        print " accessing ",key," failed "
        typ, value, traceBack = sys.exc_info()
        #_logger.error("resetDefinedJobs : %s %s" % (typ,value))
        print traceback.format_exc() 



  # Loop over data members and print them out.
  # We assume here that they are simple types or vectors of simple types
  # Get object type
  def getDataMembers(self, obj, objName, tab = " "):
    import operator
    if operator.isNumberType(obj):
      #print "Attribute name = ", objName, ", value = ", obj
      print tab, objName, " = ", obj
    else:
      objType = type(obj)
      # If Object is a method, replace obj with the result of the method
      if objType.__name__ == 'MethodProxy':
        try:
          obj = obj()
          objType = type(obj)
        # Exception is thrown for functions that take arguments. We
        # ignore them for the moment. It may be that there are two
        # methods, one providing access to individual values, and
        # another giving a whole vector of values.
        except:
          pass
        if operator.isNumberType(obj):
          print "obj type = ",objType.__name__
          #print "Attribute name = ", objName, ", value = ", obj
          print tab, objName, " = ", obj
          return
      print "obj type =",objType.__name__
      # Is this a vector?
      isVector = False
      if objType.__name__.find('vector') == 0:
        isVector = True
      elif hasattr(obj,'begin') and hasattr(obj,'end') and hasattr(obj,'size'):
        isVector = True
      # If a vector, loop over elements and print each one out
      if isVector:
        i = 0
        newTab = tab + " "
        for o in obj:
          self.getDataMembers(o, objName + '_' + str(i), newTab)
          i += 1
      # Assume a complex type, loop over attributes and print them out
      else:
        attrNames = dir(obj)
        #print "found attributes:",attrNames
        newTab = tab + " "
        for attrName in attrNames:
          # Select attributes that begin with m_ or get
          if attrName.find("m_") == 0 or attrName.find("get") == 0:
            try:
              attr = getattr(obj, attrName)
            except:
              print "Could not get attr", attrName
            self.getDataMembers(attr, attrName, newTab)
