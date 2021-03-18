/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/* Dear emacs, this is -*-c++-*- */
#ifndef _IDynamicDataConsumer_H_
#define _IDynamicDataConsumer_H_

#include <vector>
namespace Gaudi {
   class DataHandle;
}
class DataObjID;

class IDynamicDataConsumer
{
public:
   virtual ~IDynamicDataConsumer() {}
   /** Inform a dynamic data consumer about all consumed or produced data.
    * @param input_data_in the input data object IDs of all algorithms and tools
    * @param output_data_in the output data object IDs of all algorithms and tools
    * @param new_input_handles the new input data handles added by this call
    * @param new_output_handles the new output data handles added by this call
    * @return true if the consumer did update its handles, false otherwise
    */
   virtual bool updateDataNeeds(const std::vector<const DataObjID *> &input_data_in,
                                const std::vector<const DataObjID *> &output_data_in,
                                std::vector<Gaudi::DataHandle *> &new_input_handles,
                                std::vector<Gaudi::DataHandle *> &new_output_handles) = 0;
};
#endif
