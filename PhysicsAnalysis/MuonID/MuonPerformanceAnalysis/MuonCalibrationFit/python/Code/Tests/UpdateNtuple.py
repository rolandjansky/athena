# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import array, re, sys, ROOT
from optparse import OptionParser

def main():

  ##### General commands #####
  ROOT.gROOT.SetBatch()
  ROOT.gErrorIgnoreLevel = 0

  ##### Parsing options #####
  parser = OptionParser()
  parser.add_option( '-i', '--input',  dest = 'input',  action = 'store', type = 'string', default = '', help = '' )
  parser.add_option( '-o', '--output', dest = 'output', action = 'store', type = 'string', default = '', help = '' )
  ( options, args ) = parser.parse_args()

  ##### File Business #####
  InputFile = ROOT.TFile( options.input )
  InputTree = InputFile.Get( 'ZTreeReso_muid' ) 
  OutputFile = ROOT.TFile( options.output, 'recreate' )
  OutputTree = ROOT.TTree( 'muon', '' )

  ##### Variables' Definition #####
  RunNumber = array.array( 'I', [ 0 ] )
  EvtNumber = array.array( 'L', [ 0 ] )
  LumiBlock = array.array( 'I', [ 0 ] )
  ChannelNumber = array.array( 'I', [ 0 ] )
  EventWeight = array.array( 'f', [ 0. ] )
  #####
  Pos_Pt = array.array( 'f', [ 0. ] )
  Pos_Eta = array.array( 'f', [ 0. ] )
  Pos_Phi = array.array( 'f', [ 0. ] )
  Pos_Charge = array.array( 'f', [ 0. ] )
  Pos_ID_Pt  = array.array( 'f', [ 0. ] )
  Pos_MS_Pt  = array.array( 'f', [ 0. ] )
  Pos_MS_Eta = array.array( 'f', [ 0. ] )
  Pos_MS_Phi = array.array( 'f', [ 0. ] )
  Pos_Quality = array.array( 'f', [ 0. ] )
  #####
  Neg_Pt     = array.array( 'f', [ 0. ] )
  Neg_Eta    = array.array( 'f', [ 0. ] )
  Neg_Phi    = array.array( 'f', [ 0. ] )
  Neg_Charge = array.array( 'f', [ 0. ] )
  Neg_ID_Pt  = array.array( 'f', [ 0. ] )
  Neg_MS_Pt  = array.array( 'f', [ 0. ] )
  Neg_MS_Eta = array.array( 'f', [ 0. ] )
  Neg_MS_Phi = array.array( 'f', [ 0. ] )
  Neg_Quality = array.array( 'f', [ 0. ] )
  #####
  MassID = array.array( 'f', [ 0. ] )
  MassMS = array.array( 'f', [ 0. ] )

  ##### Branches' Definition #####
  OutputTree.Branch( 'RunNumber',     RunNumber,     'RunNumber/i'     );
  OutputTree.Branch( 'EvtNumber',     EvtNumber,     'EvtNumber/l'     );
  OutputTree.Branch( 'LumiBlock',     LumiBlock,     'LumiBlock/i'     );
  OutputTree.Branch( 'ChannelNumber', ChannelNumber, 'ChannelNumber/i' );
  OutputTree.Branch( 'EventWeight',   EventWeight,   'EventWeight/F'   );
  #####
  OutputTree.Branch( 'Pos_Pt',      Pos_Pt,      'Pos_Pt/F'      );
  OutputTree.Branch( 'Pos_Eta',     Pos_Eta,     'Pos_Eta/F'     );
  OutputTree.Branch( 'Pos_Phi',     Pos_Phi,     'Pos_Phi/F'     );
  OutputTree.Branch( 'Pos_Charge',  Pos_Charge,  'Pos_Charge/F'  );
  OutputTree.Branch( 'Pos_ID_Pt',   Pos_ID_Pt,   'Pos_ID_Pt/F'   );
  OutputTree.Branch( 'Pos_MS_Pt',   Pos_MS_Pt,   'Pos_MS_Pt/F'   );
  OutputTree.Branch( 'Pos_MS_Eta',  Pos_MS_Eta,  'Pos_MS_Eta/F'  );
  OutputTree.Branch( 'Pos_MS_Phi',  Pos_MS_Phi,  'Pos_MS_Phi/F'  );
  OutputTree.Branch( 'Pos_Quality', Pos_Quality, 'Pos_Quality/I' );
  #####
  OutputTree.Branch( 'Neg_Pt',      Neg_Pt,      'Neg_Pt/F'      );
  OutputTree.Branch( 'Neg_Eta',     Neg_Eta,     'Neg_Eta/F'     );
  OutputTree.Branch( 'Neg_Phi',     Neg_Phi,     'Neg_Phi/F'     );
  OutputTree.Branch( 'Neg_Charge',  Neg_Charge,  'Neg_Charge/F'  );
  OutputTree.Branch( 'Neg_ID_Pt',   Neg_ID_Pt,   'Neg_ID_Pt/F'   );
  OutputTree.Branch( 'Neg_MS_Pt',   Neg_MS_Pt,   'Neg_MS_Pt/F'   );
  OutputTree.Branch( 'Neg_MS_Eta',  Neg_MS_Eta,  'Neg_MS_Eta/F'  );
  OutputTree.Branch( 'Neg_MS_Phi',  Neg_MS_Phi,  'Neg_MS_Phi/F'  );
  OutputTree.Branch( 'Neg_Quality', Neg_Quality, 'Neg_Quality/I' );
  #####
  OutputTree.Branch( 'MassID',  MassID,  'MassID/F'  );
  OutputTree.Branch( 'MassMS',  MassMS,  'MassMS/F'  );

  for Index, Entry in enumerate( InputTree ):

    PosIndex, NegIndex = -1, -1
    if Entry.Q[ 0 ] > 0 and Entry.Q[ 1 ] < 0:
      PosIndex = 0
      NegIndex = 1
    else:
      PosIndex = 1
      NegIndex = 0

    RunNumber[ 0 ] = Entry.Run
    EvtNumber[ 0 ] = Entry.Event
    LumiBlock[ 0 ] = 0
    ChannelNumber[ 0 ] = Entry.Run
    EventWeight[ 0 ] = Entry.EventWeight
    
    Pos_Pt[ 0 ] = Entry.Pt[ PosIndex ]
    Pos_Eta[ 0 ] = Entry.Eta[ PosIndex ]
    Pos_Phi[ 0 ] = Entry.Phi[ PosIndex ]
    Pos_Charge[ 0 ] = Entry.Q[ PosIndex ]
    Pos_ID_Pt [ 0 ] = 1. / Entry.QoverPtID[ PosIndex ]
    Pos_MS_Pt [ 0 ] = 1. / Entry.QoverPtMS[ PosIndex ]
    Pos_MS_Eta[ 0 ] = Entry.Eta[ PosIndex ]
    Pos_MS_Phi[ 0 ] = Entry.Phi[ PosIndex ]
    Pos_Quality[ 0 ] = 0
   
    Neg_Pt[ 0 ] = Entry.Pt[ NegIndex ]
    Neg_Eta[ 0 ] = Entry.Eta[ NegIndex ]
    Neg_Phi[ 0 ] = Entry.Phi[ NegIndex ]
    Neg_Charge[ 0 ] = Entry.Q[ NegIndex ]
    Neg_ID_Pt [ 0 ] = 1. / Entry.QoverPtID[ NegIndex ]
    Neg_MS_Pt [ 0 ] = 1. / Entry.QoverPtMS[ NegIndex ]
    Neg_MS_Eta[ 0 ] = Entry.Eta[ NegIndex ]
    Neg_MS_Phi[ 0 ] = Entry.Phi[ NegIndex ]
    Neg_Quality[ 0 ] = 0
 
    PosID = ROOT.TLorentzVector()
    PosID.SetPtEtaPhiM( Pos_ID_Pt [ 0 ], Pos_Eta[ 0 ], Pos_Phi[ 0 ], 0.105658 )
    PosMS = ROOT.TLorentzVector()
    PosMS.SetPtEtaPhiM( Pos_MS_Pt [ 0 ], Pos_Eta[ 0 ], Pos_Phi[ 0 ], 0.105658 )

    NegID = ROOT.TLorentzVector()
    NegID.SetPtEtaPhiM( Neg_ID_Pt [ 0 ], Neg_Eta[ 0 ], Neg_Phi[ 0 ], 0.105658 )
    NegMS = ROOT.TLorentzVector()
    NegMS.SetPtEtaPhiM( Neg_MS_Pt [ 0 ], Neg_Eta[ 0 ], Neg_Phi[ 0 ], 0.105658 )

    MassID[ 0 ] = ( PosID + NegID ).M() 
    MassMS[ 0 ] = ( PosMS + NegMS ).M() 

    OutputTree.Fill()

  OutputFile.Write()
  OutputFile.Close()
  InputFile.Close()

if __name__ == '__main__':
  sys.exit( main() )
