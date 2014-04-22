#include "FadsPhysics/PhysicsListEntryT.h"

#include "QGSP_BERT_Msc95.hh"   // Using Urban95 for e-/e+ . 
static FADS::PhysicsListEntryT<QGSP_BERT_Msc95> physlist_QGSP_BERT_Msc95("QGSP_BERT_Msc95");

#include "QGSP_BERT_Conv95.hh"  // Uses improved gamma conversion. 
static FADS::PhysicsListEntryT<QGSP_BERT_Conv95> physlist_QGSP_BERT_Conv95("QGSP_BERT_Conv95");

#include "QGSP_BERT_Brem95.hh"  // Using relativisit Brem95 above threshold for e-/e+.  
static FADS::PhysicsListEntryT<QGSP_BERT_Brem95> physlist_QGSP_BERT_Brem95("QGSP_BERT_Brem95");

#include "QGSP_BERT_95.hh"      // Uses *all* 95 options, Msc95, Brem, Conv.
static FADS::PhysicsListEntryT<QGSP_BERT_95> physlist_QGSP_BERT_95("QGSP_BERT_95");

#include "QGSP_BERT_95NoMsc.hh"      // Uses 95 options for Brem and Conv.
static FADS::PhysicsListEntryT<QGSP_BERT_95NoMsc> physlist_QGSP_BERT_95NoMsc("QGSP_BERT_95NoMsc");

