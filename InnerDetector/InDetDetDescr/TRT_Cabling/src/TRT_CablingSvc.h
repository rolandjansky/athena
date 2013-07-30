/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// TRT_Cabling class - interface class for TRT Mapping
//

#ifndef TRT_CABLINGSVC_H
#define TRT_CABLINGSVC_H

#include "TRT_Cabling/ITRT_CablingSvc.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/IdContext.h"

#include "TRT_CablingData.h"
#include "TRT_FillCablingData_SR1_ECC.h"
#include "TRT_FillCablingData_SR1.h"
#include "TRT_FillCablingData_TB04.h"
#include "TRT_FillCablingData_DC1.h"
#include "TRT_FillCablingData_DC2.h"
#include "TRT_FillCablingData_DC3.h"

#include "InDetReadoutGeometry/TRT_DetectorManager.h" 
#include "InDetIdentifier/TRT_ID.h"

#include "AthenaBaseComps/AthService.h"

#include "eformat/SourceIdentifier.h"

#include <stdint.h> 


class TRT_CablingSvc : virtual public ITRT_CablingSvc, public AthService
{

public:

    // Constructor
  TRT_CablingSvc( const std::string& name, ISvcLocator *pSvcLocator ) ;

    // Destructor	
  virtual ~TRT_CablingSvc();


  virtual StatusCode initialize();

  virtual StatusCode finalize();

  virtual StatusCode queryInterface(const InterfaceID & riid, 
				    void** ppvInterface );


    // Get Identifier for each straw from BS
//  Identifier getIdentifier(const EventFormat::SubDetector& subdetector,
   Identifier getIdentifier(const eformat::SubDetector& subdetector,
    const unsigned& rod, const int& bufferOffset, IdentifierHash& hashId) const;  

   // Get buffer offset from staw Identifier
   uint32_t getBufferOffset( const Identifier &StrawId );

    // Get ROBIDs for each Detector Element (Straw Layer)
  std::vector<uint32_t> getRobID(Identifier& id) const;

    // Get all RODIDs for TRT
  const std::vector<uint32_t>& getAllRods() const;
  

private:

  const InDetDD::TRT_DetectorManager *m_manager; 
  const TRT_ID* m_idHelper;
  IdContext m_cntx;
  TRT_CablingData* m_cabling;

  TRT_FillCablingData_SR1_ECC* m_cablingTool_SR1_ECC;
  TRT_FillCablingData_SR1* m_cablingTool_SR1;
  TRT_FillCablingData_TB04* m_cablingTool_TB;
  TRT_FillCablingData_DC1* m_cablingTool_DC1;
  TRT_FillCablingData_DC2* m_cablingTool_DC2;
  TRT_FillCablingData_DC3* m_cablingTool_DC3;
  int m_TRTLayout;  

    // Parameters for DC1
  int m_numberOfStrawsInROD;
  int m_numberOfStrawsInBarrelROD;
  int m_shiftForLeftEndCapStraws;
  int m_shiftForLeftBarrelStraws;
  int m_shiftForRightBarrelStraws;
  int m_shiftForRightEndCapStraws;
  //  int m_numberOfEndCapPhiSectors;
  //  int m_numberOfIdentifierSectors;
};

#endif     // TRT_CABLINGSVC_H
