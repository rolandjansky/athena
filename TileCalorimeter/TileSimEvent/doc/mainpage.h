/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TileSimEvent Package

This package provides event data model classes for TileCal Geant4 simulation 


@author Vakhtang Tsulaia <Vakhtang.Tsulaia@cern.ch>
@author Alexander Solodkov <Sanya.Solodkov@cern.ch>

@section TileSimEventTileHit TileHit class

TileHit class is used to store visible energy deposited in active part of the 
Tile calorimeter in Geant4 simulation. Active part of the TileCal is 
scintillator and therefore Birk saturation law is applied to every Geant4
hit before adding this hit energy to total energy in TileHit 

There are few differences between TileHit and LArHit classes:

1) One TileCal cell is read out by two PMTs and TileHit contains total 
energy visible by one PMT in a cell, or, roughly speaking only "half" 
of cell energy. Identifier stored in TileHit is a PMT identifier, which
is built from cell identifier (e.g. 5/2/-1/62/12/1 ) with one extra field 
added at the end, namely PMT index (0 or 1). For details, see TileID.h class
in Calorimeter/CaloIdentifier package

2) To have possibility to study signal pulse shape, m_energy and m_time
member variables are vectors, and m_energy[i] is the energy deposited at
time m_time[i]. Granularity in time can be set at initialization time.
If, for example this granularity is set to 5 ns, m_time will contain values
0,5,10,... and m_energy vector will contain deposited energy in 5 ns bins.
For full ATLAS simulation the granularity is set to 5ns and for testbeam
and cosmic simulation granularity is usually set to 1ns.
Time for testbeam and cosmic simulation stored in TileHit 
might be rather big (>100ns), but it is corrected by global event time
in TileHitVecToCnt algorithm from TileSimAlgs package and hits which 
are used in TileDigitsMaker have time close to zero (within [-100ns,100ns] window)
 
@section TileSimEventTileHitVector TileHitVector class

TileHitVector is a typedef of AthenaHitsVector<TileHit>
and it is used to persistify TileHits


*/
