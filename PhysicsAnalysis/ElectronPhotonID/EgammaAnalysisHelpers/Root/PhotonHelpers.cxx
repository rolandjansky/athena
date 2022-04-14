/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "EgammaAnalysisHelpers/PhotonHelpers.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/Photon.h"

// ==================================================================
bool
PhotonHelpers::passOQquality(const xAOD::Photon& ph)
{

  return !((ph.OQ() & 1073741824) != 0 ||

           ((ph.OQ() & 134217728) != 0 &&

            (ph.showerShapeValue(xAOD::EgammaParameters::Reta) > 0.98

             || ph.showerShapeValue(xAOD::EgammaParameters::f1) > 0.4

             || (ph.OQ() & 67108864) != 0)

              ));
}
// ==================================================================
bool
PhotonHelpers::passOQqualityDelayed(const xAOD::Photon& ph)
{

  return !((ph.OQ() & 1073741824) != 0 ||

           ((ph.OQ() & 134217728) != 0 &&

            (ph.showerShapeValue(xAOD::EgammaParameters::Reta) > 0.98

             || ph.showerShapeValue(xAOD::EgammaParameters::f1) > 0.4

             )));
}
