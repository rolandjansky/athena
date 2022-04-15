/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "InDetReadoutGeometry/SiDetectorElementStatus.h"
#include <stdexcept>
#include <sstream>

#ifdef DO_VALIDATE_STATUS_ARRAY
#include "GaudiKernel/ThreadLocalContext.h"
#include <iostream>
namespace dbg {
   void break_point( bool input1, bool input2, const char *label)  {
      const EventContext& ctx = Gaudi::Hive::currentContext();
      std::cout << "DEBUG " << ctx.evt() << " " << "inconsistent information:" << input1 << " != " << input2 << " = " << label << std::endl;
   }
   void break_point( double input1, double input2, const char *label)  {
      const EventContext& ctx = Gaudi::Hive::currentContext();
      std::cout << "DEBUG " << ctx.evt() << " " << "inconsistent information:" << input1 << " != " << input2 << " = " << label << std::endl;
   }
   void break_point( uint64_t input1, uint64_t input2, const char *label)  {
      const EventContext& ctx = Gaudi::Hive::currentContext();
      std::cout << "DEBUG " << ctx.evt() << " " << "inconsistent information:" << input1 << " != " << input2 << " = " << label << std::endl;
   }
}
#endif

namespace InDet {
   SiDetectorElementStatus &SiDetectorElementStatus::merge(const SiDetectorElementStatus  &b) {
      if (   (!getElementStatus().empty()     && !b.getElementStatus().empty()     && getElementStatus().size()     != b.getElementStatus().size())
          || (!getElementChipStatus().empty() && !b.getElementChipStatus().empty() && getElementChipStatus().size() != b.getElementChipStatus().size())) {
         std::stringstream msg;
         msg << "Status array sizes do not match:"
             << getElementStatus().size()     << " != " << b.getElementStatus().size()     << " || "
             << getElementChipStatus().size() << " != " << b.getElementChipStatus().size();
         throw std::runtime_error(msg.str());
      }

      if (!getElementStatus().empty()      && !b.getElementStatus().empty()) {
         for (std::size_t elm_i=0; elm_i < m_elementStatus.size(); ++elm_i) {
            m_elementStatus[elm_i] = m_elementStatus[elm_i] && b.m_elementStatus[elm_i];
         }
      }
      else if (!b.getElementStatus().empty()) {
         m_elementStatus = b.m_elementStatus;
      }

      if (!getElementChipStatus().empty() && !b.getElementChipStatus().empty()) {
         for (std::size_t elm_i=0; elm_i < m_elementChipStatus.size(); ++elm_i) {
            m_elementChipStatus[elm_i] &= b.m_elementChipStatus[elm_i];
         }
      }
      else if (!b.getElementChipStatus().empty()) {
         m_elementChipStatus = b.m_elementChipStatus;
      }
      if (&getBadCells() != &b.getBadCells()) {
         if (!m_owner) notOwningBadCells();
         if (!getBadCells().empty() && !b.getBadCells().empty()) {
            unsigned int element_i=0;
            for (const std::vector<unsigned short> &bad_module_strips :  b.getBadCells()) {
               std::vector<unsigned short> &dest = (*m_badCells)[element_i];
               if (dest.empty()) {
                  dest=bad_module_strips;
               }
               else {
                  for (unsigned int bad_strip : bad_module_strips) {
                     std::vector<unsigned short>::const_iterator iter = std::lower_bound(dest.begin(),dest.end(),bad_strip);
                     if (iter == dest.end() || *iter != bad_strip) {
                        dest.insert( iter, bad_strip);
                     }
                  }
               }
               ++element_i;
            }
         }
         else if (!b.getBadCells().empty()){
            *m_badCells = b.getBadCells();
         }
      }
      return *this;
   }

   void SiDetectorElementStatus::notOwningBadCells() const { throw std::logic_error("Bad cells not owned by this  instance, cannot return non const bad cell pointer."); }
} // namespace InDetDD
