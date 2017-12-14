/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGISTRATIONSERVICES_INPUTCMD_H
#define REGISTRATIONSERVICES_INPUTCMD_H

/** @file InputCollectionMetadataCopy.h
 *  @brief This file contains the class definition for the InputCollectionMetadataCopy class.
 *  @author Jack Cranshaw <cranshaw@anl.gov>
 *  $Id: InputCollectionMetadataCopy.h,v 1.1 2009-03-04 17:00:45 cranshaw Exp $
 **/

#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaPoolKernel/IMetaDataTool.h"

class StoreGateSvc;

/** @class AthPoolEx::InputCollectionMetadataCopy
 *  @brief This class provides an example for reading in file meta data objects from Pool.
 **/
class InputCollectionMetadataCopy : public AthAlgTool, 
                                    virtual public IIncidentListener 
{
public: 
   /// Standard Service Constructor
   InputCollectionMetadataCopy(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~InputCollectionMetadataCopy();

   /// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode finalize();

   /// Incident service handle listening for BeginInputFile and EndInputFile.
   void handle(const Incident& incident);

private:
   typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
   StoreGateSvc_t m_pMetaDataStore;
   StoreGateSvc_t m_pInputStore;
};

#endif
