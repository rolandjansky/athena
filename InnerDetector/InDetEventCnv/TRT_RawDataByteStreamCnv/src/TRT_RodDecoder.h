/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_RAWDATABYTESTREAM_TRT_RODDECODER_H
#define TRT_RAWDATABYTESTREAM_TRT_RODDECODER_H


/*
 * Base class
 */
#include "AthenaBaseComps/AthAlgTool.h"  

/*
 * Interface class for this Tool
 */
#include "TRT_RawDataByteStreamCnv/ITRT_RodDecoder.h"

/*
 * TRT Tools we use
 */
#include "TRT_ConditionsServices/ITRT_ByteStream_ConditionsSvc.h"
#include "TRT_Cabling/ITRT_CablingSvc.h"

/*
 * Framework Headers
 *   Service & Tool Handles
 *   Incident notification
 */
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"


/* /\* */
/*  * Cool Headers */
/*  *\/ */
/* // COOL API include files (CoolKernel) */
/* #include "CoolKernel/DatabaseId.h" */
/* #include "CoolKernel/Exception.h" */
/* #include "CoolKernel/IDatabaseSvc.h" */
/* #include "CoolKernel/IDatabase.h" */
/* #include "CoolKernel/IFolder.h" */
/* #include "CoolKernel/IObject.h" */
/* #include "CoolKernel/IObjectIterator.h" */
/* #include "CoolKernel/Record.h" */
/* #include "CoolKernel/RecordSpecification.h" */
/* #include "CoolApplication/DatabaseSvcFactory.h" */
/* // COOL API include files (CoolApplication) */
/* #include "CoolApplication/Application.h" */

/* /\* */
/*  * Coral Headers */
/*  *\/ */
/* #include "CoralBase/Attribute.h" */

/*
 * Identifier
 */
#include "InDetIdentifier/TRT_ID.h"

/*
 * STL
 */
#include <vector>
#include <map>


using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

// the tool to decode a ROB frament

class TRT_RodDecoder : virtual public ITRT_RodDecoder, 
                       public AthAlgTool, virtual IIncidentListener
{

public: 

  //! AlgTool InterfaceID
  static const InterfaceID& interfaceID( ) ;

  //! constructor
  TRT_RodDecoder(const std::string& type, const std::string& name,
                 const IInterface* parent ) ;
  //! destructor 
  virtual ~TRT_RodDecoder(); 
  //! initialize
  virtual StatusCode initialize();
  //! finalize
  virtual StatusCode finalize();

  //! the method to fill the IDC
  StatusCode fillCollection ( const ROBFragment* robFrag,
			      TRT_RDO_Container* rdoIdc,
			      std::vector<IdentifierHash>* vecHash = 0);

  //! handle(), to get EventType on BeginRun incident.
  virtual void handle(const Incident& inc);

 private:

   ServiceHandle<ITRT_CablingSvc>   m_CablingSvc;

   ServiceHandle<ITRT_ByteStream_ConditionsSvc> m_bsErrSvc;
   ServiceHandle<IIncidentSvc> m_incsvc;

   /*
    * Do we look for Front-End Errors at all?
    */
   bool m_recordBSErrors;

   /*
    * Do we look for these types of Front-End Errors?
    */
   bool m_lookAtSidErrors;
   bool m_lookAtErrorErrors;
   bool m_lookAtL1idErrors;
   bool m_lookAtBcidErrors;
   bool m_lookAtMissingErrors;

   bool m_loadCompressTableFile;
   bool m_loadCompressTableDB;
   std::vector<int> m_LoadCompressTableVersions;
   std::string m_compressTableFolder;
   //   std::string m_compressTableFile;
   const int m_maxCompressionVersion;
   //   bool m_compressTableLoaded[16];
   bool m_compressTableLoaded[256];
   int m_forceRodVersion;

   const TRT_ID*               m_trt_id;   
   IdContext                   m_straw_layer_context;

   BooleanProperty m_TB04_RawData;   // true to create TRT_TB04_RawData RDOs
   BooleanProperty m_LoLumRawData;   // true to create TRT_LoLumRawData RDOs

   bool m_eventTypeIsSim;            // look at first event to decide if RODBlockVersion
                                    // is incorrect.

   uint32_t m_escape_marker;           // Straw word that means escaped literal

#define CTABLE_FC_LENGTH 33
#define CTABLE_LI_LENGTH 33
   typedef struct {
     int m_TableVersion;
     int m_firstcode[CTABLE_FC_LENGTH];
     int m_lengths_integral[CTABLE_FC_LENGTH];    // ..[i] = Sum(numl[0,i-1])
     unsigned int *m_syms;              // Array of symbols (straw data words)
     int m_Nsymbols;
   } t_CompressTable;

   //   t_CompressTable m_CTable;
   //   t_CompressTable *m_CompressionTables[16];
   std::map<const int, t_CompressTable *>m_CompressionTables;

   uint32_t m_Nrdos;              // Number of RDOs created

   //! private methods
private:

   StatusCode int_fillExpanded ( const ROBFragment* robFrag,
				TRT_RDO_Container* rodIdc,
				std::vector<IdentifierHash>* vecHash = 0);

   StatusCode int_fillMinimalCompress ( const ROBFragment* robFrag,
				       TRT_RDO_Container* rdoIdo,
				       std::vector<IdentifierHash>* vecHash = 0);

   StatusCode int_fillFullCompress ( const ROBFragment* robFrag,
				     TRT_RDO_Container* rdoIdo,
				     t_CompressTable* Ctable,
				     std::vector<IdentifierHash>* vecHash = 0);

   StatusCode ReadCompressTableFile( std::string TableFilename );
   StatusCode ReadCompressTableDB( std::string Tag );
   StatusCode update( IOVSVC_CALLBACK_ARGS_P(I,keys) );
 
};

#endif
