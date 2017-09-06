/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OBJECTLOADERBASE_H_
#define OBJECTLOADERBASE_H_

#include "TObject.h"
#include <memory>

namespace top {
class TopConfig;
class TopObjectSelection;

/**
 * @brief People seem to want to be able to configure the objects used in their
 * anaysis for some reason.  This is a common base class for doing just that.
 * It only has one function, init, and once that has run and made the object
 * selection object then this can be deleted.
 *
 */
class ObjectLoaderBase {
public:
    /**
     * @brief This will inherit from this, but it doesn't do anything really.
     */
    virtual ~ObjectLoaderBase() {}

    /**
     * @brief Must implement this to load your own object selection.  We have
     * two examples top::ObjectLoaderStandardCuts and top::ObjectLoaderTtres.
     *
     * @param topConfig Since you might need access to the user configuration
     * loaded from the config file at run time.
     * @return
     */
    virtual top::TopObjectSelection* init(std::shared_ptr<top::TopConfig> topConfig) = 0;

    ClassDef(top::ObjectLoaderBase, 0)
};

}

#endif
