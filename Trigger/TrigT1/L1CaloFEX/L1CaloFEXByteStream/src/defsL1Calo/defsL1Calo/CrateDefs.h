//==============================================================================
// CrateDefs.h
//==============================================================================
/*
 *  Class to define crate constants.
 */

#ifndef CRATEDEFS_H
#define CRATEDEFS_H

class CrateDefs {
public:
  static unsigned int tcmSlot()          { return 21; }
  static unsigned int cmmSlotL()         { return  3; }
  static unsigned int cmmSlotR()         { return 20; }
  static unsigned int minCpmJemSlot()    { return  4; }
  static unsigned int maxCpmJemSlot()    { return 19; }
  static unsigned int minCpmSlot()       { return  5; }
  static unsigned int maxCpmSlot()       { return 18; }
  static unsigned int minCpmLogId()      { return minCpmSlot() - minCpmJemSlot(); }
  static unsigned int maxCpmLogId()      { return maxCpmSlot() - minCpmJemSlot(); }
  static unsigned int minJemLogId()      { return 0; }
  static unsigned int maxJemLogId()      { return 15; }
  // Last and first slots of the two JEP quadrants in each crate.
  static unsigned int maxJemLQuadSlot()  { return 11; }
  static unsigned int minJemRQuadSlot()  { return 12; }
  static unsigned int minPprCrateLogId() { return  0; }
  static unsigned int maxPprCrateLogId() { return  7; }
  static unsigned int minCpCrateLogId()  { return  8; }
  static unsigned int maxCpCrateLogId()  { return 11; }
  static unsigned int minJepCrateLogId() { return 12; }
  static unsigned int maxJepCrateLogId() { return 13; }
  static unsigned int minRodCrateLogId() { return 14; }
  static unsigned int maxRodCrateLogId() { return 15; }
  static unsigned int ttcCrateLogId()    { return 16; }
  static unsigned int topoCrateLogId()   { return 17; }
  static unsigned int numCpCrates()      { return maxCpCrateLogId() - minCpCrateLogId() + 1; }
  static unsigned int numJepCrates()     { return maxJepCrateLogId() - minJepCrateLogId() + 1; }
  
  // For FEX ATCA crates.
  static unsigned int numAtcaHubSlots()      { return  2; }
  static unsigned int numAtcaFexSlots()      { return 12; }
  static unsigned int numAtcaSlots()         { return numAtcaHubSlots() + numAtcaFexSlots(); }
  static unsigned int minAtcaFexSlot()       { return numAtcaHubSlots() + 1; }
  static unsigned int maxAtcaFexSlot()       { return numAtcaSlots(); }
  
  static unsigned int atcaLogicalSlot(int physicalSlot);
  
};

#endif  // CRATEDEFS_H
