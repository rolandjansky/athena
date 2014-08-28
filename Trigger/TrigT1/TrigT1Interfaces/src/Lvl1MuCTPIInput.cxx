
#include <iostream>
#include <iomanip>

#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"

namespace LVL1MUONIF {

  Lvl1MuCTPIInput::Lvl1MuCTPIInput() {}

  Lvl1MuCTPIInput::Lvl1MuCTPIInput( const Lvl1MuCTPIInput& right ) {
    *this = right;
  }

  Lvl1MuCTPIInput::~Lvl1MuCTPIInput() {
    clearAll();
  }

  Lvl1MuCTPIInput& Lvl1MuCTPIInput::operator=( const Lvl1MuCTPIInput& right ) {
    if ( this != &right ) {
      size_t id, id_sector;
      clearAll();
      // Barrel
      if ( !right.isEmpty( Barrel ) ) {
        for ( id = 0; id < NumberOfMuonSubSystem; id++ ) {
          for ( id_sector = 0; id_sector < NumberOfBarrelSector ;++id_sector ) {
            m_barrel[ id ].push_back( ( right.m_barrel[ id ] ) [ id_sector ] );
          }
        }
      }
      // Endcap
      if ( !right.isEmpty( Endcap ) ) {
        for ( id = 0; id < NumberOfMuonSubSystem; id++ ) {
          for ( id_sector = 0; id_sector < NumberOfEndcapSector ;++id_sector ) {
            m_endcap[ id ].push_back( ( right.m_endcap[ id ] ) [ id_sector ] );
          }
        }
      }
      // Forward
      if ( !right.isEmpty( Forward ) ) {
        for ( id = 0; id < NumberOfMuonSubSystem; id++ ) {
          for ( id_sector = 0; id_sector < NumberOfForwardSector ;++id_sector ) {
            m_forward[ id ].push_back( ( right.m_forward[ id ] ) [ id_sector ] );
          }
        }
      }

    }
    return *this;
  }



  const Lvl1MuSectorLogicData&
  Lvl1MuCTPIInput::getSectorLogicData( size_t systemAddress,
                                       size_t subSystemAddress,
                                       size_t sectorAddress ) const {
    static Lvl1MuBarrelSectorLogicData dummy;
    if ( isEmpty( systemAddress ) ) return dummy;

    if ( systemAddress == Barrel ) {
      return ( m_barrel[ subSystemAddress ] ) [ sectorAddress ];
    } else if ( systemAddress == Endcap ) {
      return ( m_endcap[ subSystemAddress ] ) [ sectorAddress ];
    } else if ( systemAddress == Forward ) {
      return ( m_forward[ subSystemAddress ] ) [ sectorAddress ];
    }
    return dummy;
  }

  void Lvl1MuCTPIInput::setSectorLogicData( const Lvl1MuSectorLogicData& data,
      size_t systemAddress,
      size_t subSystemAddress,
      size_t sectorAddress ) {

    if ( isEmpty( systemAddress ) ) reserve( systemAddress );

    if ( systemAddress == Barrel ) {
      ( m_barrel[ subSystemAddress ] ) [ sectorAddress ] = data;
    } else if ( systemAddress == Endcap ) {
      ( m_endcap[ subSystemAddress ] ) [ sectorAddress ] = data;
    } else if ( systemAddress == Forward ) {
      ( m_forward[ subSystemAddress ] ) [ sectorAddress ] = data;
    }

  }

  void Lvl1MuCTPIInput::setSectorLogicData( const unsigned int & sectorWord,
      size_t systemAddress,
      size_t subSystemAddress,
      size_t sectorAddress ) {
    // convert to SectorLogicData
    Lvl1MuSectorLogicData * pData;
    if ( systemAddress == Barrel ) {
      pData = new Lvl1MuBarrelSectorLogicData();
    } else if ( systemAddress == Endcap ) {
      pData = new Lvl1MuEndcapSectorLogicData();
    } else if ( systemAddress == Forward ) {
      pData = new Lvl1MuForwardSectorLogicData();
    } else {
      return ;
    }
    pData->convertFromWordFormat( sectorWord );
    setSectorLogicData( *pData, systemAddress, subSystemAddress, sectorAddress );
    delete pData;
  }

  void Lvl1MuCTPIInput::merge( const Lvl1MuCTPIInput& right ) {
    size_t id, id_sector;

    // Barrel
    if ( isEmpty( Barrel ) ) {
      if ( !right.isEmpty( Barrel ) ) {
        for ( id = 0; id < NumberOfMuonSubSystem; id++ ) {
          for ( id_sector = 0; id_sector < NumberOfBarrelSector ;++id_sector ) {
            m_barrel[ id ].push_back( ( right.m_barrel[ id ] ) [ id_sector ] );
          }
        }
      }
    }

    // Endcap
    if ( isEmpty( Endcap ) ) {
      if ( !right.isEmpty( Endcap ) ) {
        for ( id = 0; id < NumberOfMuonSubSystem; id++ ) {
          for ( id_sector = 0; id_sector < NumberOfEndcapSector ;++id_sector ) {
            m_endcap[ id ].push_back( ( right.m_endcap[ id ] ) [ id_sector ] );
          }
        }
      }
    }

    // Forward
    if ( isEmpty( Forward ) ) {
      if ( !right.isEmpty( Forward ) ) {
        for ( id = 0; id < NumberOfMuonSubSystem; id++ ) {
          for ( id_sector = 0; id_sector < NumberOfForwardSector ;++id_sector ) {
            m_forward[ id ].push_back( ( right.m_forward[ id ] ) [ id_sector ] );
          }
        }
      }
    }

  }

  void Lvl1MuCTPIInput::reserve( size_t systemAddress ) {
    size_t id, id_sector;
    if ( systemAddress == Barrel ) {
      Lvl1MuBarrelSectorLogicData bData;
      for ( id = 0; id < NumberOfMuonSubSystem; id++ ) {
        for ( id_sector = 0; id_sector < NumberOfBarrelSector ;++id_sector ) {
          m_barrel[ id ].push_back( bData );
        }
      }

    } else if ( systemAddress == Endcap ) {
      Lvl1MuEndcapSectorLogicData eData;
      for ( id = 0; id < NumberOfMuonSubSystem; id++ ) {
        for ( id_sector = 0; id_sector < NumberOfEndcapSector ;++id_sector ) {
          m_endcap[ id ].push_back( eData );
        }
      }

    } else if ( systemAddress == Forward ) {
      Lvl1MuForwardSectorLogicData fData;
      for ( id = 0; id < NumberOfMuonSubSystem; id++ ) {
        for ( id_sector = 0; id_sector < NumberOfForwardSector ;++id_sector ) {
          m_forward[ id ].push_back( fData );
        }
      }

    }
  }

  bool Lvl1MuCTPIInput::isEmpty( size_t systemAddress ) const {
    if ( systemAddress == Barrel ) {
      return ( m_barrel[ 0 ].size() == 0 );
    } else if ( systemAddress == Endcap ) {
      return ( m_endcap[ 0 ].size() == 0 );
    } else if ( systemAddress == Forward ) {
      return ( m_forward[ 0 ].size() == 0 );
    }
    return true;
  }

  void Lvl1MuCTPIInput::clear( size_t systemAddress ) {
    size_t id;
    if ( systemAddress == Barrel ) {
      for ( id = 0; id < NumberOfMuonSubSystem; id++ ) {
        m_barrel[ id ].clear();
      }
    } else if ( systemAddress == Endcap ) {
      for ( id = 0; id < NumberOfMuonSubSystem; id++ ) {
        m_endcap[ id ].clear();
      }
    } else if ( systemAddress == Forward ) {
      for ( id = 0; id < NumberOfMuonSubSystem; id++ ) {
        m_forward[ id ].clear();
      }
    }
  }

  void Lvl1MuCTPIInput::clearAll() {
    size_t id;
    for ( id = 0; id < NumberOfMuonSubSystem; id++ ) {
      m_barrel[ id ].clear();
      m_endcap[ id ].clear();
      m_forward[ id ].clear();
    }
  }

  bool Lvl1MuCTPIInput::retrieveInWordFormat( std::ifstream& fIn, bool ascii ) {
    size_t systemAddress;
    size_t subSystemAddress;
    size_t sectorAddress;
    Lvl1MuSectorLogicData::SectorLogicWord wordData;

    size_t totalWordCount =
      2 * ( Lvl1MuCTPIInput::numberOfBarrelSector()
            + Lvl1MuCTPIInput::numberOfEndcapSector()
            + Lvl1MuCTPIInput::numberOfForwardSector() );
    // read fixed size length
    for ( size_t count = 0; count < totalWordCount; ++count ) {
      // read in one word
      if ( !ascii ) {
        fIn.read( ( char* ) ( &systemAddress ), sizeof systemAddress );
        fIn.read( ( char* ) ( &subSystemAddress ), sizeof subSystemAddress );
        fIn.read( ( char* ) ( &sectorAddress ), sizeof sectorAddress );
        fIn.read( ( char* ) ( &wordData ), sizeof wordData );
      } else {
        fIn >> systemAddress >> subSystemAddress >> sectorAddress >> wordData;
      }
      if ( fIn.eof() ) break;
      if ( fIn.fail() ) {
        std::cerr << "Lvl1MuCTPIInput::retrieveInWordFormat ";
        std::cerr << " Error in reading file ";
        std::cout << "  at " << count << "-th data" << std::endl;
        return false;
      }

      // convert to SectorLogicData
      Lvl1MuSectorLogicData* pData = 0;
      if ( systemAddress == Barrel ) {
        pData = new Lvl1MuBarrelSectorLogicData();
      } else if ( systemAddress == Endcap ) {
        pData = new Lvl1MuEndcapSectorLogicData();
      } else if ( systemAddress == Forward ) {
        pData = new Lvl1MuForwardSectorLogicData();
      }
      if (pData) {
        pData->convertFromWordFormat( wordData );

        setSectorLogicData( *pData, systemAddress, subSystemAddress, sectorAddress );
        delete pData;
      }
    }

    return true;
  }

  bool Lvl1MuCTPIInput::storeInWordFormat( std::ofstream& fOut,
      bool ascii,
      size_t systemAddress,
      size_t subSystemAddress,
      size_t sectorAddress ) {
    // Get Word Format Data
    const Lvl1MuSectorLogicData & data = getSectorLogicData( systemAddress, subSystemAddress, sectorAddress );
    Lvl1MuSectorLogicData::SectorLogicWord wordData;
    wordData = data.getWordFormat();

    // write out
    if ( !ascii ) {
      fOut.write( ( char* ) ( &systemAddress ), sizeof systemAddress );
      fOut.write( ( char* ) ( &subSystemAddress ), sizeof subSystemAddress );
      fOut.write( ( char* ) ( &sectorAddress ), sizeof sectorAddress );
      fOut.write( ( char* ) ( &wordData ), sizeof wordData );
    } else {
      fOut << systemAddress << " ";
      fOut << subSystemAddress << " ";
      fOut << sectorAddress << " ";
      fOut << wordData << std::endl;
    }
    return !fOut.fail();
  }

  bool Lvl1MuCTPIInput::storeInWordFormat( std::ofstream& fOut, bool ascii ) {
    size_t systemAddress;
    size_t subAddress;
    size_t side, id;

    // --------- Barrel Sector ---------------------------
    systemAddress = idBarrelSystem();
    for ( side = 0; side < 2; ++side ) {
      if ( side == 0 ) {
        subAddress = idSideC();
      } else {
        subAddress = idSideA();
      }
      for ( id = 0; id < numberOfBarrelSector() ;++id ) {
        if ( !storeInWordFormat( fOut, ascii, systemAddress, subAddress, id ) ) {
          std::cerr << "Lvl1MuCTPIInput::storeInWordFormat ";
          std::cerr << " Error in writing file " << std::endl;
          return false;
        }
      }
    }

    // --------- Endcap Sector ---------------------------
    systemAddress = idEndcapSystem();
    for ( side = 0; side < 2; ++side ) {
      if ( side == 0 ) {
        subAddress = idSideC();
      } else {
        subAddress = idSideA();
      }
      for ( id = 0; id < numberOfEndcapSector() ;++id ) {
        if ( !storeInWordFormat( fOut, ascii, systemAddress, subAddress, id ) ) {
          std::cerr << "Lvl1MuCTPIInput::storeInWordFormat ";
          std::cerr << " Error in writing file " << std::endl;
          return false;
        }
      }
    }

    // --------- Forward Sector ---------------------------
    systemAddress = idForwardSystem();
    for ( side = 0; side < 2; ++side ) {
      if ( side == 0 ) {
        subAddress = idSideC();
      } else {
        subAddress = idSideA();
      }
      for ( id = 0; id < numberOfForwardSector() ;++id ) {
        if ( !storeInWordFormat( fOut, ascii, systemAddress, subAddress, id ) ) {
          std::cerr << "Lvl1MuCTPIInput::storeInWordFormat ";
          std::cerr << " Error in writing file " << std::endl;
          return false;
        }
      }
    }
    return true;
  }

  std::ostream& operator<<( std::ostream& out, const Lvl1MuCTPIInput& right ) {

    size_t systemAddress;
    size_t subSystemAddress;
    size_t side, id;

    out << "--------- Barrel Sector ---------------------------" << std::endl;
    systemAddress = right.idBarrelSystem();
    for ( side = 0; side < 2; ++side ) {
      if ( side == 0 ) {
        out << "   ------ A Side (Z>0)  ---------------------------" << std::endl;
        subSystemAddress = right.idSideA();
      } else {
        out << "   ------ C Side (C>0)  ---------------------------" << std::endl;
        subSystemAddress = right.idSideC();
      }
      for ( id = 0; id < right.numberOfBarrelSector() ;++id ) {
        out << "      --- Sector ID  :  " << id << "   ----------" << std::endl;
        out << right.getSectorLogicData( systemAddress, subSystemAddress, id );
        out << std::endl;
      }
    }

    out << "--------- Endcap Sector ---------------------------" << std::endl;
    systemAddress = right.idEndcapSystem();
    for ( side = 0; side < 2; ++side ) {
      if ( side == 0 ) {
        out << "   ------ A Side (Z>0)  ---------------------------" << std::endl;
        subSystemAddress = right.idSideA();
      } else {
        out << "   ------ C Side (C>0)  ---------------------------" << std::endl;
        subSystemAddress = right.idSideC();
      }
      for ( id = 0; id < right.numberOfEndcapSector() ;++id ) {
        out << "      --- Sector ID  :  " << id << "   ----------" << std::endl;
        out << right.getSectorLogicData( systemAddress, subSystemAddress, id );
        out << std::endl;
      }
    }

    out << "--------- Forward Sector ---------------------------" << std::endl;
    systemAddress = right.idForwardSystem();
    for ( side = 0; side < 2; ++side ) {
      if ( side == 0 ) {
        out << "   ------ A Side (Z>0)  ---------------------------" << std::endl;
        subSystemAddress = right.idSideA();
      } else {
        out << "   ------ C Side (C>0)  ---------------------------" << std::endl;
        subSystemAddress = right.idSideC();
      }
      for ( id = 0; id < right.numberOfForwardSector() ;++id ) {
        out << "      --- Sector ID  :  " << id << "   ----------" << std::endl;
        out << right.getSectorLogicData( systemAddress, subSystemAddress, id );
        out << std::endl;
      }
    }

    return out;

  }

}
