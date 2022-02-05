/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECORATOR_ALG_H
#define DECORATOR_ALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"

namespace FlavorTagDiscriminants {
  template <typename CONTAINER, typename DECORATOR, typename CONSTITUENTS>
  class DecoratorAlg : public AthReentrantAlgorithm
  {
  public:
    using ExtraDependencies = std::set<std::string>;
    DecoratorAlg(const std::string& name, ISvcLocator* svcloc);
    virtual StatusCode initialize() override;
    virtual StatusCode initializeInternal(ExtraDependencies = {});
    virtual StatusCode execute(const EventContext& cxt) const override;
    virtual StatusCode finalize() override;
  protected:
    SG::ReadHandleKey<CONTAINER> m_containerKey {
      this, "container", "", "Key for the input collection"};
    SG::ReadHandleKey<CONSTITUENTS> m_constituentKey {
      this, "constituentContainer", "",
        "Key for track inputs container"};
    Gaudi::Property<std::vector<std::string>> m_undeclaredReadDecorKeys {
      this, "undeclaredReadDecorKeys", {},
      "List of read handles that we don't read, e.g. static variables" };

    ToolHandle<DECORATOR> m_decorator{
      this, "decorator", "", "Decorator tool"};

    // Keys to keep track of the inputs / outputs
    std::vector<SG::ReadDecorHandleKey<CONTAINER>> m_aux;
    std::vector<SG::ReadDecorHandleKey<CONSTITUENTS>> m_constituentAux;
    std::vector<SG::WriteDecorHandleKey<CONTAINER>> m_decor;
  };
}

#include "DecoratorAlg.icc"

#endif
