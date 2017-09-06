/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

This package is used to decode and encode TileCal bytestream.

@page TileByteStream_page 

@author Alexander Solodkov <Sanya.Solodkov@cern.ch>
@author Denis Oliveira Damazio <Denis.Oliveira.Damazio@cern.ch>
@author Aranzazu Ruiz Martinez <Aranzazu.Ruiz.Martinez@cern.ch>
@author Belen Salvachua <Belen.Salvachua@cern.ch>

@section TileByteStream_Cnv Converters
  
  The following converters are present in this package:

  - TileRawChannelContByteStreamCnv: this class contains methods to do the conversion from bytestream format to TileRawChannel stored in a TileRawChannelContainer and vice-versa making use of the TileRawChannelContByteStreamTool.

  - TileBeamElemContByteStreamCnv: this class contains methods to do the conversion from bytestream format to TileBeamElem stored in a TileBeamElemContainer.
  
  - TileDigitsContByteStreamCnv: this class contains methods to do the conversion from bytestream format to TileDigits stored in a TileDigitsContainer.

  - TileL2ContByteStreamCnv: this class contains methods to do the conversion from bytestream format to TileL2 stored in a TileL2Container and vice-versa making use of the TileL2ContByteStreamTool.

  - TileCellIDC_ByteStreamCnv: this class contains methods to do the conversion from bytestream format to TileCells stored in a TileCellIDC container (IdentifiableContainer, used for Level-2 Trigger).

@section TileByteStream_Tool AlgTools

  The following AlgTools are present in this package:

  - TileRawChannelCollByteStreamTool: this class provides conversion from bytestream to either TileRawChannel or TileCell.

  - TileRawChannelContByteStreamTool: this class provides method convert() responsible for encoding into bytestream the TileRawChannel objects stored in a TileRawChannelContainer making use of the TileROD_Encoder class.

  - TileL2ContByteStreamTool: this class provides method convert() responsible for encoding into bytestream the TileL2 objects stored in a TileL2Container making use of the TileROD_Encoder class.

  - TileROD_Decoder: this class decodes the different TileCal ROD subfragment types in bytestream data and fills TileDigitsContainer, TileRawChannelContainer or TileL2Container.
    - frag type 0x0: Digitizer fragment
    - frag type 0x1: Digitizer fragment (non-existing channels supressed)
    - frag type 0x2: Non calibrated energy reconstruction fragment
    - frag type 0x3: Non calibrated energy reconstruction fragment (non-existing channels supressed)
    - frag type 0x4: Online calibrated energy reconstruction fragment 
    - frag type 0x10: MTag-MET in staging mode (2 FEBs per DSP)
    - frag type 0x11: MTag-MET in full mode (1 FEB per DSP)
    - frag type 0x12: MTag in staging mode
    - frag type 0x13: MTag in full mode
    - frag type 0x14: MET in staging mode
    - frag type 0x15: MET in full mode

@section TileByteStream_Class Classes

  The following additional classes are also present in this package:

  - TileROD_Encoder: this class encodes TileRawChannel and TileL2 objects into bytestream data (frag types 0x2, 0x3, 0x4 and 0x10).

  - TileHid2RESrcID: this class provides conversion from Tile Frag ID to RESrcID (Raw Event Source Identifier).

  - TileCellCont: this class builds the Tile cells.

  - TileDigits2Bytes: this class converts TileDigits to and from bytestream in ROD format.

  - TileRawChannel2Bytes: this class converts the TileRawChannel object into bytes.

  - TileRawChannel2Bytes2: this class converts the TileRawChannel object into bytes as it is defined at the testbeam.

  - TileRawChannel2Bytes4: this class converts the TileRawChannel object into bytes as it is defined at the commissioning 2007.


*/
