# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from TrigValRegtestLibrary import TrigValRegtestLibrary, TrigValRegtestSet

##########################################################################

def TrigValPrintRegtest( fileObj, HLTalgo, tag ='Default'):
   refFile = TrigValRegtestLibrary[ HLTalgo].getFile(tag)
   if( refFile != "" ):
      try :
         fil = open( fileObj, mode="a")
         if( HLTalgo == "Full" or HLTalgo == "full" ) :
            fil.write("  fullregtest    REGTEST "+refFile+"\n")
         else :   
            fil.write("  regtest  "+HLTalgo+"   REGTEST "+refFile+"\n")
         fil.close()
      except IOError: 
         print "TrigValRegtestFile    ERROR:   Can not open "+refFile+", print no regtest"
  

def TrigValPrintRegtestSet( fileObj, hltsetTag, tag='Default',
                            makeReferenceLink=False, useTmpReference=False):
    try :
       fil = open( fileObj, mode="a")

       post_command = ""
       if ( TrigValRegtestLibrary.has_key( hltsetTag) ) :
         refFile = TrigValRegtestLibrary[hltsetTag ].getFile(tag)
         if( makeReferenceLink ) : post_command += "rm -f /tmp/"+refFile+"; ln -s $PWD/"+refFile+".new /tmp/"+refFile+" ;"
         if( useTmpReference ) : refFile = "/tmp/"+refFile # important to have it after post_command
         
         if( hltsetTag == "Full" or hltsetTag == "full" ) :
            fil.write("  fullregtest    REGTEST "+refFile+"\n")
         else :   
            fil.write("  regtest  "+hltsetTag+"   REGTEST "+refFile+"\n")

       elif ( TrigValRegtestSet.has_key( hltsetTag) ) :
          hltset = TrigValRegtestSet[hltsetTag]
          for hltAlgo in hltset :
            if ( TrigValRegtestLibrary.has_key(hltAlgo ) ) :
               refFile = TrigValRegtestLibrary[ hltAlgo ].getFile(tag)
               if( makeReferenceLink ) : post_command += "rm -f /tmp/"+refFile+"; ln -s $PWD/"+refFile+".new /tmp/"+refFile+" ;"
               if( useTmpReference ) : refFile = "/tmp/"+refFile # important to have it after post_command
               if( refFile != "" ):
                 fil.write("  regtest  "+hltAlgo+"   REGTEST "+refFile+"\n")
            else :
               print "TrigValRegtestFile    ERROR:   Can not find HLTalgo or HLT set "+hltAlgo+" ignore it"
       else :
         print "TrigValRegtestFile    ERROR:  Can not find HLTalgo or HLT set "+hltsetTag+" ignore it"
       fil.close()
       return post_command
    
    except IOError: 
       print "TrigValRegtestFile    ERROR:  Can not open "+refFile+", print no regtest"
       return ""
 
    

#printRegtest( 'f1.txt', 'T2CaloTau_Tau' )
#printRegtest( 'f1.txt', 'T2CaloTauHypo_tau10i', 'Default' )
#printRegtestSet( 'f1.txt', 'Tau' )
