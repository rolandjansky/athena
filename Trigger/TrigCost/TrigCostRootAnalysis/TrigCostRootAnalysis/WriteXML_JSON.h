// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_WriteXML_JSON_H
#define TrigCostRootAnalysis_WriteXML_JSON_H

// STL include(s):
#include <string>
#include <sstream>
#include <utility>
#include <map>
#include <set>
#include <fstream>
#include <limits.h>

// ROOT include(s):
#include <Rtypes.h>
#include <TError.h>

// Local include(s):
#include "Config.h"

/**
 * @file WriteXML_JSON.h
 *
 * Basic structured export code. Can write simple JSON and XML files. Contains two classes.
 */

namespace TrigCostRootAnalysis {
  class XMLExport {
  public:
    XMLExport(std::ostream& _fout) : m_level(0), m_spaces(2), m_minimal(kFALSE) {
      writeHeader(_fout);
    }

    XMLExport() : m_level(0), m_spaces(2), m_minimal(kFALSE) {
    }

    void setSpaces(UInt_t _n) {
      m_spaces = _n;
    }

    void writeHeader(std::ostream& _fout) {
      _fout << "<?xml version=\"1.0\" encoding=\"us-ascii\"?>" << std::endl;
    }

    void addNode(std::ostream& _fout, std::string _name, std::string _nodeData = "_NONE") { // With or without node data
      if (!m_minimal) _fout << std::string(m_level * m_spaces, ' ');

      _fout << "<" << _name << ">";

      if (_nodeData != "_NONE") { // Have data, note that "" may be the 'data' we want to save
        _fout << _nodeData;
        _fout << "</" << _name << ">" << std::endl; // Don't have data
      } else {
        _fout << std::endl;
        m_nodes.push_back(_name);
        ++m_level;
      }
    }

    void addNodeWProperty(std::ostream& _fout, std::string _name,
                          std::string _p1name, std::string _p1data,
                          std::string _p2name = "", std::string _p2data = "",
                          std::string _p3name = "", std::string _p3data = "",
                          std::string _p4name = "", std::string _p4data = "") { // node with up to 3 properties
      if (!m_minimal) _fout << std::string(m_level * m_spaces, ' ');

      _fout << "<" << _name << " ";

      if (_p1name != Config::config().getStr(kBlankString)) _fout << _p1name << "=\"" << _p1data << "\" ";
      if (_p2name != Config::config().getStr(kBlankString)) _fout << _p2name << "=\"" << _p2data << "\" ";
      if (_p3name != Config::config().getStr(kBlankString)) _fout << _p3name << "=\"" << _p3data << "\" ";
      if (_p4name != Config::config().getStr(kBlankString)) _fout << _p4name << "=\"" << _p4data << "\"";

      _fout << "/>" << std::endl;
    }

    void endNode(std::ostream& _fout) {
      if (m_nodes.size() == 0) {
        Error("XMLExporter::endNode", "Node list empty. All nodes already closed.");
        return;
      }
      --m_level;
      if (!m_minimal) _fout << std::string(m_level * m_spaces, ' ');
      _fout << "</" << m_nodes.back() << ">" << std::endl;
      m_nodes.pop_back();
    }

    UInt_t m_level;
    UInt_t m_spaces;
    Bool_t m_minimal;
    std::vector< std::string > m_nodes;
  };
}

#endif //TrigCostRootAnalysis_WriteXML_JSON_H
