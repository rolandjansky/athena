/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HOLDERTEMPLATE_H_
#define HOLDERTEMPLATE_H_
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/StatusCode.h"     
#include "InDetIdentifier/PixelID.h"

class array;
class string;

namespace PixMon {
enum class HistConf;

/**
 * Template class to hold a set of histogram for the different pixel components.
 * The class holds a maxmium of 10 histograms; the initialisation can be
 * controlled with the HistConf parameter given as input to the constructor.
 *
 * This class serves as template for various histogram holder classes.
 */
template <typename T>
class HolderTemplate {
 public:
  /**
   * Default constructor of the template class. Sets all pointers to null
   * and saves the configuration option as member variables.
   *
   * @param config: the histogram configuration
   * @param copy2DFEval: boolean whether to copy IBL values to neighbouring FE
   */
  inline HolderTemplate(const HistConf& config, bool copy2DFEval = false)
      : m_histograms{{nullptr}},
        IBL{m_histograms.at(0)},
        IBL2D{m_histograms.at(1)},
        IBL3D{m_histograms.at(2)},
        B0{m_histograms.at(3)},
        B1{m_histograms.at(4)},
        B2{m_histograms.at(5)},
        A{m_histograms.at(6)},
        C{m_histograms.at(7)},
        DBMA{m_histograms.at(8)},
        DBMC{m_histograms.at(9)},
        m_config{config},
        m_copy2DFEval{copy2DFEval},
        m_doIBL{true} {}

  //! Function for histogram formatting. To be reimplemented in derived classes.
  virtual void formatHist() = 0;

  //! Register the histograms in the monitoring group.
  StatusCode regHist(ManagedMonitorToolBase::MonGroup& group) {
    StatusCode sc = StatusCode::SUCCESS;
    for (auto& hist : m_histograms) {
      if (hist && group.regHist(hist).isFailure()) {
        sc = StatusCode::FAILURE;
      }
    }
    return sc;
  }

 protected:
  //! Array containing raw pointers to all histograms.
  std::array<T*, 10> m_histograms;

 public:
  //! Pointer to individal histogram: IBL (both 2D/3D modules).
  T*& IBL;

  //! Pointer to individal histogram: IBL 2D histogram (2D modules only).
  T*& IBL2D;

  //! Pointer to individal histogram: IBL 3D histogram (3D modules only).
  T*& IBL3D;

  //! Pointer to individal histogram: barrel layer 0.
  T*& B0;

  //! Pointer to individal histogram: barrel layer 1.
  T*& B1;

  //! Pointer to individal histogram: barrel layer 2.
  T*& B2;

  //! Pointer to individal histogram: end-cap side A.
  T*& A;

  //! Pointer to individal histogram: end-cap side C.
  T*& C;

  //! Pointer to individal histogram: DBM side A.
  T*& DBMA;

  //! Pointer to individal histogram: DBM side C.
  T*& DBMC;

 protected:
  //! Configuration which histograms are to be initialised.
  const HistConf m_config;

  //! Boolean whether IBL values should be copied to neighbouring FE.
  const bool m_copy2DFEval;

  //! Boolean for job option 'doIBL'. --> TO BE REMOVED
  const bool m_doIBL;
};
}

#endif  // HOLDERTEMPLATE_H_
