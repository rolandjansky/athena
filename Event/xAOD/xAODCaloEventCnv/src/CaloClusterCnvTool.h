// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterCnvTool.h 785752 2016-11-22 15:06:34Z ssnyder $
#ifndef XAODCALOEVENTCNV_CALOCLUSTERCNVTOOL_H
#define XAODCALOEVENTCNV_CALOCLUSTERCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODCaloEventCnv/ICaloClusterCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation of the ICaloClusterCnvTool interface
    *
    *         This tool holds the code doing the actual heavy lifting in
    *         converting an old CaloCluster object into an xAOD::CaloCluster
    *         one. It is used both by the package's main algorithm, and the
    *         trigger code when reading old BS files.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    * @author Walter Lampl <Walter.Lampl@cern.ch>
    *
    * $Revision: 785752 $
    * $Date: 2016-11-22 16:06:34 +0100 (Tue, 22 Nov 2016) $
    */
   class CaloClusterCnvTool : public AthAlgTool,
                              public virtual ICaloClusterCnvTool {

   public:
      /// Regular AlgTool constructor
      CaloClusterCnvTool( const std::string& type, const std::string& name,
                          const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize() override;

      /// Function that fill an existing xAOD::CaloCluster object with info
      virtual StatusCode convert( const CaloCluster* aod,
                                  xAOD::CaloCluster* xaod,
                                  CaloClusterCellLinkContainer* ccclc = 0 ) const override;

      /// Function usable by TrigBStoxAODTool
      virtual StatusCode convert( const CaloClusterContainer* aod,
                                  xAOD::CaloClusterContainer* xaod ) const override;


   }; // class CaloClusterCnvTool

} // namespace xAODMaker

#endif // XAODCALOEVENTCNV_CALOCLUSTERCNVTOOL_H
