/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_CalibBsErrorTool.h
* Header file for the SCT_CalibBsErrorTool class
* @author Shaun Roe
**/

#ifndef SCT_CalibBsErrorTool_h
#define SCT_CalibBsErrorTool_h


//STL includes
#include <string>

//Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetIdentifier/SCT_ID.h"

#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"

//local includes
#include "SCT_CalibAlgs/ISCT_CalibHistoTool.h"

//fwd declarations

class StatusCode;
class ISvcLocator;

class SCT_CalibBsErrorTool : public extends<AthAlgTool, ISCT_CalibHistoTool>
{

   public:
      //@name Tool methods, reimplemented
      //@{
      SCT_CalibBsErrorTool(const std::string&, const std::string&, const IInterface*);
      virtual ~SCT_CalibBsErrorTool() = default;
      virtual StatusCode initialize();
      virtual StatusCode finalize();
      //interfaceID() implementation is in the baseclass
      virtual StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );
      //@}
      //@name ISCT_CalibHistoSvc methods, reimplemented
      //@{
      virtual bool book();
      virtual bool read(const std::string & fileName);
      virtual bool fill(const bool fromData=false);
      virtual bool fillFromData();
      //@}

   private:
      ServiceHandle<StoreGateSvc> m_detStore;
      ServiceHandle<StoreGateSvc> m_evtStore;
      ToolHandle<ISCT_ByteStreamErrorsTool> m_bytestreamErrorsTool{this, "SCT_ByteStreamErrorsTool", "SCT_ByteStreamErrorsTool", "Tool to retrieve SCT ByteStream Errors"};
      const SCT_ID* m_pSCTHelper;

      // SCT specific numbers
      SCT_ID::const_id_iterator m_waferItrBegin;
      SCT_ID::const_id_iterator m_waferItrEnd;
      typedef std::vector<int> VecInt;
      VecInt *m_scterr_bec;
      VecInt *m_scterr_layer;
      VecInt *m_scterr_eta;
      VecInt *m_scterr_phi;
      VecInt *m_scterr_side;
      VecInt *m_scterr_type;
      int m_maxHash;

      void
      fillBsErrorsForWafer(const Identifier & waferId, const int type);

};
#endif
