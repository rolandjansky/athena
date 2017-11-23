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
 * Template structure to delete light-weight histograms. The implementation of
 * this deleter is intentionally left empty: all histograms and their memory are
 * taken care of by the central monitoring tools (i.e. the monitoring group
 * classes in which the histograms are registered).
 */
template <typename T>
struct LWHistDeleter {
  void operator()(T* hist) {
    // To avoid "unused variable" compiler warnings for the parameter.
    (void)hist;
  }
};

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
      : IBL{nullptr},
        IBL2D{nullptr},
        IBL3D{nullptr},
        B0{nullptr},
        B1{nullptr},
        B2{nullptr},
        A{nullptr},
        C{nullptr},
        DBMA{nullptr},
        DBMC{nullptr},
        m_histograms{{nullptr}},
        m_config{config},
        m_copy2DFEval{copy2DFEval},
        m_doIBL{true} {}

  //! Function for histogram formatting. To be reimplemented in derived classes.
  virtual void formatHist() = 0;

  //! Register the histograms in the monitoring group.
  StatusCode regHist(ManagedMonitorToolBase::MonGroup& group) {
    StatusCode sc = StatusCode::SUCCESS;
    for (auto& hist : m_histograms) {
      if (hist && group.regHist(hist.get()).isFailure()) {
        sc = StatusCode::FAILURE;
      }
    }
    return sc;
  }

  /**
   * Function to set the individual (raw) pointers declared below to the correct
   * memory addresses. This function is to be called *after* the histograms in
   * m_histograms are initialised in the class constructor.
   */
  inline void setHistogramPointers() {
    IBL = m_histograms.at(0).get();
    IBL2D = m_histograms.at(1).get();
    IBL3D = m_histograms.at(2).get();
    B0 = m_histograms.at(3).get();
    B1 = m_histograms.at(4).get();
    B2 = m_histograms.at(5).get();
    A = m_histograms.at(6).get();
    C = m_histograms.at(7).get();
    DBMA = m_histograms.at(8).get();
    DBMC = m_histograms.at(9).get();
  }

  //! Pointer to individal histogram: IBL (both 2D/3D modules).
  T* IBL;

  //! Pointer to individal histogram: IBL 2D histogram (2D modules only).
  T* IBL2D;

  //! Pointer to individal histogram: IBL 3D histogram (3D modules only).
  T* IBL3D;

  //! Pointer to individal histogram: barrel layer 0.
  T* B0;

  //! Pointer to individal histogram: barrel layer 1.
  T* B1;

  //! Pointer to individal histogram: barrel layer 2.
  T* B2;

  //! Pointer to individal histogram: end-cap side A.
  T* A;

  //! Pointer to individal histogram: end-cap side C.
  T* C;

  //! Pointer to individal histogram: DBM side A.
  T* DBMA;

  //! Pointer to individal histogram: DBM side C.
  T* DBMC;

 protected:
  //! Array containing unique pointers to all histograms.
  std::array<std::unique_ptr<T, LWHistDeleter<T>>, 10> m_histograms;

  //! Configuration which histograms are to be initialised.
  const HistConf m_config;

  //! Boolean whether IBL values should be copied to neighbouring FE.
  const bool m_copy2DFEval;

  //! Boolean for job option 'doIBL'. --> TO BE REMOVED
  const bool m_doIBL;
};
}  // namespace PixMon

#endif  // HOLDERTEMPLATE_H_
