/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMPARAMETRIZATION_FASTCALOSIMGEOMETRYHELPER_H
#define ISF_FASTCALOSIMPARAMETRIZATION_FASTCALOSIMGEOMETRYHELPER_H

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

#include "ISF_FastCaloSimParametrization/CaloGeometry.h"
#include "ISF_FastCaloSimParametrization/IFastCaloSimGeometryHelper.h"

class CaloDetDescrManager;

class FastCaloSimGeometryHelper:public AthAlgTool, public CaloGeometry, virtual public IFastCaloSimGeometryHelper {
 public :
  /** Constructor with parameters */
  FastCaloSimGeometryHelper( const std::string& t, const std::string& n, const IInterface* p );

  /** Destructor */
  virtual ~FastCaloSimGeometryHelper();

  // Athena algtool's Hooks
  virtual StatusCode  initialize() override final;
  virtual StatusCode  finalize() override final;

 private:
  /// DetDescr mgr for access to the calo helper
  const CaloDetDescrManager* m_caloMgr{nullptr};
    
  bool LoadGeometryFromCaloDDM();
};

#endif

