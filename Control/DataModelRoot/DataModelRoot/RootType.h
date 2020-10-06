/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELROOT_ROOT_TYPE_H
#define DATAMODELROOT_ROOT_TYPE_H

// Standard
#include <string>

// ROOT
#include "TClassRef.h"
class TMethod;
class TFunction;
class TMethodArg;

#include "CxxUtils/checker_macros.h"

#define ROOT_6

namespace Reflex {

   /** enum for printing names */
      enum ENTITY_HANDLING {
         FINAL     = (1<<0),
         QUALIFIED = (1<<1),
         SCOPED    = (1<<2),
      };

} // namespace Reflex


class TScopeAdapter;
typedef TScopeAdapter TTypeAdapter;


class TPropertyListAdapter {
public:
   TPropertyListAdapter( TDictAttributeMap* dam ) : fAttributes( dam ) {}

public:
   Bool_t HasProperty( const std::string& key ) const;
   std::string PropertyAsString(const std::string& key) const;

private:
   TDictAttributeMap* fAttributes;
};

class TReturnTypeAdapter {
public:
   TReturnTypeAdapter( const std::string& name ) : fName( name ) {}

   std::string Name( unsigned int mod = 0 ) const;

private:
   std::string fName;
};


class TMemberAdapter {
public:
   TMemberAdapter( TMethod* meth );
   operator TMethod*() const;

   TMemberAdapter( TFunction* func );
   operator TFunction*() const;

   TMemberAdapter( TDataMember* mb );
   operator TDataMember*() const;

   TMemberAdapter( TMethodArg* ma );
   operator TMethodArg*() const;

   operator Bool_t() const { return fMember != 0; }

public:
   std::string Name( unsigned int mod = 0 ) const;

   const char* GetSharedLibs() const;
   size_t Offset() const;

   Bool_t IsConstant() const;
   Bool_t IsConstructor() const;
   Bool_t IsEnum() const;
   Bool_t IsPublic() const;
   Bool_t IsStatic() const;
   Bool_t IsTransient() const;

   size_t FunctionParameterSize( Bool_t required = false ) const;
   TMemberAdapter FunctionParameterAt( size_t nth ) const;
   std::string FunctionParameterNameAt( size_t nth ) const;
   std::string FunctionParameterDefaultAt( size_t nth ) const;

   TReturnTypeAdapter   ReturnType() const;
   TScopeAdapter        DeclaringScope ATLAS_NOT_THREAD_SAFE () const;
   TTypeAdapter         DeclaringType ATLAS_NOT_THREAD_SAFE () const;
   TTypeAdapter         TypeOf ATLAS_NOT_THREAD_SAFE () const;

private:
   TDictionary* fMember;
};


class TBaseAdapter {
public:
   TBaseAdapter( TBaseClass* base ) : fBase( base ) {}
   operator Bool_t() const { return fBase != 0; }

public:
   std::string Name() const;

   TScopeAdapter ToType ATLAS_NOT_THREAD_SAFE () const;

private:
   TBaseClass* fBase;
};


class TScopeAdapter {
public:
   TScopeAdapter();
   TScopeAdapter( TClass* klass );
   TScopeAdapter( const std::string& name, Bool_t load = kTRUE );
   ATLAS_NOT_THREAD_SAFE TScopeAdapter( const std::string& name, Bool_t load, Bool_t quiet );
   TScopeAdapter( const TMemberAdapter& );
   TScopeAdapter( const std::type_info &typeinfo );
   operator TClass*() const { return fClass.GetClass(); }
   operator Bool_t() const;

public:
   static TScopeAdapter ByName ATLAS_NOT_THREAD_SAFE (
      const std::string& name, Bool_t load = kTRUE, Bool_t quiet = kTRUE );
   static TScopeAdapter ByNameNoQuiet(
      const std::string& name, Bool_t load = kTRUE );

   static TScopeAdapter TypeAt( size_t nth );
   static size_t TypeSize ();

public:
   std::string Name( unsigned int mod = Reflex::SCOPED ) const;
   TScopeAdapter DeclaringScope() const;

   void *Construct() const ;
   void *Construct(void *place) const;
   void  Destruct(void *place) const;

   const std::type_info& TypeInfo() const;
   TPropertyListAdapter Properties() const;

   Bool_t IsPointer() const;
   Bool_t IsTemplateInstance() const;
   Bool_t IsTopScope() const;
   Bool_t IsFundamental() const;
   Bool_t IsEnum() const;
   Bool_t IsTypedef() const;
   Bool_t IsArray() const;

   TBaseAdapter BaseAt( size_t nth ) const;
   size_t BaseSize() const;
  
   size_t SizeOf() const;

   TMemberAdapter FunctionMemberAt( size_t nth ) const;
   size_t FunctionMemberSize() const;

   TMemberAdapter DataMemberAt( size_t nth ) const;
   size_t DataMemberSize() const;

   TScopeAdapter TemplateArgumentAt( size_t nth ) const;
   size_t TemplateArgumentSize() const;

public:
   Bool_t IsComplete() const;

   Bool_t IsClass() const;
   Bool_t IsStruct() const;
   Bool_t IsNamespace() const;

// note: in Reflex, part of Type, not Scope
   Bool_t IsAbstract() const;

   void* Id() const { return fClass.GetClass(); }
   TClass* Class() const { return fClass.GetClass(); }

   TTypeAdapter ToType() const { return *this; }
   TScopeAdapter TypeOf() const { return *this; }

   /// Cast obj of this class type up to baseclass cl if up is true.
   /// Cast obj of this class type down from baseclass cl if up is false.
   /// If this class is not a baseclass of cl return 0, else the pointer
   /// to the cl part of this (up) or to this (down).
   void* Cast (const TScopeAdapter& cl, void* obj, bool up) const;

// for STL containers
   bool operator==( const TScopeAdapter& rh ) const;
   bool operator!=( const TScopeAdapter& rh ) const {
      return ! (*this == rh);
   }
   bool operator<( const TScopeAdapter& rh ) const;

  // Noop in ROOT6
   static void EnableCintex() {}
  
private:
   void Init ( const std::string& name, Bool_t load, Bool_t quiet );

   std::string  fName;
   TClassRef    fClass;
   bool         fIsFundamental = false;
};


typedef TBaseAdapter              RootBase;
typedef TMemberAdapter            RootDataMember;
typedef TPropertyListAdapter      RootPropertyList;
typedef TScopeAdapter             RootScope;
typedef TTypeAdapter              RootType;


#endif // !DATAMODELROOT_ROOT_TYPE_H
