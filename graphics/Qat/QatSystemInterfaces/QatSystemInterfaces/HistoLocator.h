#ifndef QATSYSTEMINTERFACES_HISTOLOCATOR_H
#define QATSYSTEMINTERFACES_HISTOLOCATOR_H

class HistoLocator1D {
  
 public:

  // Constructor:
  HistoLocator1D(){};

  // Destructor:
  ~HistoLocator1D(){};

  // Insert
  void insert(const Hist1D *histo, std::string & name) {
    m_histoMap[histo]=name;
    m_nameMap [name ]=histo;
  }

  // Find forward
  const std::string *find  (const Hist1D *histo) const {
    std::map<const Hist1D *, std::string>::const_iterator h=m_histoMap.find(histo);
    return h==m_histoMap.end() ? NULL :  & (*h).second;
  }

  // Find reverse
  const Hist1D *find  (const std::string & name) const {
    std::map<std::string, const Hist1D *>::const_iterator h=m_nameMap.find(name);
    return h==m_nameMap.end() ? NULL :  (*h).second;
  }

 private:

  std::map<const Hist1D *, std::string>       m_histoMap;
  std::map<std::string,    const Hist1D *>    m_nameMap;

};


class HistoLocator2D {
  
 public:

  // Constructor:
  HistoLocator2D(){};

  // Destructor:
  ~HistoLocator2D(){};

  // Insert
  void insert(const Hist2D *histo, std::string & name) {
    m_histoMap[histo]=name;
    m_nameMap [name ]=histo;
  }

  // Find forward
  const std::string *find  (const Hist2D *histo) const {
    std::map<const Hist2D *, std::string>::const_iterator h=m_histoMap.find(histo);
    return h==m_histoMap.end() ? NULL :  & (*h).second;
  }

  // Find reverse
  const Hist2D *find  (const std::string & name) const {
    std::map<std::string, const Hist2D *>::const_iterator h=m_nameMap.find(name);
    return h==m_nameMap.end() ? NULL :  (*h).second;
  }

 private:

  std::map<const Hist2D *, std::string>       m_histoMap;
  std::map<std::string,    const Hist2D *>    m_nameMap;

};



class VisFunctionLocator {
  
 public:

  // Constructor:
  VisFunctionLocator(){};

  // Destructor:
  ~VisFunctionLocator(){};

  // Insert
  void insert(const VisFunction *vis, std::string & name) {
    m_visMap[vis]=name;
    m_nameMap [name ]=vis;
  }

  // Find forward
  const std::string *find  (const VisFunction *vis) const {
    std::map<const VisFunction *, std::string>::const_iterator h=m_visMap.find(vis);
    return h==m_visMap.end() ? NULL :  & (*h).second;
  }

  // Find reverse
  const VisFunction *find  (const std::string & name) const {
    std::map<std::string, const VisFunction *>::const_iterator h=m_nameMap.find(name);
    return h==m_nameMap.end() ? NULL :  (*h).second;
  }

 private:

  std::map<const VisFunction *, std::string>       m_visMap;
  std::map<std::string,    const VisFunction *>    m_nameMap;

};

#endif
