/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCABLINGTOOLS_LVL1OBJECT_H
#define MUONCABLINGTOOLS_LVL1OBJECT_H

#define DISP            \
    {__osstream display; \
     display

#define DISP_DEBUG \
    display_debug(display);}

#define DISP_ERROR \
     display_error(display);}

#define DISP_WARNING \
     display_warning(display);}

#define DISP_INFO \
    display_info(display);}

#include <iostream>
#include <string>

#include "MuonCablingTools/OutputDef.h"

#ifndef LVL1_STANDALONE
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#endif

enum ObjectType { Logic, Data, Hardware, Monitoring };

class BaseObject {
private:
    ObjectType m_tag;
    std::string m_name;

protected:
    std::shared_ptr<MsgStream> m_message;

public:
    BaseObject(ObjectType, const std::string&);
    BaseObject(const BaseObject&);
    ~BaseObject();

    BaseObject& operator=(const BaseObject&);

    ObjectType tag(void) const { return m_tag; }
    std::string name(void) const { return m_name; }

    void Print(std::ostream&, bool) const {}
    void display_warning(__osstream&) const;
    void display_info(__osstream&) const;
    void display_error(__osstream&) const;
    void display_debug(__osstream&) const;
};

#endif
