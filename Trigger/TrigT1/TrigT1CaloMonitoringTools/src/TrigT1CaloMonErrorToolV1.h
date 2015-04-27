/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_TRIGT1CALOMONERRORTOOLV1_H
#define TRIGT1CALOBYTESTREAM_TRIGT1CALOMONERRORTOOLV1_H

#include <string>
#include <vector>

#include "AsgTools/AsgTool.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"

class IInterface;
class StatusCode;

namespace LVL1 {

/** Tool to retrieve ROB status and ROD unpacking errors from StoreGate.
 *
 *  Run 1 data only.
 *
 *  Forces retrieve of all containers to ensure error vector is complete.
 *
 *  The error vector returned by @c retrieve() is empty if there are no errors.<br>
 *  Otherwise the first word is the number of ROB status errors,               <br>
 *  followed by the ROB ID and error word for each ROB status error (if any),  <br>
 *  followed by the ROB ID and error code for each unpacking error (if any).   <br>
 *  Since the bytestream decoders skip a ROB fragment as soon as an error is detected
 *  there cannot be more than one error per ROB ID.
 *
 *  The behaviour of @c corrupt() is determined by the property @c FlagCorruptEvents:
 *  <table>
 *  <tr><th> @c FlagCorruptEvents      </th><th> @c corrupt() returns                                     </th></tr>
 *  <tr><td> "FullEventTimeout"        <br>
 *           (default)                 </td><td> @c true if Full Event status generic timeout bit set     <br>
 *                                               @b and there are missing ROB/ROD fragments               <br>
 *                                               @b and there are no ROB status or unpacking errors.      </td></tr>
 *  <tr><td> "AnyROBOrUnpackingError"  </td><td> @c true if there are any ROB status or unpacking errors. </td></tr>
 *  <tr><td> "None"                    <br>
 *           (or anything else)        </td><td> @c false.                                                </td></tr>
 *  </table>
 *  @c corrupt() is called by all the monitoring tools and if @c true the event is skipped.
 *
 *  <b>Related Documentation:</b>
 *
 *  <a href="https://edms.cern.ch/document/445840/4.0e/eformat.pdf">
 *  The raw event format in the ATLAS Trigger & DAQ, ATL-D-ES-0019</a><br>
 *  <a href="https://twiki.cern.ch/twiki/bin/viewauth/Atlas/ROBINFragmentErrors">
 *  Definition of the status words in a ROB fragment header</a><br>
 *  See the documentation for
 *  <a href="../../TrigT1CaloMonitoring/html/classTrigT1CaloRodMonTool.html#_details">TrigT1CaloRodMonTool</a>
 *  in the package TrigT1CaloMonitoring for the meanings of the unpacking error codes.
 *
 *  @author Peter Faulkner
 */

class TrigT1CaloMonErrorToolV1 :  public asg::AsgTool,
   virtual public ITrigT1CaloMonErrorTool {
   ASG_TOOL_CLASS(TrigT1CaloMonErrorToolV1, ITrigT1CaloMonErrorTool)
public:
   TrigT1CaloMonErrorToolV1(const std::string& name);
   virtual ~TrigT1CaloMonErrorToolV1();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Retrieve error vector
   StatusCode retrieve(const std::vector<unsigned int>*& errColl);
   /// Return true if current event has any corruption errors
   bool corrupt();
   /// Return true if current event has Full Event status generic timeout bit set
   bool fullEventTimeout();
   /// Return true if any ROB/ROD fragments are missing
   bool missingFragment();
   /// Return true if current event has any ROB or unpacking errors
   bool robOrUnpackingError();
   /// Return corrupt events flag string
   const std::string& flagCorruptEvents() const { return m_flagCorruptEvents; }

private:

   /// Trigger Tower container StoreGate key
   std::string m_triggerTowerLocation;
   /// CPM core tower container StoreGate key
   std::string m_cpmTowerLocation;
   /// CPM overlap tower container StoreGate key
   std::string m_cpmTowerLocationOverlap;
   /// CPM hits container StoreGate key
   std::string m_cpmHitsLocation;
   /// CMM-CP hits container StoreGate key
   std::string m_cmmCpHitsLocation;
   /// CPM RoI container StoreGate key
   std::string m_cpmRoiLocation;
   /// Core Jet Element container StoreGate key
   std::string m_jetElementLocation;
   /// Overlap Jet Element container StoreGate key
   std::string m_jetElementLocationOverlap;
   /// JEM hits container StoreGate key
   std::string m_jemHitsLocation;
   /// CMM-Jet hits container StoreGate key
   std::string m_cmmJetHitsLocation;
   /// JEM RoI container StoreGate key
   std::string m_jemRoiLocation;
   /// CMM RoI container StoreGate key
   std::string m_cmmRoiLocation;
   /// JEM Et sums container StoreGate key
   std::string m_jemEtSumsLocation;
   /// CMM Et sums container StoreGate key
   std::string m_cmmEtSumsLocation;
   /// ROD header container StoreGate key
   std::string m_rodHeaderLocation;
   /// CP RoIB ROD header container StoreGate key
   std::string m_cpRoibRodHeaderLocation;
   /// JEP RoIB ROD header container StoreGate key
   std::string m_jepRoibRodHeaderLocation;
   /// ROB and Unpacking Error vector StoreGate key
   std::string m_robErrorVectorLocation;
   /// Flag corrupt events
   std::string m_flagCorruptEvents;

};

} // end namespace

#endif
