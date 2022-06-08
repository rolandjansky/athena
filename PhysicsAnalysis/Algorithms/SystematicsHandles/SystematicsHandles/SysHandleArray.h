/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Jon Burr

#ifndef SYSTEMATICS_HANDLES__SYS_HANDLE_ARRAY_H
#define SYSTEMATICS_HANDLES__SYS_HANDLE_ARRAY_H

#include <AsgMessaging/StatusCode.h>
#include <AsgMessaging/AsgMessagingForward.h>
#include <SystematicsHandles/ISysHandleBase.h>
#include <SystematicsHandles/SysListHandle.h>
#include <PATInterfaces/SystematicSet.h>
#include <string>
#include <vector>

namespace CP
{
  /**
   * @brief Vector-like class for systematic handles which interact with decorations
   * @tparam HANDLE The type of handle in the vector
   */
  template <typename HANDLE> class SysHandleArray
    : public asg::AsgMessagingForward
  {
  public:
    using iterator = typename std::vector<HANDLE>::iterator;
    using const_iterator = typename std::vector<HANDLE>::const_iterator;
    using reverse_iterator = typename std::vector<HANDLE>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<HANDLE>::const_reverse_iterator;

    /**
     * @brief Construct the handle directly without declaring a property
     * @tparam OWNER The type providing the message stream
     * @param owner The owner of the handle, only used for its messaging service
     * @param values The decoration names of each individual handle
     */
    template <typename OWNER>
    SysHandleArray(OWNER *owner, const std::vector<std::string> &values);

    /**
     * @brief Declare the handle as a property on its parent
     * @tparam OWNER The type owning the handle
     * @param owner The owner of the handle, used for its properties and messaging
     * @param propertyName The name of the property to declare
     * @param propertyValue The default value of the property
     * @param propertyDescription The description of the property
     */
    template <typename OWNER>
    SysHandleArray(
        OWNER *owner,
        const std::string &propertyName,
        const std::vector<std::string> &propertyValue,
        const std::string &propertyDescription);

    /**
     * @brief Create and initialize all the sub-handles
     * 
     * The arguments to this function should be the arguments to the initialize
     * function on the wrapped handle type
     */
    template <typename... Args>
    StatusCode initialize(Args &&...args);

    /// @brief The number of defined handles
    std::size_t size() const noexcept;
    /**
     * @brief Whether any sub-handles are defined
     *
     * Note that this will return False if any sub-handles are defined, even if
     * all of these are actually themselves empty
     */
    bool empty() const noexcept;

    /**
     * @brief Retrieve the handle at the specified index
     * @param idx The index of the handle to retrieve
     * @pre idx < size()                [throws std::out_of_range]
     * @pre initialize has been called  [throws std::logic_error]
     */
    HANDLE &at(std::size_t idx);
    /**
     * @brief (Const) retrieve the handle at the specified index
     * @param idx The index of the handle to retrieve
     * @pre idx < size()                [throws std::out_of_range]
     * @pre initialize has been called  [throws std::logic_error]
     */
    const HANDLE &at(std::size_t idx) const;

    ///@{
    /**
     * @brief Standard vector iterator interface
     * @pre initialize has been called  [throws std::logic_error]
     */
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    reverse_iterator rbegin();
    reverse_iterator rend();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;
    ///@}

  private:
    /// @brief Throw a std::logic_error if the handle has not been initialized
    void requireInitialized() const;
    std::vector<std::string> m_decorNames;
    std::vector<HANDLE> m_handles;

  }; //> end class SysHandleArray<HANDLE>
} //> end namespace CP

#include "SystematicsHandles/SysHandleArray.icc"

#endif //> !SYSTEMATICS_HANDLES__SYS_HANDLE_ARRAY_H
