/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/* Dear emacs, this is -*-c++-*- */
#ifndef _DynamicDataHelper_H_
#define _DynamicDataHelper_H_

#include "AthenaBaseComps/GaudiComponentVisitor.h"
#include "AthenaBaseComps/IDynamicDataConsumer.h"
#include "GaudiKernel/StatusCode.h"

// @TODO extend Gaudi's tool visitor to allow for an externally provided list of tools which have been visited already.
namespace Ath {

   struct DataObjID_PtrHasher {
      std::size_t operator()( const DataObjID* k ) const { return DataObjID_Hasher()(*k); }
   };
   struct DataObjID_PtrEqual {
      std::size_t operator()( const DataObjID* a, const DataObjID* b) const { return *a == *b; }
   };
   using DataObjIDPtrColl = std::unordered_set<const DataObjID *, DataObjID_PtrHasher, DataObjID_PtrEqual>;

   /** Helper class to gather all declared data IDs and propagate them to components which declare data dependencies dynamically
    */
   class DynamicDataHelper {
   public:
      /** Gather the input and output data declared by the given algorithm, its child algorithms and their tools.
       * @param parent_name the name of the parent component used in messsages
       * @param theAlgorithm the top algorithm to start the collection
       * Additionally also all components are collected which dynamically declare their
       * data dependencies.
       */
      void gatherDataHandlesAndDynamicConsumers(const std::string &parent_name, Gaudi::Algorithm *theAlgorithm);

      /** Add extra output data which is not declared by any of the gathered components.
       * @param clid_svc the ClassIDSvc needed to translate the type name into a CLID.
       * @param undeclared_output_data strings which are composed of the type and variable name separated by "/".
       * @param out the message output stream used for error messages.
       */
      StatusCode addExtraDependencies(IClassIDSvc& clid_svc, std::vector<std::string> &undeclared_output_data, MsgStream &out);

      /** Update the data dependencies of all components which dynamically declare tehm
       * @param max_pass maximum number of iterations
       * @param out the message stream of the parent component
       * The update happens iteratively until non of the components declares new output data, but at most max_pass
       * times.
       */
      void updateDataNeeds(unsigned int max_pass,
                           MsgStream &out);

   protected:
      /** Update the data dependencies of all components once.
       * @param input_data_in IDs of all declared input data
       * @param output_data_in IDs of all declared output data
       * @param input_data_out IDs of newly declared input data in this iteration
       * @param output_data_out IDs of newly declared input data in this iteration
       * @return true if new input or output data was declared.
       */
      bool updateDataNeeds(const std::vector<const DataObjID *> &input_data_in,
                           const std::vector<const DataObjID *> &output_data_in,
                           std::vector<const DataObjID *> &input_data_out,
                           std::vector<const DataObjID *> &output_data_out,
                           MsgStream &out);

      std::vector<DataObjID> m_extraOutputIDs;
      DataObjIDPtrColl m_inputHandles;
      DataObjIDPtrColl m_outputHandles;

      std::vector< std::pair<IDynamicDataConsumer *,Gaudi::Algorithm *> > m_dynDataConsumer;

   };
}
#endif
