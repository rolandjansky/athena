/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   MonGroupBuilder
 *
 * @brief Class to help with building monitoring groups
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_MONGROUPBUILDER_H
#define TRIGEFMISSINGET_MONGROUPBUILDER_H 1

#include "AthenaMonitoringKernel/MonitoredGroup.h"
#include <type_traits>
#include <memory>


namespace HLT { namespace MET {
  /****************************************************************************
  * @class MonGroupBuilder
  *
  * Class allowing incremental building of Monitored::Groups.
  *
  * Currently, it's only possible to load all members of a single
  * Monitored::Group simultaneously, i.e. it's not possible to build up the
  * group a few members at a time. However in these algorithms I found times
  * where I wanted to monitor several variables at the same time, but trigger
  * these from different locations.
  ***************************************************************************/
  class MonGroupBuilder {
    public:
      /**
       * @brief Add a new monitored variable
       * @param variable The variable to monitor
       *
       * This overload will not take ownership of the variable
       */
      void add(Monitored::IMonitoredVariable& variable);

      /**
       * @brief Add a new monitored variable
       * @tparam T The type of monitored variable
       * @param variable The variable to monitor
       *
       * This overload will take ownership of the variable
       */
      template <typename T,
                typename = std::enable_if_t<
                  !std::is_lvalue_reference_v<T> &&
                  std::is_base_of_v<Monitored::IMonitoredVariable, T>>>
        void add(T&& variable)
        {
          m_ptrs.push_back(std::make_unique<T>(std::move(variable) ) );
          add(*m_ptrs.back() );
        }

      /**
       * @brief Build the monitored group
       * @param tool The monitoring tool to add the group to
       *
       * Builds the monitored group. Note that if this builder owns any
       * variables then fill *must* be called on the group before this object
       * goes out of scope (remembering that groups call fill on destruction if
       * it hasn't been called before this).
       */
      Monitored::Group build(
          const ToolHandle<GenericMonitoringTool>& tool) const;

      /**
       * @brief Increase the internal capacity
       * @param value The amount to increase capacity by
       * @param owned Whether to increase the 'owned' capacity by the same
       * amount
       *
       * Increase the capacity of the internal vectors. This function is used
       * for performance only.
       */
      void increaseCapacity(std::size_t value, bool owned=false);
    private:
      /// References to monitored variables
      std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> m_references;
      /// Any monitored variables that we own directly
      std::vector<std::unique_ptr<Monitored::IMonitoredVariable>> m_ptrs;
  }; //> end class MonGroupBuilder
} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_MONGROUPBUILDER_H
