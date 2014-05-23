/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TEST_TOYCONVERTER_H
# define TEST_TOYCONVERTER_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/StatusCode.h"

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/StorableConversions.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< FORWARD DECLARATIONS                                           >>>>>>
template <class Cnv> class CnvFactory;
//<<<<<< CLASS DECLARATIONS                                             >>>>>>
/** @class ToyConverter 
 * @brief  a toy converter template that creates a default instance of DATA
 * @param  DATA the type to be "converted". Must have a default constructor
 * @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
 *$Id: ToyConverter.h,v 1.3 2006-12-22 23:12:30 calaf Exp $
 */

#include "ToyConversion/ToyConversionSvc.h"
template <typename DATA>
class ToyConverter : public Converter {
public:
  // tran->per
  //  StatusCode createRep(DataObject* pO, IOpaqueAddress*& pA);

  // per->tran
  StatusCode createObj(IOpaqueAddress*, DataObject *& pO) {
    //could alse get DATA (perhaps as boost::any) from the IOA
    pO = SG::asStorable(new DATA);
    return StatusCode::SUCCESS;
  }

  static const CLID& classID() { return ClassID_traits<DATA>::ID(); }

  virtual long int repSvcType() const;
  static long int storageType();

protected:
  friend class CnvFactory< ToyConverter<DATA> >;
  ToyConverter(ISvcLocator* svcloc) : 
    Converter(storageType(), classID(), svcloc) {}
};


template <typename DATA>
long int ToyConverter<DATA>::storageType() {
  return ToyConversionSvc::storageType();
}
template <typename DATA>
long int ToyConverter<DATA>::repSvcType() const {
  return storageType();
}

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
#endif // TEST_TOYCONVERTER_H
