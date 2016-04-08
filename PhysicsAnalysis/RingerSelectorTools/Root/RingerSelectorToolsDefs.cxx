/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerSelectorToolsDefs.cxx 670599 2015-05-28 14:15:35Z wsfreund $
#include "RingerSelectorTools/RingerSelectorToolsDefs.h"

#ifndef RINGER_STANDALONE
#include "xAODCaloRings/CaloRingsDefs.h"
#endif

// Local includes:
#include "RingerSelectorTools/tools/Exceptions.h"

namespace Ringer {

// =============================================================================
unsigned numberOfSegments(const SegmentationType e)
{
  switch (e){
    case SegmentationType::NoSegmentation:
      return 1;
      break;
    case SegmentationType::TrackCalSegmentation:
      return numberOfSegments(SegmentationType::NoSegmentation) + 1;
      break;
    case SegmentationType::TrackCalPatTypeSegmentation:
      return numberOfSegments(SegmentationType::TrackCalSegmentation) + 1;
      break;
    case SegmentationType::TrackCalJointSections:
      return numberOfSegments(TrackCalPatTypeSegmentation) - 1 + 
#ifdef RINGER_STANDALONE
        2;
#else
        CalJointSection::NJointSections;
#endif
      break;
    case SegmentationType::TrackCalJointLayers:
      return numberOfSegments(TrackCalPatTypeSegmentation) - 1 + 
#ifdef RINGER_STANDALONE
        7;
#else
        CalJointLayer::NJointLayers;
#endif
      break;
    default:
      throw std::runtime_error(std::string("Could not recognize ") +
            "SegmentationType(" + std::to_string(int(e)) + 
            ") to numberOfSegments method on RingerSelectorToolsDefs.cxx"
            " file.");
  }
}

// =============================================================================
template<>
SegmentationType getType(const char* cStr)
{
  std::string strType(cStr);
  if (strType=="NoSegmentation"){
    return NoSegmentation;
  } else if ( strType=="TrackCalSegmentation" ){
    return TrackCalSegmentation;
  } else if ( strType=="TrackCalPatTypeSegmentation" ){
    return TrackCalPatTypeSegmentation;
  } else if ( strType=="TrackCalJointSections" ){
    return TrackCalJointSections;
  } else if ( strType == "TrackCalJointLayers" ){
    return TrackCalJointLayers;
  } else {
    throw Exceptions::no_such_type("SegmentationType",strType,
        "RingerSelectorToolsDefs");
  }
}

// =============================================================================
template<>
EtaDependency getType(const char* cStr)
{
  std::string strType(cStr);
  if (strType=="EtaIndependent"){
    return EtaIndependent; 
  } else if ( strType=="EtaDependent" ){
    return EtaDependent;
  } else {
    throw Exceptions::no_such_type("EtaDependency",strType,
        "RingerSelectorToolsDefs");
  }
}

// =============================================================================
template<>
EtDependency getType(const char* cStr)
{
  std::string strType(cStr);
  if (strType=="EtIndependent"){
    return EtIndependent; 
  } else if ( strType=="EtDependent" ){
    return EtDependent;
  } else {
    throw Exceptions::no_such_type("EtDependency",strType,
        "RingerSelectorToolsDefs");
  }
}
// =============================================================================
template<>
Requirement getType(const char* cStr)
{
  std::string strType(cStr);
  if ( strType=="Loose" ) {
    return Loose;
  } else if ( strType=="Medium" ) {
    return Medium;
  } else if ( strType=="Tight" ) {
    return Tight;
  } else if (strType=="Loose_CutID_Pd"){
    return Loose_CutID_Pd;
  } else if ( strType=="Medium_CutID_Pd" ) {
    return Medium_CutID_Pd;
  } else if ( strType=="Tight_CutID_Pd" ) {
    return Tight_CutID_Pd;
  } else if (strType=="Loose_CutID_Pd"){
    return Loose_CutID_Pf;
  } else if ( strType=="Medium_CutID_Pf" ) {
    return Medium_CutID_Pf;
  } else if ( strType=="Tight_CutID_Pf" ) {
    return Tight_CutID_Pf;
  } else if ( strType=="Medium_MaxSP" ) {
    return Medium_MaxSP;
  } else if ( strType=="NoCut" ) {
    return NoCut;
  } else {
    throw Exceptions::no_such_type("Requirement",strType,
        "RingerSelectorToolsDefs");
  }
}

// =============================================================================
const char* toStr(SegmentationType e)
{
  switch (e){
    case SegmentationType::NoSegmentation:
      return "NoSegmentation";
      break;
    case SegmentationType::TrackCalSegmentation:
      return "TrackCalSegmentation";
      break;
    case SegmentationType::TrackCalPatTypeSegmentation:
      return "TrackCalPatTypeSegmentation";
      break;
    case SegmentationType::TrackCalJointSections:
      return "TrackCalJointSections";
      break;
    case SegmentationType::TrackCalJointLayers:
      return "TrackCalJointLayers";
      break;
    default:
      throw Exceptions::no_such_type(e, "RingerSelectorToolsDefs");
  }
}

// =============================================================================
const char* toStr(EtaDependency e)
{
  switch (e){
    case EtaDependency::EtaDependent:
      return "EtaDependent";
      break;
    case EtaDependency::EtaIndependent:
      return "EtaIndependent";
      break;
    default:
      throw Exceptions::no_such_type(e, "RingerSelectorToolsDefs");
  }
}

// =============================================================================
const char* toStr(EtDependency e)
{
  switch (e){
    case EtDependency::EtDependent:
      return "EtDependent";
      break;
    case EtDependency::EtIndependent:
      return "EtIndependent";
      break;
    default:
      throw Exceptions::no_such_type(e, "RingerSelectorToolsDefs");
  }
}

// =============================================================================
const char* toStr(Requirement e)
{
  switch (e){
    case Requirement::Loose:
      return "Loose";
      break;
    case Requirement::Medium:
      return "Medium";
      break;
    case Requirement::Tight:
      return "Tight";
      break;
    case Requirement::Loose_CutID_Pd:
      return "Loose_CutID_Pd";
      break;
    case Requirement::Medium_CutID_Pd:
      return "Medium_CutID_Pd";
      break;
    case Requirement::Tight_CutID_Pd:
      return "Tight_CutID_Pd";
      break;
    case Requirement::Loose_CutID_Pf:
      return "Loose_CutID_Pf";
      break;
    case Requirement::Medium_CutID_Pf:
      return "Medium_CutID_Pf";
      break;
    case Requirement::Tight_CutID_Pf:
      return "Tight_CutID_Pf";
      break;
    case Requirement::Medium_MaxSP:
      return "Medium_MaxSP";
      break;
    case Requirement::NoCut:
      return "NoCut";
      break;
    default:
      throw Exceptions::no_such_type(e, "RingerSelectorToolsDefs");
  }
}

} // namespace Ringer

