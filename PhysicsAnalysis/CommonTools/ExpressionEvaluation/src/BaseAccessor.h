/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ExpressionParsing_BaseAccessor_h_
#define ExpressionParsing_BaseAccessor_h_

#include <stdexcept>
#include <sstream>
#include "ExpressionEvaluation/IProxyLoader.h"
#include "ExpressionEvaluation/IAccessor.h"

namespace ExpressionParsing {

   /** Base class of xAOD object content accessors.
    */
   class BaseAccessor : public IAccessor {
   public:
      BaseAccessor(IProxyLoader::VariableType variable_type) : m_variableType(variable_type) {}

      static void throwInvalidHandle(const std::string &key) {
         std::stringstream msg;
         msg << "Failed to create read handle for " << key;
         throw std::runtime_error(key);
      }
      static void throwVectorContainsNotOneElement(const std::string &key, std::size_t n_elements) {
         std::stringstream msg;
         msg << "Cannot convert to scalar since the aux vector data container " << key << " does not contain exactly one element but " << n_elements;
         throw std::runtime_error(key);
      }
      virtual IProxyLoader::VariableType variableType() const override {
         return m_variableType;
      }

   private:
      IProxyLoader::VariableType m_variableType;
   };

   /** Special accessor to handle empty containers until the correct accessor can be created.
    */
   class EmptyVectorAccessor : public IAccessor {
   public :
      virtual IProxyLoader::VariableType variableType() const override { return IProxyLoader::VT_VECEMPTY; }
      virtual int loadInt(const EventContext& ctx) const { (void) ctx; throwEmptyVector(); return 0; }
      virtual double loadDouble(const EventContext& ctx) const { (void) ctx; throwEmptyVector(); return 0.;}
      virtual std::vector<int> loadVecInt(const EventContext& ctx) const { (void) ctx; return std::vector<int>(); }
      virtual std::vector<double> loadVec(const EventContext& ctx) const { (void) ctx; return std::vector<double>(); }
   private:
      void throwEmptyVector() const {
         throw std::runtime_error("Attempt to convert empty vector into scalar.");
      }
   };

}
#endif
