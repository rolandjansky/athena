#include "EleLinkThinningHelpers.h"

namespace EleLinkThinningHelpers {
  StatusCode ThinningHelperVec::initMask(StoreGateSvc& sgSvc) 
  {
    for (auto& helper : helpers)
      ATH_CHECK( helper->initMask(sgSvc) );
    return StatusCode::SUCCESS;
  }

  StatusCode ThinningHelperVec::thin(IThinningSvc& thinSvc)
  {
    for (auto& helper : helpers)
      ATH_CHECK( helper->thin(thinSvc) );
    reset();
    return StatusCode::SUCCESS;
  }

  void ThinningHelperVec::loadElement(
      const SG::AuxVectorData* parent, std::size_t index)
  {
    m_parent = parent;
    m_index = index;
  }

  bool ThinningHelperVec::next(
      const SG::AuxVectorData*& container, std::size_t& index)
  {
    if (m_parent) {
      loadElementInternal(helpers.begin(), m_parent, m_index);
      m_parent = nullptr;
      m_index = -1;
    }
    while (!helpers.back()->next(container, index) ) {
      // The final helper's next call failing means it's exhausted on its
      // current element. Try giving it a new one.
      auto itr = helpers.end() - 1;
      while (true) {
        if (itr == helpers.begin() )
          // This means we've exhausted the parent element provided by
          // loadElement!
          return false;
        --itr;
        const SG::AuxVectorData* newContainer;
        std::size_t newIndex;
        if ( (*itr)->next(newContainer, newIndex) ) {
          loadElementInternal(++itr, newContainer, newIndex);
          break;
        }
      }
    }
    // Being outside the outer while loop means that next returned true and we
    // got an element!
    return true;
  }

  void ThinningHelperVec::reset()
  {
    for (auto& helper : helpers)
      helper->reset();
    m_parent = nullptr;
    m_index = -1;
  }

  void ThinningHelperVec::loadElementInternal(
      vec_t::iterator itr, const SG::AuxVectorData* container, std::size_t index)
  {
    auto end  = helpers.end();
    auto last = end - 1;
    while (itr != end) {
      (*itr)->loadElement(container, index);
      if (itr == last)
        break;
      if (!(*itr)->next(container, index) )
        break;
      ++itr;
    }
    for (++itr; itr != end; ++itr)
      (*itr)->reset();
  }
  
}
