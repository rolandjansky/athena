/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_EMPTYJETGROUPPRODUCT_H
#define TRIGHLTJETHYPO_EMPTYJETGROUPPRODUCT_H

#include "./IJetGroupProduct.h"
#include <vector>

class EmptyJetGroupProduct: public IJetGroupProduct{
  /*
   * Return an empty vector of jet indices.
   * The need for such a vector may arise for a number of reasons
   * for when example no jet group satisfies a Condition.
   */
public:
  EmptyJetGroupProduct(){}
  
  virtual std::vector<std::size_t> next(const Collector&collector) override {
    if(collector){
      collector->collect("EmptyJetGroup::next()",
                         "end of iteration ");
    }
    return std::vector<std::size_t>();
  }
  virtual bool valid() const override {return true;}
  
};

#endif
