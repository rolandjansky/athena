/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LOADALL_H_
#define LOADALL_H_

#include "TopExamples/ContainerNames.h"

#include "TopEvent/Event.h"

namespace top {

class EventLoaderDirect {
public:
    EventLoaderDirect(const ContainerNames& names);

    ~EventLoaderDirect();

    top::Event loadEvent(xAOD::TEvent& xaodEvent);

    void print(std::ostream& os) const;

    ///Get hold of the container names (in derived classes too).
    const ContainerNames containerNames() const { return m_containerNames; }

private:
    ///Store the container names
    ContainerNames m_containerNames;
};

}

#endif
