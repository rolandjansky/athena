#include <iostream>
#include <map>

#define arraySize 10000
typedef std::map<unsigned long, unsigned int>::const_iterator cRPCpanelListIterator;

class RPCpanelList {
public:
    RPCpanelList();
    ~RPCpanelList();
    void init(int inputLayer);
    void init();
    cRPCpanelListIterator end();
    cRPCpanelListIterator begin();
    unsigned int size();
    unsigned int getIndexInList(unsigned long panelID);
    std::string getStringId(unsigned long panelID);
    std::string getStName(unsigned long panelID);
    unsigned short getLayer(unsigned long panelID);
    unsigned short getView(unsigned long panelID);
    double getEtaMin(unsigned long panelID);
    double getEtaMax(unsigned long panelID);
    double getPhiMin(unsigned long panelID);
    double getPhiMax(unsigned long panelID);
    double getZMin(unsigned long panelID);
    double getZMax(unsigned long panelID);
    void setAcceptProximityID(bool flag) {
        if (flag) std::cout << "will accept proximity IDs " << std::endl;
        m_acceptProximityID = flag;
    }
    bool getAcceptProximityID() { return m_acceptProximityID; }

private:
    bool m_acceptProximityID;
    std::map<unsigned long, unsigned int>* m_entryInListMap;
    std::string m_stringIdArray[arraySize];
    std::string m_stNameArray[arraySize];
    double m_boundArrayEtaMin[arraySize];
    double m_boundArrayEtaMax[arraySize];
    double m_boundArrayPhiMin[arraySize];
    double m_boundArrayPhiMax[arraySize];
    double m_boundArrayZMin[arraySize];
    double m_boundArrayZMax[arraySize];
    short int m_viewArray[arraySize];
    short int m_layerArray[arraySize];

    unsigned int indexInCache;
    unsigned int panelIDinCache;
};
