/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALIBTOOLS__ISELECTIONTOOL_H
#define TRT_CALIBTOOLS__ISELECTIONTOOL_H
/********************************************************************

NAME:     SelectionTool
PACKAGE:  TRT_AlignTOOLS

AUTHORS:  Jorgen Beck Hansen
CREATED:  December 2005

PURPOSE:  Track Selection Tool interface          

********************************************************************/

#include <vector>
#include "GaudiKernel/IAlgTool.h"

template < class InputClass >
class ISelectionTool : virtual public IAlgTool {
public:
  
  virtual bool isAccepted(const InputClass *element) const = 0;

  bool operator() (const InputClass *element);
  template < typename Collection >
  void filterCollection(const Collection* inputCollection, std::vector< InputClass* >& output);
};

template<class InputClass>
inline bool ISelectionTool<InputClass>::operator()(const InputClass *element) { return isAccepted(element);}

template< class InputClass >
template< typename Collection >
inline void ISelectionTool<InputClass>::filterCollection(const Collection* inputCollection,
						    std::vector< InputClass* >& output) {
  for(typename Collection::const_iterator it=inputCollection->begin();
      it != inputCollection->end(); ++it)
    if (isAccepted(*it)) output.push_back(*it);
}
#endif // TRT_CALIBTOOLS__ISELECTIONTOOL_H

