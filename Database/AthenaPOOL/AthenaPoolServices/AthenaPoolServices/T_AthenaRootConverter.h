/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLSERVICES_TATHENAROOTCONVERTER_H
#define ATHENAPOOLSERVICES_TATHENAROOTCONVERTER_H

/** @file T_AthenaRootConverter.h
 *  @brief Athena ROOT Streaming Converter - template class to be extended by a user prividing a conversion function
 *  @author Marcin Nowak
 *  $Id: 
 **/

#include <string>
#include <typeinfo>

class T_AthenaRootConverterBase {
public:

  /// Set the name of the class for which this converter is implemented
  /// and the particualar ROOT streamerInfo checksum of the class shape version
  void		SetClassNameAndChecksum(const std::string &classname, unsigned checksum);
  /// General (typeless) invocation of the convertion function
  /// in the derived templated class it calls the actual user implemenation
  virtual void 	Convert(const void *old_obj, void *new_obj) = 0;

  const std::string&	ClassName() { return m_className; }
  unsigned		StreamerChecksum() { return m_streamerChecksum; }

  /// Create an instance of the old object
  virtual void*		InstantiateOld() = 0;
  /// Get typeinfo of the old object type
  virtual const std::type_info& TypeIDOld() = 0;		

  virtual 	~T_AthenaRootConverterBase() {}
  
protected:
  T_AthenaRootConverterBase();
  
  std::string	m_className;
  unsigned	m_streamerChecksum;
};



inline
T_AthenaRootConverterBase::T_AthenaRootConverterBase()
      : m_className(),
	m_streamerChecksum(0)
{
}


inline
void T_AthenaRootConverterBase::SetClassNameAndChecksum(const std::string &classname, unsigned checksum)
{
   m_className = classname;
   m_streamerChecksum = checksum;
}




template<class NEW, class OLD>
class T_AthenaRootConverter : public T_AthenaRootConverterBase {
public:
  /// Set the name of the class for which this converter is implemented
  /// and the particualar ROOT streamerInfo checksum of the class shape version
  /// To be implemented by the user in the derived class
  T_AthenaRootConverter() {
     SetClassNameAndChecksum("", 0);
  }

  /// The converter function which copies the content of the object
  /// from its old shape to the new shape
  /// To be implemented by the user in the derived class
  /// Default implementation simply does a cast 
  virtual void CopyOldToNew( const OLD &old_obj, NEW &new_obj ) {
     new_obj = *(NEW*)&old_obj;
  }

  /// General (typeless) invocation of the convertion function
  /// it calls the actual user implemenation with concrete argument types
  virtual void Convert(const void *old_obj, void *new_obj) {
     CopyOldToNew( *(const OLD*)old_obj, *(NEW*)new_obj);
  } 
  
  /// Create an instance of the old object
  virtual void*		InstantiateOld() { return new OLD(); }
  /// Get typeinfo of the old object type
  virtual const std::type_info& TypeIDOld() { return typeid(OLD); }
};


#endif
