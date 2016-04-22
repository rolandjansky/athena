# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import array, math, random, ROOT, sys
from optparse import OptionParser

def main():
  parser = OptionParser()
  parser.add_option( '-i', '--input',  dest = 'input',  action = 'store', type = 'string', default = '', help = '' )
  parser.add_option( '-o', '--output',  dest = 'output',  action = 'store', type = 'string', default = '', help = '' )
  parser.add_option( '-n', '--n-events',  dest = 'events',  action = 'store', type = 'int', default = 10, help = '' )
  ( options, args ) = parser.parse_args()
  Run( options.input, options.output, options.events )
    
def Run( Input, Output, Events ): 

  InFile = ROOT.TFile( Input )
  InTree = InFile.Get( 'MuonMomentumCalibrationTree' ) 

  OutFile = ROOT.TFile( Output, 'recreate' )
  OutTree = ROOT.TTree( 'MuonMomentumCalibrationTree', '' )

  print 'Running MakeToys.py Script, Input File = %s, Output File = %s' % ( Input, Output ) 

  RunNumber = array.array( 'I', [ 0 ] )
  EvtNumber = array.array( 'L', [ 0 ] )
  LumiBlock = array.array( 'I', [ 0 ] )
  ChannelNumber = array.array( 'I', [ 0 ] )
  EventWeight = array.array( 'f', [ 0. ] )

  Trig_HLT_mu24_imedium = array.array( 'i', [ 0 ] )
  Trig_HLT_mu26_imedium = array.array( 'i', [ 0 ] )
  Trig_HLT_mu50 = array.array( 'i', [ 0 ] )
  Trig_HLT_mu22_mu8noL1 = array.array( 'i', [ 0 ] )
  Trig_HLT_mu24_mu8noL1 = array.array( 'i', [ 0 ] )
  Trig_HLT_2mu14 = array.array( 'i', [ 0 ] )
  Trig_HLT_2mu4 = array.array( 'i', [ 0 ] )
  Trig_HLT_2mu4_bJpsimumu = array.array( 'i', [ 0 ] )
  Trig_HLT_2mu4_bUpsimumu = array.array( 'i', [ 0 ] )

  Pos_Pt = array.array( 'f', [ 0. ] )
  Pos_Eta = array.array( 'f', [ 0. ] )
  Pos_Phi = array.array( 'f', [ 0. ] )
  Pos_Charge = array.array( 'f', [ 0. ] )
  Pos_ID_Pt  = array.array( 'f', [ 0. ] )
  Pos_ME_Pt  = array.array( 'f', [ 0. ] )
  Pos_ME_Eta = array.array( 'f', [ 0. ] )
  Pos_ME_Phi = array.array( 'f', [ 0. ] )
  Pos_d0sig  = array.array( 'f', [ 0. ] )
  Pos_z0  = array.array( 'f', [ 0. ] )
  Pos_PtCone20  = array.array( 'f', [ 0. ] )
  Pos_PtCone30  = array.array( 'f', [ 0. ] )
  Pos_PtCone40  = array.array( 'f', [ 0. ] )
  Pos_PtVarCone20  = array.array( 'f', [ 0. ] )
  Pos_PtVarCone30  = array.array( 'f', [ 0. ] )
  Pos_PtVarCone40  = array.array( 'f', [ 0. ] )
  Pos_Quality = array.array( 'i', [ 0 ] )
  Pos_Author = array.array( 'i', [ 0 ] )
  Pos_EnergyLossType = array.array( 'i', [ 0 ] )
  Pos_PrimarySector = array.array( 'i', [ 0 ] )
  Pos_SecondarySector = array.array( 'i', [ 0 ] )

  Neg_Pt = array.array( 'f', [ 0. ] )
  Neg_Eta = array.array( 'f', [ 0. ] )
  Neg_Phi = array.array( 'f', [ 0. ] )
  Neg_Charge = array.array( 'f', [ 0. ] )
  Neg_ID_Pt  = array.array( 'f', [ 0. ] )
  Neg_ME_Pt  = array.array( 'f', [ 0. ] )
  Neg_ME_Eta = array.array( 'f', [ 0. ] )
  Neg_ME_Phi = array.array( 'f', [ 0. ] )
  Neg_d0sig  = array.array( 'f', [ 0. ] )
  Neg_z0  = array.array( 'f', [ 0. ] )
  Neg_PtCone20  = array.array( 'f', [ 0. ] )
  Neg_PtCone30  = array.array( 'f', [ 0. ] )
  Neg_PtCone40  = array.array( 'f', [ 0. ] )
  Neg_PtVarCone20  = array.array( 'f', [ 0. ] )
  Neg_PtVarCone30  = array.array( 'f', [ 0. ] )
  Neg_PtVarCone40  = array.array( 'f', [ 0. ] )
  Neg_Quality = array.array( 'i', [ 0 ] )
  Neg_Author = array.array( 'i', [ 0 ] )
  Neg_EnergyLossType = array.array( 'i', [ 0 ] )
  Neg_PrimarySector = array.array( 'i', [ 0 ] )
  Neg_SecondarySector = array.array( 'i', [ 0 ] )

  OutTree.Branch( 'RunNumber', RunNumber, 'RunNumber/i' )
  OutTree.Branch( 'EvtNumber', EvtNumber, 'EvtNumber/l' )
  OutTree.Branch( 'LumiBlock', LumiBlock, 'LumiBlock/i' )
  OutTree.Branch( 'ChannelNumber', ChannelNumber, 'ChannelNumber/i' )
  OutTree.Branch( 'EventWeight', EventWeight, 'EventWeight/F' )

  OutTree.Branch( 'Trig_HLT_mu24_imedium', Trig_HLT_mu24_imedium, 'Trig_HLT_mu24_imedium/I' )
  OutTree.Branch( 'Trig_HLT_mu26_imedium', Trig_HLT_mu26_imedium, 'Trig_HLT_mu26_imedium/I' )
  OutTree.Branch( 'Trig_HLT_mu50', Trig_HLT_mu50, 'Trig_HLT_mu50/I' )
  OutTree.Branch( 'Trig_HLT_mu22_mu8noL1', Trig_HLT_mu22_mu8noL1, 'Trig_HLT_mu22_mu8noL1/I' )
  OutTree.Branch( 'Trig_HLT_mu24_mu8noL1', Trig_HLT_mu24_mu8noL1, 'Trig_HLT_mu24_mu8noL1/I' )
  OutTree.Branch( 'Trig_HLT_2mu14', Trig_HLT_2mu14, 'Trig_HLT_2mu14/I' )
  OutTree.Branch( 'Trig_HLT_2mu4', Trig_HLT_2mu4, 'Trig_HLT_2mu4/I' )
  OutTree.Branch( 'Trig_HLT_2mu4_bJpsimumu', Trig_HLT_2mu4_bJpsimumu, 'Trig_HLT_2mu4_bJpsimumu/I' )
  OutTree.Branch( 'Trig_HLT_2mu4_bUpsimumu', Trig_HLT_2mu4_bUpsimumu, 'Trig_HLT_2mu4_bUpsimumu/I' )

  OutTree.Branch( 'Pos_Pt', Pos_Pt, 'Pos_Pt/F' )
  OutTree.Branch( 'Pos_Eta', Pos_Eta, 'Pos_Eta/F' )
  OutTree.Branch( 'Pos_Phi', Pos_Phi, 'Pos_Phi/F' )
  OutTree.Branch( 'Pos_Charge', Pos_Charge, 'Pos_Charge/F' )
  OutTree.Branch( 'Pos_ID_Pt', Pos_ID_Pt, 'Pos_ID_Pt/F' )
  OutTree.Branch( 'Pos_ME_Pt', Pos_ME_Pt, 'Pos_ME_Pt/F' )
  OutTree.Branch( 'Pos_ME_Eta', Pos_ME_Eta, 'Pos_ME_Eta/F' )
  OutTree.Branch( 'Pos_ME_Phi', Pos_ME_Phi, 'Pos_ME_Phi/F' )
  OutTree.Branch( 'Pos_d0sig', Pos_d0sig, 'Pos_d0sig/F' )
  OutTree.Branch( 'Pos_z0', Pos_z0 , 'Pos_z0/F' )
  OutTree.Branch( 'Pos_PtCone20', Pos_PtCone20, 'Pos_PtCone20/F' )
  OutTree.Branch( 'Pos_PtCone30', Pos_PtCone30, 'Pos_PtCone30/F' )
  OutTree.Branch( 'Pos_PtCone40', Pos_PtCone40, 'Pos_PtCone40/F' )
  OutTree.Branch( 'Pos_PtVarCone20', Pos_PtVarCone20, 'Pos_PtVarCone20/F' )
  OutTree.Branch( 'Pos_PtVarCone30', Pos_PtVarCone30, 'Pos_PtVarCone30/F' )
  OutTree.Branch( 'Pos_PtVarCone40', Pos_PtVarCone40, 'Pos_PtVarCone40/F' )
  OutTree.Branch( 'Pos_Quality', Pos_Quality, 'Pos_Quality/I' )
  OutTree.Branch( 'Pos_Author', Pos_Author, 'Pos_Author/I' )
  OutTree.Branch( 'Pos_EnergyLossType', Pos_EnergyLossType, 'Pos_EnergyLossType/I' )
  OutTree.Branch( 'Pos_PrimarySector', Pos_PrimarySector, 'Pos_PrimarySector/I' )
  OutTree.Branch( 'Pos_SecondarySector', Pos_SecondarySector, 'Pos_SecondarySector/I' )

  OutTree.Branch( 'Neg_Pt', Neg_Pt, 'Neg_Pt/F' )
  OutTree.Branch( 'Neg_Eta', Neg_Eta, 'Neg_Eta/F' )
  OutTree.Branch( 'Neg_Phi', Neg_Phi, 'Neg_Phi/F' )
  OutTree.Branch( 'Neg_Charge', Neg_Charge, 'Neg_Charge/F' )
  OutTree.Branch( 'Neg_ID_Pt', Neg_ID_Pt, 'Neg_ID_Pt/F' )
  OutTree.Branch( 'Neg_ME_Pt', Neg_ME_Pt, 'Neg_ME_Pt/F' )
  OutTree.Branch( 'Neg_ME_Eta', Neg_ME_Eta, 'Neg_ME_Eta/F' )
  OutTree.Branch( 'Neg_ME_Phi', Neg_ME_Phi, 'Neg_ME_Phi/F' )
  OutTree.Branch( 'Neg_d0sig', Neg_d0sig, 'Neg_d0sig/F' )
  OutTree.Branch( 'Neg_z0', Neg_z0, 'Neg_z0/F' )
  OutTree.Branch( 'Neg_PtCone20', Neg_PtCone20, 'Neg_PtCone20/F' )
  OutTree.Branch( 'Neg_PtCone30', Neg_PtCone30, 'Neg_PtCone30/F' )
  OutTree.Branch( 'Neg_PtCone40', Neg_PtCone40, 'Neg_PtCone40/F' )
  OutTree.Branch( 'Neg_PtVarCone20', Neg_PtVarCone20, 'Neg_PtVarCone20/F' )
  OutTree.Branch( 'Neg_PtVarCone30', Neg_PtVarCone30, 'Neg_PtVarCone30/F' )
  OutTree.Branch( 'Neg_PtVarCone40', Neg_PtVarCone40, 'Neg_PtVarCone40/F' )
  OutTree.Branch( 'Neg_Quality', Neg_Quality, 'Neg_Quality/I' )
  OutTree.Branch( 'Neg_Author', Neg_Author, 'Neg_Author/I' )
  OutTree.Branch( 'Neg_EnergyLossType', Neg_EnergyLossType, 'Neg_EnergyLossType/I' )
  OutTree.Branch( 'Neg_PrimarySector', Neg_PrimarySector, 'Neg_PrimarySector/I' )
  OutTree.Branch( 'Neg_SecondarySector', Neg_SecondarySector, 'Neg_SecondarySector/I' )
  
  ######

  RunNumber[ 0 ] = 0
  EvtNumber[ 0 ] = 0
  LumiBlock[ 0 ] = 0
  ChannelNumber[ 0 ] = 0
  EventWeight[ 0 ] = 1.
  Pos_Charge[ 0 ] = 1.
  Pos_Quality[ 0 ] = 0
  Neg_Charge[ 0 ] = -1.
  Neg_Quality[ 0 ] = 0
  for Index, Entry in enumerate( InTree ):
    if Index % 100000 == 0:
      print 'Executing Event #%d' % Index
    if Index > Events and Events > 0: break
    if math.fabs( Entry.Pos_Eta ) > 2.6: continue
    if math.fabs( Entry.Neg_Eta ) > 2.6: continue
    # Event
    Trig_HLT_mu24_imedium[ 0 ] = Entry.Trig_HLT_mu24_imedium
    Trig_HLT_mu26_imedium[ 0 ] = Entry.Trig_HLT_mu26_imedium
    Trig_HLT_mu50[ 0 ] = Entry.Trig_HLT_mu50
    Trig_HLT_mu22_mu8noL1[ 0 ] = Entry.Trig_HLT_mu22_mu8noL1
    Trig_HLT_mu24_mu8noL1[ 0 ] = Entry.Trig_HLT_mu24_mu8noL1
    Trig_HLT_2mu14[ 0 ] = Entry.Trig_HLT_2mu14
    Trig_HLT_2mu4[ 0 ] = Entry.Trig_HLT_2mu4
    Trig_HLT_2mu4_bJpsimumu[ 0 ] = Entry.Trig_HLT_2mu4_bJpsimumu
    Trig_HLT_2mu4_bUpsimumu[ 0 ] = Entry.Trig_HLT_2mu4_bUpsimumu
    # Positive Muon
    #ID = ( 0., 0.01 * math.sin( 2 * Entry.Pos_Eta ) / ( Entry.Pos_Eta + 5 ), 0,  0.002 + 0.01 * math.exp( - math.pow( Entry.Pos_Eta / 2, 2 ) ), 0.00016 * ( 2 + math.erf( Entry.Pos_Eta ) ) )
    #ME = ( -0.03, 0.003 * math.sin( math.erf( 2. * Entry.Pos_Eta ) ), 0.15, 0.008 * math.tanh( 5. * Entry.Pos_Eta ) / Entry.Pos_Eta, 0.00015 * ( 1 + math.cos( 2. * Entry.Pos_Eta ) ) + 0.00005 )
    Pos_Pt[ 0 ] = Entry.Pos_Pt
    Pos_Eta[ 0 ] = Entry.Pos_Eta
    Pos_Phi[ 0 ] = Entry.Pos_Phi
    #ID = ( 0., -0.001, 0., 0.007, 0.0001 ) 
    #ME = ( -0.1, -0.001, 0.001, 0.007, 0.0001 ) 
    ID = GetIDCorrections( Entry.Pos_Eta ) 
    ME = GetMECorrections( Entry.Pos_Eta ) 
    Pos_ID_Pt[ 0 ] = Correct( ID, Entry.Pos_ID_Pt, Entry.Pos_Eta )
    Pos_ME_Pt[ 0 ] = Correct( ME, Entry.Pos_ME_Pt, 0 )
    Pos_ME_Eta[ 0 ] = Entry.Pos_Eta
    Pos_ME_Phi[ 0 ] = Entry.Pos_Phi
    Pos_d0sig [ 0 ] = Entry.Pos_d0sig 
    Pos_z0 [ 0 ] = Entry.Pos_z0 
    Pos_PtCone20 [ 0 ] = Entry.Pos_PtCone20 
    Pos_PtCone30 [ 0 ] = Entry.Pos_PtCone30 
    Pos_PtCone40 [ 0 ] = Entry.Pos_PtCone40 
    Pos_PtVarCone20 [ 0 ] = Entry.Pos_PtVarCone20 
    Pos_PtVarCone30 [ 0 ] = Entry.Pos_PtVarCone30 
    Pos_PtVarCone40 [ 0 ] = Entry.Pos_PtVarCone40 
    Pos_Quality[ 0 ] = Entry.Pos_Quality
    Pos_Author[ 0 ] = Entry.Pos_Author
    Pos_EnergyLossType[ 0 ] = Entry.Pos_EnergyLossType
    Pos_PrimarySector[ 0 ] = Entry.Pos_PrimarySector
    Pos_SecondarySector[ 0 ] = Entry.Pos_SecondarySector
    #print 'eta=%.5f --- phi=%.5f --- pt=%.5f --- ptc=%.5f ---' % ( Pos_Eta[ 0 ], Pos_Phi[ 0 ], Entry.Pos_Pt, Pos_Pt[ 0 ] )
    # Negative Muon
    #ID = ( 0., 0.01 * math.sin( 2 * Entry.Neg_Eta ) / ( Entry.Neg_Eta + 5 ), 0,  0.002 + 0.01 * math.exp( - math.pow( Entry.Neg_Eta / 2, 2 ) ), 0.00016 * ( 2 + math.erf( Entry.Neg_Eta ) ) )
    #ME = ( -0.03, 0.003 * math.sin( math.erf( 2. * Entry.Neg_Eta ) ), 0.15, 0.008 * math.tanh( 5. * Entry.Neg_Eta ) / Entry.Neg_Eta, 0.00015 * ( 1 + math.cos( 2. * Entry.Neg_Eta ) ) + 0.00005 )
    Neg_Pt[ 0 ] = Entry.Neg_Pt
    Neg_Eta[ 0 ] = Entry.Neg_Eta
    Neg_Phi[ 0 ] = Entry.Neg_Phi
    ID = GetIDCorrections( Entry.Neg_Eta ) 
    ME = GetMECorrections( Entry.Neg_Eta ) 
    Neg_ID_Pt[ 0 ] = Correct( ID, Entry.Neg_ID_Pt, Entry.Neg_Eta )
    Neg_ME_Pt[ 0 ] = Correct( ME, Entry.Neg_ME_Pt, 0 )
    Neg_ME_Eta[ 0 ] = Entry.Neg_Eta
    Neg_ME_Phi[ 0 ] = Entry.Neg_Phi
    Neg_d0sig [ 0 ] = Entry.Neg_d0sig 
    Neg_z0 [ 0 ] = Entry.Neg_z0 
    Neg_PtCone20 [ 0 ] = Entry.Neg_PtCone20 
    Neg_PtCone30 [ 0 ] = Entry.Neg_PtCone30 
    Neg_PtCone40 [ 0 ] = Entry.Neg_PtCone40 
    Neg_PtVarCone20 [ 0 ] = Entry.Neg_PtVarCone20 
    Neg_PtVarCone30 [ 0 ] = Entry.Neg_PtVarCone30 
    Neg_PtVarCone40 [ 0 ] = Entry.Neg_PtVarCone40 
    Neg_Quality[ 0 ] = Entry.Neg_Quality
    Neg_Author[ 0 ] = Entry.Neg_Author
    Neg_EnergyLossType[ 0 ] = Entry.Neg_EnergyLossType
    Neg_PrimarySector[ 0 ] = Entry.Neg_PrimarySector
    Neg_SecondarySector[ 0 ] = Entry.Neg_SecondarySector
    #print 'eta=%.5f --- phi=%.5f --- pt=%.5f --- ptc=%.5f ---' % ( Neg_Eta[ 0 ], Neg_Phi[ 0 ], Entry.Neg_Pt, Neg_Pt[ 0 ] )
    #pos = ROOT.TLorentzVector()
    #neg = ROOT.TLorentzVector()
    #pos.SetPtEtaPhiM( Pos_ID_Pt[ 0 ], Pos_Eta[ 0 ], Pos_Phi[ 0 ], 0.105 ) 
    #neg.SetPtEtaPhiM( Neg_ID_Pt[ 0 ], Neg_Eta[ 0 ], Neg_Phi[ 0 ], 0.105 ) 
    #MassID[ 0 ] = ( pos + neg ).M()
    #pos.SetPtEtaPhiM( Pos_ME_Pt[ 0 ], Pos_Eta[ 0 ], Pos_Phi[ 0 ], 0.105 ) 
    #neg.SetPtEtaPhiM( Neg_ME_Pt[ 0 ], Neg_Eta[ 0 ], Neg_Phi[ 0 ], 0.105 ) 
    #MassME[ 0 ] = ( pos + neg ).M()
    OutTree.Fill()
  OutFile.Write()
  OutFile.Close()

def Correct( Vals, pt, eta ):
  add = 1.
  if abs( eta ) > 2:
    add = math.sinh( eta )
  res = ( Vals[ 0 ] + pt * ( 1. + Vals[ 1 ] ) ) / random.gauss( 1, math.sqrt( math.pow( Vals[ 2 ] / pt, 2 ) + math.pow( Vals[ 3 ], 2 ) + math.pow( Vals[ 4 ] * pt * add * add, 2 ) ) )
  return res

def GetIDCorrections( eta ):
  Regs = [ -2.6, -2, 0.4, 2, 2.6 ]
  Corr = {
    0: ( 0., -0.001, 0., 0.005, 0.00005 ), 
    1: ( 0.,      0, 0., 0.002, 0.0007 ), 
    2: ( 0.,  0.001, 0., 0.007, 0.0001 ), 
    3: ( 0.,  -0.0005, 0., 0.002, 0.00002 )
  }
  the_index = 0
  for index, min in enumerate( Regs ):
    if Regs[ index ] < eta and eta < Regs[ index + 1 ]:
      the_index = index
  return ( 0., -0.001, 0., 0.005, 0.0005 ) 
  return Corr[ the_index ]

def GetMECorrections( eta ):
  Regs = [ -2.6, -2, 0.4, 2, 2.6 ]
  ME = ( -0.1, -0.001, 0.001, 0.007, 0.0001 ) 
  return ME
  Corr = {
    0: ( 0., -0.001, 0., 0.005, 0.0005 ), 
    1: ( 0.,      0, 0., 0.002, 0.0007 ), 
    2: ( 0.,  0.001, 0., 0.007, 0.0001 ), 
    3: ( 0.,  0.002, 0., 0.002, 0.0002 )
  }
  the_index = 0
  for index, min in enumerate( Regs ):
    if Regs[ index ] < eta and eta < Regs[ index + 1 ]:
      the_index = index
  return Corr[ the_index ]

if __name__ == '__main__':
  sys.exit( main() )
