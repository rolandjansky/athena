/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef IUSERDATASVC_H
#define IUSERDATASVC_H

#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/IAthenaBarCode.h"

#include <string>
#include <typeinfo>
#include "boost/any.hpp"


/** @class IUserDataSvc
 * @brief  interface to UserDataSvc
 * @author Yushu Yao <yyao@lbl.gov> - ATLAS Collaboration
 * @author Walter Lampl <walter.lampl@cern.ch>
 *$Id: IUserDataSvc.h,v 1.8 2008/05/27 22:09:42 yyao Exp $
 */

class IUserDataSvc : virtual public IService {

public:

  //Standard destructor
  ~IUserDataSvc() {}

  //Standard interfaceID query
  static const InterfaceID&   interfaceID();


  // * * * * * * * * * * * * * * * * * * * * * * * *
  //NEW StoreGate-inspired interface:
  //1. Event Level Decorations
  // * * * * * * * * * * * * * * * * * * * * * * * *

  /** 
   * @brief Record method for event decorations (recommended)
   * @param label String label of the decoration
   * @param deco Decoration payload (template) 
   */
  template<typename DECO>
  StatusCode record(const std::string& label, const DECO& deco);


  /** 
   * @brief Retrieve method for event decorations (recommended)
   * @param label String label of the decoration
   * @param[out] deco (output) Decoration payload  
   */
  template<typename DECO>
  StatusCode retrieve(const std::string & label, DECO &deco);


  /** 
   * @brief Contains method for event decorations (recommended)
   * @param label String label of the decoration
   * @return True if the decoration exists
   */
  virtual bool contains(const std::string& label) =0;

  // * * * * * * * * * * * * * * * * * * * * * * * *
  //2. Element Level Decorations
  // * * * * * * * * * * * * * * * * * * * * * * * *

  /** 
   * @brief Record method for element decorations (recommended)
   * @param abc A data-object implmenting the IAthenaBarCode interface
   * @param label String label of the decoration
   * @param deco Decoration payload (template) 
   */
  template<typename DECO>
  StatusCode record(const IAthenaBarCode &abc, const std::string& label, const DECO& deco);


  /** 
   * @brief Retrieve method for element decorations (recommended)
    * @param abc A data-object implmenting the IAthenaBarCode interface
   * @param label String label of the decoration
   * @param[out] deco (output) Decoration payload  
   */
  template<typename DECO>
  StatusCode retrieve(const IAthenaBarCode &abc, const std::string & label, DECO &deco);

  /** 
   * @brief Contains method for element decorations (recommended)
   * @param abc A data-object implmenting the IAthenaBarCode interface
   * @param label String label of the decoration
   * @return True if the decoration exists
   */
  virtual bool contains(const IAthenaBarCode& abc,const std::string& label) =0;




  // * * * * * * * * * * * * * * * * * * * * * * * *
  // Pure Virtual Methods
  // * * * * * * * * * * * * * * * * * * * * * * * *

  //For Element decoration
  virtual StatusCode recordAny(const IAthenaBarCode& abc,const std::string& label, const boost::any& value)=0;
  virtual StatusCode retrieveAny(const IAthenaBarCode& abc,const std::string& label, const boost::any*& value) =0;

  //For Event decoration:
  virtual StatusCode recordAny(const std::string& label, const boost::any& value)=0;
  virtual StatusCode retrieveAny(const std::string& label, const boost::any*& value) =0;


  // * * * * * * * * * * * * * * * * * * * * * * * *
  //Legacy interface
  // * * * * * * * * * * * * * * * * * * * * * * * *

  
  /** 
   * @brief Legacy Retrieve method for element decorations (deprecated)
   * @param abc A data-object implmenting the IAthenaBarCode interface
   * @param label String label of the decoration
   * @param[out] deco (output) Decoration payload  
   */
  template<typename DECO>
  StatusCode getElementDecoration(const IAthenaBarCode &abc, const std::string & label, DECO &deco) {
    return retrieve(abc,label,deco);
  }

  /**
   * @brief Legacy Retrieve method for element decorations (deprecated)
   * @param abc A data-object implmenting the IAthenaBarCode interface
   * @param label String label of the decoration
   * @param[out] deco (output) Decoration payload
   */

  template<typename DECO>
  StatusCode getInMemElementDecoration(const IAthenaBarCode &abc, const std::string & label, DECO &deco) {
    return retrieve(abc,label,deco);
  }

  /** 
   * @brief Legacy Record method for element decorations (deprecated)
   * @param label String label of the decoration
   * @param deco (output) Decoration payload  
   */
  template<typename DECO>
  StatusCode decorateElement(const IAthenaBarCode &abc, const std::string& label, DECO& deco) {
    return record(abc,label,deco);
  }
  
  /** 
   * @brief Legacy Record method for event decorations (deprecated)
   * @param label String label of the decoration
   * @param[out] deco (output) Decoration payload  
   */
  template<typename DECO>
  StatusCode decorateEvent(const std::string& label, const DECO& deco) {
    return record(label,deco);
  }

  /** 
   * @brief Legacy Retrieve method for event decorations (deprecated)
    * @param abc A data-object implmenting the IAthenaBarCode interface
   * @param label String label of the decoration
   * @param[out] deco (output) Decoration payload  
   */
  template<typename DECO>
  StatusCode getEventDecoration(const std::string & label, DECO &deco) {
    return retrieve(label,deco);
  }

  /** 
   * @brief Legacy Retrieve method for event decorations (deprecated)
    * @param abc A data-object implmenting the IAthenaBarCode interface
   * @param label String label of the decoration
   * @param[out] deco (output) Decoration payload  
   */
  template<typename DECO>
  StatusCode getInMemEventDecoration(const std::string & label, DECO &deco) {
    return retrieve(label,deco);
  }

  /** 
   * @brief Legacy Contains method for Event decorations (deprecated)
   * @param label String label of the decoration
   * @return True if the decoration exists
   */
  bool containsEventDecoration(const std::string &label) {
    return contains(label);
  }
  /** 
   * @brief Legacy Contains method for Event decorations (deprecated)
   * @param label String label of the decoration
   * @return True if the decoration exists
   */

  bool containsInMemEventDecoration(const std::string &label) {
    return contains(label);
  }

  /** 
   * @brief Legacy Contains method for Event decorations (deprecated)
   * @param abc A data-object implmenting the IAthenaBarCode interface
   * @param label String label of the decoration
   * @return True if the decoration exists
   */
  bool containsElementDecoration(const IAthenaBarCode &abc, const std::string &label="") {
    return contains(abc,label);
  }

  /** 
   * @brief Legacy Contains method for Event decorations (deprecated)
   * @param abc A data-object implmenting the IAthenaBarCode interface
   * @param label String label of the decoration
   * @return True if the decoration exists
   */
  bool containsInMemElementDecoration(const IAthenaBarCode &abc, const std::string &label="") {
    return contains(abc,label);
  }



  virtual int vdecorateElement(const IAthenaBarCode &abc, const std::string& label,
			       const std::type_info &decoinfo, void* & deco) =0;

  virtual int vgetElementDecoration(const IAthenaBarCode &abc, const std::string& label,
				     const std::type_info &decoinfo, void *&deco) = 0;



  int vgetInMemElementDecoration(const IAthenaBarCode &abc, const std::string& label,
				 const std::type_info &decoinfo, void *&deco,
				 bool /*quiet*/) {
    return vgetElementDecoration(abc,label,decoinfo,deco);
  }


};




template<typename DECO>
inline StatusCode IUserDataSvc::record(const std::string& label, const DECO& deco) {  
  const boost::any value(deco);
  return this->recordAny(label, value);

}

template<typename DECO>
inline StatusCode IUserDataSvc::retrieve(const std::string & label, DECO &deco) {
  const boost::any* value;
  StatusCode sc=this->retrieveAny(label, value);
  if (sc.isFailure()) return sc;
  if (typeid(DECO) != value->type())  return StatusCode::FAILURE;
 
  deco=boost::any_cast<DECO>(*value);
  return StatusCode::SUCCESS;
}


template<typename DECO>
inline StatusCode IUserDataSvc::record(const IAthenaBarCode &abc, const std::string& label, const DECO& deco) {
  boost::any value(deco);
  return this->recordAny(abc,label, value);
}


template<typename DECO>
inline StatusCode IUserDataSvc::retrieve(const IAthenaBarCode &abc, const std::string & label, DECO &deco) {
  const boost::any* value=0;
  StatusCode sc=this->retrieveAny(abc,label, value);
  if (sc.isFailure()) return sc;
  if (typeid(DECO) != value->type())  return StatusCode::FAILURE;
  const DECO* decoPtr=boost::any_cast<DECO>(value);
  deco=*decoPtr; //Deep copy! 
  return StatusCode::SUCCESS;

}


inline const InterfaceID&
IUserDataSvc::interfaceID() {
  static const InterfaceID IID("IUserDataSvc", 1, 0);
  return IID;
}

#endif // IUSERDATASVC_H


//Old methods, not implemented in the new version:

//virtual bool containsElementDecoration(const IAthenaBarCode &abc, const std::string &label="") = 0;
  //virtual bool containsInMemElementDecoration(const IAthenaBarCode &abc, const std::string &label="") = 0;

  //Functions to iterate over existing element level decorations in this event
  //Slow, not suggested to use for production purpose
  //virtual  std::vector<AthenaBarCode_t> getElListOfABC()  =0;
  //virtual  std::vector<std::string> getElListOfLabel(const AthenaBarCode_t &abc) =0;
  //virtual  std::vector<AthenaBarCode_t> getInMemElListOfABC()  =0;
  //virtual  std::vector<std::string> getInMemElListOfLabel(const AthenaBarCode_t &abc) =0;


/*
  virtual int
  vdecorate(const std::string& label, const std::type_info &decoinfo,
      void* & deco, void* & defaultobj, void* & tobedeleted,
      const UserDataAssociation::DecoLevel& level, const CLID& clid,
      const std::string & key) = 0;

  virtual int
  vdecorateElement(const IAthenaBarCode &abc, const std::string& label,
      const std::type_info &decoinfo, void* & deco, void* & defaultobj,
      void* & tobedeleted)=0;

  virtual int
  vgetDecoration(const std::string& label, const std::type_info &decoinfo,
      void *&deco, const UserDataAssociation::DecoLevel& level,
      const CLID& clid, const std::string & key) = 0;

  virtual int
  vgetInMemDecoration(const std::string& label, const std::type_info &decoinfo,
      void *&deco, const UserDataAssociation::DecoLevel& level,
      const CLID& clid, const std::string & key) = 0;

  virtual int
  vgetElementDecoration(const IAthenaBarCode &abc, const std::string& label,
                        const std::type_info &decoinfo, void *&deco) = 0;



  */
