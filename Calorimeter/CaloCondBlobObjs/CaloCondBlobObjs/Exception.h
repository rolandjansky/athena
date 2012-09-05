/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDBLOBOBJS_EXCEPTION_H
#define CALOCONDBLOBOBJS_EXCEPTION_H

#include <exception>
#include <string>
#include <sstream>

namespace CaloCond {
  
  //
  //__________________________________________________________________________________
  /** 
      @brief Calocond Exception base class
      @author Nils Gollub <nils.gollub@cern.ch>
  */
  class Exception : public std::exception {
  public:
    explicit Exception( const std::string& domain, const std::string& message)
      : m_domain(domain), m_message(domain + ": " + message){}
    virtual ~Exception() throw() {}

    virtual const char* what() const throw(){
      return m_message.c_str();
    }
    
    virtual const std::string& domain() const{
      return m_domain;
    }
    
  protected:
    virtual void setMessage( const std::string& message ){
      m_message = m_domain + ": " + message;
    }
    
  private:
    std::string m_domain;
    std::string m_message;
  };


  //
  //__________________________________________________________________________________
  /** 
      @brief Thrown if coral::Blob does not conform with expected structure.
  */
  class InvalidBlob: public Exception {
  public:
    explicit InvalidBlob( const std::string& domain ) 
      : Exception( domain, "Invalid Blob" ) 
      {
      }
    virtual ~InvalidBlob() throw() {}
  };
  
  //
  //__________________________________________________________________________________
  /** 
      @brief Thrown if object version in BLOB does not agree with class version.
  */
  class VersionConflict: public Exception {
  public:
    explicit VersionConflict( const std::string& domain, 
			      unsigned int version_is,
			      unsigned int version_should=0)
      : Exception( domain, "" ) 
      {
	std::ostringstream msg;
	msg << "Blob Version conflict. Is " << version_is;
	if(version_should){ msg << " but should be " << version_should; }
	setMessage(msg.str());
      }
    virtual ~VersionConflict() throw() {}
  };
  
  //
  //__________________________________________________________________________________
  /** 
      @brief Thrown if object type in BLOB does not agree with class type.
  */
  class TypeConflict: public Exception {
  public:
    explicit TypeConflict( const std::string& domain, 
			      unsigned int version_is,
			      unsigned int version_should=0)
      : Exception( domain, "" ) 
      {
	std::ostringstream msg;
	msg << "Blob Type conflict. Is " << version_is;
	if(version_should){ msg << " but should be " << version_should; }
	setMessage(msg.str());
      }
    virtual ~TypeConflict() throw() {}
  };
  
  //
  //__________________________________________________________________________________
  /** 
      @brief Thrown if BLOB size is not correct.
  */
  class SizeConflict: public Exception {
  public:
    explicit SizeConflict( const std::string& domain, 
			   unsigned int size_is,
			   unsigned int size_should)
      : Exception( domain, "" ) 
      {
	std::ostringstream msg;
	msg << "Blob Size conflict. Is " << size_is
	    << " but should be " << size_should;
	setMessage(msg.str());
      }
    virtual ~SizeConflict() throw() {}
  };

  //
  //__________________________________________________________________________________
  /** 
      @brief Thrown if an index is out of range.
  */
  class IndexOutOfRange : public Exception {
  public:
    explicit IndexOutOfRange( const std::string& domain, 
			      unsigned int idx,
			      unsigned int maxRange=0)
      : Exception( domain, "" ) 
      {
	std::ostringstream msg;
	msg << "Index out of range: " << idx;
	if(maxRange>0){
	  msg << " >= " << maxRange;
	}
	setMessage(msg.str());
      }
    virtual ~IndexOutOfRange() throw() {}
  };

  //
  //__________________________________________________________________________________
  /** 
      @brief Thrown if an invalid CaloGain is encountered.
  */
  class InvalidCaloGain : public Exception {
  public:
    explicit InvalidCaloGain( const std::string& domain, 
			      int caloGain)
      : Exception( domain, "" ) 
      {
	std::ostringstream msg;
	msg << "Invalid CaloGain: " << caloGain;
	setMessage(msg.str());
      }
    virtual ~InvalidCaloGain() throw() {}
  };
  
}
#endif
