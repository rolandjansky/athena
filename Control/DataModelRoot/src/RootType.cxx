/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DataModelRoot/RootType.h"

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,99,0)

// ROOT  
#include "TBaseClass.h"
#include "TClass.h"
#include "TClassEdit.h"
#include "TClassTable.h"
#include "TDataType.h"
#include "TDataMember.h"
#include "TDictAttributeMap.h"
#include "TError.h"
#include "TFunction.h"
#include "TInterpreter.h"
#include "TApplication.h"
#include "TMethod.h"
#include "TMethodArg.h"
#include "TROOT.h"
   
// Standard
#include <assert.h>
#include <iostream>
using namespace std;

   
//= helpers ==================================================================
static const std::string ResolveTypedef( const std::string& tname )
{
// Helper; captures common code needed to find the real class name underlying
// a typedef (if any).
   std::string tclean = TClassEdit::CleanType( tname.c_str() );
   
   TDataType* dt = gROOT->GetType( tclean.c_str() );
   if ( dt ) return dt->GetFullTypeName();

   return TClassEdit::ResolveTypedef( tclean.c_str(), true );
}

static inline const std::string UnqualifiedTypeName( const std::string name )
{
    return TClassEdit::ShortType(
         TClassEdit::CleanType( name.c_str(), 1 ).c_str(), 5 );
}

namespace {
// MN:  Without creating TApplication ROOT autoloading does not work
   static const char* argv[] = { "RootType", "-b" };
   static int initialized(false);
   // use initialized to prevent recursive entry from TApplication Xtor
   inline void InitROOT()   
   {
      if( !initialized && !gApplication ) {
         int argc = 2;
         static  TApplication dummy(argv[0], &argc, (char**)argv);
         gApplication = &dummy;
      }
      //gDebug=1;
   }
}


//= TPropertListAdapter ======================================================
static std::string map_property( const std::string& key )
{
    if ( key == "ClassID" ) return "id";
    return key;
}

//____________________________________________________________________________
Bool_t TPropertyListAdapter::HasProperty( const std::string& key ) const
{
    if ( fAttributes )
       return fAttributes->GetPropertyAsString( map_property( key ).c_str() );
    return kFALSE;
}


//____________________________________________________________________________
std::string TPropertyListAdapter::PropertyAsString( const std::string& key ) const
{
    if ( fAttributes )
        return fAttributes->GetPropertyAsString( map_property( key ).c_str() );
    return "";
}


//= TReturnTypeAdapter =======================================================
std::string TReturnTypeAdapter::Name( unsigned int mod ) const
{
// get the name of the return type that is being adapted
   std::string name = fName;

   if ( mod & Reflex::FINAL )
      name = ResolveTypedef( name );

   if ( ! ( mod & Reflex::QUALIFIED ) )
      name = UnqualifiedTypeName( fName );

   return name;
}


//= TMemberAdapter ===========================================================
TMemberAdapter::TMemberAdapter( TMethod* meth ) : fMember( meth )
{
   /* empty */
}

//____________________________________________________________________________
TMemberAdapter::operator TMethod*() const
{
// cast the adapter to a TMethod* being adapted, returns 0 on failure
   return dynamic_cast< TMethod* >( const_cast< TDictionary* >( fMember ) );
}

//____________________________________________________________________________
TMemberAdapter::TMemberAdapter( TFunction* func ) : fMember( func )
{
   /* empty */
}

//____________________________________________________________________________
TMemberAdapter::operator TFunction*() const
{
// cast the adapter to a TFunction* being adapted, returns 0 on failure
   return dynamic_cast< TFunction* >( const_cast< TDictionary* >( fMember ) );
}

//____________________________________________________________________________
TMemberAdapter::TMemberAdapter( TDataMember* mb ) : fMember( mb )
{
   /* empty */
}

//____________________________________________________________________________
TMemberAdapter::operator TDataMember*() const
{
// cast the adapter to a TDataMember* being adapted, returns 0 on failure
   return dynamic_cast< TDataMember* >( const_cast< TDictionary* >( fMember ) );
}

//____________________________________________________________________________
TMemberAdapter::TMemberAdapter( TMethodArg* ma ) : fMember( ma )
{
   /* empty */
}

//____________________________________________________________________________
TMemberAdapter::operator TMethodArg*() const
{
// cast the adapter to a TMethodArg* being adapted, returns 0 on failure
   return dynamic_cast< TMethodArg* >( const_cast< TDictionary* >( fMember ) );
}

//____________________________________________________________________________
std::string TMemberAdapter::Name( unsigned int mod ) const
{
// return name of the type described by fMember
   TMethodArg* arg = (TMethodArg*)*this;

   if ( arg ) {

      std::string name = arg->GetTypeNormalizedName();
      if ( mod & Reflex::FINAL )
         name = ResolveTypedef( name );

      if ( ! ( mod & Reflex::QUALIFIED ) )
         name = UnqualifiedTypeName( name );

      return name;

   } else if ( mod & Reflex::FINAL )
      return ResolveTypedef( fMember->GetName() );

   if ( fMember )
      return fMember->GetName();
   return "<unknown>";   // happens for classes w/o dictionary
}

//____________________________________________________________________________
const char* TMemberAdapter::GetSharedLibs() const
{
// return the shared libs corresponding to this classes (should be 'lib'?)
   return gInterpreter->GetClassSharedLibs( Name().c_str() );
}

//____________________________________________________________________________
size_t TMemberAdapter::Offset() const
{
// relative offset (instance data) or global pointer (static/global data
   TDataMember* dm = (TDataMember*)*this;
   return dm ? dm->GetOffsetCint() : 0;
}

//____________________________________________________________________________
Bool_t TMemberAdapter::IsConstant() const
{
// test if the adapted member is a const method
   return fMember->Property() & kIsConstMethod;
}

//____________________________________________________________________________
Bool_t TMemberAdapter::IsConstructor() const
{
// test if the adapted member is a const method
   return ((TFunction*)fMember) ? (((TFunction*)fMember)->ExtraProperty() & kIsConstructor) : kFALSE;
}

//____________________________________________________________________________
Bool_t TMemberAdapter::IsEnum() const
{
// test if the adapted member is of an enum type
   return fMember->Property() & kIsEnum;
}

//____________________________________________________________________________
Bool_t TMemberAdapter::IsPublic() const
{
// test if the adapted member represents an public (data) member
   return fMember->Property() & kIsPublic;
}

//____________________________________________________________________________
Bool_t TMemberAdapter::IsStatic() const
{
// test if the adapted member represents a class (data) member
   return fMember->Property() & kIsStatic;
}

//____________________________________________________________________________
Bool_t TMemberAdapter::IsTransient() const
{
// test if the adapted member represents transient data
   TDataMember* dm = (TDataMember*)*this;
   return dm ? ! dm->IsPersistent() : kTRUE;
}

//____________________________________________________________________________
size_t TMemberAdapter::FunctionParameterSize( Bool_t required ) const
{
// get the total number of parameters that the adapted function/method takes
   TFunction* func = (TFunction*)fMember;
   if ( ! func )
      return 0;

   if ( required == true )
      return func->GetNargs() - func->GetNargsOpt();

   return func->GetNargs();
}

//____________________________________________________________________________
TMemberAdapter TMemberAdapter::FunctionParameterAt( size_t nth ) const
{
// get the type info of the function parameter at position nth
   return (TMethodArg*)((TFunction*)fMember)->GetListOfMethodArgs()->At( nth );
}

//____________________________________________________________________________
std::string TMemberAdapter::FunctionParameterNameAt( size_t nth ) const
{
// get the formal name, if available, of the function parameter at position nth
   const char* name =
      ((TMethodArg*)((TFunction*)fMember)->GetListOfMethodArgs()->At( nth ))->GetName();

   if ( name )
      return name;
   return "";
}

//____________________________________________________________________________
std::string TMemberAdapter::FunctionParameterDefaultAt( size_t nth ) const
{
// get the default value, if available, of the function parameter at position nth
   TMethodArg* arg = (TMethodArg*)((TFunction*)fMember)->GetListOfMethodArgs()->At( nth );
   const char* def = arg->GetDefault();

   if ( ! def )
      return "";

// special case for strings: "some value" -> ""some value"
   if ( strstr( ResolveTypedef( arg->GetTypeNormalizedName() ).c_str(), "char*" ) ) {
      std::string sdef = "\"";
      sdef += def;
      sdef += "\"";
      return sdef;
   }

   return def;
}

//____________________________________________________________________________
TReturnTypeAdapter TMemberAdapter::ReturnType() const
{
// get the return type of the wrapped function/method
   return TReturnTypeAdapter( ((TFunction*)fMember)->GetReturnTypeNormalizedName() );
}

//____________________________________________________________________________
TScopeAdapter TMemberAdapter::DeclaringScope() const
{
// get the declaring scope (class) of the wrapped function/method
   TMethod* method = (TMethod*)*this;
   if ( method )
      return method->GetClass();

// happens for free-standing functions (i.e. global scope)
   return std::string( "" );
}

TTypeAdapter TMemberAdapter::DeclaringType() const
{
// no distinction between scope/type
    return DeclaringScope();
}


//= TBaseAdapter =============================================================
std::string TBaseAdapter::Name() const
{
// get the name of the base class that is being adapted
   return fBase->GetName();
}

//____________________________________________________________________________
TScopeAdapter TBaseAdapter::ToType() const
{
// wrap the actual class representing this base
   return TScopeAdapter( fBase->GetClassPointer() );
}


//= TScopeAdapter ============================================================
//============================================================================
TScopeAdapter::TScopeAdapter()
{
   /* empty */
}

TScopeAdapter::TScopeAdapter( TClass* klass ) : fClass( klass )
{
// wrap a class (scope)
   if ( fClass.GetClass() != 0 )
      fName = fClass->GetName();
}

//____________________________________________________________________________
TScopeAdapter::TScopeAdapter( const std::string& name ) :
      fName( name )
{
   InitROOT();
   cout << "RootType: creating for type=" << name << endl;
   fClass = TClassRef( Name( Reflex::SCOPED ).c_str() );
   if( fClass.GetClass() ) {
      cout << "INFO: RootType::RootType(): Got ROOT dictionary for <" << name << "> class" << endl;
   } else  {
      cout << "ERROR!: RootType::RootType(): Cannot get the ROOT dictionary for <" << name << "> class" << endl;
      exit(0);
   }  
}


//____________________________________________________________________________
TScopeAdapter::TScopeAdapter( const std::type_info &typeinfo )      
{
   InitROOT();
   fClass = TClassRef( TClass::GetClass(typeinfo) );   // MN: is that right?
   if( fClass.GetClass() )
      fName = fClass->GetName();
}

//____________________________________________________________________________
TScopeAdapter::TScopeAdapter( const TMemberAdapter& mb ) :
      fName( mb.Name( Reflex::QUALIFIED | Reflex::SCOPED ) ),
      fClass( mb.Name( Reflex::SCOPED ).c_str() )
{
   /* empty */
}

//____________________________________________________________________________
TScopeAdapter TScopeAdapter::ByName(
      const std::string& name, Bool_t load, Bool_t quiet )
{
   InitROOT();
   // lookup a scope (class) by name
   if ( ! load ) {
   // let GetClass() have a crack at it first, to prevent accidental loading
      TClass* klass = TClass::GetClass( name.c_str(), load, quiet );
      if ( ! klass ) return TScopeAdapter();
      return klass;
   }

   Int_t oldEIL = gErrorIgnoreLevel;
   if ( quiet )
      gErrorIgnoreLevel = 3000;

   TClassRef klass( name.c_str() );
   if (klass.GetClass() && klass->GetListOfAllPublicMethods()->GetSize() == 0) {
   // sometimes I/O interferes, leading to zero methods: reload from CINT
      ClassInfo_t* cl = gInterpreter->ClassInfo_Factory( name.c_str() );
      if ( cl ) {
         gInterpreter->SetClassInfo( klass, kTRUE );
         gInterpreter->ClassInfo_Delete(cl);
      }
   }

   gErrorIgnoreLevel = oldEIL;

   return klass.GetClass();
}

//____________________________________________________________________________
TScopeAdapter TScopeAdapter::TypeAt( size_t nth )
{
// this is dreadful, but is the way checkDictionary() works ... it better make
// sure that it does not load extra classes inside its loop (seems to be how
// it should be, but is not how it is)
   return TScopeAdapter( gClassTable->At( nth ) ); 
}

//____________________________________________________________________________
size_t TScopeAdapter::TypeSize()
{
// return total number of types in the system (this is stupid, as the number
// of types is dynamic ...)
   return gClassTable ? gClassTable->Classes() : 0;
}

//____________________________________________________________________________
std::string TScopeAdapter::Name( unsigned int mod ) const
{
// Return name of type described by fClass
   if ( ! fClass.GetClass() || ! fClass->Property() ) {
   // fundamental types have no class, and unknown classes have no property
      std::string name = fName;

      if ( mod & Reflex::FINAL )
         name = ResolveTypedef( name );

      if ( ! ( mod & Reflex::QUALIFIED ) )
         name = UnqualifiedTypeName( fName );

      return name;
   }

   std::string name = fClass->GetName();
   if ( mod & Reflex::FINAL )
      name = ResolveTypedef( name );

   if ( ! (mod & Reflex::SCOPED) ) {
   // remove scope from the name
      Int_t tpl_open = 0;
      for ( std::string::size_type pos = name.size() - 1; 0 < pos; --pos ) {
         std::string::value_type c = name[ pos ];

      // count '<' and '>' to be able to skip template contents
         if ( c == '>' )
            ++tpl_open;
         else if ( c == '<' )
            --tpl_open;
         else if ( tpl_open == 0 && c == ':' && 0 < pos && name[ pos-1 ] == ':' ) {
         // found scope, strip name from it
            name = name.substr( pos+1, std::string::npos );
            break;
         }
      }
   }

   return name;
}

//____________________________________________________________________________
TScopeAdapter TScopeAdapter::DeclaringScope() const
{
   std::string name = Name( Reflex::FINAL | Reflex::SCOPED );
   std::string::size_type pos = name.rfind( "::" );
   if ( pos == std::string::npos )
      return TScopeAdapter(  /* wrap global scope */
         TClass::GetClass( gInterpreter->ClassInfo_Factory( "" ) ) );
   return TScopeAdapter( name.substr( 0, pos ) );
}

//____________________________________________________________________________
void* TScopeAdapter::Construct() const
{
   return fClass.GetClass()? fClass.GetClass()->New() : 0;
}

//____________________________________________________________________________
void *RootType::Construct(void *place) const {
   return (place && fClass.GetClass())? fClass.GetClass()->New(place) : 0;
}


//____________________________________________________________________________
const type_info& TScopeAdapter::TypeInfo() const
{
   return  *fClass.GetClass()->GetTypeInfo();
}


//____________________________________________________________________________
TPropertyListAdapter TScopeAdapter::Properties() const
{
// Reflex properties are more or less related to ROOT attributes: the names
// may be different (see map_property() above)
   return fClass->GetAttributeMap();
}

//____________________________________________________________________________
Bool_t TScopeAdapter::IsPointer() const
{
// the assumption here is that the adapter can be initialized with a sugared
// name, e.g. for function arguments
   return *fName.rbegin() == '*';
}

//____________________________________________________________________________
Bool_t TScopeAdapter::IsTemplateInstance() const
{
// this concept differs quite a bit between ROOT/meta and Reflex; this code
// supports both, with the addendum that the ROOT/meta idea of templates will
// not have template arguments
   if ( Name( Reflex::FINAL ).find('<') != std::string::npos )
      return kTRUE;          // Reflex notion of templates

   if ( gInterpreter->CheckClassTemplate( Name( Reflex::SCOPED ).c_str() ) )
      return kTRUE;

   return kFALSE;
}

//____________________________________________________________________________
Bool_t TScopeAdapter::IsTopScope() const
{
// check whether this corresponds to the global scope
   return fClass->GetClassInfo() == gInterpreter->ClassInfo_Factory( "" );
}

Bool_t TScopeAdapter::IsFundamental() const
{
   return fClass.GetClass()? fClass.GetClass()->Property() & kIsFundamental : false;
}

//____________________________________________________________________________
size_t TScopeAdapter::BaseSize() const
{
// get the total number of base classes that this class has
   if ( fClass.GetClass() && fClass->GetListOfBases() != 0 )
      return fClass->GetListOfBases()->GetSize();

   return 0;
}


//____________________________________________________________________________
size_t TScopeAdapter::SizeOf() const
{
   return fClass.GetClass()? fClass.GetClass()->Size() : 0;
}

//____________________________________________________________________________
TBaseAdapter TScopeAdapter::BaseAt( size_t nth ) const
{
// get the nth base of this class
   return (TBaseClass*)fClass->GetListOfBases()->At( nth );
}

//____________________________________________________________________________
size_t TScopeAdapter::FunctionMemberSize() const
{
// get the total number of methods that this class has
   if ( fClass.GetClass() )
      return fClass->GetListOfMethods()->GetSize();

   return 0;
}

//____________________________________________________________________________
TMemberAdapter TScopeAdapter::FunctionMemberAt( size_t nth ) const
{
// get the nth method of this class
   return (TMethod*)fClass->GetListOfMethods()->At( nth );
}

//____________________________________________________________________________
size_t TScopeAdapter::DataMemberSize() const
{
// get the total number of data members that this class has
   if ( fClass.GetClass() )
      return fClass->GetListOfDataMembers()->GetSize();

   return 0;
}

//____________________________________________________________________________
TMemberAdapter TScopeAdapter::DataMemberAt( size_t nth ) const
{
// get the nth data member of this class
   return (TDataMember*)fClass->GetListOfDataMembers()->At( nth );
}

//____________________________________________________________________________
TScopeAdapter TScopeAdapter::TemplateArgumentAt( size_t nth ) const
{
// this is stupid, but highly likely never called, and if it is, only once, so
// I don't see a reason for caching to prevent the extra loops
   std::string name = Name( Reflex::FINAL );
   size_t tpl_open = 0, argcount = 0;
   std::string::size_type last = 0;
   for ( std::string::size_type pos = 0; pos < name.size(); ++pos ) {
      std::string::value_type c = name[pos];

      // count '<' and '>' to be able to keep templated args intact
      if ( c == '<' ) {
         ++tpl_open;
         if (tpl_open == 1) last = pos+1;
         continue;
      } else if (c == '>') {
         --tpl_open;

         if ((c == ',' && tpl_open == 1) || (c == '>' && tpl_open == 0)) {
            if ( argcount++ == nth ) {
                std::string part = 
                   TClassEdit::CleanType( name.substr(last, pos-last).c_str(), 1 );
                return part;
            }
            last = pos+1;         // done with part
         }
      }
   }

   // should never be called
   assert( !"miscount between TemplateArgumentSize() and TemplateArgumentAt()" );
   return TScopeAdapter();
}

//____________________________________________________________________________
size_t TScopeAdapter::TemplateArgumentSize() const
{
// this is stupid, too, see TemplateArgumentAt() above
   std::string name = Name( Reflex::FINAL );
   size_t tpl_open = 0, argcount = 0;
   for ( std::string::size_type pos = 0; pos < name.size(); ++pos ) {
      std::string::value_type c = name[pos];

   // count '<' and '>' to be able to keep templated args intact
      if ( c == '<' ) {
          ++tpl_open;
          continue;
      } else if ( c == '>' )
          --tpl_open;

      if ((c == ',' && tpl_open == 1) || (c == '>' && tpl_open == 0))
         ++argcount;
   }
   return argcount;
}

//____________________________________________________________________________
TScopeAdapter::operator Bool_t() const
{
// check the validity of this scope (class)
   if ( fName.empty() )
      return false;

   Bool_t b = kFALSE;

   Int_t oldEIL = gErrorIgnoreLevel;
   gErrorIgnoreLevel = 3000;
   std::string scname = Name( Reflex::SCOPED );
   TClass* klass = TClass::GetClass( scname.c_str() );
   if ( klass && klass->GetClassInfo() )     // works for normal case w/ dict
      b = gInterpreter->ClassInfo_IsValid( klass->GetClassInfo() );
   else {      // special case for forward declared classes
      ClassInfo_t* ci = gInterpreter->ClassInfo_Factory( scname.c_str() );
      if ( ci ) {
         b = gInterpreter->ClassInfo_IsValid( ci );
         gInterpreter->ClassInfo_Delete( ci );    // we own the fresh class info
      }
   }
   gErrorIgnoreLevel = oldEIL;
   return b;
}

//____________________________________________________________________________
Bool_t TScopeAdapter::IsComplete() const
{
// verify whether the dictionary of this class is fully available
   Bool_t b = kFALSE;

   Int_t oldEIL = gErrorIgnoreLevel;
   gErrorIgnoreLevel = 3000;
   std::string scname = Name( Reflex::SCOPED );
   TClass* klass = TClass::GetClass( scname.c_str() );
   if ( klass && klass->GetClassInfo() )     // works for normal case w/ dict
      b = gInterpreter->ClassInfo_IsLoaded( klass->GetClassInfo() );
   else {      // special case for forward declared classes
      ClassInfo_t* ci = gInterpreter->ClassInfo_Factory( scname.c_str() );
      if ( ci ) {
         b = gInterpreter->ClassInfo_IsLoaded( ci );
         gInterpreter->ClassInfo_Delete( ci );    // we own the fresh class info
      }
   }
   gErrorIgnoreLevel = oldEIL;
   return b;
}

//____________________________________________________________________________
Bool_t TScopeAdapter::IsClass() const
{
// test if this scope represents a class
   if ( fClass.GetClass() ) {
   // some inverted logic: we don't have a TClass, but a builtin will be recognized, so
   // if it is NOT a builtin, it is a class or struct (but may be missing dictionary)
      return (fClass->Property() & kIsClass) || ! (fClass->Property() & kIsFundamental);
   }

// no class can mean either is no class (i.e. builtin), or no dict but coming in
// through PyCintex/Reflex ... as a workaround, use TDataTypes that has a full
// enumeration of builtin types
   return TDataType( Name( Reflex::FINAL | Reflex::SCOPED ).c_str() ).GetType() == kOther_t;
}

//____________________________________________________________________________
Bool_t TScopeAdapter::IsStruct() const
{
// test if this scope represents a struct
   if ( fClass.GetClass() ) {
   // same logic as for IsClass() above ...
      return (fClass->Property() & kIsStruct) || ! (fClass->Property() & kIsFundamental);
   }

// same logic as for IsClass() above ...
   return TDataType( Name( Reflex::FINAL | Reflex::SCOPED ).c_str() ).GetType() == kOther_t;
}

//____________________________________________________________________________
Bool_t TScopeAdapter::IsNamespace() const
{
// test if this scope represents a namespace
   if ( fClass.GetClass() )
      return fClass->Property() & kIsNamespace;

   return kFALSE;
}

//____________________________________________________________________________
Bool_t TScopeAdapter::IsAbstract() const
{
// test if this scope represents an abstract class
   if ( fClass.GetClass() )
      return fClass->Property() & kIsAbstract;   // assume set only for classes

   return kFALSE;
}

//____________________________________________________________________________
bool TScopeAdapter::operator==( const TScopeAdapter& rh ) const
{
// comparison operator, used for STL containers (implementation debatable)
   return fClass.GetClass() == rh.fClass.GetClass();
}

//____________________________________________________________________________
bool TScopeAdapter::operator<( const TScopeAdapter& rh ) const
{
// less-than operator, used for STL containers (makes little sense conceptually,
// but this class is used as a key in std::map<>s for some reason; choosing to
// compare on name here, as any simple comparison (e.g. GetClassInfo()) is bound
// to fall apart if classes are unloaded/updated/replaced).
   return Name( Reflex::FINAL | Reflex::SCOPED )
      < rh.Name( Reflex::FINAL | Reflex::SCOPED );
}


//____________________________________________________________________________
//____________________________________________________________________________
RootObject::RootObject(const RootType& type, void* obj)
      : m_type(type), m_object(obj)
{
}
  
//____________________________________________________________________________
RootObject
RootObject::CastObject(const RootType &toType) const
{
   return (m_type.Class() && toType.Class())?
      RootObject( toType, m_type.Class()->DynamicCast(toType.Class(), m_object) )
      : RootObject();
}


#else  // ROOT ver

#include "Cintex/Cintex.h"
void RootType::EnableCintex() {
   ROOT::Cintex::Cintex::Enable();
}

#endif // ROOT ver
