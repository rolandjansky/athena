/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVCBASE_BYTESTREAMCNVSVCBASE_H
#define BYTESTREAMCNVSVCBASE_BYTESTREAMCNVSVCBASE_H

#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthCnvSvc.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamData/RawEvent.h"

/**
 * @class ByteStreamCnvSvcBase
 * @brief The base class for offline and HLT ByteStream conversion services
 */
class ByteStreamCnvSvcBase : public ::AthCnvSvc,
		public virtual IIncidentListener,
		public virtual IByteStreamEventAccess {

public:
   /// Standard Service Constructor
   ByteStreamCnvSvcBase(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor

   virtual ~ByteStreamCnvSvcBase();
   /// Required of all Gaudi Services
   virtual StatusCode initialize() override;

   /// Required of all Gaudi services:  see Gaudi documentation for details
   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

   /// Checks whether an IOpaqueAddress is a GenericAddress
   virtual StatusCode updateServiceState(IOpaqueAddress* pAddress) override;

   /// Implementation of IIncidentListener: Handle for EndEvent incidence
   virtual void handle(const Incident&) override;

protected: // data
   std::vector<std::string> m_initCnvs;
   // This property is used by Tile BS converter, not by this class.
   Gaudi::Property<std::vector<std::string> > m_ROD2ROBmap{this,"ROD2ROBmap",{},"","OrderedSet<std::string>"};

private:
   /** @name Flags which are not used by this service.
    *  It only serves as an entity storing these values and some other components read it from here.
    *  TODO: Remove this solution and move the properties where they are needed.
    */
   ///@{
   /// flags for Simulation EventType
   Gaudi::Property<bool> m_isSimulation{this, "IsSimulation", false};
   /// flags for TestBeam EventType
   Gaudi::Property<bool> m_isTestbeam{this, "IsTestbeam", false};
   /// flags for Calibration EventType
   Gaudi::Property<bool> m_isCalibration{this, "IsCalibration", false};
   ///@}
};

#endif
