#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

import ROOT
import sys
import time
import os

from PyCool import cool, coral
from optparse import OptionParser


class L1CaloGeometryConvertor:

     def __init__(self):
#          input = open('/afs/cern.ch/user/l/l1ccalib/jb/COOLIdDump.txt')
          input = open('COOLIdDump.txt')
          self.list_of_channels_em={}
          self.list_of_channels_had={}

          for line in input.readlines():
               parts = line.split(' ')
               emCool = parts[4].rstrip()
               hadCool = parts[5].rstrip()
               self.list_of_channels_em[(parts[0],parts[1])]  = '0x'+emCool
               self.list_of_channels_had[(parts[0],parts[1])] = '0x'+hadCool
     
          input.close()
	  
     def LoadReceiverPPMMap(self):
     
         self.receiver_to_ppm_map={}
         self.UNIX2COOL = 1000000000
	 
         # get database service and open database
         dbSvc = cool.DatabaseSvcFactory.databaseService()

         dbString = 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2'
         try:
           db = dbSvc.openDatabase(dbString, False)        
         except Exception, e:
           print 'Error: Problem opening database', e
           sys.exit(1)

         folder_name = "/TRIGGER/Receivers/RxPpmIdMap"
         folder=db.getFolder(folder_name)
         ch = folder.listChannels()
       
         startUtime = int(time.time())
         endUtime = int(time.time())
         startValKey = startUtime * self.UNIX2COOL
         endValKey = endUtime * self.UNIX2COOL
         chsel = cool.ChannelSelection(0,sys.maxint)

         try:
           itr=folder.browseObjects(startValKey, endValKey, chsel)
         except Exception, e:
           print e
           sys.exit(1)

         for row in itr:
           ReceiverId = hex(int(row.channelId()))
           payload = row.payload()
           PPMId = hex(int(payload['ppmid']))
           self.receiver_to_ppm_map[ReceiverId]= PPMId
  
#         print self.receiver_to_ppm_map
         # close database
         db.closeDatabase()


     def getPPMfromReceiver(self,ReceiverId):
     
       if self.receiver_to_ppm_map.has_key(ReceiverId):
         return self.receiver_to_ppm_map[ReceiverId]
       else:
         return ''	 

     def getReceiverfromPPM(self,PPMId,strategy_string=None):
        
       ReceiverChannels = [item[0] for item in self.receiver_to_ppm_map.items() if item[1]==PPMId]       

       if strategy_string == None:
         print " Warning! in getReceiverfromPPM no runtype give, using default!"
         return ReceiverChannels[0]

       if self.isPPMFCAL(PPMId) and self.isCoolHad(PPMId):      # pick correct FCAL23 channel

         if strategy_string == "GainOneOvEmbFcalHighEta":
           for channel in ReceiverChannels:
             if self.getFCAL23RecEta(channel) == 'HighEta':
                return channel
         if strategy_string == "GainOneOvEmecFcalLowEta":
           for channel in ReceiverChannels:
             if self.getFCAL23RecEta(channel) == 'LowEta':
                return channel

       elif self.isPPMOverlap(PPMId):

         if strategy_string == "GainOneOvEmbFcalHighEta":
           for channel in ReceiverChannels:
             if self.getOverlapLayer(channel) == 'EMB':
                return channel
         if strategy_string == "GainOneOvEmecFcalLowEta":
           for channel in ReceiverChannels:
             if self.getOverlapLayer(channel) == 'EMEC':
                return channel

       else:
         return ReceiverChannels[0]
	 

     def getCoolEm(self,i_eta,i_phi):
          if self.list_of_channels_em.has_key((str(i_eta),str(i_phi))) == True:
              cool = self.list_of_channels_em[(str(i_eta),str(i_phi))]
              cool.rstrip()
              cool.lstrip()
              return (cool)
          else:
              return ('')         
     
     
     def getCoolHad(self,i_eta,i_phi):
          if self.list_of_channels_had.has_key((str(i_eta),str(i_phi))) == True:
              cool = self.list_of_channels_had[(str(i_eta),str(i_phi))]
              cool.rstrip()
              cool.lstrip()
              return (cool)
          else:
              return ('')         
     
     def isCoolEm(self,CoolId):
          return (CoolId in self.list_of_channels_em.values())
	  
     def isCoolHad(self,CoolId):
          return (CoolId in self.list_of_channels_had.values())
	  	
     def getEtaBin(self,CoolId):
          if self.isCoolEm(CoolId):
            channel = [item[0] for item in self.list_of_channels_em.items() if item[1]==CoolId]
            return int(channel[0][0])
          elif self.isCoolHad(CoolId):
            channel = [item[0] for item in self.list_of_channels_had.items() if item[1]==CoolId]
            return int(channel[0][0])
          else:
            return -1
	  
     def getPhiBin(self,CoolId):
          if self.isCoolEm(CoolId):
            channel = [item[0] for item in self.list_of_channels_em.items() if item[1]==CoolId]
            return int(channel[0][1])
          elif self.isCoolHad(CoolId):
            channel = [item[0] for item in self.list_of_channels_had.items() if item[1]==CoolId]
            return int(channel[0][1])
          else:
            return -1

     def getMissingReceiverChannels(self, channel_list):

       missing_channels= [channel for channel in self.receiver_to_ppm_map.keys() if channel not in channel_list]
       return missing_channels


     def getReceiverCMCP(self,ReceiverId):
     
       recI=int(ReceiverId,16)

       crate = recI/1024
       recI = recI - crate*1024

       module = recI/64
       recI = recI - module*64

       conn = recI/16
       recI = recI - conn*16

       pair = recI

       return [crate,module,conn,pair]
     
     def isPPMFCAL(self,CoolId):
     
       eta_bin = self.getEtaBin(CoolId)  

       if eta_bin >= 32 or eta_bin <= -36:
         return True
       else:
         return False


     def isPPMOverlap(self,CoolId):

       eta_bin = self.getEtaBin(CoolId)  
       if self.isCoolEm(CoolId) == True and (eta_bin == 14 or eta_bin == -15):
         return True
       else:
         return False

     def getOverlapLayer(self,RecCoolId):

       ppm_id = self.getPPMfromReceiver(RecCoolId)

       if not self.isPPMOverlap(ppm_id):
         return None

       cabling = self.getReceiverCMCP(RecCoolId)
       if cabling[0] < 2:                 # unconnected channel has barrel crate nr.
         return 'Unconnected'
       elif cabling[2] == 0:
         return 'EMEC'
       elif cabling[2] == 2:
         return 'EMB'
       else:
         print "Error in GetOverlapLayer, can't determine layer!"
         return None        

     def getFCAL23RecEta(self,RecCoolId):

       ppm_id = self.getPPMfromReceiver(RecCoolId)

       if (not self.isPPMFCAL(ppm_id)) or (not self.isCoolHad(ppm_id)):
         return None
       eta_bin = self.getEtaBin(ppm_id)  

       RecCoolInt = int(RecCoolId,16)
       if RecCoolInt%2 == 1:
         isRecOdd = True
       else:
         isRecOdd = False

       if eta_bin>0:
         if isRecOdd:
           return 'LowEta'
         else:
           return 'HighEta'
       else:
         if isRecOdd:
           return 'HighEta'
         else:
           return 'LowEta' 
         
   
def WriteSqlite(name,input_dict):
  
  UNIX2COOL = 1000000000
  
  dbSvc = cool.DatabaseSvcFactory.databaseService()
  connectString = 'sqlite://;schema='+name+';dbname=L1CALO'
# folder_name="/TRIGGER/Receivers/Factors/CalibGains"


  print '\nrecreating database file:',name
  dbSvc.dropDatabase( connectString )
  db = dbSvc.createDatabase( connectString )

  spec = cool.RecordSpecification()
  spec.extend("factor", cool.StorageType.Float)
  spec.extend("status", cool.StorageType.UInt32 )
  folderSpec = cool.FolderSpecification(cool.FolderVersioning.SINGLE_VERSION, spec)

  now = int(time.time())

  since = now*UNIX2COOL
# since = 0
#  until = sys.maxint
  until = cool.ValidityKeyMax
  db.createFolderSet('/TRIGGER')
  db.createFolderSet('/TRIGGER/Receivers')
  db.createFolderSet('/TRIGGER/Receivers/Factors')

  folder_description = '<timeStamp>time</timeStamp><addrHeader><address_header service_type="71" clid="1238547719"/></addrHeader><typeName>CondAttrListCollection</typeName>'
  f = db.createFolder( "/TRIGGER/Receivers/Factors/CalibGains", folderSpec, folder_description)

  print " Now creating sqlite file for ", len(input_dict.keys()), " channels"
  for i in input_dict.keys():
    data = cool.Record( spec )
    data['factor'] = input_dict[i][0]
    data['status'] = input_dict[i][1]
    f.storeObject(since,until, data, int(i,16) )

  db.closeDatabase()

class GainsFromSqlite:

     def __init__(self,name,geometryMapper):
       
#       self.cut_gain_low    = 0.5
#       self.cut_gain_high   = 1.6
#       self.cut_offset_low  = -10.
#       self.cut_offset_high = 10.

       self.cut_gain_low    = 0.                    # all gains that are not completly crazy...
       self.cut_gain_high   = 2.3
       self.cut_offset_low  = -100.
       self.cut_offset_high = 100.

       self.geometry_convertor = geometryMapper

       self.run_nr = None         #will come later
       self.strategy = None     #later

       self.measured_gains={}  
       self.measured_chi2={}  
       self.measured_offset={}
       self.measured_error_code={}
  
       self.UNIX2COOL = 1000000000

       # get database service and open database
       dbSvc = cool.DatabaseSvcFactory.databaseService()

       dbString='sqlite://;schema='+name+';dbname=L1CALO'
       try:
         db = dbSvc.openDatabase(dbString, False)        
       except Exception, e:
         print 'Error: Problem opening database', e
         sys.exit(1)

       folder_name = '/TRIGGER/L1Calo/V1/Results/EnergyScanResults'
       folder=db.getFolder(folder_name)
       ch = folder.listChannels()
       
       startUtime = int(time.time())
       endUtime = int(time.time())
       startValKey = startUtime * self.UNIX2COOL
       endValKey = endUtime * self.UNIX2COOL
       chsel = cool.ChannelSelection(0,sys.maxint)

       try:
         itr=folder.browseObjects(startValKey, endValKey, chsel)
       except Exception, e:
         print e
         sys.exit(1)

       for row in itr:
         CoolId = hex(int(row.channelId()))
         payload = row.payload()
         self.measured_gains[CoolId]      = payload['Slope']
         self.measured_chi2[CoolId]       = payload['Chi2']
         self.measured_offset[CoolId]     = payload['Offset']
         self.measured_error_code[CoolId] = payload['ErrorCode']
  
#       print self.measured_gains

       folder_gen_name = '/TRIGGER/L1Calo/V1/Results/EnergyScanRunInfo'
       folder_gen=db.getFolder(folder_gen_name)

       try:
         itr=folder_gen.browseObjects(startValKey, endValKey, chsel)
       except Exception, e:
         print e
         sys.exit(1)

       for row in itr:
          payload = row.payload()
          self.run_nr   = payload['RunNumber']
          self.strategy = payload['GainStrategy']


       # close database
       db.closeDatabase()

     def getGoodGains(self):

       good_gains={}

       for i in self.measured_gains.keys():

         if ((self.measured_gains[i] > self.cut_gain_low and self.measured_gains[i] < self.cut_gain_high) and
            (self.measured_offset[i] > self.cut_offset_low and self.measured_offset[i] < self.cut_offset_high) and
#            not (self.geometry_convertor.isCoolHad(i) and self.geometry_convertor.isPPMFCAL(i)) ):
            not (self.geometry_convertor.isPPMFCAL(i)) ):

           good_gains[i]=[self.measured_gains[i],self.measured_error_code[i]]      

         else:
           print "GainsFromSqlite::getGoodGains have rejected channel ", i

       return good_gains      # these are gains as a function of PPM Cool


class GainsFromOracle:
     def __init__(self):

       self.default_gains={}  
       self.UNIX2COOL = 1000000000

       # get database service and open database
       dbSvc = cool.DatabaseSvcFactory.databaseService()

       dbString = 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2'
       try:
         db = dbSvc.openDatabase(dbString, False)        
       except Exception, e:
         print 'Error: Problem opening database', e
         sys.exit(1)

       folder_name = "/TRIGGER/Receivers/Factors/CalibGains"
       folder=db.getFolder(folder_name)
       ch = folder.listChannels()
       
       startUtime = int(time.time())
       endUtime = int(time.time())
       startValKey = startUtime * self.UNIX2COOL
       endValKey = endUtime * self.UNIX2COOL
       chsel = cool.ChannelSelection(0,sys.maxint)

       try:
         itr=folder.browseObjects(startValKey, endValKey, chsel)
       except Exception, e:
         print e
         sys.exit(1)

       for row in itr:
         ReceiverId = hex(int(row.channelId()))
         payload = row.payload()
         gain = payload['factor']
         self.default_gains[ReceiverId]=gain

     def getGoodGains(self):
       return self.default_gains


def merge_gains(gains1,gains2,gains3,reference_gains,forced_list,geometry_map,writeAllChannels):

     output_gains={}

     n_files_Tile        = 0
     n_files_LowEta_EMEC = 0
     n_files_HighEta_EMB = 0

#loop over gains3, fill in

     if not gains3 == None:     
       good_gains=gains3.getGoodGains()
       print " Using run ", gains3.run_nr, " run strategy= ", gains3.strategy       

       if gains3.strategy == "GainOne":
         n_files_Tile = n_files_Tile + 1
       if gains3.strategy == "GainOneOvEmecFcalLowEta":
         n_files_LowEta_EMEC =  n_files_LowEta_EMEC + 1
       if gains3.strategy == "GainOneOvEmbFcalHighEta":
         n_files_HighEta_EMB =  n_files_HighEta_EMB + 1


       for ppm_channel in good_gains.keys():
         gain       = good_gains[ppm_channel][0]
         error_code = good_gains[ppm_channel][1]
         rec_chan   = geometry_map.getReceiverfromPPM(ppm_channel,gains3.strategy)
         output_gains[rec_chan]=[gain,error_code]
     else:
       print "Ignoring File 3, probably not specified"      
       
#loop over gains2, fill in

     if not gains2 == None:
       good_gains=gains2.getGoodGains()
       print " Using run ", gains2.run_nr, " run strategy= ", gains2.strategy       

       if gains2.strategy == "GainOne":
         n_files_Tile = n_files_Tile + 1
       if gains2.strategy == "GainOneOvEmecFcalLowEta":
         n_files_LowEta_EMEC =  n_files_LowEta_EMEC + 1
       if gains2.strategy == "GainOneOvEmbFcalHighEta":
         n_files_HighEta_EMB =  n_files_HighEta_EMB + 1

       for ppm_channel in good_gains.keys():
         gain       = good_gains[ppm_channel][0]
         error_code = good_gains[ppm_channel][1]
         rec_chan   = geometry_map.getReceiverfromPPM(ppm_channel,gains2.strategy)
         output_gains[rec_chan]=[gain,error_code]
     else:
       print "Ignoring File 2, probably not specified"      
     
       
#loop over gains 1, fill in

     if not gains1 == None:
       good_gains=gains1.getGoodGains()
       print " Using run ", gains1.run_nr, " run strategy= ", gains1.strategy       

       if gains1.strategy == "GainOne":
         n_files_Tile = n_files_Tile + 1
       if gains1.strategy == "GainOneOvEmecFcalLowEta":
         n_files_LowEta_EMEC =  n_files_LowEta_EMEC + 1
       if gains1.strategy == "GainOneOvEmbFcalHighEta":
         n_files_HighEta_EMB =  n_files_HighEta_EMB + 1

       for ppm_channel in good_gains.keys():
         gain       = good_gains[ppm_channel][0]
         error_code = good_gains[ppm_channel][1]
         rec_chan   = geometry_map.getReceiverfromPPM(ppm_channel,gains1.strategy)
         output_gains[rec_chan]=[gain,error_code]
     else:
       print "Ignoring File 1, probably not specified"      
     

#read in forced list, overwrite

     if not forced_list == None:
       myfile = open(forced_list,'r')
       for line in myfile.readlines():
         line.rstrip()
         line.lstrip()
         line_cont  = line.split(' ')
         line_cont  = [iii for iii in line_cont if not iii == ''] 
         rec_chan   = line_cont[0]
         gain       = float(line_cont[1])
         error_code = 10                    # code for forced channels
         output_gains[rec_chan]=[gain,error_code]
         print "forcing channel ", rec_chan, " to value ", gain 
     else:
       print "Ignoring forced channel list"

#check channels that haven't been found yet, take them from reference

     if writeAllChannels:

       print "Adding gains for missing channels from Oracle"     
       missing_channels = geometry_convertor.getMissingReceiverChannels(output_gains.keys())    
       default_gains = reference_gains.getGoodGains()
       for channel in missing_channels:
         gain = default_gains[channel]
         error_code = 100
         output_gains[channel]=[gain,error_code]

#       print "missing receiver=",channel, "PPM=", geometry_map.getPPMfromReceiver(channel) 

     if (not n_files_Tile == 1) or (not  n_files_LowEta_EMEC == 1) or (not n_files_HighEta_EMB == 1):
       print "WARNING! input files do not allow to calibrate all partitions, using defaults where needed" 

     return output_gains



if __name__ == "__main__":

  print "Starting mergeEnergyRamps"

  parser = OptionParser()
  
  parser.add_option("-1","--FirstFile"     ,action="store",type="string",dest="input_file1",help="Name of input file")
  parser.add_option("-2","--SecondFile"    ,action="store",type="string",dest="input_file2",help="Name of input file")
  parser.add_option("-3","--ThirdFile"     ,action="store",type="string",dest="input_file3",help="Name of input file")
  parser.add_option("-f","--ForcedChannels",action="store",type="string",dest="forced_file",help="Name of input file")
  parser.add_option("-o","--OutputFile"    ,action="store",type="string",dest="output_file",help="Name of input file")
  parser.add_option("-A","--WriteAllChannels",action="store_true",dest="writeAllChannels",help="Writes all channels to .sqlite file")

  (options, args) = parser.parse_args()
  
  geometry_convertor = L1CaloGeometryConvertor()
  geometry_convertor.LoadReceiverPPMMap()

  print "Processing inputs"
  
  if not options.input_file1 == None:
    gains_1 = GainsFromSqlite(options.input_file1,geometry_convertor)
  else:
    gains_1 = None

  if not options.input_file2 == None:
    gains_2 = GainsFromSqlite(options.input_file2,geometry_convertor)
  else:
    gains_2 = None

  if not options.input_file3 == None:
    gains_3 = GainsFromSqlite(options.input_file3,geometry_convertor)
  else:
    gains_3 = None

  print "Reading reference"
  gains_reference = GainsFromOracle()

  print "Merging gains"
  if options.writeAllChannels:
    print "Will write out gains for all channels"
  else:
    print "Will write out gains only for updated channels"   
  
  gains_to_load = merge_gains(gains_1,gains_2,gains_3,gains_reference,options.forced_file,geometry_convertor,options.writeAllChannels)

  if not options.output_file == None:
    output_file_name = options.output_file
  else:
    output_file_name = "MergedGains.sqlite"
 
  print "Writing output file ",output_file_name , " this may take some time ... :-( "    
  WriteSqlite(output_file_name,gains_to_load)

#  print gains_to_load 

#  for i in gains_to_load.keys():
#    print i, "  ", gains_to_load[i]
#    print int(i,16)

  print "Finished!"
