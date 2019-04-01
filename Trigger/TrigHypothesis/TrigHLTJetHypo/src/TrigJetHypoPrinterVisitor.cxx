/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./TrigJetHypoPrinterVisitor.h"
#include <sstream>
#include <fstream>
#include <iostream>
void TrigJetHypoPrinterVisitor::visit(TrigJetHypoToolHelperMT* node){
  std::cout << "visitor received node " << node->name() << '\n';
  auto p = std::pair<std::string,
                     std::string>(node->name(), node->toStringAndResetHistory());
  m_nodeStrings.push_back(p);
}

void TrigJetHypoPrinterVisitor::visit(AndHelperTool* node){
  std::cout << "visitor received node " << node->name() << '\n';
  m_nodeStrings.push_back(std::pair<std::string,
                          std::string>(node->name(), node->toStringAndResetHistory()));
}


void TrigJetHypoPrinterVisitor::visit(OrHelperTool* node){
  std::cout << "visitor received node " << node->name() << '\n';
  m_nodeStrings.push_back(std::pair<std::string,
                          std::string>(node->name(), node->toStringAndResetHistory()));
}


void TrigJetHypoPrinterVisitor::visit(NotHelperTool* node){
  std::cout << "visitor received node " << node->name() << '\n';
  m_nodeStrings.push_back(std::pair<std::string,
                          std::string>(node->name(), node->toStringAndResetHistory()));
}


void TrigJetHypoPrinterVisitor::visit(CombinationsHelperTool* node){
  std::cout << "visitor received node " << node->name() << '\n';
  m_nodeStrings.push_back(std::pair<std::string,
                          std::string>(node->name(), node->toStringAndResetHistory()));
}


std::string TrigJetHypoPrinterVisitor::toString() const noexcept{
  std::stringstream ss;
  ss << "TrigJetHypoPrinterVisitor:\n";
  for(const auto& i: m_nodeStrings){
    ss << i.second << '\n';
  }
  return ss.str();
}


void TrigJetHypoPrinterVisitor::write(std::string name,
                                      uint32_t run,
                                      uint32_t event) {
  auto report = this->toString();
  std::stringstream ss;
  ss << "jetHypo_" << name << "_" << run << "_" << event;
  std::ofstream out(ss.str());
  out << report;   
}


void TrigJetHypoPrinterVisitor::write(std::string name, std::size_t id) {
  auto report = this->toString();
  std::stringstream ss;
  ss << "jetHypo_" << name << "_" << id;
  ++m_eventSN;
  std::ofstream out(ss.str());
  out << report;
  out.close();
}
