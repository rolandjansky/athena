/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALORXCOOLCHANNELID_H
#define L1CALORXCOOLCHANNELID_H

#include <ostream>

class L1CaloRxCoolChannelId final {
	friend std::ostream& operator<<(std::ostream& output, const L1CaloRxCoolChannelId& r);

public:

  L1CaloRxCoolChannelId();
  L1CaloRxCoolChannelId(unsigned int crate, unsigned int module, unsigned int inputConn, unsigned int inputPair);
  L1CaloRxCoolChannelId(unsigned int crate, unsigned int module, unsigned int inputConn, unsigned int inputPair, unsigned int outputConn, unsigned int outputPair);
  L1CaloRxCoolChannelId(unsigned int id);
  L1CaloRxCoolChannelId(const L1CaloRxCoolChannelId& id);
  ~L1CaloRxCoolChannelId() = default;

  unsigned int crate()     const { return (unsigned int) m_id/1024; };
  unsigned int module()    const { return (unsigned int) m_id%1024/64; };
  unsigned int inputConn() const { return (unsigned int) m_id%64/16; };
  unsigned int inputPair() const { return (unsigned int) m_id%16; };

  unsigned int id()        const { return m_id; };

  unsigned int outputConn() const;
  unsigned int outputPair() const;

  void outputAttributes(unsigned int conn, unsigned int pair);

  L1CaloRxCoolChannelId& operator=(const L1CaloRxCoolChannelId& id) = default;
  bool operator==(const L1CaloRxCoolChannelId& id) const;
  bool operator<(const L1CaloRxCoolChannelId& id) const;

private:

  void setId(unsigned int crate,
             unsigned int module,
             unsigned int inputConn,
             unsigned int inputPair);

  unsigned int m_id;

  bool m_bOutputIsSet;
  unsigned int m_outputConn;
  unsigned int m_outputPair;

};

#endif
