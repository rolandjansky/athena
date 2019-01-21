/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TDVCollectionProxy.cxx 660500 2015-04-14 14:04:12Z krasznaa $

// System include(s):
#include <cassert>

// ROOT include(s):
#include <TError.h>
#include <TClassEdit.h>
#include <TClass.h>
#include <TBaseClass.h>
#include <TCollectionProxyInfo.h>
#include <TSystem.h>
#include <TList.h>

// EDM include(s):
#include "AthContainers/DataVector.h"
#include "CxxUtils/no_sanitize_undefined.h"
#include "CxxUtils/checker_macros.h"

// Local include(s):
#include "xAODCore/tools/TDVCollectionProxy.h"

namespace xAOD {

   /// Find base class offset using Reflex.
   ///
   /// Unlike @c TClass::GetBaseClassOffset, this should properly
   /// handle virtual derivation.  @a cls must have a default constructor.
   ///
   /// @param cls Class in which to find the base.
   /// @param base The base for which to search.
   /// @returns The offset of @a base in @a cls, or -1 if it's not a base.
   int safeGetBaseOffset( TClass* cls, TClass* base ) {

      return cls->GetBaseClassOffset( base );
   }

   /// Find the contained class in a @c DataVector.
   ///
   /// @param dvclass The @c TClass for a @c DataVector class.
   /// @return The @c TClass for the @c DataVector's element type.
   TClass* classFromDVClass( TClass* dvclass ) {

      // Split up the class name into template arguments.
      std::vector< std::string > args;
      int tailloc;
      TClassEdit::GetSplit( dvclass->GetName(), args, tailloc );
      assert( args.size() > 1 );

      // This should be the element type name.
      std::string elname = args[ 1 ];
      assert( elname.size() > 0 );

      // Look up the element class.
      TClass* elclass = TClass::GetClass( elname.c_str() );
      if (!elclass) {
         ::Error( "xAOD::TDVCollectionProxy", "Cannot find class %s",
                  elname.c_str() );
      }

      return elclass;
   }

   /// Find the unique base @c DataVector class.
   ///
   /// Note: @c DataVector\<T> can derive from @c DataVector\<U>.
   /// But there can also be helper classes in between
   /// that start with `DataVector' (note: no `<').
   ///
   /// @param dvclass The @c TClass for a @c DataVector class.
   /// @return The @c TClass for the unique base @c DataVector class.
   ///         Returns 0 if there is no such base, or if it
   ///         is the same as @a dvclass.
   TClass* findDVBase( TClass* dvclass ) {

      TIter next( dvclass->GetListOfBases() );
      while( TBaseClass* bcl = dynamic_cast< TBaseClass* >( next() ) ) {
         TClass* cc = bcl->GetClassPointer();
         if( strncmp( cc->GetName(), "DataVector", 10 ) == 0 ) {
            TClass* bdv = findDVBase( cc );
            if( bdv ) return bdv;
            if( strncmp( cc->GetName(), "DataVector<", 11 ) == 0 )
               return cc;
         }
      }

      return 0;
   }

   /// Find the element type of a @c DataVector class.
   ///
   /// If @a cl is a @c DataVector class, or derives from one, then we return
   /// the name of the element type (including a trailing `*').  Otherwise,
   /// we return an empty string.
   ///
   /// @param cl The @c TClass to test.
   /// @return The name of the element type if @a cl is a DV class, or empty.
   ///
   std::string findDVElement( TClass* cl ) {

      // Make sure that the minimal set of dictionaries are loaded:
      TClass* dummyCl =
         TClass::GetClass( "DataVector<xAOD::TDVCollectionProxyDummy>" );
      if( ! dummyCl ) {
         Error( "xAOD::findDVElement",
                "Couldn't load the dictionary for "
                "DataVector<xAOD::TDVCollectionProxyDummy>" );
         return "";
      }

      // Is this a DV class?
      std::string elt;
      const char* name = cl->GetName();
      if( strncmp( name, "DataVector<", 11 ) == 0 ) {

         elt = std::string( name + 11, strlen( name ) - 11 - 1 );

         // Usually the full DataVector name includes two template
         // arguments. But we only need the first one, before the first comma.
         const std::string::size_type comma = elt.find( "," );
         if( comma != std::string::npos ) {
            elt = elt.substr( 0, comma );
         }

         return elt;
      }

      // Scan base classes too.
      TIter next( cl->GetListOfBases() );
      while( TBaseClass* bcl = dynamic_cast< TBaseClass* >( next() ) ) {
         TClass* cc = bcl->GetClassPointer();
         elt = findDVElement( cc );
         if( ! elt.empty() ) {
            break;
         }
      }

      return elt;
   }

   /// Helper functions for accessing the container data via the proxy.
   class TDVCollectionFuncs {

   public:
      /// @brief The container type.
      /// We alias this with the real vector that lives inside the
      /// @c DataVector.
      typedef std::vector< char* > Cont_t;

      /// Proxy environment buffer.
      ///
      /// The first 64 bytes of the environment structure are a scratch area.
      /// The Root proxies put a collection iterator there, but we instead
      /// lay it out like this.
      ///
      /// This is slightly different in newer versions of root.
      /// Rather than having the 64-byte buffer, Environ takes a template
      /// argument giving the payload.
      ///
      struct TEnvBuff {

         /// The index of the last element retrieved.
         size_t fIndex;

         /// @brief Pointer to the container.
         /// (Note that the object pointer from the environment will point at
         /// the top-level @c DataVector class; this points directly at the vector.)
         Cont_t* fCont;

         /// The last element pointer to have been returned.
         void* fEltPtr;

         /// Offset between the pointer held by the DV and the start of the
         /// object
         int fOffset;

      }; // struct TEnvBuff

      /// The Root proxy environment structure.
      typedef ROOT::TCollectionProxyInfo::Environ< TEnvBuff >  Env_t;

      /// Fetch the container from a proxy environment.
      ///
      /// @param env The proxy environment.
      static Cont_t* cont( void* env ) {

         Env_t& e = *reinterpret_cast< Env_t* >( env );
         TEnvBuff& buff = e.fIterator;
         return buff.fCont;
      }

      /// Return the first element of the container.
      ///
      /// This resets the internal pointer to 0.
      ///
      /// @param env The proxy environment.
      /// @return A pointer to the first element, or 0 if the container is empty.
      static void* first( void* env ) {

         Env_t&  e = *reinterpret_cast< Env_t* >( env );
         Cont_t& c = *cont( env );
         TEnvBuff& buff = e.fIterator;
         buff.fIndex = 0;
         e.fSize  = c.size();
         if( 0 == e.fSize ) {
            return 0;
         }
         char* start = c[ 0 ];
         buff.fEltPtr = start + buff.fOffset;
         return buff.fEltPtr;
      }

      /// Return a following element of the container.
      ///
      /// The internal pointer will be advanced by the value of @c e.idx
      /// (after which @c e.idx will be reset to 0).  A pointer to the element
      /// referenced by this new index will be returned.
      ///
      /// @param env The proxy environment.
      /// @return A pointer to the following element, or 0 if we're past the end.
      static void* next( void* env ) {

         Env_t&  e = *reinterpret_cast< Env_t* >( env );
         Cont_t& c = *cont( env );
         TEnvBuff& buff = e.fIterator;
         buff.fIndex += e.fIdx;
         e.fIdx = 0;
         if( buff.fIndex >= e.fSize ) {
            return 0;
         }
         char* ptr = c[ buff.fIndex ];
         buff.fEltPtr = ptr + buff.fOffset;
         return buff.fEltPtr;
      }

      /// Return the size of the container.
      ///
      /// @param env The proxy environment.
      static void* size( void* env )  {

         Env_t* e = reinterpret_cast< Env_t* >( env );
         e->fSize = cont( env )->size();
         return &( e->fSize );
      }

      /// Erase the container.
      ///
      /// @param env The proxy environment.
      static void* clear( void* env ) {

         cont( env )->clear();
         return 0;
      }

      /// Return a new environment structure.
      static void* create_env() {

         return new Env_t;
      }

      //*************************************************************************
      // These methods are not needed for the xAOD usage,
      // and are not implemented.

      /// Not implemented for xAOD
      // template <typname T> // T will be equal to final class 
      static void resize( void* /*obj*/, size_t /*size*/ )  {
         ::Fatal( "xAOD::TDVCollectionProxy", "resize function not specified!" );
      }

      /// Not implemented for xAOD
      static void* construct( void* /*from*/, size_t /*size*/ )  {
         ::Fatal( "xAOD::TDVCollectionProxy", "construct not implemented" );
         return 0;
      }

      /// Not implemented for xAOD
      static void destruct( void* /*obj*/, size_t /*size*/ )  {
         ::Fatal( "xAOD::TDVCollectionProxy", "destruct not implemented" );
      }

      /// Not implemented for xAOD
      static void* feed( void* /*from*/, void* /*to*/, size_t /*size*/ )  {
         ::Fatal( "xAOD::TDVCollectionProxy", "feed not implemented" );
         return 0;
      }

      /// Not implemented for xAOD
#if ROOT_VERSION_CODE >= ROOT_VERSION( 5, 34, 6 )
      static void* collect( void* /*from*/, void* /*to*/ )  {
         ::Fatal( "xAOD::TDVCollectionProxy", "collect not implemented" );
         return 0;
      }
#else
      static void* collect( void* /*env*/ )  {
         ::Fatal( "xAOD::TDVCollectionProxy", "collect not implemented" );
         return 0;
      }
#endif

   }; // class TDVCollectionFuncs

   /// Constructor.
   ///
   /// @param conttype The name of the container type we're proxying.
   ///                 Note that this may be different from
   ///                 a @c DataVector of @a elttype for classes which
   ///                 derive from @c DataVector.
   ///
   TDVCollectionProxy::TDVCollectionProxy( const char* conttype )
      : TGenCollectionProxy( typeid( DataVector< TDVCollectionProxyDummy > ),
                             sizeof( char* ) ),
        fName( conttype ), fInitialized( kFALSE ),
        fContoff( 0 ), fOffset( 0 ), fEltBase( 0 ), fEltType( 0 ) {

      // Set up the element size.  No offset, since this isn't a map.
      fValDiff        = sizeof( void* );
      fValOffset      = 0;

      // Set up the worker functions.
      fSize.call      = TDVCollectionFuncs::size;
      fNext.call      = TDVCollectionFuncs::next;
      fFirst.call     = TDVCollectionFuncs::first;
      fClear.call     = TDVCollectionFuncs::clear;
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,34,6)
      fCollect        = TDVCollectionFuncs::collect;
#else
      fCollect.call   = TDVCollectionFuncs::collect;
#endif
      fCreateEnv.call = TDVCollectionFuncs::create_env;
      fResize         = TDVCollectionFuncs::resize;
      fConstruct      = TDVCollectionFuncs::construct;
      fDestruct       = TDVCollectionFuncs::destruct;
      fFeed           = TDVCollectionFuncs::feed;

      // Make sure that TGenCollectionProxy knows that it's not
      // fully set up yet:
      if( fValue ) {
#if ROOT_VERSION_CODE > ROOT_VERSION(6,2,5)
         delete fValue.exchange( 0 );
#else
         delete fValue;
         fValue = 0;
#endif // ROOT_VERSION
      }
      if( fVal ) {
         delete fVal;
         fVal = 0;
      }

      // This container pretends to hold objects, not pointers:
      fPointers = kFALSE;

      // Remember that we are not initialized yet:
      fProperties = 0;
   }

   /// Copy constructor.
   ///
   /// @param rhs The proxy to copy.
   TDVCollectionProxy::TDVCollectionProxy( const TDVCollectionProxy& rhs )
      : TGenCollectionProxy( rhs ),
        fName( rhs.fName ),
        fInitialized( rhs.fInitialized ),
        fContoff( rhs.fContoff ),
        fOffset( rhs.fOffset ),
        fEltBase( rhs.fEltBase ),
        fEltType( rhs.fEltType ) {

      // If we're not initialised yet, make sure that the base class is
      // on the same page...
      if( ! fInitialized ) {
         if( fValue ) {
#if ROOT_VERSION_CODE > ROOT_VERSION(6,2,5)
            delete fValue.exchange( 0 );
#else
            delete fValue;
            fValue = 0;
#endif // ROOT_VERSION
         }
         if( fVal ) {
            delete fVal;
            fVal = 0;
         }
      }
   }

   /// Start working with a new collection.
   ///
   /// @param objstart The address of the collection.
   void TDVCollectionProxy::PushProxy NO_SANITIZE_UNDEFINED ( void* objstart ) {

      // Do the base class stuff.
      // This will create an environment buffer if needed.
      ::TGenCollectionProxy::PushProxy( objstart );

      // Save the calculated element offset in the environment buffer.
      TDVCollectionFuncs::TEnvBuff& buff =
         reinterpret_cast< TDVCollectionFuncs::Env_t* >( fEnv )->fIterator;

      // Get the offset needed for the pointer operations:
      buff.fOffset = fOffset;

      // Save the address of the underlying vector of the DataVector.
      // First, adjust the address to the base DataVector.
      char* dvstart = reinterpret_cast< char* >( objstart ) + fContoff;
      // Cast to DV.
      // This gets a ubsan warning about casting between types.
      // However, this is deliberate, so suppress ubsan warnings
      // for this function.
      DataVector< char >* dv =
         reinterpret_cast< DataVector< char >* >( dvstart );
      // Find the underlying vector.
      const std::vector< char* >& vec = dv->stdcont();
      // And store its address.
      std::vector<char*>* vptr ATLAS_THREAD_SAFE =
        const_cast< std::vector< char* >* >( &vec );
      buff.fCont = vptr;

      return;
   }

   /// Clone this object.
   TVirtualCollectionProxy* TDVCollectionProxy::Generate() const {

      return new TDVCollectionProxy( *this );
   }

   /// The resize(...) functions need to be specific for the different classes.
   /// This function is used to supply a specific resize(...) function for this
   /// proxy's class.
   ///
   /// @param func The function to use
   ///
   void TDVCollectionProxy::SetResize( Sizing_t func ) {

      fResize = func;
      return;
   }

   TGenCollectionProxy* TDVCollectionProxy::InitializeEx( Bool_t silent ) {

      // Check if we're initialised already:
      if( fInitialized ) return this;

      // Check for the class's dictionary, and for its element type:
      TClass* cl = TClass::GetClass( fName );
      if( ! cl ) {
         ::Fatal( "xAOD::TDVCollectionProxy::InitializeEx",
                  "Couldn't find dictionary for class %s",
                  fName.Data() );
         return 0;
      }

      // Check if it is a DataVector:
      const std::string eltname = findDVElement( cl );
      if( eltname.empty() ) {
         ::Fatal( "xAOD::TDVCollectionProxy::InitializeEx",
                  "Class \"%s\" doesn't seem to be a DataVector",
                  cl->GetName() );
         return 0;
      }

      // Find the container and element offsets.
      FindOffsets( eltname.c_str(), fName );

      // Set up the element size.  No offset, since this isn't a map.
      fValDiff        = sizeof( void* );
      fValOffset      = 0;

      // Do the base class initialization.
      CheckFunctions();
      TGenCollectionProxy::InitializeEx( silent );
      fSTL_type = TClassEdit::kList;

      // Need to override what that set up for fValue and fVal.
      if( fValue ) {
#if ROOT_VERSION_CODE > ROOT_VERSION(6,2,5)
         delete fValue.exchange( 0 );
#else
         delete fValue;
         fValue = 0;
#endif // ROOT_VERSION
      }
      if( fVal )   delete fVal;
      fValue = new TGenCollectionProxy::Value( eltname.c_str(), false );
      fVal   = new TGenCollectionProxy::Value( *fValue );
      fClass = cl;

      // This container pretends to hold objects, not pointers:
      fPointers = kFALSE;

      // Remember that the initialisation succeeded:
      fInitialized = kTRUE;
#if ROOT_VERSION_CODE <= ROOT_VERSION( 6, 2, 5 )
      fProperties |= kIsInitialized;
#endif // ROOT_VERSION

      return this;
   }

   /// Initialize the cached pointer offsets.
   ///
   /// Suppose we have @c D deriving from @c B, and thus
   /// @c DataVector\<D> deriving from @c DataVector\<B>.
   /// In general, inheritance may be multiple or virtual,
   /// so we need to adjust the pointer offsets.
   ///
   /// Suppose we're setting up the proxy for @c DataVector\<D>.
   /// Then @c fContoff will be set to the offset of the @c DataVector\<B> base
   /// within @c DataVector\<D> --- this is the amount we need to shift the
   /// container pointer by before applying the proxy.
   /// Originally, we cached an offset for the element conversions
   /// as well.  But that doesn't work for the case of virtual derivation.
   /// Instead, we save the Reflex types for the base and derived types,
   /// and use Reflex to convert.  (We can get away with caching the
   /// offset for the container because we know what the fully-derived
   /// type will be.  We don't know that for the elements.)
   ///
   /// @param elttype The name of the contained type (including a trailing '*').
   /// @param conttype The name of the container type we're proxying.
   ///                 Note that this may be different from
   ///                 a @c DataVector of @a elttype for classes which
   ///                 derive from @c DataVector.
   void TDVCollectionProxy::FindOffsets( const char* elttype,
                                         const char* conttype ) {

      // Start by assuming no offsets.
      fContoff = 0;
      fOffset = 0;

      // Find its TClass.
      TClass* dvclass = TClass::GetClass( conttype );
      if( ! dvclass ) {
         ::Fatal( "xAOD::TDVCollectionProxy::FindOffsets",
                  "Cannot find class %s", conttype );
         return;
      }

      // Find the TClass for the base DataVector class.
      TClass* dvbase = findDVBase( dvclass );
      if( ! dvbase ) {
         TClass* elclass = classFromDVClass( dvclass );
         if( ! elclass ) {
            ::Fatal( "xAOD::TDVCollectionProxy::FindOffsets",
                     "Couldn't find element type of %s",
                     dvclass->GetName() );
            return;
         }
         fEltBase = elclass;
         fEltType = fEltBase;

         // No inheritance --- offsets are zero.
         return;
      }

      // Find the container offset.
      fContoff = safeGetBaseOffset( dvclass, dvbase );

      // Now, find the base and derived element classes.
      std::string elttype2 = elttype;
      if( elttype2[ elttype2.size() - 1 ] == '*' ) {
         elttype2.erase( elttype2.size() - 1 );
      }
      fEltType = TClass::GetClass( elttype2.c_str() );
      fEltBase = classFromDVClass( dvbase );

      if( ( ! fEltType ) || ( ! fEltBase ) ) {
         ::Fatal( "xAOD::TDVCollectionProxy::FindOffsets",
                  "Couldn't find dictionaries for DV element "
                  "type(s) for %s", conttype );
         return;
      }

      // Get the offset needed for the pointer operations:
      fOffset = safeGetBaseOffset( fEltType, fEltBase );

      return;
   }

} // namespace xAOD
