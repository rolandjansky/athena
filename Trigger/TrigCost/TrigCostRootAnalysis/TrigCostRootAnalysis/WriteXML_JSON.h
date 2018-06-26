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
    XMLExport(std::ostream& fout) : m_level(0), m_spaces(2), m_minimal(kFALSE) {
      writeHeader(fout);
    }

    XMLExport() : m_level(0), m_spaces(2), m_minimal(kFALSE) {
    }

    void setSpaces(UInt_t n) {
      m_spaces = n;
    }

    void writeHeader(std::ostream& fout) {
      fout << "<?xml version=\"1.0\" encoding=\"us-ascii\"?>" << std::endl;
    }

    void addNode(std::ostream& fout, std::string name, std::string nodeData = "_NONE") { // With or without node data
      if (!m_minimal) fout << std::string(m_level * m_spaces, ' ');

      fout << "<" << name << ">";

      if (nodeData != "_NONE") { // Have data, note that "" may be the 'data' we want to save
        fout << nodeData;
        fout << "</" << name << ">" << std::endl; // Don't have data
      } else {
        fout << std::endl;
        m_nodes.push_back(name);
        ++m_level;
      }
    }

    void addNodeWProperty(std::ostream& fout, std::string name,
                          std::string p1name, std::string p1data,
                          std::string p2name = "", std::string p2data = "",
                          std::string p3name = "", std::string p3data = "",
                          std::string p4name = "", std::string p4data = "") { // node with up to 3 properties
      if (!m_minimal) fout << std::string(m_level * m_spaces, ' ');

      fout << "<" << name << " ";

      if (p1name != Config::config().getStr(kBlankString)) fout << p1name << "=\"" << p1data << "\" ";
      if (p2name != Config::config().getStr(kBlankString)) fout << p2name << "=\"" << p2data << "\" ";
      if (p3name != Config::config().getStr(kBlankString)) fout << p3name << "=\"" << p3data << "\" ";
      if (p4name != Config::config().getStr(kBlankString)) fout << p4name << "=\"" << p4data << "\"";

      fout << "/>" << std::endl;
    }

    void endNode(std::ostream& fout) {
      if (m_nodes.size() == 0) {
        Error("XMLExporter::endNode", "Node list empty. All nodes already closed.");
        return;
      }
      --m_level;
      if (!m_minimal) fout << std::string(m_level * m_spaces, ' ');
      fout << "</" << m_nodes.back() << ">" << std::endl;
      m_nodes.pop_back();
    }

    UInt_t m_level;
    UInt_t m_spaces;
    Bool_t m_minimal;
    std::vector< std::string > m_nodes;
  };
}

#endif //TrigCostRootAnalysis_WriteXML_JSON_H
