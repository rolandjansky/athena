#!/usr/bin/env pyroot.py 

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys

doEDM=False
#doEDM=True
doxAODonly=False
bs_filename = 'bs-streamerinfos.root'

import ROOT
from ROOT import TClass, TFile
import cppyy

try:
  cppyy.Cintex
  has_cintex = True
  print "Cintex detected, assuming ROOT 5"
except:
  has_cintex = False
  print "Aassuming ROOT 6 or newer"
  print "ROOT 6 NOT SUPPORTED YET!!!   contact Marcin.Nowak"
  sys.exit(-1)

if has_cintex:  
  cppyy.Cintex.Enable()
  #Cintex.SetDebug(7)
  import StreamerInfoGenerator


from TriggerJobOpts.TriggerEDM import EDMDetails

objects = ['ElementLink_p1<unsigned int>',
           'TauRecDetails_p1',
           'ParticleBase_p1',
           'AthenaBarCode_p1',
           'Trk::TrackParticleBase_p1',
           'TauJet_p3',
           'TrigMuonEFInfoContainer_tlp1'
           ]

objects = ['TrigMuonEFInfoContainer_tlp1']
#objects = ['TrigElectron_p2', 'TrigPhoton_p2','TrigEMCluster_p2','DataLink_p1']
#objects = ['TrigEMClusterContainer_tlp1','TrigSpacePointCountsCollection_tlp1','TrigSpacePointCounts_p3']
objects = ['TrigTauClusterContainer_tlp1']
objects = ['P4PtEtaPhiM_p1']
objects = ['TrigSpacePointCountsCollection_tlp1']
objects = ['P4IPtCotThPhiM_p1']
objects = ['TrigMuonEFInfoContainer_tlp1']
objects = ['TauCommonDetails_p1']
objects = ['P4PxPyPzE_p1']
objects = ['TrigT2JetContainer_p1', 'TrigT2Jet_p1', 'TrigEMClusterContainer_tlp1','TrigCaloCluster_p1', 'egammaContainer_p1', 'P4EEtaPhiM_p1']
objects = ['TrigT2MbtsBitsContainer_p1','TrigEMClusterContainer_p1','TrigTauClusterContainer_tlp1','TrigTauCluster_p1','TrigEMCluster_p1','TrigEMClusterContainer_p1', 'TrigTauClusterContainer_p1']
objects = ['TrigMuonEF_p1']
objects = ['TrackSummaryCnv_p1','MuonTrackSummaryCnv_p1','Rec::TrackParticleContainer_tlp1','Trk::TrackParticleBase_p1','Trk::TrackSummary_p1']
objects = ['P4EEtaPhiM_p1']
objects = ['TrigTauCluster_p2']
objects = ['TrigTauClusterContainer_tlp1']
objects = []
objects = ['ElementLinkVector_p1<unsigned int>::ElementRef']
objects += ['ElementLinkVector_p1<std::string>::ElementRef']
objects += ['MuonFeature_p3']
objects = []
objects = ['xAOD::TrigTrackCountsAuxContainer']

objects = [
'xAOD::TrigEMCluster_v1',
'xAOD::TrigEMClusterContainer_v1',
'xAOD::TrigEMClusterAuxContainer_v1',
'xAOD::TrigRingerRings_v1',
'xAOD::TrigRingerRingsContainer_v1',
'xAOD::TrigRingerRingsAuxContainer_v1',
'xAOD::TrigRNNOutput_v1',
'xAOD::TrigRNNOutputContainer_v1',
'xAOD::TrigRNNOutputAuxContainer_v1',
'xAOD::CaloClusterContainer_v1',
'xAOD::CaloClusterAuxContainer_v2',
'xAOD::L2StandAloneMuonContainer_v1',
'xAOD::L2StandAloneMuonAuxContainer_v1',
'xAOD::L2CombinedMuonContainer_v1',
'xAOD::L2CombinedMuonAuxContainer_v1',
'xAOD::L2IsoMuonContainer_v1',
'xAOD::L2IsoMuonAuxContainer_v1',
'xAOD::MuonContainer_v1',
'xAOD::MuonAuxContainer_v2',
'xAOD::TrackParticleContainer_v1',
'xAOD::TrackParticleAuxContainer_v2',
'xAOD::TauJetContainer_v2',
'xAOD::TauJetAuxContainer_v2',
'xAOD::TrackParticleContainer_v1',
'xAOD::TrackParticleAuxContainer_v1',
'xAOD::VertexContainer_v1',
'xAOD::VertexAuxContainer_v1',
'xAOD::TrigCompositeContainer_v1',
'xAOD::TrigCompositeAuxContainer_v1',
'xAOD::MuonRoIContainer_v1',
'xAOD::MuonRoIAuxContainer_v1',
'xAOD::EmTauRoIContainer_v2',
'xAOD::EmTauRoIAuxContainer_v2',
'xAOD::JetRoIContainer_v2',
'xAOD::JetRoIAuxContainer_v2',
'xAOD::JetEtRoI_v1',
'xAOD::JetEtRoIAuxInfo_v1',
'xAOD::EnergySumRoI_v1',
'xAOD::EnergySumRoIAuxInfo_v1',
'xAOD::TriggerTowerContainer_v2',
'xAOD::TriggerTowerAuxContainer_v2',
'xAOD::ElectronContainer_v1',
'xAOD::ElectronAuxContainer_v1',
'xAOD::PhotonContainer_v1',
'xAOD::PhotonAuxContainer_v1',
'xAOD::TrigBphysContainer_v1',
'xAOD::TrigBphysAuxContainer_v1',
'xAOD::TrigT2MbtsBits_v1',
'xAOD::TrigT2MbtsBitsAuxContainer_v1',
'xAOD::TrigSpacePointCountsContainer_v1',
'xAOD::TrigSpacePointCountsAuxContainer_v1',
'xAOD::TrigVertexCountsContainer_v1',
'xAOD::TrigVertexCountsAuxContainer_v1',
'xAOD::TrigTrackCounts_v1',
'xAOD::TrigTrackCountsAuxContainer_v1',
'xAOD::TrigMissingETContainer_v1',
'xAOD::TrigMissingETAuxContainer_v1',
'xAOD::TrigPhotonContainer_v1',
'xAOD::TrigPhotonAuxContainer_v1',
'xAOD::TrigElectronContainer_v1',
'xAOD::TrigElectronAuxContainer_v1',
'xAOD::JetContainer_v1',
'xAOD::JetTrigAuxContainer_v1',
'xAOD::TrigDecision_v1',
'xAOD::TrigDecisionAuxInfo_v1',
'xAOD::TrigConfKeys_v1',
'xAOD::TrigNavigation_v1',
'xAOD::TrigNavigationAuxInfo_v1',
'xAOD::BTaggingContainer_v1',
'xAOD::BTaggingAuxContainer_v1',
'xAOD::BTagVertexContainer_v1',
'xAOD::BTagVertexAuxContainer_v1'
]


from collections import defaultdict
streamerChecksums = defaultdict(set)
print "Reading streamerinfos from", bs_filename
file = TFile(bs_filename, 'UPDATE')
streamer_n = 0
for i in file.GetStreamerInfoList():
    if i.GetName() != 'listOfRules':
        # print i.GetName(), "%x" % i.GetCheckSum()
        streamerChecksums[i.GetName()].add( i.GetCheckSum() )
        streamer_n += 1
print "Read", streamer_n, 'streamers for', len(streamerChecksums), 'types'
print

if doEDM:
  for item in EDMDetails.keys():
    pers = EDMDetails[item]['persistent']
    objects.append(pers)

a = StreamerInfoGenerator.StreamerInfoGenerator()
for pers in objects:
    a.inspect(pers)
    print
fulllist = a.classlist 
print fulllist
print '*******************************'


from CLIDComps.clidGenerator import clidGenerator
cgen = clidGenerator("")

types_new = 0
types_exist = 0
types_bad = 0
fulllist = list(set(fulllist))
for item in fulllist:    
  if doxAODonly and not 'xAOD' in item: continue # current issues seen because of missing xAOD libs not being loaded
  print "Trying to fill item", item, "to root file"
  c_clid = cgen.genClidFromName(item)
  c_typeinfo = cgen.getTidFromClid(c_clid)
  print "CLID", c_clid
  print "TypeInfo", c_typeinfo
  try:
   cls = ROOT.gROOT.GetClass(item)
  except:
   cls = ROOT.gROOT.GetClass(c_typeinfo)      
  print cls

  if cls!=None:
    streamerinfo = cls.GetStreamerInfo()
    if streamerinfo.GetCheckSum() == 0:
      # try to patch missing checksum in DataVectors
      print 'Warning: no checksum in streamerinfo for type: ', cls.GetName()
      print 'Attempting to fix with 0x%x' %  cls.GetCheckSum()
      streamerinfo.SetCheckSum( cls.GetCheckSum() )

    chksum = streamerinfo.GetCheckSum()
    if chksum not in streamerChecksums[cls.GetName()]:
      print 'Writing: %s  streamer size=%d, checksum=0x%x' %(cls.GetName(), streamerinfo.Sizeof(), chksum)
      obj = cls.New()
      file.WriteObjectAny(obj, cls, cls.GetName())
      types_new += 1
    else:
      print 'Skipping', cls.GetName(), 'streamer checksum', chksum, ' - already in the file'
      types_exist += 1
      
  else:
    print 'skipping ', item
    types_bad += 1
    #sys.exit()
  print '----'  

print 'Wrote', types_new, 'types'
print 'Skipped', types_exist, ' existing types'
print 'Errors with', types_bad, ' types'
#print 'experimental'
#rclass =   ROOT.gROOT.GetClass('TrigTauCluster_p2')
#ROOT.gROOT.RemoveClass(rclass)

#cl1 = ROOT.gROOT.GetClass('ElementLinkVector_p1<unsigned int>::ElementRef')
#si = cl1.GetStreamerInfo()
#print 'aa:',cl1.GetName(), si.Sizeof(), si.GetCheckSum()


#  } else {
#    TFile f("aaa.root");
#    f.ls();
#    TIter nextkey(f.GetListOfKeys());
#    TKey *key;
#    while (key = (TKey*)nextkey()) {
#       TrigMuonEFInfoContainer_tlp1 *p = (TrigMuonEFInfoContainer_tlp1*)key->ReadObj();
#       std::cout << p << std::endl;
#       if (p)
# 	std::cout << p->m_MuonEFTrack.size() << std::endl;
#    }
#  }

#  cout << tc4->GetStreamerInfo()->Sizeof() << " " << tc4->GetStreamerInfo()->GetCheckSum() << endl;

# }
