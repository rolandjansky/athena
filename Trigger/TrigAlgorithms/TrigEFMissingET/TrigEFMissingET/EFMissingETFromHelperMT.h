/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMISSINGETFROMHELPERMT_H
#define TRIGEFMISSINGET_EFMISSINGETFROMHELPERMT_H

/********************************************************************

NAME:     EFMissingETFromHelperMT.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Gabriel Gallardo, Manfredi Ronzani
CREATED:  September 27, 2018

PURPOSE:  Updates TrigMissingET using TrigMissingETHelper info for AthenaMT

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETBaseTool.h"
//#include "TrigCaloEvent/TrigMissingET.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigEFMissingET/IMissingETTool.h"


#include <vector>
//#include "TH2F.h"

/**
  $class EFMissingETFromHelper
  - Saves transient information in the persistent object,
  applying calibration only to global properties.
  - Fills component-level monitoring histograms
 **/

class EFMissingETFromHelperMT : public extends<EFMissingETBaseTool, IMissingETTool>
{
  public:

    EFMissingETFromHelperMT(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

    virtual ~EFMissingETFromHelperMT();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual StatusCode executeMT(xAOD::TrigMissingET *met,
                               TrigEFMissingEtHelper *metHelper) const;

    virtual StatusCode update( xAOD::TrigMissingET *met,
           TrigEFMissingEtHelper *metHelper ) const override;
};

#endif // TRIGEFMISSINGET_EFMISSINGETFROMHELPERMT
