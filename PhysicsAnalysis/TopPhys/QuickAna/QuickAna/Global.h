/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__GLOBAL_H
#define QUICK_ANA__GLOBAL_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.



class StatusCode;

#ifndef ROOTCORE
class StoreGateSvc;
#endif

namespace asg
{
#ifdef ROOTCORE
  class SgTEvent;
#endif
}

namespace xAOD
{
#ifdef ROOTCORE
  class TEvent;
  class TStore;
#endif
}

namespace CP
{
  class SystematicSet;
}

namespace ana
{
  template<class T> struct EnumSize;


  /// description: this enum contains the list of analysis steps that
  ///   are to be performed, in the order in which they are performed
  enum AnalysisStep
  {
    /// description: retrieve the object from the file
    STEP_RETRIEVE,

    /// \brief attach all the selection decorations to the objects
    STEP_SELECT_INIT,

    /// \brief perform the pileup reweighting
    STEP_PILEUP_REWEIGHT,

    /// \brief get the trigger decision and basic trig matching
    STEP_TRIG_DECISION,

    /// description: apply "corrections" independent on systematics
    //    (like adding some aux data that's independent on systematics)
    STEP_PRECORRECT,

    /// description: apply the basic object corrections
    STEP_CORRECT,

    /// description: split the selection path for the different
    /// working points
    STEP_SPLIT_SELECT,

    /// description: apply the kinematic selection of objects
    STEP_KINSELECT,

    /// description: apply the object specific selection of objects
    STEP_SELECT,

    /// description: combine the selection from the multiple working
    /// points into an overall selection
    STEP_SUPER_SELECT,

    /// description: applying the overlap removal
    STEP_OVERLAP_REMOVAL,

    /// description: applying trigger and trigger matching
    STEP_TRIGGER,

    /// \brief spread out the result of the overlap removal to the
    /// various working points
    STEP_SPREAD_SELECT,

    /// description: apply the MET correction
    STEP_MET,

    /// description: calculate the weight of the objects and the
    ///   overall event
    STEP_WEIGHT,

    /// description: the actual analysis
    STEP_ANALYSIS
  };

  /// \brief the various object types we have to deal with
  enum ObjectType
  {
    OBJECT_EVENTINFO,
    OBJECT_EVENT_SELECT,
    OBJECT_JET,
    OBJECT_FAT_JET,
    OBJECT_ELECTRON,
    OBJECT_PHOTON,
    OBJECT_MUON,
    OBJECT_TAU,
    OBJECT_MET,
    OBJECT_MET2,
    OBJECT_OVERLAP_REMOVAL,
    OBJECT_TRIGGER,
    OBJECT_NONE
  };

  template<> struct EnumSize<ObjectType>
  {
    static const unsigned size = unsigned (OBJECT_NONE) + 1;

    /// \brief the number of object types that have actual objects
    /// associated
    static const unsigned dataSize = unsigned (OBJECT_OVERLAP_REMOVAL);
  };

  /// \brief the type used for accept/select decorations
  typedef char SelectType;

  class AnaTool;
  template<class XAODContainer> class AnaToolCorrect;
  class AnaToolRetrieve;
  template<class XAODContainer> class AnaToolSelect;
  class AnaToolSelectAll;
  class AnaToolSelectCombine;
  class AnaToolSelectSpread;
  class AnaToolSequence;
  template<class XAODContainer> class AnaToolWeight;
  struct Configuration;
  class DefinitionArgs;
  struct DefinitionConf;
  class DefinitionMaker;
  class EventData;
  class EventObjects;
  class IAnaTool;
  class IEventObjects;
  class IQuickAna;
  class IToolScheduler;
  class InternalConfiguration;
  class IsolationBase;
  class KinObjectSelect;
  class KinSelectTool;
  struct ObjectTypeInfo;
  class OptimizedObjects;
  class OptimizedScheduler;
  class OptimizedStore;
  class OptimizedTool;
  class QuickAna;
  enum class SelectionChoice;
  class SelectionCut;
  class SelectionData;
  enum class SelectionStep;
  class ToolScheduler;

  /// \brief the type of underlying store
#ifdef ROOTCORE
  typedef asg::SgTEvent StoreType;
#else
  typedef StoreGateSvc StoreType;
#endif

#ifdef ROOTCORE
  class ELExample;
  class ValidationEL;
  class ValidationHists;
#endif
}

#define QUICK_ANA_JOIN_STR(a,b)         QUICK_ANA_JOIN_STR_HIDDEN(a,b)
#define QUICK_ANA_JOIN_STR_HIDDEN(a,b)  a ## b


#endif
