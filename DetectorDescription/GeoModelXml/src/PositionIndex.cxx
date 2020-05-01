/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>

#ifndef STANDALONE_GMX
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#else
#include <iostream>
#endif

#include <vector>
#include <string>
#include "ExpressionEvaluator/Evaluator.h"
#include "GeoModelXml/PositionIndex.h"

using namespace std;

PositionIndex::PositionIndex() {
    m_level = -1; // Level -1 represents the "world" with no Copy Number
}

PositionIndex::~PositionIndex() {}

int PositionIndex::level() {
    return m_level;
}

int PositionIndex::incrementLevel()  {
    ++m_level;
    if ((int) m_copyNo.size() < m_level + 1)
        m_copyNo.push_back(0);
    return m_level;
}

int PositionIndex::decrementLevel() {
    return --m_level;
}

int PositionIndex::copyNo(int level) {
    if (level >= 0 && level <= m_level) {
        return m_copyNo[level];
    }
    else {
        return 0;
    }
}

int PositionIndex::setCopyNo(int copyNo) {
    m_copyNo[m_level] = copyNo;
    return m_copyNo[m_level];
}

void PositionIndex::addIndex(std::string name) {
    m_name.push_back(name);
    m_formula[name] = "0";
}

string  PositionIndex::name(int whichOne) {
    if (whichOne >= 0 && whichOne < (int) m_name.size()) {
        return m_name[whichOne];
    }
    else {
        return string("");
    }
}

std::string PositionIndex::formula(std::string name) {
    return m_formula[name];
}

void PositionIndex::setFormula(std::string name, std::string formula) {
    m_formula[name] = formula;
}

void PositionIndex::indices(map<string, int> &index, Evaluator &eval) {
    index.clear(); // In case caller uses it many times
//
//    Set Copy Number of each Level (CNL_i) up to current level
//
    string name("CNL_");
    for (int i = 0; i <= m_level; ++i) {
        eval.setVariable((name + to_string(i)).c_str(), m_copyNo[i]);
    }
//
//    Set rest to zero
//
    for (int i = m_level + 1; true; ++i) {
        string varName = name + to_string(i);
        if (!eval.findVariable(varName.c_str())) break;
        eval.setVariable(varName.c_str(), "0");
    }
//
//    Evaluate each index in turn
//
//
//   We loop over the vector of names to make sure we evaluate the CNL_n values in the order they
//   are given in the <positionindex> element. Thus the user can select the order of evaluation, which is 
//   important when CNL_n are used in the formulae. Looping directly over a map<>::iterator would process them
//   in alphabetical order.
//
    for (vector <string>::iterator n = m_name.begin(); n < m_name.end(); ++n) {
        string name = *n;
        index[name] = (int) eval.evaluate(m_formula[name].c_str());
        if (eval.status() != Evaluator::OK) {
#ifndef STANDALONE_GMX
            ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
            MsgStream log(&(*msgh), "GeoModelXml");
            log << MSG::FATAL <<
                   "GeoModelXml Error processing Evaluator expression for PositionIndex. Error name " <<
                    eval.error_name() << endl << "Message: ";
            eval.print_error();
            log << m_formula[name] << endl << string(eval.error_position(), '-') << '^' << '\n' << "Exiting program." << 
                   endmsg;
#else
	    std::cout<<"GeoModelXml Error processing Evaluator expression for PositionIndex. Error name " <<
                    eval.error_name() << endl << "Message: ";
            eval.print_error();
            std::cout << m_formula[name] << endl << string(eval.error_position(), '-') << '^' << '\n' << "Exiting program." << 
		  std::endl;
#endif
            exit(999); // Should do better...
        }
//
//    And create a CLHEP variable with the same index-name in case the user needs it 
//
        eval.setVariable(name.c_str(), (double) index[name]);
    }
}
