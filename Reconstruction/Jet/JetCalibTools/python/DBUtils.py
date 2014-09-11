# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from string import *
from AthenaCommon.Logging import logging
_logger = logging.getLogger('JetCalib::JetCalibDBUtils')

configured_folders = {}

existing_folders = []

known_finders = ['AntiKt', 'Kt', 'FastJet', 'Cone', 'SISCone', 'CMSCone']

known_inputs =  ['Tower', 'TowerGhost', 'Topo', 'EMTopo', 'LCTopo', 'TopoTower', 'Truth', 'Track']

known_tooltypes = ['OffsetCorrection', 'EtaMassEnergyCorr', 'JetPropertyInvertResponseTool', 'JetCellCalibTool', 'JetH1StateCryoCorr', 'MCNumInversionResponse', 'ConstituentsBasedJetCalibDBTool', 'JetCaloOrigin', 'JetSamplingCalibrator', 'TrackBasedResponseCorrection', 'TrackAngularCorrection', 'H1WeightToolCSC12Generic']

def getConstantsName(tool, finder, mainParam, input, isMC=False, comb=False):
   """ Heart of the scheme: mapping the three ingredients into unique name, which is then used to
       compute COOL channel """

   if finder not in known_finders:
      _logger.error(' finder '+finder+' not known, doing nothing !!!')
      return False

   if input.find("_EM") != -1:
      sinput = input[0:input.find("_EM")]
   else:   
      sinput = input

   if sinput not in known_inputs:
      _logger.error(' input '+input+' not known, doing nothing !!!')
      return False

   if (not 'float' in str(type(mainParam))) and (not 'int' in str(type(mainParam))):
      _logger.error(' mainParam '+mainParam+' has wrong type, doing nothing !!!')
      return False

   from JetRec.JetAlgConfiguration import editParm               
   iParam = editParm(mainParam)               

   if isMC:
      mcsuff='_mc'
   else:   
      mcsuff=''

   if comb:
      _logger.debug(' Create name for tool '+tool.name()+' with finder '+finder+' with mainParam '+iParam+
                   ' for input '+input)
      return tool.name()+"_"+finder+"_"+iParam+"_"+input+mcsuff
   else:   
      _logger.debug(' Create name  with finder '+finder+' with mainParam '+iParam+
                   ' for input '+input)
      return finder+"_"+iParam+"_"+input+mcsuff

def getNameFromJetname(jetname, isMC=False, getList=False):
   """ Giving the string of JEt name, extract the finder, mainParam and input out of it"""

   sfind=""
   sparam=-1
   sinput=""
   scalib=""
   for finder in known_finders:
      if finder in jetname:
         # special protection Kt<->AntiKt
         if finder == "Kt" and "AntiKt" in jetname: continue
         # we have found th known finder, remember it
         sfind=finder
         break

   for input in known_inputs:
      if input in jetname:
         # special protection Topo<->LCTopo
         if input == "Topo" and "LCTopo" in jetname: continue
         # special protection Topo<->EMTopo
         if input == "Topo" and "EMTopo" in jetname: continue
         # special protection Topo<->TopoTower
         if input == "Topo" and "TopoTower" in jetname: continue
         if input == "Tower" and "TopoTower" in jetname: continue
         if input == "Tower" and "TowerGhost" in jetname: continue
         # now we have known input
         sinput=input
         # do we have _EM in jetname ?
         if "_EM" in jetname:
            if not getList:
               sinput += "_EM"
            else:   
               scalib = "_EM"
         break

   for i in range(20,0,-1):
      if str(i) in jetname:
         # really we found a size ?
         if ("mc"+str(i) in jetname) or ("MC"+str(i) in jetname):
            #no, this is mc10... construction
            continue
          
         # yes, this is a size   
         sparam=i
         break   

   if sfind == "" or sparam == -1 or sinput == "":
      print "Could not decode jetname: ",jetname,", returning empty name !!!"
      return ""
   
   if getList:
      return [sfind,sparam,sinput,scalib]
   else:   
      return getConstantsName(0,sfind,sparam,sinput,isMC)   

def getFolder(tool):
   """Returning the full folder string, basically Base+tooltype"""

   if not 'JetCalibTools' in str(type(tool)):
      _logger.error(' configFolder tool is of wrong type, doing nothing !!!')
      return False

   if not tool.getType() in known_tooltypes:
      _logger.error(' tool '+tool.getFullName()+' not known, doing nothing !!!')

   from JetRec.JetRecFlags import jetFlags
   JetDBFolderBase=jetFlags.JetCalibDBFolderBase()

   return JetDBFolderBase+"/"+tool.getType()   

def getFolderTag(tool):
   """ Return the tag folder, basically strip the / from folder"""
   
   folder=getFolder(tool)
   return join(split(folder, '/'),'')
                         

def configFolder(tool, create=False):
   """Adding the folder to configured_folders, computing the proper cool channel number.
      checking if such folder exists, create only if last parameter is setted True"""

   if not 'JetCalibTools' in str(type(tool)):
      _logger.error(' configFolder tool is of wrong type, doing nothing !!!')
      return False

   if not tool.getType() in known_tooltypes:
      _logger.error(' tool '+tool.getFullName()+' not known, doing nothing !!!')

   _logger.debug(' configuring DB folder for tool '+tool.name())

   newfold=getFolder(tool)
   from IOVDbSvc.CondDB import conddb
   from JetRec.JetRecFlags import jetFlags
   JetDB=jetFlags.JetCalibDB()
   #if conddb.isMC and 'OFLP' not in JetDB:
   #   JetDB='COOLONL_CALO/OFLP200'
   JetFolderSuffix=jetFlags.CalibrationDBSuffix()

   if not conddb.folderRequested(newfold):
      if JetFolderSuffix == "":
         if 'sqlite' in JetDB or 'oracle' in JetDB:
            conddb.addFolder("","<dbConnection>"+JetDB+"</dbConnection>"+newfold)
         else:                                  
            conddb.addFolder(JetDB,newfold)
         print "Adding folder: "+JetDB+", "+newfold
      else:
         if 'sqlite' in JetDB or 'oracle' in JetDB:
            conddb.addFolder("<dbConnection>"+JetDB+"</dbConnection>"+newfold+"<tag>"+getFolderTag(tool)+JetFolderSuffix+"</tag>")
         else:                  
            conddb.addFolder(JetDB,newfold+"<tag>"+getFolderTag(tool)+JetFolderSuffix+"</tag>")
         print "Adding folder: "+JetDB+", "+newfold+"<tag>"+getFolderTag(tool)+JetFolderSuffix+"</tag>"

   return True

#This is to check, which channels we have in DB
def GetFolderChanList(folder,tag,isMC):
    from PyCool import cool
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    #db= dbSvc.openDatabase("oracle://ATLAS_COOLPROD;schema=COOLONL_CALO;dbname=COMP200")
    #db= dbSvc.openDatabase("oracle://ATONR_COOL;schema=ATLAS_COOLONL_CALO;dbname=COMP200")
    from CoolConvUtilities.AtlCoolLib import indirectOpen 
    if isMC:
       db = indirectOpen("COOLONL_CALO/OFLP200",readOnly=True,oracle=False)
    else:   
       db = indirectOpen("COOLONL_CALO/COMP200",readOnly=True,oracle=True)

    nodelist=db.listAllNodes()

    folderchanlist=list()
    for node in nodelist:
        if node != folder: continue
        if db.existsFolder(node):
            f=db.getFolder(node)
            folderchanlist.append(node)
            #try:
            #    t=f.resolveTag(tag)
            #except:
            #    return folderchanlist
            #    pass
            chanlist=f.listChannels()
            for chan in chanlist:
               try:
                  t=f.channelName(chan)
               except:
                  t=''
                  pass                  
               folderchanlist.append(t)

    db.closeDatabase()
    return folderchanlist 
