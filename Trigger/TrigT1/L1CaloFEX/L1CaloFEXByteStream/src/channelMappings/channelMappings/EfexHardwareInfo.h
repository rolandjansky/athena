#ifndef EFEX_HARDWARE_INFO_H
#define EFEX_HARDWARE_INFO_H

class EfexHardwareInfo {
    public:
        //Blank Invalid Constructor
        EfexHardwareInfo();
        //Constructor
        EfexHardwareInfo(std::string efexlabel,
                        int fibre,
                        int inputconnector,
                        std::string mpod
                        );
        // Get methods
        std::string     getEFEXLabel() const;
        int             getFibreNumber() const;
        int             getRibbonFibreNumber() const;
        int             getInputConnector() const;
        int             getMpodNumber() const;
        std::string     getMpodLabel() const;
        bool            getValidity() const;
        void            setOverlap(int overlap);
        int             getOverlap() const;
        //Prints
        void            printInfo() const;
    private:
        bool m_valid;
        std::string m_efexlabel; 
        int m_fibre;
        int m_inputconnector;
        std::string m_mpodlabel;
        int m_overlap;

};
#endif
