/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELROOT_ROOT_TYPE_H
#define DATAMODELROOT_ROOT_TYPE_H

// ROOT
#include "TClassRef.h"
class TBaseClass;
class TDictionary;
class TDictAttributeMap;
class TMethod;
class TFunction;
class TDataMember;
class TMethodArg;

#include "RVersion.h"

#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
#include "Reflex/Kernel.h"
#include "Reflex/Type.h"
#include "Reflex/Object.h"
#include "Reflex/Member.h"

namespace std { class type_info; }

class RootType : public Reflex::Type {
public:
  RootType(const Reflex::Type& rh) : Reflex::Type(rh) {}
  RootType(const Reflex::TypeName* typName = 0, unsigned int modifiers = 0) :
        Reflex::Type(typName, modifiers) {}
  /// Better constructors
  RootType(const std::string& name) : Reflex::Type(Reflex::Type::ByName(name)) {}
  RootType(const std::type_info& info) : Reflex::Type(Reflex::Type::ByTypeInfo(info)) {}
  /// Standard destructor
  virtual ~RootType() {}

  void AddProperty(const char* key, const char* value) const {
     this->Properties().AddProperty(key, value);
  }
  std::string Name() const { return this->Reflex::Type::Name(Reflex::SCOPED); }
  std::string Name(int flags) const { return this->Reflex::Type::Name(flags); }

  using Type::operator=;
  
  /// call Cintex::Enable()
  static void EnableCintex();
};

typedef Reflex::Member RootDataMember;
typedef Reflex::Object RootObject;
typedef Reflex::PropertyList RootPropertyList;

#else   // ROOT 6
#define ROOT_6

namespace Reflex {

   /** enum for printing names */
      enum ENTITY_HANDLING {
         FINAL     = (1<<0),
         QUALIFIED = (1<<1),
         SCOPED    = (1<<2),
      };

} // namespace Reflex

// Standard
#include <string>


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
   TScopeAdapter        DeclaringScope() const;
   TTypeAdapter         DeclaringType() const;
   TTypeAdapter         TypeOf() const;

private:
   TDictionary* fMember;
};


class TBaseAdapter {
public:
   TBaseAdapter( TBaseClass* base ) : fBase( base ) {}
   operator Bool_t() const { return fBase != 0; }

public:
   std::string Name() const;

   TScopeAdapter ToType() const;

private:
   TBaseClass* fBase;
};


class TScopeAdapter {
public:
   TScopeAdapter();
   TScopeAdapter( TClass* klass );
   TScopeAdapter( const std::string& name, Bool_t load = kTRUE, Bool_t quiet = kFALSE );
   TScopeAdapter( const TMemberAdapter& );
   TScopeAdapter( const std::type_info &typeinfo );
   operator TClass*() const { return fClass.GetClass(); }
   operator Bool_t() const;

public:
   static TScopeAdapter ByName(
      const std::string& name, Bool_t load = kTRUE, Bool_t quiet = kTRUE );

   static TScopeAdapter TypeAt( size_t nth );
   static size_t TypeSize();

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

// for STL containers
   bool operator==( const TScopeAdapter& rh ) const;
   bool operator!=( const TScopeAdapter& rh ) const {
      return ! (*this == rh);
   }
   bool operator<( const TScopeAdapter& rh ) const;

  // Noop in ROOT6
   static void EnableCintex() {}
  
private:
   std::string  fName;
   TClassRef    fClass;
   bool         isFundamental = false;
};


typedef TBaseAdapter              RootBase;
typedef TMemberAdapter            RootDataMember;
typedef TPropertyListAdapter      RootPropertyList;
typedef TScopeAdapter             RootScope;
typedef TTypeAdapter              RootType;


//MN: ad-hoc simplistic  reimplementation of Reflex::Object
class RootObject 
{
public:
  RootObject(const RootType& type=RootType(), void* obj=0);
  
  void*                 Address() const  { return m_object; }
  const RootType&       Type() const { return m_type; }
  RootObject            CastObject( const RootType &toType) const;
 
private:
  RootType      m_type;
  void*         m_object = 0;
  std::string   m_objectName;   ///not used yet
};



#endif  // ROOT ver

#endif // !DATAMODELROOT_ROOT_TYPE_H
