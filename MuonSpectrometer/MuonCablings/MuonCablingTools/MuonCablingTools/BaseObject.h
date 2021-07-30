/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCABLINGTOOLS_LVL1OBJECT_H
#define MUONCABLINGTOOLS_LVL1OBJECT_H

#define DISP                \
    {                       \
        __osstream display; \
        display

#define DISP_DEBUG          \
    display_debug(display); \
    }

#define DISP_VERBOSE          \
    display_verbose(display); \
    }

#define DISP_ERROR          \
    display_error(display); \
    }

#define DISP_WARNING          \
    display_warning(display); \
    }

#define DISP_INFO          \
    display_info(display); \
    }

#include <functional>
#include <iostream>
#include <string>

#include "MuonCablingTools/OutputDef.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

enum ObjectType { Logic, Data, Hardware, Monitoring };

class BaseObject {
private:
    ObjectType m_tag;
    std::string m_name{};
    IMessageSvc* m_msgSvc{nullptr};
    std::unique_ptr<MsgStream> m_message{};
    void dump_message(__osstream& sstr, MSG::Level) const;

public:
    BaseObject(ObjectType, const std::string&, IMessageSvc* msgSvc = nullptr);
    BaseObject& operator=(const BaseObject&);
    BaseObject(const BaseObject&);

    virtual ~BaseObject() = default;

    ObjectType tag() const;
    std::string name() const;

    bool msgLevel(MSG::Level lvl) const;

    virtual void Print(std::ostream&, bool) const {}
    void display_warning(__osstream&) const;
    void display_info(__osstream&) const;
    void display_error(__osstream&) const;
    void display_debug(__osstream&) const;
    void display_verbose(__osstream&) const;
};

#endif
