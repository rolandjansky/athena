/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyGetPrepDataFromEventTool_H
#define MboyGetPrepDataFromEventTool_H

#include "AthenaBaseComps/AthAlgTool.h"

class ActiveStoreSvc;

namespace Trk {
  class  PrepRawData;
}
namespace Muon {
  class  CscPrepData;
  class  MdtPrepData;
  class  RpcPrepData;
  class  TgcPrepData;
  class  CscStripPrepData;
}

class AtlasDetectorID ;

#include "MboyEDMToolInterfaces/IMboyGetPrepDataFromEventTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

  /**
   @class MboyGetPrepDataFromEventTool

   This class is extracting PrepRawData vectors
   
  @author samusog@cern.ch
  
  */

class MboyGetPrepDataFromEventTool:public AthAlgTool, virtual public IMboyGetPrepDataFromEventTool{
public:
  MboyGetPrepDataFromEventTool(const std::string&,const std::string&,const IInterface*);
  virtual ~MboyGetPrepDataFromEventTool 	 ();

public:
///////////////////////////////////

  virtual StatusCode initialize	();
  virtual StatusCode finalize  	();

   /** Collect PrepRawData vectors */
  virtual StatusCode CollectPrepData      ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData );
  virtual StatusCode CollectPrepDataMDT   ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData );
  virtual StatusCode CollectPrepDataRPC   ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData );
  virtual StatusCode CollectPrepDataTGC   ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData );
  virtual StatusCode CollectPrepDataCSC   ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData );
  virtual StatusCode CollectPrepDataCSCClu( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData );

private:
///////////////////////////////////

   /** Collect PrepRawData vectors MDT*/
  StatusCode CollectPrepDataMDT_Method0( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData );
  StatusCode CollectPrepDataMDT_Method1( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData );

   /** Collect PrepRawData vectors RPC*/
  StatusCode CollectPrepDataRPC_Method0( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData );
  StatusCode CollectPrepDataRPC_Method1( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData );

   /** Collect PrepRawData vectors TGC*/
  StatusCode CollectPrepDataTGC_Method0( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData );
  StatusCode CollectPrepDataTGC_Method1( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData );

   /** Collect PrepRawData vectors CSC*/
  StatusCode CollectPrepDataCSC_Method0( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData );
  StatusCode CollectPrepDataCSC_Method1( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData );
                                         

  int m_CollectMDT        ; //!< Control Collection of Mdt        
  int m_CollectRPC        ; //!< Control Collection of Rpc       
  int m_CollectTGC        ; //!< Control Collection of Tgc       
  int m_CollectCSC        ; //!< Control Collection of Csc       
  int m_CollectCscCluster ; //!< Control Collection of Csc cluster

  int m_ContainerAccessMethodMDT ; //!< Control Access to containers of Mdt        
  int m_ContainerAccessMethodRPC ; //!< Control Access to containers of Rpc       
  int m_ContainerAccessMethodTGC ; //!< Control Access to containers of Tgc       
  int m_ContainerAccessMethodCSC ; //!< Control Access to containers of Csc       
  
  
  std::string m_MdtPrepDataContainerName         ; //!< Name of containers of Mdt
  std::string m_RpcPrepDataContainerName         ; //!< Name of containers of Rpc
   
  std::vector<std::string> m_TgcPrepDataContainerNameList; //!< Name of containers of Tgc

  bool m_TGC_use_prevBC, m_TGC_use_nextBC, m_TGC_use_currentBC;
  bool m_TGC_use_allBC;

  std::string m_CscPrepDataContainerName         ; //!< Name of containers of Csc
  std::string m_CscClustersPrepDataContainerName ; //!< Name of containers of Csc clusters

  ActiveStoreSvc* m_activeStore ; //!< Pointer On ActiveStoreSvc

  int m_DoDbg ; //!< Control debug print      
  const AtlasDetectorID* p_AtlasDetectorID ; //!< Pointer On AtlasDetectorID

};

#endif

