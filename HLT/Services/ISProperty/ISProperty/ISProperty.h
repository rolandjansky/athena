/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISPROPERTY_ISPROPERTY_H
#define ISPROPERTY_ISPROPERTY_H

#include <is/namedinfo.h>

#include <GaudiKernel/IProperty.h>
#include <GaudiKernel/Property.h>
#include <GaudiKernel/PropertyCallbackFunctor.h>

//forward declaration
class ISProperty;
class PropertyCallbackFunctor;

/**
 * @struct GaudiPropertyHandle
 * @brief Reference to a gaudi property via IProperty - will also store callback
 */
class GaudiPropertyHandle {

  public:

    /**
     * Constructor takes name and reference
     */
    GaudiPropertyHandle ( std::string pname, IProperty* propMgr):
      m_name(pname),
      m_pMgr(propMgr),
      m_callback( NULL )
    { };

  
    /**
     * Get a property by name from the property manager - this is mainly for the
     * reason that IProperty::getProperty returns a const object.
     **/
    Property* getProperty() const {
      return Gaudi::Utils::getProperty(m_pMgr,m_name);
    }

    /// Return name
    const std::string getName() const { return m_name; };
    
    /// Return propertyMgr
    const IProperty* getPropertyMgr() const { return m_pMgr; };
    
    /// call the stored callback handler if there is any
    void Callback() const {
      //if there had been a Callback handler, call it
      if ( m_callback != NULL ) (*m_callback)(*getProperty());
    }

    // store old callback handler
    template < class HT > void pushCallback( void ( HT::* MF ) ( Property& ) , HT* instance ){
      
      //get the property itself
      Property* theProp = getProperty();
      if (! theProp) return ;
      
      //Store the old update handler
      if (theProp->updateCallBack() != NULL )
        m_callback = theProp->updateCallBack()->clone();

      //install the new update handler
      theProp->declareUpdateHandler< HT >(MF,instance);
    }

    // reinstall old callback handler
    void popCallback(){
      
      //get the property itself
      Property* theProp = getProperty();
      if ( ! theProp ) return ;

      //reinstall the old update handler
      theProp->declareUpdateHandler( m_callback );

      m_callback = NULL ;
    }
   

  private:
    std::string m_name;  //!< the name of the propery
    IProperty* m_pMgr ;  //!< its property manager
    PropertyCallbackFunctor* m_callback ; //!< stored handle to callback functor
};

/***
 * Try to cast property as type T and write it to stream out
 **/
template <class T> bool writePropertyAs(Property* p, ISostream& out){

  //Check if the type-id matches
  if (typeid(T) != *(p->type_info()) ) return false;

  //With matching type_id static cast should never fail
  out << (static_cast<PropertyWithValue<T>* >(p))->value();

  return true;
}

/***
 * Try to cast property as type T and read it from stream in
 **/
template <class T> bool readPropertyAs(Property* p, ISistream& in){

  //Check if the type-id matches
  if (typeid(T) != *(p->type_info()) ) return false;

  //Read from stream
  T value; in >> value;
  
  //With matching type_id static cast should never fail
  (static_cast<PropertyWithValue<T>* >(p))->setValue(value);
  return true;
}

//Some handy typedefs
typedef std::map< std::string , ISProperty* > ISPropertyMap;
typedef std::vector< GaudiPropertyHandle > PropertyList;

/**
 * @class ISProperty
 * @brief: Property object published to IS
 * This ISNamedInfo object holds a collection of the Gaudi property handles
 * If one of the properties is updated from Gaudi or IS, the object is also updated
 * on the other side.
 *
 * @author: Sebastian Boeser
 */
class ISProperty: public ISNamedInfo {
  
  public:

    //Constructor
    ISProperty(  const IPCPartition part, const std::string info_name, const std::string class_name,
                 const PropertyList props, double updateTime );

    //Destructor
    ~ISProperty();

    //@group ISNameInfo interface functions
    //@{
    void publishGuts( ISostream & out );
    void refreshGuts( ISistream & in );
    //@}

    // update handler is called if property changes on athena side
    void updateFromGaudiHandler(Property& p);
    // update handler is called if property change on IS side
    void updateFromISHandler(ISInfo& is);

  private:

    PropertyList m_Properties;   //!< list of Gaudi property handles

    double m_updateTime; //!< minimum time after which the property can be updated 

};


#endif
