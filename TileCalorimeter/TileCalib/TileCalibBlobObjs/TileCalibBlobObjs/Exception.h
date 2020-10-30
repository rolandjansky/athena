/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBBLOBOBJS_EXCEPTION_H
#define TILECALIBBLOBOBJS_EXCEPTION_H


#include <exception>
#include <string>
#include <sstream>

namespace TileCalib {
  
  
  //
  //__________________________________________________________________________________
  /** 
      @brief TileCalib Exception base class
      @author Nils Gollub <nils.gollub@cern.ch>
  */
  class Exception : public std::exception {
  public:
    explicit Exception( const std::string& domain, const std::string& message)
      : m_domain(domain), m_message(message)
    {
      format();
    }
    virtual ~Exception() throw() {}

    virtual const char* what() const throw(){
      return m_result.c_str();
    }
    
    virtual const std::string& domain() const{
      return m_domain;
    }
    
  protected:
    virtual void setMessage( const std::string& message ){
      m_message = message;
      format();
    }
    
  private:
    void format()
    {
      m_result = m_domain + ": "+ m_message;
    }

    std::string m_domain;
    std::string m_message;
    std::string m_result;
  };


  //
  //__________________________________________________________________________________
  /** 
      @brief Thrown if coral::Blob does not conform with expected structure
      @author Nils Gollub <nils.gollub@cern.ch>
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
      @brief Thrown if object version in BLOB does not agree with class
      @author Nils Gollub <nils.gollub@cern.ch>
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
      @brief Thrown if object type in BLOB does not agree with class type
      @author Nils Gollub <nils.gollub@cern.ch>
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
      @author Nils Gollub <nils.gollub@cern.ch>
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
      @author Nils Gollub <nils.gollub@cern.ch>
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
      @brief Thrown by TileCalibDrawerOfc if requesting samples for invalid phase 
      @author Nils Gollub <nils.gollub@cern.ch>
  */
  class NoSuchPhase : public Exception {
  public:
    explicit NoSuchPhase( const std::string& domain, 
			  int phase)
      : Exception( domain, "" ) 
      {
	std::ostringstream msg;
	msg << "No such phase: " << phase;
	setMessage(msg.str());
      }
    virtual ~NoSuchPhase() throw() {}
  };
  

  //
  //__________________________________________________________________________________
  /** 
      @brief Thrown by TileBchBits if invalid bit pattern is detected
      @author Nils Gollub <nils.gollub@cern.ch>
  */
  class InvalidBitPattern : public Exception {
  public:
    explicit InvalidBitPattern( const std::string& domain, 
				int version)
      : Exception( domain, "" ) 
      {
	std::ostringstream msg;
	msg << "Invalid bit pattern version: " << version;
	setMessage(msg.str());
      }
    virtual ~InvalidBitPattern() throw() {}
  };
  
  //
  //__________________________________________________________________________________
  /** 
      @brief Thrown by TileBchBits if unkown problem is detected
      @author Nils Gollub <nils.gollub@cern.ch>
  */
  class InvalidBchProblem : public Exception {
  public:
    explicit InvalidBchProblem( const std::string& domain, 
				unsigned int prb)
      : Exception( domain, "" ) 
      {
	std::ostringstream msg;
	msg << "Problem not encodable in current bit pattern: " << prb;
	setMessage(msg.str());
      }
    virtual ~InvalidBchProblem() throw() {}
  };


  //
  //__________________________________________________________________________________
  /** 
      @brief Thrown to prevent invalid results, eg. division by zero
      @author Nils Gollub <nils.gollub@cern.ch>
  */
  class InvalidValue : public Exception {
  public:
    explicit InvalidValue( const std::string& domain, 
			   float value)
      : Exception( domain, "" ) 
      {
	std::ostringstream msg;
	msg << "Invalid value: " << value;
	setMessage(msg.str());
      }
    virtual ~InvalidValue() throw() {}
  };
  
}
#endif
