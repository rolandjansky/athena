/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <ISProperty/ISProperty.h>

#include <GaudiKernel/SystemOfUnits.h>

/**
 * @file: ISProperty.cxx
 * @brief: Implementation of ISProperty class
 */

/**
 * Constructor
 * @param part the IPC partition in which the IS object will be published
 * @param info_name the name of the published IS object
 * @param class_name the type name of the published IS object
 * @param props list of GaudiPropertyHandles to the Property objects 
 * @param updateTime minimum time intervall in seconds after which the property can be updated
 **/
ISProperty::ISProperty(  const IPCPartition part, const std::string info_name, const std::string class_name,
                         const PropertyList props, double updateTime  ) :
      ISNamedInfo(part,info_name,class_name),
      m_Properties(props),
      m_updateTime(updateTime)
{
  //Loop over list of propertie names
  PropertyList::iterator propItr = m_Properties.begin();
  for ( ; propItr != m_Properties.end() ; ++propItr )
    //add ourselfs as update listener, keeping the old one in store
    (*propItr).pushCallback( &ISProperty::updateFromGaudiHandler, this );
}

/**
 * Destructor
 **/
ISProperty::~ISProperty() {
  //Remove ourselves as update handler
  PropertyList::iterator propItr = m_Properties.begin();
  for ( ; propItr != m_Properties.end() ; ++propItr )
    //Reinstall old update handlers
    (*propItr).popCallback();

  //remove the ISInfo object
  remove();
}


/**
 * this update handler is called if property changes on athena side
 **/
void ISProperty::updateFromGaudiHandler( Property& p ){
  using namespace Gaudi::Units;
  //Check if the update time is not yet over
  OWLTime now;
  if (now.total_mksec_utc() - time().total_mksec_utc() < m_updateTime*second/microsecond) return; 
  //update information in IS
  checkin();
  //try to find object in list
  PropertyList::iterator propItr = m_Properties.begin();
  for ( ; propItr != m_Properties.end() ; ++propItr){
    //Check if this is the property we are looking for
    if (p.name() == (*propItr).getName())
      //if there had been a Callback handler, call it
      (*propItr).Callback();
  }
}

/**
 * this update handler is called if property changes on IS side
 **/
void ISProperty::updateFromISHandler( ISInfo& /*is*/ ){
  //update information from IS
  checkout();
}

/**
 * Implementation of the ISNamedInfo interface
 */
void ISProperty::publishGuts( ISostream& out ){
  

  PropertyList::const_iterator propItr = m_Properties.begin();
  for ( ; propItr != m_Properties.end() ; ++propItr ){
    //Get the property from the manager
    Property* theProp = (*propItr).getProperty();
    //Make sure we got it
    if (!theProp) continue ;
    //try to output the property as one of the IS types
    if (!writePropertyAs<float>(theProp,out))
    if (!writePropertyAs<double>(theProp,out))
    if (!writePropertyAs<int>(theProp,out))
    if (!writePropertyAs<long>(theProp,out))
    if (!writePropertyAs<int64_t>(theProp,out))
    if (!writePropertyAs<short>(theProp,out))
    if (!writePropertyAs<char>(theProp,out))
    if (!writePropertyAs<bool>(theProp,out))
    if (!writePropertyAs<unsigned char>(theProp,out))
    if (!writePropertyAs<unsigned short>(theProp,out))
    if (!writePropertyAs<unsigned int>(theProp,out))
    if (!writePropertyAs<unsigned long>(theProp,out))
    if (!writePropertyAs<uint64_t>(theProp,out))
    //or stream as string if all others fail
    out << theProp->toString();
  }
}
    
/**
 * Implementation of the ISNamedInfo interface
 */
void ISProperty::refreshGuts( ISistream & in ){
  PropertyList::iterator propItr = m_Properties.begin();
  for ( ; propItr != m_Properties.end() ; ++propItr ){
    //Get the property from the manager
    Property* theProp = (*propItr).getProperty();
    //Make sure we got it
    if (!theProp) continue ;
    //// Apparently, sometimes we are not listed as update handler!!!
    //// No clue yet why this happens, maybe during double refresh
    //// In principle it should only ourselves listed as update handlers
    //// However, the user may have overwritten this on purpose!
    PropertyCallbackFunctor* storedCallBack = NULL ;
    if ( theProp->updateCallBack()) { 
      //Remove update handler so we won't get a callback caused by ourselfs
      storedCallBack = theProp->updateCallBack()->clone(); 
    } else {
      std::cout << "ISProperty\tERROR Property " << theProp->name() << " does not have a callback! " << std::endl;
      std::cout.flush();
    }
    ///Clear the IS update handler temporarily
    theProp->declareUpdateHandler(NULL);
    //Now we can safely set the property
    if (!readPropertyAs<float>(theProp,in))
    if (!readPropertyAs<double>(theProp,in))
    if (!readPropertyAs<int>(theProp,in))
    if (!readPropertyAs<long>(theProp,in))
    if (!readPropertyAs<int64_t>(theProp,in))
    if (!readPropertyAs<short>(theProp,in))
    if (!readPropertyAs<char>(theProp,in))
    if (!readPropertyAs<bool>(theProp,in))
    if (!readPropertyAs<unsigned char>(theProp,in))
    if (!readPropertyAs<unsigned short>(theProp,in))
    if (!readPropertyAs<unsigned int>(theProp,in))
    if (!readPropertyAs<unsigned long>(theProp,in))
    if (!readPropertyAs<uint64_t>(theProp,in)){
      //if all fail, use string
      std::string value; in >> value;
      theProp->fromString(value).ignore();
    }
    //if there had been an athena Callback handler, call it
    (*propItr).Callback();
    //Finally, reinstall the update handler
    theProp->declareUpdateHandler(storedCallBack);
  }
}

