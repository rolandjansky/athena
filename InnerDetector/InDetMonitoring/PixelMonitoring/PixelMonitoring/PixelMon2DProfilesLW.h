/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMON2DPROFILESLW_H_
#define PIXELMON2DPROFILESLW_H_
#include "PixelMonitoring/HolderTemplate.h"

class string;
class TProfile2D_LW;
class Identifier;
class PixelID;
class StatusCode;
namespace PixMon { enum class HistConf; }
class PixelMon2DMapsLW;

/**
 * PixelMon2DProfilesLW histogram holder class
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
 */
class PixelMon2DProfilesLW : public PixMon::HolderTemplate<TProfile2D_LW> {
public:
  //! Constructor for 2D map profiles
  PixelMon2DProfilesLW(std::string name, std::string title, const PixMon::HistConf& config, bool copy2DFEval = false);

  /**
   * Standard method to fill the histograms of this container.
   *
   * @param id: the identifier of the pixel unit
   * @param pixID: instance of the class which translates the above ID
   *        into readable info (e.g. eta/phi module index)
   * @param value: the fill value
   */
  void fill(Identifier &id, const PixelID* pixID, float value);

  //! Fill method which takes values from another 2D profile
  void fill2DMon(PixelMon2DProfilesLW* oldmap);

  //! Fill method which takes values from a 2D map object
  void fillFromMap(PixelMon2DMapsLW* inputmap, bool clear_inputmap);

  //! Set maximal value of the 2D profile bins
  void setMaxValue(float max);

  //! Reset the 2D profiles
  void reset();

  //! Formatting function for histograms (reimplemented from template)
  virtual void formatHist() override;
};

#endif  // PIXELMON2DPROFILESLW_H_
