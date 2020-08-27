/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASG_DATA_HANDLES_HANDLE_KEY_ARRAY_H
#define ASG_DATA_HANDLES_HANDLE_KEY_ARRAY_H

#ifndef XAOD_STANDALONE
#include <StoreGate/HandleKeyArray.h>
#else

#include "AsgDataHandles/VarHandleKeyArray.h"

namespace SG {

  /**
   * @class SG::HandleKeyArray<T>
   * @brief class to hold an array of HandleKeys
   *
   * See StoreGate/HandleKeyArray for details.
   *
   * This currently (13 Aug 20) contains a number of commented out
   * members that were part of the original handle implementation, but
   * have not yet been implemented in the standalone version.  The plan
   * is to either implement or remove them, depending on what is needed
   * in AnalysisBase as we add more packages.
   */

  template <class T_Handle, class T_HandleKey/*, Gaudi::DataHandle::Mode MODE*/>
  class HandleKeyArray : public VarHandleKeyArrayCommon< T_HandleKey > {
  public:
    /**
     * @brief default Constructor from a HandleKeyArray
     */
    HandleKeyArray(){}

    // /**
    //  * @brief Constructor from a HandleKeyArray that takes a vector
    //  * of ReadHandleKeys
    //  * @param v vector of HandleKey
    //  */
    // HandleKeyArray( const std::vector<T_HandleKey>& v ) :
    //   VarHandleKeyArrayCommon<T_HandleKey> ( v ) {}

    // /**
    //  * @brief Constructor from a HandleKeyArray that takes an 
    //  * initializer list of HandleKeys
    //  * @param l initializer list of HandleKey
    //  */
    // HandleKeyArray( std::initializer_list<T_HandleKey> l ):
    //   VarHandleKeyArrayCommon<T_HandleKey> {l} {}

    // /**
    //  * @brief Constructor from a HandleKeyArray that takes an 
    //  * initializer list of std::strings.
    //  * @param l initializer list of std::strings used to create the
    //  *          HandleKeys
    //  */
    // HandleKeyArray( std::initializer_list<std::string> key_names):
    //   VarHandleKeyArrayCommon<T_HandleKey> {key_names} {}

    /**
     * @brief auto-declaring Property Constructor from a HandleKeyArray 
     * that takes an initializer list of std::strings, and associates the WHKA
     * with the specified Property name
     * @param name name of Property
     * @param l initializer list of std::strings used to create the
     *          HandleKeys
     * @param doc documentation string
     */
    template <class OWNER>
    inline HandleKeyArray( OWNER* owner,
                                std::string name,
                                std::initializer_list<std::string> l,
                                std::string doc="") :
      VarHandleKeyArrayCommon<T_HandleKey> {l} {
      owner->declareProperty(std::move(name), *this, std::move(doc));
    }


    // /**
    //  * @brief return the type (Read/Write/Update) of handle
    //  */
    // Gaudi::DataHandle::Mode mode() const { return MODE; }

    /**
     * @brief create a vector of Handles from the HandleKeys
     * in the array
     */
    std::vector< T_Handle > makeHandles() const {
      std::vector< T_Handle > hndl;
      typename std::vector<T_HandleKey>::const_iterator itr;
      for (itr = this->begin(); itr != this->end(); ++itr) {
        hndl.push_back ( T_Handle( *itr) );
      }
      return hndl;
    }

    // /**
    //  * @brief create a vector of Handles from the HandleKeys
    //  * in the array, with explicit EventContext.
    //  */
    // std::vector< T_Handle > makeHandles (const EventContext& ctx) const
    // {
    //   std::vector< T_Handle > hndl;
    //   typename std::vector<T_HandleKey>::const_iterator itr;
    //   for (itr = this->begin(); itr != this->end(); ++itr) {
    //     hndl.push_back ( T_Handle( *itr, ctx) );
    //   }
    //   return hndl;
    // }

  };

} // namespace SG

namespace asg
{
  namespace detail
  {
    template<typename T> struct GetStringHelper;

    template<typename T1,typename T2> struct GetStringHelper<SG::HandleKeyArray<T1,T2> >
      : public GetStringHelper<std::vector<T2> >
    {
    };

    template<typename T> struct SetStringHelper;

    template<typename T1,typename T2> struct SetStringHelper<SG::HandleKeyArray<T1,T2> >
      : public SetStringHelper<std::vector<T2> >
    {
    };
  }
}

#endif

#endif
