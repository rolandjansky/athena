import sys
sys.argv.append("-b") # for running ROOT in batch! verrrry important!
sys.rtt = True
ThisIsRTT=True
thisIsRTT = True

from CoolDozer import *
class MuonSummaryRTTDozerApp( RTTDozerApp ):         
  def __init__( self, argDict={}):
       super(MuonSummaryRTTDozerApp, self).__init__( argDict )
         
  def schema( self ): # schema function
        myDozerSchema = DozerSchema()  # this one object would be constructed ad returned
        trackCollections=["ConvertedMBoyTracks", "MuGirlRefittedTracks", "ConvertedMuIdCBTracks", "ConvertedStacoTracks", "MooreTracks"] # Order must not change!
        
        # Mustn't have slashes in CoolDDozer channels
        translations={"Scat/Tk":"ScatPerTrk", 
                      "Ch2/dof/T":"Chi2perDOFperTrk",
                      "Trk/Evt":"TrkPerEvt", 
                      "Hole/Tk":"HolesPerTrk", 
                      "TrigEta/T":"TrigEtaPerTrk", 
                      "Tracks":"NumTracks",
                      "Eta/Trk":"EtaPerTrk",
                      "Phi/Trk":"PhiPerTrk", 
                      "Hit/Trk":"HitPerTrk", 
                      "Events":"NumEvents"}
        
        for trackCollName in trackCollections:
          trackCollData = DozerChannel(trackCollName, "Summary data about tracks from "+trackCollName) # here comes DozerChannel
          trackCollData += DozerData(translations["Events"],    "Int") 
          trackCollData += DozerData(translations["Tracks"],    "Int") 
          trackCollData += DozerData(translations["Trk/Evt"],   "Float") 
          trackCollData += DozerData(translations["Hit/Trk"],   "Float") 
          trackCollData += DozerData(translations["Eta/Trk"],   "Float") 
          trackCollData += DozerData(translations["TrigEta/T"], "Float") 
          trackCollData += DozerData(translations["Phi/Trk"],   "Float") 
          trackCollData += DozerData(translations["Scat/Tk"]  , "Float") 
          trackCollData += DozerData(translations["Hole/Tk"],   "Float") 
          trackCollData += DozerData(translations["Ch2/dof/T"], "Float") 
          # mboyTracks += DozerData("my random float data;error", "Float", "unit") # and its error
          # mboyTracks += DozerData("my string data", "String" ) # why not to keep some string as well
          myDozerSchema += trackCollData # adding channel to schema
          
        return myDozerSchema # and at the end returning of newly created schema

  def writer( self ): # overwriting write function, will use specially created self.dozerSchema object (of type DozerSchema of course)
       from MuonRecRTT import ParseTrkSummary

       summaries = ParseTrkSummary.parseSummaryFile( self.runPath+"/trkSummary.txt")

       # Mustn't have slashes in CoolDDozer channels
       translations={"Scat/Tk":"ScatPerTrk", 
                     "Ch2/dof/T":"Chi2perDOFperTrk",
                     "Trk/Evt":"TrkPerEvt", 
                     "Hole/Tk":"HolesPerTrk", 
                     "TrigEta/T":"TrigEtaPerTrk", 
                     "Tracks":"NumTracks",
                     "Eta/Trk":"EtaPerTrk",
                     "Phi/Trk":"PhiPerTrk", 
                     "Hit/Trk":"HitPerTrk", 
                     "Events":"NumEvents"}
       
       for trackCollName in summaries:
         values = summaries[trackCollName]
         print values
         for k,v in values.iteritems():
           key = translations[k]
           if key=="NumEvents" or key=="NumTracks":
             self.dozerSchema[trackCollName][key].setValue( int(v) ) # setting value for  DozerData
           else:
             self.dozerSchema[trackCollName][key].setValue( v ) # setting value for  DozerData
        

## dummy standalone run outside RTT
if __name__ == "__main__":
   print "Dummy test running..."

   ##
   # @class Paths
   # @brief fake RTT Paths
   class Paths:
       def __init__(self):
           self.release    = "rel_0"
           self.branch     = "dev"
           self.runType    = "build"
           self.cmtConfig  = "i686-slc4-gcc34-opt"
           self.topProject = "AtlasProduction"
           self.packageTag = "MuonRecRTT-00-00-00"
   ##
   # @class Desc
   # @brief fake RTT JobDescriptor for testing
   class Desc:
       def __init__(self):
           self.package    = "MuonRecRTT"
           self.trendId    = "Zmumu_MboyAndMoreOnly"
           self.runPath    = "/afs/cern.ch/user/e/emoyse/public/MuonRecRTTdata"
           self.paths      = Paths()
           self.log        = "dummy.log"

   testCfg = { "JobDescriptor"    : Desc(),
               "LastNDays"        : 30,
               "Fit"              : True, 
               "Histos"           : True, 
               "DryRun"           : True,
               "Style"            : "Dozer" }

   rttRandomDozerApp = MuonSummaryRTTDozerApp( testCfg )
   rttRandomDozerApp.run()

