/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MCEventDict_dict_fix.cxx 
 * 
 * @brief This class replaces the reflection classes for those
 *        containing HepLorentzVectors to allow reading of root3 files 
 * 
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * 
 * $Id: McEventDict_dict_fix.cxx,v 1.1 2005-08-12 12:52:38 schaffer Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

// Generated at Fri Aug 12 09:48:24 2005. Do not modify it

#ifdef _WIN32
#pragma warning ( disable : 4786 )
#ifndef LCGDICT_STRING 
#include <string> // Included here since it is sensitive to private->public trick
#endif
#endif
#define private public
#define protected public
#include "../GeneratorObjects/McEventDict.h"
#undef private
#undef protected
#include "ReflectionBuilder/ReflectionBuilder.h"
#include "ReflectionBuilder/CollectionProxy.h"
#include <typeinfo>
using namespace seal::reflect;


//------Dictionary for class GenVertex -------------------------------
class HepMC__GenVertex_dict { 
  public:
  HepMC__GenVertex_dict();
  static void* constructor_6443( void*, const std::vector<void*>& );
  static void* constructor_6444( void*, const std::vector<void*>& );
  static void  destructor( void* o ) { ((HepMC::GenVertex*)o)->~GenVertex(); }
  static void* operator_6446( void*, const std::vector<void*>& );
  static void* operator_6447( void*, const std::vector<void*>& );
  static void* operator_6448( void*, const std::vector<void*>& );
  static void  method_6449( void*, const std::vector<void*>& );
  static void* method_6450( void* );
  static void  method_6451( void*, const std::vector<void*>& );
  static void  method_6452( void*, const std::vector<void*>& );
  static void* method_6453( void*, const std::vector<void*>& );
  static void* converter_6454( void* );
  static void* converter_6455( void* );
  static void* method_6456( void* );
  static void* method_6457( void* );
  static void* method_6458( void* );
  static void  method_6459( void*, const std::vector<void*>& );
  static void* method_6460( void* );
  static void  method_6461( void*, const std::vector<void*>& );
  static void* method_6462( void* );
  static void* method_6463( void*, const std::vector<void*>& );
  static void* method_6464( void* );
  static void* method_6465( void* );
  static void* method_6466( void* );
  static void* method_6467( void* );
  static void* method_6468( void* );
  static void* method_6469( void* );
  static void* method_6470( void* );
  static void* method_6471( void* );
  static void* method_6478( void*, const std::vector<void*>& );
  static void* method_6479( void*, const std::vector<void*>& );
  static void* method_6480( void*, const std::vector<void*>& );
  static void* method_6481( void*, const std::vector<void*>& );
  static void* constructor_x4( void* );
};
HepMC__GenVertex_dict::HepMC__GenVertex_dict() {
  ClassBuilder _c_("HepMC::GenVertex", "", typeid(HepMC::GenVertex), sizeof(HepMC::GenVertex),std::vector<const std::type_info*>(), false, NOCONTAINER, 0, PUBLIC | VIRTUAL);
  _c_.addField("m_position", "HepLorentzVector", "", (size_t)(&((HepMC::GenVertex*)64)->m_position)-64, PRIVATE );
  _c_.addField("m_particles_in", "std::set<HepMC::GenParticle*>", "", (size_t)(&((HepMC::GenVertex*)64)->m_particles_in)-64, PRIVATE );
  _c_.addField("m_particles_out", "std::set<HepMC::GenParticle*>", "", (size_t)(&((HepMC::GenVertex*)64)->m_particles_out)-64, PRIVATE );
  _c_.addField("m_id", "int", "", (size_t)(&((HepMC::GenVertex*)64)->m_id)-64, PRIVATE );
  _c_.addField("m_weights", "HepMC::WeightContainer", "", (size_t)(&((HepMC::GenVertex*)64)->m_weights)-64, PRIVATE );
  _c_.addField("m_event", "HepMC::GenEvent*", "", (size_t)(&((HepMC::GenVertex*)64)->m_event)-64, PRIVATE );
  _c_.addField("m_barcode", "int", "", (size_t)(&((HepMC::GenVertex*)64)->m_barcode)-64, PRIVATE );
  _c_.addMethod("GenVertex", "", "HepMC::GenVertex", "CLHEP::HepLorentzVector&=; int=; HepMC::WeightContainer&=", constructor_6443, PUBLIC);
  _c_.addMethod("GenVertex", "", "HepMC::GenVertex", "HepMC::GenVertex&", constructor_6444, PUBLIC);
  _c_.addMethod("~GenVertex", "", destructor, PUBLIC | VIRTUAL);
  _c_.addMethod("=", "", "HepMC::GenVertex&", "HepMC::GenVertex&", operator_6446, PUBLIC);
  _c_.addMethod("==", "", "bool", "HepMC::GenVertex&", operator_6447, PUBLIC);
  _c_.addMethod("!=", "", "bool", "HepMC::GenVertex&", operator_6448, PUBLIC);
  _c_.addMethod("print", "", "std::basic_ostream<char,std::char_traits<char> >&=", method_6449, PUBLIC);
  _c_.addMethod("check_momentum_conservation", "", "double", method_6450, PUBLIC);
  _c_.addMethod("add_particle_in", "", "HepMC::GenParticle*", method_6451, PUBLIC);
  _c_.addMethod("add_particle_out", "", "HepMC::GenParticle*", method_6452, PUBLIC);
  _c_.addMethod("remove_particle", "", "HepMC::GenParticle*", "HepMC::GenParticle*", method_6453, PUBLIC);
  _c_.addMethod("CLHEP::HepLorentzVector", "", "CLHEP::HepLorentzVector", converter_6454, PUBLIC);
  _c_.addMethod("HepGeom::Point3D<double>", "", "HepGeom::Point3D<double>", converter_6455, PUBLIC);
  _c_.addMethod("parent_event", "", "HepMC::GenEvent*", method_6456, PUBLIC);
  _c_.addMethod("point3d", "", "HepGeom::Point3D<double>", method_6457, PUBLIC);
  _c_.addMethod("position", "", "CLHEP::HepLorentzVector", method_6458, PUBLIC);
  _c_.addMethod("set_position", "", "CLHEP::HepLorentzVector&=", method_6459, PUBLIC);
  _c_.addMethod("id", "", "int", method_6460, PUBLIC);
  _c_.addMethod("set_id", "", "int", method_6461, PUBLIC);
  _c_.addMethod("barcode", "", "int", method_6462, PUBLIC);
  _c_.addMethod("suggest_barcode", "", "bool", "int", method_6463, PUBLIC);
  _c_.addMethod("weights", "", "HepMC::WeightContainer&", method_6464, PUBLIC);
  _c_.addMethod("weights", "", "HepMC::WeightContainer&", method_6465, PUBLIC);
  _c_.addMethod("particles_in_const_begin", "", "std::_Rb_tree_iterator<HepMC::GenParticle*,HepMC::GenParticle* const&,HepMC::GenParticle* const*>", method_6466, PUBLIC);
  _c_.addMethod("particles_in_const_end", "", "std::_Rb_tree_iterator<HepMC::GenParticle*,HepMC::GenParticle* const&,HepMC::GenParticle* const*>", method_6467, PUBLIC);
  _c_.addMethod("particles_out_const_begin", "", "std::_Rb_tree_iterator<HepMC::GenParticle*,HepMC::GenParticle* const&,HepMC::GenParticle* const*>", method_6468, PUBLIC);
  _c_.addMethod("particles_out_const_end", "", "std::_Rb_tree_iterator<HepMC::GenParticle*,HepMC::GenParticle* const&,HepMC::GenParticle* const*>", method_6469, PUBLIC);
  _c_.addMethod("particles_in_size", "", "int", method_6470, PUBLIC);
  _c_.addMethod("particles_out_size", "", "int", method_6471, PUBLIC);
  _c_.addMethod("vertices_begin", "", "HepMC::GenVertex::vertex_iterator", "int=", method_6478, PUBLIC);
  _c_.addMethod("vertices_end", "", "HepMC::GenVertex::vertex_iterator", "int", method_6479, PUBLIC);
  _c_.addMethod("particles_begin", "", "HepMC::GenVertex::particle_iterator", "int=", method_6480, PUBLIC);
  _c_.addMethod("particles_end", "", "HepMC::GenVertex::particle_iterator", "int", method_6481, PUBLIC);
  _c_.addMethod("GenVertex", "", "HepMC::GenVertex", constructor_x4, PUBLIC);
  _c_.build();
}

//------Stub functions for class GenVertex -------------------------------
void* HepMC__GenVertex_dict::constructor_6443( void* mem, const std::vector<void*>& arg) {
  if ( arg.size() == 0 ) {
    return ::new(mem) HepMC::GenVertex();
  }
  else if ( arg.size() == 1 ) { 
    return ::new(mem) HepMC::GenVertex(*(const CLHEP::HepLorentzVector*)arg[0]);
  }
  else if ( arg.size() == 2 ) { 
    return ::new(mem) HepMC::GenVertex(*(const CLHEP::HepLorentzVector*)arg[0],
                                     *(int*)arg[1]);
  }
  else if ( arg.size() == 3 ) { 
    return ::new(mem) HepMC::GenVertex(*(const CLHEP::HepLorentzVector*)arg[0],
                                     *(int*)arg[1],
                                     *(const HepMC::WeightContainer*)arg[2]);
  }
  return 0;
}

void* HepMC__GenVertex_dict::constructor_6444( void* mem, const std::vector<void*>& arg) {
  return ::new(mem) HepMC::GenVertex(*(const HepMC::GenVertex*)arg[0]);
}

void* HepMC__GenVertex_dict::operator_6446( void* o, const std::vector<void*>& arg)
{
  return (void*)&((HepMC::GenVertex*)o)->operator =(*(const HepMC::GenVertex*)arg[0]);
}

void* HepMC__GenVertex_dict::operator_6447( void* o, const std::vector<void*>& arg)
{
  static bool ret;
  ret = ((const HepMC::GenVertex*)o)->operator ==(*(const HepMC::GenVertex*)arg[0]);
  return &ret;
}

void* HepMC__GenVertex_dict::operator_6448( void* o, const std::vector<void*>& arg)
{
  static bool ret;
  ret = ((const HepMC::GenVertex*)o)->operator !=(*(const HepMC::GenVertex*)arg[0]);
  return &ret;
}

void HepMC__GenVertex_dict::method_6449( void* o, const std::vector<void*>& arg)
{
  if ( arg.size() == 0 ) {
    ((const HepMC::GenVertex*)o)->print();
  }
  else if ( arg.size() == 1 ) { 
    ((const HepMC::GenVertex*)o)->print(*(std::basic_ostream<char,std::char_traits<char> >*)arg[0]);
  }
}

void* HepMC__GenVertex_dict::method_6450( void* o)
{
  static double ret;
  ret = ((const HepMC::GenVertex*)o)->check_momentum_conservation();
  return &ret;
}

void HepMC__GenVertex_dict::method_6451( void* o, const std::vector<void*>& arg)
{
  ((HepMC::GenVertex*)o)->add_particle_in((HepMC::GenParticle*)arg[0]);
}

void HepMC__GenVertex_dict::method_6452( void* o, const std::vector<void*>& arg)
{
  ((HepMC::GenVertex*)o)->add_particle_out((HepMC::GenParticle*)arg[0]);
}

void* HepMC__GenVertex_dict::method_6453( void* o, const std::vector<void*>& arg)
{
  return (void*)((HepMC::GenVertex*)o)->remove_particle((HepMC::GenParticle*)arg[0]);
}

void* HepMC__GenVertex_dict::converter_6454( void* o)
{
  return new CLHEP::HepLorentzVector(((const HepMC::GenVertex*)o)->operator CLHEP::HepLorentzVector());
}

void* HepMC__GenVertex_dict::converter_6455( void* o)
{
  return new HepGeom::Point3D<double>(((const HepMC::GenVertex*)o)->operator HepGeom::Point3D<double>());
}

void* HepMC__GenVertex_dict::method_6456( void* o)
{
  return (void*)((const HepMC::GenVertex*)o)->parent_event();
}

void* HepMC__GenVertex_dict::method_6457( void* o)
{
  return new HepGeom::Point3D<double>(((const HepMC::GenVertex*)o)->point3d());
}

void* HepMC__GenVertex_dict::method_6458( void* o)
{
  return new CLHEP::HepLorentzVector(((const HepMC::GenVertex*)o)->position());
}

void HepMC__GenVertex_dict::method_6459( void* o, const std::vector<void*>& arg)
{
  if ( arg.size() == 0 ) {
    ((HepMC::GenVertex*)o)->set_position();
  }
  else if ( arg.size() == 1 ) { 
    ((HepMC::GenVertex*)o)->set_position(*(const CLHEP::HepLorentzVector*)arg[0]);
  }
}

void* HepMC__GenVertex_dict::method_6460( void* o)
{
  static int ret;
  ret = ((const HepMC::GenVertex*)o)->id();
  return &ret;
}

void HepMC__GenVertex_dict::method_6461( void* o, const std::vector<void*>& arg)
{
  ((HepMC::GenVertex*)o)->set_id(*(int*)arg[0]);
}

void* HepMC__GenVertex_dict::method_6462( void* o)
{
  static int ret;
  ret = ((const HepMC::GenVertex*)o)->barcode();
  return &ret;
}

void* HepMC__GenVertex_dict::method_6463( void* o, const std::vector<void*>& arg)
{
  static bool ret;
  ret = ((HepMC::GenVertex*)o)->suggest_barcode(*(int*)arg[0]);
  return &ret;
}

void* HepMC__GenVertex_dict::method_6464( void* o)
{
  return (void*)&((HepMC::GenVertex*)o)->weights();
}

void* HepMC__GenVertex_dict::method_6465( void* o)
{
  return (void*)&((const HepMC::GenVertex*)o)->weights();
}

void* HepMC__GenVertex_dict::method_6466( void* o)
{
  return new std::_Rb_tree_iterator<HepMC::GenParticle*,HepMC::GenParticle* const&,HepMC::GenParticle* const*>(((const HepMC::GenVertex*)o)->particles_in_const_begin());
}

void* HepMC__GenVertex_dict::method_6467( void* o)
{
  return new std::_Rb_tree_iterator<HepMC::GenParticle*,HepMC::GenParticle* const&,HepMC::GenParticle* const*>(((const HepMC::GenVertex*)o)->particles_in_const_end());
}

void* HepMC__GenVertex_dict::method_6468( void* o)
{
  return new std::_Rb_tree_iterator<HepMC::GenParticle*,HepMC::GenParticle* const&,HepMC::GenParticle* const*>(((const HepMC::GenVertex*)o)->particles_out_const_begin());
}

void* HepMC__GenVertex_dict::method_6469( void* o)
{
  return new std::_Rb_tree_iterator<HepMC::GenParticle*,HepMC::GenParticle* const&,HepMC::GenParticle* const*>(((const HepMC::GenVertex*)o)->particles_out_const_end());
}

void* HepMC__GenVertex_dict::method_6470( void* o)
{
  static int ret;
  ret = ((const HepMC::GenVertex*)o)->particles_in_size();
  return &ret;
}

void* HepMC__GenVertex_dict::method_6471( void* o)
{
  static int ret;
  ret = ((const HepMC::GenVertex*)o)->particles_out_size();
  return &ret;
}

void* HepMC__GenVertex_dict::method_6478( void* o, const std::vector<void*>& arg)
{
  if ( arg.size() == 0 ) {
    return new HepMC::GenVertex::vertex_iterator(((HepMC::GenVertex*)o)->vertices_begin());
  }
  else if ( arg.size() == 1 ) { 
    return new HepMC::GenVertex::vertex_iterator(((HepMC::GenVertex*)o)->vertices_begin(*(HepMC::IteratorRange*)arg[0]));
  }
  return 0;
}

void* HepMC__GenVertex_dict::method_6479( void* o, const std::vector<void*>& arg)
{
  return new HepMC::GenVertex::vertex_iterator(((HepMC::GenVertex*)o)->vertices_end(*(HepMC::IteratorRange*)arg[0]));
}

void* HepMC__GenVertex_dict::method_6480( void* o, const std::vector<void*>& arg)
{
  if ( arg.size() == 0 ) {
    return new HepMC::GenVertex::particle_iterator(((HepMC::GenVertex*)o)->particles_begin());
  }
  else if ( arg.size() == 1 ) { 
    return new HepMC::GenVertex::particle_iterator(((HepMC::GenVertex*)o)->particles_begin(*(HepMC::IteratorRange*)arg[0]));
  }
  return 0;
}

void* HepMC__GenVertex_dict::method_6481( void* o, const std::vector<void*>& arg)
{
  return new HepMC::GenVertex::particle_iterator(((HepMC::GenVertex*)o)->particles_end(*(HepMC::IteratorRange*)arg[0]));
}

void* HepMC__GenVertex_dict::constructor_x4( void* mem) {
  return ::new(mem) HepMC::GenVertex();
}


//------Dictionary for class GenParticle -------------------------------
class HepMC__GenParticle_dict { 
  public:
  HepMC__GenParticle_dict();
  static void* constructor_6550( void* );
  static void* constructor_6551( void*, const std::vector<void*>& );
  static void* constructor_6552( void*, const std::vector<void*>& );
  static void  destructor( void* o ) { ((HepMC::GenParticle*)o)->~GenParticle(); }
  static void* operator_6554( void*, const std::vector<void*>& );
  static void* operator_6555( void*, const std::vector<void*>& );
  static void* operator_6556( void*, const std::vector<void*>& );
  static void  method_6557( void*, const std::vector<void*>& );
  static void* converter_6558( void* );
  static void* method_6559( void* );
  static void* method_6560( void* );
  static void* method_6561( void* );
  static void* method_6562( void* );
  static void* method_6563( void*, const std::vector<void*>& );
  static void* method_6564( void* );
  static void* method_6565( void* );
  static void* method_6566( void* );
  static void* method_6567( void* );
  static void* method_6568( void* );
  static void* method_6569( void*, const std::vector<void*>& );
  static void  method_6570( void*, const std::vector<void*>& );
  static void  method_6571( void*, const std::vector<void*>& );
  static void  method_6572( void*, const std::vector<void*>& );
  static void  method_6573( void*, const std::vector<void*>& );
  static void  method_6574( void*, const std::vector<void*>& );
  static void  method_6575( void*, const std::vector<void*>& );
};
HepMC__GenParticle_dict::HepMC__GenParticle_dict() {
  ClassBuilder _c_("HepMC::GenParticle", "", typeid(HepMC::GenParticle), sizeof(HepMC::GenParticle),std::vector<const std::type_info*>(), false, NOCONTAINER, 0, PUBLIC | VIRTUAL);
  _c_.addField("m_momentum", "HepLorentzVector", "", (size_t)(&((HepMC::GenParticle*)64)->m_momentum)-64, PRIVATE );
  _c_.addField("m_pdg_id", "int", "", (size_t)(&((HepMC::GenParticle*)64)->m_pdg_id)-64, PRIVATE );
  _c_.addField("m_status", "int", "", (size_t)(&((HepMC::GenParticle*)64)->m_status)-64, PRIVATE );
  _c_.addField("m_flow", "HepMC::Flow", "", (size_t)(&((HepMC::GenParticle*)64)->m_flow)-64, PRIVATE );
  _c_.addField("m_polarization", "HepMC::Polarization", "", (size_t)(&((HepMC::GenParticle*)64)->m_polarization)-64, PRIVATE );
  _c_.addField("m_production_vertex", "HepMC::GenVertex*", "", (size_t)(&((HepMC::GenParticle*)64)->m_production_vertex)-64, PRIVATE );
  _c_.addField("m_end_vertex", "HepMC::GenVertex*", "", (size_t)(&((HepMC::GenParticle*)64)->m_end_vertex)-64, PRIVATE );
  _c_.addField("m_barcode", "int", "", (size_t)(&((HepMC::GenParticle*)64)->m_barcode)-64, PRIVATE );
  _c_.addMethod("GenParticle", "", "HepMC::GenParticle", constructor_6550, PUBLIC);
  _c_.addMethod("GenParticle", "", "HepMC::GenParticle", "CLHEP::HepLorentzVector&; int; int=; HepMC::Flow&=; HepMC::Polarization&=", constructor_6551, PUBLIC);
  _c_.addMethod("GenParticle", "", "HepMC::GenParticle", "HepMC::GenParticle&", constructor_6552, PUBLIC);
  _c_.addMethod("~GenParticle", "", destructor, PUBLIC | VIRTUAL);
  _c_.addMethod("=", "", "HepMC::GenParticle&", "HepMC::GenParticle&", operator_6554, PUBLIC);
  _c_.addMethod("==", "", "bool", "HepMC::GenParticle&", operator_6555, PUBLIC);
  _c_.addMethod("!=", "", "bool", "HepMC::GenParticle&", operator_6556, PUBLIC);
  _c_.addMethod("print", "", "std::basic_ostream<char,std::char_traits<char> >&=", method_6557, PUBLIC);
  _c_.addMethod("CLHEP::HepLorentzVector", "", "CLHEP::HepLorentzVector", converter_6558, PUBLIC);
  _c_.addMethod("momentum", "", "CLHEP::HepLorentzVector", method_6559, PUBLIC);
  _c_.addMethod("pdg_id", "", "int", method_6560, PUBLIC);
  _c_.addMethod("status", "", "int", method_6561, PUBLIC);
  _c_.addMethod("flow", "", "HepMC::Flow", method_6562, PUBLIC);
  _c_.addMethod("flow", "", "int", "int", method_6563, PUBLIC);
  _c_.addMethod("polarization", "", "HepMC::Polarization", method_6564, PUBLIC);
  _c_.addMethod("production_vertex", "", "HepMC::GenVertex*", method_6565, PUBLIC);
  _c_.addMethod("end_vertex", "", "HepMC::GenVertex*", method_6566, PUBLIC);
  _c_.addMethod("parent_event", "", "HepMC::GenEvent*", method_6567, PUBLIC);
  _c_.addMethod("barcode", "", "int", method_6568, PUBLIC);
  _c_.addMethod("suggest_barcode", "", "bool", "int", method_6569, PUBLIC);
  _c_.addMethod("set_momentum", "", "CLHEP::HepLorentzVector&", method_6570, PUBLIC);
  _c_.addMethod("set_pdg_id", "", "int", method_6571, PUBLIC);
  _c_.addMethod("set_status", "", "int=", method_6572, PUBLIC);
  _c_.addMethod("set_flow", "", "HepMC::Flow&", method_6573, PUBLIC);
  _c_.addMethod("set_flow", "", "int; int=", method_6574, PUBLIC);
  _c_.addMethod("set_polarization", "", "HepMC::Polarization&=", method_6575, PUBLIC);
  _c_.build();
}

//------Stub functions for class GenParticle -------------------------------
void* HepMC__GenParticle_dict::constructor_6550( void* mem) {
  return ::new(mem) HepMC::GenParticle();
}

void* HepMC__GenParticle_dict::constructor_6551( void* mem, const std::vector<void*>& arg) {
  if ( arg.size() == 2 ) {
    return ::new(mem) HepMC::GenParticle(*(const CLHEP::HepLorentzVector*)arg[0],
                                       *(int*)arg[1]);
  }
  else if ( arg.size() == 3 ) { 
    return ::new(mem) HepMC::GenParticle(*(const CLHEP::HepLorentzVector*)arg[0],
                                       *(int*)arg[1],
                                       *(int*)arg[2]);
  }
  else if ( arg.size() == 4 ) { 
    return ::new(mem) HepMC::GenParticle(*(const CLHEP::HepLorentzVector*)arg[0],
                                       *(int*)arg[1],
                                       *(int*)arg[2],
                                       *(const HepMC::Flow*)arg[3]);
  }
  else if ( arg.size() == 5 ) { 
    return ::new(mem) HepMC::GenParticle(*(const CLHEP::HepLorentzVector*)arg[0],
                                       *(int*)arg[1],
                                       *(int*)arg[2],
                                       *(const HepMC::Flow*)arg[3],
                                       *(const HepMC::Polarization*)arg[4]);
  }
  return 0;
}

void* HepMC__GenParticle_dict::constructor_6552( void* mem, const std::vector<void*>& arg) {
  return ::new(mem) HepMC::GenParticle(*(const HepMC::GenParticle*)arg[0]);
}

void* HepMC__GenParticle_dict::operator_6554( void* o, const std::vector<void*>& arg)
{
  return (void*)&((HepMC::GenParticle*)o)->operator =(*(const HepMC::GenParticle*)arg[0]);
}

void* HepMC__GenParticle_dict::operator_6555( void* o, const std::vector<void*>& arg)
{
  static bool ret;
  ret = ((const HepMC::GenParticle*)o)->operator ==(*(const HepMC::GenParticle*)arg[0]);
  return &ret;
}

void* HepMC__GenParticle_dict::operator_6556( void* o, const std::vector<void*>& arg)
{
  static bool ret;
  ret = ((const HepMC::GenParticle*)o)->operator !=(*(const HepMC::GenParticle*)arg[0]);
  return &ret;
}

void HepMC__GenParticle_dict::method_6557( void* o, const std::vector<void*>& arg)
{
  if ( arg.size() == 0 ) {
    ((const HepMC::GenParticle*)o)->print();
  }
  else if ( arg.size() == 1 ) { 
    ((const HepMC::GenParticle*)o)->print(*(std::basic_ostream<char,std::char_traits<char> >*)arg[0]);
  }
}

void* HepMC__GenParticle_dict::converter_6558( void* o)
{
  return new CLHEP::HepLorentzVector(((const HepMC::GenParticle*)o)->operator CLHEP::HepLorentzVector());
}

void* HepMC__GenParticle_dict::method_6559( void* o)
{
  return new CLHEP::HepLorentzVector(((const HepMC::GenParticle*)o)->momentum());
}

void* HepMC__GenParticle_dict::method_6560( void* o)
{
  static int ret;
  ret = ((const HepMC::GenParticle*)o)->pdg_id();
  return &ret;
}

void* HepMC__GenParticle_dict::method_6561( void* o)
{
  static int ret;
  ret = ((const HepMC::GenParticle*)o)->status();
  return &ret;
}

void* HepMC__GenParticle_dict::method_6562( void* o)
{
  return new HepMC::Flow(((const HepMC::GenParticle*)o)->flow());
}

void* HepMC__GenParticle_dict::method_6563( void* o, const std::vector<void*>& arg)
{
  static int ret;
  ret = ((const HepMC::GenParticle*)o)->flow(*(int*)arg[0]);
  return &ret;
}

void* HepMC__GenParticle_dict::method_6564( void* o)
{
  return new HepMC::Polarization(((const HepMC::GenParticle*)o)->polarization());
}

void* HepMC__GenParticle_dict::method_6565( void* o)
{
  return (void*)((const HepMC::GenParticle*)o)->production_vertex();
}

void* HepMC__GenParticle_dict::method_6566( void* o)
{
  return (void*)((const HepMC::GenParticle*)o)->end_vertex();
}

void* HepMC__GenParticle_dict::method_6567( void* o)
{
  return (void*)((const HepMC::GenParticle*)o)->parent_event();
}

void* HepMC__GenParticle_dict::method_6568( void* o)
{
  static int ret;
  ret = ((const HepMC::GenParticle*)o)->barcode();
  return &ret;
}

void* HepMC__GenParticle_dict::method_6569( void* o, const std::vector<void*>& arg)
{
  static bool ret;
  ret = ((HepMC::GenParticle*)o)->suggest_barcode(*(int*)arg[0]);
  return &ret;
}

void HepMC__GenParticle_dict::method_6570( void* o, const std::vector<void*>& arg)
{
  ((HepMC::GenParticle*)o)->set_momentum(*(const CLHEP::HepLorentzVector*)arg[0]);
}

void HepMC__GenParticle_dict::method_6571( void* o, const std::vector<void*>& arg)
{
  ((HepMC::GenParticle*)o)->set_pdg_id(*(int*)arg[0]);
}

void HepMC__GenParticle_dict::method_6572( void* o, const std::vector<void*>& arg)
{
  if ( arg.size() == 0 ) {
    ((HepMC::GenParticle*)o)->set_status();
  }
  else if ( arg.size() == 1 ) { 
    ((HepMC::GenParticle*)o)->set_status(*(int*)arg[0]);
  }
}

void HepMC__GenParticle_dict::method_6573( void* o, const std::vector<void*>& arg)
{
  ((HepMC::GenParticle*)o)->set_flow(*(const HepMC::Flow*)arg[0]);
}

void HepMC__GenParticle_dict::method_6574( void* o, const std::vector<void*>& arg)
{
  if ( arg.size() == 1 ) {
    ((HepMC::GenParticle*)o)->set_flow(*(int*)arg[0]);
  }
  else if ( arg.size() == 2 ) { 
    ((HepMC::GenParticle*)o)->set_flow(*(int*)arg[0],
                                       *(int*)arg[1]);
  }
}

void HepMC__GenParticle_dict::method_6575( void* o, const std::vector<void*>& arg)
{
  if ( arg.size() == 0 ) {
    ((HepMC::GenParticle*)o)->set_polarization();
  }
  else if ( arg.size() == 1 ) { 
    ((HepMC::GenParticle*)o)->set_polarization(*(const HepMC::Polarization*)arg[0]);
  }
}


//---------  Bootstrap for building classes ------

namespace {
    struct Dictionaries {
	Dictionaries() {
            HepMC__GenParticle_dict dict1;
	    HepMC__GenVertex_dict dict2;
	    Class::resolvePending();
	}
    };
  static Dictionaries instance;
}


//---------  Capabilities specification ------


// This file has been generated by lcgdict with the --capabilities option
static  const char* clnames[] = {
//--Begin AtlasSealCLHEPDictFix
    "LCGDict/HepMC::GenParticle",
    "LCGDict/HepMC::GenVertex"
//--End   AtlasSealCLHEPDict
//--Final End
};

extern "C" void SEAL_CAPABILITIES (const char**& names, int& n )
{ 
  names = clnames;
  n = sizeof(clnames)/sizeof(char*);
}
