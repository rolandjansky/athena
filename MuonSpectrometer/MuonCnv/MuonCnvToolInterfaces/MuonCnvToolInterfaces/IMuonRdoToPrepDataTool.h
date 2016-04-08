/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCNVTOOLINTERFACES_IMUONRDOTOPREPDATATOOL_H
#define MUONCNVTOOLINTERFACES_IMUONRDOTOPREPDATATOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>

class IdentifierHash;

static const InterfaceID IID_IMuonRdoToPrepDataTool( "Muon::IMuonRdoToPrepDataTool", 1, 0 );

namespace Muon {

/**Interface defining the tools used to convert Muon RDOs to MuonPrepRawData.
For the concrete implementations look at e.g. 
- Muon::MdtRdoToPrepDataTool
- Muon::CscRdoToPrepDataTool
- Muon::TgcRdoToPrepDataTool
- Muon::RpcRdoToPrepDataTool
*/
class IMuonRdoToPrepDataTool : virtual public IAlgTool {
    public:
      static const InterfaceID& interfaceID() {return IID_IMuonRdoToPrepDataTool;};

    public:
      /** Decoding method. A vector of IdentifierHash are passed in, and the data corresponding to this list (i.e. in a Region of Interest) are converted.
      @param idVect          Vector of hashes to convert i.e. the hashes of ROD collections in a 'Region of Interest'
      @return selectedIdVect This is the subset of idVect which were actually found to contain data (i.e. if you want you can use
                             this vector of hashes to optimise the retrieval of data in subsequent steps.) */
      virtual StatusCode decode( std::vector<IdentifierHash>& idVect,  std::vector<IdentifierHash>& selectedIdVect) = 0;

      //Method for ROB based decoding. Shold decode all chambers asociated with the inputted ROB list. 
      virtual StatusCode decode( const std::vector<uint32_t>& /*robIds*/ ) {return StatusCode::FAILURE;}

      /** Method used for debugging*/ 
      virtual void printPrepData() = 0;
      virtual void printInputRdo() = 0;
};
}


#endif // !MUONCNVTOOLINTERFACES_IMUONRDOTOPREPDATATOOL_H
