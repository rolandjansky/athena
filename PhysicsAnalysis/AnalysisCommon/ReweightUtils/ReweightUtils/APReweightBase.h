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
  unsigned int m_ID;                                                              /*!< Holds the unique ID for assignment of APWeightEntries to source. */
  static unsigned int s_NID;                                                      /*!< Holds the overall number of assigned IDs. */
  double m_scale;                                                                 /*!< Holds the scale factor that was calculated from sample sizes upon instantiation. */
  bool m_isTrig;                                                                  /*!< Flag to determine if the class holds trigger efficiencies or "simple" MC weights. */
  bool m_isQuiet;                                                                 /*!< Flag to turn off messages. */
  double m_syst_uncert_global;                                                    /*!< Holds the global relative (!) systematic uncertainty of all efficiencies/weights. */
  APWeightEntry* m_empty_weight;                                                  /*!< Dummy weight (equals 0.) to return if value out of range is provided. */
};

#endif
