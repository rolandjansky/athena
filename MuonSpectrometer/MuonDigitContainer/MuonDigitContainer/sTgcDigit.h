// sTgcDigit.h

#ifndef sTgcDigitUH
#define sTgcDigitUH

// Nektarios Chr. Benekos
// March 2013
//
// sTGC digitization.

#include <iosfwd>
#include <inttypes.h>
#include "MuonDigitContainer/MuonDigit.h"
#include "MuonIdHelpers/sTgcIdHelper.h"

class sTgcDigit : public MuonDigit {

 public:  // functions

  // Default constructor.
  sTgcDigit();

  // Full constructor --- From Identifier.
  sTgcDigit(const Identifier& id, float time);
  
  // Full constructor --- From Identifier and bctag
  sTgcDigit(const Identifier& id, uint16_t bctag, float time);

  // Full constructor --- From Identifier, bctag and charge, status flags
  sTgcDigit(const Identifier& id, uint16_t bctag, float time, float charge, bool isDead, bool isPileup);

  // Is this a valid digit?
  bool is_valid( const sTgcIdHelper *sTgcHelper) const;

  // get BC Tag
  uint16_t bcTag() const;
  enum {BC_UNDEFINED=0, BC_PREVIOUS, BC_CURRENT, BC_NEXT};

  // get the charge
  float charge() const;
  int charge_6bit() const;
  int charge_10bit() const;

  // get the time 
  float time() const;

  // get isDead 
  bool isDead() const;

  // get isPileup 
  bool isPileup() const;

  // Set bcTag 
  void set_bcTag(uint16_t newbcTag);
  
  // Set the charge
  void set_charge(float newCharge);

  // Set the time
  void set_time(float newTime);

  // Set the isDead
  void set_isDead(bool newIsDead);

  // Set the isPileup
  void set_isPileup(bool newIsPileup);
  

 private:  // data

  uint16_t  m_bcTag;
  float m_charge;
  float m_time;
  bool m_isDead;
  bool m_isPileup;
};

#endif
