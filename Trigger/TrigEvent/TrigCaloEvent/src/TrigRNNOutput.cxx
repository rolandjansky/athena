/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigRNNOutput.h"
#include "TrigCaloEvent/TrigEMCluster.h"
#include "AthLinks/ElementLink.h"

TrigRNNOutput::TrigRNNOutput()
  : m_output(), m_cluster() {
}

TrigRNNOutput::TrigRNNOutput(const std::vector<float> &output)
  : m_output(output), m_cluster() {
}

std::vector<float> &TrigRNNOutput::output() {
  return m_output;
}

const std::vector<float> &TrigRNNOutput::output() const {
  return m_output;
}

void TrigRNNOutput::output(const std::vector<float> &o) {
  m_output = o;
}

float &TrigRNNOutput::at(const unsigned int i) {
  return m_output.at(i);
}

const float &TrigRNNOutput::at(const unsigned int i) const {
  return m_output.at(i);
}

unsigned int TrigRNNOutput::size() const {
  return m_output.size();
}

const float &TrigRNNOutput::operator [](const unsigned int i) const {
  return m_output[i];
}

float &TrigRNNOutput::operator [](const unsigned int i) {
  return m_output[i];
}

bool TrigRNNOutput::isValid() const {
  return m_cluster.isValid();
}

void TrigRNNOutput::setCluster(const TrigEMClusterContainer &cluster_con, unsigned int cluster_idx) {
  if (cluster_idx < cluster_con.size()) {
    m_cluster.toIndexedElement(cluster_con, cluster_idx);
    if (!m_cluster.isValid()) {
      m_cluster.reset();
    }
  } else {
    m_cluster.reset();
  }
}

const TrigEMCluster * TrigRNNOutput::cluster() const {
  if (m_cluster.isValid()) {
    return *m_cluster;
  }
  return 0;
}


void TrigRNNOutput::setCluster(const ElementLink<TrigEMClusterContainer>& cluster)
{
  m_cluster = cluster;
}


const ElementLink<TrigEMClusterContainer>& TrigRNNOutput::clusterLink() const
{
  return m_cluster;
}
