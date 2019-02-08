/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibHitmapTool.h
 * Header file for the SCT_CalibHitmapTool class
 * @author Shaun Roe
**/

#ifndef SCT_CalibHitmapTool_h
#define SCT_CalibHitmapTool_h

//STL includes
#include <string>

//Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"

//Inner detector includes
#include "InDetIdentifier/SCT_ID.h"
#include "InDetRawData/SCT_RDO_Container.h"

//local includes
#include "SCT_CalibAlgs/ISCT_CalibHistoTool.h"

//fwd declarations

class StatusCode;

//class SCT_CalibHitmapTool : virtual public ISCT_CalibHistoTool, public AthAlgTool
class SCT_CalibHitmapTool : public extends<AthAlgTool, ISCT_CalibHistoTool>
 {

   public:
      //@name Tool methods, reimplemented
      //@{
      SCT_CalibHitmapTool(const std::string&, const std::string&, const IInterface*);
      virtual ~SCT_CalibHitmapTool() = default;
      virtual StatusCode initialize();
      virtual StatusCode finalize();
      //interfaceID() implementation is in the baseclass
      virtual StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );
      //@}
      //@name ISCT_CalibHistoTool methods, reimplemented
      //@{
      virtual bool book();
      virtual bool read(const std::string & fileName);
      virtual bool fill(const bool fromData=false);
      virtual bool fillFromData();
      //virtual bool binHistograms(const int eventsPerWindow);
      //@}

   private:
      ServiceHandle<StoreGateSvc> m_detStore;
      ServiceHandle<StoreGateSvc> m_evtStore;

      const SCT_ID* m_pSCTHelper;
      SCT_ID::const_id_iterator m_waferItrBegin;
      SCT_ID::const_id_iterator m_waferItrEnd;
      typedef std::vector<int> VecInt;
      VecInt *m_sct_waferHash;
      VecInt *m_sct_firstStrip;
      VecInt *m_sct_rdoGroupSize;

      SG::ReadHandleKey<SCT_RDO_Container> m_rdoContainerKey;

};
#endif
