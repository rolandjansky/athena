#!/usr/bin/env pyroot.py 

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import StreamerInfoGenerator

doEDM=False
#doEDM=True
from ROOT import Cintex, TClass, TFile
Cintex.Enable()
#Cintex.SetDebug(7)

from TriggerJobOpts.TriggerEDM import EDMDetails
file = TFile('bs-streamerinfos.root','UPDATE')

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
objects = ['MuonFeature_p3']


if doEDM:
  for item in EDMDetails.keys():
    pers = EDMDetails[item]['persistent']
    objects.append(pers)

a = StreamerInfoGenerator.StreamerInfoGenerator()
for item in objects:
  pers = item
  print pers
  a.inspect(pers)

print '*******************************'
  
fulllist = a.classlist + objects
fulllist = list(set(fulllist))
#for item in a.classlist:  
for item in fulllist:  
  cls = ROOT.gROOT.GetClass(item)
  print cls
  
  if cls!=None:
    streamerinfo = cls.GetStreamerInfo()
    print 'jmasik: ', cls.GetName(), streamerinfo.Sizeof(), streamerinfo.GetCheckSum()
    obj = cls.New()
    file.WriteObjectAny(obj, cls, cls.GetName())
    #aa=cls.GetListOfRealData()
    #for mem in aa:
    #  memtype = mem.GetDataType()
      
  else:
    print 'skipping ', item


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
