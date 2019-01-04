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
    friend class ThinningHelperVec;
    public:
      /**
       * @brief Constructor
       * @param auxName The name of the link being accessed by this helper.
       * @param containerList The list of candidate containers to which these
       * links are required to belong.
       * @param op The IThinningSvc operator.
       * @param doThin Whether to apply thinning to elements processed by this
       * helper.
       */
      ThinningHelperBase(
          const std::string& auxName,
          const std::vector<std::string>& containerList,
          IThinningSvc::Operator::Type op,
          bool doThin)
        : auxName(auxName), 
          containerList(containerList), 
          op(op), 
          doThin(doThin) {}

      /// The name of the link being accessed by this helper.
      const std::string auxName;
      /// The list of candidate containers to which the processed links are
      /// required to belong.
      const std::vector<std::string> containerList;
      /// The IThinningSvc operator.
      const IThinningSvc::Operator::Type op;
      /// Whether to apply thinning to elements processed by this helper.
      const bool doThin;
    protected:
      /**
       * @brief Initialise the mask, populating it with the candidate containers
       * and masks with all elements set to false. This will then clear the
       * mask.
       * @param sgSvc The StoreGateSvc used to retrieve the candidate
       * containers.
       */
      virtual StatusCode initMask(StoreGateSvc& sgSvc) = 0;
      /**
       * @brief Prime this helper with an element from the parent container.
       * @param container The container to take the element from.
       * @param index The index at which the element is stored.
       */
      virtual void loadElement(
          const SG::AuxVectorData* container, std::size_t index) = 0;
      /**
       * @brief Step through one element. If there is an element available then
       * the provided parameters will be filled with its information and true
       * will be returned. If there is no such element then reset will be called
       * and false will be returned. In this case the arguments will not be
       * changed.
       * @param[out] container Container of the next element.
       * @param[out] index Index of the next element.
       * @return Whether or not a new element was returned.
       */
      virtual bool next(
          const SG::AuxVectorData*& container, std::size_t& index) = 0;
      /**
       * @brief Apply thinning to the candidate containers according to the
       * mask.
       * @param thinSvc The IThinningSvc to use.
       */
      virtual StatusCode thin(IThinningSvc& thinSvc) = 0;
      /**
       * @brief Reset the position of the helper. This is usually necessary in
       * advance of a 'loadElement' call to ensure that the helper isn't
       * accidentally left pointing at an old element. The mask is not changed
       * by this function.
       */
      virtual void reset() = 0;
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
       */
      ThinningHelperTemplateBase(
          const std::string& auxName,
          const std::vector<std::string>& containerList,
          IThinningSvc::Operator::Type op,
          bool doThin)
        : ThinningHelperBase(auxName, containerList, op, doThin),
          m_acc(auxName) {}

    protected:
      /**
       * @brief Initialise the mask, populating it with the candidate containers
       * and masks with all elements set to false. This will also clear the
       * mask.
       * @param sgSvc The StoreGateSvc used to retrieve the candidate
       * containers.
       */
      StatusCode initMask(StoreGateSvc& sgSvc) override
      {
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
      StatusCode thin(IThinningSvc& thinSvc) override
      {
        for (const auto& instruction : m_thinInstr)
          ATH_CHECK( 
              thinSvc.filter(*instruction.first, instruction.second, op) );
        m_thinInstr.clear();
        return StatusCode::SUCCESS;
      }

      /**
       * @brief Insert a link into the mask. If thinning isn't requested on this
       * helper it will do nothing. If the link points to an unknown container
       * an exception will be raised.
       */
      void insert(const link_t<T>& link) {
        if (!doThin)
          return;
        auto itr = m_thinInstr.find(link.getDataPtr() );
        if (itr == m_thinInstr.end() )
          throw UnknownContainerInLink(link.key(), containerList);
        itr->second.at(link.index() ) = true;
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
    class ThinningHelper<T, false> : public ThinningHelperTemplateBase<T>
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
       */
      ThinningHelper(
        const std::string& auxName,
        const std::vector<std::string>& containerList,
        IThinningSvc::Operator::Type op,
        bool doThin)
      : ThinningHelperTemplateBase<T>(auxName, containerList, op, doThin) {}

    protected:
      /**
       * @brief Prime this helper with an element from the parent container.
       * @param container The container to take the element from.
       * @param index The index at which the element is stored.
       */
      void loadElement(
          const SG::AuxVectorData* container, std::size_t index) override
      {
        if (!container) 
          throw std::invalid_argument("Nullptr recieved!?");
        m_element = &this->m_acc(*container, index);
      }

      /**
       * @brief Step through one element. If there is an element available then
       * the provided parameters will be filled with its information and true
       * will be returned. If there is no such element then reset will be called
       * and false will be returned. In this case the arguments will not be
       * changed.
       * @param[out] container Container of the next element.
       * @param[out] index Index of the next element.
       * @return Whether or not a new element was returned.
       */
      bool next(
          const SG::AuxVectorData*& container, std::size_t& index) override
      {
        if (!m_element || !m_element->isValid() ) {
          return false;
        }
        this->insert(*m_element);
        container = m_element->getDataPtr();
        index = m_element->index();
        m_element = nullptr;
        return true;
      }

      /**
       * @brief Reset the position of the helper. This is usually necessary in
       * advance of a 'loadElement' call to ensure that the helper isn't
       * accidentally left pointing at an old element. The mask is not changed
       * by this function.
       */
      void reset() override {
        m_element = nullptr;
      }
    private:
       /// The ElementLink pointed to by the decorator.
      const T* m_element{nullptr};
  };
  
  /**
   * @brief Implementation of ThinningHelper for non-vector classes.
   * @tparam The type of the link decoration: i.e. ElementLink<X>.
   */
  template <typename T>
    class ThinningHelper<T, true> : public ThinningHelperTemplateBase<T>
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
       */
      ThinningHelper(
        const std::string& auxName,
        const std::vector<std::string>& containerList,
        IThinningSvc::Operator::Type op,
        bool doThin)
      : ThinningHelperTemplateBase<T>(auxName, containerList, op, doThin) {}

    protected:
      /**
       * @brief Prime this helper with an element from the parent container.
       * @param container The container to take the element from.
       * @param index The index at which the element is stored.
       */
      void loadElement(
          const SG::AuxVectorData* container, std::size_t index) override
      {
        if (!container) 
          throw std::invalid_argument("Nullptr recieved!?");
        m_element = &this->m_acc(*container, index);
        m_itr = m_element->begin();
      }

      /**
       * @brief Step through one element. If there is an element available then
       * the provided parameters will be filled with its information and true
       * will be returned. If there is no such element then reset will be called
       * and false will be returned. In this case the arguments will not be
       * changed.
       * @param[out] container Container of the next element.
       * @param[out] index Index of the next element.
       * @return Whether or not a new element was returned.
       */
      bool next(
          const SG::AuxVectorData*& container, std::size_t& index) override
      {
        if (!m_element || m_itr == m_element->end() ) {
          // This means that the current input element has been exhausted.
          reset();
          return false;
        }
        // We need to step past any invalid elements.
        while (!m_itr->isValid() ) {
          if (++m_itr == m_element->end() ) {
            // The current input element has been exhausted.
            reset();
            return false;
          }
        }
        this->insert(*m_itr);
        container = m_itr->getDataPtr();
        index = m_itr->index();
        ++m_itr;
        return true;
      }

      /**
       * @brief Reset the position of the helper. This is usually necessary in
       * advance of a 'loadElement' call to ensure that the helper isn't
       * accidentally left pointing at an old element. The mask is not changed
       * by this function.
       */
      void reset() override {
        m_element = nullptr;
        m_itr = typename T::const_iterator();
      }
    private:
      /// The element link vector pointed to by the decorator.
      const T* m_element{nullptr};
      /// The iterator within that vector.
      typename T::const_iterator m_itr;
  };

  /**
   * @brief Vector of ThinningHelpers. This represents an ordered  way of
   * iterating through all elements referenced by a chain of element links.
   */
  class ThinningHelperVec {
    using vec_t = std::vector<std::unique_ptr<ThinningHelperBase>>;
    public:
      /**
       * @brief Call the initMask methods of all the helpers.
       * @param sgSvc The StoreGateSvc to use to init the masks.
       */
      StatusCode initMask(StoreGateSvc& sgSvc);
      /**
       * @brief Call the thin methods of all the helpers.
       * @param thinSvc The IThinningSvc to use.
       */
      StatusCode thin(IThinningSvc& thinSvc);
      /**
       * @brief Queue up an element to load into the helpers. The element is not
       * actually loaded in here. This is because this involves calling 'next'
       * on some of the helpers so they do not point to the start of their held
       * containers which is counter-intuitive.
       * @param parent The container to take the element from.
       * @param index The index at which the element is stored.
       */
      void loadElement(const SG::AuxVectorData* parent, std::size_t index);
      /**
       * @brief Get the next element from the final container in the step. If
       * the current input element (from loadElement) is exhausted then false is
       * returned and the output parameters are not altered.
       * @param[out] container Container of the next element.
       * @param[out] index Index of the next element.
       * @return Whether or not a new element was returned.
       */
      bool next(const SG::AuxVectorData*& container, std::size_t& index);
      /**
       * @brief Reset all helpers and remove any queued elements. Will not
       * changed the masks of the helpers.
       */
      void reset();
      /// The helpers.
      vec_t helpers;
    private:
      /// If an element is queued this is its parent.
      const SG::AuxVectorData* m_parent{nullptr};
      /// If an element is queued this is its index.
      std::size_t m_index = -1;
      /**
       * @brief Recurse down the chain of links. Starting with the helper
       * pointed to by itr, the element pointed to by (container, index) is
       * loaded into this helper, then the first element is retrieved from
       * *that* helper and loaded into the next, continuing until either all
       * helpers have an element loaded into them or a helper has not contained
       * any element. In this case all remaining helpers are reset.
       * @param itr The iterator to the helper to start with.
       * @param container The container of the starting element.
       * @param index The index of the starting element.
       */
      void loadElementInternal(
          vec_t::iterator itr, 
          const SG::AuxVectorData* container, 
          std::size_t index);
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
    */
  template <typename... Ts>
    std::unique_ptr<ThinningHelperBase> createHelper(
        const std::tuple<Ts...>*,
        const std::type_info& ti,
        const std::string& auxName,
        const std::vector<std::string>& containerList,
        IThinningSvc::Operator::Type op,
        bool doThin)
    {
      return FunctorHelpers::constructSwitch<
        ThinningHelperBase, ThinningHelper_t,
        ElementLink<Ts>..., std::vector<ElementLink<Ts>>...>(ti,
            auxName, containerList, op, doThin);
    }
}

#endif //> !ThinningUtils_EleLinkThinningHelpers_H
