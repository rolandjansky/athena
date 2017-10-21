/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMON2DMAPSLW_H_
#define PIXELMON2DMAPSLW_H_
#include "PixelMonitoring/HolderTemplate.h"

class string;
class TH2F_LW;
class Identifier;
class PixelID;
class StatusCode;
namespace PixMon { enum class HistConf; }

/**
 * PixelMon2DMapsLW histogram holder class
 *   --> Base type: 2D Maps (TH2F_LW)
 *   --> Supports:  IBL
 *                  IBL 2D/3D
 *                  B0, B1, B2
 *                  ECA/C
 *                  DBMA/C
 *
 * Collection of 2D pixel component maps, based on the TH2F_LW histogram class.
 * The initialisation of the components is controlled by the HistConf object.
 * The fill method will take the identifier as the input and fill the correct
 * histogram and bin. The histograms are also public so that they can be
 * formatted/accessed like any other histograms in the monitoring.
 */
class PixelMon2DMapsLW : public PixMon::HolderTemplate<TH2F_LW> {
 public:
  //! Constructor for 2D map objects
  PixelMon2DMapsLW(std::string name, std::string title, const PixMon::HistConf& config, bool copy2DFEval = false);

  /**
   * Standard method to fill the histograms of this container.
   *
   * @param id: the identifier of the pixel unit
   * @param pixID: instance of the class which translates the above ID
   *        into readable info (e.g. eta/phi module index)
   * @param weight: optional weighting factor
   */
  void fill(Identifier &id, const PixelID* pixID, float weight = 1.0);

  //! Fill method which takes values from another map
  void fill2DMon(PixelMon2DMapsLW* oldmap);

  //! Formatting function for histograms (reimplemented from template)
  virtual void formatHist() override;

 protected:
  friend class PixelMon2DProfilesLW;
};

#endif  // PIXELMON2DMAPSLW_H_
