/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_CALOCELLTHINNINGTOOL_H
#define DERIVATIONFRAMEWORK_CALOCELLTHINNINGTOOL_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"

class TileID;
class IThinningSvc;

namespace DerivationFramework {

  class CaloCellThinningTool : public AthAlgTool, 
                               public IThinningTool {

  public:
    CaloCellThinningTool(const std::string& t, 
                         const std::string& n, 
                         const IInterface* p );
    ~CaloCellThinningTool();

    StatusCode  initialize();
    StatusCode  finalize();
    virtual StatusCode doThinning() const;

  private:
    ServiceHandle<IThinningSvc> m_thinningSvc;
    double m_caloCellId;
  }; 
}

#endif
