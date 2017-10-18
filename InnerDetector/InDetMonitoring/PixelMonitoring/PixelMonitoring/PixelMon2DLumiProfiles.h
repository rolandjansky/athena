/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMON2DLUMIPROFILES_H_
#define PIXELMON2DLUMIPROFILES_H_
#include "PixelMonitoring/HolderTemplate.h"

class string;
class TProfile2D_LW;
class Identifier;
class PixelID;
class StatusCode;
namespace PixMon { enum class HistConf; }

/**
 * PixelMon2DMapsLW histogram holder class
 *   --> Base type: 2D Profiles (TProfile2D_LW)
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
 *
 * N.B. Currently no support for IBL2D/3D histograms. Should this be added?
 * Also, if the histogram configuration contains unsupported histograms, this is
 * completely ignored. Should we do anything against it?
 */
class PixelMon2DLumiProfiles : public PixMon::HolderTemplate<TProfile2D_LW> {
 public:
  //! Constructor for 2D lumi profiles
  PixelMon2DLumiProfiles(std::string name, std::string title, std::string zlabel, const PixMon::HistConf& config);

  /**
   * Standard method to fill the histograms of this container.
   *
   * @param LB: the luminosity block number
   * @param id: the identifier of the pixel unit
   * @param pixID: instance of the class which translates the above ID
   *        into readable info (e.g. eta/phi module index)
   * @param weight: optional weighting factor
   */
  void fill(double LB, Identifier &id, const PixelID* pixID, double weight = 1.0);

  //! Formatting function for histograms (reimplemented from template)
  void formatHist();
};

#endif  // PIXELMON2DLUMIPROFILES_H_
