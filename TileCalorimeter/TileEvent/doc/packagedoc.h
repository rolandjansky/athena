/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TileEvent_page TileEvent Package

This package provides event data model classes for TileCal reconstruction



@author Alexander Solodkov <Sanya.Solodkov@cern.ch>
@author Frank Merritt <Frank.Merritt@cern.ch>

@section TileEvent_TileEventEDM Raw data EDM and reconstruction EDM

All classes in TileEvent package can be split in two parts:
raw data classes and classes for reconstructed objects.
Raw data classes are: TileDigits, TileRawChannel, TileBeamElem, TileTTL1, 
TileL2, TileTrigger
Reco classes are: TileCell, TileMu, TileCosmicMuon, TileTTL1Cell
There is another set of raw data classes to keep information from 
laser system: TileLaserObject which holds TileLaserControl, 
TileLaser, vector of TileLaserPmt and TileLaserDiode


@section TileEvent_TileEventRawData Raw data EDM

TileDigits, TileRawChannel and TileBeamElem raw data classes inherit from 
TileRawData class, which contains just one member 'm_adc_hwid' - so-called 
hardware (or online) identifier. It identifies one ADC channel in readout 
electronics  (see TileHWID.h class in TileCalorimeter/TileIdentifier).
The same TileRawData class provides methods to convert from online identifier 
to offline identifier, one can retrieve offline identifiers pmt_id, cell_id 
for any channel. If channel is not connected invalid identifier is returned, and
pmt_id.is_valid() can be used to skip disconnected channels 

TileDigits class contains vector of ADC 'digits' - signal from the calorimeter
measured in 25ns time slices. 
TileBeamElem is similar to TileDigits, but contains output from extra
detectors at the testbeam, e.g. beam chambers, beam counters etc. In other
words TileBeamElem is the class for everything which comes from special
Tile Beam ROD fragment in bytestream file. Finally, TileRawChannel is the
class for results of Optimal Filter applied to TileDigits. It contains
amplitude, time, quality and pedestal. If Optimal Filter runs offline,
it can find more than one signal in a vector of digits and amplitude,
time and quality are vectors. Pedestal is a scalar variable.

In StoreGate raw data objects like TileDigits, TileRawChannel, TileBeamElem 
as well as TileCell can be stored in IdentifiableContainer. Every 
IdentifiableContainer contains 256 collections, one collection corresponds 
to one TileCal drawer. Every collection contains up to 48 raw data objects
(== channels) or up to 23 TileCells. Templates TileRawDataCollection and 
TileRawDataContainer are used for all collections and containers, and, 
for example TileRawChannelCollection is TileRawDataCollection<TileRawChannel>,
TileRawChannelContainer is TileRawDataContainer<TileRawChannelCollection>, etc.
TileRawChannelContainer keeps one additional enum which tells what are the 
units used in those TileRawChannels. Units can be TileInfo::ADCcounts, 
TileInfo::PicoCoulombs (afer CIS calibration) TileInfo::CesiumPicoCoulombs 
(i.e. additional Cesium and laser calibration 
are applied afeter CIS) or TileInfo::MegaElectronVolts. Another property 
of TileRawChannelContainer is it's type indicating which algorithm was used 
to create TileRawChannel (FlatFilter, FitMethod, OF1, OF2, ManyAmps - see 
TileRecUtils for details). 
This type is important, becuase CIS calibration depends on the algorithm
and conditions database keeps different calibration constants for different 
methods.

@section TileEvent_TileEventTriggerData Tile Trigger EDM

TileTTL1 class is similar to TileDigits, but
corresponds to the signal which goes to Level 1 trigger and therefore
TileTTL1 contains special Level 1 identifier and not TileCal online identifier.
vector of amplitudes which is stored in TileTTL1 normally corresponds to 25ns 
time slices, but it can also have just one maximal amplitude (used for simulation
of cosmic muon trigger) or vector of amplitudes with 0.5ns granularity (full trigger 
pulse shape scaled proportiinally to signal in a PMT)

TileL2 is the class with additional information provided by the ROD for Level 2 trigger.
It contains fragment ID from the bytestream (to identify drawer):
0x100-0x13F, 0x200-0x23F, 0x300-0x33F, 0x400-0x43F
and then several quantities calculated for this drawer: Et (transverse energy) and 
muon candidates for trigger: their energy, eta and phi.

TileTrigger is the class that contains the event trigger information
from the simulated Cosmics trigger boards.
This class contais information about which TileCal towers cave a trigger
in each of three possible trigger modes: single tower, back-2-back, and
board coincidence.

TileTTL1, TileL2 TileTrigger are never stored in IdentifiableContainer. 
Simple DataVector<TileTTL1>, DataVector<TileL2> DataVector<TileTrigger> 
are used instead.

@section TileEvent_TileEventLaserData Tile Laser EDM

TileLaserObject class contains everything which comes from TileCal Laser system
This information is unpacked from special laser fragment in the ByteStream and then
stored in TileLaserControl, TileLaser, TileLaserPmt and TileLaserDiode.
TileLaserObject keeps all the objects mentioned above as private members and
provides access methods to all the elements.
There is only one TileLaserObject in StoreGate.

@section TileEvent_TileEventRecData Reconstruction EDM

TileCell class inherits from CaloCell class (see Calorimeter/CaloEvent package)
And contains result of reconstruction - total energy deposited in a cell,
calibrated with EM scale. In Tile Calorimeter almost all cells are read out by
two PMTs. To preserve information about both PMTs in a cell, two extra 
variables were added to TileCell: ene_diff and time_diff - difference between
two PMTs. Special member functions ene1(), ene2(), time1(), time2(), gain1()
gain2(), quality1(), quality2() return corresponding values
for both PMTs separately, while CaloCell functions energy() and time()
returns total energy deposited in a cell and average signal time from two
PMTs in a cell.

Normally TileCells are stored in CaloCellContainer, common container for
all LArCells and TileCells. For Level 2 Trigger TileCells can be stored
in IdentifiableContainer TileCellIDC, similar to TileRawChannelContainer.
TileCells which corresponds to Minimum Bias Trigger Scintillators (MBTS)
as well as TileCells for ancillary detectors at the testbeam (Muon Wall,
cryostat scintillators, etc) with special TileTBID identifiers are stored
in TileCellContainer, which is DataVector<TileCell>.

TileMu is a class to keep results of TileMuId algorithm. It doesn't contain
any identifier, only eta,phi,energy,quality for every muon candidate.
DataVector<TileMu> is used to keep all muon candidates in StoreGate

TileCosmicMuon contains the results of the TileMuonFitter (see TileCosmicAlgs
package) algorithm, which is basically a straight line fit to the TileCal cell
positions, useful for cosmic muons analysis. It contains information about the
fit (a fit quality parameter and the number of cells used), the geometrical
track parameters (position at the y=0 plane, direction angles) and derived
quantities (time at Y=0, path length in TileCal and energy along track). A list
of cells close to the track is also kept.

TileTTL1Cell contains the L1 trigger tower energy sum but built from Tile cell energy information. 
This information is used mainly by L1Calo to compare their trigger tower energy to Tile's digitial
readout energy. Also stored is the average time in the tower from all cells and a status word which
contains information about dead cells and channels. 

@section TileEvent_TileEventRecData Reconstruction EDM

*/
