/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class APReweightBase
 *
 * Class to provide common methods for all Reweight classes
 *
 *
 * @author fabian.Kohn@cern.ch
 **/
/////////////////////////////////////////////////////////////////////////////

#ifndef APReweightBase_h
#define APReweightBase_h

#include "TNamed.h"

class APWeightEntry;

class APReweightBase : public TNamed {

public:
  APReweightBase();                                                              /*!< Default constructor. */
  virtual ~APReweightBase();							 /*!< Default destructor. */
  
  unsigned int GetID() const;                                                    /*!< Returns the unique ID for assignment of APWeightEntries to source. */

  ClassDef(APReweightBase, 1)

protected:
  unsigned int _ID;                                                              /*!< Holds the unique ID for assignment of APWeightEntries to source. */
  static unsigned int _NID;                                                      /*!< Holds the overall number of assigned IDs. */
  double _scale;                                                                 /*!< Holds the scale factor that was calculated from sample sizes upon instantiation. */
  bool _isTrig;                                                                  /*!< Flag to determine if the class holds trigger efficiencies or "simple" MC weights. */
  bool _isQuiet;                                                                 /*!< Flag to turn off messages. */
  double _syst_uncert_global;                                                    /*!< Holds the global relative (!) systematic uncertainty of all efficiencies/weights. */
  APWeightEntry* _empty_weight;                                                  /*!< Dummy weight (equals 0.) to return if value out of range is provided. */
};

#endif
