/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "AthenaBaseComps/DynamicDataHelper.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IClassIDSvc.h"

#include <string_view>
#include <sstream>


namespace Ath {
   namespace {
      void insertHandles(DataObjIDPtrColl &handle_set, const std::vector<Gaudi::DataHandle*> &new_handles) {
         for( const Gaudi::DataHandle*handle : new_handles) {
            handle_set.insert( &handle->fullKey() );
         }
      }

      bool msgLvl(const MsgStream &out, const MSG::Level level) {
         return level<= out.level();
      }
   }

   StatusCode DynamicDataHelper::addExtraDependencies(IClassIDSvc &clid_svc, std::vector<std::string> &undeclared_output_data, MsgStream &out) {
      std::stringstream error;
      if (!m_extraOutputIDs.empty()) {
         out << MSG::FATAL << "Logic erroor. Already added some extra output data IDs. This method can only be called once." << endmsg;
         return StatusCode::FAILURE;
      }
      m_extraOutputIDs.reserve( undeclared_output_data.size() );
      for(const std::string &type_and_name :  undeclared_output_data) {
         std::string::size_type pos = type_and_name.find("/");
         if (pos == std::string::npos) {
            error << " " << type_and_name;
         }
         else {
            std::string_view type_name(type_and_name.data(), pos);
            std::string_view var_name(&(type_and_name.data()[pos+1]), type_and_name.size()-pos-1);
            CLID clid;
            if (clid_svc.getIDOfTypeName(std::string(type_name), clid).isFailure() ) {
               // out << MSG::FATAL << " Failed to get CLID for " << type_name << endmsg;
               // return StatusCode::FAILURE;
               error << " no clid: "  << type_name;
            }
            else {
               m_extraOutputIDs.push_back( DataObjID(clid, std::string(var_name)) );
               if (msgLvl(out,MSG::VERBOSE)) {
                  out << MSG::VERBOSE << "DEBUG add extra dependency " << type_name << " / " << var_name << " -> " << clid << endmsg;
               }
            }
         }
      }
      if (!error.str().empty()) {
         out << MSG::FATAL << "Invaid alements in property " << error.str().c_str() << endmsg;
         return StatusCode::FAILURE;
      }
      for (const DataObjID &id : m_extraOutputIDs ) {
         m_outputHandles.insert( &id );
      }
      return StatusCode::SUCCESS;
   }

   void DynamicDataHelper::gatherDataHandlesAndDynamicConsumers(const std::string &/*parent_name*/, Gaudi::Algorithm *theAlgorithm) {
         //      |> debugging
         auto algorithm_visitor =
            [this](Gaudi::Algorithm *an_alg) {
               IDynamicDataConsumer *a_dyn_data_consumer=dynamic_cast<IDynamicDataConsumer *>(an_alg);
               if (a_dyn_data_consumer) {
                  m_dynDataConsumer.push_back(std::make_pair(a_dyn_data_consumer, an_alg));
               }
               // have to collect input and output handles because the sequence will only collect them  after initialize
               insertHandles(m_inputHandles, an_alg->inputHandles());
               insertHandles(m_outputHandles, an_alg->outputHandles());
               std::vector<IAlgTool *> &tools ATLAS_THREAD_SAFE
                  = const_cast< std::vector<IAlgTool *> &>(const_cast<const Gaudi::Algorithm *>(an_alg)->tools());
               //                               |> debugging
               auto tool_visitor = [this, an_alg](IAlgTool *alg_tool) {
                  IDynamicDataConsumer *a_dyn_data_consumer=dynamic_cast<IDynamicDataConsumer *>(alg_tool);
                  if (a_dyn_data_consumer) {
                     m_dynDataConsumer.push_back(std::make_pair(a_dyn_data_consumer, an_alg));
                  }
                  if (auto* tool_impl = dynamic_cast<AlgTool*>( alg_tool ); tool_impl) {
                     insertHandles(m_inputHandles, tool_impl->inputHandles());
                     insertHandles(m_outputHandles, tool_impl->outputHandles());
                  }
               };
               xGaudi::ToolVisitor::visit( tools,tool_visitor);
            };
         xGaudi::AlgorithmVisitor::visit({theAlgorithm},algorithm_visitor);
   }

   void DynamicDataHelper::updateDataNeeds(unsigned int max_pass,
                                           MsgStream &out) {
      if (msgLvl(out,MSG::VERBOSE)) out << MSG::VERBOSE << "Allow dynamic data consumers to update their data dependencies." << endmsg;
      std::vector<const DataObjID *> all_input_data;
      std::vector<const DataObjID *> all_output_data;
      all_input_data.insert( all_input_data.end(), m_inputHandles.begin(), m_inputHandles.end());
      all_output_data.insert( all_output_data.end(), m_outputHandles.begin(), m_outputHandles.end());

      std::vector<const DataObjID *> new_input_data;
      std::vector<const DataObjID *> new_output_data;
      unsigned int pass_i=max_pass;
      unsigned last_out_size=new_output_data.size();
      for (; --pass_i>0 && updateDataNeeds(all_input_data, all_output_data, new_input_data, new_output_data,out); ) {
         if (last_out_size == new_output_data.size() ) break;
         last_out_size=new_output_data.size();
      };
      if (last_out_size != new_output_data.size()) {
         out << MSG::WARNING << "Not iterating any further, although after " << max_pass << " iterations there are still algorithms which updated their data needs. " << endmsg;
      }
   }

   bool DynamicDataHelper::updateDataNeeds(const std::vector<const DataObjID *> &input_data_in,
                                           const std::vector<const DataObjID *> &output_data_in,
                                           std::vector<const DataObjID *> &input_data_out,
                                           std::vector<const DataObjID *> &output_data_out,
                                           MsgStream &out) {
      bool updated=false;
      if (msgLvl(out,MSG::VERBOSE)) {
         out<< MSG::VERBOSE << " inputs:";
         for (const DataObjID *id : input_data_in) {
            out << " " << *id ;
         }
         out << endmsg;
         out << MSG::VERBOSE << " outputs:";
         for (const DataObjID *id : output_data_in) {
            out << " " << *id ;
         }
         out << endmsg;
      }
      for (std::pair<IDynamicDataConsumer *,Gaudi::Algorithm *> &dyn_data_consumer :  m_dynDataConsumer ) {
         std::vector<Gaudi::DataHandle *> tmp_new_input_handles;
         std::vector<Gaudi::DataHandle *> tmp_new_output_handles;
         bool ret = dyn_data_consumer.first->updateDataNeeds(input_data_in, output_data_in, tmp_new_input_handles, tmp_new_output_handles);
         if (msgLvl(out,MSG::VERBOSE)) {
            out << MSG::VERBOSE << " new data from " << dyn_data_consumer.second->name()
                << " i:" << tmp_new_input_handles.size()
                << " o:" << tmp_new_output_handles.size()
                << endmsg;
         }
         updated |=  ret;
         if (ret) {
            if (msgLvl(out,MSG::DEBUG)) {
               if (Gaudi::Algorithm *an_alg = dynamic_cast<Gaudi::Algorithm *>(dyn_data_consumer.first); an_alg) {
                  out << MSG::DEBUG << an_alg->name() << " updated its data needs." << endmsg;
                  // DEBUG:
                  out << MSG::DEBUG << an_alg->name() << " inputs:";
                  for (const Gaudi::DataHandle *handle : an_alg->inputHandles()) {
                     out << " " << handle->fullKey();
                  }
                  out << endmsg;
               }
               else if (AlgTool *alg_tool = dynamic_cast<AlgTool *>(dyn_data_consumer.first); alg_tool) {
                  out << MSG::DEBUG << alg_tool->name() << " updated its data needs." << endmsg;
                  // DEBUG:
                  out << MSG::DEBUG << alg_tool->name() << " inputs:";
                  for (const Gaudi::DataHandle *handle : alg_tool->inputHandles()) {
                     out << " " << handle->fullKey();
                  }
                  out << endmsg;
               }
               else {
                  out << MSG::WARNING << "Unknown dynamic data consumer type " << typeid(dyn_data_consumer.first).name() << endmsg;
               }
            }
         }
         Gaudi::Algorithm *parent_alg = dyn_data_consumer.second;
         input_data_out.reserve(input_data_out.size()+tmp_new_input_handles.size());
         for (Gaudi::DataHandle *a_handle : tmp_new_input_handles) {
            parent_alg->addDependency(a_handle->fullKey(), Gaudi::DataHandle::Reader);
            input_data_out.push_back(&(a_handle->fullKey()));
            if (msgLvl(out,MSG::DEBUG)) out << MSG::DEBUG << " addDependency new input for " << parent_alg->name() << " "
                                            << a_handle->fullKey().className() << " / " <<  a_handle->objKey() << endmsg;
         }
         output_data_out.reserve(output_data_out.size()+tmp_new_output_handles.size());
         for (Gaudi::DataHandle *a_handle : tmp_new_output_handles) {
            parent_alg->addDependency(a_handle->fullKey(), Gaudi::DataHandle::Writer);
            output_data_out.push_back(&(a_handle->fullKey()));
            if (msgLvl(out,MSG::DEBUG)) out << MSG::DEBUG << " addDependency new output for " << parent_alg->name() << " "
                                            << a_handle->fullKey().className() << " / "  << a_handle->objKey() << endmsg;
         }
      }
      return updated;
   }
}
