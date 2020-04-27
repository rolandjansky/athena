# Needed to work around some cling bugs seen in root 6.20.02.
# Spurious error message like
#  fatal error: cannot open file 'input_line_538': No such file or directory
import ROOT
getattr(ROOT,'SG::BaseInfo<DataVector<Trk::Track> >')
getattr(ROOT,'ElementLink<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::MdtPrepData> >')

infile = 'aod/AOD-16.6.2.1/AOD-16.6.2.1-full.pool.root'
keys = [
    #MVFVxContainer_tlp1
    #43.479 kb        7.172 kb        0.717 kb        0.164       20  (T) MVFVxContainer_tlp1???

    #Trk::VxContainer_tlp2
    'SecVertices',
    'AllPhotonsVxCandidates',
    'VxPrimaryCandidate',
    'HLT_ConversionVx',
    'HLT_PrimVx',
    'ConversionCandidate',

    #Trk::TrackCollection_tlp3
    'MuonSlimmedTrackCollection',
    'Tracks',

    #Trk::SegmentCollection_tlp1
    'ConvertedMBoySegments',
    'MuGirlSegments',
    'MooreSegments',

    #V0Container_tlp2
    'V0Candidates',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')

