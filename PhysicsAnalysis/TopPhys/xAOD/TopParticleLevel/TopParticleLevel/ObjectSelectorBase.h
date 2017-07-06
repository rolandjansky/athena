/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: ObjectSelectorBase.h
// Description:
// Author: Fabian Wilk
// Created: Sun Feb 22 15:46:33 2015

#ifndef _TOP_OBJECTSELECTORBASE_H_
#define _TOP_OBJECTSELECTORBASE_H_

namespace top {

    template <typename ObjectT>
    class ObjectSelectorBase {
    public:
	ObjectSelectorBase(){}
	virtual ~ObjectSelectorBase(){}
	virtual bool apply( const ObjectT & obj ) = 0;
    };
}


#endif /* _TOP_OBJECTSELECTORBASE_H_ */
