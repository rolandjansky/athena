/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMON2DLUMIMAPS_H_
#define PIXELMON2DLUMIMAPS_H_
#include "PixelMonitoring/HolderTemplate.h"

class string;
class TH2F_LW;
class Identifier;
class PixelID;
class StatusCode;
namespace PixMon { enum class HistConf; }

/**
 * PixelMon2DLumiMaps histogram holder class
 *   --> Base type: 2D Maps (TH2F_LW)
 *   --> Supports:  IBL
 *                  B0, B1, B2
 *                  ECA/C
 *                  DBMA/C
 *
 * Collection of 2D pixel component maps, based on the TH2F_LW histogram class.
 * The initialisation of the components is controlled by the HistConf object.
 * The fill method will take the identifier as the input and fill the correct
 * histogram and bin. The histograms are also public so that they can be
 * formatted/accessed like any other histograms in the monitoring.
 *
 * N.B. Currently no support for IBL2D/3D histograms. Should this be added?
 * Also, if the histogram configuration contains unsupported histograms, this is
 * completely ignored. Should we do anything against it?
 */
class PixelMon2DLumiMaps : public PixMon::HolderTemplate<TH2F_LW> {
 public:
  //! Constructor for 2D lumi objects
  PixelMon2DLumiMaps(std::string name, std::string title, std::string zlabel, const PixMon::HistConf& config);

  /**
   * Standard method to fill the histograms of this container.
   *
   * @param LB: the luminosity block number
   * @param id: the identifier of the pixel unit
   * @param pixID: instance of the class which translates the above ID
   *        into readable info (e.g. eta/phi module index)
   * @param weight: optional weighting factor
   */
  void fill(double LB, Identifier &id, const PixelID* pixID, float weight = 1.0);

  //! Formatting function for histograms (reimplemented from template)
  virtual void formatHist() override;
};

#endif  // PIXELMON2DLUMIMAPS_H_
