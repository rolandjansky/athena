/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 MuonByteStreamErrors package
 MdtByteStreamErrorContainer class
 -----------------------------------------
 ***************************************************************************/


#ifndef MUONBYTESTREAMERRORS_MDTBYTESTREAMERRORCONTAINER_H
#define MUONBYTESTREAMERRORS_MDTBYTESTREAMERRORCONTAINER_H


#include "Identifier/IdentifierHash.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"


namespace Muon {
    enum MdtBSerrorTypes {    
    CSMHeaderErrors,
    CSMSubHeaderErrors,
    Mdt_NUM_ERROR_TYPES  // always have this one last, so we can use it as a loop index
  };

  class MdtByteStreamErrorContainer 
  {
  public: 
    MdtByteStreamErrorContainer();
    MdtByteStreamErrorContainer( const DataVector<std::pair<IdentifierHash, int> > & errorVector );
    ~MdtByteStreamErrorContainer();

    void addError(IdentifierHash, int); 
    const DataVector<std::pair<IdentifierHash, int> >& getAllErrors() const;
  private:
    DataVector<std::pair<IdentifierHash, int> > m_errorVector; 
  };

}

CLASS_DEF( Muon::MdtByteStreamErrorContainer , 1143400055 , 1 )
#endif // MUONBYTESTREAMERRORS_MDTBYTESTREAMERRORCONTAINER_H

