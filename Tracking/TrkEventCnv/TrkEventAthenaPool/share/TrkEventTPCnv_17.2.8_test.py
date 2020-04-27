# Needed to work around some cling bugs seen in root 6.20.02.
# Spurious error message like
#  fatal error: cannot open file 'input_line_538': No such file or directory
import ROOT
getattr(ROOT,'SG::BaseInfo<DataVector<Trk::Track> >')
getattr(ROOT,'ElementLink<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::MdtPrepData> >')

infile = 'aod/AOD-17.2.8/AOD-17.2.8-full.pool.root'
keys = [
    #Trk::VxContainer_tlp2
    'HLT_ConversionVx',
    'SecVertices',
    'AllPhotonsVxCandidates',
    'VxPrimaryCandidate',
    'ConversionCandidate',
    'HLT_PrimVx',

    #Trk::TrackCollection_tlp3
    'Tracks',
    'MuonSlimmedTrackCollection',
    'GSFTracks',

    #Trk::SegmentCollection_tlp1
    'ConvertedMBoySegments',
    'MuonSegments',
    'MooreSegments',
    'MuGirlSegments',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')

