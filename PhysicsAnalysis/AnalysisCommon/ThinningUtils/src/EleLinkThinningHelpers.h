/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ThinningUtils_EleLinkThinningHelpers_H
#define ThinningUtils_EleLinkThinningHelpers_H

// package helpers
#include "ApplyFunctorSwitch.h"

// athena includes
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxVectorData.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthenaBaseComps/AthCheckMacros.h"

// STL includes
#include <utility>
#include <type_traits>
#include <exception>

namespace EleLinkThinningHelpers {

  // Helper struct to identify template specializations. This only works on
  // types that only accept typename templates, not int etc.
  template <typename T, template <typename...> class Candidate>
    struct is_specialization_of : public std::false_type {};
  template <template <typename...> class Candidate, typename... Args>
    struct is_specialization_of<Candidate<Args...>, Candidate> 
    : public std::true_type {};
  // Helper typedef std::vector<ElementLink<T>>, ElementLink<T> ->
  // ElementLink<T>
  template <typename T>
    using link_t = std::conditional_t<is_specialization_of<T, std::vector>{},
          typename T::value_type, T>;
  // Helper typdef std::vector<ElementLink<T>>, ElementLink<T> -> T
  template <typename T>
    using storable_t = typename link_t<T>::value_type;

  // Exception class to throw when an unexpected container is found in a link.
  class UnknownContainerInLink : public std::out_of_range {
    public:
      UnknownContainerInLink(
          SG::sgkey_t key, std::vector<std::string> candidates) 
        : std::out_of_range("Unexpected container received!"),
          key(key),
          candidates(candidates) {}
      const SG::sgkey_t key;
      const std::vector<std::string> candidates;
  };

  /**
   * @brief Base class for the helpers.
   *
   * The helpers act similarly to forward iterators. They are primed with one
   * input element at a time (using the loadElement function) and then the
   * linked particle(s) are stepped through one at a time using the next
   * function. Each time a helper steps through an element it is loaded into its
   * internal map which is then later used to thin those elements.
   * The links are required to point to one of a candidate list of containers.
   * These candidate containers are always thinned so if no link points to a
   * particular container the whole container will be marked for removal.
   */
  class ThinningHelperBase {
    public:
      /**
       * @brief Constructor
       * @param auxName The name of the link being accessed by this helper.
       * @param containerList The list of candidate containers to which these
       * links are required to belong.
       * @param op The IThinningSvc operator.
       * @param doThin Whether to apply thinning to elements processed by this
       * helper.
       * @param maxToPass The maximum number of elements that this helper is
       * allowed to pass per event.
       * @param prev The previous helper in the chain (if any).
       */
      ThinningHelperBase(
          const std::string& auxName,
          const std::vector<std::string>& containerList,
          IThinningSvc::Operator::Type op,
          bool doThin,
          std::size_t maxToPass,
          std::unique_ptr<ThinningHelperBase> prev)
        : auxName(auxName), 
          containerList(containerList), 
          op(op), 
          doThin(doThin),
          maxToPass(maxToPass),
          previous(std::move(prev) ) {}

      /// The name of the link being accessed by this helper.
      const std::string auxName;
      /// The list of candidate containers to which the processed links are
      /// required to belong.
      const std::vector<std::string> containerList;
      /// The IThinningSvc operator.
      const IThinningSvc::Operator::Type op;
      /// Whether to apply thinning to elements processed by this helper.
      const bool doThin;
      /// The maximum number of elements that this helper is allowed to pass per
      /// event.
      const std::size_t maxToPass;
      /// The previous helper in the chain (if any)
      const std::unique_ptr<ThinningHelperBase> previous;
      /**
       * @brief Initialise the mask, populating it with the candidate containers
       * and masks with all elements set to false.
       * @param sgSvc The StoreGateSvc used to retrieve the candidate
       * containers.
       */
      virtual StatusCode initMask(StoreGateSvc& sgSvc) = 0;
      /**
       * @brief Prime this helper with an element from the parent container.
       * @param container The container to take the element from.
       * @param index The index at which the element is stored.
       *
       * This recurses up the chain, passing container and index to the top,
       * then that top helper is loaded, peeks its first element which is passed
       * back down to the following and so-on back to the bottom.
       */
      virtual void loadElement(
          const SG::AuxVectorData* container, std::size_t index)
      {
        reset();
        // MaxToPass applies once per parent object
        m_nPassed = 0;
        if (previous) {
          // pass this container and index up
          previous->loadElement(container, index);
          // and get the one that we need here. This changes the values of
          // container and index to the ones that we need.
          if (!previous->peek(container, index) )
            // If peek was false then there was no element in one of the
            // upstream helpers so we must remain exhausted!
            return;
        }
        loadElementImpl(container, index);
        if (!peek(container, index) )
          // This triggers if we start pointing at an invalid element
          advance();
      }

      /**
       * @brief Peek at the current element we are pointing to. If there is an
       * element availabled then the provided parameters will be filled with its
       * information and true will be returned. If there is no such element then
       * false will be returned. In this case the arguments will not be changed.
       * @param[out] container Container of the current element.
       * @param[out] index Index of the current element.
       * @return Whether or not a new element was returned.
       */
      virtual bool peek(
          const SG::AuxVectorData*& container, std::size_t& index) const = 0;

      /**
       * @brief Step through one element. If there is an element available then
       * the provided parameters will be filled with its information and true
       * will be returned. If there is no such element false will be returned.
       * In this case the arguments will not be changed.
       * @param[out] container Container of the current element.
       * @param[out] index Index of the current element.
       * @return Whether or not a new element was returned.
       */
      virtual bool next(
          const SG::AuxVectorData*& container, std::size_t& index)
      {
        if (peek(container, index) ) {
          process();
          if (m_nPassed == maxToPass)
            reset();
          else
            advance();
          return true;
        }
        else
          return false;
      }

      /**
       * @brief Apply thinning to the candidate containers according to the
       * mask.
       * @param thinSvc The IThinningSvc to use.
       */
      virtual StatusCode thin(IThinningSvc& thinSvc) = 0;

      /**
       * @brief Have any of the helpers hit the maximum number of elements to
       * thin?
       */
    protected:
      /**
       * @brief Prime this helper with an element from the parent container.
       * @param container The container to take the element from.
       * @param index The index at which the element is stored.
       */
      virtual void loadElementImpl(
          const SG::AuxVectorData* container, std::size_t index) = 0;

      /**
       * @brief Advance to the next valid element or 'end'.
       * @return Whether there was a next valid element.
       *
       * After this call this and all upstream helpers will either point to the
       * next valid element or end.
       */
      virtual bool advance() = 0;
      /**
       * @brief Reset the position of the helper. This is usually necessary in
       * advance of a 'loadElement' call to ensure that the helper isn't
       * accidentally left pointing at an old element. The mask is not changed
       * by this function.
       */
      virtual void reset() = 0;
      /**
       * @brief Process the element that we're pointing at (by loading it into
       * the map). Called by \ref next so it can rely on the element being
       * pointed to being valid.
       */
      virtual void process() = 0;

      /// The number of elements this helper has passed.
      std::size_t m_nPassed{0};
  };

  /**
   * @brief Templated (but still abstract) partial implementation of the
   * ThinningHelperBase. Deals with the templated issues of interacting with the
   * ThinningSvc, StoreGate and ConstAccessor.
   * @tparam T The type of the link decoration: i.e. ElementLink<X> or
   * std::vector<ElementLink<X>>
   */
  template <typename T>
    class ThinningHelperTemplateBase : public ThinningHelperBase
  {
    // Make sure early that this type should work.
    static_assert(is_specialization_of<link_t<T>, ElementLink>{},
        "Templated type of ThinningHelper is incompatible!");
    public:
      /**
       * @brief Constructor
       * @param auxName The name of the link being accessed by this helper.
       * @param containerList The list of candidate containers to which these
       * links are required to belong.
       * @param op The IThinningSvc operator.
       * @param doThin Whether to apply thinning to elements processed by this
       * helper.
       * @param maxToPass The maximum number of elements that this helper is
       * allowed to pass per event.
       * @param prev The previous helper in the chain (if any).
       */
      ThinningHelperTemplateBase(
          const std::string& auxName,
          const std::vector<std::string>& containerList,
          IThinningSvc::Operator::Type op,
          bool doThin,
          std::size_t maxToPass,
          std::unique_ptr<ThinningHelperBase> prev)
        : ThinningHelperBase(
            auxName, containerList, op, doThin, maxToPass, std::move(prev) ),
          m_acc(auxName) {}

      /**
       * @brief Initialise the mask, populating it with the candidate containers
       * and masks with all elements set to false.
       * @param sgSvc The StoreGateSvc used to retrieve the candidate
       * containers.
       */
      StatusCode initMask(StoreGateSvc& sgSvc) final
      {
        if (previous)
          ATH_CHECK(previous->initMask(sgSvc) );
        m_thinInstr.clear();
        if (!doThin)
          return StatusCode::SUCCESS;
        for (const std::string& contName : containerList) {
          const storable_t<T>* container = nullptr;
          ATH_CHECK( sgSvc.retrieve(container, contName) );
          m_thinInstr[container] = std::vector<bool>(container->size(), false);
        }
        return StatusCode::SUCCESS;
      }

      /**
       * @brief Apply thinning to the candidate containers according to the
       * mask.
       * @param thinSvc The IThinningSvc to use.
       */
      StatusCode thin(IThinningSvc& thinSvc) final
      {
        if (previous)
          ATH_CHECK(previous->thin(thinSvc) );
        if (!doThin)
          return StatusCode::SUCCESS;
        for (const auto& instruction : m_thinInstr)
          ATH_CHECK( 
              thinSvc.filter(*instruction.first, instruction.second, op) );
        m_thinInstr.clear();
        return StatusCode::SUCCESS;
      }
    protected:

      /**
       * @brief Insert a link into the mask. If thinning isn't requested on this
       * helper it will do nothing. If the link points to an unknown container
       * an exception will be raised.
       */
      void insert(const link_t<T>& link) {
        if (!doThin) {
          ++m_nPassed;
          return;
        }
        auto itr = m_thinInstr.find(link.getDataPtr() );
        if (itr == m_thinInstr.end() )
          throw UnknownContainerInLink(link.key(), containerList);
        if (!itr->second.at(link.index() ) ) {
          ++m_nPassed;
          itr->second.at(link.index() ) = true;
        }
      }

      /// The accessor.
      const SG::AuxElement::ConstAccessor<T> m_acc;
      /// The thinning instructions (referred to as the mask).
      std::map<const storable_t<T>*, std::vector<bool>> m_thinInstr;
  };

  /**
   * @brief Implementation class. Takes care of the iterating through the
   * element link structure.
   * @tparam T The type of the link decoration: i.e. ElementLink<X> or
   * std::vector<ElementLink<X>>
   * @tparam IsVector whether or not T is a vector - used as a hook to
   * specialize the class.
   */
  template <typename T, bool IsVector = is_specialization_of<T, std::vector>{}>
    class ThinningHelper;

  /**
   * @brief Implementation of ThinningHelper for non-vector classes.
   * @tparam The type of the link decoration: i.e. ElementLink<X>.
   */
  template <typename T>
    class ThinningHelper<T, false> final : public ThinningHelperTemplateBase<T>
  {
    public:
      /**
       * @brief Constructor
       * @param auxName The name of the link being accessed by this helper.
       * @param containerList The list of candidate containers to which these
       * links are required to belong.
       * @param op The IThinningSvc operator.
       * @param doThin Whether to apply thinning to elements processed by this
       * helper.
       * @param maxToPass The maximum number of elements that this helper is
       * allowed to pass per event.
       * @param prev The previous helper in the chain (if any).
       */
      ThinningHelper(
          const std::string& auxName,
          const std::vector<std::string>& containerList,
          IThinningSvc::Operator::Type op,
          bool doThin,
          std::size_t maxToPass,
          std::unique_ptr<ThinningHelperBase> prev)
        : ThinningHelperTemplateBase<T>(
            auxName, containerList, op, doThin, maxToPass, std::move(prev) ) {}

      /**
       * @brief Peek at the current element we are pointing to. If there is an
       * element availabled then the provided parameters will be filled with its
       * information and true will be returned. If there is no such element then
       * false will be returned. In this case the arguments will not be changed.
       * @param[out] container Container of the current element.
       * @param[out] index Index of the current element.
       * @return Whether or not a new element was returned.
       */
      bool peek(
          const SG::AuxVectorData*& container, std::size_t& index) const final
      {
        if (!m_element || !m_element->isValid() )
          return false;
        container = m_element->getDataPtr();
        index = m_element->index();
        return true;
      }
    protected:
      /**
       * @brief Prime this helper with an element from the parent container.
       * @param container The container to take the element from.
       * @param index The index at which the element is stored.
       */
      void loadElementImpl(
          const SG::AuxVectorData* container, std::size_t index) final
      {
        if (!container) 
          throw std::invalid_argument("Nullptr recieved!?");
        // Actually load the element now.
        m_element = &this->m_acc(*container, index);
      }

      /**
       * @brief Reset the position of the helper. This is usually necessary in
       * advance of a 'loadElement' call to ensure that the helper isn't
       * accidentally left pointing at an old element. The mask is not changed
       * by this function.
       */
      void reset() final {
        m_element = nullptr;
      }

      /**
       * @brief Process the element that we're pointing at (by loading it into
       * the map). Called by \ref next so it can rely on the element being
       * pointed to being valid.
       */
     void process() final {
       this->insert(*m_element);
     }

      /**
       * @brief Advance to the next valid element or 'end'.
       * @return Whether there was a next valid element.
       *
       * After this call this and all upstream helpers will either point to a
       * valid element or end.
       */
      bool advance() final {
        // To advance a non-vector type we have to get the next element from the
        // previous helper. If that doesn't exist then we must have exhausted
        // everything.
        if (!this->previous) {
          reset();
          return false;
        }
        else {
          const SG::AuxVectorData* container;
          std::size_t index;
          // This enforces that we keep going until we either hit a valid
          // element (peek evaluates to true) or we exhaust the previous helper
          // (previous->next evaluates to false). Use a do...while to ensure
          // that we make at least one step.
          do {
            if (this->previous->next(container, index) )
              loadElementImpl(container, index);
            else {
              reset();
              return false;
            }
          } while (!peek(container, index) );
          // If we got here then we hit a valid state (peek evaluated to true).
          return true;
        }
      }
    private:
       /// The ElementLink pointed to by the decorator.
      const T* m_element{nullptr};
  };
  
  /**
   * @brief Implementation of ThinningHelper for vector classes.
   * @tparam The type of the link decoration: i.e. ElementLink<X>.
   */
  template <typename T>
    class ThinningHelper<T, true> final : public ThinningHelperTemplateBase<T>
  {
    public:
      /**
       * @brief Constructor
       * @param auxName The name of the link being accessed by this helper.
       * @param containerList The list of candidate containers to which these
       * links are required to belong.
       * @param op The IThinningSvc operator.
       * @param doThin Whether to apply thinning to elements processed by this
       * @param maxToPass The maximum number of elements that this helper is
       * allowed to pass per event.
       * @param prev The previous helper in the chain (if any).
       * helper.
       */
      ThinningHelper(
          const std::string& auxName,
          const std::vector<std::string>& containerList,
          IThinningSvc::Operator::Type op,
          bool doThin,
          std::size_t maxToPass,
          std::unique_ptr<ThinningHelperBase> prev)
        : ThinningHelperTemplateBase<T>(
            auxName, containerList, op, doThin, maxToPass, std::move(prev) ) {}

      /**
       * @brief Peek at the current element we are pointing to. If there is an
       * element availabled then the provided parameters will be filled with its
       * information and true will be returned. If there is no such element then
       * false will be returned. In this case the arguments will not be changed.
       * @param[out] container Container of the current element.
       * @param[out] index Index of the current element.
       * @return Whether or not a new element was returned.
       */
      virtual bool peek(
          const SG::AuxVectorData*& container, std::size_t& index) const
      {
        if (!m_element || m_itr == m_end || !m_itr->isValid() )
          return false;
        container = m_itr->getDataPtr();
        index = m_itr->index();
        return true;
      }

    protected:
      /**
       * @brief Prime this helper with an element from the parent container.
       * @param container The container to take the element from.
       * @param index The index at which the element is stored.
       */
      void loadElementImpl(
          const SG::AuxVectorData* container, std::size_t index) final
      {
        if (!container) 
          throw std::invalid_argument("Nullptr recieved!?");
        m_element = &this->m_acc(*container, index);
        m_itr = m_element->begin();
        m_end = m_element->end();
      }

      /**
       * @brief Reset the position of the helper. This is usually necessary in
       * advance of a 'loadElement' call to ensure that the helper isn't
       * accidentally left pointing at an old element. The mask is not changed
       * by this function.
       */
      void reset() final {
        m_element = nullptr;
        m_itr = typename T::const_iterator();
        m_end = typename T::const_iterator();
      }

      /**
       * @brief Advance to the next valid element or 'end'.
       * @return Whether there was a next valid element.
       *
       * After this call this and all upstream helpers will either point to a
       * valid element or end.
       */
      bool advance() final {
        // To advance a vector type first we have to try advancing the iterator
        if (m_element && m_itr != m_end) {
          while (true) {
            if (++m_itr == m_end)
              break;
            if (m_itr->isValid() )
              // We found a valid element!
              return true;
          }
        }
        // if we're here then we've exhausted our current element :(
        if (!this->previous) {
          reset();
          return false;
        }
        else {
          // Try and get a new one 
          const SG::AuxVectorData* container;
          std::size_t index;
          if (this->previous->next(container, index) ) {
            loadElementImpl(container, index);
            // Check that this one worked
            if (peek(container, index) )
              return true;
            else
              // We need to try advancing again
              return advance();
          }
          else {
            // There are no more elements to process
            reset();
            return false;
          }
        }
      }
      /**
       * @brief Process the element that we're pointing at (by loading it into
       * the map). Called by \ref next so it can rely on the element being
       * pointed to being valid.
       */
     void process() final {
       this->insert(*m_itr);
     }
    private:
      /// The element link vector pointed to by the decorator.
      const T* m_element{nullptr};
      /// The iterator within that vector.
      typename T::const_iterator m_itr;
      /// The end of that vector
      typename T::const_iterator m_end;
  };
  
  /**
   * @brief Helper typedef to allow the class to be used in the constructSwitch
   * function.
   * @tparam The contained link type.
   */
  template <typename T>
    using ThinningHelper_t = ThinningHelper<T, is_specialization_of<T, std::vector>{}>;

  /**
   * @brief Create a helper using a type_info object to select the right
   * template type. The first argument is a dummy used to pass in the template
   * parameter.
   * @tparam Ts The candidate template types that can be used.
   * @param ti The type info to perform the type-switch on.
   * @param auxName The name of the link being accessed by this helper.
   * @param containerList The list of candidate containers to which these links
   * are required to belong.
   * @param op The IThinningSvc operator.
   * @param doThin Whether to apply thinning to elements processed by this
   * helper.
   * @param maxToPass The maximum number of elements that this helper is
   * allowed to pass per event.
   * @param prev The previous helper in the chain (if any).
   */
  template <typename... Ts>
    std::unique_ptr<ThinningHelperBase> createHelper(
        const std::tuple<Ts...>*,
        const std::type_info& ti,
        const std::string& auxName,
        const std::vector<std::string>& containerList,
        IThinningSvc::Operator::Type op,
        bool doThin,
        std::size_t maxToPass,
        std::unique_ptr<ThinningHelperBase> prev)
    {
      return FunctorHelpers::constructSwitch<
        ThinningHelperBase, ThinningHelper_t,
        ElementLink<Ts>..., std::vector<ElementLink<Ts>>...>(ti,
            auxName, containerList, op, doThin, maxToPass, std::move(prev) );
    }
} //> end namespace EleLinkThinningHelpers

#endif //> !ThinningUtils_EleLinkThinningHelpers_H
