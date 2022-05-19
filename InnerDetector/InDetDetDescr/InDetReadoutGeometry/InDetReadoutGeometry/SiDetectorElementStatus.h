/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiDetectorElementStatus.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef INDETREADOUTGEOMETRY_SIDETECTORELEMENTINFO_H
#define INDETREADOUTGEOMETRY_SIDETECTORELEMENTINFO_H

#include <vector>
#include <utility>
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"


// @TODO remove in the future once new Pixel and SCT conditions summary information is fully validated
//#ifndef DO_VALIDATE_STATUS_ARRAY
//#  define DO_VALIDATE_STATUS_ARRAY
//#endif

#ifdef DO_VALIDATE_STATUS_ARRAY
#define  VALIDATE_STATUS_ARRAY_ACTIVATED true
#include <stdexcept>
namespace dbg {
   void break_point( bool input1, bool input2, const char *label);
   void break_point( double input1, double input2, const char *label);
   void break_point( uint64_t input1, uint64_t input2, const char *label);

   
}

#define  VALIDATE_STATUS_ARRAY( use_info, info_val, summary_val)     \
   {  \
   auto is_good = summary_val; \
   auto is_good_precomp =  (use_info) ? info_val : is_good; \
   if (is_good != is_good_precomp) { \
        dbg::break_point(is_good, is_good_precomp,#info_val);          \
        [[maybe_unused]] auto is_good_precomp2 = info_val;  \
        [[maybe_unused]] auto is_good2 = summary_val;       \
        throw std::logic_error("Inconsistent conditions summary information." ); \
   } } \
   do { } while (0)
#else
#define  VALIDATE_STATUS_ARRAY_ACTIVATED false
#define  VALIDATE_STATUS_ARRAY( use_info, info_val, summary_val)       do { } while (0)
#endif


// @TODO move to namespace InDetDD ?
namespace InDet {
    class SiDetectorDesign;

   //  @TODO or use bitset ?
    using ChipFlags_t = unsigned short;

    class SiDetectorElementStatus  {
    public:
       SiDetectorElementStatus(const InDetDD::SiDetectorElementCollection &detector_elements)
          : m_detectorElements(&detector_elements), m_badCells(new std::vector<std::vector<unsigned short> >()), m_owner(true) {}

       SiDetectorElementStatus(const SiDetectorElementStatus &si_detector_element_status)
          : m_detectorElements(si_detector_element_status.m_detectorElements),
            m_elementStatus( si_detector_element_status.m_elementStatus ),
            m_elementChipStatus( si_detector_element_status.m_elementChipStatus ),
            // cppcheck-suppress copyCtorPointerCopying
            m_badCells( si_detector_element_status.m_badCells ),
            m_owner( false )
       {
       }

       SiDetectorElementStatus(const InDetDD::SiDetectorElementCollection &detector_elements,
                               const std::vector<std::vector<unsigned short> > &bad_cells)
          : m_detectorElements(&detector_elements),
            m_badCells(const_cast<std::vector<std::vector<unsigned short> > *>(&bad_cells)), // const_cast but not owned object treated as const.
            m_owner(false)
       {}

       virtual ~SiDetectorElementStatus() { if (m_owner) delete m_badCells; }

       SiDetectorElementStatus& operator= (const SiDetectorElementStatus &) = delete;

       unsigned int numberOfChips(const IdentifierHash& hash) const {
          return numberOfChips(getDetectorElement(hash)->design());
       }
       const InDetDD::SiDetectorElement* getDetectorElement(const IdentifierHash& hash) const {
          return m_detectorElements->at(hash.value());
       }
       const std::vector<InDetDD::SiDetectorElement*> &getDetectorElements() const {
          return *m_detectorElements;
       }
       bool isGood(IdentifierHash hash)                             const {
          return m_elementStatus.at(hash.value());
       }
       bool isChipGood(IdentifierHash hash, unsigned int chip)      const {
#ifndef NDEBUG
          assert( m_elementStatus.empty() || (chip < numberOfChips(hash) && ((1<<(numberOfChips(hash)-1) ) <= std::numeric_limits<ChipFlags_t>::max())));
#endif
          return m_elementChipStatus.empty() || m_elementChipStatus.at(hash.value()) & (1<<chip);
       }

       bool isCellGood(IdentifierHash hash, unsigned short cell_i) const {
          const std::vector<unsigned short> &bad_cells= std::as_const(m_badCells)->at(hash);
          return !std::binary_search(bad_cells.begin(),bad_cells.end(),cell_i);
       }

       /** bitwise AND of module and chip status bits.
        */
       SiDetectorElementStatus  &merge(const SiDetectorElementStatus  &a);

       const std::vector<bool>         &getElementStatus()             const { return m_elementStatus; }
       std::vector<bool>               &getElementStatus()                   { return m_elementStatus; }
       const std::vector<ChipFlags_t>  &getElementChipStatus()         const { return m_elementChipStatus; }
       std::vector<ChipFlags_t>        &getElementChipStatus()               { return m_elementChipStatus; }
       const std::vector<std::vector<unsigned short> >  &getBadCells() const { return *m_badCells; }

       /** Get a non-const bad cells container owned by this instance.
        * Will create a copy of the bad cells container if it is not owned by this instance.
        */
       std::vector<std::vector<unsigned short> >        &getBadCells()       { if (!m_owner) { copyBadCells(); } return *m_badCells; }

    protected:
       virtual unsigned int numberOfChips(const InDetDD::SiDetectorDesign& design) const = 0;
    private:
       void notOwningBadCells() const;
       void copyBadCells() {
          m_badCells = new std::vector<std::vector<unsigned short> > (*m_badCells);
          m_owner=true;
       }

       const InDetDD::SiDetectorElementCollection *m_detectorElements = nullptr;
       std::vector<bool>                           m_elementStatus;
       std::vector<ChipFlags_t>                    m_elementChipStatus;
       std::vector<std::vector<unsigned short> >   *m_badCells = nullptr;
       bool                                         m_owner = false;
    };

} // namespace InDetDD

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( InDet::SiDetectorElementStatus , 154354418 , 1 )

CONDCONT_MIXED_DEF( InDet::SiDetectorElementStatus, 207353830 );

#endif // INDETREADOUTGEOMETRY_SIDETECTORELEMENTINFO_H
