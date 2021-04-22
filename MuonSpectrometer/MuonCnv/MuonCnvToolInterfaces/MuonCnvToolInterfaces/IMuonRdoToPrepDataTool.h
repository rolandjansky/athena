/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCNVTOOLINTERFACES_IMUONRDOTOPREPDATATOOL_H
#define MUONCNVTOOLINTERFACES_IMUONRDOTOPREPDATATOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>

class IdentifierHash;


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
      DeclareInterfaceID( IMuonRdoToPrepDataTool, 1, 0 );

    public:
      /** Decoding method. A vector of IdentifierHash are passed in, and the data corresponding to this list (i.e. in a Region of Interest) are converted.
      @param idVect          Vector of hashes to convert i.e. the hashes of ROD collections in a 'Region of Interest'
      @return selectedIdVect This is the subset of idVect which were actually found to contain data (i.e. if you want you can use
                             this vector of hashes to optimise the retrieval of data in subsequent steps.) */
      virtual StatusCode decode( std::vector<IdentifierHash>& idVect,  std::vector<IdentifierHash>& selectedIdVect) const = 0;

      //Method for ROB based decoding. Shold decode all chambers asociated with the inputted ROB list. 
      virtual StatusCode decode( const std::vector<uint32_t>& /*robIds*/ ) const {return StatusCode::FAILURE;}

      /** Method used for debugging*/ 
      virtual void printPrepData() const = 0;
      virtual void printInputRdo() const = 0;
};
}


#endif // !MUONCNVTOOLINTERFACES_IMUONRDOTOPREPDATATOOL_H
