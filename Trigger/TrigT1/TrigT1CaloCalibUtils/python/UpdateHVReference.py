# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
import sys
import time
import os
import struct 
from array import *

from PyCool import cool, coral
from optparse import OptionParser

class HVRefUpdator:

     def __init__(self):
       
         self.reference_HV = {}
         self.reference_AffectedCells = {}
         self.forced_HV    = {}
         self.NLayers      = {}
         self.LayerName    = {}
         self.NCells       = {}
         self.UNIX2COOL = 1000000000

     def read_forced_list(self,forced_file_name):
         print " Loading forced file", forced_file_name

         new_hv = []
         myfile = open(forced_file_name,'r')
         for line in myfile.readlines():
           line.rstrip()
           line.lstrip()
           line_cont  = line.split(' ')
           line_cont  = [iii for iii in line_cont if not iii == ''] 
           rec_chan   = line_cont[0]

           new_hv      = [float(line_cont[iii]) for iii in range(2,6)]
         
           self.forced_HV[rec_chan]=new_hv
           print "updating channel ", rec_chan, " to HV  ", new_hv 

         return

     def read_HV_reference(self,ref_file_name):
         print " Loading reference HV file", ref_file_name
      # get database service and open database
         dbSvc = cool.DatabaseSvcFactory.databaseService()

         dbString='sqlite://;schema='+ref_file_name+';dbname=L1CALO'
         try:
           db = dbSvc.openDatabase(dbString, False)        
         except Exception, e:
           print 'Error: Problem opening database', e
           sys.exit(1)

         folder_name = '/TRIGGER/L1Calo/V1/Results/HVCorrections'
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
           HV_corrections=4*[0]
           Affected_cells = 4*[0]

           CoolId = hex(int(row.channelId()))
           payload = row.payload()

           HV_corrections[0]  = payload['LayerMean1']
           HV_corrections[1]  = payload['LayerMean2']
           HV_corrections[2]  = payload['LayerMean3']
           HV_corrections[3]  = payload['LayerMean4']

           Affected_cells[0]  = struct.unpack('B',payload['AffectedCells1'])[0]
           Affected_cells[1]  = struct.unpack('B',payload['AffectedCells2'])[0]
           Affected_cells[2]  = struct.unpack('B',payload['AffectedCells3'])[0]
           Affected_cells[3]  = struct.unpack('B',payload['AffectedCells4'])[0]
     
           self.reference_AffectedCells[CoolId] =  Affected_cells
           self.reference_HV[CoolId]            =  HV_corrections
            
#         for iii in self.reference_AffectedCells.keys():
# 	   print iii,"  ",self.reference_AffectedCells[iii] 

#         for iii in self.reference_HV.keys():
# 	   print iii,"  ",self.reference_HV[iii] 
      
      # close database
         db.closeDatabase()

         return 0


     def read_HV_geometry(self,ref_file_name):
         print " Reading HV geometry from file", ref_file_name
      # get database service and open database
         dbSvc = cool.DatabaseSvcFactory.databaseService()

         dbString='sqlite://;schema='+ref_file_name+';dbname=L1CALO'
         try:
           db = dbSvc.openDatabase(dbString, False)        
         except Exception, e:
           print 'Error: Problem opening database', e
           sys.exit(1)

         folder_name = '/TRIGGER/L1Calo/V1/Results/RxLayers'
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
           HV_layer_names=4*[0]
           HV_Ncells = 4*[0]

           CoolId = hex(int(row.channelId()))
           payload = row.payload()

           self.NLayers[CoolId]  = struct.unpack('B',payload['NLayers'])[0]

           HV_layer_names[0]  = struct.unpack('B',payload['Name1'])[0]
           HV_layer_names[1]  = struct.unpack('B',payload['Name2'])[0]
           HV_layer_names[2]  = struct.unpack('B',payload['Name3'])[0]
           HV_layer_names[3]  = struct.unpack('B',payload['Name4'])[0]
 
           HV_Ncells[0]  = struct.unpack('B',payload['NCells1'])[0]
           HV_Ncells[1]  = struct.unpack('B',payload['NCells2'])[0]
           HV_Ncells[2]  = struct.unpack('B',payload['NCells3'])[0]
           HV_Ncells[3]  = struct.unpack('B',payload['NCells4'])[0]
     
           self.LayerName[CoolId] =  HV_layer_names
           self.NCells[CoolId]    =  HV_Ncells
            

#         for iii in self.NLayers.keys():
# 	   print iii," NLayers: ",self.NLayers[iii], " Names: ",self.LayerName[iii], "  NCells: ",self.NCells[iii]


      # close database
         db.closeDatabase()

         return 0



     def update_HV(self):

         print " Updating internal HV list"
         
         for iii in self.forced_HV.keys():

           isNominal = false
           if (self.forced_HV[iii][0] == 0 or  self.forced_HV[iii][0] == 1.) and \
              (self.forced_HV[iii][1] == 0 or  self.forced_HV[iii][1] == 1.) and \
              (self.forced_HV[iii][2] == 0 or  self.forced_HV[iii][2] == 1.) and \
              (self.forced_HV[iii][3] == 0 or  self.forced_HV[iii][3] == 1.) :

             isNominal = true
 
           if isNominal:                                # if the channel is on nominal HV, doesn't go to the file
             del self.reference_HV[iii]
             del self.reference_AffectedCells[iii]
           else:                                        # 
             self.reference_HV[iii] = self.forced_HV[iii]
             self.reference_AffectedCells[iii] = [99,99,99,99]    # don't know what to put there, hope this is OK
           

#         print "***************************************************************"
#         print " Now changed HV dictionary"
#         print "***************************************************************"
#         for iii in self.reference_HV.keys():
# 	   print iii,"  ",self.reference_HV[iii] 

         return 

     def write_geometry(self,new_file_name):
         print " Writing geometry to file " , new_file_name

         dbSvc = cool.DatabaseSvcFactory.databaseService()
         connectString = 'sqlite://;schema='+new_file_name+';dbname=L1CALO'


         print 'Writing into database file',new_file_name
         db = dbSvc.openDatabase(connectString, False)

         spec = cool.RecordSpecification()
         spec.extend('NLayers',cool.StorageType.UChar)

         spec.extend( 'Name1', cool.StorageType.UChar)
         spec.extend( 'Name2', cool.StorageType.UChar)
         spec.extend( 'Name3', cool.StorageType.UChar)
         spec.extend( 'Name4', cool.StorageType.UChar)

         spec.extend('NCells1',cool.StorageType.UChar)
         spec.extend('NCells2',cool.StorageType.UChar)
         spec.extend('NCells3',cool.StorageType.UChar)
         spec.extend('NCells4',cool.StorageType.UChar)

         now = int(time.time())

         since = now*self.UNIX2COOL
         until = cool.ValidityKeyMax

         folder_description = "<timeStamp>time</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\"/></addrHeader><typeName>CondAttrListCollection</typeName>"
         f = db.createFolder( "/TRIGGER/L1Calo/V1/Results/RxLayers" , spec,folder_description)

         for i in self.NLayers.keys():

           data = cool.Record( spec )

           data['NLayers']    = struct.pack('B',self.NLayers[i])
           data['Name1']      = struct.pack('B',self.LayerName[i][0])
           data['Name2']      = struct.pack('B',self.LayerName[i][1])
           data['Name3']      = struct.pack('B',self.LayerName[i][2])
           data['Name4']      = struct.pack('B',self.LayerName[i][3])

           data['NCells1'] = struct.pack('B',self.NCells[i][0])
           data['NCells2'] = struct.pack('B',self.NCells[i][1])
           data['NCells3'] = struct.pack('B',self.NCells[i][2])
           data['NCells4'] = struct.pack('B',self.NCells[i][3])

           f.storeObject(since,until, data, int(i,16) )

         db.closeDatabase()

         return


     def write_new_file(self,new_file_name):

         print " Writing output to file " , new_file_name

         dbSvc = cool.DatabaseSvcFactory.databaseService()
         connectString = 'sqlite://;schema='+new_file_name+';dbname=L1CALO'

         print 'recreating database file',new_file_name
         dbSvc.dropDatabase( connectString )
         db = dbSvc.createDatabase( connectString )

         spec = cool.RecordSpecification()
         spec.extend('RxMean',cool.StorageType.Float)

         spec.extend( 'AffectedCells1', cool.StorageType.UChar)
         spec.extend( 'AffectedCells2', cool.StorageType.UChar)
         spec.extend( 'AffectedCells3', cool.StorageType.UChar)
         spec.extend( 'AffectedCells4', cool.StorageType.UChar)

         spec.extend('LayerMean1',cool.StorageType.Float)
         spec.extend('LayerMean2',cool.StorageType.Float)
         spec.extend('LayerMean3',cool.StorageType.Float)
         spec.extend('LayerMean4',cool.StorageType.Float)

         now = int(time.time())

         since = now*self.UNIX2COOL
         until = cool.ValidityKeyMax
         db.createFolderSet('/TRIGGER')
         db.createFolderSet('/TRIGGER/L1Calo')
         db.createFolderSet('/TRIGGER/L1Calo/V1')
         db.createFolderSet('/TRIGGER/L1Calo/V1/Results')
#         db.createFolderSet('/TRIGGER/L1Calo/V1/Results/HVCorrections')

         folder_description = "<timeStamp>time</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\"/></addrHeader><typeName>CondAttrListCollection</typeName>"
         f = db.createFolder( "/TRIGGER/L1Calo/V1/Results/HVCorrections" , spec,folder_description)
#  f = db.createFolder( folder_name , spec, True )

         for i in self.reference_HV.keys():

           data = cool.Record( spec )

           MeanHV=0.
           TotalNrCells=0

           for ilayer in range(4):
             if self.NCells[i][ilayer]>0:
               MeanHV += self.reference_HV[i][ilayer]*self.NCells[i][ilayer]
               TotalNrCells += self.NCells[i][ilayer]

           MeanHV /= TotalNrCells

#           data['RxMean']     =(self.reference_HV[i][0]+self.reference_HV[i][1]+self.reference_HV[i][2]+self.reference_HV[i][3])/4. 
           data['RxMean'] = MeanHV

           data['LayerMean1'] = self.reference_HV[i][0]
           data['LayerMean2'] = self.reference_HV[i][1]
           data['LayerMean3'] = self.reference_HV[i][2]
           data['LayerMean4'] = self.reference_HV[i][3]

           data['AffectedCells1'] = struct.pack('B',self.reference_AffectedCells[i][0])
           data['AffectedCells2'] = struct.pack('B',self.reference_AffectedCells[i][1])
           data['AffectedCells3'] = struct.pack('B',self.reference_AffectedCells[i][2])
           data['AffectedCells4'] = struct.pack('B',self.reference_AffectedCells[i][3])

           f.storeObject(since,until, data, int(i,16) )

         db.closeDatabase()

         return

if __name__ == "__main__":

  print "Starting UpdateHVReference"

  parser = OptionParser()
 
  parser.add_option("-r","--ReferenceFile",action="store",type="string",dest="reference_file",help="Name of input file") 
  parser.add_option("-f","--ForcedChannels",action="store",type="string",dest="forced_file",help="Name of input file")
  parser.add_option("-o","--OutputFile"    ,action="store",type="string",dest="output_file",help="Name of input file")

  (options, args) = parser.parse_args()


  HVUpdatingTool = HVRefUpdator()

  if options.forced_file:
    HVUpdatingTool.read_forced_list(options.forced_file)
  else:
    print "No forced channel list given, run python UpdateHVReference.py -h for more information"
    sys.exit(1)


  if options.reference_file:
    HVUpdatingTool.read_HV_reference(options.reference_file)
    HVUpdatingTool.read_HV_geometry(options.reference_file)
  else:
    print "No HV reference file given, run python UpdateHVReference.py -h for more information"
    sys.exit(1)

  HVUpdatingTool.update_HV()

  if options.output_file:
    print "Writing output into file ", options.output_file
    HVUpdatingTool.write_new_file(options.output_file)
    HVUpdatingTool.write_geometry(options.output_file)
  else:
    print "Writing output into file new_hv_ref.sqlite"   
    HVUpdatingTool.write_new_file("new_hv_ref.sqlite")
    HVUpdatingTool.write_geometry("new_hv_ref.sqlite")

  print "Done!"
